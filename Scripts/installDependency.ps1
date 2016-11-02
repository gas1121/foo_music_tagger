$project_path = "c:\\foo_music_tagger"
$catchHeaderPath = "ThirdParty\Catch\catch.hpp"
if (!(Test-Path $catchHeaderPath)) {
    $source = "https://raw.githubusercontent.com/philsquared/Catch/v1.5.6/single_include/catch.hpp"
    $destination = $project_path + "\ThirdParty\Catch\catch.hpp"
    mkdir ThirdParty\Catch
    Invoke-WebRequest $source -OutFile $destination
}

$beastPath = "ThirdParty\Beast"
if (!(Test-Path $beastPath)) {
    cd ThirdParty
    git clone https://github.com/vinniefalco/Beast.git
    cd ..
}

$foobar2000SDKPath = "ThirdParty\foobar2000"
if (!(Test-Path $foobar2000SDKPath)) {
    cd ThirdParty
    git clone https://github.com/gas1121/foobar2000-dependency.git
    Move-Item -Path "foobar2000-dependency\*" -Destination .
    cd ..
}

$rapidjsonPath = "ThirdParty\rapidjson"
if (!(Test-Path $rapidjsonPath)) {
    cd ThirdParty
    git clone https://github.com/miloyip/rapidjson.git
    cd ..
}