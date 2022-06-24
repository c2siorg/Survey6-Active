# cmake version to be used
cmake_minimum_required( VERSION 3.0 )

# project name
project( survey6_active )

# flags

# files

# include
include_directories( ./include ./src  )

# target
add_executable( active_probe ./src/main.cpp ./src/scan.cpp )