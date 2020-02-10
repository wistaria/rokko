/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2012-2020 by Rokko Developers https://github.com/t-sakashita/rokko
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#ifndef ROKKO_SLEPC_DISTRIBUTED_CRS_MATRIX_HPP
#define ROKKO_SLEPC_DISTRIBUTED_CRS_MATRIX_HPP

#include <rokko/slepc/mapping_1d.hpp>
#include <rokko/distributed_crs_matrix.hpp>
#include <rokko/utility/timer.hpp>

#include <rokko/slepc.hpp>

#include <numeric>
#include <iostream>

namespace rokko {
namespace slepc {

class distributed_crs_matrix : public rokko::detail::ps_crs_base {
public:
  distributed_crs_matrix() = default;
  ~distributed_crs_matrix() = default;

  explicit distributed_crs_matrix(rokko::mapping_1d const& map, int num_entries_per_row) {
    initialize(map, num_entries_per_row);
  }

  explicit distributed_crs_matrix(rokko::slepc::mapping_1d const& map, int num_entries_per_row) {
    initialize(map, num_entries_per_row);
  }
  explicit distributed_crs_matrix(std::array<int,2> const& dims) {
    initialize(dims);
  }
  explicit distributed_crs_matrix(std::array<int,2> const& dims, int num_entries_per_row) {
    initialize(dims, num_entries_per_row);
  }

  void initialize(rokko::mapping_1d const& map, int num_entries_per_row) {
    if (map.get_solver_name() != "slepc") {
      throw std::invalid_argument("SLEPc's distributed_crs_matrix() : " + map.get_solver_name() + "'s mapping_1d is given.");
    }
    initialize(*dynamic_cast<const rokko::slepc::mapping_1d*>(map.get_ptr()->get_impl()), num_entries_per_row);
  }

  void initialize(rokko::slepc::mapping_1d const& map, int num_entries_per_row) {
    map_ = &map;
    int dim = map_->get_dim();
    ierr = MatCreate(map_->get_mpi_comm().get_comm(), &matrix_);  //CHKERRQ(ierr);
    ierr = MatSetSizes(matrix_, PETSC_DECIDE, PETSC_DECIDE, dim, dim);  //CHKERRQ(ierr);
    ierr = MatSetFromOptions(matrix_);  //CHKERRQ(ierr);
    ierr = MatSeqAIJSetPreallocation(matrix_, num_entries_per_row, NULL);
    ierr = MatMPIAIJSetPreallocation(matrix_, num_entries_per_row, NULL, num_entries_per_row, NULL);
  }

  #undef __FUNCT__
  #define __FUNCT__ "distributed_crs_matrix/initialize"
  void initialize(std::array<int,2> const& dims) {
    map_ = new rokko::slepc::mapping_1d(dims[0]);
    ierr = MatCreate(map_->get_mpi_comm().get_comm(), &matrix_);  //CHKERRQ(ierr);
    ierr = MatSetSizes(matrix_, PETSC_DECIDE, PETSC_DECIDE, dims[0], dims[1]);  //CHKERRQ(ierr);
    ierr = MatSetFromOptions(matrix_);  //CHKERRQ(ierr);
    ierr = MatSetUp(matrix_);  //CHKERRQ(ierr);
  }
  #undef __FUNCT__
  #define __FUNCT__ "distributed_crs_matrix/initialize with num_entries_per_row"
  void initialize(std::array<int,2> const& dims, int num_entries_per_row) {
    map_ = new rokko::slepc::mapping_1d(dims[0]);
    ierr = MatCreate(map_->get_mpi_comm().get_comm(), &matrix_);  //CHKERRQ(ierr);
    ierr = MatSetSizes(matrix_, PETSC_DECIDE, PETSC_DECIDE, dims[0], dims[1]);  //CHKERRQ(ierr);
    ierr = MatSetFromOptions(matrix_);  //CHKERRQ(ierr);
    ierr = MatSeqAIJSetPreallocation(matrix_, num_entries_per_row, NULL);
    ierr = MatMPIAIJSetPreallocation(matrix_, num_entries_per_row, NULL, num_entries_per_row, NULL);
  }
  #undef __FUNCT__
  #define __FUNCT__ "distributed_crs_matrix/insert"
  void insert(int row, std::vector<int> const& cols, std::vector<double> const& values) {
    ierr = MatSetValues(matrix_, 1, &row, cols.size(), cols.data(), values.data(), INSERT_VALUES);  //CHKERRQ(ierr);
  }
  void insert(int row, int col_size, int const*const cols, double const*const values) {
    ierr = MatSetValues(matrix_, 1, &row, col_size, cols, values, INSERT_VALUES);  //CHKERRQ(ierr);
  }
  #undef __FUNCT__
  #define __FUNCT__ "distributed_crs_matrix/complete"
  void complete() {
    ierr = MatAssemblyBegin(matrix_, MAT_FINAL_ASSEMBLY);  //CHKERRQ(ierr);
    ierr = MatAssemblyEnd(matrix_, MAT_FINAL_ASSEMBLY);  //CHKERRQ(ierr);
  }
  Mat& get_matrix() {
    return matrix_;
  }
  const Mat& get_matrix() const {
    return matrix_;
  }
  int get_dim() const {
    return map_->get_dim();
  }
  int num_local_rows() const {
    int start_row, end_row;
    PetscErrorCode ierr = MatGetOwnershipRange(matrix_, &start_row, &end_row); //CHKERRQ(ierr);
    return end_row - start_row;
  }
  int start_row() const {
    int start_row;
    PetscErrorCode ierr = MatGetOwnershipRange(matrix_, &start_row, NULL); //CHKERRQ(ierr);
    return start_row;
  }
  int end_row() const {
    int end_row;
    PetscErrorCode ierr = MatGetOwnershipRange(matrix_, NULL, &end_row); //CHKERRQ(ierr);
    return end_row;
  }
  int get_nnz() const {
    MatInfo info;
    MatGetInfo(matrix_, MAT_GLOBAL_SUM, &info);
    return static_cast<int>(info.nz_used);
  }
  void print() const {
    MatView(matrix_, PETSC_VIEWER_STDOUT_(map_->get_mpi_comm().get_comm()));
  }
  void output_matrix_market(std::ostream& os = std::cout) const {
    constexpr int root_proc = 0;
    std::vector<int> idx;

    PetscInt num_cols;
    const PetscInt * cols;
    const PetscScalar * values;
    const int nnz = get_nnz();
    if (map_->get_mpi_comm().get_myrank() == root_proc) {
      os << "%%MatrixMarket matrix coordinate real general" << std::endl;
      os << get_dim() << " " << get_dim() << " " << nnz << std::endl;
    }
    MPI_Barrier(map_->get_mpi_comm().get_comm());
    for (int global_row=0; global_row<get_dim(); ++global_row) {
      if ((global_row >= start_row()) && (global_row < end_row())) {
        MatGetRow(matrix_, global_row, &num_cols, &cols, &values);
        idx.resize(num_cols);
        std::iota(idx.begin(), idx.end(), 0);
        std::sort(idx.begin(), idx.end(), [&cols](auto i, auto j) { return cols[i] < cols[j]; });
        for (int i=0; i<num_cols; ++i) {
          os << global_row + 1 << " " << cols[idx[i]] + 1 << " " << values[idx[i]] << std::endl;
        }
        MatRestoreRow(matrix_, global_row, &num_cols, &cols, &values);
      }
      MPI_Barrier(map_->get_mpi_comm().get_comm());
    }
  }

  const rokko::slepc::mapping_1d* get_map() const { return map_; }

  ps_crs_base* get_impl() { return this; }
  const ps_crs_base* get_impl() const { return this; }

private:
  const rokko::slepc::mapping_1d* map_;
  PetscErrorCode ierr;
  Mat matrix_;
};

} // namespace slepc
} // namespace rokko

#endif // ROKKO_SLEPC_DISTRIBUTED_CRS_MATRIX_HPP
