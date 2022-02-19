#include "tanuki/parallel/mpi/mpi_shared_memory.h"

namespace tanuki {
namespace parallel {
namespace mpi {

using std::string;

using boost::interprocess::open_only;
using boost::interprocess::read_write;

MpiSharedMemory::MpiSharedMemory(
    MPI_Comm mpi_comm,
    const string &mem_name,
    size_t size,
    open_or_create_t create_mode)
        : comms_(MpiHostBasedComms(mpi_comm)) {
  int intrahost_rank;
  MPI_Comm_rank(comms_.intrahost(), &intrahost_rank);

  // Allocate or get the shared memory at this host.
  if (intrahost_rank == 0) {
    shared_mem_ =
        shared_memory_object(create_mode, mem_name.data(), read_write);

    shared_mem_.truncate(size);

    MPI_Barrier(comms_.intrahost());
  } else {
    MPI_Barrier(comms_.intrahost());

    shared_mem_ =
        shared_memory_object(open_only, mem_name.data(), read_write);
  }

  mem_region_ = mapped_region(shared_mem_, read_write);
}

MpiSharedMemory::MpiSharedMemory(
    MPI_Comm mpi_comm,
    const string &mem_name,
    size_t size,
    create_only_t create_mode)
        : comms_(MpiHostBasedComms(mpi_comm)) {
  int intrahost_rank;
  MPI_Comm_rank(comms_.intrahost(), &intrahost_rank);

  // Allocate or get the shared memory at this host.
  if (intrahost_rank == 0) {
    shared_mem_ =
        shared_memory_object(create_mode, mem_name.data(), read_write);

    shared_mem_.truncate(size);

    MPI_Barrier(comms_.intrahost());
  } else {
    MPI_Barrier(comms_.intrahost());

    shared_mem_ =
        shared_memory_object(open_only, mem_name.data(), read_write);
  }

  mem_region_ = mapped_region(shared_mem_, read_write);
}

MpiSharedMemory::~MpiSharedMemory() {
  int intrahost_rank;
  MPI_Comm_rank(comms_.intrahost(), &intrahost_rank);

  if (intrahost_rank == 0) {
    shared_memory_object::remove(shared_mem_.get_name());
  }
}

const MpiHostBasedComms &MpiSharedMemory::comms() const {
  return comms_;
}

const shared_memory_object &MpiSharedMemory::shared_mem() const {
  return shared_mem_;
}

shared_memory_object &MpiSharedMemory::shared_mem() {
  return shared_mem_;
}

void *MpiSharedMemory::mem_address() const {
  return mem_region_.get_address();
}

} // namespace mpi
} // namespace parallel
} // namespace tanuki
