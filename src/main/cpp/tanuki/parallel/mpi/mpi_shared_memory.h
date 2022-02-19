#ifndef TANUKI_PARALLEL_MPI_MPI_SHARED_MEMORY_H
#define TANUKI_PARALLEL_MPI_MPI_SHARED_MEMORY_H

#include <cstddef>
#include <string>

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <mpi.h>

#include "tanuki/parallel/mpi/mpi_host_based_comms.h"

namespace tanuki {
namespace parallel {
namespace mpi {

using boost::interprocess::create_only_t;
using boost::interprocess::mapped_region;
using boost::interprocess::open_or_create_t;
using boost::interprocess::shared_memory_object;

using parallel::mpi::MpiHostBasedComms;

/**
 *  @brief Read-write shared memory at each host that the MPI processes in a
 *  communicator are at.
 *
 *  An instance of this class is to be kept by each MPI process in the
 *  communicator.
 */
class MpiSharedMemory final {
 public:
  /**
   *  It must be invoked by all MPI processes in <tt>mpi_comm</tt>.
   *
   *  @param mpi_comm
   *    MPI communicator of the processes to allocated shared memory at each
   *    host for.
   *
   *  @param mem_name
   *    Name of the shared memory.
   *
   *  @param size
   *    Size, in bytes, of the shared memory at each host.
   */
  MpiSharedMemory(
      MPI_Comm mpi_comm,
      const std::string &mem_name,
      size_t size,
      open_or_create_t create_mode);

  MpiSharedMemory(
      MPI_Comm mpi_comm,
      const std::string &mem_name,
      size_t size,
      create_only_t create_mode);

  MpiSharedMemory(const MpiSharedMemory &other) = delete;

  MpiSharedMemory(MpiSharedMemory &&other) = default;

  MpiSharedMemory &operator=(MpiSharedMemory &&other) = default;

  ~MpiSharedMemory();

  /**
   *  @brief Host-based MPI communicators.
   */
  const MpiHostBasedComms &comms() const;

  /**
   *  @brief Shared memory object at this host.
   */
  const shared_memory_object &shared_mem() const;

  shared_memory_object &shared_mem();

  /**
   *  @brief Convenience function for getting the pointer to the beginning of
   *  the shared memory region.
   */
  void *mem_address() const;

 private:
  /**
   *  @brief Backing data for @link comms @endlink.
   */
  MpiHostBasedComms comms_;

  /**
   *  @brief Backing data for @link shared_mem @endlink.
   */
  shared_memory_object shared_mem_;

  /**
   *  @brief Memory region mapped to the beginning.
   */
  mapped_region mem_region_;
};

} // namespace mpi
} // namespace parallel
} // namespace tanuki

#endif
