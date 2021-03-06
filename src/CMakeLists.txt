project(jetty CXX)
cmake_minimum_required(VERSION 2.8.11)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" c11flag)
#message( STATUS "c11flag is ${c11flag}")
if (c11flag)
	SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11" )
else()
	CHECK_CXX_COMPILER_FLAG("-std=gnu++11" gnu11flag)
	#message( STATUS "gnu11flag is ${gnu11flag}")
	if(gnu11flag)
		SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=gnu++11" )
	else()
		message( FATAL_ERROR "Compiler does not support -std=c++11 nor -std=gnu++11" )
	endif()
endif()
message ( STATUS "CMAKE_CXX_FLAGS =${CMAKE_CXX_FLAGS}")

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

find_package(Boost REQUIRED math_c99 log log_setup)
if (BOOST-NOTFOUND)
	message(SEND_ERROR "Boost or one of the components not found. Will not compile ${EXE_NAME}")
else()
	include_directories(${Boost_INCLUDE_DIRS})
	link_libraries(${Boost_LIBRARIES})
	SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DBOOST_LOG_DYN_LINK" )
endif (BOOST-NOTFOUND)

# indep. if root built with cmake assume the root-cint is in the path
execute_process ( COMMAND root-config --prefix WORKING_DIRECTORY /tmp OUTPUT_VARIABLE ROOT_DIR OUTPUT_STRIP_TRAILING_WHITESPACE )
execute_process ( COMMAND root-config --etcdir WORKING_DIRECTORY /tmp OUTPUT_VARIABLE ROOT_ETCDIR OUTPUT_STRIP_TRAILING_WHITESPACE )
set( CMAKE_MODULE_PATH "${ROOT_ETCDIR}/cmake" )
find_package(ROOT REQUIRED EG)

execute_process ( COMMAND root-config --libs WORKING_DIRECTORY /tmp OUTPUT_VARIABLE ROOT_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE )

if(DEFINED ROOT_USE_FILE)
  include(${ROOT_USE_FILE})
else()
  include_directories(${ROOT_INCLUDE_DIRS})
endif()

set(CMAKE_MACOSX_RPATH 1)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(util)
add_subdirectory(examples)
