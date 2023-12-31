#ifndef TANUKI_SCF_MI_LMO_PROJECTION_FACTORS_H
#define TANUKI_SCF_MI_LMO_PROJECTION_FACTORS_H

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
#include "tanuki/number/number_cast.h"
#include "tanuki/number/types.h"
#include "tanuki/scf_mi/projection_factors.h"

namespace tanuki {
namespace scf_mi {

using std::vector;

using arma::Col;
using arma::Mat;
using arma::uvec;
using arma::uword;

using tanuki::algorithm::StableIndexSort;
using tanuki::math::linear::EigSolver;
using tanuki::math::linear::MatrixProduct;
using tanuki::number::NumberCast;
using tanuki::number::complex_t;
using tanuki::number::real_t;

template <typename T>
using LmoProjectionFactors = ProjectionFactors<ScfMiMethod::LMO, T>;

/**
 *  @brief Solves the eigenvalue equations of LMO SCF MI for each unit.
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
 *  @param d_eff_h_mat_first
 *    Beginning of the destination range of effective Hamiltonian matrices in
 *    in the representation of unit basis set for each unit.
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
    const LmoProjectionFactors<T> &proj_factors,
    const Mat<T> &sys_h_op,
    InputIt unit_basis_first,
    OutputIt1 d_eff_h_mat_first,
    OutputIt2 d_mo_energies_first,
    OutputIt3 d_mos_first,
    EigSolver<T> eig_solver);

/**
 *  @brief Factors in the effective Hamiltonians of LMO SCF MI for each unit.
 */
template <typename T>
struct ProjectionFactors<ScfMiMethod::LMO, T> final {
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
   *  @param nonortho_unit_mos_first
   *    Beginning of the range of ket matrices of occupied normalized
   *    nonorthogonal spatial molecular orbitals for each unit. All ket
   *    matrices must have the same number of rows. The range must not be
   *    empty.
   *
   *  @param nonortho_unit_mos_last
   *    Past-the-end of the range of ket matrices of occupied normalized
   *    nonorthogonal spatial molecular orbitals for each unit.
   *
   *  @param ortho_mos
   *    Ket matrix of occupied spatial molecular orbitals from all units that
   *    are orthonormal across all units. It must have the same basis
   *    dimensionality, same number of molecular orbitals, and have molecular
   *    orbitals in the same order as <tt>[nonortho_unit_mos_first,
   *    nonortho_unit_mos_last)</tt>.
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
      InputIt nonortho_unit_mos_first,
      InputIt nonortho_unit_mos_last,
      const Mat<T> &ortho_mos) {
    if (nonortho_unit_mos_first == nonortho_unit_mos_last) {
      throw std::logic_error(
          "Range of nonorthogonal unit molecular orbitals is empty.");
    }

    // Projection operator.
    const auto proj_op = MatrixProduct(
        mpi_comm, ortho_mos, Mat<T>(ortho_mos.t()));

    // Identity matrix minus projection operator.
    const Mat<T> eye_minus_proj(
        Mat<T>(arma::size(proj_op), arma::fill::eye) - proj_op);

    uword col_idx_offset = 0;

    // Create prefactor and postfactor for each unit.
    for (auto it = nonortho_unit_mos_first;
         it != nonortho_unit_mos_last;
         ++it) {
      const Mat<T> &nonortho_unit_mos = *it;

      const Mat<T> ortho_unit_mos(
          ortho_mos.cols(
              col_idx_offset,
              col_idx_offset + nonortho_unit_mos.n_cols - 1));

      const auto partial_star_op = MatrixProduct(
          mpi_comm, ortho_unit_mos, Mat<T>(nonortho_unit_mos.t()));

      if (!partial_star_op.is_square()) {
        throw std::logic_error("Basis dimensionality is not the same.");
      }

      this->prefactors_.push_back(eye_minus_proj + partial_star_op.t());
      this->postfactors_.push_back(eye_minus_proj + partial_star_op);

      col_idx_offset += nonortho_unit_mos.n_cols;
    }
  }

  template <
      typename U,
      typename InputIt,
      typename OutputIt1,
      typename OutputIt2,
      typename OutputIt3,
      typename std::enable_if<
          std::is_convertible<
              typename std::iterator_traits<InputIt>::value_type,
              Mat<U>>::value,
          bool>::type,
      typename std::enable_if<
          std::is_convertible<
              typename std::iterator_traits<OutputIt1>::value_type,
              Mat<U>>::value,
          bool>::type,
      typename std::enable_if<
          std::is_convertible<
              typename std::iterator_traits<OutputIt2>::value_type,
              Col<real_t>>::value,
          bool>::type,
      typename std::enable_if<
          std::is_convertible<
              typename std::iterator_traits<OutputIt3>::value_type,
              Mat<U>>::value,
          bool>::type>
  friend void SolveScfMi(
      MPI_Comm mpi_comm,
      const LmoProjectionFactors<U> &proj_factors,
      const Mat<U> &sys_h_op,
      InputIt unit_basis_first,
      OutputIt1 d_eff_h_mat_first,
      OutputIt2 d_mo_energies_first,
      OutputIt3 d_mos_first,
      EigSolver<U> eig_solver);

 private:
  /**
   *  @brief Prefactors for each unit.
   *
   *  Each element is the conjugate transpose of the corresponding element in
   *  @link postfactors @endlink.
   */
  vector<Mat<T>> prefactors_;

  /**
   *  @brief Postfactors for each unit.
   *
   *  For a unit \f$ J \f$, it is defined as \f$ 1 - \hat{P} + \hat{\Omega}_{J}
   *  \f$, where \f$ \hat{P} \f$ is the projection operator, and \f$
   *  \hat{\Omega}_{J} \f$ is the partial star operator.
   *
   *  The projection operator is defined as \f$ \hat{P} \equiv
   *  \boldsymbol{\Psi}_{*} \boldsymbol{\Psi}^{\dagger}_{*} \f$, where \f$
   *  \boldsymbol{\Psi}_{*} \f$ is the ket matrix of occupied spatial molecular
   *  orbitals that are orthonormal across all units using occupation numbers
   *  as orthogonalization weights. The partial star operator for unit \f$ J
   *  \f$ is defined as \f$ \hat{\Omega}_{J} \equiv \boldsymbol{\Psi}_{J*}
   *  \boldsymbol{\Psi}^{\dagger}_{J} \f$, where \f$ \boldsymbol{\Psi}_{J} \f$
   *  is the ket matrix of occupied normalized nonorthogonal spatial molecular
   *  orbitals of unit \f$ J \f$, and \f$ \boldsymbol{\Psi}_{J*} \f$ is the
   *  matrix of the columns of \f$ \boldsymbol{\Psi}_{*} \f$ that correspond to
   *  unit \f$ J \f$.
   */
  vector<Mat<T>> postfactors_;
};

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
    const LmoProjectionFactors<T> &proj_factors,
    const Mat<T> &sys_h_op,
    InputIt unit_basis_first,
    OutputIt1 d_eff_h_mat_first,
    OutputIt2 d_mo_energies_first,
    OutputIt3 d_mos_first,
    EigSolver<T> eig_solver) {
  const auto &prefactors = proj_factors.prefactors_;
  const auto &postfactors = proj_factors.postfactors_;

  if (prefactors.empty()) {
    throw std::logic_error("Projection factors are empty.");
  }

  if (postfactors.size() != prefactors.size()) {
    throw std::logic_error("Mismatch in the number of factors.");
  }

  const size_t num_units = prefactors.size();

  // Input iterators.
  auto prefactor_it = prefactors.begin();
  auto postfactor_it = postfactors.begin();
  auto unit_basis_it = unit_basis_first;

  // Output iterators.
  auto d_eff_h_mat_it = d_eff_h_mat_first;
  auto d_mo_energies_it = d_mo_energies_first;
  auto d_mos_it = d_mos_first;

  for (size_t unit_idx = 0; unit_idx != num_units; ++unit_idx) {
    const auto &prefactor = *prefactor_it++;
    const auto &postfactor = *postfactor_it++;
    const Mat<T> &unit_basis = *unit_basis_it++;

    const Mat<T> unit_basis_t(unit_basis.t());

    // Effective Hamiltonian operator.
    const auto eff_h_op = MatrixProduct(
        mpi_comm, prefactor, sys_h_op, postfactor);

    // Effective Hamiltonian matrix.
    auto eff_h_mat = MatrixProduct(
        mpi_comm, unit_basis_t, eff_h_op, unit_basis);

    // Energies (as eigenvalues) and coefficient matrix (as eigenvectors).
    Col<real_t> unit_mo_energies;
    Mat<T> unit_mo_coeffs;

    eig_solver(
        unit_mo_energies, unit_mo_coeffs,
        eff_h_mat, MatrixProduct(mpi_comm, unit_basis_t, unit_basis));

    if (!unit_mo_coeffs.is_square()) {
      throw std::runtime_error("Eigenvectors are not in a square matrix.");
    }

    if (unit_mo_coeffs.n_cols != unit_mo_energies.n_elem) {
      throw std::runtime_error(
          "Number of eigenvectors is not equal to the number of eigenvalues.");
    }

    // Output effective Hamiltonian matrix.
    static_cast<Mat<T> &>(*d_eff_h_mat_it++) = std::move(eff_h_mat);

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
