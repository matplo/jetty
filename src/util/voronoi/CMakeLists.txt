set(MODULE_NAME voronoi_util)
set(MODULE ${PROJECT_NAME}_${MODULE_NAME})
set(NAME_LIB ${MODULE})
set(NAME_EXE ${MODULE}_exe)

set(SOURCES_LIB
    varea_calc.cxx
    event_area.cxx
)

set(SOURCES_EXE main.cxx)

set(NAME_EXE ${PROJECT_NAME}-voronoi-test)

# Headers from sources
string(REPLACE ".cxx" ".h" HEADERS_LIB "${SOURCES_LIB}")
string(REPLACE ".cxx" ".h" HEADERS_EXE "${SOURCES_EXE}")

add_library(${NAME_LIB} SHARED ${SOURCES_LIB})
target_include_directories(${NAME_LIB} PUBLIC ${ROOT_INCLUDE_DIR} ${FASTJET_DIR}/include ${CGAL_INCLUDE_DIR})
target_link_libraries( ${NAME_LIB} ${PROJECT_NAME}_util ${ROOT_LIBRARIES})

if (HEPMC_FOUND)
	add_executable(${NAME_EXE} ${SOURCES_EXE})
	target_link_libraries( ${NAME_EXE} ${NAME_LIB} ${HEPMC_LIBRARIES} ${ROOT_LIBRARIES} ${PROJECT_NAME}_hepmc_util)
	install(TARGETS ${NAME_EXE} RUNTIME DESTINATION bin)
else()
	message(WARNING "HepMC not found - ${NAME_EXE} part of util will not be compiled")
endif()

install(TARGETS ${NAME_EXE} RUNTIME DESTINATION bin)
install(TARGETS ${NAME_LIB} LIBRARY DESTINATION lib)
install(FILES ${HEADERS_LIB} DESTINATION include/${PROJECT_NAME}/${MODULE_NAME})
