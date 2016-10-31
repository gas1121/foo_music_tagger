cmake_minimum_required(VERSION 3.1)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

project(MusicTagger)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Third party library dependencies
find_package(Beast REQUIRED)
find_package(rapidjson REQUIRED)
find_package(Foobar2000SDK REQUIRED)

# Use static version of boost
SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost REQUIRED 
    COMPONENTS system date_time regex coroutine chrono thread filesystem program_options)

add_subdirectory(ThirdParty)
add_subdirectory(MusicTagger)
add_subdirectory(foo_music_tagger)
add_subdirectory(ConsoleTest)

set(JSON_FOLDER ${CMAKE_SOURCE_DIR}/MusicTagger/Resource/json)

install(DIRECTORY ${JSON_FOLDER}
    DESTINATION ${CMAKE_BINARY_DIR}/bin/Debug
    CONFIGURATIONS Debug 
    COMPONENT Boost)
install(DIRECTORY ${JSON_FOLDER}
    DESTINATION ${CMAKE_BINARY_DIR}/bin/Release
    CONFIGURATIONS Release 
    COMPONENT Boost)