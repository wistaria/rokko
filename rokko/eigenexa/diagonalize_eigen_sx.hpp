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

#ifndef ROKKO_EIGENEXA_DIAGONALIZE_EIGEN_SX_HPP
#define ROKKO_EIGENEXA_DIAGONALIZE_EIGEN_SX_HPP

#include <rokko/distributed_matrix.hpp>
#include <rokko/localized_vector.hpp>
#include <rokko/parameters.hpp>
#include <rokko/eigenexa.hpp>
#include <rokko/utility/timer.hpp>

namespace rokko {
namespace eigenexa {

// eigen_sx eigenvalues / eigenvectors
template <typename MATRIX_MAJOR>
parameters diagonalize_eigen_sx(rokko::distributed_matrix<double, MATRIX_MAJOR>& mat,
				localized_vector<double>& eigvals,
				rokko::distributed_matrix<double, MATRIX_MAJOR>& eigvecs,
				parameters const& params) {
  parameters params_out;
  if(mat.is_row_major())
    throw std::invalid_argument("eigenexa::diagonalize_eigen_sx() : eigenexa doesn't support matrix_row_major.  Use eigenexa with matrix_col_major.");
  if((mat.get_mb() != 1) || (mat.get_nb() != 1))
    throw std::invalid_argument("eigenexa::diagonalize_eigen_sx() : eigenexa supports only 1x1 block size.");
  rokko::eigenexa::init(mat.get_grid().get_comm(), (mat.get_grid().is_row_major() ? 'R' : 'C'));
  int m_forward = 48, m_backward = 128;
  get_key(params, "m_forward", m_forward);
  get_key(params, "m_backward", m_backward);

  rokko::eigenexa::eigen_sx(mat, eigvals, eigvecs, m_forward, m_backward, 'A');

  rokko::eigenexa::free(1);
  return params_out;
}

// eigen_sx only eigenvalues
template <typename MATRIX_MAJOR>
parameters diagonalize_eigen_sx(rokko::distributed_matrix<double, MATRIX_MAJOR>& mat,
				localized_vector<double>& eigvals,
				parameters const& params) {
  parameters params_out;
  if(mat.is_row_major())
    throw std::invalid_argument("eigenexa::diagonalize_eigen_sx() : eigenexa doesn't support matrix_row_major.  Use eigenexa with matrix_col_major.");
  if((mat.get_mb() != 1) || (mat.get_nb() != 1))
    throw std::invalid_argument("eigenexa::diagonalize_eigen_sx() : eigenexa supports only 1x1 block size.");
  rokko::eigenexa::init(mat.get_grid().get_comm(), (mat.get_grid().is_row_major() ? 'R' : 'C'));
  int m_forward = 48, m_backward = 128;
  get_key(params, "m_forward", m_forward);
  get_key(params, "m_backward", m_backward);

  rokko::eigenexa::eigen_sx(mat, eigvals, m_forward, m_backward, 'N');

  ceigenexa_free1(1);
  return params_out;
}

} // namespace eigenexa
} // namespace rokko

#endif // ROKKO_EIGENEXA_DIAGONALIZE_EIGEN_SX_HPP