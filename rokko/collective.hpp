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

#ifndef ROKKO_COLLECTIVE_HPP
#define ROKKO_COLLECTIVE_HPP

#include <mpi.h>
#include <rokko/blacs.hpp>
#include <rokko/scalapack.hpp>
#include <rokko/pblas.hpp>
#include <rokko/distributed_matrix.hpp>
#include <rokko/localized_matrix.hpp>

namespace rokko {

template<typename T, typename MATRIX_MAJOR>
void gather(rokko::distributed_matrix<T, MATRIX_MAJOR> const& from, T* to, int root) {
  if (!from.is_col_major()) {
    std::cerr << "Error (gather): matrix_row_major is not supported\n";
    MPI_Abort(MPI_COMM_WORLD,67);
    exit(67);
  }
  int ictxt = from.get_grid().get_blacs_context();
  int m = from.get_m_global();
  int n = from.get_n_global();
  int rsrc = (from.get_grid().is_row_major() ? (root / from.get_npcol()) :
              (root % from.get_nprow()));
  int csrc = (from.get_grid().is_row_major() ? (root % from.get_npcol()) :
              (root / from.get_nprow()));
  int descFrom[9], descTo[9];
  scalapack::descinit(descFrom, m, n, from.get_mb(), from.get_nb(), 0, 0, ictxt, from.get_lld());
  scalapack::descinit(descTo, m, n, m, n, rsrc, csrc, ictxt, m);
  for (int j = 0; j < n; ++j)
    pblas::pcopy(m, from.get_array_pointer(), 1, (j+1), descFrom, 1, to, 1, (j+1), descTo, 1);
}

template<typename T, typename MATRIX_MAJOR>
void gather(rokko::distributed_matrix<T, MATRIX_MAJOR> const& from,
  localized_matrix<T, MATRIX_MAJOR>& to, int root) {
  gather(from, &to(0,0), root);
}

template<typename T, typename MATRIX_MAJOR>
void scatter(const T* from, distributed_matrix<T, MATRIX_MAJOR>& to, int root) {
  if (!to.is_col_major()) {
    std::cerr << "Error (scatter): matrix_row_major is not supported\n";
    MPI_Abort(MPI_COMM_WORLD,67);
    exit(67);
  }
  int ictxt = to.get_grid().get_blacs_context();
  int m = to.get_m_global();
  int n = to.get_n_global();
  int rsrc = (to.get_grid().is_row_major() ? (root / to.get_npcol()) : (root % to.get_nprow()));
  int csrc = (to.get_grid().is_row_major() ? (root % to.get_npcol()) : (root / to.get_nprow()));
  int descFrom[9], descTo[9];
  scalapack::descinit(descFrom, m, n, m, n, rsrc, csrc, ictxt, m);
  scalapack::descinit(descTo, m, n, to.get_mb(), to.get_nb(), 0, 0, ictxt, to.get_lld());
  for (int j = 0; j < n; ++j)
    pblas::pcopy(m, from, 1, (j+1), descFrom, 1, to.get_array_pointer(), 1, (j+1), descTo, 1);
}

template<typename T, typename MATRIX_MAJOR>
void scatter(localized_matrix<T, MATRIX_MAJOR> const& from,
  distributed_matrix<T, MATRIX_MAJOR>& to, int root) {
  scatter(&from(0,0), to, root);
}

} // namespace rokko

#endif // ROKKO_COLLECTIVE_HPP
