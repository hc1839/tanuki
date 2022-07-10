#ifndef TANUKI_MATH_LINEAR_MATRIX_PRODUCT_H
#define TANUKI_MATH_LINEAR_MATRIX_PRODUCT_H

#include <cstddef>
#include <iterator>
#include <type_traits>

#include <armadillo>
#include <mpi.h>
#include <omp.h>

#include "tanuki/common/divider/group_delimiter.h"

namespace tanuki {
namespace math {
namespace linear {

using arma::Mat;

using tanuki::common::divider::GroupIndices;

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

/**
 *  @brief Evaluates the matrix product, \f$ \mathbf{A} \mathbf{b} \f$, with
 *  OpenMP and MPI parallelization, where \f$ \mathbf{b} \f$ is a diagonal
 *  matrix.
 *
 *  @tparam T
 *    Type of elements in an Armadillo matrix. It must be @link
 *    tanuki::number::real_t @endlink or @link tanuki::number::complex_t
 *    @endlink.
 *
 *  @tparam ForwardIt
 *    Must meet the requirements of <tt>LegacyForwardIterator</tt> and have a
 *    dereferenced type that is convertible to <tt>T</tt>.
 *
 *  @param mpi_comm
 *    MPI communicator.
 *
 *  @param a
 *    \f$ \mathbf{A} \f$.
 *
 *  @param b_first
 *    Beginning of the range of elements that are along the diagonal of \f$
 *    \mathbf{b} \f$. Behavior is undefined if the range has fewer elements
 *    than there are columns in \f$ \mathbf{A} \f$.
 *
 *  @return
 *    \f$ \mathbf{A} \mathbf{b} \f$.
 */
template <
    typename T,
    typename ForwardIt,
    typename std::enable_if<
        std::is_convertible<
            typename std::iterator_traits<ForwardIt>::value_type, T>::value,
        bool>::type = true>
Mat<T> DuoProduct(MPI_Comm mpi_comm, const Mat<T> &a, ForwardIt b_first) {
  // Matrix A with each column multiplied by the corresponding diagonal
  // element.
  Mat<T> retval(arma::size(a));

  // Chunks of column indices grouped by OpenMP thread of this MPI process.
  const auto chunks = GroupIndices(0, a.n_cols, omp_get_max_threads());

  #pragma omp parallel default(shared)
  {
    auto b_it = b_first;
    std::advance(b_it, chunks[omp_get_thread_num()]);

    for (size_t j = chunks[omp_get_thread_num()];
         j != chunks[omp_get_thread_num() + 1];
         ++j) {
      retval.col(j) = a.col(j) * *b_it++;
    }
  }

  return retval;
}

/**
 *  @brief Evaluates the matrix product, \f$ \mathbf{A} \mathbf{b}
 *  \mathbf{A}^{\dagger} \f$, with OpenMP and MPI parallelization, where \f$
 *  \mathbf{b} \f$ is a diagonal matrix.
 *
 *  @tparam T
 *    Type of elements in an Armadillo matrix. It must be @link
 *    tanuki::number::real_t @endlink or @link tanuki::number::complex_t
 *    @endlink.
 *
 *  @tparam ForwardIt
 *    Must meet the requirements of <tt>LegacyForwardIterator</tt> and have a
 *    dereferenced type that is convertible to <tt>T</tt>.
 *
 *  @param mpi_comm
 *    MPI communicator.
 *
 *  @param a
 *    \f$ \mathbf{A} \f$.
 *
 *  @param b_first
 *    Beginning of the range of elements that are along the diagonal of \f$
 *    \mathbf{b} \f$. Behavior is undefined if the range has fewer elements
 *    than there are columns in \f$ \mathbf{A} \f$.
 *
 *  @return
 *    \f$ \mathbf{A} \mathbf{b} \mathbf{A}^{\dagger} \f$.
 */
template <
    typename T,
    typename ForwardIt,
    typename std::enable_if<
        std::is_convertible<
            typename std::iterator_traits<ForwardIt>::value_type, T>::value,
        bool>::type = true>
Mat<T> TrioProduct(MPI_Comm mpi_comm, const Mat<T> &a, ForwardIt b_first) {
  return MatrixProduct(
      mpi_comm, DuoProduct(mpi_comm, a, b_first), Mat<T>(a.t()));
}

} // namespace linear
} // namespace math
} // namespace tanuki

#include "tanuki/math/linear/matrix_product.hxx"

#endif
