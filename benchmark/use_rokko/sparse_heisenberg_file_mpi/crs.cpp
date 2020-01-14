/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2012-2015 Rokko Developers https://github.com/t-sakashita/rokko
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#include <rokko/rokko.hpp>
#include <rokko/utility/lattice.hpp>

int main(int argc, char *argv[]) {
  int provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  double init_tick, initend_tick, gen_tick, diag_tick, end_tick;

  std::string name("anasazi");
  if (argc >= 2) name = argv[1];
  std::string lattice_file("xyz.dat");
  if (argc >= 3) lattice_file = argv[2];
  int L;
  std::vector<std::pair<int, int>> lattice;
  rokko::read_lattice_file(lattice_file, L, lattice);
  int dim = 1 << L;
  if (rank == 0)
    std::cout << "Eigenvalue decomposition of antiferromagnetic Heisenberg chain" << std::endl
              << "solver = " << name << std::endl
              << "L = " << L << std::endl
              << "dimension = " << dim << std::endl;

  MPI_Barrier(MPI_COMM_WORLD);
  init_tick = MPI_Wtime();
  rokko::parallel_sparse_ev solver(name);
  MPI_Barrier(MPI_COMM_WORLD);
  initend_tick = MPI_Wtime();

  MPI_Barrier(MPI_COMM_WORLD);
  gen_tick = MPI_Wtime();
  rokko::distributed_crs_matrix mat({dim, dim}, solver);
  std::vector<double> values;
  std::vector<int> cols;
  for (int row = mat.start_row(); row <= mat.end_row(); ++row) {
    cols.clear();
    values.clear();
    double diag = 0;
    for (int l = 0;  l < lattice.size(); ++l) {
      int i = lattice[l].first;
      int j = lattice[l].second;
      int m1 = 1 << i;
      int m2 = 1 << j;
      int m3 = m1 + m2;
      if (((row & m3) == m1) || ((row & m3) == m2)) {
        cols.push_back(row^m3);
        values.push_back(0.5);
        diag += -0.25;
      } else {
        diag += 0.25;
      }
    }
    if (diag != 0.) {
      cols.push_back(row);
      values.push_back(diag);
    }
    mat.insert(row, cols, values);
  }
  mat.complete();
  
  MPI_Barrier(MPI_COMM_WORLD);
  diag_tick = MPI_Wtime();
  rokko::parameters params;
  //params.set("max_block_size", 5);
  //params.set("max_iters", 500);
  //params.set("conv_tol", 1.0e-8);
  //params.set("num_eigvals", 1);
  rokko::parameters info = solver.diagonalize(mat, params);
  MPI_Barrier(MPI_COMM_WORLD);
  end_tick = MPI_Wtime();
  
  int num_conv = info.get<int>("num_conv");
  if (num_conv == 0) {
    throw std::runtime_error("diagonalize : solver does not converge.");
  }
  std::vector<double> eigvec;
  solver.eigenvector(0, eigvec);
  if (rank == 0) {
    std::cout << "number of converged eigenpairs = " << num_conv << std::endl;
    std::cout << "smallest eigenvalues: ";
    for (int i = 0; i < num_conv; ++i) std::cout << ' ' << solver.eigenvalue(i);
    std::cout << std::endl;
    std::cout << "init_time = " << initend_tick - init_tick << std::endl
              << "gen_time = " << diag_tick - gen_tick << std::endl
              << "diag_time = " << end_tick - diag_tick << std::endl;
  }

  solver.finalize();
  MPI_Finalize();
}
