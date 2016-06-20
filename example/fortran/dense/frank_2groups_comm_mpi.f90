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
  use MPI
  use rokko
  implicit none
  integer :: dim
  type(rokko_distributed_matrix) :: mat, Z
  type(rokko_grid) :: grid
  type(rokko_mapping_bc) :: map
  type(rokko_parallel_dense_ev) :: solver
  type(rokko_localized_vector) :: w
  character(len=20) :: library, routine
  character(len=100) :: library_routine, tmp_str
  integer arg_len, status

  integer :: provided, ierr, myrank, nprocs
  integer :: i
  integer :: comm, even_comm, odd_comm
  integer :: world_group, even_group, odd_group
  integer :: num_even, num_odd
  integer, allocatable,dimension(:) :: even_members, odd_members
    
  call MPI_init_thread(MPI_THREAD_MULTIPLE, provided, ierr)
  call MPI_comm_rank(MPI_COMM_WORLD, myrank, ierr)
  call MPI_comm_size(MPI_COMM_WORLD, nprocs, ierr)

  num_even = (nprocs+1)/2
  num_odd = nprocs - num_even
  allocate( even_members(num_even) )
  allocate( odd_members(num_odd) )
  do i=0, num_even-1
     even_members(i+1) = 2 * i     
  enddo
  do i=0, num_odd-1
     odd_members(i+1) = 2 * i + 1
  end do
  call mpi_comm_group(MPI_COMM_WORLD, world_group, ierr)
  call mpi_group_incl(world_group, num_even, even_members, even_group, ierr)
  call mpi_group_incl(world_group, num_odd, odd_members, odd_group, ierr)
  call mpi_comm_create(MPI_COMM_WORLD, even_group, even_comm, ierr)
  call mpi_comm_create(MPI_COMM_WORLD, odd_group, odd_comm, ierr)
  call mpi_group_free(world_group, ierr)
  call mpi_group_free(even_group, ierr)
  call mpi_group_free(odd_group, ierr)
  
  if ( mod(myrank,2) == 0 ) then
     !comm = even_comm
     call mpi_comm_dup(even_comm, comm, ierr)
     call mpi_comm_free(even_comm, ierr)
  else
     !comm = odd_comm
     call mpi_comm_dup(odd_comm, comm, ierr)     
     call mpi_comm_free(odd_comm, ierr)
  endif

  if (command_argument_count() >= 1) then
     call get_command_argument(1, library_routine, arg_len, status)
  else
     call rokko_parallel_dense_ev_default_solver(library_routine)
  endif
  call rokko_split_solver_name(library_routine, library, routine)

  if (command_argument_count() == 2) then  
     call get_command_argument(2, tmp_str, arg_len, status)
     read(tmp_str, *) dim
  else
     dim = 10
  endif

  print *,"library = ", library
  print *,"routine = ", routine
  print *,"dimension = ", dim

  call rokko_parallel_dense_ev_construct(solver, library)
  call rokko_grid_construct(grid, comm, rokko_grid_row_major)
  call rokko_parallel_dense_ev_default_mapping(solver, dim, grid, map)
  call rokko_distributed_matrix_construct(mat, map)
  call rokko_distributed_matrix_construct(Z, map)
  call rokko_localized_vector_construct(w, dim)

  ! generate frank matrix
  call rokko_frank_matrix_generate_distributed_matrix(mat)
  call rokko_distributed_matrix_print(mat)

  call rokko_parallel_dense_ev_diagonalize(solver, mat, w, Z)

  if (myrank.eq.0) then
     write(*,'(A)') "Computed Eigenvalues = "
     do i = 1, dim
        write(*,"(f30.20)") rokko_localized_vector_get(w, i)
     enddo
  endif

  call rokko_distributed_matrix_destruct(mat)
  call rokko_distributed_matrix_destruct(Z)
  call rokko_localized_vector_destruct(w)
  call rokko_parallel_dense_ev_destruct(solver)
  call rokko_mapping_bc_destruct(map)
  call rokko_grid_destruct(grid)

  call mpi_comm_free(comm, ierr)
  call MPI_finalize(ierr)
end program frank_matrix