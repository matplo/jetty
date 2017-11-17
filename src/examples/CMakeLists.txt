set(MODULE_NAME examples)
set(MODULE ${PROJECT_NAME}_${MODULE_NAME})
set(NAME_LIB ${MODULE})
set(NAME_EXE ${MODULE}_exe)

set(SOURCES_LIB
	run_pythia.cxx
	run_pythia_wrapper.cxx
	run_pythia_pool.cxx
	et.cxx
)

set(SOURCES_EXE main.cxx)

# Headers from sources
set_headers_lib(HEADERS_LIB SOURCES_LIB)
string(REPLACE ".cxx" ".h" HEADERS_EXE "${SOURCES_EXE}")

add_library(${NAME_LIB} SHARED ${SOURCES_LIB})
target_include_directories(${NAME_LIB} PUBLIC ${ROOT_INCLUDE_DIR} ${FASTJET_DIR}/include ${PYTHIA8_DIR}/include)
target_link_libraries(${NAME_LIB} ${PYTHIA8_LDFLAGS} ${FASTJET_LIBS} ${ROOT_LIBRARIES} ${PROJECT_NAME}_util ${PROJECT_NAME}_util_pythia)

add_executable(${NAME_EXE} ${SOURCES_EXE})
target_include_directories(${NAME_EXE} PUBLIC ${ROOT_INCLUDE_DIR} ${FASTJET_DIR}/include ${PYTHIA8_DIR}/include)
target_link_libraries( ${NAME_EXE} ${NAME_LIB})

install_targets_custom()
