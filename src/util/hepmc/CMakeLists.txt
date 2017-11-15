set(MODULE_NAME hepmc_util)
set(MODULE ${PROJECT_NAME}_${MODULE_NAME})
set(NAME_LIB ${MODULE})
set(NAME_EXE ${MODULE}_exe)

set(SOURCES_LIB
    readfile.cxx
)

set(SOURCES_EXE main.cxx)

set(NAME_EXE ${PROJECT_NAME}-hepmc-test)

# Headers from sources
string(REPLACE ".cxx" ".h" HEADERS_LIB "${SOURCES_LIB}")
string(REPLACE ".cxx" ".h" HEADERS_EXE "${SOURCES_EXE}")

add_library(${NAME_LIB} SHARED ${SOURCES_LIB})
target_include_directories(${NAME_LIB} PUBLIC ${HEPMC_INCLUDE_DIRS} ${ROOT_INCLUDE_DIR} ${FASTJET_DIR}/include)
target_link_libraries( ${NAME_LIB} ${PROJECT_NAME}_util ${HEPMC_LIBRARIES} ${ROOT_LIBRARIES})

add_executable(${NAME_EXE} ${SOURCES_EXE})
message(STATUS "HEPMC libs: ${HEPMC_LIBRARIES}")
target_link_libraries( ${NAME_EXE} ${NAME_LIB} ${HEPMC_LIBRARIES} ${ROOT_LIBRARIES})

install(TARGETS ${NAME_EXE} RUNTIME DESTINATION bin)
install(TARGETS ${NAME_LIB} LIBRARY DESTINATION lib)
install(FILES ${HEADERS_LIB} DESTINATION include/${PROJECT_NAME}/${MODULE_NAME})
