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

#ifndef ROKKO_LAPACK_DIAGONALIZE_GET_PARAMETERS_HPP
#define ROKKO_LAPACK_DIAGONALIZE_GET_PARAMETERS_HPP

namespace rokko {
namespace lapack {

void get_matrix_part(parameters const& params, std::string& matrix_part, char& uplow);

void get_eigenvalues_range(parameters const& params, std::string& matrix_part, char& range, double vu, double vl, int iu, int il, bool& is_upper_value, bool& is_lower_value, bool& is_upper_index, bool& is_lower_index);

} // namespace lapack
} // namespace rokko

#endif // ROKKO_LAPACK_DIAGONALIZE_GET_PARAMETERS_HPP

