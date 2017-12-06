set(MODULE_NAME util_pythia)
set(MODULE ${PROJECT_NAME}_${MODULE_NAME})
set(NAME_LIB ${MODULE})

set(SOURCES_LIB
	pyargs.cxx
	pyutil.cxx
	outkinematics.cxx
	crosssections.cxx
	pythia_wrapper.cxx
	presample.cxx
	scanhard.cxx
	pythia_pool.cxx
	event_pool.cxx
	param_sigmas.cxx
)

# Headers from sources
set_headers_lib(HEADERS_LIB SOURCES_LIB)

add_library(${NAME_LIB} SHARED ${SOURCES_LIB})
target_include_directories(${NAME_LIB} PUBLIC ${PYTHIA8_DIR}/include ${ROOT_INCLUDE_DIR} ${FASTJET_DIR}/include)
target_link_libraries(${NAME_LIB} ${PYTHIA8_LDFLAGS} ${ROOT_LIBRARIES} ${FASTJET_LIBS} ${PROJECT_NAME}_util ${PROJECT_NAME}_util_rstream)

install_targets_custom()
