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

#ifndef TITPACK_LARGE_HPP
#define TITPACK_LARGE_HPP

#include "common.hpp"
#include "hamiltonian.hpp"
#include <vector>

//
// eigenvalues by the Lanczos method
// --- dummy routine for simple working area
//
// return value # number of iterations required for convergence
// nvec      @ number of eigenvectors to calculate in lncvec
// iv        @ location of the nonzero element of the initial vector
// E         # eigenvalues
// wk        working areas

int lnc1(hamiltonian const& hop, int nvec, int iv, std::vector<double>& E,
         std::vector<double>& alpha, std::vector<double>& beta, matrix_type& coeff,
         matrix_type& wk);

//
// eigenvalues by the Lanczos method
//

int lnc1z(hamiltonian const& hop, int nvec, int iv, std::vector<double>& E,
          std::vector<double>& alpha, std::vector<double>& beta, matrix_type& coeff,
          double *v1, double *v0);

//
// eigenvector by the Lanczos method
//
// nvec      @ number of eigenvectors to be calculated
// iv        @ location of the nonzero element of the initial vector
// x         # eigenvector
// itr     @ number of interations for convergence
// wk         working area

void lncv1(hamiltonian const& hop, int nvec, int iv, std::vector<double> const& alpha,
           std::vector<double> const& beta, matrix_type const& coeff, matrix_type& x,
           int itr, matrix_type& wk);

//
// eigenvector by the Lanczos method
//

void lncv1z(hamiltonian const& hop, int nvec, int iv, std::vector<double> const& alpha,
            std::vector<double> const& beta, matrix_type const& coeff, matrix_type& x,
            int itr, double *v1, double *v0);

//
// check of the eigenvector and eigenvalue
//
// x           @ eigenvector to be checked
// v           # H*x

double check1(hamiltonian const& hop, const double *x, matrix_type& v, int vindex);

double check1(hamiltonian const& hop, matrix_type const& x, int xindex, matrix_type& v, int vindex);

double check1(hamiltonian const& hop, std::vector<double> const& x, matrix_type& v, int vindex);

//
// inverse iteration
//   --- dummy routine for simple working area
//

void inv1(hamiltonian const& hop, double Eig, int iv, std::vector<double>& x, matrix_type& wk);

//
// inverse iteration
//
// b            working area for the rhs of (H-E(approx))*x=b
// p,r,y        working area used in the routine cg1

void inv1z(hamiltonian const& hop, double Eig, int iv, std::vector<double>& x, double *b,
           double *p, double *r, double *y);

//
// solution of linear equations -- cg method
//
// return value # number of iterations required for convergence
// Eig         @ eigenvalue
// x           # eigenvector
// b             working area for the rhs of (H-E(approx))*x=b
// p,r,y         working area used in the routine cg

int cg1(hamiltonian const& hop, double Eig, std::vector<double>& x, double *b,
        double *p, double *r, double *y);

#endif
