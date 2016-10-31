#include "MusicTagger/QueryInstance.h"

#include <iostream>
#include <chrono>
#include <boost/asio/spawn.hpp>
#include <beast/http.hpp>

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
                std::cout << ec.message() << std::endl;
                ec_ = MusicTaggerErrorCode::kNetworkError;
                return;
            }

            status_ = InstanceStatus::kFoundSite;
            tcp::socket sock(*service_);
            async_connect(sock, iter, yield[ec]);
            if (ec)
            {
                std::cout << ec.message() << std::endl;
                ec_ = MusicTaggerErrorCode::kNetworkError;
                return;
            }

            status_ = InstanceStatus::kConnected;
            beast::http::request_v1<beast::http::empty_body> req;
            req.method = "GET";
            req.url = source_->SetupQueryString();
            req.version = 11;
            auto ep = sock.remote_endpoint();
            string finalHost = source_->GetFinalAddress();
            req.headers.insert("Host", finalHost +
                string(":") + std::to_string(ep.port()));
            req.headers.insert("User-Agent", "foo_music_tagger");
            beast::http::prepare(req);
            beast::http::async_write(sock, req, yield[ec]);
            if (ec)
            {
                std::cout << ec.message() << std::endl;
                ec_ = MusicTaggerErrorCode::kNetworkError;
                return;
            }

            beast::streambuf sb;
            beast::http::response_v1<beast::http::string_body> res;
            beast::http::async_read(sock, sb, res, yield[ec]);
            if (ec)
            {
                std::cout << ec.message() << std::endl;
                ec_ = MusicTaggerErrorCode::kNetworkError;
                return;
            }

            status_ = InstanceStatus::kGetReleaseList;
            sb.consume(sb.size());
            if (source_->NeedDetailQuery())
            {
                std::vector<std::string> detailQueryUrls = source_->ConstructDetailQueryUrls(res.body);
                assert(!detailQueryUrls.empty());
                for (const auto& currUrl:detailQueryUrls)
                {
                    req.url = currUrl;
                    beast::http::async_write(sock, req, yield[ec]);
                    if (ec)
                    {
                        std::cout << ec.message() << std::endl;
                        ec_ = MusicTaggerErrorCode::kNetworkError;
                        return;
                    }

                    beast::http::async_read(sock, sb, res, yield[ec]);
                    if (ec)
                    {
                        std::cout << ec.message() << std::endl;
                        ec_ = MusicTaggerErrorCode::kNetworkError;
                        return;
                    }

                    source_->AddReleasesFromResponse(res.body);
                    sb.consume(sb.size());
                }
            }
            else
            {
                source_->AddReleasesFromResponse(res.body);
                sb.consume(sb.size());
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
