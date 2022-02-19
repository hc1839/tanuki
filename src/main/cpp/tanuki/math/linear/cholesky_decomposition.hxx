#ifndef TANUKI_MATH_LINEAR_CHOLESKY_DECOMPOSITION_HXX
#define TANUKI_MATH_LINEAR_CHOLESKY_DECOMPOSITION_HXX

#include <algorithm>
#include <cassert>
#include <cmath>
#include <complex>
#include <utility>
#include <vector>

#include "tanuki/common/divider/group_delimiter.h"
#include "tanuki/parallel/mpi/mpi_basic_datatype.h"

namespace tanuki {
namespace math {
namespace linear {

using std::complex;
using std::shared_ptr;
using std::unique_ptr;
using std::vector;

using arma::Col;
using arma::Mat;

using tanuki::parallel::mpi::MpiBasicDatatype;

template <typename T>
CholeskyDecomposition<T>::CholeskyDecomposition(
    const Mat<T> &matrix, MPI_Comm mpi_comm)
        : matrix_(matrix),
          mpi_comm_(mpi_comm) {
  assert(!matrix_.is_empty());
  assert(matrix_.is_square());
}

template <typename T>
const Mat<T> &CholeskyDecomposition<T>::l() {
  if (l_ != nullptr) {
    return *l_;
  }

  int mpi_rank;
  MPI_Comm_rank(mpi_comm_, &mpi_rank);

  int mpi_comm_size;
  MPI_Comm_size(mpi_comm_, &mpi_comm_size);

  const auto &a = matrix_;
  Mat<T> lower = arma::trimatl(a);

  // Initialize the first column.
  {
    const auto first_element = std::sqrt(a(0, 0));
    lower.col(0) /= first_element;
    lower(0, 0) = first_element;
  }

  for (size_t j = 1; j != lower.n_cols; ++j) {
    const auto part_j_row = lower.submat(j, 0, j, j - 1);

    lower(j, j) = std::sqrt(
        a(j, j) - as_scalar(part_j_row * part_j_row.t()));

    // Indices of row groups across MPI processes.
    const auto row_groups = common::divider::GroupIndices(
        j + 1, lower.n_rows, mpi_comm_size);

    {
      const size_t row_first = row_groups[mpi_rank];
      const size_t row_last = row_groups[mpi_rank + 1];

      if (row_last != row_first) {
        // View of part of the column below the diagonal for this MPI process.
        auto below = lower.submat(row_first, j, row_last - 1, j);

        below -= lower.submat(row_first, 0, row_last - 1, j - 1) *
            part_j_row.t();

        below /= lower(j, j);
      }
    }

    // Broadcast the column below the diagonal.
    for (int rank = 0; rank != mpi_comm_size; ++rank) {
      const size_t row_first = row_groups[rank];
      const size_t row_last = row_groups[rank + 1];

      if (row_last == row_first) {
        break;
      }

      vector<T> part_col_buf(row_last - row_first);
      auto below = lower.submat(row_first, j, row_last - 1, j);

      // Store the part of the column solved by this MPI process in the
      // broadcast buffer.
      if (rank == mpi_rank) {
        std::copy(below.begin(), below.end(), part_col_buf.begin());
      }

      MPI_Bcast(
          part_col_buf.data(),
          part_col_buf.size(),
          MpiBasicDatatype<T>(),
          rank,
          mpi_comm_);

      // Get the part of the column solved by another MPI process.
      if (rank != mpi_rank) {
        std::copy(part_col_buf.begin(), part_col_buf.end(), below.begin());
      }
    }
  }

  l_.reset(new Mat<T>(std::move(lower)));

  return *l_;
}

template <typename T>
const Mat<T> &CholeskyDecomposition<T>::lt() {
  if (lt_ != nullptr) {
    return *lt_;
  }

  lt_.reset(new Mat<T>(l().t()));

  return *lt_;
}

} // namespace linear
} // namespace math
} // namespace tanuki

#endif
