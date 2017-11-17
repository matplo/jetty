set(MODULE_NAME util_wrapper)
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
set_headers_lib(HEADERS_LIB SOURCES_LIB)

add_library(${NAME_LIB} SHARED ${SOURCES_LIB})

add_executable(${NAME_EXE} ${SOURCES_EXE})
target_link_libraries( ${NAME_EXE} ${NAME_LIB} )

install_targets_custom()
