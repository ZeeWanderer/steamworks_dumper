cmake_minimum_required(VERSION 3.14)

project(steamworks_dumper LANGUAGES CXX)

include(FindPkgConfig)
find_package (CAPSTONE REQUIRED capstone) # installed via VCPKG

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external)

add_executable(steamworks_dumper
    src/main.cpp
    src/linux
    src/moduleimage.h
    src/moduleimage.cpp
    src/clientmodule.h
    src/clientmodule.cpp
    src/dumperbase.h
    src/dumperbase.cpp
    src/randomstack.cpp
    src/randomstack.h
    src/enumdumper.h
    src/enumdumper.cpp
    src/clientinterfacedumper.h
    src/clientinterfacedumper.cpp
    src/callbackdumper.h
    src/callbackdumper.cpp
    src/emsgdumper.h
    src/emsgdumper.cpp)

target_include_directories(steamworks_dumper PUBLIC ${CAPSTONE_INCLUDE_DIRS})
target_link_libraries(steamworks_dumper PRIVATE capstone::capstone argparse)
