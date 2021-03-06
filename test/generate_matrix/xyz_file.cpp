/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2012-2013 by Tatsuya Sakashita <t-sakashita@issp.u-tokyo.ac.jp>,
*                            Synge Todo <wistaria@comp-phys.org>
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#include <iostream>
#include <fstream>
#include <vector>
#include <boost/tuple/tuple.hpp>

#include <rokko/utility/xyz_hamiltonian.hpp>
#include <rokko/localized_matrix.hpp>
#include <rokko/localized_vector.hpp>

int main(int argc, char *argv[])
{
  if (argc <= 1) {
    std::cerr << "error: " << argv[0] << " xyz.ip" << std::endl;
    exit(1);
  }

  std::ifstream ifs(argv[1]);
  if (!ifs) {
    std::cout << "can't open file" << std::endl;
    exit(2);
  }

  int L, num_bonds;
  std::vector<std::pair<int, int> > lattice;
  std::vector<boost::tuple<double, double, double> > coupling;
  ifs >> L >> num_bonds;
  for (int i=0; i<num_bonds; ++i) {
    int j, k;
    ifs >> j >> k;
    lattice.push_back(std::make_pair(j, k));
  }
  
  for (int i=0; i<num_bonds; ++i) {
    double jx, jy, jz;
    ifs >> jx >> jy >> jz;
    coupling.push_back(boost::make_tuple(jx, jy, jz));
  }
  
  std::cout << "L=" << L << " num_bonds=" << num_bonds << std::endl;
  for (int i=0; i<num_bonds; ++i) {
    std::cout << lattice[i].first << " " << lattice[i].second << " " << coupling[i].get<0>() << " " << coupling[i].get<1>() << " " << coupling[i].get<2>() << std::endl;
  }
  int dim = 1 << L;
  int N = dim;
  std::cout << "dim=" << dim << std::endl;

  rokko::localized_matrix<rokko::matrix_col_major> mat1(N, N);
  std::cout << "multiply:" << std::endl;
  for (int i=0; i<N; ++i) {
    std::vector<double> v, w;
    v.assign(N, 0);
    v[i] = 1;
    w.assign(N, 0);
    rokko::xyz_hamiltonian::multiply(L, lattice, coupling, v, w);
    for (int j=0; j<N; ++j) {
      mat1(j,i) = w[j];
      std::cout << w[j] << " ";
    }
    std::cout << std::endl;
  }

  std::cout << "fill_diagonal:" << std::endl;
  rokko::localized_vector diagonal(N);
  std::vector<double> v(N);
  rokko::xyz_hamiltonian::fill_diagonal(L, lattice, coupling, v);
  for (int j=0; j<N; ++j) {
    diagonal(j) = v[j];
    std::cout << v[j] << " ";
  }
  std::cout << std::endl;

  std::cout << "fill_matrix:" << std::endl;
  rokko::localized_matrix<rokko::matrix_col_major> mat2(N, N);
  rokko::xyz_hamiltonian::generate(L, lattice, coupling, mat2);
  for (int i=0; i<N; ++i) {
    for (int j=0; j<N; ++j) {
      std::cout << mat2(i,j) << " ";
    }
    std::cout << std::endl;
  }

  if (mat1 == mat2) {
    std::cout << "OK: matrix by 'multiply' equals to a matrix by 'generate'." << std::endl;
  } else {
    std::cout << "ERROR: matrix by 'multiply' is differnet from a matrix by 'generate'."<< std::endl;
    exit(1);
  }

  if (diagonal == mat2.diagonal()) {
    std::cout << "OK: diagonal by 'fill_diagonal' equals to diagonal elementas of a matrix by 'genertate'."<< std::endl;
  } else {
    std::cout << "ERROR: diagonal by 'fill_diagonal' is differnet from diagonal elementas of a matrix by 'genertate'."<< std::endl;
    exit(1);
  }

}


