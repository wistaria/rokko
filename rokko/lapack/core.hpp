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

#ifndef ROKKO_LAPACK_CORE_HPP
#define ROKKO_LAPACK_CORE_HPP

#include <rokko/lapack/diagonalize.hpp>
#include <rokko/lapack/diagonalize_d.hpp>
#include <rokko/lapack/diagonalize_r.hpp>

namespace rokko {
namespace lapack {

struct dsyev {};
// struct dsyevd {};
// struct dsyevx {};

class solver {
public:
  void initialize(int& argc, char**& argv) {}
  void finalize() {}
  template<typename MATRIX_MAJOR>
  void diagonalize(std::string const& routine, localized_matrix<double, MATRIX_MAJOR>& mat,
		   double* eigvals,
                   rokko::parameters const& params, timer& timer);
  template<typename MATRIX_MAJOR>  
  void diagonalize(std::string const& routine, localized_matrix<double, MATRIX_MAJOR>& mat,
		   double* eigvals, localized_matrix<double, MATRIX_MAJOR>& eigvecs,
		   rokko::parameters const& params, timer& timer);
  template<typename MATRIX_MAJOR, typename VEC>
  void diagonalize(std::string const& routine, localized_matrix<double, MATRIX_MAJOR>& mat,
		   VEC& eigvals,
                   rokko::parameters const& params, timer& timer);
  template<typename MATRIX_MAJOR, typename VEC>  
  void diagonalize(std::string const& routine, localized_matrix<double, MATRIX_MAJOR>& mat,
		   VEC& eigvals, localized_matrix<double, MATRIX_MAJOR>& eigvecs,
		   rokko::parameters const& params, timer& timer);
private:
  //std::string routine_;  // record routine_;
};

// only eigenvalues
template<typename MATRIX_MAJOR>
void solver::diagonalize(std::string const& routine, localized_matrix<double, MATRIX_MAJOR>& mat,
			 double* eigvals,
			 rokko::parameters const& params, timer& timer) {
  if (routine=="lapack:dsyev") {
    std::cout << "lapack:dyev" << std::endl;
    rokko::lapack::diagonalize(mat, eigvals, params, timer);
  } else if (routine=="lapack:dsyevr") {
    rokko::lapack::diagonalize_r(mat, eigvals, params, timer);
  } else if (routine=="lapack:dsyevd") {
    rokko::lapack::diagonalize_d(mat, eigvals, params, timer);
  } else {
    std::cerr << "error: " << routine << " is not lapack routine" << std::endl;
    throw;
  }
}

template<typename MATRIX_MAJOR, typename VEC>
void solver::diagonalize(std::string const& routine, localized_matrix<double, MATRIX_MAJOR>& mat,
			 VEC& eigvals,
			 rokko::parameters const& params, timer& timer) {
  timer.start(timer_id::diagonalize_initialize);
  int dim = mat.rows();
  if (eigvals.size() < dim) eigvals.resize(dim);
  timer.stop(timer_id::diagonalize_initialize);
  return solver::diagonalize(routine, mat, &eigvals[0], params, timer);
}

// eigenvalues/eigenvectors
template<typename MATRIX_MAJOR>
void solver::diagonalize(std::string const& routine, localized_matrix<double, MATRIX_MAJOR>& mat,
			 double* eigvals, localized_matrix<double, MATRIX_MAJOR>& eigvecs,
			 rokko::parameters const& params, timer& timer) {
  if (routine=="lapack:dsyev") {
    std::cout << "lapack:dyev" << std::endl;
    rokko::lapack::diagonalize(mat, eigvals, eigvecs, params, timer);
  } else if (routine=="lapack:dsyevr") {
    rokko::lapack::diagonalize_r(mat, eigvals, eigvecs, params, timer);
  } else if (routine=="lapack:dsyevd") {
    rokko::lapack::diagonalize_d(mat, eigvals, eigvecs, params, timer);
  } else {
    std::cerr << "error: " << routine << " is not lapack routine" << std::endl;
    throw;
  }
}

template<typename MATRIX_MAJOR, typename VEC>
void solver::diagonalize(std::string const& routine, localized_matrix<double, MATRIX_MAJOR>& mat,
			 VEC& eigvals, localized_matrix<double, MATRIX_MAJOR>& eigvecs,
			 rokko::parameters const& params, timer& timer) {
  timer.start(timer_id::diagonalize_initialize);
  int dim = mat.rows();
  if (eigvals.size() < dim) eigvals.resize(dim);
  timer.stop(timer_id::diagonalize_initialize);
  return solver::diagonalize(routine, mat, &eigvals[0], eigvecs, params, timer);
}

} // namespace lapack
} // namespace rokko

#endif // ROKKO_LAPACK_CORE_HPP
