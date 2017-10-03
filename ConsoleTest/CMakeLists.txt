set(EXE_NAME ConsoleTest)

set(SOURCE
    "Src/main.cpp")

source_group("" FILES ${SOURCE})

include_directories(${CMAKE_SOURCE_DIR}/MusicTagger/include)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${BEAST_INCLUDE_DIRS})
include_directories(${RAPIDJSON_INCLUDE_DIRS})

add_executable(${EXE_NAME} ${SOURCE})

#To keep C/C++->Code Generation->Runtime Library option same with foobar project.
target_compile_options(${EXE_NAME} PRIVATE "/MT$<$<CONFIG:Debug>:d>")

#Disable safeseh
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")

target_link_libraries(${EXE_NAME} MusicTagger)
#target_link_libraries(${EXE_NAME} MusicTagger)
target_link_libraries(${EXE_NAME} ${Boost_LIBRARIES})

add_dependencies(${EXE_NAME} MusicTagger)

#disable Boost autolinking
add_definitions("-DBOOST_ALL_NO_LIB")
#disable warning for WIN32_WINNT in boost
add_definitions("-D_WIN32_WINNT=0x0501")
#disable min max micro for beast to avoid compile error when windows header is included
add_definitions(-DNOMINMAX)
