#ifndef TANUKI_MATH_LINEAR_MATRIX_PRODUCT_H
#define TANUKI_MATH_LINEAR_MATRIX_PRODUCT_H

#include <cstddef>

#include <armadillo>
#include <mpi.h>

namespace tanuki {
namespace math {
namespace linear {

using arma::Mat;

/**
 *  @brief Multiplication of Armadillo matrices with parallelization across MPI
 *  processes.
 *
 *  It cannot be invoked in an OpenMP parallel region.
 *
 *  @tparam T
 *    Type of elements in Armadillo matrices. It must be supported by
 *    <tt>arma::Mat</tt> and @link tanuki::parallel::mpi::MpiBasicDatatype
 *    @endlink.
 *
 *  @tparam Tmats
 *    Types where each is <tt>arma::Mat&lt;T&gt;</tt>.
 *
 *  @param mpi_comm
 *    MPI communicator.
 *
 *  @param a
 *    First matrix.
 *
 *  @param b
 *    Second matrix.
 *
 *  @param mats
 *    Remaining matrices.
 *
 *  @return
 *    Product of <tt>a</tt>, <tt>b</tt>, and <tt>mats</tt> in order.
 */
template <typename T, typename... Tmats>
Mat<T> MatrixProduct(
    MPI_Comm mpi_comm, const Mat<T> &a, const Mat<T> &b, Tmats... mats);

} // namespace linear
} // namespace math
} // namespace tanuki

#include "tanuki/math/linear/matrix_product.hxx"

#endif
