/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2014-2014 by Synge Todo <wistaria@comp-phys.org>,
*                            Tatsuya Sakashita <t-sakashita@issp.u-tokyo.ac.jp>
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#ifndef ROKKO_ANASAZI_MAPPING_1D_HPP
#define ROKKO_ANASAZI_MAPPING_1D_HPP

#include <rokko/grid_1d.hpp>

#include <Epetra_MpiComm.h>
#include <Epetra_Map.h>
#include <Teuchos_RCPDecl.hpp>

namespace rokko {

namespace anasazi {

class mapping_1d {
public:
  explicit mapping_1d() : dim_(0), g_(), ep_comm_(Epetra_MpiComm(MPI_COMM_WORLD)), map_(dim_, 0, ep_comm_) {}
  explicit mapping_1d(int dim) : dim_(dim), g_(), ep_comm_(Epetra_MpiComm(MPI_COMM_WORLD)), map_(dim_, 0, ep_comm_) {
    num_local_rows_ = map_.NumMyElements();
  }
  explicit mapping_1d(int dim, grid_1d const& g_in) :
    dim_(dim), g_(g_in), ep_comm_(Epetra_MpiComm(g_.get_comm())), map_(dim_, 0, ep_comm_) {
    num_local_rows_ = map_.NumMyElements();
    //num_rows = dim / g.get_nprocs();
    //rest_row = dim % g.get_nprocs();
    //if (g.get_myrank() < rest_row) {
    //  ++num_rows;
    //}
  }
  int get_dim() const { return dim_; }
  int get_num_local_rows() const { return num_local_rows_; }
  Epetra_MpiComm const& get_epetra_comm() const { return ep_comm_; }
  Epetra_Map const& get_epetra_map() const { return map_; }
  grid_1d const& get_grid_1d() const { return g_; }

private:
  int dim_;
  int num_local_rows_;
  grid_1d g_;
  Epetra_MpiComm ep_comm_;
  Epetra_Map map_;
};

} // namespace anasazi

} // namespace rokko

#endif // ROKKO_ANASAZI_MAPPING_1D_HPP
