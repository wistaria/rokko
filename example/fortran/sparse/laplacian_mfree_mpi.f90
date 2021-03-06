module laplacian
  use mpi
  use rokko
  use, intrinsic :: iso_c_binding
  implicit none
  integer :: comm
  integer :: private, nprocs, myrank
  integer(c_int), private :: dim, local_offset, num_local_rows
  integer(c_int), private :: start_row, end_row
  integer(c_int), private :: start_k, end_k
  logical, private :: is_first_proc, is_last_proc
  double precision, private :: buf_p = 0, buf_m = 0
  integer, private :: status_m(mpi_status_size), status_p(mpi_status_size)
contains
  subroutine initialize (mat, dim_in)
    type(rokko_distributed_mfree), intent(inout) :: mat
    integer(c_int), intent(in) :: dim_in
    integer :: ierr
    integer :: tmp, rem

    comm = mpi_comm_world
    call mpi_comm_rank(comm, myrank, ierr)
    call mpi_comm_size(comm, nprocs, ierr)
    dim = dim_in
    tmp = dim / nprocs
    rem = mod(dim, nprocs)
    num_local_rows = (dim + nprocs - myrank - 1) / nprocs
    start_row = tmp * myrank + min(rem, myrank) + 1  ! extra plus 1
    end_row = start_row + num_local_rows - 1 ! extra plus 1
    
    if (start_row == 1) then
       is_first_proc = .true.
    else
       is_first_proc = .false.
    endif
 
    if (end_row == dim) then
       is_last_proc = .true.
    else
       is_last_proc = .false.
    endif

    end_k = num_local_rows
    !print*, "myrank=", myrank, "start_row=", start_row, "end_row=", end_row,&
    !     "is_first_proc", is_first_proc, " is_last_proc", is_last_proc
    !print*, "myrank=", myrank, "num_local_rows=", num_local_rows
    call rokko_distributed_mfree_construct(mat, multiply, dim, num_local_rows)
  end subroutine initialize

  integer function get_num_local_rows ()
    get_num_local_rows = num_local_rows
  end function get_num_local_rows
  
  ! subroutine passed to c function.
  ! it must be interoperable!
  subroutine multiply (n, x, y) bind(c)
    implicit none
    integer(c_int), intent(in), value :: n
    double precision, intent(in) :: x(n)
    double precision, intent(out) :: y(n)
    integer :: ierr
    integer :: k

    if (num_local_rows == 0) then
       return
    endif
    
    if (.not.(is_first_proc) .and. (nprocs /= 1)) then
       !print*, "recv myrank=", myrank
       call mpi_send(x(1), 1, mpi_double_precision, myrank-1, 0, comm, ierr)
       call mpi_recv(buf_m, 1, mpi_double_precision, myrank-1, 0, comm, status_m, ierr)
    endif
    
    if (.not.(is_last_proc) .and. (nprocs /= 1)) then
       !print*, "send myrank=", myrank
       call mpi_recv(buf_p, 1, mpi_double_precision, myrank+1, 0, comm, status_p, ierr)
       call mpi_send(x(end_k), 1, mpi_double_precision, myrank+1, 0, comm, ierr)
    endif
    
    if (is_first_proc) then
       if (num_local_rows /= 1) then
          y(1) = x(1) - x(2)
          if (nprocs /= 1) then
             y(end_k) = - x(end_k - 1) + 2 * x(end_k) - buf_p
          endif
       else 
          y(1) = x(1) - buf_p
       endif
    endif
    
    if (is_last_proc) then
       if (num_local_rows /= 1) then
          if (nprocs /= 1) then
             y(1) = - buf_m + 2 * x(1) - x(2)
          endif
          y(end_k) = 2 * x(end_k) - x(end_k - 1)      
       else
          y(end_k) = 2 * x(end_k) - buf_m
       endif
    endif
    if (.not.(is_first_proc .or. is_last_proc)) then ! neither first or last process
       if (num_local_rows /= 1) then
          y(1) = - buf_m + 2 * x(1) - x(2)
          y(end_k) = - x(end_k - 1) + 2 * x(end_k) - buf_p
       else
          y(1) = - buf_m + 2 * x(1) - buf_p
       endif
    endif
    
    ! from 2 to end
    do k=2, end_k-1
       y(k) = - x(k-1) + 2 * x(k) - x(k+1)
    enddo
  end subroutine multiply
end module laplacian

program main
  use mpi
  use rokko
  use laplacian
  implicit none
  integer :: provided, ierr
  
  double precision :: eig_val
  double precision, allocatable, dimension(:) :: eig_vec

  type(rokko_parallel_sparse_solver) :: solver
  character(len=100) :: solver_name, tmp_str
  integer arg_len, status
  type(rokko_distributed_mfree) :: mat
  integer :: dim, i
  integer :: num_evals, block_size, max_iters
  integer :: num_local_rows, num_conv
  double precision :: tol
  double precision :: x(10), y(10)

  call mpi_init_thread(mpi_thread_multiple, provided, ierr)
  call mpi_comm_rank(comm, myrank, ierr)
  call mpi_comm_size(comm, nprocs, ierr)

  solver_name = "anasazi"  ! default
  dim = 10
  if (command_argument_count().ge.1) then
     call get_command_argument(1, tmp_str, arg_len, status)
     solver_name = trim(tmp_str)
  endif

  if (command_argument_count().eq.2) then
     call get_command_argument(2, tmp_str, arg_len, status)
     read(tmp_str, *) dim
  endif

  if (myrank == 0) then
     write(*,*) "solver name = ", trim(solver_name)
     write(*,*) "matrix dimension = ", dim
  endif

  call rokko_parallel_sparse_solver_construct(solver, solver_name)
  call initialize(mat, dim)

  num_evals = 3
  block_size = 5
  max_iters = 500
  tol = 1.0e-3
  !num_local_rows = get_num_local_rows()
  !do i=1, num_local_rows
  !   x = 0
  !   x(i) = 1
  !   call multiply(num_local_rows, x, y);
  !   print*, "y=", y
  !enddo
  call rokko_parallel_sparse_solver_diagonalize_distributed_mfree(solver, mat, num_evals, block_size, max_iters, tol)

  num_conv = rokko_parallel_sparse_solver_num_conv(solver)
  eig_val = rokko_parallel_sparse_solver_eigenvalue(solver, 0)
  num_local_rows = rokko_distributed_mfree_num_local_rows(mat)
  if (myrank == 0) then
     print*, "eigval=", eig_val
  endif
  call rokko_distributed_mfree_destruct(mat)
  call rokko_parallel_sparse_solver_destruct(solver)

  call mpi_finalize(ierr)
end program main
