/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2014 Rokko Developers https://github.com/t-sakashita/rokko
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#ifndef ROKKO_ANASAZI_CORE_HPP
#define ROKKO_ANASAZI_CORE_HPP

#include <rokko/distributed_vector.hpp>
#include <rokko/distributed_mfree.hpp>
#include <rokko/anasazi/mapping_1d.hpp>
#include <rokko/anasazi/distributed_crs_matrix.hpp>
#include <rokko/utility/timer.hpp>

#include <AnasaziEpetraAdapter.hpp>
#include <Epetra_CrsMatrix.h>
#include <Epetra_MpiComm.h>
#include <Epetra_Map.h>

#include <AnasaziBasicEigenproblem.hpp>
#include <AnasaziBlockKrylovSchurSolMgr.hpp>
#include <Teuchos_RCPDecl.hpp>

namespace rokko {

namespace anasazi {

class anasazi_mfree_operator : public Epetra_Operator {
public:
  anasazi_mfree_operator(rokko::distributed_mfree* const op, mapping_1d const* map) :
    op_(op), ep_map(map->get_epetra_map()), ep_comm(map->get_epetra_comm()) {}
  ~anasazi_mfree_operator() {};
  virtual int SetUseTranspose(bool UseTranspose) { return 0; };
  virtual int Apply(const Epetra_MultiVector& X, Epetra_MultiVector& Y) const {
    const int numvectors = X.NumVectors();
    Y.PutScalar(0);
    for (int i=0; i<numvectors; ++i) {
      const double* x = X[i];
      double* y = Y[i];
      op_->multiply(x, y);
    }
    return 0;
  }
  virtual int ApplyInverse(const Epetra_MultiVector& X, Epetra_MultiVector& Y) const { return 0; }
  virtual double NormInf() const { return 0; }
  virtual const char * Label() const { return "Anasazi matrix_free"; }
  virtual bool UseTranspose() const { return false; }
  virtual bool HasNormInf() const  { return false; }
  virtual const Epetra_Comm & Comm() const { return ep_comm; }
  virtual const Epetra_Map & OperatorDomainMap() const { return ep_map; }
  virtual const Epetra_Map & OperatorRangeMap() const { return ep_map; }
private:
  distributed_mfree* op_;
  MPI_Comm comm_;
  mutable std::vector<double> buffer_;
  int L_;
  std::vector<std::pair<int, int> > lattice_;
  Epetra_MpiComm ep_comm;
  Epetra_Map ep_map;
};

class solver {
public:
  typedef Anasazi::BasicEigenproblem<double, Epetra_MultiVector, Epetra_Operator> eigenproblem_t;
  typedef Anasazi::BlockKrylovSchurSolMgr<double, Epetra_MultiVector, Epetra_Operator>
    solvermanager_t;
  solver() {}
  ~solver() {}
  void initialize(int& argc, char**& argv) {}
  void finalize() {}
  void diagonalize(rokko::distributed_crs_matrix& mat, int num_evals, int block_size,
    int max_iters, double tol, timer& timer) {
    timer.start(timer_id::diagonalize_initialize);
    map_ = new mapping_1d(mat.get_dim());
    multivector_ = Teuchos::rcp(new Epetra_MultiVector(map_->get_epetra_map(), block_size));
    multivector_->Random();
    problem_ = Teuchos::rcp(new eigenproblem_t(reinterpret_cast<anasazi::distributed_crs_matrix*>(
      mat.get_matrix())->get_matrix(), multivector_));
    problem_->setHermitian(true);
    problem_->setNEV(num_evals);
    problem_->setProblem();
    Teuchos::ParameterList pl;
    pl.set("Which", "SR");
    pl.set("Block Size", block_size);
    pl.set("Maximum Iterations", max_iters);
    pl.set("Convergence Tolerance", tol);
    solvermanager_t solvermanager(problem_, pl);
    bool boolret = problem_->setProblem();
    if (boolret != true) {
      std::cout << "setProblem()_error" << std::endl;
    }
    timer.stop(timer_id::diagonalize_initialize);
    timer.start(timer_id::diagonalize_diagonalize);
    Anasazi::ReturnType returnCode = solvermanager.solve();
    if (returnCode == Anasazi::Unconverged) {
      std::cout << "solvermanager.solve()_error" << std::endl;
    }
    timer.stop(timer_id::diagonalize_diagonalize);
    timer.start(timer_id::diagonalize_finalize);
    num_conv_ = problem_->getSolution().numVecs;
    timer.stop(timer_id::diagonalize_finalize);
  }

  void diagonalize(rokko::distributed_mfree& mat_in, int num_evals, int block_size, int max_iters,
    double tol, timer& timer) {
    timer.start(timer_id::diagonalize_initialize);
    rokko::distributed_mfree* mat = &mat_in;
    map_ = new mapping_1d(mat->get_dim());
    Teuchos::RCP<anasazi_mfree_operator> anasazi_op_ =
      Teuchos::rcp(new anasazi_mfree_operator(mat, map_));
    multivector_ = Teuchos::rcp(new Epetra_MultiVector(map_->get_epetra_map(), block_size));
    multivector_->Random();
    problem_ = Teuchos::rcp(new eigenproblem_t(anasazi_op_, multivector_));
    problem_->setHermitian(true);
    problem_->setNEV(num_evals);
    problem_->setProblem();
    Teuchos::ParameterList pl;
    pl.set("Which", "SR");
    pl.set("Block Size", block_size);
    pl.set("Maximum Iterations", max_iters);
    pl.set("Convergence Tolerance", tol);
    solvermanager_t solvermanager(problem_, pl);
    bool boolret = problem_->setProblem();
    if (boolret != true) {
      std::cout << "setProblem()_error" << std::endl;
    }
    timer.stop(timer_id::diagonalize_initialize);
    timer.start(timer_id::diagonalize_diagonalize);
    Anasazi::ReturnType returnCode = solvermanager.solve();
    if (returnCode == Anasazi::Unconverged) {
      std::cout << "solvermanager.solve()_error" << std::endl;
    }
    timer.stop(timer_id::diagonalize_diagonalize);
    timer.start(timer_id::diagonalize_finalize);
    num_conv_ = problem_->getSolution().numVecs;
    timer.stop(timer_id::diagonalize_finalize);
  }

  rokko::detail::distributed_crs_matrix_base* create_distributed_crs_matrix(int row_dim,
    int col_dim) {
    return new anasazi::distributed_crs_matrix(row_dim, col_dim);
  }

  double eigenvalue(int i) const { return problem_->getSolution().Evals[i].realpart; }

  void eigenvector(int k, std::vector<double>& vec) const {
    if (vec.size() < map_->get_num_local_rows()) vec.resize(map_->get_num_local_rows());
    eigenvector(k, &(vec[0]));
  }
  void eigenvector(int k, double* vec) const {
    double* vec_pt = (*problem_->getSolution().Evecs)[k];
    std::copy(vec_pt, vec_pt + map_->get_num_local_rows(), vec);
  }
  void eigenvector(int k, distributed_vector& vec) const {
    vec.initialize(map_->get_dim(), map_->get_epetra_map().MinMyGID(),
                   map_->get_epetra_map().MaxMyGID() + 1);
    eigenvector(k, vec.get_storage());
  }

  int num_conv() const { return num_conv_; }

private:
  mapping_1d* map_;
  Teuchos::RCP<Epetra_MultiVector> multivector_;
  Teuchos::RCP<eigenproblem_t> problem_;
  //std::vector<Anasazi::Value<double> > evals_;
  int num_conv_;
};

} // namespace anasazi

} // namespace rokko

#endif // ROKKO_ANASAZI_CORE_HPP
