#
# Rokko: Integrated Interface for libraries of eigenvalue decomposition
#
# Copyright (C) 2012-2014 by Synge Todo <wistaria@comp-phys.org>
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

add_subdirectory(dense)
if(BUILD_PARALLEL_SPARSE_SOLVER)
  add_subdirectory(sparse)
endif(BUILD_PARALLEL_SPARSE_SOLVER)
