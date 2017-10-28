# - Find CGAL
# Find the CGAL includes and client library
# This module defines
#  CGAL_INCLUDE_DIR, where to find CGAL.h
#  CGAL_LIBRARIES, the libraries needed to use CGAL.
#  CGAL_FOUND, If false, do not try to use CGAL.

if(CGAL_INCLUDE_DIR AND CGAL_LIBRARIES)
   set(CGAL_FOUND TRUE)

else(CGAL_INCLUDE_DIR AND CGAL_LIBRARIES)

  find_path(CGAL_INCLUDE_DIR CGAL/basic.h
            $ENV{CGAL_DIR}/include
            ${CGAL_ROOT}/include
            /usr/include
            /usr/local/include
            $ENV{ProgramFiles}/CGAL/*/include
            $ENV{SystemDrive}/CGAL/*/include)

  find_library(CGAL_LIBRARIES NAMES CGAL libCGAL
               PATHS
               $ENV{CGAL_DIR}/lib
               ${CGAL_ROOT}/lib
               /usr/lib
               /usr/local/lib
               /usr/lib/CGAL
               /usr/lib64
               /usr/local/lib64
               /usr/lib64/CGAL
               $ENV{ProgramFiles}/CGAL/*/lib
               $ENV{SystemDrive}/CGAL/*/lib
               )

  mark_as_advanced(CGAL_INCLUDE_DIR CGAL_LIBRARIES)

endif(CGAL_INCLUDE_DIR AND CGAL_LIBRARIES)

if(CGAL_INCLUDE_DIR AND CGAL_LIBRARIES)
  message(STATUS "CGAL_INCLUDE_DIR=${CGAL_INCLUDE_DIR}")
  message(STATUS "CGAL_LIBRARIES=${CGAL_LIBRARIES}")
  set(CGAL_FOUND TRUE)
else(CGAL_INCLUDE_DIR AND CGAL_LIBRARIES)
  set(CGAL_FOUND FALSE)
  message(STATUS "CGAL not found.")
endif(CGAL_INCLUDE_DIR AND CGAL_LIBRARIES)
