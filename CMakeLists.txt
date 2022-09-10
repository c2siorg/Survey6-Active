# cmake version to be used
cmake_minimum_required( VERSION 3.0 )

# project name
project( survey6_active )
find_package(Boost COMPONENTS program_options REQUIRED)

# include
include_directories( ${Boost_INCLUDE_DIR} ./include ./src  )

# target
add_executable( active_probe ./src/main.cpp ./src/scan.cpp include/survery6_active/ping.h src/ping.cpp)
target_link_libraries( active_probe LINK_PUBLIC  ${Boost_LIBRARIES} )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
