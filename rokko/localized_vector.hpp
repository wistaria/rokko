/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2012-2013 by Tatsuya Sakashita <t-sakashita@issp.u-tokyo.ac.jp>,
*                            Synge Todo <wistaria@comp-phys.org>
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#ifndef ROKKO_LOCALIZED_VECTOR_HPP
#define ROKKO_LOCALIZED_VECTOR_HPP

#include <Eigen/Dense>

namespace rokko {

struct localized_vector : public Eigen::VectorXd {
public:
  localized_vector() : Eigen::VectorXd() {};
  localized_vector(int size) : Eigen::VectorXd(size) {};
};


} // namespace rokko

#endif // ROKKO_LOCALIZED_VECTOR_HPP
