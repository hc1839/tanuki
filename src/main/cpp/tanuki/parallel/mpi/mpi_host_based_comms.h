#ifndef TANUKI_PARALLEL_MPI_MPI_HOST_BASED_COMMS_H
#define TANUKI_PARALLEL_MPI_MPI_HOST_BASED_COMMS_H

#include <cstddef>

#include <mpi.h>

#include "tanuki/parallel/mpi/mpi_hosts.h"

namespace tanuki {
namespace parallel {
namespace mpi {

/**
 *  @brief MPI communicators grouped by intrahost and interhost.
 */
class MpiHostBasedComms {
 public:
  /**
   *  It must be invoked by all MPI processes in <tt>mpi_comm</tt>.
   *
   *  @param mpi_comm
   *    MPI communicator to be split both ways by intrahost and interhost.
   */
  MpiHostBasedComms(MPI_Comm mpi_comm);

  MpiHostBasedComms(const MpiHostBasedComms &other) = delete;

  MpiHostBasedComms(MpiHostBasedComms &&other) = default;

  MpiHostBasedComms &operator=(MpiHostBasedComms &&other) = default;

  virtual ~MpiHostBasedComms();

  /**
   *  @brief MPI communicator of the processes at this host.
   *
   *  Ranks are in the same relative order as in the MPI communicator that was
   *  passed to the constructor.
   */
  const MPI_Comm &intrahost() const;

  /**
   *  @brief Color of @link intrahost @endlink, starting consecutively from
   *  zero.
   */
  int intrahost_color() const;

  /**
   *  @brief MPI communicator containing this and corresponding processes at
   *  different hosts.
   *
   *  Ranks are in the same relative order as the value of @link
   *  intrahost_color @endlink. The returned communicator contains MPI
   *  processes that have the same rank in their respective intrahost MPI
   *  communicator.
   */
  const MPI_Comm &interhost() const;

  /**
   *  @brief Color of @link interhost @endlink, starting consecutively from
   *  zero.
   */
  int interhost_color() const;

  /**
   *  @brief Information about the hosts with respect to the original MPI
   *  communicator.
   */
  const MpiHosts &hosts() const;

 private:
  /**
   *  @brief Backing data for @link intrahost @endlink.
   */
  MPI_Comm intrahost_;

  /**
   *  @brief Backing data for @link intrahost_color @endlink.
   */
  int intrahost_color_;

  /**
   *  @brief Backing data for @link interhost @endlink.
   */
  MPI_Comm interhost_;

  /**
   *  @brief Backing data for @link interhost_color @endlink.
   */
  int interhost_color_;

  /**
   *  @brief Backing data for @link hosts @endlink.
   */
  MpiHosts hosts_;
};

} // namespace mpi
} // namespace parallel
} // namespace tanuki

#endif
