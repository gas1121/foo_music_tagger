#include "MusicTagger/Query.h"

#include <iostream>
#include "CddbQuerySource.h"
#include "JsonQuerySource.h"
#include "Utility.h"
#include "MusicTagger/QueryInstance.h"

namespace MusicTagger {

	Query::Query()
		:sourceEc_(MusicTaggerErrorCode::kNoError)
	{
	}

	void Query::AddQuerySource(const QuerySourceType type, const std::string& sourceName, const std::string& queryData)
	{
		QuerySourcePtr source = CreateQuerySource(type,sourceName, queryData);

		if (source->Status() == MusicTaggerErrorCode::kNoError)
		{
			instanceArray_.push_back(std::make_shared<QueryInstance>(source, &service_));
		}
		else if (sourceEc_ == MusicTaggerErrorCode::kNoError)
		{
			//store first received error code, and will abort query
			sourceEc_ = source->Status();
		}
	}

    MusicTaggerErrorCode Query::Start()
	{
		if (sourceEc_!=MusicTaggerErrorCode::kNoError)
		{
			return sourceEc_;
		}

		for (auto instance : instanceArray_)
		{
			instance->StartQueryInstance();
		}
		service_.run();
        for (auto instance : instanceArray_)
        {
            if (instance->Status()==QueryInstance::InstanceStatus::kFinished)
            {
                //if multiple instances finished, we only use one of them
                result_ = instance->GetResult();
                break;
            }
        }
		if (result_.empty()&&(!instanceArray_.empty()))
		{
            // return first query instance's error code
            return instanceArray_.front()->ErrorCode();
		}
		return MusicTaggerErrorCode::kNoError;
	}

	void Query::Cancel()
	{
		service_.stop();
	}

	int Query::GetCompletePercent() const
	{
		QueryInstance::InstanceStatus status = QueryInstance::InstanceStatus::kInitialized;
		for (auto instance : instanceArray_)
		{
			auto currStatus = instance->Status();
			if (currStatus>status)
			{
				status = currStatus;
			}
		}
		switch (status)
		{
		case QueryInstance::InstanceStatus::kFailed:
		case QueryInstance::InstanceStatus::kInitialized:
			return 0;
		case QueryInstance::InstanceStatus::kFoundSite:
			return 15;
		case QueryInstance::InstanceStatus::kConnected:
			return 33;
		case QueryInstance::InstanceStatus::kGetReleaseList:
			return 66;
		case QueryInstance::InstanceStatus::kFinished:
		default:
			return 100;
		}
		return 0;
	}

    std::string Query::GetResult() const
    {
        return result_;
    }

	std::vector<Release> ExtractReleaseListFromJsonString(const std::string& jsonString)
	{
		using namespace rapidjson;

		std::vector<Release> result;
		
		Document releaseListDocument;
		releaseListDocument.Parse(jsonString.c_str());
		if (releaseListDocument.HasParseError())
		{
			return result;
		}
		//verify jsonString format
        if (ValidateJsonBySchemaFile(releaseListDocument, "resultValidator"))
        {
            return result;
        }

		Value::ConstMemberIterator iter = releaseListDocument.FindMember("releaseList");
		if (iter==releaseListDocument.MemberEnd())
		{
			return result;
		}
		for (auto& v: iter->value.GetArray())
		{
			Release currRelease;

			Value::ConstMemberIterator detailIter = v.FindMember("title");
			if (detailIter != v.MemberEnd())
			{
				currRelease.title = detailIter->value.GetString();
			}

			detailIter = v.FindMember("albumArtist");
			if (detailIter != v.MemberEnd())
			{
				currRelease.albumArtist = detailIter->value.GetString();
			}

			detailIter = v.FindMember("date");
			if (detailIter != v.MemberEnd())
			{
				currRelease.date = detailIter->value.GetString();
			}

			detailIter = v.FindMember("genre");
			if (detailIter != v.MemberEnd())
			{
				currRelease.genre = detailIter->value.GetString();
			}

			detailIter = v.FindMember("trackTitles");
			if (detailIter != v.MemberEnd())
			{
				for (auto& v : detailIter->value.GetArray())
				{
					currRelease.trackTitle.push_back(v.GetString());
				}
			}

			detailIter = v.FindMember("trackArtists");
			if (detailIter != v.MemberEnd())
			{
				for (auto& v : detailIter->value.GetArray())
				{
					currRelease.trackArtist.push_back(v.GetString());
				}
			}
			result.push_back(currRelease);
		}
		return result;
	}
}
