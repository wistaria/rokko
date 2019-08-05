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

#ifndef ROKKO_SCALAPACK_DIAGONALIZE_PDSYEV_HPP
#define ROKKO_SCALAPACK_DIAGONALIZE_PDSYEV_HPP

#include <rokko/distributed_matrix.hpp>
#include <rokko/localized_vector.hpp>
#include <rokko/parameters.hpp>
#include <rokko/blacs.hpp>
#include <rokko/blacs/utility_routines.hpp>
#include <rokko/cscalapack.h>
#include <rokko/lapack/diagonalize_get_parameters.hpp>
#include <rokko/utility/timer.hpp>

namespace rokko {
namespace scalapack {

// eigenvalues / eigenvectors
template<typename MATRIX_MAJOR>
parameters diagonalize_pdsyev(distributed_matrix<double, MATRIX_MAJOR>& mat,
			      localized_vector<double>& eigvals, distributed_matrix<double, MATRIX_MAJOR>& eigvecs,
			      parameters const& params) {
  parameters params_out;
  char jobz = 'V';  // eigenvalues / eigenvectors
  char uplow = lapack::get_matrix_part(params);
  int ictxt = mat.get_grid().get_blacs_context();
  int dim = mat.get_m_global();
  int desc[9];
  blacs::set_desc(ictxt, mat, desc);
  int info;

  info = cscalapack_pdsyev(jobz, uplow, dim, mat.get_array_pointer(), 0, 0, desc, &eigvals[0],
  		      eigvecs.get_array_pointer(), 0, 0, desc);

  params_out.set("info", info);
  if (info) {
    std::cerr << "error at pdsyev function. info=" << info << std::endl;
    exit(1);
  }

  if ((mat.get_myrank() == 0) && params.get_bool("verbose")) {
    lapack::print_verbose("pdsyev", jobz, uplow);
  }
  return params_out;
}

// only eigenvalues
template<typename MATRIX_MAJOR>
parameters diagonalize_pdsyev(distributed_matrix<double, MATRIX_MAJOR>& mat,
			      localized_vector<double>& eigvals,
			      parameters const& params) {
  parameters params_out;
  char jobz = 'N';  // only eigenvalues
  char uplow = lapack::get_matrix_part(params);

  int ictxt = mat.get_grid().get_blacs_context();
  int dim = mat.get_m_global();
  int desc[9];
  blacs::set_desc(ictxt, mat, desc);
  int info;

  info = cscalapack_pdsyev(jobz, uplow, dim, mat.get_array_pointer(), 0, 0, desc, &eigvals[0],
		      NULL, 0, 0, desc);

  params_out.set("info", info);
  if (info) {
    std::cerr << "error at pdsyev function. info=" << info << std::endl;
    exit(1);
  }

  if ((mat.get_myrank() == 0) && params.get_bool("verbose")) {
    lapack::print_verbose("pdsyev", jobz, uplow);
  }
  return params_out;
}

} // namespace scalapack
} // namespace rokko

#endif // ROKKO_SCALAPACK_DIAGONALIZE_PDSYEV_HPP
