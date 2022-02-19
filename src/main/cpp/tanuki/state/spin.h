#ifndef TANUKI_STATE_SPIN_H
#define TANUKI_STATE_SPIN_H

namespace tanuki {
namespace state {

/**
 *  @brief Enumeration of up/down, up, and down spin directions.
 */
enum class Spin : int {
  /**
   *  @brief One of the same up or down direction in a system that is not spin
   *  polarized.
   */
  DUAL,

  /**
   *  @brief Up direction in a system that is spin polarized.
   */
  UP,

  /**
   *  @brief Down direction in a system that is spin polarized.
   */
  DOWN
};

} // namespace state
} // namespace tanuki

#endif
