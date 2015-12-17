/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2012-2015 Rokko Developers https://github.com/t-sakashita/rokko
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#include <rokko/rokko.hpp>
#include <rokko/utility/laplacian_matrix.hpp>

class laplacian_op : public rokko::distributed_mfree {
public:
  laplacian_op(int dim) : dim_(dim) {
    comm_ = MPI_COMM_WORLD;
    MPI_Comm_size(comm_, &nprocs);
    MPI_Comm_rank(comm_, &myrank);

    int tmp = dim_ / nprocs;
    int rem = dim % nprocs;
    num_local_rows_ = (dim + nprocs - myrank - 1) / nprocs;
    start_row_ = tmp * myrank + std::min(rem, myrank);
    end_row_ = start_row_ + num_local_rows_ - 1;

    if (start_row_ == 0)  is_first_proc = true;
    else is_first_proc = false;
    
    if (end_row_ == (dim-1))  is_last_proc = true;
    else is_last_proc = false;

    end_k_ = num_local_rows_ - 1;
  }
  ~laplacian_op() {}

  void multiply(const double* x, double* y) const {
    if (num_local_rows_ == 0) return;
    
    if ((!is_first_proc) && (nprocs != 1)) {
      //std::cout << "recv myrank=" << myrank << std::endl;
      MPI_Send(&x[0], 1, MPI_DOUBLE, myrank-1, 0, comm_);
      MPI_Recv(&buf_m, 1, MPI_DOUBLE, myrank-1, 0, comm_, &status_m);
      //std::cout << "buffff=" << buf << std::endl;
    }

    if ((!is_last_proc) && (nprocs != 1)) {
      //std::cout << "send myrank=" << myrank << std::endl;
      MPI_Recv(&buf_p, 1, MPI_DOUBLE, myrank+1, 0, comm_, &status_p);
      MPI_Send(&x[end_k_], 1, MPI_DOUBLE, myrank+1, 0, comm_);
      //std::cout << "buffff=" << buf2 << std::endl;
    }

    if (is_first_proc) {
      if (num_local_rows_ != 1) {
        y[0] = x[0] - x[1];
        if (nprocs != 1) y[end_k_] = - x[end_k_ - 1] + 2 * x[end_k_] - buf_p;
      }
      else {
        y[0] = x[0] - buf_p;
      }
    }

    if (is_last_proc) {
      if (num_local_rows_ != 1) {
        if (nprocs != 1) y[0] = - buf_m + 2 * x[0] - x[1];
        y[end_k_] = 2 * x[end_k_] - x[end_k_ - 1];
      }
      else {
        y[end_k_] = 2 * x[end_k_] - buf_m;
      }
    }
    if (!(is_first_proc || is_last_proc)) { // neither first or last process
      if (num_local_rows_ != 1) {
        y[0] = - buf_m + 2 * x[0] - x[1];
        y[end_k_] = - x[end_k_ - 1] + 2 * x[end_k_] - buf_p;
      }
      else {
        y[0] = - buf_m + 2 * x[0] - buf_p;
      }
    }
    // from 1 to end-1
    for (int k=1; k<end_k_; ++k) {
      y[k] = - x[k-1] + 2 * x[k] - x[k+1];
    }
  }
  int get_dim() const { return dim_; }
  int get_local_offset() const { return local_offset_; }
  int get_num_local_rows() const { return num_local_rows_; }
  int get_start_row() const { return start_row_; }
  int get_end_row() const { return end_row_; }

private:
  MPI_Comm comm_;
  int nprocs, myrank;
  int dim_, local_offset_, num_local_rows_;
  int start_row_, end_row_;
  int start_k_, end_k_;
  bool is_first_proc, is_last_proc;
  mutable double buf_m, buf_p;
  mutable MPI_Status status_m, status_p;
};

int main(int argc, char *argv[]) {
  int provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  std::vector<std::string> solvers;
  if (argc >= 2) {
    solvers.push_back(argv[1]);
  } else {
    solvers = rokko::parallel_sparse_ev::solvers();
  }

  int dim = (argc >= 3) ? boost::lexical_cast<int>(argv[2]) : 100;

  rokko::parameters params;
  params.set("Block Size", 5);
  params.set("Maximum Iterations", 500);
  params.set("Convergence Tolerance", 1.0e-8);
  params.set("num_eigenvalues", 10);
  BOOST_FOREACH(std::string const& name, solvers) {
    rokko::parallel_sparse_ev solver(name);
    laplacian_op  mat(dim);
    if (rank == 0)
      std::cout << "Eigenvalue decomposition of Laplacian" << std::endl
                << "solver = " << name << std::endl
                << "dimension = " << mat.get_dim() << std::endl;
    
    rokko::parameters info = solver.diagonalize(mat, params);

    int num_conv = info.get<int>("num_conv");
    if (num_conv == 0) MPI_Abort(MPI_COMM_WORLD, -1);
    std::vector<double> eigvec;
    solver.eigenvector(0, eigvec);
    if (rank == 0) {
      std::cout << "number of converged eigenpairs = " << num_conv << std::endl;
      std::cout << "largest eigenvalues:     ";
      for (int i = 0; i < num_conv; ++i) std::cout << solver.eigenvalue(i) << ' ';
      std::cout << std::endl;
      std::cout << "theoretical eigenvalues: ";
      for (int k = dim - 1; k >= dim - num_conv; --k)
        std::cout << rokko::laplacian_matrix::eigenvalue(dim, k) << ' ';
      std::cout << std::endl;
      std::cout << "largest eigenvector: ";
      for (int j = 0; j < eigvec.size(); ++j) std::cout << eigvec[j] << ' ';
      std::cout << std::endl;
    }
    solver.finalize();
  }

  MPI_Finalize();
}
