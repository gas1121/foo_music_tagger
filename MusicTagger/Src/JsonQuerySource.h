#pragma once

#include <MusicTagger/MusicTagger.h>
#include "QuerySourceBase.h"

namespace MusicTagger {

    class JsonQuerySource :public QuerySourceBase
    {
    public:
        JsonQuerySource(const std::string& sourceName, const std::string& queryJsonString);
		~JsonQuerySource() = default;

        virtual std::string SetupQueryString() const override;
        virtual std::vector<std::string> ConstructDetailQueryUrls(const std::string& response) const override;
        virtual void AddReleasesFromResponse(const std::string& response) override;
    };
}