#include <fstream>
#include <future>
#include <chrono>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include "MusicTagger/Config.h"
#include "MusicTagger/Query.h"

using std::string;
using namespace MusicTagger;

int main(int argc, char *argv[])
{
	using std::cout;
	using std::endl;
	namespace po = boost::program_options;
	namespace fs = boost::filesystem;
	using boost::system::error_code;
	po::options_description desc("Usage");
	desc.add_options()
		("help", "give help message")
		("data,D", po::value<string>(), "source data json string")
		("file,F", po::value <string>(), "file name that contains json data");
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
	if (vm.count("help"))
	{
		cout << desc << endl;
		return 1;
	}
	string queryJson;
	if (vm.count("data"))
	{
		//#TODOLATER borken with double quotes in string
		queryJson = vm["data"].as<string>();
	}
	else if (vm.count("file"))
	{
		const string fileName = vm["file"].as<string>();
		fs::path p(fileName);
		error_code ec;
		if (!fs::exists(p,ec))
		{
			cout << "File do not exist.\n";
			return 1;
		}
		std::ifstream file(p.string());
		string line;
		while (std::getline(file, line))
		{
			queryJson += line;
		}
	}
	else
	{
		cout << desc << endl;
		return 1;
	}

    Config config;
    Config::ConfigData configData = config.GetConfigData();
    Query query;
    for (const auto& source:configData.sourceList)
    {
        query.AddQuerySource(source.type, source.name, queryJson);
    }

    std::future<MusicTaggerErrorCode> queryResult = std::async(std::launch::async, &Query::Start, &query);
    std::future_status status;
    auto startTime = std::chrono::system_clock::now();
    for (;;)
    {
        //check timeout
        auto currTime = std::chrono::system_clock::now();
        auto currDuration = std::chrono::duration_cast<std::chrono::seconds>(currTime - startTime);
        if (currDuration.count() > configData.timeOutSeconds)
        {
            query.Cancel();
            return 0;
        }

		int percent = query.GetCompletePercent();
		cout << "Progress:" << percent << endl;

        status = queryResult.wait_for(std::chrono::milliseconds(500));
        if (status == std::future_status::ready)
        {
            break;
        }
    }
    MusicTaggerErrorCode queryStatus = queryResult.get();
    if (queryStatus ==MusicTaggerErrorCode::kNoError)
    {
        std::string result = query.GetResult();
		std::cout << result << std::endl;
    }
    else
    {
        std::cout << GetMusicTaggerErrorMessage(queryStatus) << std::endl;
    }

	cout << "Press any key to continue...\n";
    std::getchar();
    return 0;
}