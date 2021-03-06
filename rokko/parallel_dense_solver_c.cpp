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

#include <rokko/solver.hpp>
#include <rokko/rokko_dense.h>
#include <rokko/parameters.h>

void rokko_parallel_dense_solver_construct(rokko_parallel_dense_solver* solver, const char* solver_name, int argc, char** argv) {
  solver->ptr = new rokko::parallel_dense_solver(std::string(solver_name));
  static_cast<rokko::parallel_dense_solver*>(solver->ptr)->initialize(argc, argv);
}

void rokko_parallel_dense_solver_construct_f(rokko_parallel_dense_solver* solver, const char* solver_name) {
  int argc = 0;
  char** argv;
  rokko_parallel_dense_solver_construct(solver, solver_name, argc, argv);
}

void rokko_parallel_dense_solver_destruct(rokko_parallel_dense_solver* solver) {
  rokko::parallel_dense_solver* ptr = static_cast<rokko::parallel_dense_solver*>(solver->ptr);
  ptr->finalize();
  delete ptr;
}

struct rokko_parameters rokko_parallel_dense_solver_diagonalize_distributed_matrix(struct rokko_parallel_dense_solver* solver,
  struct rokko_distributed_matrix* mat, struct rokko_localized_vector* eigvals,
  struct rokko_distributed_matrix* eigvecs) {
  struct rokko_parameters params_out;
  rokko_parameters_construct(&params_out);

  if (mat->major == rokko_matrix_col_major)
    *static_cast<rokko::parameters*>(params_out.ptr) = static_cast<rokko::parallel_dense_solver*>(solver->ptr)->diagonalize(
      *static_cast<rokko::distributed_matrix<double, rokko::matrix_col_major>*>(mat->ptr),
      *static_cast<rokko::localized_vector<double>*>(eigvals->ptr),
      *static_cast<rokko::distributed_matrix<double, rokko::matrix_col_major>*>(eigvecs->ptr));
  else
    *static_cast<rokko::parameters*>(params_out.ptr) = static_cast<rokko::parallel_dense_solver*>(solver->ptr)->diagonalize(
      *static_cast<rokko::distributed_matrix<double, rokko::matrix_row_major>*>(mat->ptr),
      *static_cast<rokko::localized_vector<double>*>(eigvals->ptr),
      *static_cast<rokko::distributed_matrix<double, rokko::matrix_row_major>*>(eigvecs->ptr));
  return params_out;
}
