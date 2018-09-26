function(get_eclipse_flag ECLIPSE_BIN FLAG RESULT)
	execute_process(COMMAND
		${ECLIPSE_BIN} -e "get_flag(${FLAG}, D), printf(\"%p\", [D])"
		OUTPUT_VARIABLE CMD_OUTPUT
		ERROR_VARIABLE CMD_ERROR)
	set(${RESULT} ${CMD_OUTPUT} PARENT_SCOPE)
endfunction()

execute_process(COMMAND /usr/bin/which eclipse OUTPUT_VARIABLE ECLIPSE_BIN RESULT_VARIABLE WHICH_RESULT)

string(STRIP ${ECLIPSE_BIN} ECLIPSE_BIN)

if ((${WHICH_RESULT} EQUAL 0) AND (IS_ABSOLUTE ${ECLIPSE_BIN}))
	get_eclipse_flag(${ECLIPSE_BIN} "version" ECLIPSE_VERSION)
	get_eclipse_flag(${ECLIPSE_BIN} "installation_directory" ECLIPSE_DIR)

	if (IS_DIRECTORY ${ECLIPSE_DIR})
		message(STATUS "Found eclipse-clp ${ECLIPSE_VERSION} in ${ECLIPSE_DIR}")
		set(ECLIPSE_FOUND true)
		get_eclipse_flag(${ECLIPSE_BIN} "hostarch" ECLIPSE_HOSTARCH)
		set(ECLIPSE_LIBRARIES "eclipse")
		set(ECLIPSE_LIBDIR "${ECLIPSE_DIR}/lib/${ECLIPSE_HOSTARCH}")
		set(ECLIPSE_INCDIR "${ECLIPSE_DIR}/include/${ECLIPSE_HOSTARCH}")
		set(ECLIPSE_CFLAGS -I${ECLIPSE_INCDIR})
		set(ECLIPSE_LDFLAGS "-L${ECLIPSE_LIBDIR} -Wl,-R${ECLIPSE_LIBDIR} -leclipse")
		set(ECLIPSE_LIBRARY_DIRS ${ECLIPSE_LIBDIR})
		set(ECLIPSE_INCLUDE_DIRS ${ECLIPSE_INCDIR})
	endif()
endif()
