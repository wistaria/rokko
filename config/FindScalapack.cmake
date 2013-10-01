# - Try to find Scalapack
#
# Once done this will define
#
#  SCALAPACK_FOUND        - system has Scalapack
#  SCALAPACK_LIBARIES     - libraries for Scalapack

if(DEFINED SCALAPACK_FOUND)
  return()
endif(DEFINED SCALAPACK_FOUND)
  
message(STATUS "Checking for Scalapack library")

if(DEFINED SCALAPACK_LIB)
  set(SCALAPACK_FOUND TRUE)
  set(SCALAPACK_LIBRARIES ${SCALAPACK_LIB})
  message(STATUS "Scalapack libraries: ${SCALAPACK_LIBRARIES}")
  return()
endif(DEFINED SCALAPACK_LIB)

if(DEFINED MKL_INCLUDE_DIR)

  find_library(_SCALAPACK_LIBRARY
    NAMES mkl_scalapack_lp64
    PATHS $ENV{MKLROOT}/lib/intel64
    DOC "The Scalapack library")
  if(_SCALAPACK_LIBRARY)
    list(APPEND SCALAPACK_LIBRARIES ${_SCALAPACK_LIBRARY})
  else(_SCALAPACK_LIBRARY)
    message(STATUS "Scalapack library: not found")
    set(SCALAPACK_FOUND FALSE)
    return()
  endif(_SCALAPACK_LIBRARY)
  find_library(_SCALAPACK_BLACS_LIBRARY
    NAMES mkl_blacs_intelmpi_lp64
    PATHS $ENV{MKLROOT}/lib/intel64
    DOC "The BLACS library")
  if(_SCALAPACK_BLACS_LIBRARY)
    list(APPEND SCALAPACK_LIBRARIES ${_SCALAPACK_BLACS_LIBRARY})
  endif(_SCALAPACK_BLACS_LIBRARY)

else(DEFINED MKL_INCLUDE_DIR)

  # Standard search path
  set(_PATHS "")
  if(ROKKO_SOLVER_DIR)
    list(APPEND _PATHS ${ROKKO_SOLVER_DIR})
  endif(ROKKO_SOLVER_DIR)
  list(APPEND _PATHS ${CMAKE_INSTALL_PREFIX} "/opt/nano/rokko" "/opt/rokko" "/opt" "$ENV{HOME}/opt/rokko" "$ENV{HOME}/opt")

  foreach (_PATH ${_PATHS})
    list(APPEND _LIBPATHS "${_PATH}/lib")
  endforeach()

  find_library(_SCALAPACK_LIBRARY
    NAMES scalapack
    PATHS ${_LIBPATHS}
    DOC "The Scalapack library")
  if(_SCALAPACK_LIBRARY)
    list(APPEND SCALAPACK_LIBRARIES ${_SCALAPACK_LIBRARY})
  else(_SCALAPACK_LIBRARY)
    message(STATUS "Scalapack library: not found")
    set(SCALAPACK_FOUND FALSE)
    return()
  endif(_SCALAPACK_LIBRARY)

endif(DEFINED MKL_INCLUDE_DIR)

set(SCALAPACK_FOUND TRUE)
message(STATUS "Scalapack libraries: ${SCALAPACK_LIBRARIES}")