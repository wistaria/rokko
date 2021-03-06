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

#ifndef ROKKO_SCALAPACK_DIAGONALIZE_PDSYEVD_HPP
#define ROKKO_SCALAPACK_DIAGONALIZE_PDSYEVD_HPP

#include <rokko/distributed_matrix.hpp>
#include <rokko/localized_vector.hpp>
#include <rokko/parameters.hpp>
#include <rokko/blacs/blacs_wrap.h>
#include <rokko/blacs/utility_routines.hpp>
#include <rokko/scalapack/scalapack_wrap.h>
#include <rokko/lapack/diagonalize_get_parameters.hpp>
#include <rokko/utility/timer.hpp>

#include <mpi.h>

namespace rokko {

namespace scalapack {

// pdsyevd eigenvalues / eigenvectors
template<typename MATRIX_MAJOR>
parameters diagonalize_pdsyevd(distributed_matrix<double, MATRIX_MAJOR>& mat,
			       localized_vector<double>& eigvals, distributed_matrix<double, MATRIX_MAJOR>& eigvecs,
			       parameters const& params) {
  parameters params_out;
  char jobz = 'V';  // eigenvalues / eigenvectors
  char uplow = lapack::get_matrix_part(params);

  int ictxt = ROKKO_blacs_get(-1, 0);
  char char_grid_major = rokko::blacs::set_grid_blacs(ictxt, mat);
  int dim = mat.get_m_global();
  int desc[9];
  rokko::blacs::set_desc(ictxt, mat, desc);
  int info;

  info = ROKKO_pdsyevd(jobz, uplow, dim, mat.get_array_pointer(), 1, 1, desc, &eigvals[0],
		       eigvecs.get_array_pointer(), 1, 1, desc);

  if (info) {
    std::cerr << "error at pdsyevd function. info=" << info << std::endl;
    exit(1);
  }
  if ((mat.get_myrank() == 0) && params.get_bool("verbose")) {
    lapack::print_verbose("pdsyevd", jobz, uplow);
  }
  ROKKO_blacs_gridexit(&ictxt);

  return params_out;
}

// pdsyevd only eigenvalues
template<typename MATRIX_MAJOR>
parameters diagonalize_pdsyevd(distributed_matrix<double, MATRIX_MAJOR>& mat,
			       localized_vector<double>& eigvals,
			       parameters const& params) {
  parameters params_out;
  char jobz = 'N';  // only eigenvalues
  char uplow = lapack::get_matrix_part(params);

  int ictxt = ROKKO_blacs_get(-1, 0);
  char char_grid_major = rokko::blacs::set_grid_blacs(ictxt, mat);
  int dim = mat.get_m_global();
  int desc[9];
  rokko::blacs::set_desc(ictxt, mat, desc);
  int info;

  info = ROKKO_pdsyevd(jobz, uplow, dim, mat.get_array_pointer(), 1, 1, desc, &eigvals[0],
		       NULL, 1, 1, desc);

  if (info) {
    std::cerr << "error at pdsyevd function. info=" << info << std::endl;
    exit(1);
  }
  if ((mat.get_myrank() == 0) && params.get_bool("verbose")) {
    lapack::print_verbose("pdsyevd", jobz, uplow);
  }
  ROKKO_blacs_gridexit(&ictxt);

  return params_out;
}

} // namespace scalapack
} // namespace rokko

#endif // ROKKO_SCALAPACK_DIAGONALIZE_PDSYEVD_HPP
