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

#ifndef ROKKO_MAPPING_LOCAL2ARRAY_HPP
#define ROKKO_MAPPING_LOCAL2ARRAY_HPP

#include <rokko/grid.hpp>
#include <rokko/matrix_major.hpp>
#include <rokko/mapping_common_sizes.hpp>

#include <rokko/grid.hpp>
#include <boost/type_traits/is_same.hpp>

namespace rokko {

class mapping_local2array : virtual public mapping_common_sizes {
public:
  explicit mapping_local2array() {}
  template<typename MATRIX_MAJOR>
  explicit mapping_local2array(MATRIX_MAJOR) {
    is_row = boost::is_same<MATRIX_MAJOR, matrix_row_major>::value;
    set_default_lld();
    set_default_length_array();
  }
  template<typename MATRIX_MAJOR>
  explicit mapping_local2array(int lld_in, MATRIX_MAJOR) : lld(lld_in) {
    is_row = boost::is_same<MATRIX_MAJOR, matrix_row_major>::value;
    set_default_length_array();
  }
  //template<typename MATRIX_MAJOR>
  //explicit mapping_local2array(int lld_in, int length_array_in, MATRIX_MAJOR) : lld(lld_in), length_array(length_array_in) {
  //  is_row = boost::is_same<MATRIX_MAJOR, matrix_row_major>::value;
  //}

  void set_length_array(int value) { length_array = value; }
  int get_length_array() const { return length_array; }

  int get_lld() const { return lld; };

  void set_lld(int value) { lld = value; };
  void set_default_lld() { set_lld(get_default_lld()); }

  int get_default_lld() const {
    return is_row ? get_n_local() : get_m_local();
  }
  int get_default_length_array() const {
    return is_row ? (get_m_local() * lld) : (lld * get_n_local());
  }
  void set_default_length_array() { set_length_array(get_default_length_array()); }

  int get_array_index(int local_i, int local_j) const {
    return is_row ? (local_i * lld + local_j) : (local_i + local_j * lld);
  }

  bool is_row_major() const {
    return is_row;
  }
  bool is_col_major() const {
    return !is_row;
  }

private:
  bool is_row;
  int length_array;
  int lld;
};

} // namespace rokko

#endif // ROKKO_MAPPING_LOCAL2ARRAY_HPP