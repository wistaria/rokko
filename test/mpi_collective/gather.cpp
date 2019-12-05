/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2013-2019 Rokko Developers https://github.com/t-sakashita/rokko
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#include <rokko/solver.hpp>
#include <rokko/collective.hpp>
#include <rokko/utility/frank_matrix.hpp>

#include <random>

#include <gtest/gtest.h>

constexpr double eps = 1e-11;

int global_argc;
char** global_argv;

template<typename GRID_MAJOR, typename DIST_MAT_MAJOR, typename LOC_MAT_MAJOR>
void run_test(MPI_Comm comm, int dim, GRID_MAJOR const& grid_major, DIST_MAT_MAJOR const&, LOC_MAT_MAJOR const&) {
  int size, rank;
  MPI_Comm_size(comm, &size);
  MPI_Comm_rank(comm, &rank);

  // same seed for all processes
  std::mt19937 engine(123lu);
  std::uniform_real_distribution<> dist(-1.0, 1.0);
  
  rokko::parallel_dense_ev solver(rokko::parallel_dense_ev::default_solver());
  rokko::grid g(comm, grid_major);
  rokko::mapping_bc<DIST_MAT_MAJOR> map = solver.default_mapping(dim, g);
  rokko::distributed_matrix<double, DIST_MAT_MAJOR> mat(map);
  for (int i = 0; i < dim; ++i) {
    for (int j = 0; j < dim; ++j) {
      double d = dist(engine);
      if (mat.is_gindex(i, j)) mat.set_global(i, j, d);
    }
  }
#ifndef NDEBUG
  mat.print();
#endif
  
  Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,rokko::eigen3_major<LOC_MAT_MAJOR>> lmat(dim, dim);
  for (int r = 0; r < size; ++r) {
    rokko::gather(mat, lmat, r);
#ifndef NDEBUG
    if (rank == r) { 
      std::cout << lmat << std::endl;
      std::cout.flush();
    }
    MPI_Barrier(comm);
#endif
  }
  for (int i = 0; i < dim; ++i) {
    for (int j = 0; j < dim; ++j) {
      if (mat.is_gindex(i, j))
        EXPECT_NEAR(mat.get_global(i, j), lmat(i, j), eps);
    }
  }
}

TEST(mpi_communication, gather) {
  MPI_Comm comm = MPI_COMM_WORLD;
  int rank;
  MPI_Comm_rank(comm, &rank);

  int dim = 100;
  if (global_argc > 1) {
    dim = boost::lexical_cast<int>(global_argv[1]);
  }
  if (rank == 0) std::cout << "dimension = " << dim << std::endl;

  if (rank == 0) std::cout << "test for grid = col, dist = col, loc = col\n";
  run_test(comm, dim, rokko::grid_col_major, rokko::matrix_col_major(), rokko::matrix_col_major());

  /*
  if (rank == 0) std::cout << "test for grid = col, dist = col, loc = row\n";
  run_test(comm, dim, rokko::grid_col_major, rokko::matrix_col_major(), rokko::matrix_row_major());

  if (rank == 0) std::cout << "test for grid = col, dist = row, loc = col\n";
  run_test(comm, dim, rokko::grid_col_major, rokko::matrix_row_major(), rokko::matrix_col_major());

  if (rank == 0) std::cout << "test for grid = col, dist = row, loc = row\n";
  run_test(comm, dim, rokko::grid_col_major, rokko::matrix_row_major(), rokko::matrix_row_major());
  */

  if (rank == 0) std::cout << "test for grid = row, dist = col, loc = col\n";
  run_test(comm, dim, rokko::grid_row_major, rokko::matrix_col_major(), rokko::matrix_col_major());

  /*
  if (rank == 0) std::cout << "test for grid = row, dist = col, loc = row\n";
  run_test(comm, dim, rokko::grid_row_major, rokko::matrix_col_major(), rokko::matrix_row_major());

  if (rank == 0) std::cout << "test for grid = row, dist = row, loc = col\n";
  run_test(comm, dim, rokko::grid_row_major, rokko::matrix_row_major(), rokko::matrix_col_major());

  if (rank == 0) std::cout << "test for grid = row, dist = row, loc = row\n";
  run_test(comm, dim, rokko::grid_row_major, rokko::matrix_row_major(), rokko::matrix_row_major());
  */
}

int main(int argc, char** argv) {
  int result = 0;
  ::testing::InitGoogleTest(&argc, argv);
  MPI_Init(&argc, &argv);
  global_argc = argc;
  global_argv = argv;
  result = RUN_ALL_TESTS();
  MPI_Finalize();
  return result;
}
