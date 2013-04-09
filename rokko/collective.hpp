#include <mpi.h>


class block_cyclic_adaptor
{
public:
  block_cyclic_adaptor(const Distributed_Matrix& mat)
    : m_global(mat.m_global), n_global(mat.n_global), blockrows(mat.mb), blockcols(mat.nb), m_local(mat.m_local), n_local(mat.n_local), myrow(mat.myrow), mycol(mat.mycol), nprow(mat.nprow), npcol(mat.npcol), local_array(mat.array)
  {
    int array_of_psizes[2] = {0, 0};  // initial values should be 0                                                                                                                                   
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Dims_create(numprocs, 2, array_of_psizes);  // 2次元グリッド                                                                                                                          
    int periodic[2] = {0, 0};

    array_of_psizes[0] = nprow;
    array_of_psizes[1] = npcol;
    MPI_Cart_create(MPI_COMM_WORLD, 2, array_of_psizes, periodic, false, &cart_comm);

    MPI_Comm_rank(cart_comm, &myrank_cart);
    MPI_Comm_size(cart_comm, &numprocs_cart);
    MPI_Cart_coords(cart_comm, myrank_cart, 2, coords);
    myrow = coords[0];  mycol = coords[1];

    calculate_local_matrix_size(myrow, mycol, local_num_block_rows, local_num_block_cols, local_matrix_rows, local_matrix_cols, local_rest_block_rows, local_rest_block_cols);
    cout << "local_num_block_rows=" << local_num_block_rows << "  local_num_block_cols=" << local_num_block_cols << endl;
    create_struct();
    create_global_matrix();
  } 

  /*
  ~block_cyclic_adaptor()
  {
    cout << "Destructor ~block_cyclic_adaptor()" << endl;
    delete[] global_array_type;
    global_array_type = NULL;
    delete[] local_array_type;
    local_array_type = NULL;

    if (myrank_cart == 0) {
      delete[] global_array;
      global_array = NULL;
    }
  }
  */

  void create_struct();
  int gather();
  int scatter();

  void create_global_matrix();
  void calculate_local_matrix_size(const int& proc_row, const int& proc_col, int& local_num_block_rows, int& local_num_block_cols, int& local_matrix_rows, int& local_matrix_cols, int& local_rest_block_rows, int& local_rest_block_cols);
  void print_matrix();

  // variables of class Distributed_Matrix
  int m_global, n_global;
  int mb, nb;
  int m_local, n_local;
  // variables of class Grid                                                                                                                                                                
  //int myrank, nprocs;
  int ictxt;
  //int nprow, npcol;

  int numprocs;
  int coords[2];

  int myrank_cart, numprocs_cart;

  MPI_Status  status;
  int ierr;

  double* global_array;  // グローバル行列
  double* local_array;  // ローカル行列

  int nprow, npcol;
  int myrow, mycol;
  int blockrows, blockcols;
  int local_matrix_rows, local_matrix_cols;
  int local_num_block_rows, local_num_block_cols;

private:
  int local_rest_block_rows, local_rest_block_cols;
  int count_max;

  MPI_Comm cart_comm;
  MPI_Datatype* global_array_type;
  MPI_Datatype* local_array_type;
};

// 送信・受信のデータ型の構造体の作成
void block_cyclic_adaptor::create_struct()
{
  cout << "numprocs_cart=" << numprocs_cart << endl;
  global_array_type = new MPI_Datatype[numprocs_cart];
  local_array_type = new MPI_Datatype[numprocs_cart];

  const int type_block_rows = ( (m_global + nprow * blockrows - 1) / (nprow * blockrows) ) * blockrows;   // 切り上げ
  const int type_block_cols = (n_global + blockcols - 1) / blockcols;   // 切り上げ
  int count_max = type_block_rows * type_block_cols;
  
  cout << "count_max=" << count_max << endl;
  /*
  int          array_of_blocklengths[count_max];
  MPI_Aint     array_of_displacements[count_max];
  MPI_Datatype array_of_types[count_max];
  */

  int*          array_of_blocklengths = new int[count_max];
  MPI_Aint*     array_of_displacements = new MPI_Aint[count_max];
  MPI_Datatype* array_of_types = new MPI_Datatype[count_max];

  for (int proc = 0; proc < numprocs_cart; ++proc) {
    MPI_Cart_coords(cart_comm, proc, 2, coords);
    int proc_row = coords[0], proc_col = coords[1];
    int proc_num_block_rows, proc_num_block_cols, proc_local_matrix_rows, proc_local_matrix_cols, proc_rest_block_rows, proc_rest_block_cols;
    calculate_local_matrix_size(proc_row, proc_col, proc_num_block_rows, proc_num_block_cols, proc_local_matrix_rows, proc_local_matrix_cols, proc_rest_block_rows, proc_rest_block_cols);

    // 送信データの構造体
    int count = 0;

    for (int i=0; i<proc_num_block_rows; ++i) {
      for (int k=0; k<blockrows; ++k) {
	for (int j=0; j<proc_num_block_cols; ++j) {
	  array_of_blocklengths[count] = blockcols;
	  array_of_displacements[count] = ( ((i*nprow + proc_row)*blockrows+k) * n_global + (j * npcol + proc_col) * blockcols ) * sizeof(double);
	  array_of_types[count] = MPI_DOUBLE;
	  if (myrank_cart == 0)
	    cout << "verify: count=" << count << "  length=" << array_of_blocklengths[count] << "  disp="  << (int)array_of_displacements[count] << endl;
	  count++;
	}
	if (proc_rest_block_cols != 0) {
	  array_of_blocklengths[count] = proc_rest_block_cols;
	  array_of_displacements[count] = ( ((i*nprow + proc_row)*blockrows+k) * n_global + (proc_num_block_cols * npcol + proc_col) * blockcols ) * sizeof(double);
          if (myrank_cart == 0)                                                                                                                                        
	    cout << "amari: count=" << count << "  length=" << array_of_blocklengths[count] << "  disp="  << array_of_displacements[count] << endl;
	  array_of_types[count] = MPI_DOUBLE;
	  count++;
	}
      }
    }
    //cout << "before amari: count=" << count << endl;
    // 行のあまり
    for (int k=0; k<proc_rest_block_rows; ++k) {
      cout << "ddddddddddddddddddddddddddddddd" << endl;
      for (int j=0; j<proc_num_block_cols; ++j) {
	array_of_blocklengths[count] = blockcols;
	array_of_displacements[count] = ( ((proc_num_block_rows * nprow + proc_row)*blockrows+k) * n_global + (j * npcol + proc_col) * blockcols ) * sizeof(double);
	array_of_types[count] = MPI_DOUBLE;
	count++;
      }
      if (proc_rest_block_cols != 0) {
	cout << "rest: count=" << count << "  disp="  << array_of_displacements[count] << endl;
	array_of_blocklengths[count] = proc_rest_block_cols;
	array_of_displacements[count] = ( ((proc_num_block_rows * nprow + proc_row)*blockrows+k) * n_global + (proc_num_block_cols * npcol + proc_col) * blockcols ) * sizeof(double);
	if (myrank_cart == 0)
	  cout << "rest_amari: count=" << count << "  disp="  << array_of_displacements[count] << endl;
	array_of_types[count] = MPI_DOUBLE;
	count++;
      }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    //if (myrank_cart == 0)
      //cout << "myrank" << myrank_cart << "  imano_count=" << count << "   num_block_cols_r=" << num_block_cols_r << "  num_block_rows=" << num_block_rows << "  num_block_cols=" << num_block_cols << endl;
    MPI_Barrier(MPI_COMM_WORLD);

    //cout << "blockcols=" << blockcols << endl;
    // 作成した送信データTypeの表示
    if (myrank_cart == 0) {
      for (int i=0; i<count; ++i) {
	//cout << "proc=" << proc << "  count=" << count << " lengthhhhh=" << array_of_blocklengths[i] << endl;
	printf("send Type proc=%d count=%d:  length=%3d  disp=%3d\n", proc, i, array_of_blocklengths[i], (int)array_of_displacements[i]);
      }
    }

    MPI_Type_struct(count, array_of_blocklengths, array_of_displacements, array_of_types, &global_array_type[proc]);
    MPI_Type_commit(&global_array_type[proc]);
 
    // 受信データの構造体
    count = 0;
    for (int i=0; i<proc_num_block_rows; ++i) {
      for (int k=0; k<blockrows; ++k) {
	for (int j=0; j<proc_num_block_cols; ++j) {
	  array_of_blocklengths[count] = blockcols;
	  array_of_displacements[count] = ( (i*blockrows+k) * proc_local_matrix_cols + j * blockcols ) * sizeof(double);
	  array_of_types[count] = MPI_DOUBLE;
	  count++;
	}
	if (proc_rest_block_cols != 0) {
	  array_of_blocklengths[count] = proc_rest_block_cols;
	  array_of_displacements[count] = ( (i*blockrows+k) * proc_local_matrix_cols + proc_num_block_cols * blockcols ) * sizeof(double);
	  array_of_types[count] = MPI_DOUBLE;
	  count++;
	}
      }
    }
    // 行のあまり
    for (int k=0; k<proc_rest_block_rows; ++k) {
      for (int j=0; j<proc_num_block_cols; ++j) {
	array_of_blocklengths[count] = blockcols;
	array_of_displacements[count] = ( (proc_num_block_rows * blockrows+k) * proc_local_matrix_cols + j * blockcols ) * sizeof(double);
	array_of_types[count] = MPI_DOUBLE;
	++count;
      }
      if (proc_rest_block_cols != 0) {
	array_of_blocklengths[count] = proc_rest_block_cols;
	array_of_displacements[count] = ( (proc_num_block_rows * blockrows+k) * proc_local_matrix_cols + proc_num_block_cols * blockcols ) * sizeof(double);
	array_of_types[count] = MPI_DOUBLE;
	++count;
      }
    }
    //cout << "count=" << count << endl;
    
    MPI_Barrier(MPI_COMM_WORLD);
  
    // 作成した受信データTypeの表示
    if (myrank_cart == 0) {
      for (int i=0; i<count; ++i) {
	printf("recv Type proc=%d count=%d:  length=%3d  disp=%3d\n", proc, i, array_of_blocklengths[i], (int)array_of_displacements[i]);
      }
    }
    
    MPI_Type_struct(count, array_of_blocklengths, array_of_displacements, array_of_types, &local_array_type[proc]);
    MPI_Type_commit(&local_array_type[proc]);
  } // end of "for (int proc = 0; proc < numprocs_cart; ++proc)"

  //delete[] array_of_blocklengths;
  //array_of_blocklengths = NULL;
  //delete[] array_of_displacements;
  //array_of_displacements = NULL;
  //delete[] array_of_types;
  //array_of_types = NULL;
}

//　送受信(Scatter)関数
int block_cyclic_adaptor::scatter()
{
  int sendcount, recvcount;

  int rank_send = 0;  // プロセスrank_sendから分散

  for (int proc = 0; proc < numprocs_cart; ++proc) {
    //cout << endl << endl << "##myrank_cart=" << myrank_cart << "  proc_row=" << proc_row << "  proc_col=" << proc_col << endl;
    
    int rank_recv = proc;
    
    int dest, source;
    if (myrank_cart == rank_recv) {
      source = rank_send;
      recvcount = 1;
    } 
    else {  // 自プロセスが受信者ではない場合(送信者である場合を含む)
      source = MPI_PROC_NULL;
      recvcount = 0;
    }
    if (myrank_cart == rank_send)  {
      dest = rank_recv;
      sendcount = 1;
    }
    else {  // 自プロセスが送信者ではない場合(受信者である場合を含む)
      dest = MPI_PROC_NULL;
      sendcount = 0;
    }
    
    ierr = MPI_Sendrecv(global_array, sendcount, global_array_type[proc], dest, 0, local_array, recvcount, local_array_type[proc], source, 0, cart_comm, &status);

    if (ierr != 0) {
      printf("Problem with Sendrecv (Scatter).\nExiting\n");
      MPI_Abort(MPI_COMM_WORLD,77);
      exit(77);
    }
  } // for (int proc = 0; proc < numprocs_cart; ++proc)

  MPI_Barrier(MPI_COMM_WORLD);
  return ierr;
}

// 送受信(gather)ルーチン
int block_cyclic_adaptor::gather()
{
  int rank_recv = 0;  // プロセスrank_recvに集約
  int sendcount, recvcount;

  for (int proc = 0; proc < numprocs_cart; ++proc) {    
    int rank_send = proc; // 全プロセス（ルートプロセスも含む）から集約

    int dest, source;
    if (myrank_cart == rank_recv) {
      source = rank_send;
      recvcount = 1;
    } 
    else {  // 自プロセスが受信者ではない場合(送信者である場合を含む)
      source = MPI_PROC_NULL;
      recvcount = 0;
    }
    if (myrank_cart == rank_send) {
      dest = rank_recv; 
      sendcount = 1;
    }
    else {  // 自プロセスが送信者ではない場合(受信者である場合を含む)
      dest = MPI_PROC_NULL;
      sendcount = 0;
    }
    ierr = MPI_Sendrecv(local_array, sendcount, local_array_type[proc], dest, 0, global_array, recvcount, global_array_type[proc], source, 0, cart_comm, &status);
    //ierr = 0;

    if (ierr != 0) {
      printf("Problem with Sendrecv (Scatter).\nExiting\n");
      MPI_Abort(MPI_COMM_WORLD,78);
      exit(78);
    }
  } // for (int proc = 0; proc < numprocs_cart; ++proc)
  
  MPI_Barrier(MPI_COMM_WORLD);  
  return ierr;
}



// ローカル行列のサイズ，ブロック数の計算
void block_cyclic_adaptor::calculate_local_matrix_size(const int& proc_row, const int& proc_col, int& local_num_block_rows, int& local_num_block_cols, int& local_matrix_rows, int& local_matrix_cols, int& local_rest_block_rows, int& local_rest_block_cols)
{
  int tmp = m_global / blockrows;
  local_num_block_rows = (tmp + nprow-1 - proc_row) / nprow;
  
  tmp = n_global / blockcols;
  local_num_block_cols = (tmp + npcol-1 - proc_col) / npcol;
  int rest_block_row = ((m_global / blockrows) % nprow + 2) % nprow; // 最後のブロックを持つプロセスの次のプロセス
  if (proc_row == rest_block_row)
    local_rest_block_rows = m_global % blockrows;
  else
    local_rest_block_rows = 0;

  int rest_block_col = ((n_global / blockcols) % npcol + 2) % npcol; // 最後のブロックを持つプロセスの次のプロセス
  if (proc_col == rest_block_col)
    local_rest_block_cols = n_global % blockcols;
  else
    local_rest_block_cols = 0;

  local_matrix_rows = local_num_block_rows * blockrows + local_rest_block_rows;
  local_matrix_cols = local_num_block_cols * blockcols + local_rest_block_cols;
}


void block_cyclic_adaptor::print_matrix()
{
  /* each proc prints it's local_array out, in order */
  for (int proc=0; proc<numprocs_cart; ++proc) {
    if (proc == myrank_cart) {
      printf("Rank = %d  myrow=%d mycol=%d\n", myrank_cart, myrow, mycol);
      if (myrank_cart == 0) {
	printf("Global matrix: \n");
	for (int jj=0; jj<n_global; ++jj) {
	  for (int ii=0; ii<m_global; ++ii) {
	    printf("%3.2f ",global_array[jj*n_global+ii]);
	  }
	  printf("\n");
	}
      }
      printf("Local Matrix:\n");
      for (int jj=0; jj<local_matrix_cols; ++jj) {
	for (int ii=0; ii<local_matrix_rows; ++ii) {
	  printf("%3.2f ",local_array[jj*local_matrix_cols+ii]);
	}
	printf("\n");
      }
      printf("\n");
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }
}

class Symmetric_EigenSolver
{
public:
  Symmetric_EigenSolver(Distributed_Matrix& A);
  int compute();

  /*
  ~Symmetric_EigenSolver()
  {
    cout << "Destructor ~Symmetric_EigenSolver()" << endl;
    delete[] w;
    w = NULL;
  }
  */

  int dim;
  Distributed_Matrix& A_;
  double *w;
  Distributed_Matrix Z;
private:
};

Symmetric_EigenSolver::Symmetric_EigenSolver(Distributed_Matrix& A) : Z(A.m_global, A.n_global, A.g), A_(A)
{
  dim = A.m_global;
  w = new double[dim];
  if (w == NULL) {
    cerr << "failed to allocate w." << endl;
    exit(1);
  }
  compute();
}

int Symmetric_EigenSolver::compute()
{
  double* work = new double[1];
  long lwork = -1;
  int info = 0;

  const int ZERO=0, ONE=1;
  
  // work配列のサイズの問い合わせ
  pdsyev_( "V",  "U",  A_.m_global,  A_.array, ONE,  ONE,  A_.desc, w, Z.array, ONE, ONE,
 	   Z.desc, work, lwork, info );
  
  lwork = work[0];
  work = new double [lwork];
  if (work == NULL) {
    cerr << "failed to allocate work. info=" << info << endl;
    return info;
  }
  info = 0;
  
  // 固有値分解
  pdsyev_( "V",  "U",  A_.m_global,  A_.array,  ONE,  ONE,  A_.desc, w, Z.array, ONE, ONE,
	   Z.desc, work, lwork, info );
  
  if (info) {
    cerr << "error at pdsyev function. info=" << info  << endl;
    exit(1);
  }

  return info;
}

void generate_Frank_matrix_local(Distributed_Matrix& A)
{
  for(int local_i=0; local_i<A.m_local; ++local_i) {
    for(int local_j=0; local_j<A.n_local; ++local_j) {
      int global_i = A.translate_l2g_row(local_i);
      int global_j = A.translate_l2g_col(local_j);
      //A.array(H.Set( i, j, n - max(i,j) );
      A.array[local_i * A.n_local + local_j] = A.m_global - max(global_i, global_j);
    }
  }
}


// 固有値ルーチンのテストに使うFrank行列の理論固有値
void exact_eigenvalue_frank(const int ndim, VectorXd& ev1)
{
  double dt = M_PI;   dt /= (2*ndim+1);
  for (int i=0; i<ndim; ++i) ev1[i] = 1 / (2*(1 - cos((2*i+1)*dt)));
}


int main(int argc, char *argv[])
{
  Initialize(argc, argv);
  MPI_Comm comm = (MPI_Comm) MPI_COMM_WORLD;
  Grid g(comm);
  
  int dim = 10;

  Distributed_Matrix A(dim, dim, g);
  generate_Frank_matrix_local(A);
  A.print_matrix();

  /*
  // デバッグ用グローバル行列
  double* A_global = new double[dim*dim];
  if (A_global == NULL) {
    cerr << "failed to allocate A_global." << endl;
    exit(1);
  }
  for (int ii=0; ii<dim * dim; ++ii) {
    global_array[ii] = ii;
  }

  // ローカル行列とグローバル行列が対応しているかを確認
  double value;
  if (g.myrank == 0) cout << "A=" << endl;
  for (int i=0; i<dim; ++i) {
    for (int j=0; j<dim; ++j) {
      pdelget_("A", " ", value, A.array, i + 1, j + 1, A.desc);  // Fortranでは添字が1から始まることに注意
      if (g.myrank == 0)  cout << value << "  ";
    }
     if (g.myrank == 0)  cout << endl;
  }
  */

  Symmetric_EigenSolver Solver(A);

  // scalapack::diagonalize(A, eigvals, Z)
  double* global_array_pointer; //= new double[];
    
  MatrixXd global_matrix;
  const int root = 0;
  gather(Z, global_matrix, root);
  print_matrix(Z);
  if (myrank == root)
    print_matrix(global_matrix);
  }
  // gather(Z)
  //block_cyclic_adaptor Z_adaptor(Solver.Z);
  
  //Z_adaptor.scatter();
  Z_adaptor.gather();
  Z_adaptor.print_matrix();

  Map<RowVectorXd> eigval(Solver.w, dim);
  /*
  if (myrank_cart == 0) {
    global_array = new double[m_global * n_global];
    if (global_array == NULL) {
      cerr << "failed to allocate global_array." << endl;
      //return 1;                                                                                                                                                                          
      exit(1);
    }
  */

  if (Z_adaptor.myrank_cart == 0) {                                                                                           Map<MatrixXd>  eigvec(Z_adaptor.global_array, dim, dim);
    cout << "raw_eigval=" << endl << eigval << endl;
    cout << "forsentences ";
    for (int i=0; i<eigvec.rows(); ++i) {
      for (int j=0; j<eigvec.cols(); ++j) {
	cout << eigvec(i,j) << " ";
      }
      cout << endl;
    }
    cout << "cout matrix";
    cout << eigvec << endl;
  }

  MPI_Barrier(MPI_COMM_WORLD);

  Map<Matrix<double,Dynamic,Dynamic,RowMajor> >  eigvec(Z_adaptor.global_array, dim, dim);

  //cout << "dim=" << dim <<  " m_global=" << Z_adaptor.m_global << " n_global="<< Z_adaptor.n_global << endl;
  //cout <<  " eigvec.rows=" << eigvec.rows() << " eig_vec.cols=" << eigvec.cols() << endl;

  /*
  double value;
  // 固有ベクトルの取り出し
  for (int i=0; i<dim; ++i) {
    eigval(i) = Solver.w[i];
    //for (int j=0; j<dim; ++j) {
    //  pdelget_("A", " ", value, Solver.Z.array, i + 1, j + 1, Solver.Z.desc);  // Fortranは添字が1から始まることに注意
    //  eigvec(i,j) = value;
    //}
  }
  */



  // 固有値の絶対値の降順に固有値(と対応する固有ベクトル)をソート
  // ソート後の固有値の添字をベクトルqに求める．
  double absmax;
  int qq;

  VectorXi q(dim);
  
  VectorXd eigval_sorted(dim);
  cout << "aadim=" << dim << endl;
  MatrixXd eigvec_sorted(dim,dim);
  
  if(Z_adaptor.myrank_cart == 0) {
    // 固有値・固有ベクトルを絶対値の降順にソート
    for (int i=0; i<eigval.size(); ++i) q[i] = i;
    for (int m=0; m<eigval.size(); ++m) {
      absmax = abs(eigval[q[m]]);
      for (int i=m+1; i<eigvec.rows(); ++i) {
	if (absmax < abs(eigval[q[i]])) {
	  absmax = eigval[q[i]];
	  qq = q[m];
	  q[m] = q[i];
	  q[i] = qq;
	}
      }
      eigval_sorted(m) = eigval(q[m]);
      eigvec_sorted.col(m) = eigvec.col(q[m]);    
    }


    cout << "Computed eigenvalues= " << eigval_sorted.transpose() << endl;
    
    cout << "Eigenvector:" << endl << eigvec_sorted << endl << endl;
    cout << "Check the orthogonality of eigenvectors:" << endl
	 << eigvec_sorted * eigvec_sorted.transpose() << endl;   // Is it equal to indentity matrix?
    //cout << "residual := A x - lambda x = " << endl
    //	 << A_global_matrix * eigvec_sorted.col(0)  -  eigval_sorted(0) * eigvec_sorted.col(0) << endl;
    //cout << "Are all the following values equal to some eigenvalue = " << endl
    //	 << (A_global_matrix * eigvec_sorted.col(0)).array() / eigvec_sorted.col(0).array() << endl;
    //cout << "A_global_matrix=" << endl << A_global_matrix << endl; 

  }



  //cout << "myrank=" << g.myrank << "  nprocs=" << g.nprocs << endl;

  //delete[] A_global;

  //hamiltonian.~Hamiltonian();
  //Z_adaptor.~block_cyclic_adaptor();
  //Solver.~Symmetric_EigenSolver();
  //A.~Distributed_Matrix();

  //g.~Grid();

  Finalize();
}

