#pragma once

#include <rapidjson/document.h>
#include "MusicTagger/MusicTagger.h"
#include "MusicTagger/Release.h"
#include "MusicTagger/QuerySourceInterface.h"

namespace MusicTagger {
	//! class for setting up query detail data and turn result into json format
	class QuerySourceBase:public QuerySourceInterface
	{
	public:
		QuerySourceBase(const std::string& sourceName, const std::string& queryJsonString);
		~QuerySourceBase();

        virtual MusicTaggerErrorCode Status() const override;

        virtual std::string GetFinalAddress() const override;
        virtual std::string GetHost() const override;
        virtual std::string GetPortString() const override;
		virtual bool NeedDetailQuery() const override;
        virtual std::string GetStandardizeResult() const override;

	protected:
		rapidjson::Document extractedData_;
		rapidjson::Document setting_;
        rapidjson::Document result_;
		MusicTaggerErrorCode ec_;

        void AddReleaseToResultJson(const Release& release);

	private:
		void Validate();
	};
}