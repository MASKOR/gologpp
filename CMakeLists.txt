project(gologpp)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake PATH)

find_package(PkgConfig)
find_package(Boost REQUIRED)
find_package(Eclipse)

pkg_check_modules(SWIPL REQUIRED "swipl")

file(GLOB INTERFACE_SRC src/gologpp/*.cpp)
file(GLOB READYLOG_SRC src/backends/readylog/*.cpp)

add_compile_options(${SWIPL_CFLAGS} -Wall)
foreach(flag IN LISTS SWIPL_LDFLAGS)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${flag}")
endforeach()
include_directories(${ECLIPSE_INCLUDE_DIRS} ${Boost_INCLUDE_DIR} src)

add_library(readylogpp SHARED ${INTERFACE_SRC} ${READYLOG_SRC})

set_property(TARGET readylogpp PROPERTY CXX_STANDARD 14)


