set(MODULE ${PROJECT_NAME}Util)
set(NAME_LIB ${MODULE})

set(SOURCES_LIB
	strutil.cxx
	sysutil.cxx
	looputil.cxx
	args.cxx
	blog.cxx
)

# Headers from sources
string(REPLACE ".cxx" ".h" HEADERS_LIB "${SOURCES_LIB}")

add_library(${NAME_LIB} SHARED ${SOURCES_LIB})
target_include_directories(${NAME_LIB} PUBLIC ${PYTHIA8_DIR}/include ${ROOT_INCLUDE_DIR} ${FASTJET_DIR}/include)
target_link_libraries(${NAME_LIB} ${PYTHIA8_LDFLAGS} ${ROOT_LIBRARIES} ${FASTJET_LIBS})

install(TARGETS ${NAME_LIB} LIBRARY DESTINATION lib)

install(FILES ${HEADERS_LIB} DESTINATION include/${MODULE})

add_subdirectory(wrapper)
add_subdirectory(rstream)
add_subdirectory(pythia)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(HepMC REQUIRED HepMC HepMCfio)
if (HEPMC_FOUND)
	add_subdirectory(hepmc)
else()
	message(WARNING "HepMC not found - part of util will not be compiled")
endif()

find_path(CGAL_DIR_GUESS CGALConfig.cmake HINT $ENV{CGAL_DIR}/lib/CGAL)
message(STATUS "CGAL_GUESS in ${CGAL_DIR_GUESS}")
set(CGAL_DIR ${CGAL_DIR_GUESS})
find_package(CGAL)
if (CGAL_FOUND)
	message(STATUS "using CGAL in ${CGAL_DIR}")
	add_subdirectory(voronoi)
else()
	message(WARNING "CGAL not found - part of util will not be compiled")
endif()
