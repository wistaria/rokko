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
  integer :: provided, ierr, myrank, nprocs

  integer :: L, k
  integer :: dim
  integer, allocatable, dimension(:) :: lattice_first, lattice_second
  double precision :: diag
  integer :: i, j, m1, m2, m3, count
  integer :: row, start_row, end_row
  integer(c_int), allocatable, dimension(:) :: cols
  double precision, allocatable, dimension(:) :: values

  double precision :: eig_val
  double precision, allocatable, dimension(:) :: eig_vec

!  integer(4), allocatable, dimension(:) :: cols
!  double precision, allocatable, dimension(:) :: values

  type(rokko_parallel_sparse_solver) :: solver
  type(rokko_distributed_crs_matrix) :: mat
  character(len=100) :: solver_name, tmp_str
  integer :: arg_len, status
  integer :: num_evals, block_size, max_iters
  integer :: num_local_rows, num_conv
  double precision :: tol

  call MPI_init_thread(MPI_THREAD_MULTIPLE, provided, ierr)
  call MPI_comm_rank(MPI_COMM_WORLD, myrank, ierr)
  call MPI_comm_size(MPI_COMM_WORLD, nprocs, ierr)

  solver_name = "anasazi"

  L = 8
  dim = ishft(1,L)
  allocate( lattice_first(L) )
  allocate( lattice_second(L) )
  do k = 1, L
     lattice_first(k) = k - 1
     lattice_second(k) = mod(k, L)
  end do

  if (myrank == 0) then
     write(*,*) "solver name = ", trim(solver_name)
     write(*,*) "matrix dimension = ", dim
  endif

  call rokko_parallel_sparse_solver_construct(solver, solver_name)

  call rokko_distributed_crs_matrix_construct(mat, dim, dim, solver)

  start_row = rokko_distributed_crs_matrix_start_row(mat);
  end_row = rokko_distributed_crs_matrix_end_row(mat);

  allocate( cols(dim) )
  allocate( values(dim) )

  do row = start_row, end_row
     count = 0
     diag = 0
     do k = 1, L
        i = lattice_first(k)
        j = lattice_second(k)
        m1 = ishft(1, i)
        m2 = ishft(1, j)
        m3 = m1 + m2
        if ( (iand(row, m3) == m1) .or. (iand(row, m3) == m2) ) then
           count = count + 1
           cols(count) = ieor(row, m3)
           values(count) = 0.5
           diag = diag - 0.25
        else
           diag = diag + 0.25
        end if
     end do
     count = count + 1
     cols(count) = row
     values(count) = diag
     call rokko_distributed_crs_matrix_insert(mat, row, count, cols, values)
  end do
  call rokko_distributed_crs_matrix_complete(mat)
!  call rokko_distributed_crs_matrix_print(mat)

  num_evals = 10
  block_size = 5
  max_iters = 500
  tol = 1.0e-8
  call rokko_parallel_sparse_solver_diagonalize_distributed_crs_matrix(solver, mat, num_evals, block_size, max_iters, tol)

  num_conv = rokko_parallel_sparse_solver_num_conv(solver);
  eig_val = rokko_parallel_sparse_solver_eigenvalue(solver, 0);
  num_local_rows = rokko_distributed_crs_matrix_num_local_rows(mat);
  print*, "num_local_rows=", num_local_rows
  allocate( eig_vec(num_local_rows) )
  call rokko_parallel_sparse_solver_eigenvector(solver, 0, eig_vec)

  if (myrank.eq.0) then
     print*, "number of converged eigenpairs=", num_conv
     print*, "Computed Eigenvalues = ", eig_val
     print*, "Computed Eigenvector = "
     print '(8f10.4)', eig_vec
  endif

  call rokko_distributed_crs_matrix_destruct(mat)
  call rokko_parallel_sparse_solver_destruct(solver)

  call MPI_finalize(ierr)
end program frank_matrix
