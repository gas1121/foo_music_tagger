#undef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1

#include "CddbQuerySource.h"

#include <numeric>
#include <boost/algorithm/string.hpp>
#include <rapidjson/pointer.h>
#include "Utility.h"

using std::string;

namespace MusicTagger {

	CddbQuerySource::CddbQuerySource(const string& sourceName, const std::string& queryJsonString)
        :QuerySourceBase(sourceName,queryJsonString)
    {
    }

    string CddbQuerySource::SetupQueryString() const
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

		string freedbID = ComputeCddbID(count, trackLength);
		string countString = std::to_string(count);
		string offsetString = ComputeTrackOffsetString(trackLength);
		string totalLengthString = std::to_string(std::accumulate(trackLength.begin(), trackLength.end(), 0));

		const string replaceString = freedbID + " " + countString + " " + offsetString + " " + totalLengthString;
		boost::replace_first(urlTemplate, "{$1}", replaceString);

		//#TODO gbk? shift_js?
		return UrlEncode(GBKToUTF8(urlTemplate.c_str()));
    }

    std::vector<std::string> CddbQuerySource::ConstructDetailQueryUrls(const std::string& response) const
    {
        assert(NeedDetailQuery());

		std::vector<std::string> dividedLines;
		boost::split(dividedLines, response, boost::is_any_of("\r\n"), boost::algorithm::token_compress_on);

        std::vector<std::string> result;
        //first line is useless
        //catch up to 10 results
        //#TODO add to config
        const size_t maxCount = 10;
        for (size_t i = 1; i < dividedLines.size(); i++)
        {
            if (dividedLines[i] == "." || i > maxCount)
            {
                break;
            }
            std::vector<std::string> dividedWords;
            boost::split(dividedWords, dividedLines[i], boost::is_any_of(" "), boost::algorithm::token_compress_on);

            string urlTemplate = *GetOptionalStringValue(setting_, "/detailUrlTemplate");

            const string replaceString = dividedWords[0] + " " + dividedWords[1];
            boost::replace_first(urlTemplate, "{$1}", replaceString);

            result.push_back(UrlEncode(GBKToUTF8(urlTemplate.c_str())));
        }
		
        return result;
    }

    void CddbQuerySource::AddReleasesFromResponse(const std::string& response)
    {
        assert(Status() == MusicTaggerErrorCode::kNoError);
        assert(NeedDetailQuery());

        Release release;
        std::vector<std::string> dividedLines;
        boost::split(dividedLines, response, boost::is_any_of("\r\n"), boost::algorithm::token_compress_on);

        //if return status is 401 then break
        std::vector<std::string> firstLineWords;
        boost::split(firstLineWords, dividedLines[0], boost::is_any_of(" "), boost::algorithm::token_compress_on);
        if (firstLineWords[0]=="401")
        {
            return;
        }

        for (const auto& currLine:dividedLines)
        {
            std::vector<std::string> dividedWords;
            boost::split(dividedWords, currLine, boost::is_any_of("="), boost::algorithm::token_compress_on);
            if (dividedWords.size()<=1)
            {
                continue;
            }

            if (dividedWords[0] == "DTITLE")
            {
                std::vector<std::string> dividedTitles;
                boost::split(dividedTitles, dividedWords[1], boost::is_any_of("/"), boost::algorithm::token_compress_on);
                release.albumArtist = dividedTitles[0];
				boost::trim(release.albumArtist);
				std::vector<std::string> usedTitles(dividedTitles.begin() + 1, dividedTitles.end());
                release.title = boost::join(usedTitles, "/");
				boost::trim(release.title);
            }
            else if (dividedWords[0] == "DYEAR")
            {
                release.date = dividedWords[1];
            }
            else if (dividedWords[0] == "DGENRE")
            {
                release.genre = dividedWords[1];
            }
            else if (dividedWords[0].find("TTITLE") != std::string::npos)
            {
                //extract track artist if exist
                std::vector<std::string> dividedTitles;
                boost::split(dividedTitles, dividedWords[1], boost::is_any_of("/"), boost::algorithm::token_compress_on);
                if (dividedTitles.size()<=1)
                {
                    release.trackTitle.push_back(dividedWords[1]);
                    release.trackArtist.push_back("");
                }
                else
                {
                    boost::trim(dividedTitles[0]);
                    release.trackArtist.push_back(dividedTitles[0]);
                    std::vector<std::string> usedTitles(dividedTitles.begin() + 1, dividedTitles.end());
                    release.trackTitle.push_back(boost::join(usedTitles, "/"));
                    boost::trim(release.trackTitle.back());
                }
            }
        }
        AddReleaseToResultJson(release);
    }
}
