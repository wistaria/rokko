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

#include <rokko/solver_name.h>
#include <rokko/utility/solver_name.hpp>

char* copy_string(std::string const& str);

void rokko_split_solver_name(char* str, char* library, char* routine) {
  std::string tmp_library, tmp_routine;
  rokko::split_solver_name(str, tmp_library, tmp_routine); 
  library = copy_string(tmp_library);
  routine = copy_string(tmp_routine);
}

void rokko_split_solver_name_f(char* str, char** library_ptr, char** routine_ptr) {
  std::string tmp_library, tmp_routine;
  rokko::split_solver_name(str, tmp_library, tmp_routine); 
  *library_ptr = copy_string(tmp_library);
  *routine_ptr = copy_string(tmp_routine);
}

