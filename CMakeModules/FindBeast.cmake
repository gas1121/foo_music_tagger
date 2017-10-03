set(BEAST_SEARCH_PATH "${BEAST_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/ThirdParty/boost")

find_path(BEAST_INCLUDE_DIR 
    NAMES boost/beast.hpp
    PATHS ${BEAST_SEARCH_PATH}
    PATH_SUFFIXES include)

set(BEAST_INCLUDE_DIRS ${BEAST_INCLUDE_DIR})
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Beast DEFAULT_MSG
    BEAST_INCLUDE_DIR
    BEAST_INCLUDE_DIRS)