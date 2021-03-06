/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2013-2015 Rokko Developers https://github.com/t-sakashita/rokko
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

#include <rokko/grid.hpp>
#include <rokko/solver.hpp>
#include <rokko/distributed_matrix.hpp>
#include <rokko/localized_matrix.hpp>

#include <rokko/utility/frank_matrix.hpp>

#define BOOST_TEST_MODULE test_distributed_matrix
#ifndef BOOST_TEST_DYN_LINK
#include <boost/test/included/unit_test.hpp>
#else
#include <boost/test/unit_test.hpp>
#endif

template<typename T, typename MATRIX_MAJOR>
void localized_2_distributed(const rokko::localized_matrix<T, MATRIX_MAJOR>& lmat, rokko::distributed_matrix<MATRIX_MAJOR>& mat) {
  if (mat.get_m_global() != mat.get_n_global())
    BOOST_THROW_EXCEPTION(std::invalid_argument("frank_matrix::generate() : non-square matrix"));
  for(int local_i = 0; local_i < mat.get_m_local(); ++local_i) {
    for(int local_j = 0; local_j < mat.get_n_local(); ++local_j) {
      int global_i = mat.translate_l2g_row(local_i);
      int global_j = mat.translate_l2g_col(local_j);
      mat.set_local(local_i, local_j, lmat(global_i, global_j));
    }
  }  
}

BOOST_AUTO_TEST_CASE(test_distributed_matrix) {
  int argc = boost::unit_test::framework::master_test_suite().argc;
  char** argv = boost::unit_test::framework::master_test_suite().argv;
  unsigned int dim = 10;
  int provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  MPI_Comm comm = MPI_COMM_WORLD;
  rokko::grid g(comm);
  BOOST_FOREACH(std::string name, rokko::parallel_dense_solver::solvers()) {
    rokko::parallel_dense_solver solver(name);
    solver.initialize(argc, argv);
    rokko::distributed_matrix<rokko::matrix_col_major> mat(dim, dim, g, solver);
    rokko::localized_matrix<double, rokko::matrix_col_major> lmat(dim, dim);
    rokko::frank_matrix::generate(lmat);
    localized_2_distributed(lmat, mat);
    rokko::frank_matrix::generate(mat);
    mat.print();
    //BOOST_CHECK_CLOSE(mat.get_global(0, 0), dim, 1e-14);
    solver.finalize();
  }
  MPI_Finalize();
}
