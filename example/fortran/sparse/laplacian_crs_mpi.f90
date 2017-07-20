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

program heisenberg_crs_mpi
  use rokko
  implicit none
  include 'mpif.h'
  integer :: provided, ierr, myrank, nprocs

  integer :: dim
  integer :: row, start_row, end_row, end_loop_row
  integer, dimension(3) :: cols
  double precision, dimension(3) :: values

  double precision :: eig_val
  double precision, allocatable, dimension(:) :: eig_vec

  type(rokko_parallel_sparse_ev) :: solver
  type(rokko_distributed_crs_matrix) :: mat
  character(len=100) :: library_routine, tmp_str
  character(len=50) :: library, routine
  integer :: arg_len, status
  type(rokko_parameters) :: params, params_out
  integer :: num_local_rows, num_conv

  call MPI_init_thread(MPI_THREAD_MULTIPLE, provided, ierr)
  call MPI_comm_rank(MPI_COMM_WORLD, myrank, ierr)
  call MPI_comm_size(MPI_COMM_WORLD, nprocs, ierr)

  if (command_argument_count() >= 1) then
     call get_command_argument(1, library_routine, arg_len, status)
  else
     call rokko_parallel_sparse_ev_default_solver(library_routine)
  endif
  call rokko_split_solver_name(library_routine, library, routine)
  
  if (command_argument_count() == 2) then  
     call get_command_argument(2, tmp_str, arg_len, status)
     read(tmp_str, *) dim
  else
     dim = 10
  endif
  
  if (myrank == 0) then
     write(*,*) "solver name = ", trim(library)
     write(*,*) "matrix dimension = ", dim
  endif

  call rokko_construct(solver, library)

  call rokko_construct(mat, dim, dim, solver)

  start_row = rokko_distributed_crs_matrix_start_row(mat)
  end_row = rokko_distributed_crs_matrix_end_row(mat)
  if (start_row == 1) then
     values(1) = 1d0;  values(2) = -1d0
     cols(1) = 1;   cols(2) = 2
     start_row = start_row + 1
     call rokko_insert(mat, 1, 2, cols, values)
  endif
  end_loop_row = end_row;
  if (end_row == dim) then
     end_loop_row = end_loop_row - 1
  endif
  values(1) = -1d0;  values(2) = 2d0;  values(3) = -1d0
  do row = start_row, end_loop_row
     cols(1) = row-1;   cols(2) = row;   cols(3) = row+1
     call rokko_insert(mat, row, 3, cols, values)
  enddo
  
  if (end_row == dim) then
     values(1) = -1d0;  values(2) = 2d0
     cols(1) = dim-1;   cols(2) = dim
     call rokko_insert(mat, dim, 2, cols, values)
  endif
  
  call rokko_complete(mat)
!  call rokko_distributed_crs_matrix_print(mat)

  call rokko_construct(params)
  call rokko_set(params, "routine", routine)
  call rokko_set(params, "verbose", .true.)
  call rokko_set(params, "num_evals", 1)
  call rokko_set(params, "block_size", 5)
  call rokko_set(params, "max_iters", 500)
  call rokko_set(params, "conv_tol", 1.0d-8)
  call rokko_diagonalize(solver, mat, params)
  
  num_conv = rokko_parallel_sparse_ev_num_conv(solver)
  if (num_conv >= 1) then
     eig_val = rokko_parallel_sparse_ev_eigenvalue(solver, 0)
     num_local_rows = rokko_distributed_crs_matrix_num_local_rows(mat)
     if (myrank == 0) then
        print *, "eigval=", eig_val
        print*, "num_local_rows=", num_local_rows
     endif
     allocate( eig_vec(num_local_rows) )
     call rokko_eigenvector(solver, 0, eig_vec)
  endif
  
  if (myrank.eq.0) then
     print*, "number of converged eigenpairs=", num_conv
     print*, "Computed Eigenvalues = ", eig_val
     print*, "Computed Eigenvector = "
     print '(8f10.4)', eig_vec
  endif

  call rokko_destruct(mat)
  call rokko_destruct(solver)

  call MPI_finalize(ierr)
end program heisenberg_crs_mpi

