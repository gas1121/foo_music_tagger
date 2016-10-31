#undef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1

#include "JsonQuerySource.h"

#include <numeric>
#include <iostream>
#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>
#include <rapidjson/pointer.h>
#include "Utility.h"

using std::string;

namespace MusicTagger {

	JsonQuerySource::JsonQuerySource(const string& sourceName, const std::string& queryJsonString)
        :QuerySourceBase(sourceName,queryJsonString)
    {
    }

    string JsonQuerySource::SetupQueryString() const
    {
        assert(Status() == MusicTaggerErrorCode::kNoError);
		string urlTemplate = *GetOptionalStringValue(setting_, "/tocSearchUrlTemplate");

		//#TODO
        const boost::optional<int> opCount = GetOptionalIntValue(extractedData_, "/count");
        if (!opCount)
        {
            //#TODO
        }
        const int count = *opCount;

		std::vector<int> trackLength;
        for (int i = 0; i < count; i++)
        {
            const boost::optional<int> opCurrTrackLength = GetOptionalIntValue(extractedData_, "/trackLength/" + std::to_string(i));
            if (!opCurrTrackLength)
            {
                //#TODO
            }
            trackLength.push_back(*opCurrTrackLength);
        }
		assert((int)trackLength.size() == count);

		string countString = std::to_string(count);
		string offsetString = ComputeTrackOffsetString(trackLength, "+");
		//frame unit total length
		string totalLengthString = std::to_string(std::accumulate(trackLength.begin(), trackLength.end(), 0)*kFramePerSecond 
			+ kFramePerSecond*kPreSeconds);
		const string replaceString = "1+" + countString + "+" + totalLengthString + "+" + offsetString;
		boost::replace_first(urlTemplate, "{$1}", replaceString);
		boost::replace_all(urlTemplate, " ", "+");

		return UrlEncode(GBKToUTF8(urlTemplate.c_str()));
    }

    std::vector<std::string> JsonQuerySource::ConstructDetailQueryUrls(const std::string& response) const
    {
        //#TODO replace assert to prevent crash
		//#TODO other sites expect musicbrainz may need detail query
		assert(!NeedDetailQuery());
		std::vector<std::string> result(1, SetupQueryString());
		return result;
    }

    void JsonQuerySource::AddReleasesFromResponse(const std::string& response)
    {
        using namespace rapidjson;
        assert(Status() == MusicTaggerErrorCode::kNoError);

        //#TODO musicbrainz toc lookup result need to be sorted by offset
        Document document;
        document.Parse(response.c_str());
        if (document.HasParseError())
        {
            return;
        }

        //#TODO only support musicbrainz toc lookup now
        Value::ConstMemberIterator iter = document.FindMember("releases");
        if (iter == document.MemberEnd()||(!iter->value.IsArray()))
        {
            return;
        }
        const auto releaseCount = iter->value.GetArray().Size();
		for (SizeType i = 0;i < releaseCount;i++)
        {
            Release currRelease;
            const string titlePointerString = "/releases/" + std::to_string(i) + "/title";
            const Value* title = Pointer(titlePointerString.c_str()).Get(document);
            if (title!=nullptr&&title->IsString())
            {
                currRelease.title = title->GetString();
            }

            //#TODO multiple artist
            const string artistPointerString = "/releases/" + std::to_string(i) + "/artist-credit/0/name";
            const Value* artist = Pointer(artistPointerString.c_str()).Get(document);
            if (artist != nullptr&&artist->IsString())
            {
                currRelease.albumArtist = artist->GetString();
            }

            const string datePointerString = "/releases/" + std::to_string(i) + "/date";
            const Value* date = Pointer(datePointerString.c_str()).Get(document);
            if (date != nullptr&&date->IsString())
            {
                currRelease.date = date->GetString();
            }

            //#TODO genre is not in return result

            //#TODO single release may contain multiple discs, need to select exactly witch one is correct
            const string trackCountPointerString = "/releases/" + std::to_string(i) + "/media/0/track-count";
            const Value* trackCountPointer = Pointer(trackCountPointerString.c_str()).Get(document);
            int trackCount = 0;
            if (trackCountPointer != nullptr&&trackCountPointer->IsInt())
            {
                trackCount = trackCountPointer->GetInt();
            }
            currRelease.trackTitle.clear();
            currRelease.trackArtist.clear();
            //#TODO track artist
            for (int j=0;j<trackCount;j++)
            {
                const string trackTitlePointerString = "/releases/" + std::to_string(i) + "/media/0/tracks/" + std::to_string(j) + "/title";
                const Value* trackTitle = Pointer(trackTitlePointerString.c_str()).Get(document);
                if (trackTitle != nullptr&&trackTitle->IsString())
                {
                    currRelease.trackTitle.push_back(trackTitle->GetString());
					currRelease.trackArtist.push_back("");
                }
            }

            AddReleaseToResultJson(currRelease);
        }
    }
}
