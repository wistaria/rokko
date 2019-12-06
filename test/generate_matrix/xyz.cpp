/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2012-2019 Rokko Developers https://github.com/t-sakashita/rokko
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>

#include <rokko/utility/xyz_hamiltonian.hpp>
#include <rokko/eigen3.hpp>

#include <gtest/gtest.h>

TEST(generate_matrix, xyz_hamiltonian) {
  std::size_t L = 4;
  std::size_t num_bonds = L - 1;
  std::size_t N = 1 << L;
  std::vector<std::pair<int, int>> lattice;
  std::vector<std::tuple<double, double, double>> coupling;
  for (std::size_t i=0; i<L-1; ++i) {
    lattice.push_back(std::make_pair(i, i+1));
    coupling.push_back(std::make_tuple(1, 1, 1));
  }

  std::cout << "dim=" << N << std::endl;  
  std::cout << "L=" << L << std::endl;
  for (std::size_t i=0; i<num_bonds; ++i) {
    std::cout << lattice[i].first << " " << lattice[i].second << " " << std::get<0>(coupling[i]) << " " << std::get<1>(coupling[i]) << " " << std::get<2>(coupling[i]) << std::endl;
  }

  Eigen::MatrixXd mat1(N, N);
  Eigen::VectorXd v(N), w(N);
  std::cout << "multiply:" << std::endl;
  for (int i=0; i<N; ++i) {
    v.setZero();
    v(i) = 1;
    w.setZero();
    rokko::xyz_hamiltonian::multiply(L, lattice, coupling, v, w);
    mat1.col(i) = w;
    std::cout << "w = " << w.transpose() << std::endl;
  }

  std::cout << "fill_diagonal:" << std::endl;
  Eigen::VectorXd diagonal(N);
  rokko::xyz_hamiltonian::fill_diagonal(L, lattice, coupling, diagonal);
  std::cout << "diagonal = " << diagonal.transpose() << std::endl;

  std::cout << "fill_matrix:" << std::endl;
  Eigen::MatrixXd mat2(N, N);
  rokko::xyz_hamiltonian::generate(L, lattice, coupling, mat2);
  std::cout << mat2 << std::endl;

  EXPECT_TRUE(mat1 == mat2) << "matrix generated by 'multiply' is differnet from the matrix generated by 'generate'.";

  EXPECT_TRUE(diagonal == mat2.diagonal()) << "diagonal elements generated by 'fill_diagonal' is differnet from diagonal elements of the matrix generated by 'genertate'.";
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
