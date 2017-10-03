#include "MusicTagger/QueryInstance.h"

#include <iostream>
#include <chrono>
#include <boost/beast.hpp>
#include <boost/asio/spawn.hpp>

using std::string;

namespace MusicTagger {

	QueryInstance::QueryInstance(const QuerySourcePtr& source, boost::asio::io_service* service)
		:source_(source), service_(service), ec_(MusicTaggerErrorCode::kNoError), status_(InstanceStatus::kInitialized), result_("")
	{

	}

	QueryInstance::~QueryInstance()
	{
	}

	void QueryInstance::StartQueryInstance()
	{
        using boost::system::error_code;
        using namespace boost::asio;
        using boost::asio::ip::tcp;
        namespace http = boost::beast::http;

        auto self = shared_from_this();
        spawn(*service_,
            [this, self](yield_context yield) {
            error_code ec;
            string host = source_->GetHost();
            string port = source_->GetPortString();
            tcp::resolver r(*service_);
            auto iter = r.async_resolve(tcp::resolver::query{ host, port }, yield[ec]);
            if (ec)
            {
                ec_ = MusicTaggerErrorCode::kNetworkError;
                return;
            }

            status_ = InstanceStatus::kFoundSite;
            tcp::socket sock(*service_);
            async_connect(sock, iter, yield[ec]);
            if (ec)
            {
                ec_ = MusicTaggerErrorCode::kNetworkError;
                return;
            }

            status_ = InstanceStatus::kConnected;
            http::request<http::string_body> req(http::verb::get, source_->SetupQueryString(), 11);
            req.set(http::field::host, source_->GetFinalAddress());
            req.set(http::field::user_agent, "foo_music_tagger");
            auto ep = sock.remote_endpoint();

            http::async_write(sock, req, yield[ec]);
            if (ec)
            {
                ec_ = MusicTaggerErrorCode::kNetworkError;
                return;
            }

            boost::beast::flat_buffer buff;
            http::response<http::string_body> res;
            http::async_read(sock, buff, res, yield[ec]);
            if (ec)
            {
                ec_ = MusicTaggerErrorCode::kNetworkError;
                return;
            }

            status_ = InstanceStatus::kGetReleaseList;
            buff.consume(buff.size());
            if (source_->NeedDetailQuery())
            {
                std::vector<std::string> detailQueryUrls = source_->ConstructDetailQueryUrls(res.body());
                assert(!detailQueryUrls.empty());
                for (const auto& currUrl:detailQueryUrls)
                {
                    req.set(http::field::uri, currUrl);
                    http::async_write(sock, req, yield[ec]);
                    if (ec)
                    {
                        ec_ = MusicTaggerErrorCode::kNetworkError;
                        return;
                    }

                    http::async_read(sock, buff, res, yield[ec]);
                    if (ec)
                    {
                        ec_ = MusicTaggerErrorCode::kNetworkError;
                        return;
                    }

                    source_->AddReleasesFromResponse(res.body());
                    buff.consume(buff.size());
                }
            }
            else
            {
                source_->AddReleasesFromResponse(res.body());
                buff.consume(buff.size());
            }
            result_ = source_->GetStandardizeResult();

            status_ = InstanceStatus::kFinished;
            //cancel all other instance
            service_->stop();
        });
	}

    MusicTaggerErrorCode QueryInstance::ErrorCode() const
	{
		return ec_;
	}

	QueryInstance::InstanceStatus QueryInstance::Status() const
	{
		return status_;
	}

	string QueryInstance::GetResult() const
	{
		return result_;
	}
}
