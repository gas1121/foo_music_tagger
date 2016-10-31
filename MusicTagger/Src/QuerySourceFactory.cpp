#include "MusicTagger/QuerySourceFactory.h"

#include "CddbQuerySource.h"
#include "JsonQuerySource.h"

namespace MusicTagger {
	
	QuerySourcePtr CreateQuerySource
	(
		const QuerySourceType sourceType, 
		const std::string& sourceName, 
		const std::string& queryJsonString
	)
	{
		switch (sourceType)
		{
		case QuerySourceType::kCDDB:
			return std::make_shared<CddbQuerySource>(sourceName, queryJsonString);
		case QuerySourceType::kJson:
		default:
			return std::make_shared<JsonQuerySource>(sourceName, queryJsonString);
		}
	}
}
