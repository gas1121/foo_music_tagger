set(LIB_NAME MusicTagger)

set(HEADER
    "Include/MusicTagger/MusicTagger.h"
    "Include/MusicTagger/Config.h"
    "Include/MusicTagger/Release.h"
    "Include/MusicTagger/QuerySourceInterface.h"
    "Include/MusicTagger/QuerySourceFactory.h"
    "Include/MusicTagger/Query.h")

set(QUERY_SOURCE
    "Src/QuerySourceFactory.cpp"
    "Src/CddbQuerySource.h"
    "Src/CddbQuerySource.cpp"
    "Src/JsonQuerySource.h"
    "Src/JsonQuerySource.cpp"
    "Src/QuerySourceBase.h"
    "Src/QuerySourceBase.cpp")

set(SOURCE
    "Src/Config.cpp"
    "Src/Utility.h"
    "Src/Utility.cpp"
    "Src/Query.cpp"
    "Include/MusicTagger/QueryInstance.h"
    "Src/QueryInstance.cpp")



source_group("Source" FILES ${SOURCE})
source_group("Source\\QuerySource" FILES ${QUERY_SOURCE})
source_group("Header" FILES ${HEADER})

include_directories(./Include)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${BEAST_INCLUDE_DIRS})
include_directories(${RAPIDJSON_INCLUDE_DIRS})

add_library(${LIB_NAME} STATIC ${SOURCE} ${HEADER} ${QUERY_SOURCE})

#To keep C/C++->Code Generation->Runtime Library option same with foobar project.
target_compile_options(${LIB_NAME} PRIVATE "/MT$<$<CONFIG:Debug>:d>")

#Unicode support
target_compile_definitions(${LIB_NAME} PRIVATE -D_UNICODE -DUNICODE)

target_link_libraries(${LIB_NAME} ${Boost_LIBRARIES})

#disable min max micro for beast to avoid compile error when windows header is included
add_definitions(-DNOMINMAX)
#disable Boost autolinking
add_definitions("-DBOOST_ALL_NO_LIB")
#disable warning for WIN32_WINNT in boost
add_definitions("-D_WIN32_WINNT=0x0501")
