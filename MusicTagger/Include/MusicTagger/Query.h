#pragma once

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include "MusicTagger/MusicTagger.h"
#include "MusicTagger/Release.h"
#include "MusicTagger/QuerySourceInterface.h"
#include "MusicTagger/QuerySourceFactory.h"
#include "MusicTagger/QueryInstance.h"

namespace MusicTagger {
	//! Core query class
	/*! 
		 return json string when query succeeded.
	*/
    class Query:public boost::noncopyable
    {
    public:
        Query();
		~Query() = default;

        //#TODO remove this function, use config file to set query source
		void AddQuerySource(const QuerySourceType type, const std::string& sourceName, const std::string& queryData);

        MusicTaggerErrorCode Start();
		void Cancel();
		//! complete percent for query progress
		//! @return 0-100
		int GetCompletePercent() const;
        std::string GetResult() const;

	private:
		boost::asio::io_service service_;
		MusicTaggerErrorCode sourceEc_;
		QueryInstanceArray instanceArray_;

        std::string result_;
	};

	std::vector<Release> ExtractReleaseListFromJsonString(const std::string& jsonString);
}