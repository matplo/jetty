set(MODULE_NAME util_hepmc)
set(MODULE ${PROJECT_NAME}_${MODULE_NAME})
set(NAME_LIB ${MODULE})
set(NAME_EXE ${MODULE}_exe)

set(SOURCES_LIB
    readfile.cxx
    readertask.cxx
    eventwrapper.cxx
)

set(SOURCES_EXE main.cxx)

set(NAME_EXE ${PROJECT_NAME}-hepmc-test)

# Headers from sources
set_headers_lib(HEADERS_LIB SOURCES_LIB)
string(REPLACE ".cxx" ".h" HEADERS_EXE "${SOURCES_EXE}")

add_library(${NAME_LIB} SHARED ${SOURCES_LIB})
target_include_directories(${NAME_LIB} PUBLIC ${HEPMC_INCLUDE_DIRS} ${ROOT_INCLUDE_DIR} ${FASTJET_DIR}/include)
target_link_libraries( ${NAME_LIB} ${FASTJET_LIBS} ${PROJECT_NAME}_util ${PROJECT_NAME}_util_tasks ${HEPMC_LIBRARIES} ${ROOT_LIBRARIES})

add_executable(${NAME_EXE} ${SOURCES_EXE})
message(STATUS "HEPMC libs: ${HEPMC_LIBRARIES}")
target_link_libraries( ${NAME_EXE} ${NAME_LIB} ${HEPMC_LIBRARIES} ${ROOT_LIBRARIES})

install_targets_custom()
