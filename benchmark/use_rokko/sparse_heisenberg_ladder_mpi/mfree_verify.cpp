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
#include <rokko/utility/heisenberg_hamiltonian_mpi.hpp>
#include <rokko/utility/lattice.hpp>
#include <rokko/utility/machine_info.hpp>

class heisenberg_op : public rokko::distributed_mfree {
public:
  heisenberg_op(int L, const std::vector<std::pair<int, int>>& lattice)
    : L_(L), lattice_(lattice) {
    comm_ = MPI_COMM_WORLD;
    int size, rank;
    MPI_Comm_size(comm_, &size);
    MPI_Comm_rank(comm_, &rank);
    int n = size;
    int p = -1;
    do {
      n /= 2;
      ++p;
    } while (n > 0);
    dim_ = 1 << L;
    num_local_rows_ = 1 << (L-p);
    local_offset_ = num_local_rows_ * rank;
    buffer_.assign(num_local_rows_, 0);
  }
  ~heisenberg_op() {}

  void multiply(const double* x, double* y) const {
    rokko::heisenberg_hamiltonian::multiply(comm_, L_, lattice_, x, y, buffer_.data());
  }
  int get_dim() const { return dim_; }
  int get_local_offset() const { return local_offset_; }
  int get_num_local_rows() const { return num_local_rows_; }

private:
  MPI_Comm comm_;
  int L_;
  std::vector<std::pair<int, int>> lattice_;
  int dim_, local_offset_, num_local_rows_;
  mutable std::vector<double> buffer_;
};

int main(int argc, char *argv[]) {
  int provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  double init_tick, initend_tick, gen_tick, diag_tick, end_tick;

  std::string name("anasazi");
  if (argc >= 2) name = argv[1];
  int len_ladder = 5;
  if (argc >= 3) len_ladder = boost::lexical_cast<int>(argv[2]);

  const int L = 2 * len_ladder;
  std::vector<std::pair<int, int>> lattice;
  rokko::create_ladder_lattice_1dim(len_ladder, lattice);
  if (rank == 0)
    rokko::print_lattice(lattice);
  const int dim = 1 << L;
  if (rank == 0)
    std::cout << "Eigenvalue decomposition of antiferromagnetic Heisenberg 1D ladder lattice" << std::endl
              << "solver = " << name << std::endl
              << "L = " << L << std::endl
              << "dimension = " << dim << std::endl;

  MPI_Barrier(MPI_COMM_WORLD);
  init_tick = MPI_Wtime();
  rokko::parallel_sparse_ev solver(name);
  MPI_Barrier(MPI_COMM_WORLD);
  initend_tick = MPI_Wtime();
  
  MPI_Barrier(MPI_COMM_WORLD);
  gen_tick = MPI_Wtime();
  heisenberg_op mat(L, lattice);
  
  MPI_Barrier(MPI_COMM_WORLD);
  diag_tick = MPI_Wtime();
  rokko::parameters params;
  //params.set("max_block_size", 5);
  //params.set("max_iters", 500);
  //params.set("conv_tol", 1.0e-8);
  //params.set("conv_tol", 1.0e-12);
  //params.set("num_eigvals", 1)
  params.set("verbose", true);
  rokko::parameters params_out = solver.diagonalize(mat, params);
  MPI_Barrier(MPI_COMM_WORLD);
  end_tick = MPI_Wtime();

  const int num_conv = params_out.get<int>("num_conv");
  if (num_conv == 0) {
    throw std::runtime_error("diagonalize : solver does not converge.");
  }
  std::vector<double> eigvec;
  solver.eigenvector(0, eigvec);
  if (rank == 0) {
    std::cout << "number of converged eigenpairs = " << num_conv << std::endl;
    std::cout << "smallest eigenvalues: ";
    for (int i = 0; i < num_conv; ++i) std::cout << ' ' << solver.eigenvalue(i);
    std::cout << std::endl;
    std::cout << "init_time = " << initend_tick - init_tick << std::endl
              << "gen_time = " << diag_tick - gen_tick << std::endl
              << "diag_time = " << end_tick - diag_tick << std::endl;
    rokko::machine_info();
  }

  solver.finalize();
  MPI_Finalize();
}
