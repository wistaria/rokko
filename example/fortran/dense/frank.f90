!*****************************************************************************
!
! Rokko: Integrated Interface for libraries of eigenvalue decomposition
!
! Copyright (C) 2012-2016 by Rokko Developers https://github.com/t-sakashita/rokko
!
! Distributed under the Boost Software License, Version 1.0. (See accompanying
! file LICENSE_1_0.txt or copy at http://www.boost.org/license_1_0.txt)
!
!*****************************************************************************

program frank_matrix
  use rokko
  implicit none
  integer :: dim
  type(rokko_serial_dense_ev) :: solver
  type(rokko_localized_matrix) :: mat, Z
  type(rokko_localized_vector) :: w
  type(rokko_parameters) :: params, params_out
  character(len=20) :: library, routine
  character(len=20) :: solver_name, tmp_str
  integer arg_len, status

  integer :: provided, ierr, myrank, nprocs
  integer :: i, info

  if (command_argument_count() >= 1) then
     call get_command_argument(1, solver_name, arg_len, status)
  else
     call rokko_serial_dense_ev_default_solver(solver_name)
  endif
  call rokko_split_solver_name(solver_name, library, routine)

  if (command_argument_count() == 2) then  
     call get_command_argument(2, tmp_str, arg_len, status)
     read(tmp_str, *) dim
  else
     dim = 10
  endif

  print *,"library = ", library
  print *,"routine = ", routine
  print *,"dimension = ", dim

  call construct(solver, library)

  call construct(mat, dim, dim, rokko_matrix_col_major)
  call construct(Z, dim, dim, rokko_matrix_col_major)
  call construct(w, dim)

  ! generate frank matrix
  call frank_matrix_generate(mat)
  call print(mat)

  call construct(params)

  call set(params, "routine", "dsyevr")
  call set(params, "verbose", .true.)
  call set(params, "upper_index", 4)
  call set(params, "lower_index", 2)
  call set(params, "upper_value", 3.2d0)
  call set(params, "lower_value", 1.11d0)

!  call diagonalize(solver, mat, w, Z, params, params_out)
!  call diagonalize(solver, mat, w, Z, params)
!  call diagonalize(solver, mat, w, Z)
  call diagonalize(solver, mat, w, params, params_out)
!  call diagonalize(solver, mat, w, params)
!  call diagonalize(solver, mat, w)
  
  call get(params_out, "info", info)

  print *, "info=", info

  write(*,'(A)') "Computed Eigenvalues = "
  do i = 1, dim
     write(*,"(f30.20)") rokko_localized_vector_get(w ,i)
  enddo

  call destruct(mat)
  call destruct(Z)
  call destruct(w)
  call destruct(solver)

end program frank_matrix
