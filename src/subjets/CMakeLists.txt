set(MODULE_NAME subjets)
set(MODULE ${PROJECT_NAME}_${MODULE_NAME})
set(NAME_LIB ${MODULE})
set(NAME_EXE ${MODULE}_exe)

set(SOURCES_LIB
	fjutils.cxx
	run_pythia.cxx
	run_jets.cxx
	run_test.cxx
	run_tasks.cxx
	subjettask.cxx
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(HepMC REQUIRED HepMC HepMCfio)
if (HEPMC_FOUND)
	set(SOURCES_LIB ${SOURCES_LIB} run_hepmc.cxx)
else()
	message(WARNING "HepMC not found - run_hepmc.cxx will not be compiled")
endif()

set(SOURCES_EXE main.cxx)

# Headers from sources
set_headers_lib(HEADERS_LIB SOURCES_LIB)
string(REPLACE ".cxx" ".h" HEADERS_EXE "${SOURCES_EXE}")

add_library(${NAME_LIB} SHARED ${SOURCES_LIB})
target_include_directories(${NAME_LIB} PUBLIC ${ROOT_INCLUDE_DIR} ${FASTJET_DIR}/include ${PYTHIA8_DIR}/include)
if (HEPMC_FOUND)
	target_include_directories(${NAME_LIB} PUBLIC ${HEPMC_INCLUDE_DIRS})
	add_definitions(-DUSE_HEPMC)
endif()
target_link_libraries(${NAME_LIB} ${PYTHIA8_LDFLAGS} ${FASTJET_LIBS} libRecursiveTools.a ${ROOT_LIBRARIES} ${PROJECT_NAME}_util ${PROJECT_NAME}_util_pythia ${PROJECT_NAME}_util_tasks)
if (HEPMC_FOUND)
	target_link_libraries(${NAME_LIB} ${HEPMC_LIBRARIES} ${PROJECT_NAME}_util_hepmc)
endif()

add_executable(${NAME_EXE} ${SOURCES_EXE})
target_include_directories(${NAME_EXE} PUBLIC ${ROOT_INCLUDE_DIR} ${FASTJET_DIR}/include ${PYTHIA8_DIR}/include)
target_link_libraries(${NAME_EXE} ${NAME_LIB})

install_targets_custom()
