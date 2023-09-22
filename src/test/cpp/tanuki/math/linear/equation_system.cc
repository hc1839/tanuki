#include <tanuki.h>

#include <armadillo>
#include <gtest/gtest.h>
#include <mpi.h>

#define ABS_ERROR 1.0e-6

namespace tanuki {
namespace math {
namespace linear {

using arma::Col;
using arma::Mat;

using tanuki::number::real_t;

/**
 *  @brief Tests solving a system of linear equations that has real
 *  coefficients.
 */
TEST(EquationSystemSolution, RealCoefficients) {
  const Mat<real_t> coeffs{
    { 3.0, 2.0, -1.0 },
    { 2.0, -2.0, 4.0 },
    { -1.0, 0.5, -1.0 }
  };

  const Col<real_t> constants{ 1.0, -2.0, 0.0 };

  const auto solution = EquationSystemSolution(
      MPI_COMM_WORLD, coeffs, constants);

  ASSERT_NEAR(solution(0, 0), 1.0, ABS_ERROR);
  ASSERT_NEAR(solution(1, 0), -2.0, ABS_ERROR);
  ASSERT_NEAR(solution(2, 0), -2.0, ABS_ERROR);
}

} // namespace linear
} // namespace math
} // namespace tanuki
