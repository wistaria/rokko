/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2014 by Synge Todo <wistaria@comp-phys.org>
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#include <mpi.h>
#include <iostream>

#include <rokko/grid_1d.hpp>
#include <rokko/parallel_sparse_solver.hpp>
#include <rokko/distributed_crs_matrix.hpp>
#include <rokko/utility/laplacian_matrix.hpp>
#include <rokko/parameters.hpp>

int main(int argc, char *argv[]) {
  int provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  MPI_Comm comm = MPI_COMM_WORLD;

  rokko::grid_1d g(comm);
  int myrank = g.get_myrank();
  int root = 0;

  std::cout.precision(5);
  int nev = 5;
  int block_size = 2;
  int max_iters = 500;
  double tol = 1.0e-6;

  int dim = 10;

  rokko::parallel_sparse_solver solver("slepc");
  if (myrank == root)
    std::cout << "Eigenvalue decomposition of tridiagonal Laplacian matrix" << std::endl
              << "solver = LOBPCG" << std::endl
              << "dimension = " << dim << std::endl;

  rokko::distributed_crs_matrix mat(dim, dim, solver);


  
  std::vector<double> values;
  std::vector<int> cols;

  cols.clear();
  values.clear();
  int start_row = mat.start_row();
  if (start_row == 0) {
    values.push_back(1.);  values.push_back(-1.);
    cols.push_back(0);  cols.push_back(1);
    ++start_row;
  }
  mat.insert(0, cols, values);

  int end_row = mat.end_row();
  if (end_row == (dim-1)) {
    --end_row;
  }
  
  values.clear();
  values.push_back(-1.);  values.push_back(2.);  values.push_back(-1.);
  for (int row = start_row; row <= end_row; ++row) {
    cols.clear();
    cols.push_back(row-1);  cols.push_back(row);  cols.push_back(row+1);
    mat.insert(row, cols, values);
  }

  cols.clear();
  values.clear();
  if (mat.end_row() == (dim-1)) {
    values.push_back(-1.);  values.push_back(2.);
    cols.push_back(dim-2);  cols.push_back(dim-1);
  }
  mat.insert(dim-1, cols, values);

  mat.complete();
  mat.print();

  rokko::parameters params;
  params.set("Which", "LM");
  params.set("Block Size", block_size);
  params.set("Maximum Iterations", max_iters);
  params.set("Convergence Tolerance", tol);
  params.set("num_eigenvalues", nev);
  params.set("routine", "lanczos");
  //params.set("routine", "SimpleLOBPCG");
  //params.set("routine", "BlockDavidson");

  solver.diagonalize(mat, params);

  std::vector<double> eigvec;
  if (myrank == root) {
    std::cout << "number of converged eigenpairs=" << solver.num_conv() << std::endl;
    std::cout << "smallest eigenvalues:";
    for (int i = 0; i < solver.num_conv(); ++i)
      std::cout << ' ' << solver.eigenvalue(i);
    std::cout << std::endl;
  }

  if (myrank == root) {
    std::cout << "theoretical eigenvalues=";
    for (int k = 0; k < dim; ++k) {
      std::cout << rokko::laplacian_matrix::eigenvalue(dim, k) << " ";
    }
    std::cout << std::endl;
  }

  solver.eigenvector(0, eigvec);

  if (myrank == root) {
    std::cout << "corresponding eigenvectors:";
    for (int j=0; j<eigvec.size(); ++j)
      std::cout << ' ' << eigvec[j];
    std::cout << std::endl;
  }
  //}

  MPI_Finalize();
}
