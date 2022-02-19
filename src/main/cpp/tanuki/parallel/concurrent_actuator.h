#ifndef TANUKI_PARALLEL_CONCURRENT_ACTUATOR_H
#define TANUKI_PARALLEL_CONCURRENT_ACTUATOR_H

#include <functional>

#include <mpi.h>

namespace tanuki {
namespace parallel {

/**
 *  @brief Interface of an actuator of a parallelization strategy.
 *
 *  @tparam Query
 *    Type of query from the actuator.
 *
 *  @tparam Response
 *    Type of response to a query from the actuator.
 *
 *  @tparam Actee
 *    Type of object undergoing actuation.
 *
 *  @tparam Result
 *    Type of actuation result.
 */
template <typename Query, typename Response, typename Actee, typename Result>
class ConcurrentActuator {
 public:
  virtual ~ConcurrentActuator() = default;

  /**
   *  @brief Actuates the parallelization strategy through an inquiry function.
   *
   *  @param input
   *    Input object to the actuation.
   *
   *  @param inquiry_fn
   *    Inquiry function that returns a response to a query from the actuator.
   *
   *  @return
   *    Actuation result.
   */
  virtual Result Actuate(
      const Actee &input, std::function<Response(Query)> inquiry_fn) = 0;

  /**
   *  @brief MPI communicator.
   *
   *  Level of OpenMP thread support required depends on the parallelization
   *  strategy.
   */
  virtual MPI_Comm mpi_comm() const = 0;

  /**
   *  @brief Maximum number of OpenMP threads to use.
   *
   *  It must be a positive number. Valid maximum depends on the
   *  parallelization strategy but generally should not exceed
   *  <tt>omp_get_max_threads()</tt>.
   */
  virtual size_t max_threads() const = 0;
};

} // namespace parallel
} // namespace tanuki

#endif
