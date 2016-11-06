#include "stdafx.h"
#include "QueryThread.h"

#include <rapidjson/pointer.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <future>
#include <chrono>
#include "MusicTagger/Config.h"
#include "MusicTagger/Query.h"
#include "TaggerDialog.h"

using namespace MusicTagger;

namespace {
    const int kTimeoutSeconds = 20;
}

namespace MusicTagger {

	QueryThread::QueryThread(pfc::list_t<metadb_handle_ptr> tracks, TaggerDialog* targetDialog)
		:tracks_(tracks), taggerDialog_(targetDialog)
	{

	}

	void QueryThread::run(threaded_process_status & p_status, abort_callback & p_abort)
    {
		//prepare query data
        std::string queryJson;
        if (!PrepareQueryJsonString(&queryJson))
        {
            popup_message::g_show("Can not prepare query data", "QueryThread");
            return;
        }

        Config config;
        Config::ConfigData configData = config.GetConfigData();
        Query query;
        //set query source
        for (const auto& source : configData.sourceList)
        {
            query.AddQuerySource(source.type, source.name, queryJson);
        }

		std::future<MusicTaggerErrorCode> queryResult = std::async(std::launch::async, &Query::Start, &query);
		std::future_status status;
        auto startTime = std::chrono::system_clock::now();
		int completePercent = 0;
        for (;;)
        {
            //check if user canceled
            try
            {
                p_abort.check();
            }
            catch (pfc::exception)
            {
				query.Cancel();
                popup_message::g_show("Query canceled", "QueryThread");
                return;
            }
            //check timeout
            auto currTime = std::chrono::system_clock::now();
            auto currDuration = std::chrono::duration_cast<std::chrono::seconds>(currTime - startTime);
            if (currDuration.count() > kTimeoutSeconds)
            {
				query.Cancel();
				popup_message::g_show("Timeout", "QueryThread");
                return;
            }

			//check progress status
			int newCompletePercent = query.GetCompletePercent();
			if (completePercent != newCompletePercent)
			{
				completePercent = newCompletePercent;
				p_status.set_progress(threaded_process_status::progress_max * completePercent/100);
			}

            status = queryResult.wait_for(std::chrono::milliseconds(500));
            if (status == std::future_status::ready)
            {
                break;
            }
        }
		MusicTaggerErrorCode queryStatus = queryResult.get();
		if (queryStatus == MusicTaggerErrorCode::kNoError)
		{
			std::string result = query.GetResult();
			std::vector<Release> releaseList = ExtractReleaseListFromJsonString(result);
			if (releaseList.size()>0)
			{
				taggerDialog_->DisplayDialog(releaseList);
			}
			else
			{
				//close tagger dialog if no release available
				taggerDialog_->OnClose();
				popup_message::g_show("No release available.", "QueryThread");
			}
		}
		else
		{
			popup_message::g_show(GetMusicTaggerErrorMessage(queryStatus).c_str(), "QueryThread");
		}
    }

    bool QueryThread::PrepareQueryJsonString(std::string* queryJsonString)
    {
		using namespace rapidjson;
		assert(queryJsonString);
		Document d;

		const int count = tracks_.get_count();
		Pointer("/count").Set(d, count);

        Pointer("/trackLength").Set(d, Value(kArrayType));
		for (t_size i = 0; i < tracks_.get_count(); ++i) 
		{
			const double currTrackSeconds = tracks_[i]->get_length();
			//#TODO use double type to improve accuracy
			Pointer("/trackLength/-").Set(d, (int)currTrackSeconds);
		}

		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		d.Accept(writer);
        *queryJsonString = buffer.GetString();
        return true;
    }
}
