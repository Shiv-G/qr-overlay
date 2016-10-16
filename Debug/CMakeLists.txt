cmake_minimum_required (VERSION 2.6)
project (ZBARCV)

set(CMAKE_MODULE_PATH ${ZBARCV_SOURCE_DIR})
find_package (OpenCV)
find_package (ZBar0)


find_package (Awesomium REQUIRED)
include_directories (${AWESOMIUM_INCLUDE_DIR})
add_executable (zbar_opencv web.cpp main.cpp)
target_link_libraries (zbar_opencv ${OpenCV_LIBRARIES} ${ZBAR_LIBRARIES} ${AWESOMIUM_LIBRARY})
