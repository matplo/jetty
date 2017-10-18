set(MODULE ${PROJECT_NAME}HepMCUtil)
set(NAME_LIB ${MODULE})

set(SOURCES_LIB
    readfile.cxx
)

set(SOURCES_EXE main.cxx)

set(NAME_EXE ${PROJECT_NAME}-hepmc-test)

# Headers from sources
string(REPLACE ".cxx" ".h" HEADERS_LIB "${SOURCES_LIB}")
string(REPLACE ".cxx" ".h" HEADERS_EXE "${SOURCES_EXE}")

add_library(${NAME_LIB} SHARED ${SOURCES_LIB})
target_include_directories(${NAME_LIB} PUBLIC ${HEPMC_INCLUDE_DIRS})

add_executable(${NAME_EXE} ${SOURCES_EXE})
target_link_libraries( ${NAME_EXE} ${NAME_LIB} ${HEPMC_LIBRARIES})

install(TARGETS ${NAME_EXE} RUNTIME DESTINATION bin)
install(TARGETS ${NAME_LIB} LIBRARY DESTINATION lib COMPONENT library)

install(FILES ${HEADERS_LIB} DESTINATION include/${MODULE})
