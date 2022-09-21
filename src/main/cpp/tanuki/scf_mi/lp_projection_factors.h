#ifndef TANUKI_SCF_MI_LP_PROJECTION_FACTORS_H
#define TANUKI_SCF_MI_LP_PROJECTION_FACTORS_H

#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include <armadillo>
#include <mpi.h>

#include "tanuki/algorithm/algorithm.h"
#include "tanuki/math/linear/eigen.h"
#include "tanuki/math/linear/matrix_product.h"
#include "tanuki/number/types.h"
#include "tanuki/scf_mi/projection_factors.h"

namespace tanuki {
namespace scf_mi {

using std::vector;

using arma::Col;
using arma::Mat;
using arma::uvec;

using tanuki::algorithm::StableIndexSort;
using tanuki::math::linear::EigSolver;
using tanuki::math::linear::MatrixProduct;
using tanuki::number::real_t;

template <typename T>
using LpProjectionFactors = ProjectionFactors<ScfMiMethod::LP, T>;

/**
 *  @brief Creates the effective Hamiltonians of LP SCF MI for each unit.
 *
 *  @tparam T
 *    Type of numbers in the bra-ket matrices. It must be @link
 *    tanuki::number::real_t @endlink or @link tanuki::number::complex_t
 *    @endlink.
 *
 *  @tparam OutputIt
 *    Must meet the requirements of <tt>LegacyOutputIterator</tt> and have a
 *    dereferenced type that is convertible to <tt>arma::Mat</tt>.
 *
 *  @param mpi_comm
 *    MPI communicator.
 *
 *  @param proj_factors
 *    Projection factors. It must not be empty.
 *
 *  @param sys_h_op
 *    System Hamiltonian operator in bra-ket matrix. Its size must agree with
 *    the factors in <tt>proj_factors</tt>.
 *
 *  @param d_eff_h_op_first
 *    Beginning of the destination range of effective Hamiltonian operators in
 *    bra-ket matrix for each unit.
 */
template <
    typename T, typename OutputIt,
    typename std::enable_if<
        std::is_convertible<
            typename std::iterator_traits<OutputIt>::value_type,
            Mat<T>>::value,
        bool>::type = true>
void ScfMiHamiltonians(
    MPI_Comm mpi_comm,
    const LpProjectionFactors<T> &proj_factors,
    const Mat<T> &sys_h_op,
    OutputIt d_eff_h_op_first);

/**
 *  @brief Solves the eigenvalue equations of LP SCF MI for each unit.
 *
 *  @tparam T
 *    Type of numbers in the bra-ket matrices. It must be @link
 *    tanuki::number::real_t @endlink or @link tanuki::number::complex_t
 *    @endlink.
 *
 *  @tparam InputIt
 *    Must meet the requirements of <tt>LegacyInputIterator</tt> and have a
 *    dereferenced type that is convertible to <tt>arma::Mat</tt>.
 *
 *  @tparam OutputIt1
 *    Must meet the requirements of <tt>LegacyOutputIterator</tt> and have a
 *    dereferenced type that is convertible to <tt>arma::Mat</tt>.
 *
 *  @tparam OutputIt2
 *    Must meet the requirements of <tt>LegacyOutputIterator</tt> and have a
 *    dereferenced type that is convertible to
 *    <tt>arma::Col&lt;real_t&gt;</tt>.
 *
 *  @tparam OutputIt3
 *    Must meet the requirements of <tt>LegacyOutputIterator</tt> and have a
 *    dereferenced type that is convertible to <tt>arma::Mat</tt>.
 *
 *  @param mpi_comm
 *    MPI communicator.
 *
 *  @param proj_factors
 *    Projection factors. It must not be empty.
 *
 *  @param sys_h_op
 *    System Hamiltonian operator in bra-ket matrix. Its size must agree with
 *    the factors in <tt>proj_factors</tt>.
 *
 *  @param unit_basis_first
 *    Beginning of the range of ket matrices of basis for each unit.
 *
 *  @param d_eff_h_op_first
 *    Beginning of the destination range of effective Hamiltonian operators in
 *    bra-ket matrix for each unit.
 *
 *  @param d_mo_energies_first
 *    Beginning of the destination range of ascending energies of spatial
 *    molecular orbitals for each unit.
 *
 *  @param d_mos_first
 *    Beginning of the destination range of ket matrices of spatial molecular
 *    orbitals in ascending order by energy for each unit.
 *
 *  @param eig_solver
 *    Function that solves a generalized eigenvalue equation.
 */
template <
    typename T,
    typename InputIt,
    typename OutputIt1,
    typename OutputIt2,
    typename OutputIt3,
    typename std::enable_if<
        std::is_convertible<
            typename std::iterator_traits<InputIt>::value_type,
            Mat<T>>::value,
        bool>::type = true,
    typename std::enable_if<
        std::is_convertible<
            typename std::iterator_traits<OutputIt1>::value_type,
            Mat<T>>::value,
        bool>::type = true,
    typename std::enable_if<
        std::is_convertible<
            typename std::iterator_traits<OutputIt2>::value_type,
            Col<real_t>>::value,
        bool>::type = true,
    typename std::enable_if<
        std::is_convertible<
            typename std::iterator_traits<OutputIt3>::value_type,
            Mat<T>>::value,
        bool>::type = true>
void SolveScfMi(
    MPI_Comm mpi_comm,
    const LpProjectionFactors<T> &proj_factors,
    const Mat<T> &sys_h_op,
    InputIt unit_basis_first,
    OutputIt1 d_eff_h_op_first,
    OutputIt2 d_mo_energies_first,
    OutputIt3 d_mos_first,
    EigSolver<T> eig_solver);

/**
 *  @brief Factors in the effective Hamiltonians of LP SCF MI for each unit.
 */
template <typename T>
struct ProjectionFactors<ScfMiMethod::LP, T> final {
 public:
  ProjectionFactors() = default;

  /**
   *  @tparam InputIt
   *    Must meet the requirements of <tt>LegacyInputIterator</tt> and have a
   *    dereferenced type that is convertible to <tt>arma::Mat</tt>.
   *
   *  @param mpi_comm
   *    MPI communicator.
   *
   *  @param ortho_unitx_mos_first
   *    Beginning of the range of ket matrices of occupied orthonormal spatial
   *    molecular orbitals, where each ket matrix excludes the corresponding
   *    unit and orthogonality does not span the excluded unit. All ket
   *    matrices must have the same number of rows. The range must not be
   *    empty.
   *
   *  @param ortho_unitx_mos_last
   *    Past-the-end of the range of ket matrices of occupied orthonormal
   *    spatial molecular orbitals.
   */
  template <
      typename InputIt,
      typename std::enable_if<
          std::is_convertible<
              typename std::iterator_traits<InputIt>::value_type,
              Mat<T>>::value,
          bool>::type = true>
  ProjectionFactors(
      MPI_Comm mpi_comm,
      InputIt ortho_unitx_mos_first,
      InputIt ortho_unitx_mos_last) {
    if (ortho_unitx_mos_first == ortho_unitx_mos_last) {
      throw std::logic_error(
          "Range of orthonormal unit molecular orbitals is empty.");
    }

    // Identity matrix of the matching size.
    const Mat<T> eye(
        static_cast<const Mat<T> &>(*ortho_unitx_mos_first).n_rows,
        static_cast<const Mat<T> &>(*ortho_unitx_mos_first).n_rows,
        arma::fill::eye);

    // Create projection operators for each excluded unit.
    for (auto it = ortho_unitx_mos_first; it != ortho_unitx_mos_last; ++it) {
      const Mat<T> &ortho_unitx_mos = *it;

      const auto projx_op = MatrixProduct(
          mpi_comm, ortho_unitx_mos, Mat<T>(ortho_unitx_mos.t()));

      this->proj_ops_.push_back(eye - projx_op);
    }
  }

  template <
      typename T, typename OutputIt,
      typename std::enable_if<
          std::is_convertible<
              typename std::iterator_traits<OutputIt>::value_type,
              Mat<T>>::value,
          bool>::type>
  friend void ScfMiHamiltonians(
      MPI_Comm mpi_comm,
      const LpProjectionFactors<T> &proj_factors,
      const Mat<T> &sys_h_op,
      OutputIt d_eff_h_op_first);

  template <
      typename T,
      typename InputIt,
      typename OutputIt1,
      typename OutputIt2,
      typename OutputIt3,
      typename std::enable_if<
          std::is_convertible<
              typename std::iterator_traits<InputIt>::value_type,
              Mat<T>>::value,
          bool>::type,
      typename std::enable_if<
          std::is_convertible<
              typename std::iterator_traits<OutputIt1>::value_type,
              Mat<T>>::value,
          bool>::type,
      typename std::enable_if<
          std::is_convertible<
              typename std::iterator_traits<OutputIt2>::value_type,
              Col<real_t>>::value,
          bool>::type,
      typename std::enable_if<
          std::is_convertible<
              typename std::iterator_traits<OutputIt3>::value_type,
              Mat<T>>::value,
          bool>::type>
  friend void SolveScfMi(
      MPI_Comm mpi_comm,
      const LpProjectionFactors<T> &proj_factors,
      const Mat<T> &sys_h_op,
      InputIt unit_basis_first,
      OutputIt1 d_eff_h_op_first,
      OutputIt2 d_mo_energies_first,
      OutputIt3 d_mos_first,
      EigSolver<T> eig_solver);

 private:
  /**
   *  @brief Projection operators for each unit.
   *
   *  For a unit \f$ J \f$, it is defined as \f$ 1 - \hat{P}_{J'} \f$, where
   *  \f$ \hat{P}_{J'} \f$ is the projection operator created from orthonormal
   *  molecular orbitals not in \f$ J \f$ and orthogonality does not span \f$ J
   *  \f$. Occupation numbers are used as orthogonalization weights.
   */
  vector<Mat<T>> proj_ops_;
};

template <
    typename T, typename OutputIt,
    typename std::enable_if<
        std::is_convertible<
            typename std::iterator_traits<OutputIt>::value_type,
            Mat<T>>::value,
        bool>::type>
void ScfMiHamiltonians(
    MPI_Comm mpi_comm,
    const LpProjectionFactors<T> &proj_factors,
    const Mat<T> &sys_h_op,
    OutputIt d_eff_h_op_first) {
  const auto &proj_ops = proj_factors.proj_ops_;

  if (proj_ops.empty()) {
    throw std::logic_error("Projection factors are empty.");
  }

  const size_t num_units = proj_ops.size();

  auto proj_op_it = proj_ops.begin();
  auto d_eff_h_op_it = d_eff_h_op_first;

  // Create and output effective Hamiltonian operators for each unit.
  for (size_t unit_idx = 0; unit_idx != num_units; ++unit_idx) {
    const auto &proj_op = *proj_op_it++;

    // Effective Hamiltonian operator.
    auto eff_h_op = MatrixProduct(mpi_comm, proj_op, sys_h_op, proj_op);

    // Output effective Hamiltonian operator.
    static_cast<Mat<T> &>(*d_eff_h_op_it++) = std::move(eff_h_op);
  }
}

template <
    typename T,
    typename InputIt,
    typename OutputIt1,
    typename OutputIt2,
    typename OutputIt3,
    typename std::enable_if<
        std::is_convertible<
            typename std::iterator_traits<InputIt>::value_type,
            Mat<T>>::value,
        bool>::type,
    typename std::enable_if<
        std::is_convertible<
            typename std::iterator_traits<OutputIt1>::value_type,
            Mat<T>>::value,
        bool>::type,
    typename std::enable_if<
        std::is_convertible<
            typename std::iterator_traits<OutputIt2>::value_type,
            Col<real_t>>::value,
        bool>::type,
    typename std::enable_if<
        std::is_convertible<
            typename std::iterator_traits<OutputIt3>::value_type,
            Mat<T>>::value,
        bool>::type>
void SolveScfMi(
    MPI_Comm mpi_comm,
    const LpProjectionFactors<T> &proj_factors,
    const Mat<T> &sys_h_op,
    InputIt unit_basis_first,
    OutputIt1 d_eff_h_op_first,
    OutputIt2 d_mo_energies_first,
    OutputIt3 d_mos_first,
    EigSolver<T> eig_solver) {
  const auto &proj_ops = proj_factors.proj_ops_;

  if (proj_ops.empty()) {
    throw std::logic_error("Projection factors are empty.");
  }

  const size_t num_units = proj_ops.size();

  // Input iterators.
  auto proj_op_it = proj_ops.begin();
  auto unit_basis_it = unit_basis_first;

  // Output iterators.
  auto d_eff_h_op_it = d_eff_h_op_first;
  auto d_mo_energies_it = d_mo_energies_first;
  auto d_mos_it = d_mos_first;

  // Effective Hamiltonian operators.
  vector<Mat<T>> eff_h_ops(num_units);

  ScfMiHamiltonians(mpi_comm, proj_factors, sys_h_op, eff_h_ops.begin());

  for (size_t unit_idx = 0; unit_idx != num_units; ++unit_idx) {
    const auto &proj_op = *proj_op_it++;
    const Mat<T> &unit_basis = *unit_basis_it++;

    const Mat<T> unit_basis_t(unit_basis.t());

    // Effective Hamiltonian matrix.
    const auto eff_h_mat = MatrixProduct(
        mpi_comm, unit_basis_t, eff_h_ops[unit_idx], unit_basis);

    // Overlap matrix of projected unit basis set.
    const auto proj_unit_basis_overlap = MatrixProduct(
        mpi_comm, unit_basis_t, proj_op, unit_basis);

    // Energies (as eigenvalues) and coefficient matrix (as eigenvectors).
    Col<real_t> unit_mo_energies;
    Mat<T> unit_mo_coeffs;

    eig_solver(
        unit_mo_energies, unit_mo_coeffs,
        eff_h_mat, proj_unit_basis_overlap);

    if (!unit_mo_coeffs.is_square()) {
      throw std::runtime_error("Eigenvectors are not in a square matrix.");
    }

    if (unit_mo_coeffs.n_cols != unit_mo_energies.n_elem) {
      throw std::runtime_error(
          "Number of eigenvectors is not equal to the number of eigenvalues.");
    }

    // Output effective Hamiltonian operator.
    static_cast<Mat<T> &>(*d_eff_h_op_it++) = std::move(eff_h_ops[unit_idx]);

    // Indices of energies in ascending order.
    uvec energy_sort_idxs(arma::size(unit_mo_energies));

    // Output energies as real numbers in ascending order.
    {
      StableIndexSort(
          unit_mo_energies.begin(),
          unit_mo_energies.end(),
          energy_sort_idxs.begin());

      static_cast<Col<real_t> &>(*d_mo_energies_it++) =
          unit_mo_energies.rows(energy_sort_idxs);
    }

    // Output ket matrix of molecular orbitals in ascending order by energy.
    {
      // Ket matrix of normalized molecular orbitals.
      const Mat<T> unit_mos(
          arma::normalise(
              MatrixProduct(mpi_comm, unit_basis, unit_mo_coeffs)));

      static_cast<Mat<T> &>(*d_mos_it++) = unit_mos.cols(energy_sort_idxs);
    }
  }
}

} // namespace scf_mi
} // namespace tanuki

#endif
