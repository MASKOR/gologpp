project(gologpp)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake PATH)

find_package(PkgConfig)
find_package(Boost REQUIRED)
find_package(Eclipse)

#pkg_check_modules(SWIPL REQUIRED "swipl")

#file(GLOB INTERFACE_SRC src/gologpp/*.cpp)
set(INTERFACE_SRC
		src/gologpp/Action.cpp
		src/gologpp/atoms.cpp
		src/gologpp/EffectAxiom.cpp
		src/gologpp/Fluent.cpp
		src/gologpp/Formula.cpp
		src/gologpp/Procedure.cpp
		src/gologpp/Translation.cpp
		src/gologpp/Language.cpp
		src/gologpp/utilities.cpp
)

#file(GLOB READYLOG_SRC src/backends/readylog/*.cpp)
set(READYLOG_SRC
	src/backends/readylog/gologpp.cpp
	src/backends/readylog/Action.cpp
	src/backends/readylog/atoms.cpp
	src/backends/readylog/EffectAxiom.cpp
	src/backends/readylog/Execution.cpp
	src/backends/readylog/Fluent.cpp
	src/backends/readylog/Formula.cpp
	src/backends/readylog/Procedure.cpp
	src/backends/readylog/Reference.cpp
	src/backends/readylog/Implementation.cpp
)

set(TEST_SRC
	src/tests/readylogpp.cpp)

#add_compile_options(${SWIPL_CFLAGS} -Wall)
#foreach(flag IN LISTS SWIPL_LDFLAGS)
#    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${flag}")
#endforeach()
include_directories(${ECLIPSE_INCLUDE_DIRS} ${Boost_INCLUDE_DIR} src)
link_directories(${ECLIPSE_LIBRARY_DIRS})

find_file(READYLOG_PL readylog.pl
        PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../
        PATH_SUFFIXES /readylog/interpreter /kbsgolog/interpreters/readylog)
add_definitions(-DREADYLOG_PATH="\\\"${READYLOG_PL}\\\"" -DBACKEND=readylog)

add_library(readylogpp SHARED ${INTERFACE_SRC} ${READYLOG_SRC})
target_link_libraries(readylogpp ${ECLIPSE_LIBRARIES})

add_executable(readylog-test ${TEST_SRC})
target_link_libraries(readylog-test readylogpp)

set_property(TARGET readylogpp PROPERTY CXX_STANDARD 14)

