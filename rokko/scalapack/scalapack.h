/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2012-2013 by Tatsuya Sakashita <t-sakashita@issp.u-tokyo.ac.jp>,
*                            Synge Todo <wistaria@comp-phys.org>
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#ifndef ROKKO_SCALAPACK_H
#define ROKKO_SCALAPACK_H

#ifdef __cplusplus
extern "C" {
#endif

void pdelget_(const char* scope, const char* top, double* alpha,
              const double* A, const int* ia, const int* ja, const int* descA);
void pdelset_(double* A, const int* ia, const int* ja, const int* descA, const double* alpha);
double pdlamch_(const int* icnt, const char* cmch);
void pdlaprnt_(const int* m, const int* n, const double* A, const int* ia, const int* ja,
               const int* descA, const int* irprnt, const int* icprnt, const char* cmatnm,
               const int* nout, double* work);
void pdsyev_(const char* jobz, const char* uplo, const int* n,
             double* A, const int* ia, const int* ja, const int* descA,
             double* w, double* Z, const int* iz, const int* jz, const int* descZ,
             double* work, const int* lwork, int* info);
void pdsyevd_(const char* jobz, const char* uplo, const int* n,
              double* A, const int* ia, const int* ja, const int* descA,
              double* w,
              double* Z, const int* iz, const int* jz, const int* descZ,
              double* work, const int* lwork, int* iwork, const int* liwork, int* info);
void pdsyevr_(const char* jobz, const char* uplo, const int* n,
              double* A, const int* ia, const int* ja, const int* descA,
              const double* vl, const double* vu, const int* il, const int* iu,
              int* m, int* nz, double* w,
              double* Z, const int* iz, const int* jz, const int* descZ,
              double* work, const int* lwork, int* iwork, const int* liwork, int* info);
void pdsyevx_(const char* jobz, const char* range, const char* uplo, const int* n,
              double* A, const int* iA, const int* jA, const int* descA,
              const double* vl, const double* vu, const int* il, const int* iu,
              const double* abstol, int* m, int* nZ, double* w, const double* orfac,
              double* Z, const int* iZ, const int* jZ, const int* descZ,
              double* work, const int* lwork, int* iwork, const int* liwork,
              int* ifail, int* iclustr, double* gap, int* info);

void ROKKO_pdelget(char scope, char top, double* alpha, const double* A, int ia, int ja,
                   const int* descA);
void ROKKO_pdelset(double* A, int ia, int ja, const int* descA, double alpha);
double ROKKO_pdlamch(int icnt, char cmch);
void ROKKO_pdlaprnt(int m, int n, const double* A, int ia, int ja, const int* descA, int irprnt,
                    int icprnt, const char* cmatnm, int nout, double* work);
void ROKKO_pdsyev(char jobz, char uplo, int n,
                  double* A, int ia, int ja, const int* descA,
                  double* w, double* Z, int iz, int jz, const int* descZ,
                  double* work, int lwork, int* info);
void ROKKO_pdsyevd(char jobz, char uplo, int n,
                   double* A, int ia, int ja, const int* descA,
                   double* w, double* Z, int iz, int jz, const int* descZ,
                   double* work, int lwork, int* iwork, int liwork, int* info);
void ROKKO_pdsyevr(char jobz, char uplo, int n,
                   double* A, int ia, int ja, const int* descA,
                   double vl, double vu, int il, int iu,
                   int* m, int* nz, double* w,
                   double* Z, int iz, int jz, const int* descZ,
                   double* work, int lwork, int* iwork, int liwork, int* info);
void ROKKO_pdsyevx(char jobz, char range, char uplo, int n,
                   double* A, int iA, int jA, const int* descA,
                   double vl, double vu, int il, int iu,
                   double abstol, int* m, int* nZ, double* w, double orfac,
                   double* Z, int iZ, int jZ, const int* descZ,
                   double* work, int lwork, int* iwork, int liwork,
                   int* ifail, int* iclustr, double* gap, int* info);

#ifdef __cplusplus
}
#endif

#endif // ROKKO_SCALAPACK_H