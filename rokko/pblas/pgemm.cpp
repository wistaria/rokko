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

#include <rokko/pblas.h>

#define PBLAS_PGEMM_IMPL(NAMES, NAMEL, TYPEC, TYPEX) \
extern "C" { \
void PBLASE_ ## NAMES (char TRANSA, char TRANSB, int M, int N, int K, TYPEC ALPHA, const TYPEC * A, int IA, int JA, int* DESCA, const TYPEC * B, int IB, int JB, int* DESCB, TYPEC BETA, TYPEC * C, int IC, int JC, int* DESCC) { \
  LAPACK_GLOBAL(NAMES, NAMEL) (&TRANSA, &TRANSB, &M, &N, &K, &ALPHA, A, &IA, &JA, DESCA, B, &IB, &JB, DESCB, &BETA, C, &IC, &JC, DESCC); }\
} \
void PBLASE_pgemm(char TRANSA, char TRANSB, int M, int N, int K, TYPEX ALPHA, const TYPEX * A, int IA, int JA, int* DESCA, const TYPEX * B, int IB, int JB, int* DESCB, TYPEX BETA, TYPEX * C, int IC, int JC, int* DESCC) { \
  PBLASE_ ## NAMES (TRANSA, TRANSB, M, N, K, ALPHA, A, IA, JA, DESCA, B, IB, JB, DESCB, BETA, C, IC, JC, DESCC); }

PBLAS_PGEMM_IMPL(psgemm, PSGEMM, float, float);
PBLAS_PGEMM_IMPL(pdgemm, PDGEMM, double, double);
PBLAS_PGEMM_IMPL(pcgemm, PCGEMM, lapack_complex_float, std::complex<float>);
PBLAS_PGEMM_IMPL(pzgemm, PZGEMM, lapack_complex_double, std::complex<double>);

#undef PBLAS_PGEMM_IMPL
