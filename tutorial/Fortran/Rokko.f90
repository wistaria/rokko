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

program frank_matrix
  use MPI
  use rokko
  implicit none
  integer :: dim
  type(rokko_distributed_matrix) :: mat, Z
  type(rokko_grid) :: grid
  type(rokko_parallel_dense_solver) :: solver
  type(rokko_localized_vector) :: w
  character(len=100) :: solver_name, tmp_str
  integer arg_len, status
  integer :: provided, ierr, myrank, nprocs
  integer :: i, j
  double precision value

  call MPI_init_thread(MPI_THREAD_MULTIPLE, provided, ierr)
  call MPI_comm_rank(MPI_COMM_WORLD, myrank, ierr)
  call MPI_comm_size(MPI_COMM_WORLD, nprocs, ierr)

  if (command_argument_count().eq.1) then
     call get_command_argument(1, tmp_str, arg_len, status)
     solver_name = trim(tmp_str)
     !call get_command_argument(2, tmp_str, arg_len, status)
     !read(tmp_str, *) dim
     dim = 4
  else
     write(*,'(A)') "Error: Rokko solver_name"
     stop
  endif

  write(*,*) "solver name = ", trim(solver_name)
  write(*,*) "matrix dimension = ", dim

  call rokko_parallel_dense_solver_construct(solver, solver_name)
  call rokko_grid_construct(grid, MPI_COMM_WORLD, rokko_grid_row_major)

  call rokko_distributed_matrix_construct(mat, dim, dim, grid, solver, rokko_matrix_col_major)
  call rokko_distributed_matrix_construct(Z, dim, dim, grid, solver, rokko_matrix_col_major)
  call rokko_localized_vector_construct(w, dim)

  ! generate frank matrix
  do  i = 0, dim-1
     do j = 0, dim-1
      value = dim - max(i,j)
         call rokko_distributed_matrix_set_global(mat, i, j, value)
     enddo
  enddo

  
  if (myrank.eq.0) then
     write(*,'(A)') "Frank matrix = "
  endif
  call rokko_distributed_matrix_print(mat)

  call MPI_Barrier(MPI_COMM_WORLD, ierr)

  call rokko_parallel_dense_solver_diagonalize_distributed_matrix(solver, mat, w, Z)
  
  call MPI_Barrier(MPI_COMM_WORLD, ierr)

  if (myrank.eq.0) then
     write(*,'(A)') "Computed Eigenvalues = "
     do i = 1, dim
        write(*,"(f30.20)") rokko_localized_vector_get(w ,i)
     enddo
  endif

  if (myrank.eq.0) then
     write(*,'(A)') "Eigenstates = "
  endif

  call MPI_Barrier(MPI_COMM_WORLD, ierr)

  call rokko_distributed_matrix_print(Z)

  call rokko_distributed_matrix_destruct(mat)
  call rokko_distributed_matrix_destruct(Z)
  call rokko_localized_vector_destruct(w)
  call rokko_parallel_dense_solver_destruct(solver)
  call rokko_grid_destruct(grid)

  call MPI_finalize(ierr)
end program frank_matrix
