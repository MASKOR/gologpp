project(golog++)
cmake_minimum_required(VERSION 2.8)
include(CMakeDependentOption)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake PATH)

find_package(PkgConfig)
find_package(Boost REQUIRED)
find_package(EclipseClp)

#pkg_check_modules(SWIPL REQUIRED "swipl")

include_directories(${ECLIPSE_INCLUDE_DIRS} ${Boost_INCLUDE_DIR} src)
link_directories(${ECLIPSE_LIBRARY_DIRS})

add_compile_options(-Wall)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -fno-omit-frame-pointer")

find_file(READYLOG_PL preprocessor.pl
        PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../
        PATH_SUFFIXES /readylog/interpreter /kbsgolog/interpreters/readylog)
add_definitions(-DREADYLOG_PATH=\"${READYLOG_PL}\")
add_definitions(-DECLIPSE_DIR=\"${ECLIPSE_DIR}\")
add_definitions(-DSOURCE_DIR=\"${CMAKE_SOURCE_DIR}\")

set(EXTRA_COMPILER_MACROS "" CACHE STRING "Extra macro definitions to pass to the compiler with the -D option")
add_definitions(-DUSES_NO_ENGINE_HANDLE)

foreach(def ${EXTRA_COMPILER_MACROS})
	message(${def})
	add_definitions(-D${def})
endforeach()

option(USE_LIBASAN "Enable the AddressSanitizer to detect memory management bugs" OFF)
if (${USE_LIBASAN})
	add_compile_options(-fsanitize=address)
endif()

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
		src/model/global.cpp
)
add_library(golog++ SHARED ${INTERFACE_SRC})
target_compile_options(golog++ PUBLIC -fPIC)
set_property(TARGET golog++ PROPERTY CXX_STANDARD 14)


option(BUILD_PARSER "Build the parser library" ON)
CMAKE_DEPENDENT_OPTION(DEBUG_PARSER "Enable debug output for the parser" OFF "BUILD_PARSER" OFF)
if (${BUILD_PARSER})
	set(PARSER_SRC
		src/parser/atoms.cpp
		src/parser/action.cpp
		src/parser/arithmetic.cpp
		src/parser/formula.cpp
		src/parser/functions.cpp
		src/parser/utilities.cpp
		src/parser/statements.cpp
		src/parser/parser.cpp
		src/parser/assignment.cpp
		src/parser/fluent.cpp
	)
	add_library(parsegolog++ SHARED ${PARSER_SRC})
	target_link_libraries(parsegolog++ golog++)
	set_property(TARGET parsegolog++ PROPERTY CXX_STANDARD 14)
	add_definitions(-DGOLOGPP_BUILD_PARSER)
	if (${DEBUG_PARSER})
		add_definitions(-DDEBUG_PARSER)
	endif()
endif()



option(BUILD_READYLOG_IMPL "Build the readylog implementation" ON)
if (${BUILD_READYLOG_IMPL})
	set(READYLOG_SRC
		src/semantics/readylog/action.cpp
		src/semantics/readylog/atoms.cpp
		src/semantics/readylog/arithmetic.cpp
		src/semantics/readylog/execution.cpp
		src/semantics/readylog/fluent.cpp
		src/semantics/readylog/formula.cpp
		src/semantics/readylog/procedural.cpp
		src/semantics/readylog/scope.cpp
		src/semantics/readylog/semantics.cpp
	)
	add_library(readylog++ SHARED ${READYLOG_SRC})
	target_link_libraries(readylog++ golog++ ${ECLIPSE_LIBRARIES})
	set_property(TARGET readylog++ PROPERTY CXX_STANDARD 14)
	add_definitions(-DGOLOGPP_BUILD_READYLOG_IMPL)
endif()


option(BUILD_TESTS "Build tests" ON)

CMAKE_DEPENDENT_OPTION(TEST_OBJECTMODEL "Enable test for object model" ON "BUILD_TESTS" OFF)
CMAKE_DEPENDENT_OPTION(TEST_PARSER "Enable test for parser" ON "BUILD_TESTS;BUILD_PARSER" OFF)
CMAKE_DEPENDENT_OPTION(TEST_ECLIPSE "Enable test for eclipse-clp" ON "BUILD_TESTS;ECLIPSE_FOUND" OFF)
CMAKE_DEPENDENT_OPTION(TEST_READYLOG "Try running test programs in readylog" ON "BUILD_TESTS;ECLIPSE_FOUND;BUILD_READYLOG_IMPL" OFF)

if (${BUILD_TESTS})
	add_definitions(-DGOLOGPP_BUILD_TESTS)

	if (${TEST_OBJECTMODEL})
		add_definitions(-DGOLOGPP_TEST_OBJECTMODEL)
		set(TEST_LINK_LIBRARIES ${TEST_LINK_LIBRARIES} golog++)
	endif()

	if (${TEST_PARSER})
		add_definitions(-DGOLOGPP_TEST_PARSER)
		set(TEST_LINK_LIBRARIES ${TEST_LINK_LIBRARIES} parsegolog++)
	endif()

	if (${TEST_ECLIPSE})
		add_definitions(-DGOLOGPP_TEST_ECLIPSE)
		set(TEST_LINK_LIBRARIES ${TEST_LINK_LIBRARIES} ${ECLIPSE_LIBRARIES})
	endif()

	if (${TEST_READYLOG})
		add_definitions(-DGOLOGPP_TEST_READYLOG)
		set(TEST_LINK_LIBRARIES ${TEST_LINK_LIBRARIES} readylog++)
	endif()

	set(TEST_SRC
		src/tests/readylog-test.cpp)

	add_executable(readylog-test ${TEST_SRC})
	if (USE_LIBASAN)
		target_link_libraries(readylog-test asan)
	endif()
	target_link_libraries(readylog-test ${TEST_LINK_LIBRARIES})
	set_property(TARGET readylog-test PROPERTY CXX_STANDARD 14)
endif()

