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

#include <rokko/rokko.hpp>
#include <rokko/utility/frank_matrix.hpp>

#include <gtest/gtest.h>

template<typename T, typename MATRIX_MAJOR>
void test(int dim) {
  rokko::localized_matrix<T, MATRIX_MAJOR> mat(dim, dim);
  rokko::frank_matrix::generate(mat);  
  EXPECT_NEAR(mat.trace(), dim * (dim+1) * 0.5, 10e-5);
}

TEST(matrix_generator, frank_matrix) {
  const int dim = 100;
  std::cout << "dimension = " << dim << std::endl;

  std::cout << "  test for row major" << std::endl;
  test<double, rokko::matrix_row_major>(dim);
  std::cout << "  test for column major" << std::endl;
  test<double, rokko::matrix_col_major>(dim);
}

int main(int argc, char** argv) {
  int result = 0;
  ::testing::InitGoogleTest(&argc, argv);
  result = RUN_ALL_TESTS();
  return result;
}
