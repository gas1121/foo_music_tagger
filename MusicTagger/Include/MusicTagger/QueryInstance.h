#pragma once

#include <vector>
#include <memory>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include "MusicTagger/MusicTagger.h"
#include "MusicTagger/QuerySourceInterface.h"

namespace MusicTagger {

	class QueryInstance :public boost::noncopyable,public std::enable_shared_from_this<QueryInstance>
	{
	public:
		enum class InstanceStatus
		{
			kFailed,
			kInitialized,
			kFoundSite,
			kConnected,
			kGetReleaseList,
			kFinished
		};

		QueryInstance(const QuerySourcePtr& source, boost::asio::io_service* service);
		~QueryInstance();

		void StartQueryInstance();

		MusicTaggerErrorCode ErrorCode() const;
		InstanceStatus Status() const;
		std::string GetResult() const;

	private:
		QuerySourcePtr source_;

		boost::asio::io_service *service_;

        MusicTaggerErrorCode ec_;
		InstanceStatus status_;
		std::string result_;
	};

	typedef std::shared_ptr<QueryInstance> QueryInstancePtr;
	typedef std::vector<QueryInstancePtr> QueryInstanceArray;
}