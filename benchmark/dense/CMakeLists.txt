#
# Rokko: Integrated Interface for libraries of eigenvalue decomposition
#
# Copyright (C) 2012-2015 by Rokko Developers https://github.com/t-sakashita/rokko
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

if(BUILD_PARALLEL_DENSE_SOLVER)
  set(BENCHMARKS frank_mpi heisenberg_dense_mpi)
  foreach(name ${BENCHMARKS})
    set(target_name benchmark_${name})
    add_executable(${target_name} ${name}.cpp)
    set_target_properties(${target_name} PROPERTIES OUTPUT_NAME ${name})
    target_link_libraries(${target_name} rokko ${ELEMENTAL_LIBRARIES} ${SCALAPACK_LIB}
      ${MPI_CXX_LIBRARIES})
  endforeach(name)
endif(BUILD_PARALLEL_DENSE_SOLVER)
