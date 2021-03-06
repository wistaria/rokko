!/*****************************************************************************
!*
!* Rokko: Integrated Interface for libraries of eigenvalue decomposition
!*
!* Copyright (C) 2012-2013 by Tatsuya Sakashita <t-sakashita@issp.u-tokyo.ac.jp>,
!*                            Synge Todo <wistaria@comp-phys.org>,
!*                            Tsuyoshi Okubo <t-okubo@issp.u-tokyo.ac.jp>
!*
!* Distributed under the Boost Software License, Version 1.0. (See accompanying
!* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
!*
!*****************************************************************************/


module frank_mod
  use iso_c_binding
  implicit none
  public frank_matrix_element
  integer(c_int), private :: dim
contains
  double precision function frank_matrix_element(i, j) bind(c)
    integer(c_int), value, intent(in) :: i, j
    frank_matrix_element = dble(dim - max(i, j))
  end function frank_matrix_element
  
  subroutine frank_matrix_set_dimension(dim_in)
    integer(c_int), value, intent(in) :: dim_in
    dim = dim_in
  end subroutine frank_matrix_set_dimension
end module frank_mod

program frank_matrix
  use MPI
  use rokko
  use frank_mod
  implicit none
  integer :: dim
  type(rokko_distributed_matrix) :: mat, Z
  type(rokko_grid) :: grid
  type(rokko_parallel_dense_solver) :: solver
  type(rokko_localized_vector) :: w
  character(len=100) :: solver_name, tmp_str
  integer arg_len, status

  integer :: provided,ierr, myrank, nprocs
  integer :: i
  integer :: m_local, n_local

  call MPI_init_thread(MPI_THREAD_MULTIPLE, provided, ierr)
  call MPI_comm_rank(MPI_COMM_WORLD, myrank, ierr)
  call MPI_comm_size(MPI_COMM_WORLD, nprocs, ierr)

  if (command_argument_count().eq.2) then
     call get_command_argument(1, tmp_str, arg_len, status)
     solver_name = trim(tmp_str)
     call get_command_argument(2, tmp_str, arg_len, status)
     read(tmp_str, *) dim
  else
     write(*,'(A)') "Error: frank_function solver_name dimension"
     stop
  endif

  write(*,*) "solver name = ", trim(solver_name)
  write(*,*) "matrix dimension = ", dim

  call rokko_parallel_dense_solver_construct(solver, solver_name)
  call rokko_grid_construct(grid, MPI_COMM_WORLD, rokko_grid_row_major)

  call rokko_distributed_matrix_construct(mat, dim, dim, grid, solver, rokko_matrix_col_major)
  call rokko_distributed_matrix_construct(Z, dim, dim, grid, solver, rokko_matrix_col_major)
  call rokko_localized_vector_construct(w, dim)

  ! generate frank matrix from frank_matrix_element function
  call frank_matrix_set_dimension(dim)
  call rokko_distributed_matrix_generate_function_f(mat, frank_matrix_element)
  call rokko_distributed_matrix_print(mat)

  call rokko_parallel_dense_solver_diagonalize_distributed_matrix(solver, mat, w, Z)

  if (myrank.eq.0) then
     write(*,*) "Computed Eigenvalues = "
     do i = 1, dim
        write(*,"(f30.20)") rokko_localized_vector_get(w, i)
     enddo
  endif

  call rokko_distributed_matrix_destruct(mat)
  call rokko_distributed_matrix_destruct(Z)
  call rokko_localized_vector_destruct(w)
  call rokko_parallel_dense_solver_destruct(solver)
  call rokko_grid_destruct(grid)

  call MPI_finalize(ierr)
end program frank_matrix
