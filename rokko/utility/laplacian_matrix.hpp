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

#ifndef ROKKO_UTILITY_LAPLACIAN_MATRIX_HPP
#define ROKKO_UTILITY_LAPLACIAN_MATRIX_HPP

#include <cmath>
#include <stdexcept>
#include <boost/throw_exception.hpp>
#include <rokko/localized_matrix.hpp>

namespace rokko {

class laplacian_matrix {
public:
  template<typename T>
  static void multiply(int dim, const T* x, T* y) {
    y[0] = x[0] - x[1];
    y[dim-1] = 2 * x[dim-1] - x[dim - 2];
    for (int k = 1; k < (dim-1); ++k) { // from 1 to end-1
      y[k] = - x[k-1] + 2 * x[k] - x[k+1];
    }
  }

  template<typename T>
  static void multiply(int dim, const std::vector<T>& v, std::vector<T>& w) {
    multiply(dim, &v[0], &w[0]);
  }

  template<typename T, typename MATRIX_MAJOR>
  static void generate(rokko::localized_matrix<T, MATRIX_MAJOR>& mat) {
    if (mat.rows() != mat.cols())
      BOOST_THROW_EXCEPTION(std::invalid_argument("laplacian_matrix::generate() : non-square matrix"));
    mat.setZero();
    int n = mat.rows();
    mat(0, 0) = 1; mat(0, 1) = -1;
    mat(n-1, n-2) = -1;  mat(n-1, n-1) = 2;
    for(int i = 1; i < n-1; ++i) {
      mat(i, i-1) = -1;
      mat(i, i) = 2;
      mat(i, i+1) = -1;
    }
  }
  
  // calculate k-th smallest eigenvalue of dim-dimensional Laplacian matrix (k=0...dim-1)
  static double eigenvalue(int dim, int k) {
    return 2 * (1 - std::cos(M_PI * (2 * k + 1) / (2 * dim + 1)));
  }
};
    
} // namespace rokko

#endif // ROKKO_UTILITY_LAPLACIAN_MATRIX_HPP
