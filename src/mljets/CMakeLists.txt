set(MODULE_NAME mljets)
set(MODULE ${PROJECT_NAME}_${MODULE_NAME})
set(NAME_LIB ${MODULE})
set(NAME_EXE ${MODULE}_exe)

set(SOURCES_LIB
	efp7.cxx 
	jet_efp.cxx
	jet.cxx
)

set(SOURCES_EXE main.cxx)

set(NAME_EXE ${PROJECT_NAME}_efp)

# include_directories(${MKL_INCLUDE_DIRS})
# target_link_libraries(untitled ${MKL_CORE_LIBRARY})

# Headers from sources
set_headers_lib(HEADERS_LIB SOURCES_LIB)
string(REPLACE ".cxx" ".h" HEADERS_EXE "${SOURCES_EXE}")

add_library(${NAME_LIB} SHARED ${SOURCES_LIB})
target_include_directories(${NAME_LIB} PUBLIC ${ROOT_INCLUDE_DIR} ${FASTJET_DIR}/include ${CGAL_INCLUDE_DIR} ${MKL_INCLUDE_DIRS})
target_link_libraries( ${NAME_LIB} ${PROJECT_NAME}_util ${ROOT_LIBRARIES} ${CGAL_LIBRARIES} ${MKL_LIBRARIES})

if (HEPMC_FOUND)
	add_executable(${NAME_EXE} ${SOURCES_EXE})
	target_link_libraries( ${NAME_EXE} ${NAME_LIB} ${HEPMC_LIBRARIES} ${ROOT_LIBRARIES} ${PROJECT_NAME}_util_hepmc)
else()
	message(WARNING "HepMC not found - ${NAME_EXE} part of util will not be compiled")
endif()

install_targets_custom()
