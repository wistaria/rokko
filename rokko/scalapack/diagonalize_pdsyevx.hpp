/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2012-2014 by Tatsuya Sakashita <t-sakashita@issp.u-tokyo.ac.jp>,
*                            Synge Todo <wistaria@comp-phys.org>,
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#ifndef ROKKO_SCALAPACK_DIAGONALIZE_PDSYEVX_HPP
#define ROKKO_SCALAPACK_DIAGONALIZE_PDSYEVX_HPP

#include <rokko/distributed_matrix.hpp>
#include <rokko/localized_vector.hpp>
#include <rokko/blacs/blacs.h>
#include <rokko/scalapack/scalapack.h>

#include <mpi.h>

namespace rokko {
namespace scalapack {

template<typename MATRIX_MAJOR, typename TIMER>
int diagonalize_x(distributed_matrix<MATRIX_MAJOR>& mat, localized_vector& eigvals,
  distributed_matrix<MATRIX_MAJOR>& eigvecs, TIMER& timer) {
  int ictxt;
  int info;

  ROKKO_blacs_get(-1, 0, &ictxt);

  char char_grid_major;
  if(mat.get_grid().is_row_major())  char_grid_major = 'R';
  else  char_grid_major = 'C';

  ROKKO_blacs_gridinit(&ictxt, char_grid_major, mat.get_grid().get_nprow(), mat.get_grid().get_npcol());

  int dim = mat.get_m_global();
  int desc[9];
  ROKKO_descinit(desc, mat.get_m_global(), mat.get_n_global(), mat.get_mb(), mat.get_nb(), 0, 0, ictxt, mat.get_lld(), &info);
  if (info) {
    std::cerr << "error " << info << " at descinit function of descA " << "mA=" << mat.get_m_local() << "  nA=" << mat.get_n_local() << "." << std::endl;
    MPI_Abort(MPI_COMM_WORLD, 89);
  }

#ifndef NDEBUG
  for (int proc=0; proc<mat.get_nprocs(); ++proc) {
    if (proc == mat.get_myrank()) {
      std::cout << "pdsyev:proc=" << proc << " m_global=" << mat.get_m_global() << "  n_global=" << mat.get_n_global() << "  mb=" << mat.get_mb() << "  nb=" << mat.get_nb() << std::endl;
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }
#endif

  int vl = 0, vu = 0;
  int il, iu;
  double abstol = ROKKO_pdlamch(ictxt, 'U');
  int num_eigval_found, num_eigvec_found;
  int orfac = -1;  // default value 10^{-3} is used.
  int* ifail = new int[dim];
  int* iclustr = new int[2 * mat.get_nprow() * mat.get_npcol()];
  double* gap = new double[mat.get_nprow() * mat.get_npcol()];
  if (ifail == 0) {
    std::cerr << "failed to allocate ifail." << std::endl;
    exit(1);
  }
  if (iclustr == 0) {
    std::cerr << "failed to allocate iclustr." << std::endl;
    exit(1);
  }
  if (gap == 0) {
    std::cerr << "failed to allocate gap." << std::endl;
    exit(1);
  }

  double* work = new double[1];
  int* iwork = new int[1];
  long lwork = -1;
  long liwork = -1;

  // work配列のサイズの問い合わせ
  timer.start(1);
  ROKKO_pdsyevx('V', 'A', 'U', dim, mat.get_array_pointer(), 1, 1, desc,
                vl, vu, il, iu,
                abstol, &num_eigval_found, &num_eigvec_found, &eigvals[0], orfac,
                eigvecs.get_array_pointer(), 1, 1, desc,
                work, lwork, iwork, liwork, ifail, iclustr, gap, &info);
  timer.stop(1);

  if (info) {
    std::cerr << "error at pdsyevx function (query for sizes for workarrays." << std::endl;
    exit(1);
  }

  lwork = work[0];
  liwork = iwork[0];
  delete[] work;
  delete[] iwork;

  work = new double[lwork];
  iwork = new int[liwork];
  if (work == 0) {
    std::cerr << "failed to allocate work. info=" << info << std::endl;
    return info;
  }

  // 固有値分解
  ROKKO_pdsyevx('V', 'A', 'U', dim, mat.get_array_pointer(), 1, 1, desc,
                vl, vu, il, iu, abstol, &num_eigval_found, &num_eigvec_found, &eigvals[0], orfac,
                eigvecs.get_array_pointer(), 1, 1, desc,
                work, lwork, iwork, liwork, ifail, iclustr, gap, &info);

  if (info) {
    std::cerr << "error at pdsyevx function. info=" << info << std::endl;
    exit(1);
  }

  delete[] work;
  delete[] iwork;
  delete[] ifail;
  delete[] iclustr;
  delete[] gap;
  return info;
}

} // namespace scalapack
} // namespace rokko

#endif // ROKKO_SCALAPACK_DIAGONALIZE_PDSYEVX_HPP
