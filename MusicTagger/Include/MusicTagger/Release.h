#pragma once

#include <string>
#include <vector>

namespace MusicTagger {
    struct Release
    {
        std::string title;
        std::string albumArtist;
        std::string date;
        std::string genre;
        std::vector<std::string> trackTitle;
        std::vector<std::string> trackArtist;
    };
}