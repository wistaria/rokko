/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2012-2015 by Rokko Developers https://github.com/t-sakashita/rokko
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#include <rokko/cblacs.h>
#include <rokko/blacs/blacs_interface.h>

int cblacs_sys2blacs_handle(int *SysCtxt) { return BLACS_sys2blacs_handle(SysCtxt); }