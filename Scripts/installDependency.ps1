$project_path = "c:\\foo_music_tagger"
$catchHeaderPath = "ThirdParty\Catch\catch.hpp"
if (!(Test-Path $catchHeaderPath)) {
    $source = "https://raw.githubusercontent.com/philsquared/Catch/v1.5.6/single_include/catch.hpp"
    $destination = $project_path + "\ThirdParty\Catch\catch.hpp"
    mkdir ThirdParty\Catch
    Invoke-WebRequest $source -OutFile $destination
}