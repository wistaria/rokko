/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2012-2014  Rokko Developers https://github.com/t-sakashita/rokko
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#ifndef ROKKO_ELEMENTAL_DIAGONALIZE_HPP
#define ROKKO_ELEMENTAL_DIAGONALIZE_HPP

#include <rokko/distributed_matrix.hpp>
#include <rokko/localized_vector.hpp>
#include <elemental.hpp>
#include <rokko/utility/timer.hpp>

namespace rokko {
namespace elemental {

template<typename MATRIX_MAJOR>
int diagonalize(distributed_matrix<MATRIX_MAJOR>& mat, localized_vector& eigvals,
  distributed_matrix<MATRIX_MAJOR>& eigvecs, timer& timer) {
  timer.start(timer_id::diagonalize_initialize);
  MPI_Comm comm = mat.get_grid().get_comm();
  enum elem::GridOrder elemental_grid_order;  //elem::ROW_MAJOR;
  if (mat.get_grid().is_row_major()) {
    elemental_grid_order = elem::ROW_MAJOR;
  } else {
    elemental_grid_order = elem::COLUMN_MAJOR;
  }
  elem::Grid elem_grid(comm, mat.get_grid().get_nprow(), elemental_grid_order);
  elem::DistMatrix<double> elem_mat;
  elem_mat.Attach(mat.get_m_global(), mat.get_n_global(), elem_grid, 0, 0,
		  mat.get_array_pointer(), mat.get_lld());
  elem::DistMatrix<double> elem_eigvecs(0, 0, elem_grid);
  elem::DistMatrix<double, elem::VR, elem::STAR> elem_w(elem_grid);
  timer.stop(timer_id::diagonalize_initialize);

  timer.start(timer_id::diagonalize_diagonalize);
  elem::HermitianEig(elem::LOWER, elem_mat, elem_w, elem_eigvecs); // only access lower half of H
  timer.stop(timer_id::diagonalize_diagonalize);

  timer.start(timer_id::diagonalize_finalize);
  for (int i = 0; i < eigvals.size(); ++i) eigvals(i) = elem_w.Get(i, 0);
  double* result_mat = elem_eigvecs.Buffer();
  for(int local_i=0; local_i<mat.get_m_local(); ++local_i) {
    for(int local_j=0; local_j<mat.get_n_local(); ++local_j) {
      eigvecs.set_local(local_i, local_j, result_mat[local_j * mat.get_lld() + local_i]);
    }
  }
  timer.stop(timer_id::diagonalize_finalize);
}

} // namespace elemental
} // namespace rokko

#endif // ROKKO_ELEMENTAL_DIAGONALIZE_HPP
