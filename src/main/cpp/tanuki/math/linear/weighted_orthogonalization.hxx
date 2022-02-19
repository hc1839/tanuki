#ifndef TANUKI_MATH_LINEAR_WEIGHTED_ORTHOGONALIZATION_HXX
#define TANUKI_MATH_LINEAR_WEIGHTED_ORTHOGONALIZATION_HXX

#include <cassert>

#include "tanuki/parallel/grs/actuator.h"
#include "tanuki/parallel/grs/jacobi_sidedness.h"

namespace tanuki {
namespace math {
namespace linear {

using arma::Col;
using arma::as_scalar;

using math::linear::CreateIdentityRotation;
using parallel::grs::GrsOneSidedRelaxParam;
using parallel::grs::JacobiSidedness;

using tanuki::number::complex_t;

namespace {

  /**
   *  @brief Callback function that evaluates the angle to rotate two columns.
   *
   *  @tparam T
   *    Type of elements in an Armadillo matrix.
   *
   *  @param norm_col1
   *    Nonorthogonal column that is normalized.
   *
   *  @param ortho_col1
   *    Orthonormalized column to rotate. It must have the same number of rows
   *    as there are in <tt>norm_col1</tt>.
   *
   *  @param weight1
   *    Non-negative orthogonalization weight for <tt>ortho_col1</tt>.
   *
   *  @param norm_col2
   *    Another column as that of <tt>norm_col1</tt>.
   *
   *  @param ortho_col2
   *    Another column as that of <tt>ortho_col1</tt> for <tt>norm_col2</tt>.
   *
   *  @param weight2
   *    Another weight as that of <tt>weight1</tt> for <tt>orthonorm_col2</tt>.
   *
   *  @return
   *    Specification of the rotation matrix for <tt>ortho_col1</tt> and
   *    <tt>ortho_col2</tt>, respectively.
   */
  template <typename T>
  RotationMatrixSpec rotate(
      const Col<T> &norm_col1,
      const Col<T> &ortho_col1,
      real_t weight1,
      const Col<T> &norm_col2,
      const Col<T> &ortho_col2,
      real_t weight2,
      real_t zero_abs_thresh) {
    assert(
        norm_col1.n_rows == norm_col2.n_rows &&
        norm_col1.n_rows == ortho_col1.n_rows &&
        norm_col2.n_rows == ortho_col2.n_rows);
    assert(weight1 >= 0.0);
    assert(weight2 >= 0.0);

    // Do not rotate if both weights are close enough to zero.
    if (weight1 < zero_abs_thresh && weight2 < zero_abs_thresh) {
      return CreateIdentityRotation();
    }

    // Intermediate values for the B coefficient.
    const auto B1 = weight1 * as_scalar(norm_col1.t() * ortho_col1);
    const auto B2 = weight2 * as_scalar(norm_col2.t() * ortho_col2);

    // B coefficient.
    const complex_t B = B1 + B2;

    // Intermediate values for the C coefficient.
    const auto C1 = weight2 * as_scalar(norm_col2.t() * ortho_col1);
    const auto C2 = -weight1 * as_scalar(norm_col1.t() * ortho_col2);

    // C coefficient.
    const complex_t C = C1 + C2;

    // A coefficient.
    const auto A = sqrt(B * B + C * C);

    // Do not rotate if the A coefficient is close enough to zero.
    if (abs(A) < zero_abs_thresh) {
      return CreateIdentityRotation();
    }

    return {
      .cosine = (B / A).real(),
      .sine = (C / A).real()
    };
  }

}

template <typename T>
parallel::grs::Result<T> WeightOrthogonalized(
    const Mat<T> &nonortho_matrix,
    const Mat<T> &prelim_ortho_matrix,
    const vector<real_t> &weights,
    real_t zero_abs_thresh,
    Actuator<T> &actuator) {
  assert(!nonortho_matrix.is_empty());
  assert(arma::size(prelim_ortho_matrix) == arma::size(nonortho_matrix));
  assert(weights.size() == nonortho_matrix.n_cols);
  assert(zero_abs_thresh > 0.0);

  // Ensure all weights are non-negative.
  assert(
      all_of(
          weights.begin(), weights.end(),
          [](const real_t &weight) -> bool { return weight >= 0.0; }));

  // Inquiry function for the GRS actuator.
  auto inquiry_fn = [&nonortho_matrix, &weights, zero_abs_thresh](
      const IndexedVectorPair<T> &vectors) -> RotationMatrixSpec {
    const auto &first_idx = vectors.indices.first;
    const auto &second_idx = vectors.indices.second;

    return rotate(
        Col<T>(
            const_cast<T *>(nonortho_matrix.colptr(first_idx)),
            nonortho_matrix.n_rows, false, true),
        vectors.first,
        weights[first_idx],
        Col<T>(
            const_cast<T *>(nonortho_matrix.colptr(second_idx)),
            nonortho_matrix.n_rows, false, true),
        vectors.second,
        weights[second_idx],
        zero_abs_thresh);
  };

  return actuator.Actuate(prelim_ortho_matrix, inquiry_fn);
}

template <typename T>
parallel::grs::Result<T> WeightOrthogonalized(
    MPI_Comm mpi_comm,
    const Mat<T> &nonortho_matrix,
    const Mat<T> &prelim_ortho_matrix,
    const vector<real_t> &weights,
    size_t max_sweeps,
    real_t zero_abs_thresh) {
  assert(max_sweeps > 0);

  int mpi_comm_size;
  MPI_Comm_size(mpi_comm, &mpi_comm_size);

  // Function that determines whether GRS has converged.
  auto convergence_checker =
      [zero_abs_thresh](const Mat<T> &prev, const Mat<T> &curr) -> bool {
        return norm(curr - prev, "fro") < zero_abs_thresh;
      };

  // Initial relaxation parameter for GRS.
  const auto init_relax_param =
      GrsOneSidedRelaxParam(prelim_ortho_matrix.n_cols);

  // Function that halves the relaxation parameter after sweeping each group of
  // rotation sets.
  auto relax_fn = [](size_t, real_t prev_relax, size_t) -> real_t {
    return prev_relax * 0.5;
  };

  // Create the default GRS actuator for WO.
  parallel::grs::Actuator<T> actuator(
      mpi_comm,
      omp_get_max_threads(),
      JacobiSidedness::ONE_SIDED_RIGHT,
      init_relax_param,
      mpi_comm_size,
      max_sweeps,
      relax_fn,
      convergence_checker);

  return WeightOrthogonalized(
      nonortho_matrix,
      prelim_ortho_matrix,
      weights,
      zero_abs_thresh,
      actuator);
}

} // namespace linear
} // namespace math
} // namespace tanuki

#endif
