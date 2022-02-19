#include "tanuki/parallel/mpi/mpi_host_based_comms.h"

#include <algorithm>
#include <cassert>

namespace tanuki {
namespace parallel {
namespace mpi {

using std::set;
using std::string;

MpiHostBasedComms::MpiHostBasedComms(MPI_Comm mpi_comm)
    : hosts_(MpiHosts(mpi_comm)) {
  const auto host_name = hosts_.HostName(hosts_.rank());

  // Split the MPI communicator by host.
  {
    intrahost_color_ = 0;

    for (const auto &host : hosts_.HostNames()) {
      if (host == host_name) {
        break;
      }

      ++intrahost_color_;
    }

    MPI_Comm_split(mpi_comm, intrahost_color_, 0, &intrahost_);
  }

  MPI_Comm_rank(intrahost_, &interhost_color_);

  // Split the MPI communicator across hosts.
  MPI_Comm_split(mpi_comm, interhost_color_, intrahost_color_, &interhost_);
}

MpiHostBasedComms::~MpiHostBasedComms() {
  MPI_Comm_free(&interhost_);
  MPI_Comm_free(&intrahost_);
}

const MPI_Comm &MpiHostBasedComms::intrahost() const {
  return intrahost_;
}

int MpiHostBasedComms::intrahost_color() const {
  return intrahost_color_;
}

const MPI_Comm &MpiHostBasedComms::interhost() const {
  return interhost_;
}

int MpiHostBasedComms::interhost_color() const {
  return interhost_color_;
}

const MpiHosts &MpiHostBasedComms::hosts() const {
  return hosts_;
}

} // namespace mpi
} // namespace parallel
} // namespace tanuki
