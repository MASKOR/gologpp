###########################################################################
# This file is part of golog++.
#
# golog++ is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# golog++ is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with golog++.  If not, see <https://www.gnu.org/licenses/>.
###########################################################################

project(golog++)
cmake_minimum_required(VERSION 2.8)

include(CMakeDependentOption)
include(GNUInstallDirs)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(PkgConfig)
find_package(Boost REQUIRED COMPONENTS filesystem program_options)
find_package(EclipseClp)
find_package(Threads)

pkg_check_modules(SWIPL "swipl")


option(BUILD_TAPTENC_SEMANTICS "Build the taptenc platform model semantics" OFF)
option(USE_LIBASAN "Enable the AddressSanitizer to detect memory management bugs" OFF)
option(BUILD_PARSER "Build the parser library" ON)
CMAKE_DEPENDENT_OPTION(DEBUG_PARSER "Enable debug output for the parser" OFF "BUILD_PARSER" OFF)
if (${ECLIPSE_FOUND})
	option(BUILD_READYLOG_IMPL "Build the readylog semantics (ECLiPSe Prolog)" ON)
endif()


include_directories(${Boost_INCLUDE_DIR} src)

add_compile_options(-Wall -Wno-unknown-pragmas)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -fno-omit-frame-pointer -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O2")

add_definitions(-DSOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")

set(EXTRA_COMPILER_MACROS "" CACHE STRING "Extra macro definitions to pass to the compiler with the -D option")

foreach(def ${EXTRA_COMPILER_MACROS})
	message(${def})
	add_definitions(-D${def})
endforeach()

if (${USE_LIBASAN})
	add_compile_options(-fsanitize=address)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Options are: Debug Release RelWithDebInfo MinSizeRel."
	FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(SEMANTICS_INSTALL_DIR ${CMAKE_INSTALL_DATAROOTDIR}/golog++/semantics)


set(GOLOGPP_VERSION 0.0.1)



######################################################################################
# The golog++ language metamodel: libgolog++.so
######################################################################################

set(INTERFACE_SRC
	src/model/model_element.cpp
	src/model/abstract_reference.cpp
	src/model/action.cpp
	src/model/variable.cpp
	src/model/value.cpp
	src/model/effect_axiom.cpp
	src/model/fluent.cpp
	src/model/expressions.cpp
	src/model/formula.cpp
	src/model/procedural.cpp
	src/model/scope.cpp
	src/model/utilities.cpp
	src/model/arithmetic.cpp
	src/model/error.cpp
	src/model/reference.cpp
	src/model/global.cpp
	src/model/domain.cpp
	src/model/string.cpp
	src/model/mapping.cpp
	src/model/language.cpp
	src/model/types.cpp
	src/model/compound_expression.cpp
	src/model/list_expression.cpp
	src/model/semantics.cpp
	src/model/logger.cpp
	src/model/name.cpp

	src/execution/transition.cpp
	src/execution/history.cpp
	src/execution/activity.cpp
	src/execution/clock.cpp
	src/execution/dummy_backend.cpp
	src/execution/controller.cpp
	src/execution/plan.cpp
	src/execution/platform_backend.cpp
	src/execution/transformation.cpp

	src/model/platform/component.cpp
	src/model/platform/constraint.cpp
	src/model/platform/clock_formula.cpp
	src/model/platform/semantics.cpp
	src/model/platform/reference.cpp
	src/model/platform/switch_state_action.cpp
	src/model/platform/component_backend.cpp
)
add_library(golog++ SHARED ${INTERFACE_SRC})
target_compile_options(golog++ PUBLIC -fPIC)
target_link_libraries (golog++ ${CMAKE_THREAD_LIBS_INIT})
set_property(TARGET golog++ PROPERTY SOVERSION ${GOLOGPP_VERSION})
install(TARGETS golog++ DESTINATION ${CMAKE_INSTALL_LIBDIR})

if (NOT BUILD_TAPTENC_SEMANTICS)
	target_compile_definitions(golog++ PRIVATE USE_DUMMY_PLATFORM)
endif()

install(FILES
	src/model/model_element.h
	src/model/abstract_reference.h
	src/model/semantics.h
	src/model/formula.h
	src/model/effect_axiom.h
	src/model/variable.h
	src/model/value.h
	src/model/arithmetic.h
	src/model/procedural.h
	src/model/error.h
	src/model/scope.h
	src/model/fluent.h
	src/model/action.h
	src/model/utilities.h
	src/model/language.h
	src/model/global.h
	src/model/reference.h
	src/model/expressions.h
	src/model/gologpp.h
	src/model/domain.h
	src/model/string.h
	src/model/mapping.h
	src/model/types.h
	src/model/compound_expression.h
	src/model/list_expression.h
	src/model/logger.h
	src/model/name.h

	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/golog++/model
)

install(FILES
	src/execution/transition.h
	src/execution/history.h
	src/execution/activity.h
	src/execution/clock.h
	src/execution/dummy_backend.h
	src/execution/controller.h
	src/execution/plan.h
	src/execution/platform_backend.h
	src/execution/transformation.h

	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/golog++/execution
)

install(FILES
	src/model/platform/component.h
	src/model/platform/constraint.h
	src/model/platform/clock_formula.h
	src/model/platform/semantics.h
	src/model/platform/reference.h
	src/model/platform/switch_state_action.h
	src/model/platform/component_backend.h

	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/golog++/model/platform
)

get_target_property(GOLOGPP_CXXFLAGS_L golog++ INTERFACE_COMPILE_OPTIONS)
set(GOLOGPP_CXXFLAGS "")
if (${GOLOGPP_CXXFLAGS_L})
	foreach(i IN LISTS GOLOGPP_CXXFLAGS_L)
		set(GOLOGPP_CXXFLAGS "${GOLOGPP_CXXFLAGS} ${i}")
	endforeach()
endif()
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/golog++.pc.in ${CMAKE_CURRENT_BINARY_DIR}/golog++.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/golog++.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)



######################################################################################
# The golog++ parser: libparsegolog++.so
######################################################################################

if (${BUILD_PARSER})
	set(OPTIMIZE_PARSER 2 CACHE STRING "Optimization level for the parser (0 - 3)")
	set(PARSER_SRC
		src/parser/variable.cpp
		src/parser/action.cpp
		src/parser/arithmetic.cpp
		src/parser/formula.cpp
		src/parser/effect_axiom.cpp
		src/parser/functions.cpp
		src/parser/utilities.cpp
		src/parser/statements.cpp
		src/parser/types.cpp
		src/parser/parser.cpp
		src/parser/assignment.cpp
		src/parser/reference.cpp
		src/parser/fluent.cpp
		src/parser/expressions.cpp
		src/parser/symbolic_expression.cpp
		src/parser/string_expression.cpp
		src/parser/domain.cpp
		src/parser/compound_expression.cpp
		src/parser/mixed_member_access.cpp
		src/parser/mapping.cpp
		src/parser/list_expression.cpp
		src/parser/value.cpp

		src/parser/platform/clock_formula.cpp
		src/parser/platform/component.cpp
		src/parser/platform/constraint.cpp
		src/parser/platform/reference.cpp
	)
	add_library(parsegolog++ SHARED ${PARSER_SRC})
	target_link_libraries(parsegolog++ golog++)
	set_property(TARGET parsegolog++ PROPERTY SOVERSION ${GOLOGPP_VERSION})
	target_compile_options(parsegolog++ PRIVATE -Wno-undefined-func-template -O${OPTIMIZE_PARSER})
	if (${DEBUG_PARSER})
		target_compile_definitions(parsegolog++ PRIVATE -DBOOST_SPIRIT_DEBUG -DGOLOGPP_DEBUG_PARSER)
	endif()
	install(TARGETS parsegolog++ DESTINATION ${CMAKE_INSTALL_LIBDIR})
	install(FILES
		src/parser/grammar.h
		src/parser/parser.h
		src/parser/formula.h
		src/parser/effect_axiom.h
		src/parser/functions.h
		src/parser/variable.h
		src/parser/arithmetic.h
		src/parser/statements.h
		src/parser/types.h
		src/parser/fluent.h
		src/parser/action.h
		src/parser/utilities.h
		src/parser/assignment.h
		src/parser/reference.h
		src/parser/expressions.h
		src/parser/symbolic_expression.h
		src/parser/domain.h
		src/parser/compound_expression.h
		src/parser/mixed_member_access.h
		src/parser/mapping.h
		src/parser/list_expression.h
		src/parser/value.h
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/golog++/parser
	)

	install(FILES
		src/parser/platform/clock_formula.h
		src/parser/platform/constraint.h
		src/parser/platform/component.h
		src/parser/platform/reference.h
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/golog++/parser/platform
	)

	get_target_property(PARSEGOLOGPP_CXXFLAGS_L parsegolog++ INTERFACE_COMPILE_OPTIONS)
	set(PARSEGOLOGPP_CXXFLAGS "")
	if (${PARSEGOLOGPP_CXXFLAGS_L})
		foreach(i IN LISTS PARSEGOLOGPP_CXXFLAGS_L)
			set(PARSEGOLOGPP_CXXFLAGS "${PARSEGOLOGPP_CXXFLAGS} ${i}")
		endforeach()
	endif()
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/parsegolog++.pc.in ${CMAKE_CURRENT_BINARY_DIR}/parsegolog++.pc @ONLY)
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/parsegolog++.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()



######################################################################################
# The golog++ readylog semantics: libreadylog++.so
######################################################################################

if (${BUILD_READYLOG_IMPL})
	set(READYLOG_SRC
		src/semantics/readylog/action.cpp
		src/semantics/readylog/variable.cpp
		src/semantics/readylog/value.cpp
		src/semantics/readylog/domain.cpp
		src/semantics/readylog/arithmetic.cpp
		src/semantics/readylog/execution.cpp
		src/semantics/readylog/fluent.cpp
		src/semantics/readylog/formula.cpp
		src/semantics/readylog/procedural.cpp
		src/semantics/readylog/scope.cpp
		src/semantics/readylog/semantics.cpp
		src/semantics/readylog/utilities.cpp
		src/semantics/readylog/string.cpp
		src/semantics/readylog/history.cpp
		src/semantics/readylog/reference.cpp
		src/semantics/readylog/effect_axiom.cpp
		src/semantics/readylog/compound_expression.cpp
		src/semantics/readylog/list_expression.cpp
		src/semantics/readylog/transition.cpp
		src/semantics/readylog/activity.cpp
		src/semantics/readylog/plan.cpp
	)

	link_directories(${ECLIPSE_LIBRARY_DIRS})
	add_library(readylog++ SHARED ${READYLOG_SRC})
	target_compile_definitions(readylog++ PUBLIC
		-DECLIPSE_DIR=\"${ECLIPSE_DIR}\"
		-DUSES_NO_ENGINE_HANDLE
		-DSEMANTICS_INSTALL_DIR=\"${CMAKE_INSTALL_PREFIX}/${SEMANTICS_INSTALL_DIR}\"
	)
	target_include_directories(readylog++ PUBLIC ${ECLIPSE_INCLUDE_DIRS})
	target_link_libraries(readylog++ golog++ ${ECLIPSE_LIBRARIES} stdc++fs)
	set_property(TARGET readylog++ PROPERTY SOVERSION ${GOLOGPP_VERSION})
	install(TARGETS readylog++ DESTINATION ${CMAKE_INSTALL_LIBDIR})
	install(FILES
		src/semantics/readylog/semantics.h
		src/semantics/readylog/formula.h
		src/semantics/readylog/effect_axiom.h
		src/semantics/readylog/variable.h
		src/semantics/readylog/value.h
		src/semantics/readylog/domain.h
		src/semantics/readylog/arithmetic.h
		src/semantics/readylog/procedural.h
		src/semantics/readylog/scope.h
		src/semantics/readylog/fluent.h
		src/semantics/readylog/action.h
		src/semantics/readylog/execution.h
		src/semantics/readylog/utilities.h
		src/semantics/readylog/reference.h
		src/semantics/readylog/string.h
		src/semantics/readylog/history.h
		src/semantics/readylog/compound_expression.h
		src/semantics/readylog/list_expression.h
		src/semantics/readylog/transition.h
		src/semantics/readylog/activity.h
		src/semantics/readylog/plan.h

		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/golog++/semantics/readylog
	)

	install(FILES
		src/semantics/readylog/boilerplate.pl
		DESTINATION ${SEMANTICS_INSTALL_DIR}/readylog
	)

	get_target_property(READYLOGPP_CXXFLAGS_L readylog++ INTERFACE_COMPILE_OPTIONS)
	set(READYLOGPP_CXXFLAGS "-I${ECLIPSE_INCLUDE_DIRS}")
	if (${READYLOGPP_CXXFLAGS_L})
		foreach(i IN LISTS ${READYLOGPP_CXXFLAGS_L})
			set(READYLOGPP_CXXFLAGS "${READYLOGPP_CXXFLAGS} ${i}")
		endforeach()
	endif()
	set(READYLOGPP_LDFLAGS "${ECLIPSE_LDFLAGS}")
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/readylog++.pc.in ${CMAKE_CURRENT_BINARY_DIR}/readylog++.pc @ONLY)
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/readylog++.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()


if (BUILD_TAPTENC_SEMANTICS)
	set(TAPTENC_SRC
		external/taptenc/src/constraints/constraints.cpp
		external/taptenc/src/encoder/direct_encoder.cpp
		external/taptenc/src/encoder/enc_interconnection_info.cpp
		external/taptenc/src/encoder/encoder_utils.cpp
		external/taptenc/src/encoder/filter.cpp
		external/taptenc/src/encoder/modular_encoder.cpp
		external/taptenc/src/encoder/plan_ordered_tls.cpp
		external/taptenc/src/parser/utap_trace_parser.cpp
		external/taptenc/src/printer/xml_printer.cpp
		external/taptenc/src/printer/xta_printer.cpp
		external/taptenc/src/timed-automata/timed_automata.cpp
		external/taptenc/src/timed-automata/vis_info.cpp
		external/taptenc/src/platform_model_generator.cpp
		external/taptenc/src/transformation.cpp
		external/taptenc/src/uppaal_calls.cpp
		external/taptenc/src/utils.cpp
	)

	set(TAPTENC_SEMANTICS_SRC
		src/semantics/platform/taptenc/semantics.cpp
		src/semantics/platform/taptenc/clock_formula.cpp
		src/semantics/platform/taptenc/component.cpp
		src/semantics/platform/taptenc/constraint.cpp
		src/semantics/platform/taptenc/reference.cpp
		src/semantics/platform/taptenc/transformation.cpp
	)

	set(TAPTENC_INCLUDE_PATHS
		external/taptenc/src/constraints
		external/taptenc/src/encoder
		external/taptenc/src/parser
		external/taptenc/src/printer
		external/taptenc/src/timed-automata
		external/taptenc/src
	)

	add_library(taptenc-golog++ SHARED ${TAPTENC_SRC} ${TAPTENC_SEMANTICS_SRC})
	target_include_directories(taptenc-golog++ PUBLIC ${TAPTENC_INCLUDE_PATHS})
	set_property(TARGET taptenc-golog++ PROPERTY SOVERSION ${GOLOGPP_VERSION})
	target_link_libraries(taptenc-golog++ parsegolog++)
	target_include_directories(taptenc-golog++ PUBLIC external)
	target_compile_options(taptenc-golog++ PRIVATE -std=c++17)
	set(TEST_LIBS ${TEST_LIBS} taptenc-golog++)

	add_executable(taptenc_rcll_perception external/taptenc/src/rcll_perception.cpp ${TAPTENC_SRC})
	target_include_directories(taptenc_rcll_perception PRIVATE ${TAPTENC_INCLUDE_PATHS})
	target_compile_options(taptenc_rcll_perception PRIVATE -std=c++17)
	if (USE_LIBASAN)
		target_link_libraries(taptenc_rcll_perception asan)
	endif()
endif()



######################################################################################
# Testing (requires parser & readylog semantics)
######################################################################################

set(GPP_TESTS
	arithmetic.gpp
	blocksworld.gpp
	some_exog.gpp
	blocksworld_platform.gpp
)

if (BUILD_PARSER AND BUILD_READYLOG_IMPL)
	include(CTest)

	add_executable(gologpp-test src/tests/gologpp-test.cpp)
	if (USE_LIBASAN)
		set(TEST_LIBS asan)
	endif()

	set(TEST_LIBS ${TEST_LIBS} boost_filesystem boost_program_options parsegolog++ readylog++)
	target_link_libraries(gologpp-test ${TEST_LIBS})

	if (BUILD_TAPTENC_SEMANTICS)
		target_link_libraries(gologpp-test taptenc-golog++)
	endif()

	if (BUILD_TESTING)
		foreach(TI ${GPP_TESTS})
			add_test(
				NAME "gologpp-test_${TI}"
				COMMAND gologpp-test "${CMAKE_CURRENT_SOURCE_DIR}/src/tests/${TI}"
			)
		endforeach()
	endif()
endif()


