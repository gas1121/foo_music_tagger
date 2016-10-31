set(LIB_NAME foo_music_tagger)

set(SOURCE
    "Src/stdafx.h"
    "Src/foo_music_tagger.cpp"
    "Src/ContextMenu.cpp"
    "Src/TaggerDialog.h"
    "Src/TaggerDialog.cpp"
    "Src/QueryThread.h"
    "Src/QueryThread.cpp"
    "Src/resource.h")

set(RESOURCE_FILES
    "Src/${LIB_NAME}.rc")

source_group("" FILES ${SOURCE})
source_group("Resource Files" FILES ${RESOURCE_FILES})

include_directories(${CMAKE_SOURCE_DIR}/MusicTagger/include)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${BEAST_INCLUDE_DIRS})
include_directories(${RAPIDJSON_INCLUDE_DIRS})
include_directories(${FOOBAR2000SDK_INCLUDE_DIRS})

add_library(${LIB_NAME} SHARED ${SOURCE} ${RESOURCE_FILES})

#To keep C/C++->Code Generation->Runtime Library option same with foobar project.
target_compile_options(${LIB_NAME} PRIVATE "/MT$<$<CONFIG:Debug>:d>")

#Unicode support
target_compile_definitions(${LIB_NAME} PRIVATE -D_UNICODE -DUNICODE)

target_link_libraries(${LIB_NAME} MusicTagger)
target_link_libraries(${LIB_NAME} ${FOOBAR2000SDK_LIBRARIES})

add_dependencies(${LIB_NAME} pfc)
add_dependencies(${LIB_NAME} foobar2000_SDK)
add_dependencies(${LIB_NAME} foobar2000_sdk_helpers)
add_dependencies(${LIB_NAME} foobar2000_component_client)
add_dependencies(${LIB_NAME} MusicTagger)

#disable min max micro for beast to avoid compile error when windows header is included
add_definitions(-DNOMINMAX)
