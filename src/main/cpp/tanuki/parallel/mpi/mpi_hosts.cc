#include "tanuki/parallel/mpi/mpi_hosts.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>

#include <boost/asio.hpp>

namespace tanuki {
namespace parallel {
namespace mpi {

using std::calloc;
using std::free;
using std::set;
using std::strcpy;
using std::string;
using std::vector;

MpiHosts::MpiHosts(MPI_Comm mpi_comm) : comm_(mpi_comm) {
  MPI_Comm_rank(this->comm_, &this->rank_);

  {
    int mpi_comm_size;
    MPI_Comm_size(this->comm_, &mpi_comm_size);

    this->size_ = static_cast<size_t>(mpi_comm_size);
  }

  this->host_name_by_rank_.resize(this->size_);

  const auto host_name = boost::asio::ip::host_name();

  // Broadcast host names from all MPI processes.
  for (int n = 0; n != size_; ++n) {
    unsigned int host_name_len = host_name.size();
    MPI_Bcast(&host_name_len, 1, MPI_UNSIGNED, n, this->comm_);

    auto host_name_buf = static_cast<char *>(
        calloc(host_name_len + 1, sizeof(char)));

    if (n == this->rank_) {
      strcpy(host_name_buf, host_name.data());
    }

    MPI_Bcast(host_name_buf, host_name_len, MPI_CHAR, n, this->comm_);
    this->host_name_by_rank_[n] = string(host_name_buf);

    free(host_name_buf);
  }
}

string MpiHosts::HostName(int rank) const {
  assert(rank >= 0 && rank < host_name_by_rank_.size());

  return host_name_by_rank_[rank];
}

set<string> MpiHosts::HostNames() const {
  return set<string>(host_name_by_rank_.begin(), host_name_by_rank_.end());
}

set<int> MpiHosts::RanksAt(const string &host_name) const {
  set<int> retval;

  int rank = 0;

  for (const auto &host : host_name_by_rank_) {
    if (host == host_name) {
      retval.insert(rank);
    }

    ++rank;
  }

  assert(!retval.empty());

  return retval;
}

MPI_Comm MpiHosts::comm() const {
  return comm_;
}

int MpiHosts::rank() const {
  return rank_;
}

size_t MpiHosts::size() const {
  return static_cast<size_t>(size_);
}

size_t MpiHosts::num_hosts() const {
  return HostNames().size();
}

} // namespace mpi
} // namespace parallel
} // namespace tanuki
