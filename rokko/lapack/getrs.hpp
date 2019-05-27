/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2017 by Rokko Developers https://github.com/t-sakashita/rokko
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#ifndef ROKKO_LAPACK_GETRS_HPP
#define ROKKO_LAPACK_GETRS_HPP

#include <complex>
#include <stdexcept>
#include <lapacke.h>
#undef I
#include "complex_cast.hpp"

namespace rokko {
namespace lapack {

namespace {

template<typename T> struct getrs_dispatch;
  
template<>
struct getrs_dispatch<float> {
  template<typename MATRIX0, typename MATRIX1, typename VECTOR>
  static lapack_int getrs(int matrix_layout, char trans, lapack_int n, lapack_int nrhs,
                          MATRIX0& a, VECTOR& ipiv, MATRIX1& b) {
    return LAPACKE_sgetrs(matrix_layout, trans, n, nrhs, storage(a), lda(a), storage(ipiv),
                          storage(b), lda(b));
  }
};

template<>
struct getrs_dispatch<double> {
  template<typename MATRIX0, typename MATRIX1, typename VECTOR>
  static lapack_int getrs(int matrix_layout, char trans, lapack_int n, lapack_int nrhs,
                          MATRIX0& a, VECTOR& ipiv, MATRIX1& b) {
    return LAPACKE_dgetrs(matrix_layout, trans, n, nrhs, storage(a), lda(a), storage(ipiv),
                          storage(b), lda(b));
  }
};

template<>
struct getrs_dispatch<std::complex<float> > {
  template<typename MATRIX0, typename MATRIX1, typename VECTOR>
  static lapack_int getrs(int matrix_layout, char trans, lapack_int n, lapack_int nrhs,
                          MATRIX0& a, VECTOR& ipiv, MATRIX1& b) {
    return LAPACKE_cgetrs(matrix_layout, trans, n, nrhs, complex_cast(storage(a)), lda(a),
                          storage(ipiv), complex_cast(storage(b)), lda(b));
  }
};

template<>
struct getrs_dispatch<std::complex<double> > {
  template<typename MATRIX0, typename MATRIX1, typename VECTOR>
  static lapack_int getrs(int matrix_layout, char trans, lapack_int n, lapack_int nrhs,
                          MATRIX0& a, VECTOR& ipiv, MATRIX1& b) {
    return LAPACKE_zgetrs(matrix_layout, trans, n, nrhs, complex_cast(storage(a)), lda(a),
                          storage(ipiv), complex_cast(storage(b)), lda(b));
  }
};

}
  
template<typename MATRIX0, typename MATRIX1, typename VECTOR>
lapack_int getrs(char trans, lapack_int nrhs, MATRIX0 const& a,
                 VECTOR const& ipiv, MATRIX1& b) {
  BOOST_STATIC_ASSERT(boost::is_same<typename value_t<VECTOR>::type, lapack_int>::value);
  BOOST_STATIC_ASSERT(boost::is_same<typename value_t<MATRIX0>::type,
                      typename value_t<MATRIX1>::type>::value);
  lapack_int n = rows(a);
  if (rows(a) != cols(a))
    throw std::invalid_argument("matrix A size mismatch");
  if (size(ipiv) != n)
    throw std::invalid_argument("vector ipiv size mismatch");
  if (rows(b) != n || cols(b) != nrhs)
    throw std::invalid_argument("matrix B size mismatch");
  return getrs_dispatch<typename value_t<MATRIX0>::type>
    ::getrs((is_col_major(a) ? LAPACK_COL_MAJOR : LAPACK_ROW_MAJOR), trans, n, nrhs, a,
            ipiv, b);
}

} // end namespace lapack
} // end namespace rokko

#endif // ROKKO_LAPACK_GETRS_HPP
