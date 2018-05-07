project(gologpp)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake PATH)

find_package(PkgConfig)
find_package(Boost REQUIRED)
find_package(Eclipse)

#pkg_check_modules(SWIPL REQUIRED "swipl")

set(INTERFACE_SRC
		src/model/action.cpp
		src/model/atoms.cpp
		src/model/effect_axiom.cpp
		src/model/fluent.cpp
		src/model/expressions.cpp
		src/model/formula.cpp
		src/model/procedural.cpp
		src/model/scope.cpp
		src/model/situation.cpp
		src/model/utilities.cpp
		src/model/arithmetic.cpp
		src/model/error.cpp
		src/model/user_error.cpp
		src/model/execution.cpp
)

set(READYLOG_SRC
	src/impl/readylog/action.cpp
	src/impl/readylog/atoms.cpp
	src/impl/readylog/arithmetic.cpp
	src/impl/readylog/execution.cpp
	src/impl/readylog/fluent.cpp
	src/impl/readylog/formula.cpp
	src/impl/readylog/procedural.cpp
	src/impl/readylog/scope.cpp
	src/impl/readylog/implementation.cpp
)

set(TEST_SRC
	src/tests/readylog-test.cpp)

include_directories(${ECLIPSE_INCLUDE_DIRS} ${Boost_INCLUDE_DIR} src)
link_directories(${ECLIPSE_LIBRARY_DIRS})

add_compile_options(-Wall)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")

find_file(READYLOG_PL preprocessor.pl
        PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../
        PATH_SUFFIXES /readylog/interpreter /kbsgolog/interpreters/readylog)
add_definitions(-DREADYLOG_PATH=\"${READYLOG_PL}\" -DBACKEND=readylog)
add_definitions(-DECLIPSE_DIR=\"${ECLIPSE_DIR}\")

add_library(readylogpp SHARED ${INTERFACE_SRC} ${READYLOG_SRC})
target_link_libraries(readylogpp ${ECLIPSE_LIBRARIES})
target_compile_options(readylogpp PUBLIC -fPIC)
set_property(TARGET readylogpp PROPERTY CXX_STANDARD 17)

add_executable(readylog-test ${TEST_SRC})
target_link_libraries(readylog-test readylogpp)
set_property(TARGET readylog-test PROPERTY CXX_STANDARD 17)

