#pragma once

#include <string>

namespace MusicTagger {
	static const int kPreSeconds = 2;
	static const int kFramePerSecond = 75;

    static const std::string kJsonFilePath[2] = { 
        "user-components/foo_music_tagger/json/", 
        "json/" 
    };

    static const std::string kSiteSuffix = "site";
    static const std::string kValidatorSuffix = "validator";

    //! error code for MusicTagger core library
    enum class MusicTaggerErrorCode
    {
        kNoError = 0,//!< no error

        kJsonFileNotExist,//!< can't find json file
        kJsonFileInvalid,//!< json file do not contain valid json string
        kJsonSchemaCheckFailed,//!< json schema check failed
        kNetworkError,//!< failed to connect target site
        kUnknownError
    };

    inline const std::string GetMusicTaggerErrorMessage(MusicTaggerErrorCode errorCode)
    {
        switch (errorCode)
        {
        case MusicTagger::MusicTaggerErrorCode::kNoError:
            return "No error.";
        case MusicTagger::MusicTaggerErrorCode::kJsonFileNotExist:
            return "Can not find json file.";
        case MusicTagger::MusicTaggerErrorCode::kJsonFileInvalid:
            return "Data in json file is not valid.";
        case MusicTagger::MusicTaggerErrorCode::kJsonSchemaCheckFailed:
            return "Json schema check failed.";
        case MusicTagger::MusicTaggerErrorCode::kNetworkError:
            return "Network connect failed.";
        case MusicTaggerErrorCode::kUnknownError:
        default:
            return "Unknown error.";
        }
    }
}