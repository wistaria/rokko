/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2014 by Synge Todo <wistaria@comp-phys.org>
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#ifndef ROKKO_ANASAZI_DISTRIBUTED_CRS_MATRIX_HPP
#define ROKKO_ANASAZI_DISTRIBUTED_CRS_MATRIX_HPP

#include <rokko/anasazi/mapping_1d.hpp>
#include <rokko/distributed_crs_matrix.hpp>

#include <Epetra_CrsMatrix.h>
#include <AnasaziEpetraAdapter.hpp>
#include <Teuchos_RCPDecl.hpp>

namespace rokko {
namespace anasazi {

class distributed_crs_matrix : public rokko::detail::distributed_crs_matrix_base {
public:
  explicit distributed_crs_matrix(int row_dim, int col_dim) {
    initialize(row_dim, col_dim);
    num_local_rows_ = map_->get_epetra_map().NumMyElements();
    start_rows_ = map_->get_epetra_map().MinMyGID();
    end_rows_ = map_->get_epetra_map().MaxMyGID();
  }
  void initialize(int row_dim, int col_dim) {
    map_ = new mapping_1d(row_dim);
    matrix_ = Teuchos::rcp(new Epetra_CrsMatrix(Copy, map_->get_epetra_map(), col_dim));    
  }
  void insert(int row, std::vector<int> const& cols, std::vector<double> const& values) {
    matrix_->InsertGlobalValues(row, cols.size(), &values[0], &cols[0]);
  }
  void complete() {
    matrix_->FillComplete();
    matrix_->SetTracebackMode(1);
  }
  Teuchos::RCP<Epetra_CrsMatrix> get_matrix() {
    return matrix_;
  }
  int get_dim() {
    return dim_;
  }
  int num_local_rows() {
    return num_local_rows_;
  }
  int start_rows() {
    return start_rows_;
  }
  int end_rows() {
    return end_rows_;
  }
  //private:
  mapping_1d* map_;
  Teuchos::RCP<Epetra_CrsMatrix> matrix_;
  int dim_;
  int num_local_rows_, start_rows_, end_rows_;
};

} // namespace anasazi
} // namespace rokko

#endif // ROKKO_ANASAZI_DISTRIBUTED_CRS_MATRIX_HPP
