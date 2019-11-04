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

#ifndef ROKKO_LAPACK_DIAGONALIZE_DSYEVX_HPP
#define ROKKO_LAPACK_DIAGONALIZE_DSYEVX_HPP

#include <rokko/parameters.hpp>
#include <rokko/eigen3.hpp>
#include <rokko/utility/timer.hpp>
#include <rokko/lapack.hpp>
#include <rokko/lapack/diagonalize_get_parameters.hpp>

namespace rokko {
namespace lapack {

// dsyevx only eigenvalues
template<int MATRIX_MAJOR>
parameters diagonalize_dsyevx(Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,MATRIX_MAJOR>& mat, double* eigvals,
			      parameters const& params) {
  parameters params_out;
  const char jobz = 'N';  // only eigenvalues
  const int dim = mat.outerSize();
  const int ld_mat = mat.innerSize();
  lapack_int m;  // output: found eigenvalues
  double abstol = params.defined("abstol") ? params.get<double>("abstol") : 2*LAPACKE_dlamch('S');
  params_out.set("abstol", abstol);

  lapack_int il, iu;
  double vl, vu;
  const char range = get_eigenvalues_range(params, vl, vu, il, iu);
  const char uplow = get_matrix_part(params);

  std::vector<lapack_int> ifail(dim);
  int info;

  if(MATRIX_MAJOR == Eigen::ColMajor)
    info = LAPACKE_dsyevx(LAPACK_COL_MAJOR, jobz, range, uplow, dim,
			  &mat(0,0), ld_mat, vl, vu, il, iu,
			  abstol, &m, eigvals, NULL, ld_mat, &ifail[0]);
  else
    info = LAPACKE_dsyevx(LAPACK_ROW_MAJOR, jobz, range, uplow, dim,
			  &mat(0,0), ld_mat, vl, vu, il, iu,
			  abstol, &m, eigvals, NULL, ld_mat, &ifail[0]);

  if (info) {
    std::cerr << "error at dsyevx function. info=" << info << std::endl;
    if (info < 0) {
      std::cerr << "This means that ";
      std::cerr << "the " << abs(info) << "-th argument had an illegal value." << std::endl;
    }
  }
  params_out.set("info", info);
  params_out.set("m", m);
  params_out.set("ifail", ifail);
  
  if (params.get_bool("verbose")) {
    print_verbose("dsyevx", jobz, range, uplow, vl, vu, il, iu, params_out);
  }

  return params_out;
}


// dsyevx eigenvalues / eigenvectors
template<int MATRIX_MAJOR>
parameters diagonalize_dsyevx(Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,MATRIX_MAJOR>& mat, double* eigvals,
			      Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,MATRIX_MAJOR>& eigvecs,
			      parameters const& params) {
  rokko::parameters params_out;
  const char jobz = 'V';  // eigenvalues / eigenvectors
  const int dim = mat.outerSize();
  const int ld_mat = mat.innerSize();
  const int ld_eigvecs = eigvecs.innerSize();
  std::vector<lapack_int> ifail(dim);

  lapack_int m;  // output: found eigenvalues
  double abstol = params.defined("abstol") ? params.get<double>("abstol") : 2*LAPACKE_dlamch('S');
  params_out.set("abstol", abstol);

  lapack_int il, iu;
  double vl, vu;
  const char range = get_eigenvalues_range(params, vl, vu, il, iu);
  const char uplow = get_matrix_part(params);

  int info;
  if(MATRIX_MAJOR == Eigen::ColMajor)
    info = LAPACKE_dsyevx(LAPACK_COL_MAJOR, jobz, range, uplow, dim,
			  &mat(0,0), ld_mat, vl, vu, il, iu,
			  abstol, &m, eigvals, &eigvecs(0,0), ld_eigvecs, &ifail[0]);
  else
    info = LAPACKE_dsyevx(LAPACK_ROW_MAJOR, jobz, range, uplow, dim,
			  &mat(0,0), ld_mat, vl, vu, il, iu,
			  abstol, &m, eigvals, &eigvecs(0,0), ld_eigvecs, &ifail[0]);

  if (info) {
    std::cerr << "Error at dsyevx function. info=" << info << std::endl;
    if (params.get_bool("verbose")) {
      std::cerr << "This means that ";
      if (info < 0) {
        std::cerr << "the " << abs(info) << "-th argument had an illegal value." << std::endl;
      } else {
        std::cerr << "This means that "	<< info << " eigenvectors failed to converge." << std::endl;
        std::cerr << "The indices of the eigenvectors that failed to converge:" << std::endl;
        for (std::size_t i = 0; i < ifail.size(); ++i) {
          if (ifail[i] == 0) break;
          std::cerr << ifail[i] << " ";
        }
        std::cerr << std::endl;
      }
    }
  }
  params_out.set("info", info);
  params_out.set("m", m);
  params_out.set("ifail", ifail);
  
  if (params.get_bool("verbose")) {
    print_verbose("dsyevx", jobz, range, uplow, vl, vu, il, iu, params_out);
  }

  return params_out;
}


} // namespace lapack
} // namespace rokko

#endif // ROKKO_LAPACK_DIAGONALIZE_DSYEVX_HPP
