/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2012-2019 by Synge Todo <wistaria@phys.s.u-tokyo.ac.jp>
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#include <mpi.h>
#include <cmath>
#include <iostream>
#include <rokko/distributed_matrix.hpp>
#include <rokko/localized_vector.hpp>
#include <rokko/scalapack.hpp>

int main(int argc, char** argv) {
  int provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  rokko::grid grid(MPI_COMM_WORLD);

  int n = 8;
  if (argc > 1) n = std::atoi(argv[1]);

  if (grid.get_myrank() == 0) {
    std::cout << "n = " << n << std::endl;
    std::cout << "nprocs = " << grid.get_nprocs() << std::endl;
    std::cout << "nprow = " << grid.get_nprow() << std::endl;
    std::cout << "npcol = " << grid.get_npcol() << std::endl;
    if ((grid.get_nprocs() != grid.get_nprow() * grid.get_npcol()) ||
        (n % grid.get_nprow() != 0) || (n % grid.get_npcol() != 0)) {
      std::cerr << "incompatible matrix size and number of processes\n";
      MPI_Abort(MPI_COMM_WORLD, 127);
    }
  }

  rokko::mapping_bc<rokko::matrix_col_major> map(n, 1);
  rokko::distributed_matrix<double> a(map);
  rokko::distributed_matrix<double> z(map);
  rokko::localized_vector<double> w(n);
  for (int j = 0; j < n; ++j)
    for (int i = 0; i < n; ++i)
      a.set_global(i, j, n - std::max(i, j));
  
  a.print();
  // // info = rokko::scalapack::psyev('V', 'U', a, desc, w, z, desc);
  // info = cscalapack_pdsyev('V', 'U', n, storage(a), 0, 0, desc, storage(w), storage(z), 0, 0, desc);
  // if (info) {
  //   std::cerr << "Error in cscalapack_pdsyev\n";
  //   MPI_Abort(MPI_COMM_WORLD, info);
  // }
  // if (myrank == 0)
  //   std::cout << "eigenvalues: " << w.transpose() << std::endl;

  // rokko::blacs::gridexit(context);
  MPI_Finalize();
}
