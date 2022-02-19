#ifndef TANUKI_MATH_LINEAR_CHOLESKY_DECOMPOSITION_H
#define TANUKI_MATH_LINEAR_CHOLESKY_DECOMPOSITION_H

#include <cstddef>
#include <memory>

#include <armadillo>
#include <mpi.h>

namespace tanuki {
namespace math {
namespace linear {

/**
 *  @brief Cholesky decomposition of a matrix.
 *
 *  Decomposition is lazily performed when the lower triangular matrix or its
 *  conjugate transpose is requested.
 *
 *  @tparam T
 *    Type of elements in an Armadillo matrix. It must be @link
 *    tanuki::number::real_t @endlink or @link tanuki::number::complex_t
 *    @endlink.
 */
template <typename T>
class CholeskyDecomposition {
 public:
  /**
   *  @param matrix
   *    Matrix to decompose.
   *
   *  @param mpi_comm
   *    MPI communicator.
   */
  CholeskyDecomposition(
      const arma::Mat<T> &matrix, MPI_Comm mpi_comm = MPI_COMM_WORLD);

  CholeskyDecomposition(CholeskyDecomposition &&other) = default;

  /**
   *  @brief Lower triangular matrix.
   */
  const arma::Mat<T> &l();

  /**
   *  @brief Conjugate transpose of the lower triangular matrix.
   */
  const arma::Mat<T> &lt();

  virtual ~CholeskyDecomposition() = default;

 private:
  /**
   *  @brief MPI communicator.
   */
  MPI_Comm mpi_comm_;

  /**
   *  @brief Matrix to decompose.
   */
  arma::Mat<T> matrix_;

  /**
   *  @brief Lower triangular matrix, or <tt>nullptr</tt> if decomposition has
   *  not been done.
   */
  std::unique_ptr<arma::Mat<T>> l_;

  /**
   *  @brief Conjugate transpose of the lower triangular matrix, or
   *  <tt>nullptr</tt> if decomposition has not been done.
   */
  std::unique_ptr<arma::Mat<T>> lt_;
};

} // namespace linear
} // namespace math
} // namespace tanuki

#include "tanuki/math/linear/cholesky_decomposition.hxx"

#endif
