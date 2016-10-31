#include <vector>
#include <boost/noncopyable.hpp>
#include <rapidjson/document.h>
#include "MusicTagger/QuerySourceFactory.h"

namespace MusicTagger {
	class Config : public boost::noncopyable
	{
	public:
		struct ConfigData
		{
			struct Source
			{
				std::string name;
                QuerySourceType type;
			};
			int timeOutSeconds;
			std::vector<Source> sourceList;
		};

        Config();
        ~Config();

		ConfigData GetConfigData() const;

    private:
		ConfigData data_;
    };
}