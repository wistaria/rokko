#include <mpi.h>

// Eigen3に関するヘッダファイル
#include <Eigen/Dense>

#include <iostream>

using namespace std;

#include <rokko/scalapack/scalapack.hpp>
#include <rokko/scalapack/core.hpp>
#include <rokko/scalapack/grid.hpp>
#include <rokko/scalapack/distributed_matrix.hpp>
#include <rokko/scalapack/diagonalize.hpp>

#include <rokko/pblas/pblas.hpp>

#include <rokko/collective.hpp>
#include <rokko/utility/frank_matrix.hpp>
#include <rokko/utility/sort_eigenpairs.hpp>


#undef __FUNCT__
#define __FUNCT__ "main"
int main (int argc, char *argv[])
{
  typedef rokko::scalapack  solver;
  MPI_Init(&argc, &argv);
  rokko::Initialize<solver>(argc, argv);
  MPI_Comm comm = MPI_COMM_WORLD;
  rokko::grid<solver, rokko::grid_row_major<solver> > g(comm);
  //rokko::grid<solver> g(comm);
  int myrank = g.myrank, nprocs = g.nprocs;

  const int root = 0;
  const int dim = 10;

  //rokko::distributed_matrix<solver> mat(dim, dim, g);
  rokko::distributed_matrix<solver> mat(dim, dim, g);
  rokko::distributed_matrix<solver> Z(dim, dim, g);

  rokko::generate_frank_matrix(mat);
  Eigen::MatrixXd global_mat;
  //Eigen::MatrixXd global_mat(dim, dim);

  //rokko::scatter(frank_mat, mat_global, root);
  rokko::gather(mat, global_mat, root);
  mat.print();

  if (myrank == root)
    cout << "global_mat:" << endl << global_mat << endl;

  Eigen::VectorXd w(dim);

  //rokko::diagonalize<solver, rokko::grid_row_major>(mat, w, Z);
  //rokko::diagonalize<rokko::grid_row_major>(mat, w, Z);

  rokko::diagonalize(mat, w, Z);

  Z.print();
  // gather of eigenvectors
  Eigen::MatrixXd global_eigvec;
  Eigen::MatrixXd eigvec_sorted(dim, dim);
  Eigen::VectorXd eigval_sorted(dim);
  rokko::gather(Z, global_eigvec, root);

  if (myrank == root) {
    cout << "eigvec:" << endl << global_eigvec << endl;
    rokko::sort_eigenpairs(w, global_eigvec, eigval_sorted, eigvec_sorted);
    cout.precision(20);
    //cout << "w=" << endl;
    //for (int i=0; i<dim; ++i) {
    //  cout << w[i] << " ";
    //}
    //cout << endl;
    cout << "Computed Eigenvalues= " << eigval_sorted.transpose() << endl;

    cout.precision(3);
    cout << "Check the orthogonality of Eigenvectors:" << endl
	 << eigvec_sorted * eigvec_sorted.transpose() << endl;   // Is it equal to indentity matrix?

    cout << "residual := A x - lambda x = " << endl
         << global_mat * eigvec_sorted.col(1)  -  eigval_sorted(1) * eigvec_sorted.col(1) << endl;
    cout << "Are all the following values equal to some eigenvalue = " << endl
      << (global_mat * eigvec_sorted.col(0)).array() / eigvec_sorted.col(0).array() << endl;
  }

  /*
  double time;
  if (rank == 0) {
    time = end - start;
    cout << "time=" << time << endl;
    ofs << "time=" << time << endl;
    //cout << "iter=" << iter << endl;
    //ofs << "iter=" << iter << endl;
  }
  */

  MPI_Finalize();
  return 0;
}

