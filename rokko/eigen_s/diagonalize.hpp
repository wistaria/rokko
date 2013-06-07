#ifndef ROKKO_EIGEN_S_DIAGONALIZE_HPP
#define ROKKO_EIGEN_S_DIAGONALIZE_HPP

#include <mpi.h>
#include <rokko/distributed_matrix.hpp>
#include <rokko/localized_vector.hpp>
#include <rokko/eigen_s/eigen_s.hpp>
#include <rokko/utility/timer.hpp>

namespace rokko {
namespace eigen_s {

template<typename MATRIX_MAJOR>
void diagonalize(distributed_matrix<MATRIX_MAJOR>& mat, double* eigvals, distributed_matrix<MATRIX_MAJOR>& eigvecs, timer& timer_in) {
  if(mat.get_grid().is_row_major()) throw "eigen_s doesn't support grid_row_major.  Use eigen_s with grid_col_major.";
  if(mat.is_row_major()) throw "eigen_s doesn't support matrix_row_major.  Use eigen_s with matrix_col_major.";

  int m = 32;  // block_size

  int iflag;
  //if (flag_eigvecs)
  iflag = 0;
  //else
  //  iflag = 1;

  int dim = mat.get_m_global();
  int mat_lld = mat.get_lld();
  int eigvecs_lld = mat.get_lld();
  double* mat_array = mat.get_array_pointer();
  double* eigvecs_array= eigvecs.get_array_pointer();

  timer_in.start(1);
  eigen_s_(dim, mat_array, mat_lld, eigvals, eigvecs_array, eigvecs_lld, m, iflag);
  timer_in.stop(1);
}

template<typename MATRIX_MAJOR>
void diagonalize(distributed_matrix<MATRIX_MAJOR>& mat, localized_vector& eigvals, distributed_matrix<MATRIX_MAJOR>& eigvecs, timer& timer_in) {
  if(mat.get_grid().is_row_major()) throw "eigen_s doesn't support grid_row_major. Use eigen_s with grid_col_major.";
  if(mat.is_row_major()) throw "eigen_s doesn't support matrix_row_major. Use eigen_s with matrix_col_major.";

  return diagonalize(mat, &eigvals[0], eigvecs, timer_in);
}

} // namespace eigen_s
} // namespace rokko

#endif // ROKKO_EIGEN_S_DIAGONALIZE_HPP

