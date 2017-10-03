# foo_music_tagger
Multiple sites based music tagger component for foobar2000.

# Build
Update cmake to at least 3.93, boost to at least 1.65.1
In **ThirdParty** dir, prepare **foobar2000** SDK, **pfc**, **rapidjson**, **beast**
in **Build** directory, run 
```
cmake .. -DBOOST_LIBRARYDIR="Your boost library dir" -DBOOST_INCLUDEDIR="Your boost header dir"
```
