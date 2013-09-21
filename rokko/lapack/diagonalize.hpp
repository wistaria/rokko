#ifndef ROKKO_LAPACK_DIAGONALIZE_H
#define ROKKO_LAPACK_DIAGONALIZE_H

#include "rokko/localized_matrix.hpp"
#include "rokko/localized_vector.hpp"
#include <lapacke.h>

namespace rokko {
namespace lapack {

template<typename MATRIX_MAJOR>
int diagonalize(localized_matrix<MATRIX_MAJOR>& mat, double* eigvals, localized_matrix<MATRIX_MAJOR>& eigvecs, timer& timer_in) {
  int info;

  int dim = mat.rows();

  // eigenvalue decomposition
  timer_in.start(1);
  info = LAPACKE_dsyev(LAPACK_COL_MAJOR, 'V', 'U', dim, &mat(0,0), dim, eigvals);
  timer_in.stop(1);
  eigvecs = mat;

  if (info) {
    std::cerr << "error at dsyev function. info=" << info  << std::endl;
    exit(1);
  }

  return info;
}

template<class MATRIX_MAJOR>
int diagonalize(localized_matrix<MATRIX_MAJOR>& mat, localized_vector& eigvals,
                localized_matrix<MATRIX_MAJOR>& eigvecs, timer& timer_in) {
  return diagonalize(mat, &eigvals[0], eigvecs, timer_in);
}

} // namespace lapack
} // namespace rokko

#endif // ROKKO_LAPACK_DIAGONALIZE_H