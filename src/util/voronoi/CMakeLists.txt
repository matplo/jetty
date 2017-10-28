set(MODULE ${PROJECT_NAME}VoronoiUtil)
set(NAME_LIB ${MODULE})

set(SOURCES_LIB
    varea_calc.cxx
)

set(SOURCES_EXE main.cxx)

set(NAME_EXE ${PROJECT_NAME}-voronoi-test)

# Headers from sources
string(REPLACE ".cxx" ".h" HEADERS_LIB "${SOURCES_LIB}")
string(REPLACE ".cxx" ".h" HEADERS_EXE "${SOURCES_EXE}")

add_library(${NAME_LIB} SHARED ${SOURCES_LIB})
target_include_directories(${NAME_LIB} PUBLIC ${HEPMC_INCLUDE_DIRS} ${ROOT_INCLUDE_DIR} ${FASTJET_DIR}/include)
target_link_libraries( ${NAME_LIB} ${PROJECT_NAME}Util ${HEPMC_LIBRARIES} ${ROOT_LIBRARIES})

if (HEPMC_FOUND)
	add_executable(${NAME_EXE} ${SOURCES_EXE})
	target_link_libraries( ${NAME_EXE} ${NAME_LIB} ${HEPMC_LIBRARIES} ${ROOT_LIBRARIES} ${PROJECT_NAME}HepMCUtil)
	install(TARGETS ${NAME_EXE} RUNTIME DESTINATION bin)
else()
	message(WARNING "HepMC not found - ${NAME_EXE} part of util will not be compiled")
endif()

install(TARGETS ${NAME_LIB} LIBRARY DESTINATION lib COMPONENT library)

install(FILES ${HEADERS_LIB} DESTINATION include/${MODULE})
