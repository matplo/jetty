set(MODULE_NAME util)
set(MODULE ${PROJECT_NAME}_${MODULE_NAME})
set(NAME_LIB ${MODULE})

set(SOURCES_LIB
	strutil.cxx
	sysutil.cxx
	looputil.cxx
	args.cxx
	blog.cxx
	userinfo.cxx
)

set_headers_lib(HEADERS_LIB SOURCES_LIB)

add_library(${NAME_LIB} SHARED ${SOURCES_LIB})
target_include_directories(${NAME_LIB} PUBLIC ${PYTHIA8_DIR}/include ${ROOT_INCLUDE_DIR} ${FASTJET_DIR}/include)
target_link_libraries(${NAME_LIB} ${PYTHIA8_LDFLAGS} ${ROOT_LIBRARIES} ${FASTJET_LIBS})

install_targets_custom()

add_subdirectory(wrapper)
add_subdirectory(rstream)
add_subdirectory(tasks)
add_subdirectory(pythia)
add_subdirectory(tglaubermc)

if (HEPMC_FOUND)
	add_subdirectory(hepmc)
else()
	message(WARNING "HepMC not found - part of util will not be compiled")
endif()

if (CGAL_FOUND)
	add_subdirectory(voronoi)
else()
	message(WARNING "CGAL not found - part of util will not be compiled")
endif()
