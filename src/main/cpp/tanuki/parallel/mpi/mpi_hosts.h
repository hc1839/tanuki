#ifndef TANUKI_PARALLEL_MPI_MPI_HOSTS_H
#define TANUKI_PARALLEL_MPI_MPI_HOSTS_H

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include <mpi.h>

namespace tanuki {
namespace parallel {
namespace mpi {

/**
 *  @brief Information about the hosts of the MPI processes in a communicator.
 */
class MpiHosts {
 public:
  /**
   *  It must be invoked by all MPI processes in <tt>mpi_comm</tt>.
   *
   *  @param mpi_comm
   *    MPI communicator.
   */
  MpiHosts(MPI_Comm mpi_comm);

  virtual ~MpiHosts() = default;

  /**
   *  @brief Host name of an MPI process.
   *
   *  @param rank
   *    Rank of the MPI process. It must be a valid rank in @link comm
   *    @endlink.
   *
   *  @return
   *    Host name of <tt>rank</tt>.
   */
  std::string HostName(int rank) const;

  /**
   *  @brief Host names of the MPI processes.
   */
  std::set<std::string> HostNames() const;

  /**
   *  @brief Ranks of the MPI processes at the specified host.
   *
   *  @param host_name
   *    Host name.
   *
   *  @return
   *    Ranks of the MPI processes at <tt>host_name</tt>.
   */
  std::set<int> RanksAt(const std::string &host_name) const;

  /**
   *  @brief MPI communicator.
   */
  MPI_Comm comm() const;

  /**
   *  @brief Rank of this MPI process in @link comm @endlink.
   */
  int rank() const;

  /**
   *  @brief Size of @link comm @endlink.
   */
  size_t size() const;

  /**
   *  @brief Number of hosts.
   */
  size_t num_hosts() const;

 protected:
  /**
   *  @brief Host names grouped by MPI rank.
   */
  std::vector<std::string> host_name_by_rank_;

 private:
  /**
   *  @brief Backing data member for @link comm @endlink.
   */
  MPI_Comm comm_;

  /**
   *  @brief Backing data member for @link rank @endlink.
   */
  int rank_;

  /**
   *  @brief Backing data member for @link size @endlink.
   */
  size_t size_;
};

} // namespace mpi
} // namespace parallel
} // namespace tanuki

#endif
