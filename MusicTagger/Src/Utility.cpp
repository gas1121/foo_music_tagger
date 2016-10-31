#undef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1

#include "Utility.h"

//#TODO remove windows header use std::codecvt std::c32rtomb u8 string std::locale
//see http://bbs.csdn.net/topics/391040755
#include <windows.h>
#include <sstream>
#include <boost/filesystem.hpp>
#include <rapidjson/pointer.h>
#include <rapidjson/writer.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/schema.h>
#include "MusicTagger/MusicTagger.h"

using std::string;

namespace {
	const int kBufferSize = 65536;
}

namespace MusicTagger {

    string ComputeCddbID(const int trackCount, std::vector<int> trackSeconds)
    {
        int n = 0;
        int totalSeconds = 0;
        for (int i = 0; i < (int)trackSeconds.size(); i++)
        {
            totalSeconds += trackSeconds[i];
            int sum = 0;
            int j = trackSeconds[i];
            while (j > 0)
            {
                sum += (j % 10);
                j /= 10;
            }
            n += sum;
        }

        //Compute DISC ID
        unsigned long dwDiscID = ((n % 0xFF) << 24 | totalSeconds << 8 | trackCount);
        std::stringstream stream;
        stream << std::hex << dwDiscID;
        return stream.str();
    }

    string ComputeTrackOffsetString(std::vector<int> trackSeconds)
    {
		return ComputeTrackOffsetString(trackSeconds, " ");
    }

	std::string ComputeTrackOffsetString(std::vector<int> trackSeconds, const std::string delimiter)
	{
		string result;
		int currOffset = kPreSeconds*kFramePerSecond;
		result += std::to_string(currOffset);
		//compute total offset before current track
		for (int i = 0; i < (int)trackSeconds.size() - 1; i++)
		{
			currOffset += trackSeconds[i] * kFramePerSecond;
			result = result + delimiter + std::to_string(currOffset);
		}
		return result;
	}

	bool ReadJsonFromFile(const string& filePath, rapidjson::Document* document)
	{
		using namespace boost::filesystem;
		path file = system_complete(filePath);
		boost::system::error_code ec;

		if (!exists(file, ec))
		{
			return false;
		}

		std::FILE* fp = std::fopen(file.string().c_str(), "rb");
		if (fp == nullptr)
		{
			return false;
		}
		char readBuffer[kBufferSize] = "";
		rapidjson::FileReadStream fs(fp, readBuffer, sizeof(readBuffer));
		document->ParseStream<rapidjson::kParseCommentsFlag>(fs);
		if (!document->IsObject())
		{
			return false;
		}
		return true;
	}

	bool ValidateJsonBySchemaFile(const rapidjson::Document& document, const string& validateFileName)
	{
		using namespace rapidjson;
		//validate source json
		Document schemaDocument;

        bool isFileExist = false;
        for (std::string currPath : kJsonFilePath)
        {
            string filePath = currPath + "/" + kValidatorSuffix + "/" + validateFileName + ".json";
            if (ReadJsonFromFile(filePath, &schemaDocument))
            {
                isFileExist = true;
                break;
            }
        }
        if ((!isFileExist)||(!schemaDocument.IsObject()))
		{
			return false;
		}
		SchemaDocument schema(schemaDocument);
		SchemaValidator validator(schema);
		if (!document.Accept(validator))
		{
			return false;
		}
		return true;
	}

	boost::optional<int> GetOptionalIntValue(const rapidjson::Document& document, const string& jsonPointerString)
	{
		using namespace rapidjson;

		const Value* value = GetValueByPointer(document, Pointer(jsonPointerString));
		if (value != nullptr && value->IsInt())
		{
			return value->GetInt();
		}
		return boost::none;
	}

	boost::optional<string> GetOptionalStringValue(const rapidjson::Document& document, const string& jsonPointerString)
	{
		using namespace rapidjson;

		const Value* value = GetValueByPointer(document, Pointer(jsonPointerString));
		if (value != nullptr && value->IsString())
		{
			return value->GetString();
		}
		return boost::none;
	}

	boost::optional<bool> GetOptionalBoolValue(const rapidjson::Document& document, const string& jsonPointerString)
	{
		using namespace rapidjson;

		const Value* value = GetValueByPointer(document, Pointer(jsonPointerString));
		if (value != nullptr && value->IsBool())
		{
			return value->GetBool();
		}
		return boost::none;
	}

	string GBKToUTF8(const char* strGBK)
	{
		string strOutUTF8 = "";
		wchar_t * str1;
		int n = MultiByteToWideChar(CP_ACP, 0, strGBK, -1, nullptr, 0);
		str1 = new wchar_t[n];
		MultiByteToWideChar(CP_ACP, 0, strGBK, -1, str1, n);
		n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, nullptr, 0, nullptr, nullptr);
		char * str2 = new char[n];
		WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, nullptr, nullptr);
		strOutUTF8 = str2;
		delete[]str1;
		str1 = nullptr;
		delete[]str2;
		str2 = nullptr;
		return strOutUTF8;
	}

	string UrlEncode(const string& szToEncode)
	{
		string src = szToEncode;
		char hex[] = "0123456789ABCDEF";
		string dst;

		for (size_t i = 0; i < src.size(); ++i)
		{
			unsigned char cc = src[i];
			if (isascii(cc))
			{
				if (cc == ' ')
				{
					dst += "%20";
				}
				else
					dst += cc;
			}
			else
			{
				unsigned char c = static_cast<unsigned char>(src[i]);
				dst += '%';
				dst += hex[c / 16];
				dst += hex[c % 16];
			}
		}
		return dst;
	}


	string UrlDecode(const string& szToDecode)
	{
		string result;
		int hex = 0;
		for (size_t i = 0; i < szToDecode.length(); ++i)
		{
			switch (szToDecode[i])
			{
			case '+':
				result += ' ';
				break;
			case '%':
				if (isxdigit(szToDecode[i + 1]) && isxdigit(szToDecode[i + 2]))
				{
					string hexStr = szToDecode.substr(i + 1, 2);
					hex = strtol(hexStr.c_str(), 0, 16);
					//字母和数字[0-9a-zA-Z]、一些特殊符号[$-_.+!*'(),] 、以及某些保留字[$&+,/:;=?@]
					//可以不经过编码直接用于URL
					if (!((hex >= 48 && hex <= 57) || //0-9
						(hex >= 97 && hex <= 122) ||   //a-z
						(hex >= 65 && hex <= 90) ||    //A-Z
													   //一些特殊符号及保留字[$-_.+!*'(),]  [$&+,/:;=?@]
						hex == 0x21 || hex == 0x24 || hex == 0x26 || hex == 0x27 || hex == 0x28 || hex == 0x29
						|| hex == 0x2a || hex == 0x2b || hex == 0x2c || hex == 0x2d || hex == 0x2e || hex == 0x2f
						|| hex == 0x3A || hex == 0x3B || hex == 0x3D || hex == 0x3f || hex == 0x40 || hex == 0x5f
						))
					{
						result += char(hex);
						i += 2;
					}
					else result += '%';
				}
				else {
					result += '%';
				}
				break;
			default:
				result += szToDecode[i];
				break;
			}
		}
		return result;
	}
}