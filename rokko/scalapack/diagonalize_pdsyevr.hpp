/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2012-2016 Rokko Developers https://github.com/t-sakashita/rokko
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#ifndef ROKKO_SCALAPACK_DIAGONALIZE_PDSYEVR_HPP
#define ROKKO_SCALAPACK_DIAGONALIZE_PDSYEVR_HPP

#include <rokko/distributed_matrix.hpp>
#include <rokko/parameters.hpp>
#include <rokko/cscalapack.h>
#include <rokko/lapack/diagonalize_get_parameters.hpp>
#include <rokko/utility/timer.hpp>

#include <mpi.h>

namespace rokko {

namespace scalapack {

// pdsyevd eigenvalues / eigenvectors
template<typename MATRIX_MAJOR, typename VEC>
parameters diagonalize_pdsyevr(distributed_matrix<double, MATRIX_MAJOR>& mat,
			VEC& eigvals, distributed_matrix<double, MATRIX_MAJOR>& eigvecs,
			parameters const& params) {
  parameters params_out;
  char jobz = 'V';  // eigenvalues / eigenvectors
  char uplow = lapack::get_matrix_part(params);
  double vl = 0, vu = 0;
  int il = 0, iu = 0;
  char range = lapack::get_eigenvalues_range(params, vu, vl, iu, il);
  const int* desc = mat.get_mapping().get_blacs_descriptor();
  int info = ROKKO_pdsyevr(jobz, range, uplow, mat.get_m_global(), mat.get_array_pointer(), 0, 0, desc,
                           vl, vu, il, iu, &m, &nz,
                           &eigvals[0], eigvecs.get_array_pointer(), 0, 0, desc);
  if (info) {
    std::cerr << "error at pdsyevr function. info=" << info << std::endl;
    exit(1);
  }
  params_out.set("info", info);
  params_out.set("m", m);
  params_out.set("nz", nz);
  if ((mat.get_myrank() == 0) && params.get_bool("verbose")) {
    lapack::print_verbose("pdsyevr", jobz, range, uplow, vl, vu, il, iu, params_out);
  }
  return params_out;
}

// pdsyevd only eigenvalues
  template<typename MATRIX_MAJOR, typename VEC>
parameters diagonalize_pdsyevr(distributed_matrix<double, MATRIX_MAJOR>& mat,
			       VEC& eigvals,
			       parameters const& params) {
  parameters params_out;
  char jobz = 'N';  // only eigenvalues
  char uplow = lapack::get_matrix_part(params);
  double vl = 0, vu = 0;
  int il = 0, iu = 0;
  char range = lapack::get_eigenvalues_range(params, vu, vl, iu, il);
  const int* desc = mat.get_mapping().get_blacs_descriptor();
  int info = ROKKO_pdsyevr(jobz, range, uplow, mat.get_m_global(), mat.get_array_pointer(), 0, 0, desc,
		       vl, vu, il, iu, &m, &nz,
		       &eigvals[0], NULL, 0, 0, desc);

  if (info) {
    std::cerr << "error at pdsyevr function. info=" << info << std::endl;
    exit(1);
  }
  params_out.set("info", info);
  params_out.set("m", m);
  params_out.set("nz", nz);
  if ((mat.get_myrank() == 0) && params.get_bool("verbose")) {
    lapack::print_verbose("pdsyevr", jobz, range, uplow, vl, vu, il, iu, params_out);
  }
  return params_out;
}

} // namespace scalapack
} // namespace rokko

#endif // ROKKO_SCALAPACK_DIAGONALIZE_PDSYEVR_HPP
