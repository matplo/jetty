set(MODULE_NAME wrapper)
set(MODULE ${PROJECT_NAME}_${MODULE_NAME})
set(NAME_LIB ${MODULE})
set(NAME_EXE ${MODULE}_exe)

set(SOURCES_LIB
	wraptype.cxx
	wrapcontainer.cxx
	wrapperiterator.cxx
	wrapper.cxx
	wraptestclass.cxx
)

set(SOURCES_EXE main.cxx)

set(NAME_EXE ${PROJECT_NAME}-wrapper-test)

# Headers from sources
string(REPLACE ".cxx" ".h" HEADERS_LIB "${SOURCES_LIB}")
string(REPLACE ".cxx" ".h" HEADERS_EXE "${SOURCES_EXE}")

add_library(${NAME_LIB} SHARED ${SOURCES_LIB})

add_executable(${NAME_EXE} ${SOURCES_EXE})
target_link_libraries( ${NAME_EXE} ${NAME_LIB} )

install(TARGETS ${NAME_EXE} RUNTIME DESTINATION bin)
install(TARGETS ${NAME_LIB} LIBRARY DESTINATION lib)
install(FILES ${HEADERS_LIB} DESTINATION include/${PROJECT_NAME}/${MODULE_NAME})
