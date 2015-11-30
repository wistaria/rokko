*     This is a modified version of the example on the Netlib,
*     which can be found at
*     http://www.netlib.org/scalapack/examples/sample_pdsyev_call.f

*     The license of the original one is the modified BSD license.
*     http://www.netlib.org/scalapack/LICENSE
*
      PROGRAM SAMPLE_PDSYEV_CALL
*
*  -- ScaLAPACK routine (version 1.2) --
*     University of Tennessee, Knoxville, Oak Ridge National Laboratory,
*     and University of California, Berkeley.
*     May 10, 1996
*
*     This routine contains a sample call to PDSYEV.
*     When compiled and run, it produces output which can be
*     pasted directly into matlab.
*
*     .. Parameters ..
      INTEGER            N
      character(len=10) :: tmp_str
      integer arg_len, status
*     ..
*     .. Local Scalars ..
      INTEGER            IERR
      INTEGER            LWORK, LDA
      INTEGER            CONTEXT, I, IAM, INFO, MYCOL, MYROW, MB, NB,
     $                   NPCOL, NPROCS, NPROW
      DOUBLE PRECISION   INIT_TICK, GEN_TICK, DIAG_TICK, END_TICK
*     ..
*     .. Local Arrays ..
      INTEGER            DESC( 50 )
      real(8), pointer :: A(:,:),W(:),Z(:,:)

      DOUBLE PRECISION   TMP_WORK(1)
      INTEGER            TMP_IWORK(1)
      DOUBLE PRECISION, allocatable :: WORK(:)
      INTEGER, allocatable :: IWORK(:)
*     ..
*     .. External Subroutines ..
      EXTERNAL           BLACS_EXIT, BLACS_GET, BLACS_GRIDEXIT,
     $                   BLACS_GRIDINFO, BLACS_GRIDINIT, BLACS_PINFO,
     $                   BLACS_SETUP, DESCINIT, PDLAMODHILB, PDLAPRNT,
     $                   PDSYEV
*     ..
*     .. Executable Statements ..
*
*
*     Set up the problem
*
      include 'mpif.h'

      !call mpi_init(ierr)      
      call mpi_init_thread( MPI_THREAD_MULTIPLE, i, ierr )
      if (command_argument_count().eq.1) then
      call get_command_argument(1, tmp_str, arg_len, status)
      read(tmp_str, *) n
      else
      write(*,'(A)') "Error: eigen_exa dimension"
      stop
      endif
      INIT_TICK = MPI_WTIME()
      call MPI_Comm_Size(mpi_comm_world,nprocs,ierr)
      NPROW = INT(SQRT(NPROCS + 0.5))
      DO WHILE ( (NPROW .ne. 1) .AND. (MOD(NPROCS,NPROW).ne.0) )
         NPROW = NPROW - 1
      ENDDO
      NPCOL = NPROCS / NPROW
*     
*     Initialize a single BLACS context
*
      CALL BLACS_GET( -1, 0, CONTEXT )
      CALL BLACS_GRIDINIT( CONTEXT, 'R', NPROW, NPCOL )
      CALL BLACS_GRIDINFO( CONTEXT, NPROW, NPCOL, MYROW, MYCOL )
*     
*     These are basic array descriptors
*
      GEN_TICK = MPI_WTIME()
      MB = N / NPROW
      NB = N / NPCOL
      LDA = N
      allocate( a(N, N), z(N, N), w(N))
      CALL DESCINIT( DESC, N, N, MB, NB, 0, 0, CONTEXT, LDA, INFO )
      CALL PDLAMODHILB( N, A, 1, 1, DESC, INFO )
*      CALL PDLAPRNT( N, N, A, 1, 1, DESC, 0, 0, 'A', 6, PRNWORK )
*     
*     Ask PDSYEV to compute the entire eigendecomposition
*
      DIAG_TICK = MPI_WTIME()
      CALL PDSYEVD( 'V', 'U', N, A, 1, 1, DESC, W, Z, 1, 1,
     $             DESC, TMP_WORK, -1, TMP_IWORK, -1, INFO )
      LWORK = INT(TMP_WORK(1))
      allocate ( WORK(LWORK) )
      LIWORK = TMP_IWORK(1)
      allocate ( IWORK(LIWORK) )
      CALL PDSYEVD( 'V', 'U', N, A, 1, 1, DESC, W, Z, 1, 1,
     $             DESC, WORK, LWORK, IWORK, LIWORK, INFO )
      END_TICK = MPI_WTIME()
*     
*     Print out the eigenvalues and eigenvectors
*
      IF( MYROW.EQ.0 .AND. MYCOL.EQ.0 ) THEN
         write(*,'(" Eigenvalues:",5f10.5)') W(1:N)
         write(*,*) "init_time = ", GEN_TICK - INIT_TICK
         write(*,*) "gen_time = ", DIAG_TICK - GEN_TICK
         write(*,*) "diag_time = ", END_TICK - DIAG_TICK
      END IF

      CALL BLACS_GRIDEXIT( CONTEXT )
*      CALL BLACS_EXIT( 0 )
*
      call mpi_finalize(ierr)
      STOP
      END
*
      SUBROUTINE PDLAMODHILB( N, A, IA, JA, DESCA, INFO )
*
*  -- ScaLAPACK routine (version 1.2) --
*     University of Tennessee, Knoxville, Oak Ridge National Laboratory,
*     and University of California, Berkeley.
*     May 10, 1996
*
*
*
*
*     .. Parameters ..
      INTEGER            BLOCK_CYCLIC_2D, DLEN_, DT_, CTXT_, M_, N_,
     $                   MB_, NB_, RSRC_, CSRC_, LLD_
      PARAMETER          ( BLOCK_CYCLIC_2D = 1, DLEN_ = 9, DT_ = 1,
     $                   CTXT_ = 2, M_ = 3, N_ = 4, MB_ = 5, NB_ = 6,
     $                   RSRC_ = 7, CSRC_ = 8, LLD_ = 9 )
      DOUBLE PRECISION   ONE
      PARAMETER          ( ONE = 1.0D+0 )
*     ..
*     .. Scalar Arguments ..
      INTEGER            IA, INFO, JA, N
*     ..
*     .. Array Arguments ..
      INTEGER            DESCA( * )
      DOUBLE PRECISION   A( * )
*     ..
*     .. Local Scalars ..
      INTEGER            I, J, MYCOL, MYROW, NPCOL, NPROW
*     ..
*     .. External Subroutines ..
      EXTERNAL           BLACS_GRIDINFO, PDELSET
*     ..
*     .. Intrinsic Functions ..
      INTRINSIC          DBLE
*     ..
*     .. Executable Statements ..
*

*       This is just to keep ftnchek happy
*      IF( BLOCK_CYCLIC_2D*CSRC_*CTXT_*DLEN_*DT_*LLD_*MB_*M_*NB_*N_*
*     $    RSRC_.LT.0 )RETURN
*
      INFO = 0
*
*      CALL BLACS_GRIDINFO( DESCA( CTXT_ ), NPROW, NPCOL, MYROW, MYCOL )
*
*
      IF( IA.NE.1 ) THEN
         INFO = -3
      ELSE IF( JA.NE.1 ) THEN
         INFO = -4
      END IF
*
*     Create Frank matrix
      DO 20 J = 1, N
         DO 10 I = 1, N
            CALL PDELSET( A, I, J, DESCA, dble(N - MAX(I,J) + 1))
   10    CONTINUE
   20 CONTINUE

*     
*
      RETURN
*
*     End of PDLAMODHLIB
*
      END
