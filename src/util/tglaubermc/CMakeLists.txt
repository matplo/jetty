set(MODULE_NAME util_tglaubermc)
set(MODULE ${PROJECT_NAME}_${MODULE_NAME})
set(NAME_LIB ${MODULE})

set(SOURCES_LIB
    tglaubermc.cxx
)

# Headers from sources
set_headers_lib(HEADERS_LIB SOURCES_LIB)

# generate dictionaries
set(DICT ${MODULE_NAME}_Dict )
set(LINKDEFS LinkDef.hh)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
ROOT_GENERATE_DICTIONARY(${DICT} ${HEADERS_LIB} LINKDEF ${LINKDEFS})

add_library(${NAME_LIB} SHARED ${SOURCES_LIB} ${DICT})
target_include_directories(${NAME_LIB} PUBLIC ${ROOT_INCLUDE_DIR})
target_link_libraries(${NAME_LIB} ${PYTHIA8_LDFLAGS} ${ROOT_LIBRARIES} ${PROJECT_NAME}_util ${PROJECT_NAME}_util_pythia)

install_targets_custom()
