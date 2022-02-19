#ifndef TANUKI_PARALLEL_JACOBI_SIDEDNESS
#define TANUKI_PARALLEL_JACOBI_SIDEDNESS

namespace tanuki {
namespace parallel {
namespace grs {

/**
 *  @brief Sidedness of GRS.
 */
enum class JacobiSidedness : int {
  /**
   *  @brief One-sided GRS with the columns being rotated.
   */
  ONE_SIDED_RIGHT,

  /**
   *  @brief Two-sided GRS.
   */
  TWO_SIDED
};

} // namespace grs
} // namespace parallel
} // namespace tanuki

#endif
