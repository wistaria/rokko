/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2015 by Rokko Developers https://github.com/t-sakashita/rokko
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#include <boost/python.hpp>

#include "rokko_dense.h"
#include "rokko_sparse.h"
#include "utility/frank_matrix_c.h"

enum rokko {
  grid_col_major = rokko_grid_col_major, grid_row_major = rokko_grid_row_major,
  matrix_col_major = rokko_matrix_col_major, matrix_row_major = rokko_matrix_row_major
};

class wrap_rokko_localized_matrix {
	rokko_localized_matrix* raw;
public:
	wrap_rokko_localized_matrix(int dim1, int dim2, int matrix_major) {
		raw = new rokko_localized_matrix();
		rokko_localized_matrix_construct(raw, dim1, dim2, matrix_major);
	}
	rokko_localized_matrix* get_raw(void) {
		return raw;
	}

	void print(void) {
		rokko_localized_matrix_print(*raw);
	}

	~wrap_rokko_localized_matrix(void) {
		rokko_localized_matrix_destruct(raw);
		delete raw;
	}
};

class wrap_rokko_localized_vector {
	rokko_localized_vector* raw;
public:
	wrap_rokko_localized_vector(int dim1) {
		raw = new rokko_localized_vector();
		rokko_localized_vector_construct(raw, dim1);
	}
	rokko_localized_vector* get_raw(void) {
		return raw;
	}

	double get(int i) {
		return rokko_localized_vector_get(*raw, i);
	}

	~wrap_rokko_localized_vector(void) {
		rokko_localized_vector_destruct(raw);
		delete raw;
	}
};

class wrap_rokko_serial_dense_solver {
	rokko_serial_dense_solver* raw;
public:
	wrap_rokko_serial_dense_solver(char* solver_name, int argc, char** argv) {
		raw = new rokko_serial_dense_solver();
		rokko_serial_dense_solver_construct(raw, solver_name, argc, argv);
	}
	rokko_serial_dense_solver* get_raw(void) {
		return raw;
	}

	void diagonalize_localized_matrix(wrap_rokko_localized_matrix* mat, wrap_rokko_localized_vector* eigvals, wrap_rokko_localized_matrix* eigvecs) {
		rokko_serial_dense_solver_diagonalize_localized_matrix(raw, mat->get_raw(), eigvals->get_raw(), eigvecs->get_raw());
	}

	~wrap_rokko_serial_dense_solver(void) {
		rokko_serial_dense_solver_destruct(raw);
		delete raw;
	}
};

static void wrap_rokko_frank_matrix_generate_localized_matrix(wrap_rokko_localized_matrix* mat)
{
	rokko_frank_matrix_generate_localized_matrix(mat->get_raw());
}

#if defined(ROKKO_HAVE_PARALLEL_DENSE_SOLVER) || defined(ROKKO_HAVE_PARALLEL_SPARSE_SOLVER)

class wrap_rokko_grid {
	rokko_grid* raw;
public:
	wrap_rokko_grid(boost::python::object comm, int grid_major) {
		boost::python::object comm_func = comm.attr("py2f");
		boost::python::object comm_f_obj = comm_func();
		int comm_f = boost::python::extract<int>(comm_f_obj);
		raw = new rokko_grid();
		rokko_grid_construct_f(raw, comm_f, grid_major);
	}
	rokko_grid* get_raw(void) {
		return raw;
	}

	int get_myrank(void) {
		return rokko_grid_get_myrank(*raw);
	}

	int get_nprocs(void) {
		return rokko_grid_get_nprocs(*raw);
	}

	~wrap_rokko_grid(void) {
		rokko_grid_destruct(raw);
		delete raw;
	}
};

class wrap_rokko_distributed_matrix;

class wrap_rokko_parallel_dense_solver {
	rokko_parallel_dense_solver* raw;
public:
	wrap_rokko_parallel_dense_solver(char* solver_name, int argc, char** argv) {
		raw = new rokko_parallel_dense_solver();
		rokko_parallel_dense_solver_construct(raw, solver_name, argc, argv);
	}
	rokko_parallel_dense_solver* get_raw(void) {
		return raw;
	}

	void diagonalize_distributed_matrix(wrap_rokko_distributed_matrix*, wrap_rokko_localized_vector*, wrap_rokko_distributed_matrix*);

	~wrap_rokko_parallel_dense_solver(void) {
		rokko_parallel_dense_solver_destruct(raw);
		delete raw;
	}
};

class wrap_rokko_distributed_matrix {
	rokko_distributed_matrix* raw;
public:
	wrap_rokko_distributed_matrix(int dim1, int dim2, wrap_rokko_grid* grid, wrap_rokko_parallel_dense_solver* solver, int matrix_major) {
		raw = new rokko_distributed_matrix();
		rokko_distributed_matrix_construct(raw, dim1, dim2, *grid->get_raw(), *solver->get_raw(), matrix_major);
	}
	rokko_distributed_matrix* get_raw(void) {
		return raw;
	}

	void print(void) {
		rokko_distributed_matrix_print(*raw);
	}

	void set_local(int local_i, int local_j, double value) {
		rokko_distributed_matrix_set_local(raw, local_i, local_j, value);
	}

	double get_local(int local_i, int local_j) {
		return rokko_distributed_matrix_get_local(*raw, local_i, local_j);
	}

	void set_global(int global_i, int global_j, double value) {
		rokko_distributed_matrix_set_global(raw, global_i, global_j, value);
	}

	double get_global(int global_i, int global_j) {
		return rokko_distributed_matrix_get_global(*raw, global_i, global_j);
	}

	int get_m_local(void) {
		return rokko_distributed_matrix_get_m_local(*raw);
	}

	int get_n_local(void) {
		return rokko_distributed_matrix_get_n_local(*raw);
	}

	int get_m_global(void) {
		return rokko_distributed_matrix_get_m_global(*raw);
	}

	int get_n_global(void) {
		return rokko_distributed_matrix_get_n_global(*raw);
	}

	int get_nprocs(void) {
		return rokko_distributed_matrix_get_nprocs(*raw);
	}

	int get_myrank(void) {
		return rokko_distributed_matrix_get_myrank(*raw);
	}

	int translate_l2g_row(int local_i) {
		return rokko_distributed_matrix_translate_l2g_row(*raw, local_i);
	}

	int translate_l2g_col(int local_j) {
		return rokko_distributed_matrix_translate_l2g_col(*raw, local_j);
	}

	int translate_g2l_row(int global_i) {
		return rokko_distributed_matrix_translate_g2l_row(*raw, global_i);
	}

	int translate_g2l_col(int global_j) {
		return rokko_distributed_matrix_translate_g2l_col(*raw, global_j);
	}

	~wrap_rokko_distributed_matrix(void) {
		rokko_distributed_matrix_destruct(raw);
		delete raw;
	}
};

void wrap_rokko_parallel_dense_solver::diagonalize_distributed_matrix(wrap_rokko_distributed_matrix* mat, wrap_rokko_localized_vector* eigvals, wrap_rokko_distributed_matrix* eigvecs)
{
	rokko_parallel_dense_solver_diagonalize_distributed_matrix(raw, mat->get_raw(), eigvals->get_raw(), eigvecs->get_raw());
}

void wrap_rokko_gather(wrap_rokko_distributed_matrix* matrix, double* array, int root)
{
	return rokko_gather(matrix->get_raw(), array, root);
}

void wrap_rokko_scatter(double* global_array, wrap_rokko_distributed_matrix* matrix, int root)
{
	rokko_scatter(global_array, matrix->get_raw(), root);
}

void wrap_rokko_all_gather(wrap_rokko_distributed_matrix* matrix, double* array)
{
	rokko_all_gather(matrix->get_raw(), array);
}

static void wrap_rokko_frank_matrix_generate_distributed_matrix(wrap_rokko_distributed_matrix* mat)
{
	rokko_frank_matrix_generate_distributed_matrix(mat->get_raw());
}

class wrap_rokko_distributed_crs_matrix;

class wrap_rokko_parallel_sparse_solver {
	rokko_parallel_sparse_solver* raw;
public:
	wrap_rokko_parallel_sparse_solver(char* solver_name, int argc, char** argv) {
		raw = new rokko_parallel_sparse_solver();
		rokko_parallel_sparse_solver_construct(raw, solver_name, argc, argv);
	}
	rokko_parallel_sparse_solver* get_raw(void) {
		return raw;
	}

	void diagonalize_distributed_crs_matrix(wrap_rokko_distributed_crs_matrix*, int, int, int, double);

	double eigenvalue(int i) {
		return rokko_parallel_sparse_solver_eigenvalue(raw, i);
	}

	void eigenvector(int i, boost::python::list& vec) {
		int len_vec = boost::python::len(vec);
		double raw_vec[len_vec];
		rokko_parallel_sparse_solver_eigenvector(raw, i, raw_vec);
		for (int i = 0; i < len_vec; i++) {
			vec[i] = raw_vec[i];
		}
	}

	int num_conv(void) {
		return rokko_parallel_sparse_solver_num_conv(raw);
	}

	~wrap_rokko_parallel_sparse_solver(void) {
		rokko_parallel_sparse_solver_destruct(raw);
		delete raw;
	}
};

class wrap_rokko_distributed_crs_matrix {
	rokko_distributed_crs_matrix* raw;
public:
	wrap_rokko_distributed_crs_matrix(int dim1, int dim2, wrap_rokko_parallel_sparse_solver* solver) {
		raw = new rokko_distributed_crs_matrix();
		rokko_distributed_crs_matrix_construct(raw, dim1, dim2, *solver->get_raw());
	}
	rokko_distributed_crs_matrix* get_raw(void) {
		return raw;
	}

	void insert(int row, int col_size, boost::python::list& cols, boost::python::list& values) {
		int len_cols = boost::python::len(cols);
		int raw_cols[len_cols];
		for (int i = 0; i < len_cols; i++) {
			raw_cols[i] = boost::python::extract<int>(cols[i]);
		}

		int len_values = boost::python::len(values);
		double raw_values[len_values];
		for (int i = 0; i < len_cols; i++) {
			raw_values[i] = boost::python::extract<double>(values[i]);
		}

		rokko_distributed_crs_matrix_insert(raw, row, col_size, raw_cols, raw_values);
	}

	void complete(void) {
		rokko_distributed_crs_matrix_complete(raw);
	}

	int num_local_rows(void) {
		return rokko_distributed_crs_matrix_num_local_rows(raw);
	}

	int start_row(void) {
		return rokko_distributed_crs_matrix_start_row(raw);
	}	

	int end_row(void) {
		return rokko_distributed_crs_matrix_end_row(raw);
	}

	void print(void) {
		rokko_distributed_crs_matrix_print(raw);
	}

	~wrap_rokko_distributed_crs_matrix(void) {
		rokko_distributed_crs_matrix_destruct(raw);
		delete raw;
	}
};

void wrap_rokko_parallel_sparse_solver::diagonalize_distributed_crs_matrix(wrap_rokko_distributed_crs_matrix* mat, int num_evals, int block_size, int max_iters, double tol)
{
	rokko_parallel_sparse_solver_diagonalize_distributed_crs_matrix(raw, mat->get_raw(), num_evals, block_size, max_iters, tol);
}

#endif

BOOST_PYTHON_MODULE(rokko_ext) {
  using namespace boost::python;

  enum_<rokko>("rokko")
    .value("grid_col_major", grid_col_major)
    .value("grid_row_major", grid_row_major)
    .value("matrix_col_major", matrix_col_major)
    .value("matrix_row_major", matrix_row_major);
  
  class_<wrap_rokko_serial_dense_solver>("rokko_serial_dense_solver", init<char*, int, char**>())
    .def("diagonalize_localized_matrix",
         &wrap_rokko_serial_dense_solver::diagonalize_localized_matrix);
  class_<wrap_rokko_localized_vector>("rokko_localized_vector", init<int>())
    .def("get", &wrap_rokko_localized_vector::get);
  class_<wrap_rokko_localized_matrix>("rokko_localized_matrix", init<int, int, int>())
    .def("show", &wrap_rokko_localized_matrix::print);
  def("rokko_frank_matrix_generate_localized_matrix",
      &wrap_rokko_frank_matrix_generate_localized_matrix);

#if defined(ROKKO_HAVE_PARALLEL_DENSE_SOLVER) || defined(ROKKO_HAVE_PARALLEL_SPARSE_SOLVER)
  class_<wrap_rokko_grid>("rokko_grid", init<boost::python::object, int>())
    .def("get_myrank", &wrap_rokko_grid::get_myrank)
    .def("get_nprocs", &wrap_rokko_grid::get_nprocs);
  class_<wrap_rokko_parallel_dense_solver>("rokko_parallel_dense_solver",
                                           init<char*, int, char**>())
    .def("diagonalize_distributed_matrix",
         &wrap_rokko_parallel_dense_solver::diagonalize_distributed_matrix);
  class_<wrap_rokko_distributed_matrix>("rokko_distributed_matrix",
    init<int, int, wrap_rokko_grid*, wrap_rokko_parallel_dense_solver*, int>())
    .def("show", &wrap_rokko_distributed_matrix::print)
    .def("set_local", &wrap_rokko_distributed_matrix::set_local)
    .def("get_local", &wrap_rokko_distributed_matrix::get_local)
    .def("set_global", &wrap_rokko_distributed_matrix::set_global)
    .def("get_global", &wrap_rokko_distributed_matrix::get_global)
    .def("get_m_local", &wrap_rokko_distributed_matrix::get_m_local)
    .def("get_n_local", &wrap_rokko_distributed_matrix::get_n_local)
    .def("get_m_global", &wrap_rokko_distributed_matrix::get_m_global)
    .def("get_n_global", &wrap_rokko_distributed_matrix::get_n_global)
    .def("get_nprocs", &wrap_rokko_distributed_matrix::get_nprocs)
    .def("get_myrank", &wrap_rokko_distributed_matrix::get_myrank)
    .def("translate_l2g_row", &wrap_rokko_distributed_matrix::translate_l2g_row)
    .def("translate_l2g_col", &wrap_rokko_distributed_matrix::translate_l2g_col)
    .def("translate_g2l_row", &wrap_rokko_distributed_matrix::translate_g2l_row)
    .def("translate_g2l_col", &wrap_rokko_distributed_matrix::translate_g2l_col);
  class_<wrap_rokko_parallel_sparse_solver>("rokko_parallel_sparse_solver",
                                            init<char*, int, char**>())
    .def("diagonalize_distributed_crs_matrix",
         &wrap_rokko_parallel_sparse_solver::diagonalize_distributed_crs_matrix)
    .def("eigenvalue", &wrap_rokko_parallel_sparse_solver::eigenvalue)
    .def("eigenvector", &wrap_rokko_parallel_sparse_solver::eigenvector)
    .def("num_conv", &wrap_rokko_parallel_sparse_solver::num_conv);
  class_<wrap_rokko_distributed_crs_matrix>("rokko_distributed_crs_matrix",
                                            init<int, int, wrap_rokko_parallel_sparse_solver*>())
    .def("insert", &wrap_rokko_distributed_crs_matrix::insert)
    .def("complete", &wrap_rokko_distributed_crs_matrix::complete)
    .def("num_local_rows", &wrap_rokko_distributed_crs_matrix::num_local_rows)
    .def("start_row", &wrap_rokko_distributed_crs_matrix::start_row)
    .def("end_row", &wrap_rokko_distributed_crs_matrix::end_row)
    .def("show", &wrap_rokko_distributed_crs_matrix::print);
/*
  With a callback:
  def("rokko_distributed_matrix_generate_function", &rokko_distributed_matrix_generate_function);
*/
  def("rokko_gather", &wrap_rokko_gather);
  def("rokko_scatter", &wrap_rokko_scatter);
  def("rokko_all_gather", &wrap_rokko_all_gather);

  def("rokko_frank_matrix_generate_distributed_matrix",
      &wrap_rokko_frank_matrix_generate_distributed_matrix);
#endif
}
