#ifndef TANUKI_MATH_LINEAR_MATRIX_PRODUCT_HXX
#define TANUKI_MATH_LINEAR_MATRIX_PRODUCT_HXX

#include <algorithm>
#include <cassert>
#include <type_traits>
#include <utility>
#include <vector>

#include <omp.h>

#include "tanuki/common/divider/group_delimiter.h"
#include "tanuki/parallel/mpi/mpi_basic_datatype.h"

namespace tanuki {
namespace math {
namespace linear {

using tanuki::common::divider::GroupIndices;
using tanuki::parallel::mpi::MpiBasicDatatype;

template <typename T>
Mat<T> MatrixProduct(MPI_Comm mpi_comm, arma::Mat<T> a) {
  return a;
}

template <typename T, typename... Tmats>
Mat<T> MatrixProduct(
    MPI_Comm mpi_comm, const Mat<T> &a, const Mat<T> &b, Tmats... mats) {
  assert(!omp_in_parallel());
  assert(a.n_cols == b.n_rows);

  int mpi_rank;
  MPI_Comm_rank(mpi_comm, &mpi_rank);

  int mpi_comm_size;
  MPI_Comm_size(mpi_comm, &mpi_comm_size);

  const auto batch_idxs = GroupIndices(0, b.n_cols, mpi_comm_size);
  Mat<T> retval(a.n_rows, b.n_cols);

  // Perform matrix multiplication on the column block associated with this MPI
  // process.
  {
    const size_t col_idx_first = batch_idxs[mpi_rank];
    const size_t col_idx_last = batch_idxs[mpi_rank + 1];

    if (col_idx_last != col_idx_first) {
      retval.cols(col_idx_first, col_idx_last - 1) =
          a * b.cols(col_idx_first, col_idx_last - 1);
    }
  }

  // Broadcast column blocks.
  for (int rank = 0; rank != mpi_comm_size; ++rank) {
    // Number of columns in the block.
    const size_t num_cols = batch_idxs[rank + 1] - batch_idxs[rank];

    MPI_Bcast(
        retval.colptr(batch_idxs[rank]),
        retval.n_rows * num_cols,
        MpiBasicDatatype<T>(),
        rank,
        mpi_comm);
  }

  return MatrixProduct(mpi_comm, std::move(retval), mats...);
}

} // namespace linear
} // namespace math
} // namespace tanuki

#endif
