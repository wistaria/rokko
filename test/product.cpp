/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2013 by Synge Todo <wistaria@comp-phys.org>
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#include <rokko/solver.hpp>
#include <rokko/pblas/pblas.hpp>
#include <rokko/utility/frank_matrix.hpp>

#define BOOST_TEST_MODULE test_product
#ifndef BOOST_TEST_DYN_LINK
#include <boost/test/included/unit_test.hpp>
#else
#include <boost/test/unit_test.hpp>
#endif

BOOST_AUTO_TEST_CASE(test_product) {
  MPI_Init(&boost::unit_test::framework::master_test_suite().argc,
             &boost::unit_test::framework::master_test_suite().argv);
  MPI_Comm comm = MPI_COMM_WORLD;
  int rank;
  MPI_Comm_rank(comm, &rank);

  int dim = 100;
  if (boost::unit_test::framework::master_test_suite().argc > 1) {
    dim = boost::lexical_cast<int>(boost::unit_test::framework::master_test_suite().argv[1]);
  }

  if (rank == 0) std::cout << "dimension = " << dim << std::endl;
  rokko::solver solver(rokko::solver_factory::instance()->solver_names()[0]);
  rokko::grid g(comm, rokko::grid_col_major);
  rokko::distributed_matrix<rokko::matrix_col_major> matA(dim, dim, g, solver);
  rokko::distributed_matrix<rokko::matrix_col_major> matB(dim, dim, g, solver);
  rokko::distributed_matrix<rokko::matrix_col_major> matC(dim, dim, g, solver);
  rokko::frank_matrix::generate(matA);
  rokko::frank_matrix::generate(matB);
  rokko::product(1, matA, false, matB, false, 0, matC);
  matC.print();
  // calculate trace
  double sum_local;
  for (int i = 0; i < dim; ++i) {
    if (matC.is_gindex(i, i)) sum_local += matC.get_global(i, i);
  }
  double sum_global;
  MPI_Allreduce(&sum_local, &sum_global, 1, MPI_DOUBLE, MPI_SUM, comm);
  if (rank == 0) std::cout << "trace of distributed matrix = " << sum_global << std::endl;

  rokko::localized_matrix<rokko::matrix_col_major> lmatA(dim, dim);
  rokko::frank_matrix::generate(lmatA);
  rokko::localized_matrix<rokko::matrix_col_major> lmatC = lmatA * lmatA;
  if (rank == 0) std::cout << lmatC << std::endl;
  // calculate trace
  double sum;
  for (int i = 0; i < dim; ++i) {
    sum += lmatC(i, i);
  }
  if (rank == 0) std::cout << "trace of localized matrix = " << sum << std::endl;

  if (rank == 0) BOOST_CHECK_CLOSE(sum_global, sum, 10e-12);

  MPI_Finalize();
}
