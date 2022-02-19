#ifndef TANUKI_MATH_LINEAR_ITERATED_GRAM_SCHMIDT_HXX
#define TANUKI_MATH_LINEAR_ITERATED_GRAM_SCHMIDT_HXX

#include <cassert>
#include <utility>
#include <vector>

#include "tanuki/common/divider/group_delimiter.h"
#include "tanuki/math/linear/matrix_product.h"
#include "tanuki/parallel/mpi/mpi_basic_datatype.h"

namespace tanuki {
namespace math {
namespace linear {

using std::vector;

using arma::Col;
using arma::Mat;
using arma::uvec;
using arma::uword;

using tanuki::parallel::mpi::MpiBasicDatatype;

/**
 *  @brief Internal class for iterated Gram-Schmidt process.
 *
 *  @private
 */
struct IteratedGramSchmidtImpl final {
 public:
  IteratedGramSchmidtImpl() = delete;

  template <typename T>
  friend QrDecomposition<T> IteratedGramSchmidt(
      MPI_Comm mpi_comm,
      const Mat<T> &matrix,
      real_t reortho_thresh_factor,
      size_t max_reorthos,
      real_t zero_norm_abs_thresh);

 private:
  /**
   *  @brief Performs iterated CGS in place on a block (which is a contiguous
   *  subset of columns) in an Armadillo matrix.
   *
   *  @tparam T
   *    Type of elements in the Armadillo matrix.
   *
   *  @param matrix
   *    Matrix to orthonormalize.
   *
   *  @param first
   *    Index of the first column in the subset to orthonormalize.
   *
   *  @param last
   *    Index of the past-the-end column in the subset to orthonormalize. It
   *    must be greater than or equal to <tt>first</tt>.
   *
   *  @param reortho_thresh_factor
   *    See @link IteratedGramSchmidt @endlink.
   *
   *  @param max_reorthos
   *    See @link IteratedGramSchmidt @endlink.
   */
  template <typename T>
  static void OrthonormalizeBlock(
      Mat<T> &matrix,
      size_t first,
      size_t last,
      real_t reortho_thresh_factor,
      size_t max_reorthos) {
    assert(matrix.n_rows >= matrix.n_cols);
    assert(last >= first);
    assert(first >= 0 && last <= matrix.n_cols);

    if (last == first) {
      return;
    }

    // Normalize the first column in the block.
    matrix.col(first) /= arma::norm(matrix.col(first));

    // Iterate over the kets to be orthogonalized, starting from the second ket
    // in the block.
    for (size_t k = first + 1; k != last; ++k) {
      auto q_k = matrix.col(k);

      real_t q_k_pre_norm = arma::norm(q_k);
      real_t q_k_post_norm;

      size_t p = 0;

      // Orthogonalize the ket with reorthogonalizations as needed.
      do {
        // Orthogonalize the current ket through projection and subtraction.
        {
          const Mat<T> q_sub(matrix.submat(0, first, matrix.n_rows - 1, k - 1));
          const Col<T> s_k(q_sub.t() * q_k);
          const Col<T> qrk(q_sub * s_k);

          q_k -= qrk;
        }

        q_k_post_norm = arma::norm(q_k);

        // Decide whether to continue with reorthogonalization.
        if (q_k_post_norm > q_k_pre_norm * reortho_thresh_factor) {
          break;
        } else {
          q_k_pre_norm = q_k_post_norm;
        }
      } while (p++ != max_reorthos);

      // Normalize the orthogonalized ket.
      q_k /= q_k_post_norm;
    }
  }
};

template <typename T>
QrDecomposition<T> IteratedGramSchmidt(
    MPI_Comm mpi_comm,
    const Mat<T> &matrix,
    real_t reortho_thresh_factor,
    size_t max_reorthos,
    real_t zero_norm_abs_thresh) {
  assert(matrix.n_rows >= matrix.n_cols);
  assert(zero_norm_abs_thresh > 0.0);

  int mpi_rank;
  MPI_Comm_rank(mpi_comm, &mpi_rank);

  int mpi_comm_size;
  MPI_Comm_size(mpi_comm, &mpi_comm_size);

  const auto &a = matrix;

  QrDecomposition<T> retval;
  auto &q = retval.q;
  auto &r = retval.r;

  if (a.is_square()) {
    q = a;
  } else {
    // Append an identity matrix to the right.
    q = arma::join_horiz(a, arma::eye<Mat<T>>(a.n_rows, a.n_rows - a.n_cols));
  }

  const auto block_idxs = common::divider::GroupIndices(
      0, q.n_cols, mpi_comm_size);

  // Index range of the block associated with the rank of this MPI process.
  const size_t rank_b_first = block_idxs[mpi_rank];
  const size_t rank_b_last = block_idxs[mpi_rank + 1];

  // Sequentially orthonormalize each block, and update the remaining blocks.
  for (int ortho_rank = 0; ortho_rank != mpi_comm_size; ++ortho_rank) {
    // Index range of the block associated with the rank that is
    // orthonormalizing it.
    const size_t ortho_b_first = block_idxs[ortho_rank];
    const size_t ortho_b_last = block_idxs[ortho_rank + 1];

    if (ortho_b_last == ortho_b_first) {
      break;
    }

    if (mpi_rank == ortho_rank) {
      IteratedGramSchmidtImpl::OrthonormalizeBlock(
          q, ortho_b_first, ortho_b_last,
          reortho_thresh_factor, max_reorthos);
    }

    // Broadcast the orthonormal block.
    MPI_Bcast(
        q.colptr(ortho_b_first),
        (ortho_b_last - ortho_b_first) * q.n_rows,
        MpiBasicDatatype<T>(),
        ortho_rank,
        mpi_comm);

    if (mpi_rank != ortho_rank && rank_b_last != rank_b_first) {
      // Block associated with this MPI process.
      auto rank_block = q.submat(
          0, rank_b_first, q.n_rows - 1, rank_b_last - 1);

      // Orthonormalized block.
      const auto ortho_block = q.submat(
          0, ortho_b_first, q.n_rows - 1, ortho_b_last - 1);

      rank_block -= ortho_block * ortho_block.t() * rank_block;
    }
  }

  // Remove zero columns that come from the orthogonalization of the appended
  // identity matrix.
  if (!q.is_square()) {
    {
      vector<uword> zero_col_idxs;
      for (uword j = a.n_cols; j != q.n_cols; ++j) {
        if (arma::norm(q.col(j)) < zero_norm_abs_thresh) {
          zero_col_idxs.push_back(j);
        }
      }
      q.shed_cols(uvec(std::move(zero_col_idxs)));
    }

    // Remove any extraneous columns to make it a square matrix.
    if (!q.is_square()) {
      q.shed_cols(q.n_rows, q.n_cols - 1);
    }
  }

  r = MatrixProduct(mpi_comm, Mat<T>(q.t()), a);

  if (!r.is_square()) {
    r.rows(r.n_cols, r.n_rows - 1).zeros();
  }

  return retval;
}

} // namespace linear
} // namespace math
} // namespace tanuki

#endif
