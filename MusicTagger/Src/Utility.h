#pragma once

#include <string>
#include <vector>
#include <boost/optional.hpp>
#include <rapidjson/document.h>

namespace MusicTagger {

    std::string ComputeCddbID(const int trackCount, std::vector<int> trackSeconds);
	std::string ComputeTrackOffsetString(std::vector<int> trackSeconds);
	std::string ComputeTrackOffsetString(std::vector<int> trackSeconds, const std::string delimiter);

	std::string GBKToUTF8(const char* strGBK);
	std::string UrlEncode(const std::string& szToEncode);
	std::string UrlDecode(const std::string& szToDecode);

	bool ReadJsonFromFile(const std::string& filePath, rapidjson::Document* document);
	bool ValidateJsonBySchemaFile(const rapidjson::Document& document, const std::string& validateFileName);

	boost::optional<int> GetOptionalIntValue(const rapidjson::Document& document, const std::string& jsonPointerString);
	boost::optional<std::string> GetOptionalStringValue(const rapidjson::Document& document, const std::string& jsonPointerString);
	boost::optional<bool> GetOptionalBoolValue(const rapidjson::Document& document, const std::string& jsonPointerString);
}