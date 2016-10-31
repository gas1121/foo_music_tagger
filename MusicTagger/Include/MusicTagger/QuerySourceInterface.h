#pragma once

#include <vector>
#include <string>
#include <memory>
#include <boost/noncopyable.hpp>
#include <MusicTagger/MusicTagger.h>

namespace MusicTagger {

	//! interface for setting up query detail data and turn result into json format
	class QuerySourceInterface :public boost::noncopyable
	{
	public:
		QuerySourceInterface() = default;
		virtual ~QuerySourceInterface() = default;

		virtual MusicTaggerErrorCode Status() const = 0;

		virtual std::string GetFinalAddress() const = 0;
		virtual std::string GetHost() const = 0;
		virtual std::string GetPortString() const = 0;
		virtual std::string SetupQueryString() const = 0;
		virtual bool NeedDetailQuery() const = 0;
		virtual std::vector<std::string> ConstructDetailQueryUrls(const std::string& response) const = 0;
        virtual void AddReleasesFromResponse(const std::string& response) = 0;
		virtual std::string GetStandardizeResult() const = 0;
	};

	typedef std::shared_ptr<QuerySourceInterface> QuerySourcePtr;
	typedef std::vector<QuerySourcePtr> QuerySourceArray;
}