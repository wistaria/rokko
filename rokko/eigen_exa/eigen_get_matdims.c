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

#include <rokko/eigen_exa/eigen_exa.h>

void ROKKO_eigen_get_matdims(int nprow, int npcol, int n, int *nx, int *ny) {
  eigen_get_matdims_wrap_(&nprow, &npcol, &n, nx, ny);
}