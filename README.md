# foo_music_tagger
freedb based music tagger for foobar2000 with http proxy support.

# Usage
extract zip to foobar's user-component folder. Restart foobar and it's available in right click menu.

# Build
Update cmake to latest version(3.93), and boost to at least 1.65.1
In **ThirdParty** dir, put **foobar2000 SDK** in **foobar2000** folder, **pfc** in **pfc** folder, **wtl** in **wtl** folder, **rapidjson**'s headers in **rapidjson/include** folder, **Boostorg/beast**'s headers in **boost/include** folder
in **Build** directory, run 
```
cmake .. -DBOOST_LIBRARYDIR="Your boost library dir" -DBOOST_INCLUDEDIR="Your boost header dir"
```
