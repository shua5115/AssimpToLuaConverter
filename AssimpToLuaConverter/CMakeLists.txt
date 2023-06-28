# CMakeList.txt : CMake project for AssimpToLuaConverter, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

# find_library(ASSIMP_LIB assimp-vc142-mtd.lib ${CMAKE_BINARY_DIR} REQUIRED)

# Add source to this project's executable.
add_executable (AssimpToLuaConverter
AssimpToLuaConverter.h AssimpToLuaConverter.cpp
lua_converter.hpp lua_converter.cpp
)
target_link_libraries(AssimpToLuaConverter assimp)