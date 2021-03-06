/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2015 by Rokko Developers https://github.com/t-sakashita/rokko
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#include <rokko/lapack.h>

#define LAPACKE_ORGQR_IMPL(NAME, TYPE) \
lapack_int LAPACKE_ungqr(int matrix_order, lapack_int m, lapack_int n, lapack_int k, TYPE * a, lapack_int lda, const TYPE * tau) { \
  return LAPACKE_ ## NAME (matrix_order, m, n, k, a, lda, tau); }
  
LAPACKE_ORGQR_IMPL(sorgqr, float);
LAPACKE_ORGQR_IMPL(dorgqr, double);

#undef LAPACKE_ORGQR_IMPL

#define LAPACKE_UNGQR_IMPL(NAME, TYPE)                                  \
lapack_int LAPACKE_ungqr(int matrix_order, lapack_int m, lapack_int n, lapack_int k, TYPE * a, lapack_int lda, const TYPE * tau) { \
  return LAPACKE_ ## NAME (matrix_order, m, n, k, a, lda, tau); }

LAPACKE_UNGQR_IMPL(cungqr, lapack_complex_float);
LAPACKE_UNGQR_IMPL(zungqr, lapack_complex_double);

#undef LAPACKE_UNGQR_IMPL
