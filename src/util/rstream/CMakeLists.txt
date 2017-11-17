set(MODULE_NAME util_rstream)
set(MODULE ${PROJECT_NAME}_${MODULE_NAME})
set(NAME_LIB ${MODULE})

set(SOURCES_LIB
    tstream.cxx
    hstream.cxx
)

# Headers from sources
set_headers_lib(HEADERS_LIB SOURCES_LIB)

add_library(${NAME_LIB} SHARED ${SOURCES_LIB})
target_include_directories(${NAME_LIB} PUBLIC ${PYTHIA8_DIR}/include ${ROOT_INCLUDE_DIR} ${FASTJET_DIR}/include)
target_link_libraries(${NAME_LIB} ${PYTHIA8_LDFLAGS} ${ROOT_LIBRARIES} ${FASTJET_LIBS} ${PROJECT_NAME}_util)

install_targets_custom()
