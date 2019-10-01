/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2014-2019 Rokko Developers https://github.com/t-sakashita/rokko
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#include <rokko/utility/sort_eigenpairs.hpp>

#include <numeric>
#include <algorithm>

#include <gtest/gtest.h>

#define make_test(major, ascending) \
  int num = 10;\
  std::vector<int> index(num);                  \
  std::iota(index.begin(), index.end(), 0);\
  std::random_shuffle(index.begin(), index.end());\
  rokko::localized_vector<double> eigvals(num), eigvals_sorted(num);   \
  rokko::localized_matrix<double, major > eigvecs(num, num), eigvecs_sorted(num, num); \
  for(int i=0; i<num; ++i){\
    eigvals(i) = 1.0*index[i];\
    for(int j=0; j<num; ++j){\
      if(eigvecs.is_row_major()){\
        eigvecs(i,j) = eigvals(i);\
      }else{\
        eigvecs(j,i) = eigvals(i);\
      }\
    }\
  }\
  rokko::sort_eigenpairs(eigvals, eigvecs, eigvals_sorted, eigvecs_sorted, ascending);\
  for(int i=0; i<num; ++i){\
    std::cout << "dim: " << i << std::endl;\
    double e = ascending ? 1.0 * i : 1.0*(num-i-1);\
    if(eigvecs.is_row_major()){\
      ASSERT_EQ( eigvals_sorted(i), e);\
      ASSERT_EQ( eigvecs_sorted(i,0), e);\
      ASSERT_EQ( eigvecs_sorted(i,1), e);\
    }else{\
      ASSERT_EQ( eigvals_sorted(i), e);\
      ASSERT_EQ( eigvecs_sorted(0,i), e);\
      ASSERT_EQ( eigvecs_sorted(1,i), e);\
    }\
  }

TEST(sort_eigenpairs, matrix_majors) {
  {
    std::cout << "row_major, ascending order\n";
    make_test(rokko::matrix_row_major, true)
  }
  {
    std::cout << "row_major, descending order\n";
    make_test(rokko::matrix_row_major, false)
  }
  {
    std::cout << "col_major, ascending order\n";
    make_test(rokko::matrix_col_major, true)
  }
  {
    std::cout << "col_major, descending order\n";
    make_test(rokko::matrix_col_major, false)
  }
}

#undef make_test

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
