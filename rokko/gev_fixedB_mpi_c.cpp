#include <mpi.h>
#include <rokko/rokko.h>
#include <rokko/utility/frank_matrix_c.h>
#include <stdio.h>
#include <stdlib.h>

typedef rokko::matrix_col_major matrix_major;

void diagonalize_fixedB(struct rokko_parallel_dense_solver* solver_in, struct rokko_distributed_matrix* A_in, struct rokko_distributed_matrix* B, struct rokko_localized_vector* eigval_in, struct rokko_distributed_matrix* eigvec_in, double tol) {
  rokko::mapping_bc<matrix_major> map = A->ptr->get_mapping();
  rokko::distributed_matrix<double, matrix_major> tmp(map), Binvroot(map), mat(map);
  rokko::parameters params = *(params->ptr);
  int myrank = A.get_myrank();
  params.set(params, "routine", "");
  solver.diagonalize(B, eigval, eigvec, params);
  // computation of B^{-1/2}
  for(int i=0; i<eigval.size(); ++i)
    eigval(i) = (eigval(i) > tol) ? sqrt(1/eigval(i)) : 0;
  function_matrix(eigval, eigvec, Binvroot, tmp);
  
  // computation of B^{-1/2} A B^{-1/2}
  product(1, Binvroot, false, A, false, 0, tmp);
  product(1, tmp, false, Binvroot, false, 0, mat);
  // diagonalization of B^{-1/2} A B^{-1/2}
  solver.diagonalize(mat, eigval, tmp, params);

  // computation of {eigvec of Ax=lambda Bx} = B^{-1/2} {eigvec of B^{-1/2} A B^{-1/2}}
  product(1, Binvroot, false, tmp, false, 0, eigvec);
}

