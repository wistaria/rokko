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

// C++ version of TITPACK Ver.2 by H. Nishimori

/************ Sample main program #11 *****************
* 1d Heisenberg antiferromagnet with 8 spins
* Eigenvalues and an eigenvector by diag
******************************************************/

#include "titpack.hpp"
#include "options.hpp"
#include <boost/timer.hpp>

int main(int argc, char** argv) {
  std::cout.precision(10);
  options opt(argc, argv, 8);
  if (!opt.valid) std::abort();
  boost::timer tm;
  double t1 = tm.elapsed();

  // lattice structure
  int n = opt.N;
  int ibond = n;
  std::vector<int> ipair;
  for (int i = 0; i < ibond; ++i) {
    ipair.push_back(i);
    ipair.push_back((i + 1) % n);
  }

  // Hamiltonian parameters
  std::vector<double> bondwt(ibond, -1);
  std::vector<double> zrtio(ibond, 1);

  // table of configurations and Hamiltonian operator
  subspace ss(n, 0);
  hamiltonian hop(ss, ipair, bondwt, zrtio);

  // Hamiltonian matrix
  matrix_type elemnt;
  elm3(hop, elemnt);
  double t2 = tm.elapsed();
  
  std::vector<double> E;
  matrix_type v;
  int nvec = 1;
  diag(elemnt, E, v, nvec);
  double t3 = tm.elapsed();

  int ne = 4;
  std::cout << "[Eigenvalues]\n";
  for (int i = 0; i < ne; ++i) std::cout << '\t' << E[i];
  std::cout << std::endl;

  // // Do not forget to call elm3 again before calling check3
  elm3(hop, elemnt);
  check3(elemnt, v, 0);
  double t4 = tm.elapsed();
  
  std::vector<int> npair;
  npair.push_back(1);
  npair.push_back(2);
  std::vector<double> sxx(1);
  xcorr(ss, npair, v, 0, sxx);
  std::cout << "sxx: " << sxx[0] << std::endl;
  std::vector<double> szz(1);
  zcorr(ss, npair, v, 0, szz);
  std::cout << "szz: " << szz[0] << std::endl;
  double t5 = tm.elapsed();
  std::cerr << "initialize      " << (t2-t1) << " sec\n"
            << "diagonalization " << (t3-t2) << " sec\n"
            << "check           " << (t4-t3) << " sec\n"
            << "correlation     " << (t5-t4) << " sec\n";
}
