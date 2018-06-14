project(gologpp)
cmake_minimum_required(VERSION 2.8)
include(CMakeDependentOption)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake PATH)

find_package(PkgConfig)
find_package(Boost REQUIRED)
find_package(Eclipse)

#pkg_check_modules(SWIPL REQUIRED "swipl")

include_directories(${ECLIPSE_INCLUDE_DIRS} ${Boost_INCLUDE_DIR} src)
link_directories(${ECLIPSE_LIBRARY_DIRS})

add_compile_options(-Wall)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")

find_file(READYLOG_PL preprocessor.pl
        PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../
        PATH_SUFFIXES /readylog/interpreter /kbsgolog/interpreters/readylog)
add_definitions(-DREADYLOG_PATH=\"${READYLOG_PL}\")
add_definitions(-DECLIPSE_DIR=\"${ECLIPSE_DIR}\")

set(EXTRA_COMPILER_MACROS "" CACHE STRING "Extra macro definitions to pass to the compiler with the -D option")
add_definitions(-DUSES_NO_ENGINE_HANDLE)

foreach(def ${EXTRA_COMPILER_MACROS})
	message(${def})
	add_definitions(-D${def})
endforeach()

#add_definitions(-DHAVE_PTHREAD_H)


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
		src/model/reference.cpp
)
add_library(gologpp SHARED ${INTERFACE_SRC})
target_compile_options(gologpp PUBLIC -fPIC)
set_property(TARGET gologpp PROPERTY CXX_STANDARD 14)


option(BUILD_PARSER "Build the parser library" ON)
if (${BUILD_PARSER})
	set(PARSER_SRC
		src/parser/parser.cpp
	)
	add_library(parsegologpp SHARED ${PARSER_SRC})
	target_link_libraries(parsegologpp gologpp)
	set_property(TARGET parsegologpp PROPERTY CXX_STANDARD 14)
	add_definitions(-DGOLOGPP_BUILD_PARSER)
endif()


option(BUILD_READYLOG_IMPL "Build the readylog implementation" ON)
if (${BUILD_READYLOG_IMPL})
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
	add_library(readylogpp SHARED ${READYLOG_SRC})
	target_link_libraries(readylogpp gologpp ${ECLIPSE_LIBRARIES})
	set_property(TARGET readylogpp PROPERTY CXX_STANDARD 14)
	add_definitions(-DGOLOGPP_BUILD_READYLOG_IMPL)
endif()


option(BUILD_TESTS "Build tests" ON)
CMAKE_DEPENDENT_OPTION(TEST_OBJECTMODEL "Enable test for object model" ON "BUILD_TESTS;BUILD_READYLOG_IMPL" OFF)
CMAKE_DEPENDENT_OPTION(TEST_PARSER "Enable test for parser" ON "BUILD_TESTS;BUILD_PARSER" OFF)
CMAKE_DEPENDENT_OPTION(TEST_ECLIPSE "Enable test for eclipse-clp" ON "BUILD_TESTS;ECLIPSE_FOUND" OFF)
if (${BUILD_TESTS})
	add_definitions(-DGOLOGPP_BUILD_TESTS)
	set(TEST_LINK_LIBRARIES gologpp)

	if (${TEST_OBJECTMODEL})
		add_definitions(-DGOLOGPP_TEST_OBJECTMODEL)
		set(TEST_LINK_LIBRARIES ${TEST_LINK_LIBRARIES} readylogpp)
	endif()

	if (${TEST_PARSER})
		add_definitions(-DGOLOGPP_TEST_PARSER)
		set(TEST_LINK_LIBRARIES ${TEST_LINK_LIBRARIES} parsegologpp)
	endif()

	if (${TEST_ECLIPSE})
		add_definitions(-DGOLOGPP_TEST_ECLIPSE)
	endif()

	set(TEST_SRC
		src/tests/readylog-test.cpp)

	add_executable(readylog-test ${TEST_SRC})
	target_link_libraries(readylog-test ${TEST_LINK_LIBRARIES})
	set_property(TARGET readylog-test PROPERTY CXX_STANDARD 14)
endif()
