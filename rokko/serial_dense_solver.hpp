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

#ifndef ROKKO_SERIAL_DENSE_SOLVER_HPP
#define ROKKO_SERIAL_DENSE_SOLVER_HPP

#include <string>
#include <rokko/factory.hpp>
#include <rokko/localized_matrix.hpp>
#include <rokko/localized_vector.hpp>
#include <rokko/utility/timer.hpp>
#include <rokko/parameters.hpp>

namespace rokko {

namespace detail {

class sd_solver_base {
public:
  virtual ~sd_solver_base() {}
  virtual void initialize(int& argc, char**& argv) = 0;
  virtual void finalize() = 0;
  // -------------- standard eigenvalue probelm ---------------
  // with parameters, eigenvalues/eigenvectors
  virtual parameters diagonalize(localized_matrix<double, matrix_row_major>& mat,
				 localized_vector<double>& eigvals,
				 localized_matrix<double, matrix_row_major>& eigvecs,
				 parameters const& params) = 0;
  virtual parameters diagonalize(localized_matrix<double, matrix_col_major>& mat,
				 localized_vector<double>& eigvals,
				 localized_matrix<double, matrix_col_major>& eigvecs,
				 parameters const& params) = 0;
  virtual parameters diagonalize(localized_matrix<double, matrix_row_major>& mat,
				 std::vector<double>& eigvals,
				 localized_matrix<double, matrix_row_major>& eigvecs,
				 parameters const& params) = 0;
  virtual parameters diagonalize(localized_matrix<double, matrix_col_major>& mat,
				 std::vector<double>& eigvals,
				 localized_matrix<double, matrix_col_major>& eigvecs,
				 parameters const& params) = 0;
  // with parameters, only eigenvalues
  virtual parameters diagonalize(localized_matrix<double, matrix_row_major>& mat,
				 localized_vector<double>& eigvals,
				 parameters const& params) = 0;
  virtual parameters diagonalize(localized_matrix<double, matrix_col_major>& mat,
				 localized_vector<double>& eigvals,
				 parameters const& params) = 0;
  virtual parameters diagonalize(localized_matrix<double, matrix_row_major>& mat,
				 std::vector<double>& eigvals,
				 parameters const& params) = 0;
  virtual parameters diagonalize(localized_matrix<double, matrix_col_major>& mat,
				 std::vector<double>& eigvals,
				 parameters const& params) = 0;
  // -------------- generalized eigenvalue probelm ---------------
  // with parameters, eigenvalues/eigenvectors
  virtual parameters diagonalize(localized_matrix<double, matrix_row_major>& mata, localized_matrix<double, matrix_row_major>& matb,
				 localized_vector<double>& eigvals,
				 localized_matrix<double, matrix_row_major>& eigvecs,
				 parameters const& params) = 0;
  virtual parameters diagonalize(localized_matrix<double, matrix_col_major>& mata, localized_matrix<double, matrix_col_major>& matb,
				 localized_vector<double>& eigvals,
				 localized_matrix<double, matrix_col_major>& eigvecs,
				 parameters const& params) = 0;
  virtual parameters diagonalize(localized_matrix<double, matrix_row_major>& mata, localized_matrix<double, matrix_row_major>& matb,
				 std::vector<double>& eigvals,
				 localized_matrix<double, matrix_row_major>& eigvecs,
				 parameters const& params) = 0;
  virtual parameters diagonalize(localized_matrix<double, matrix_col_major>& mata, localized_matrix<double, matrix_col_major>& matb,
				 std::vector<double>& eigvals,
				 localized_matrix<double, matrix_col_major>& eigvecs,
				 parameters const& params) = 0;
  // with parameters, only eigenvalues
  virtual parameters diagonalize(localized_matrix<double, matrix_row_major>& mata, localized_matrix<double, matrix_row_major>& matb,
				 localized_vector<double>& eigvals,
				 parameters const& params) = 0;
  virtual parameters diagonalize(localized_matrix<double, matrix_col_major>& mata, localized_matrix<double, matrix_col_major>& matb,
				 localized_vector<double>& eigvals,
				 parameters const& params) = 0;
  virtual parameters diagonalize(localized_matrix<double, matrix_row_major>& mata, localized_matrix<double, matrix_row_major>& matb,
				 std::vector<double>& eigvals,
				 parameters const& params) = 0;
  virtual parameters diagonalize(localized_matrix<double, matrix_col_major>& mata, localized_matrix<double, matrix_col_major>& matb,
				 std::vector<double>& eigvals,
				 parameters const& params) = 0;
};
  
template<typename SOLVER>
class sd_solver_wrapper : public sd_solver_base {
  typedef SOLVER solver_type;
public:
  sd_solver_wrapper() : solver_impl_() {}
  virtual ~sd_solver_wrapper() {}
  void initialize(int& argc, char**& argv) {
    solver_impl_.initialize(argc, argv);
  }
  void finalize() { solver_impl_.finalize(); }
  // -------------- standard eigenvalue probelm ---------------
  // with parameters, eigenvalues/eigenvectors
  parameters diagonalize(localized_matrix<double, matrix_row_major>& mat,
			 localized_vector<double>& eigvals, localized_matrix<double, matrix_row_major>& eigvecs,
			 parameters const& params) {
    return solver_impl_.diagonalize(mat, eigvals, eigvecs, params);
  }
  parameters diagonalize(localized_matrix<double, matrix_col_major>& mat,
			 localized_vector<double>& eigvals, localized_matrix<double, matrix_col_major>& eigvecs,
			 parameters const& params) {
    return solver_impl_.diagonalize(mat, eigvals, eigvecs, params);
  }
  parameters diagonalize(localized_matrix<double, matrix_row_major>& mat,
			 std::vector<double>& eigvals, localized_matrix<double, matrix_row_major>& eigvecs,
			 parameters const& params) {
    return solver_impl_.diagonalize(mat, eigvals, eigvecs, params);
  }
  parameters diagonalize(localized_matrix<double, matrix_col_major>& mat,
			 std::vector<double>& eigvals, localized_matrix<double, matrix_col_major>& eigvecs,
			 parameters const& params) {
    return solver_impl_.diagonalize(mat, eigvals, eigvecs, params);
  }
  // with parameters, only eigenvalues
  parameters diagonalize(localized_matrix<double, matrix_row_major>& mat,
			 localized_vector<double>& eigvals,
			 parameters const& params) {
    return solver_impl_.diagonalize(mat, eigvals, params);
  }
  parameters diagonalize(localized_matrix<double, matrix_col_major>& mat,
			 localized_vector<double>& eigvals,
			 parameters const& params) {
    return solver_impl_.diagonalize(mat, eigvals, params);
  }
  parameters diagonalize(localized_matrix<double, matrix_row_major>& mat,
			 std::vector<double>& eigvals,
			 parameters const& params) {
    return solver_impl_.diagonalize(mat, eigvals, params);
  }
  parameters diagonalize(localized_matrix<double, matrix_col_major>& mat,
			 std::vector<double>& eigvals,
			 parameters const& params) {
    return solver_impl_.diagonalize(mat, eigvals, params);
  }
  // -------------- generalized eigenvalue probelm ---------------
  // with parameters, eigenvalues/eigenvectors
  parameters diagonalize(localized_matrix<double, matrix_row_major>& mata, localized_matrix<double, matrix_row_major>& matb,
			 localized_vector<double>& eigvals, localized_matrix<double, matrix_row_major>& eigvecs,
			 parameters const& params) {
    return solver_impl_.diagonalize(mata, matb, eigvals, eigvecs, params);
  }
  parameters diagonalize(localized_matrix<double, matrix_col_major>& mata, localized_matrix<double, matrix_col_major>& matb,
			 localized_vector<double>& eigvals, localized_matrix<double, matrix_col_major>& eigvecs,
			 parameters const& params) {
    return solver_impl_.diagonalize(mata, matb, eigvals, eigvecs, params);
  }
  parameters diagonalize(localized_matrix<double, matrix_row_major>& mata, localized_matrix<double, matrix_row_major>& matb,
			 std::vector<double>& eigvals, localized_matrix<double, matrix_row_major>& eigvecs,
			 parameters const& params) {
    return solver_impl_.diagonalize(mata, matb, eigvals, eigvecs, params);
  }
  parameters diagonalize(localized_matrix<double, matrix_col_major>& mata, localized_matrix<double, matrix_col_major>& matb,
			 std::vector<double>& eigvals, localized_matrix<double, matrix_col_major>& eigvecs,
			 parameters const& params) {
    return solver_impl_.diagonalize(mata, matb, eigvals, eigvecs, params);
  }
  // with parameters, only eigenvalues
  parameters diagonalize(localized_matrix<double, matrix_row_major>& mata, localized_matrix<double, matrix_row_major>& matb,
			 localized_vector<double>& eigvals,
			 parameters const& params) {
    return solver_impl_.diagonalize(mata, matb, eigvals, params);
  }
  parameters diagonalize(localized_matrix<double, matrix_col_major>& mata, localized_matrix<double, matrix_col_major>& matb,
			 localized_vector<double>& eigvals,
			 parameters const& params) {
    return solver_impl_.diagonalize(mata, matb, eigvals, params);
  }
  parameters diagonalize(localized_matrix<double, matrix_row_major>& mata, localized_matrix<double, matrix_row_major>& matb,
			 std::vector<double>& eigvals,
			 parameters const& params) {
    return solver_impl_.diagonalize(mata, matb, eigvals, params);
  }
  parameters diagonalize(localized_matrix<double, matrix_col_major>& mata, localized_matrix<double, matrix_col_major>& matb,
			 std::vector<double>& eigvals, parameters const& params) {
    return solver_impl_.diagonalize(mata, matb, eigvals, params);
  }
private:
  solver_type solver_impl_;
};
    
typedef factory<sd_solver_base> sd_solver_factory;
  
} // end namespace detail
  
class serial_dense_solver {
public:
  void construct(std::string const& solver_name) {
    solver_impl_ = detail::sd_solver_factory::instance()->make_product(solver_name);
  }
  serial_dense_solver(std::string const& solver_name) {
    this->construct(solver_name);
  }
  serial_dense_solver() {
    this->construct(this->default_solver());
  }
  void initialize(int& argc, char**& argv) {
    solver_impl_->initialize(argc, argv);
  }
  void finalize() {
    solver_impl_->finalize();
  }
  // -------------- standard eigenvalue probelm ---------------
  // with parameters, eigenvalues/eigenvectors
  template<typename MATRIX_MAJOR, typename VEC>
  parameters diagonalize(localized_matrix<double, MATRIX_MAJOR>& mat, VEC& eigvals,
			 localized_matrix<double, MATRIX_MAJOR>& eigvecs,
			 parameters const& params) {
    return solver_impl_->diagonalize(mat, eigvals, eigvecs, params);
  }
  // with parameters, only eigenvalues
  template<typename MATRIX_MAJOR, typename VEC>
  parameters diagonalize(localized_matrix<double, MATRIX_MAJOR>& mat, VEC& eigvals,
			 parameters const& params) {
    return solver_impl_->diagonalize(mat, eigvals, params);
  }
  // no parameters, eigenvalues/eigenvectors
  template<typename MATRIX_MAJOR, typename VEC>
  parameters diagonalize(localized_matrix<double, MATRIX_MAJOR>& mat, VEC& eigvals,
			 localized_matrix<double, MATRIX_MAJOR>& eigvecs) {
    return solver_impl_->diagonalize(mat, eigvals, eigvecs, null_params);
  }
  // no parameters, only eigenvalues
  template<typename MATRIX_MAJOR, typename VEC>
  parameters diagonalize(localized_matrix<double, MATRIX_MAJOR>& mat, VEC& eigvals) {
    return solver_impl_->diagonalize(mat, eigvals, null_params);
  }
  // -------------- generalized eigenvalue probelm ---------------
  // no routine, with parameters, eigenvalues/eigenvectors
  template<typename MATRIX_MAJOR, typename VEC>
  parameters diagonalize(localized_matrix<double, MATRIX_MAJOR>& mata, localized_matrix<double, MATRIX_MAJOR>& matb,
			 VEC& eigvals,
			 localized_matrix<double, MATRIX_MAJOR>& eigvecs,
			 parameters const& params) {
    return solver_impl_->diagonalize(mata, matb, eigvals, eigvecs, params);
  }
  // no routine, with parameters, only eigenvalues
  template<typename MATRIX_MAJOR, typename VEC>
  parameters diagonalize(localized_matrix<double, MATRIX_MAJOR>& mata, localized_matrix<double, MATRIX_MAJOR>& matb,
			 VEC& eigvals,
			 parameters const& params) {
    return solver_impl_->diagonalize(mata, matb, eigvals, params);
  }
  // no routine, no parameters, eigenvalues/eigenvectors
  template<typename MATRIX_MAJOR, typename VEC>
  parameters diagonalize(localized_matrix<double, MATRIX_MAJOR>& mata, localized_matrix<double, MATRIX_MAJOR>& matb,
			 VEC& eigvals,
			 localized_matrix<double, MATRIX_MAJOR>& eigvecs) {
    return solver_impl_->diagonalize(mata, matb, eigvals, eigvecs, null_params);
  }
  // no routine, no parameters, only eigenvalues
  template<typename MATRIX_MAJOR, typename VEC>
  parameters diagonalize(localized_matrix<double, MATRIX_MAJOR>& mata, localized_matrix<double, MATRIX_MAJOR>& matb,
			 VEC& eigvals) {
    return solver_impl_->diagonalize(mata, matb, eigvals, null_params);
  }
  static std::vector<std::string> solvers() {
    return detail::sd_solver_factory::product_names();
  }
  static std::string default_solver() {
    return detail::sd_solver_factory::default_product_name();
  }
private:
  detail::sd_solver_factory::product_pointer_type solver_impl_;
  parameters null_params;
  //std::string routine_;
};

} // end namespace rokko

#define ROKKO_REGISTER_SERIAL_DENSE_SOLVER(solver, name, priority) \
namespace { namespace BOOST_JOIN(register, __LINE__) { \
struct register_caller { \
  typedef rokko::factory<rokko::detail::sd_solver_base> factory; \
  typedef rokko::detail::sd_solver_wrapper<solver> product; \
  register_caller() { factory::instance()->register_creator<product>(name, priority); } \
} caller; \
} }

#endif // ROKKO_SERIAL_DENSE_SOLVER_HPP
