#include "MusicTagger/Config.h"
#include <string>
#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include "MusicTagger/MusicTagger.h"
#include "Utility.h"

namespace {
	using std::string;
	
	static const std::string kConfigFileName = "config.json";
}

namespace MusicTagger {

	Config::Config()
	{
		using namespace rapidjson;
		Document document;
		bool isFileExist = false;
		for (std::string currPath : kJsonFilePath)
		{
			string filePath = currPath + kConfigFileName;
			if (ReadJsonFromFile(filePath, &document))
			{
				isFileExist = true;
				break;
			}
		}
		if (!isFileExist)
		{
			//set default data
			data_.timeOutSeconds = 20;
            return;
        }
        if (ValidateJsonBySchemaFile(document, "configValidator"))
        {
            data_.timeOutSeconds = 20;
            return;
        }

        const Value* siteList = Pointer("/site").Get(document);
        const auto siteCount = siteList->GetArray().Size();
        data_.sourceList.clear();
        for (SizeType i = 0;i<siteCount;i++)
        {
            ConfigData::Source source;
            const std::string currSiteNameString = "/site/" + std::to_string(i) + "/name";
            const Value* siteName = Pointer(currSiteNameString.c_str()).Get(document);
            source.name = siteName->GetString();
            const std::string currSiteTypeString = "/site/" + std::to_string(i) + "/type";
            const Value* siteType = Pointer(currSiteTypeString.c_str()).Get(document);
            const std::string type = siteType->GetString();
            if (type== "json")
            {
                source.type = QuerySourceType::kJson;
            }
            else if (type=="cddb")
            {
                source.type = QuerySourceType::kCDDB;
            }
            data_.sourceList.push_back(source);
        }
        const Value* timeout = Pointer("/timeout").Get(document);
        data_.timeOutSeconds = timeout->GetInt();
	}

    Config::~Config()
    {

    }

	Config::ConfigData Config::GetConfigData() const
	{
		return data_;
	}

}