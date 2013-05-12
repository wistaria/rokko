#ifndef ROKKO_DISTRIBUTED_H
#define ROKKO_DISTRIBUTED_H

#include <iostream>
#include <cstdlib>
#include <rokko/grid.hpp>
#include <boost/type_traits/is_same.hpp>

namespace rokko {

class solver;

struct matrix_row_major {};

struct matrix_col_major {};

template<typename MATRIX_MAJOR = rokko::matrix_row_major>
class distributed_matrix {
public:
  /*
  template<typename GRID_MAJOR>
  distributed_matrix(int m_global_in, int n_global_in, const grid<GRID_MAJOR>& g_in)
    : m_global(m_global_in), n_global(n_global_in), myrank(g_in.myrank), nprocs(g_in.nprocs), myrow(g_in.myrow), mycol(g_in.mycol), nprow(g_in.nprow), npcol(g_in.npcol), g(g_in) {

    // ローカル行列の形状を指定
    mb = m_global / nprow;
    if (mb == 0) mb = 1;
    //mb = 10;
    nb = n_global / npcol;
    if (nb == 0) nb = 1;
    //nb = 10;
    // mbとnbを最小値にそろえる．（注意：pdsyevではmb=nbでなければならない．）
    //mb = min(mb, nb);
    mb = nb = 1;

    m_local = calculate_row_size();
    n_local = calculate_col_size();
    lld = get_default_lld();

#ifndef NDEBUG
    for (int proc=0; proc<nprocs; ++proc) {
      if (proc == myrank) {
	std::cout << "proc=" << proc << std::endl;
	std::cout << "  mb=" << mb << "  nb=" << nb << std::endl;
	std::cout << "  mA=" << m_local << "  nprow=" << nprow << std::endl;
	std::cout << "  nA=" << n_local << "  npcol=" << npcol << std::endl;
	std::cout << " m_local=" << m_local << " n_local=" << n_local << std::endl;
        std::cout << " myrow=" << myrow << " mycol=" << mycol << std::endl;
      }
      MPI_Barrier(g.get_comm());
    }
#endif

    array = new double[m_local * n_local];
    if (array == 0) {
      std::cerr << "failed to allocate array." << std::endl;
      MPI_Abort(MPI_COMM_WORLD, 3);
    }
  }
  */

  template<typename GRID_MAJOR>
  distributed_matrix(int m_global_in, int n_global_in, const grid<GRID_MAJOR>& g_in, rokko::solver& solver_in)
    : m_global(m_global_in), n_global(n_global_in), myrank(g_in.myrank), nprocs(g_in.nprocs), myrow(g_in.myrow), mycol(g_in.mycol), nprow(g_in.nprow), npcol(g_in.npcol), g(g_in) {
    // Determine mb, nb, lld, larray
    solver_in.optimized_matrix_size(*this);

#ifndef NDEBUG
    for (int proc=0; proc<nprocs; ++proc) {
      if (proc == myrank) {
	std::cout << "proc=" << proc << std::endl;
	std::cout << "  mb=" << mb << "  nb=" << nb << std::endl;
	std::cout << "  nprow=" << nprow << "  npcol=" << npcol << std::endl;
	std::cout << "  m_local=" << m_local << " n_local=" << n_local << std::endl;
        std::cout << "  myrow=" << myrow << " mycol=" << mycol << std::endl;
        std::cout << "  lld=" << lld << std::endl;
        std::cout << "  length_array=" << length_array << std::endl;
      }
      MPI_Barrier(g.get_comm());
    }
#endif

    array = new double[length_array];
    if (array == 0) {
      std::cerr << "failed to allocate array." << std::endl;
      MPI_Abort(g.get_comm(), 3);
    }
  }

  ~distributed_matrix() {
    delete[] array;
    array = 0;
  }

  double* get_array_pointer() { return array; }

  int get_length_array() const { return length_array; }

  void set_length_array(int value) { length_array = value; }
  void set_block_size(int mb_in, int nb_in) {
    mb = mb_in;
    nb = nb_in;
  }

  int get_mb() const { return mb; }
  int get_nb() const { return nb; }

  int get_nprow() const { return nprow; }
  int get_npcol() const { return npcol; }
  int get_nprocs() const { return nprocs; }

  int get_m_global() const { return m_global; }
  int get_n_global() const { return n_global; }

  void set_local_size(int m_local_in, int n_local_in) {
    m_local = m_local_in;
    n_local = n_local_in;
  }

  void set_default_local_size() {
    MPI_Barrier(g.get_comm());
    set_local_size(calculate_row_size(), calculate_col_size());
  }

  int calculate_row_size() const {
    int tmp = m_global / mb;
    int local_num_block_rows = tmp / nprow;
    int rest_block_row = tmp % nprow; // 最後のブロックを持つプロセスの次のプロセス，余りのあるブロックを持つプロセス
    if (myrow < rest_block_row)  ++local_num_block_rows;
    //local_num_block_rows = (tmp - myrow -1) / nprow + 1;

    //std::cout << "local_num_block_rows=" << local_num_block_rows << std::endl;
    int local_rest_block_rows;
    if (myrow == rest_block_row)
      local_rest_block_rows = m_global % mb;
    else
      local_rest_block_rows = 0;

    MPI_Barrier(g.get_comm());
    for (int proc=0; proc<nprocs; ++proc) {
      if (proc == myrank) {
        printf("Rank = %d  myrow=%d mycol=%d\n", myrank, myrow, mycol);
        std::cout << "local_num_block_rows=" << local_num_block_rows
                  << "  local_rest_block_rows=" << local_rest_block_rows
                  << "  rest_block_row=" << rest_block_row << std::endl;
      }
      MPI_Barrier(g.get_comm());
    }

    return  local_num_block_rows * mb + local_rest_block_rows;
  }

  int calculate_col_size() const {
    int tmp = n_global / nb;
    int local_num_block_cols = tmp / npcol;
    int rest_block_col = tmp % npcol; // 最後のブロックを持つプロセスの次のプロセス
    if (mycol < rest_block_col)  ++local_num_block_cols;
    int local_rest_block_cols;
    if (mycol == rest_block_col) {
      local_rest_block_cols = n_global % nb;
    }
    else
      local_rest_block_cols = 0;

    MPI_Barrier(g.get_comm());
    for (int proc=0; proc<nprocs; ++proc) {
      if (proc == myrank) {
        printf("Rank = %d  myrow=%d mycol=%d\n", myrank, myrow, mycol);
        std::cout << "local_num_block_cols=" << local_num_block_cols
                  << "  local_rest_block_cols=" << local_rest_block_cols
                  << "  rest_block_col=" << rest_block_col << std::endl;
      }
      MPI_Barrier(g.get_comm());
    }
    return  local_num_block_cols * nb + local_rest_block_cols;
  }

  int get_lld() const { return lld; };

  void set_lld(int value) { lld = value; };
  void set_default_lld() { set_lld(get_default_lld()); }

  int get_default_lld() const;

  int get_default_length_array() const;
  void set_default_length_array() { set_length_array(get_default_length_array()); }


  int get_array_index(int local_i, int local_j) const;

  int translate_l2g_row(const int& local_i) const {
    return (myrow * mb) + local_i + (local_i / mb) * mb * (nprow - 1);
  }

  int translate_l2g_col(const int& local_j) const {
    return (mycol * nb) + local_j + (local_j / nb) * nb * (npcol - 1);
  }

  int translate_g2l_row(const int& global_i) const {
    const int local_offset_block = global_i / mb;
    return (local_offset_block - myrow) / nprow * mb + global_i % mb;
  }

  int translate_g2l_col(const int& global_j) const {
    const int local_offset_block = global_j / nb;
    return (local_offset_block - mycol) / npcol * nb + global_j % nb;
  }

  bool is_gindex_myrow(const int& global_i) const {
    int local_offset_block = global_i / mb;
    return (local_offset_block % nprow) == myrow;
  }

  bool is_gindex_mycol(const int& global_j) const {
    int local_offset_block = global_j / nb;
    return (local_offset_block % npcol) == mycol;
  }

  void set_local(int local_i, int local_j, double value) {
    array[get_array_index(local_i, local_j)] = value;
  }

  void set_global(int global_i, int global_j, double value) {
    if ((is_gindex_myrow(global_i)) && (is_gindex_mycol(global_j)))
      set_local(translate_g2l_row(global_i), translate_g2l_col(global_j), value);
  }

  bool is_row_major() const {
    return boost::is_same<MATRIX_MAJOR, matrix_row_major>::value;
  }
  bool is_col_major() const {
    return boost::is_same<MATRIX_MAJOR, matrix_col_major>::value;
  }

  void print() const {
    /* each proc prints it's local_array out, in order */
    for (int proc=0; proc<nprocs; ++proc) {
      if (proc == myrank) {
	printf("Rank = %d  myrow=%d mycol=%d\n", myrank, myrow, mycol);
	printf("Local Matrix:\n");
	for (int local_i=0; local_i<m_local; ++local_i) {
	  for (int local_j=0; local_j<n_local; ++local_j) {
	    printf("%e ",array[get_array_index(local_i, local_j)]);
	  }
	  printf("\n");
	}
	printf("\n");
      }
      MPI_Barrier(g.get_comm());
    }
  }

  int m_global, n_global;
  double* array;
  int mb, nb;
  int m_local, n_local;
  // variables of class Grid
  int myrank, nprocs;
  int myrow, mycol;
  int nprow, npcol;
  int lld;
  int length_array;

  const grid_base& g;

private:
  ///int info;
};


template<>
inline int distributed_matrix<rokko::matrix_row_major>::get_default_lld() const {
  return n_local;
}

template<>
inline int distributed_matrix<rokko::matrix_col_major>::get_default_lld() const {
  return m_local;
}

template<>
inline int distributed_matrix<rokko::matrix_row_major>::get_default_length_array() const {
  return m_local * lld;
}

template<>
inline int distributed_matrix<rokko::matrix_col_major>::get_default_length_array() const {
  return lld * n_local;
}

template<>
inline int distributed_matrix<rokko::matrix_row_major>::get_array_index(int local_i, int local_j) const {
  return local_i * lld + local_j;
}

template<>
inline int distributed_matrix<rokko::matrix_col_major>::get_array_index(int local_i, int local_j) const {
  return  local_i + local_j * lld;
}


template<typename MATRIX_MAJOR>
void print_matrix(const rokko::distributed_matrix<MATRIX_MAJOR>& mat) {
  // each proc prints it's local_array out, in order
  for (int proc=0; proc<mat.nprocs; ++proc) {
    if (proc == mat.myrank) {
      printf("Rank = %d  myrow=%d mycol=%d\n", mat.myrank, mat.myrow, mat.mycol);
      printf("Local Matrix:\n");
      for (int local_i=0; local_i<mat.m_local; ++local_i) {
	for (int local_j=0; local_j<mat.n_local; ++local_j) {
	  printf("%e ", mat.array[mat.get_array_index(local_i, local_j)]);
	}
	printf("\n");
      }
      printf("\n");
    }
    MPI_Barrier(g.get_comm());
  }
}

} // namespace rokko

#endif // ROKKO_DISTRIBUTED_H
