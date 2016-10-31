#pragma once

#include <MusicTagger/MusicTagger.h>
#include "QuerySourceBase.h"

namespace MusicTagger {

    class CddbQuerySource :public QuerySourceBase
    {
    public:
        CddbQuerySource(const std::string& sourceName, const std::string& queryJsonString);
		~CddbQuerySource() = default;

        virtual std::string SetupQueryString() const override;
        virtual std::vector<std::string> ConstructDetailQueryUrls(const std::string& response) const override;
        virtual void AddReleasesFromResponse(const std::string& response) override;
    };
}