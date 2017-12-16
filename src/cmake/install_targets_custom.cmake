#message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

FUNCTION(INSTALL_TARGETS_CUSTOM)
	if (NAME_EXE)
		install(TARGETS ${NAME_EXE} RUNTIME DESTINATION bin)
	endif(NAME_EXE)
	if (NAME_LIB)
		# message (STATUS "NAME_LIB=${NAME_LIB} CURRENT_DIR=${CMAKE_CURRENT_SOURCE_DIR}")
		install(TARGETS ${NAME_LIB} LIBRARY DESTINATION lib)
	endif(NAME_LIB)
	if (HEADERS_LIB)
		# message (STATUS "HEADERS_LIB=${HEADERS_LIB} CURRENT_DIR=${CMAKE_CURRENT_SOURCE_DIR} from ${PROJECT_SOURCE_DIR}" )
		string(REPLACE ${PROJECT_SOURCE_DIR} "" DEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
		install(FILES ${HEADERS_LIB} DESTINATION include/${PROJECT_NAME}/${DEST_DIR})
	endif(HEADERS_LIB)
ENDFUNCTION(INSTALL_TARGETS_CUSTOM)