/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2012-2015 by Rokko Developers https://github.com/t-sakashita/rokko
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#include <mpi.h>
#include <rokko/rokko.h>
#include <rokko/utility/frank_matrix_c.h>
#include <stdio.h>
#include <stdlib.h>

#include "gev_fixedB_mpi.h"


int main(int argc, char *argv[]) {
  int dim;
  struct rokko_parallel_dense_solver solver;
  struct rokko_distributed_matrix A, B, eigvec;
  struct rokko_localized_matrix locA, locB;

  struct rokko_grid grid;
  struct rokko_localized_vector eigval;
  char* solver_name;

  int provided, ierr, myrank, nprocs, i;

  ierr = MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  ierr = MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  ierr = MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  if (argc == 1) {
    solver_name = argv[1];
  } else {
    solver_name = "eigen_exa";
  }

  dim = 4;
  printf("solver name = %s\n", solver_name);
  printf("matrix dimension = %d\n", dim);

  rokko_parallel_dense_solver_construct(&solver, solver_name, argc, argv);
  rokko_grid_construct(&grid, MPI_COMM_WORLD, rokko_grid_row_major);

  rokko_localized_matrix_construct(&locA, dim, dim, rokko_matrix_col_major);
  rokko_localized_matrix_construct(&locB, dim, dim, rokko_matrix_col_major);
  set_A_B_c(locA, locB);

  rokko_distributed_matrix_construct(&A, dim, dim, grid, solver, rokko_matrix_col_major);
  rokko_distributed_matrix_construct(&B, dim, dim, grid, solver, rokko_matrix_col_major);
  rokko_distributed_matrix_construct(&eigvec, dim, dim, grid, solver, rokko_matrix_col_major);

  rokko_localized_vector_construct(&eigval, dim);
  rokko_scatter_localized_matrix(locA, A, 0);
  rokko_scatter_localized_matrix(locB, B, 0);

  diagonalize_fixedB_c(solver, A, B, eigval, eigvec, 0);

  if (myrank == 0) {
    printf("Computed Eigenvalues =\n");
    for (i = 0; i < dim; ++i)
      printf("%30.20f\n", rokko_localized_vector_get(eigval, i));
  }
  printf("Computed Eigenvectors:\n");
  rokko_distributed_matrix_print(eigvec);
  
  rokko_distributed_matrix_destruct(&A);
  rokko_distributed_matrix_destruct(&B);
  rokko_distributed_matrix_destruct(&eigvec);
  rokko_localized_vector_destruct(&eigval);
  rokko_parallel_dense_solver_destruct(&solver);
  rokko_grid_destruct(&grid);

  MPI_Finalize();
  return 0;
}
