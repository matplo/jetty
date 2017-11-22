project(jetty CXX)
cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)
message(STATUS "CMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}")

include(safeguards)
include(set_headers_lib)
include(install_targets_custom)

execute_process ( COMMAND fastjet-config --prefix WORKING_DIRECTORY /tmp OUTPUT_VARIABLE FASTJET_DIR OUTPUT_STRIP_TRAILING_WHITESPACE )
execute_process ( COMMAND fastjet-config --cxxflags WORKING_DIRECTORY /tmp OUTPUT_VARIABLE FASTJET_CXXFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE )
execute_process ( COMMAND fastjet-config --libs --plugins WORKING_DIRECTORY /tmp OUTPUT_VARIABLE FASTJET_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE )
message( STATUS "FASTJET_DIR: ${FASTJET_DIR}")
message( STATUS "FASTJET_CXXFLAGS: ${FASTJET_CXXFLAGS}")
message( STATUS "FASTJET_LIBS: ${FASTJET_LIBS}")

execute_process ( COMMAND pythia8-config --prefix WORKING_DIRECTORY /tmp OUTPUT_VARIABLE PYTHIA8_DIR OUTPUT_STRIP_TRAILING_WHITESPACE )
execute_process ( COMMAND pythia8-config --cxxflags WORKING_DIRECTORY /tmp OUTPUT_VARIABLE PYTHIA8_CXXFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE )
execute_process ( COMMAND pythia8-config --ldflags WORKING_DIRECTORY /tmp OUTPUT_VARIABLE PYTHIA8_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE )
message( STATUS "PYTHIA8_DIR: ${PYTHIA8_DIR}")
message( STATUS "PYTHIA8_CXXFLAGS: ${PYTHIA8_CXXFLAGS}")
message( STATUS "PYTHIA8_LDFLAGS: ${PYTHIA8_LDFLAGS}")

# indep. if root built with cmake assume the root-cint is in the path
execute_process ( COMMAND root-config --prefix WORKING_DIRECTORY /tmp OUTPUT_VARIABLE ROOT_DIR OUTPUT_STRIP_TRAILING_WHITESPACE )
execute_process ( COMMAND root-config --etcdir WORKING_DIRECTORY /tmp OUTPUT_VARIABLE ROOT_ETCDIR OUTPUT_STRIP_TRAILING_WHITESPACE )
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ROOT_ETCDIR}/cmake)
find_package(ROOT REQUIRED EG)

execute_process ( COMMAND root-config --libs WORKING_DIRECTORY /tmp OUTPUT_VARIABLE ROOT_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE )

if(DEFINED ROOT_USE_FILE)
  include(${ROOT_USE_FILE})
else()
  include_directories(${ROOT_INCLUDE_DIRS})
endif()

find_package(Boost REQUIRED math_c99 log log_setup)
if (BOOST-NOTFOUND)
	message(SEND_ERROR "Boost or one of the components not found. Will not compile ${EXE_NAME}")
else()
	include_directories(${Boost_INCLUDE_DIRS})
	link_libraries(${Boost_LIBRARIES})
	SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DBOOST_LOG_DYN_LINK" )
endif (BOOST-NOTFOUND)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(HepMC REQUIRED HepMC HepMCfio)
if (HEPMC_FOUND)
	include_directories(${HEPMC_INCLUDE_DIRS})
	add_definitions(-DUSE_HEPMC)
endif()

find_package(CGAL)

set(CMAKE_MACOSX_RPATH 1)

message( STATUS "CMAKE_CURRENT_SOURCE_DIR is ${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_subdirectory(util)
add_subdirectory(examples)
add_subdirectory(subjets)
add_subdirectory(gamma_h)
add_subdirectory(spectra)
add_subdirectory(gen)
add_subdirectory(area)

# FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/../include/*.h")
# INSTALL(FILES ${files} DESTINATION include)

# make install and cpack
# include(install_target)
