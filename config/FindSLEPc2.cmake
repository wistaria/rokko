# - Try to find SLEPC
# Once done this will define
#
#  SLEPC_FOUND        - system has SLEPc
#  SLEPC_INCLUDE_DIR  - include directories for SLEPc
#  SLEPC_LIBARIES     - libraries for SLEPc
#  SLEPC_DIR          - directory where SLEPc is built
#
# Assumes that PETSC_DIR and PETSC_ARCH has been set by
# alredy calling find_package(PETSc)

if(DEFINED SLEPC_FOUND)
  return()
endif(DEFINED SLEPC_FOUND)
  
message(STATUS "Checking for SLEPc library")
find_package(PETSc)
set(SLEPC_FOUND FALSE)

# Standard search path
set(_PATHS "")
if(ROKKO_SOLVER_DIR)
  list(APPEND _PATHS ${ROKKO_SOLVER_DIR})
endif(ROKKO_SOLVER_DIR)
list(APPEND _PATHS ${CMAKE_INSTALL_PREFIX} "/opt/nano/rokko" "/opt/rokko" "/opt" "$ENV{HOME}/opt/rokko" "$ENV{HOME}/opt")

# Set debian_arches (PETSC_ARCH for Debian-style installations)
#foreach (debian_arches linux kfreebsd)
#  if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
#    set(DEBIAN_FLAVORS ${debian_arches}-gnu-c-debug ${debian_arches}-gnu-c-opt ${DEBIAN_FLAVORS})
#  else()
#    set(DEBIAN_FLAVORS ${debian_arches}-gnu-c-opt ${debian_arches}-gnu-c-debug ${DEBIAN_FLAVORS})
#  endif()
#endforeach()

# Try to figure out SLEPC_DIR by finding slepc.h
find_path(SLEPC_DIR include/slepc.h
  PATHS ${_PATHS}
  DOC "SLEPc directory")

# Report result of search for SLEPC_DIR
#if (DEFINED SLEPC_DIR)
#  message(STATUS "SLEPC_DIR is ${SLEPC_DIR}")
#else()
#  message(STATUS "SLEPC_DIR is empty")
#endif()

if(SLEPC_DIR)
  set(SLEPC_INCLUDE_DIR "${SLEPC_DIR}/include")
else(SLEPC_DIR)
  message(STATUS "SLEPc library: not found")
  set(SLEPSC_FOUND FALSE)
  return()
endif(SLEPC_DIR)

find_library(SLEPC_LIBRARIES
  NAMES slepc
  HINTS ${SLEPC_DIR}/lib
  HINTS ${SLEPC_DIR}/${PETSC_ARCH}/lib
  DOC "The SLEPc library"
  )
mark_as_advanced(SLEPC_LIBRARIES)

set(SLEPC_FOUND TRUE)
message(STATUS "SLEPc include directory: ${SLEPC_INCLUDE_DIR}")
message(STATUS "SLEPc libraries: ${SLEPC_LIBRARIES}")

return()

  # Create a temporary Makefile to probe the SLEPcc configuration
  set(slepc_config_makefile ${PROJECT_BINARY_DIR}/Makefile.slepc)
  file(WRITE ${slepc_config_makefile}
"# This file was autogenerated by FindSLEPc.cmake
SLEPC_DIR  = $ENV{SLEPC_DIR}
PETSC_ARCH = $ENV{PETSC_ARCH}
PETSC_DIR = $ENV{PETSC_DIR}
include $ENV{SLEPC_DIR}/conf/slepc_common
show :
	-@echo -n \${\${VARIABLE}}
")

  # Define macro for getting SLEPc variables from Makefile
  macro(SLEPC_GET_VARIABLE var name)
    set(${var} "NOTFOUND" CACHE INTERNAL "Cleared" FORCE)
    execute_process(COMMAND ${CMAKE_MAKE_PROGRAM} --no-print-directory -f ${slepc_config_makefile} show VARIABLE=${name}
      OUTPUT_VARIABLE ${var}
      RESULT_VARIABLE slepc_return)
  endmacro()

  # Call macro to get the SLEPc variables
  slepc_get_variable(SLEPC_INCLUDE SLEPC_INCLUDE)
  slepc_get_variable(SLEPC_EXTERNAL_LIB SLEPC_EXTERNAL_LIB)

  # Remove temporary Makefile
  file(REMOVE ${slepc_config_makefile})

  # Extract include paths and libraries from compile command line
  include(ResolveCompilerPaths)
  resolve_includes(SLEPC_INCLUDE_DIRS "${SLEPC_INCLUDE}")
  resolve_libraries(SLEPC_EXTERNAL_LIB "${SLEPC_EXTERNAL_LIB}")

  # Add variables to CMake cache and mark as advanced
  set(SLEPC_INCLUDE_DIRS ${SLEPC_INCLUDE_DIRS} CACHE STRING "SLEPc include paths." FORCE)
  set(SLEPC_LIBRARIES ${SLEPC_LIBRARY} ${SLEPC_EXTERNAL_LIB} CACHE STRING "SLEPc libraries." FORCE)
  mark_as_advanced(SLEPC_INCLUDE_DIRS SLEPC_LIBRARIES)

  # Set flags for building test program
  set(CMAKE_REQUIRED_INCLUDES ${SLEPC_INCLUDE_DIRS} ${PETSC_INCLUDE_DIRS})
  set(CMAKE_REQUIRED_LIBRARIES ${SLEPC_LIBRARIES} ${PETSC_LIBRARIES})

  # Add MPI variables if MPI has been found
  if (MPI_FOUND)
    set(CMAKE_REQUIRED_INCLUDES  ${CMAKE_REQUIRED_INCLUDES} ${MPI_INCLUDE_PATH})
    set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ${MPI_LIBRARIES})
    set(CMAKE_REQUIRED_FLAGS     "${CMAKE_REQUIRED_FLAGS} ${MPI_COMPILE_FLAGS}")
  endif()

  # Run SLEPc test program
  include(CheckCXXSourceRuns)
  check_cxx_source_runs("
#include \"petsc.h\"
#include \"slepceps.h\"
int main()
{
  PetscErrorCode ierr;
  int argc = 0;
  char** argv = NULL;
  ierr = SlepcInitialize(&argc, &argv, PETSC_NULL, PETSC_NULL);
  EPS eps;
  ierr = EPSCreate(PETSC_COMM_SELF, &eps); CHKERRQ(ierr);
  //ierr = EPSSetFromOptions(eps); CHKERRQ(ierr);
#if PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR <= 1
  ierr = EPSDestroy(eps); CHKERRQ(ierr);
#else
  ierr = EPSDestroy(&eps); CHKERRQ(ierr);
#endif
  ierr = SlepcFinalize(); CHKERRQ(ierr);
  return 0;
}
" SLEPC_TEST_RUNS)

  if (SLEPC_TEST_RUNS)
    message(STATUS "SLEPc test runs")
  else()
    message(STATUS "SLEPc test failed")
  endif()

endif()

# Standard package handling
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SLEPc
  "SLEPc could not be found. Be sure to set SLEPC_DIR, PETSC_DIR, and PETSC_ARCH."
  SLEPC_LIBRARIES SLEPC_DIR SLEPC_INCLUDE_DIRS SLEPC_TEST_RUNS)
