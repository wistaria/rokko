# - Try to find Scalapack
# Once done this will define
#
#  SCALAPACK_FOUND        - system has ScaLAPACK
#  SCALAPACK_LIBRARIES     - libraries for ScaLAPACK

if(DEFINED SCALAPACK_FOUND)
  return()
endif(DEFINED SCALAPACK_FOUND)
  
message(STATUS "Checking for ScaLAPACK library")

if(DEFINED SCALAPACK_LIB)
  set(SCALAPACK_FOUND TRUE)
  set(SCALAPACK_LIBRARIES ${SCALAPACK_LIB})
  message(STATUS "ScaLAPACK libraries: ${SCALAPACK_LIBRARIES}")
  return()
endif(DEFINED SCALAPACK_LIB)

if(DEFINED MKL_INCLUDE_DIR)

  find_library(_SCALAPACK_LIBRARY
    NAMES mkl_scalapack_lp64
    PATHS $ENV{MKLROOT}/lib/intel64 $ENV{MKLROOT}/lib/em64t
    DOC "The ScaLAPACK library")
  if(_SCALAPACK_LIBRARY)
    list(APPEND _SCALAPACK_LIBRARIES ${_SCALAPACK_LIBRARY})
  else(_SCALAPACK_LIBRARY)
    message(STATUS "ScaLAPACK library: not found")
    set(SCALAPACK_FOUND FALSE)
    return()
  endif(_SCALAPACK_LIBRARY)

  # Check whether SGI MPT is used
  try_compile(_SGI_MPT
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/config/check_sgimpt.c
    OUTPUT_VARIABLE LOG)
  if(_SGI_MPT)
    find_library(_SCALAPACK_BLACS_LIBRARY
      NAMES mkl_blacs_sgimpt_lp64
      PATHS $ENV{MKLROOT}/lib/intel64 $ENV{MKLROOT}/lib/em64t
      DOC "The BLACS library")
    MESSAGE(STASTUS "SGI MPT is used")
  else(_SGI_MPT)
    try_compile(_OPENMPI
      ${CMAKE_CURRENT_BINARY_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/config/check_openmpi.c
      OUTPUT_VARIABLE LOG)
    if(_OPENMPI)
      find_library(_SCALAPACK_BLACS_LIBRARY
        NAMES mkl_blacs_openmpi_lp64
        PATHS $ENV{MKLROOT}/lib/intel64 $ENV{MKLROOT}/lib/em64t
        DOC "The BLACS library")
      MESSAGE(STATUS "OpenMPI is used")
    else(_OPENMPI)
      find_library(_SCALAPACK_BLACS_LIBRARY
        NAMES mkl_blacs_intelmpi_lp64
        PATHS $ENV{MKLROOT}/lib/intel64 $ENV{MKLROOT}/lib/em64t
        DOC "The BLACS library")
      MESSAGE(STATUS "Intel MPI/MPICH2/MVAPICH is used")
    endif(_OPENMPI)
  endif(_SGI_MPT)
  if(_SCALAPACK_BLACS_LIBRARY)
    list(APPEND _SCALAPACK_LIBRARIES ${_SCALAPACK_BLACS_LIBRARY})
  endif(_SCALAPACK_BLACS_LIBRARY)

else(DEFINED MKL_INCLUDE_DIR)

  # Standard search path
  set(_PATHS "")
  if(SCALAPACK_DIR)
    set(_PATHS ${SCALAPACK_DIR})
  else(SCALAPACK_DIR)
    list(APPEND _PATHS
  	 ${SCALAPACK_ROOT}/${CMAKE_BUILD_TYPE}
	 ${SCALAPACK_ROOT}
  	 $ENV{SCALAPACK_ROOT}/${CMAKE_BUILD_TYPE}
	 $ENV{SCALAPACK_ROOT}
  	 ${ROKKO_SOLVER_ROOT}/scalapack/${CMAKE_BUILD_TYPE}
	 ${ROKKO_SOLVER_ROOT}/scalapack
  	 $ENV{ROKKO_SOLVER_ROOT}/scalapack/${CMAKE_BUILD_TYPE}
	 $ENV{ROKKO_SOLVER_ROOT}/scalapack
	 ${CMAKE_INSTALL_PREFIX}/scalapack/${CMAKE_BUILD_TYPE}
	 ${CMAKE_INSTALL_PREFIX}/${CMAKE_BUILD_TYPE}
	 $ENV{HOME}/rokko/scalapack/${CMAKE_BUILD_TYPE}
	 $ENV{HOME}/rokko/scalapack
	 /opt/rokko/scalapack/${CMAKE_BUILD_TYPE}
	 /opt/rokko/scalapack
	 /opt/rokko/${CMAKE_BUILD_TYPE}
	 /opt/rokko
	 /opt/local /opt
	 )
    list(APPEND _PATHS /usr/lib64/openmpi) # for CentOS
  endif(SCALAPACK_DIR)

  foreach (_PATH ${_PATHS})
    list(APPEND _LIBPATHS "${_PATH}/lib")
  endforeach()

  find_library(_SCALAPACK_LIBRARY
    NAMES scalapack scalapack-openmpi scalapack-mpich
    PATHS ${_LIBPATHS}
    DOC "The ScaLAPACK library")
  if(_SCALAPACK_LIBRARY)
    list(APPEND _SCALAPACK_LIBRARIES ${_SCALAPACK_LIBRARY})
  else(_SCALAPACK_LIBRARY)
    message(STATUS "ScaLAPACK library: not found")
    set(SCALAPACK_FOUND FALSE)
    return()
  endif(_SCALAPACK_LIBRARY)

endif(DEFINED MKL_INCLUDE_DIR)

set(SCALAPACK_FOUND TRUE)
set(SCALAPACK_LIBRARIES ${_SCALAPACK_LIBRARIES})
message(STATUS "ScaLAPACK libraries: ${SCALAPACK_LIBRARIES}")
