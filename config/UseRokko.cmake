#
# This module is provided as ROKKO_USE_FILE by RokkoConfig.cmake.  It can
# be included in a project to load the needed compiler and linker
# settings to use Rokko.
#

if(NOT ROKKO_USE_FILE_INCLUDED)
  set(ROKKO_USE_FILE_INCLUDED 1)
  list(APPEND CMAKE_MODULE_PATH ${ROKKO_ROOT_DIR}/share/rokko)

  # OpenMP
  find_package(OpenMP)
  if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} ${OpenMP_C_FLAGS}")
  endif(OPENMP_FOUND)

  # MPI library
  find_package(MPI)
  set(CMAKE_EXE_LINKER_FLAGS ${MPI_CXX_LINK_FLAGS})

  # Add include directories needed to use ROKKO and dependent libraries
  include_directories(${ROKKO_INCLUDE_DIR} ${Rokko_Boost_INCLUDE_DIR} ${ROKKO_EIGENEXA_INCLUDE_DIR} ${ROKKO_ELEMENTAL_INCLUDE_DIR} ${ROKKO_ANASAZI_INCLUDE_DIR} ${ROKKO_PETSC_INCLUDE_DIR} ${ROKKO_SLEPC_INCLUDE_DIR})

  # Add link directories needed to use ROKKO and dependent libraries
  link_directories(${ROKKO_ROOT_DIR}/lib)

  # test macro
  include(${ROKKO_ROOT_DIR}/share/rokko/add_rokko_test.cmake)
endif(NOT ROKKO_USE_FILE_INCLUDED)