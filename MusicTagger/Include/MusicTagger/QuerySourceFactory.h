#pragma once

#include <MusicTagger/MusicTagger.h>
#include <MusicTagger/QuerySourceInterface.h>

namespace MusicTagger {
	enum class QuerySourceType
	{
		kCDDB,
		kJson
	};

	QuerySourcePtr CreateQuerySource
	(
		const QuerySourceType sourceType,
		const std::string& sourceName, 
		const std::string& queryJsonString
	);
}