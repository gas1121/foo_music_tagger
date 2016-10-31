#ifndef RAPIDJSON_HAS_STDSTRING
    #define RAPIDJSON_HAS_STDSTRING 1
#endif // !RAPIDJSON_HAS_STDSTRING

#include "QuerySourceBase.h"

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <rapidjson/pointer.h>
#include <rapidjson/writer.h>
#include "Utility.h"

using std::string;

namespace MusicTagger {

	QuerySourceBase::QuerySourceBase(const string& sourceName, const std::string& queryJsonString)
		:ec_(MusicTaggerErrorCode::kNoError)
	{
        bool isFileExist = false;
        for (std::string currPath:kJsonFilePath)
        {
            string filePath = currPath + "/" + kSiteSuffix + "/" + sourceName + ".json";
            if (ReadJsonFromFile(filePath, &setting_))
            {
                isFileExist = true;
                break;
            }
        }
        if (!isFileExist)
        {
            ec_ = MusicTaggerErrorCode::kJsonFileNotExist;
            return;
        }
		extractedData_.Parse<rapidjson::kParseCommentsFlag>(queryJsonString.c_str());

        Validate();

        //set up result format:{"releases":[]}
        using namespace rapidjson;
        Pointer("/releaseList").Set(result_, Value(kArrayType));
	}

	QuerySourceBase::~QuerySourceBase()
	{

	}

    void QuerySourceBase::AddReleaseToResultJson(const Release& release)
    {
        using namespace rapidjson;

        Pointer("/releaseList/-").Set(result_, kObjectType);
        const Value* releases = Pointer("/releaseList").Get(result_);
        const size_t count = releases->Size();
        const string queryStr = "/releaseList/" + std::to_string(count - 1);
        Pointer(queryStr + "/title").Set(result_, release.title);
        Pointer(queryStr + "/albumArtist").Set(result_, release.albumArtist);
        Pointer(queryStr + "/date").Set(result_, release.date);
        Pointer(queryStr + "/genre").Set(result_, release.genre);
        Pointer(queryStr + "/trackTitles").Set(result_, kArrayType);
        Pointer(queryStr + "/trackArtists").Set(result_, kArrayType);
        for (size_t i=0;i<release.trackTitle.size();i++)
        {
            Pointer(queryStr + "/trackTitles/-").Set(result_, release.trackTitle[i]);
            if (release.trackArtist.size()>i)
            {
                Pointer(queryStr + "/trackArtists/-").Set(result_, release.trackArtist[i]);
            }
        }
    }

    void QuerySourceBase::Validate()
	{
		if (!extractedData_.IsObject() || (!setting_.IsObject()))
		{
            ec_ = MusicTaggerErrorCode::kJsonFileInvalid;
			return;
		}
        
        if (ValidateJsonBySchemaFile(setting_, "siteValidator")&& ValidateJsonBySchemaFile(extractedData_, "extractValidator"))
        {
            return;
        }
        ec_ = MusicTaggerErrorCode::kJsonSchemaCheckFailed;
	}

    MusicTaggerErrorCode QuerySourceBase::Status() const
	{
        return ec_;
	}

	string QuerySourceBase::GetFinalAddress() const
	{
        assert(Status()==MusicTaggerErrorCode::kNoError);
        return *GetOptionalStringValue(setting_, "/host");
	}

    string QuerySourceBase::GetHost() const
    {
        assert(Status() == MusicTaggerErrorCode::kNoError);
        if (*GetOptionalBoolValue(setting_, "/useProxy"))
        {
            return *GetOptionalStringValue(setting_, "/proxy");
        }
        else
        {
            return *GetOptionalStringValue(setting_, "/host");
        }
    }

    string QuerySourceBase::GetPortString() const
    {
        assert(Status() == MusicTaggerErrorCode::kNoError);
        if (*GetOptionalBoolValue(setting_, "/useProxy"))
        {
            return *GetOptionalStringValue(setting_, "/proxyPort");
        }
        else
        {
            return "http";
        }
    }

	bool QuerySourceBase::NeedDetailQuery() const
	{
		assert(Status() == MusicTaggerErrorCode::kNoError);
		bool needDetailQuery = *GetOptionalBoolValue(setting_, "/needDetailQuery");
		return needDetailQuery;
	}

    std::string QuerySourceBase::GetStandardizeResult() const
    {
        using namespace rapidjson;
        StringBuffer sb;
        Writer<StringBuffer> writer(sb);
        result_.Accept(writer);
        return sb.GetString();
    }

}
