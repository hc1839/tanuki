#include "tanuki/parallel/memory/copy.h"

#include <cassert>
#include <cstring>

#include <omp.h>

#include "tanuki/common/divider/group_delimiter.h"

namespace tanuki {
namespace parallel {
namespace memory {

using std::memcpy;

using common::divider::GroupIndices;

void Copy(
    MPI_Comm mpi_comm, const void *src, void *dst, size_t count) {
  assert(src != nullptr);
  assert(dst != nullptr);

  if (count == 0) {
    return;
  }

  int mpi_rank;
  MPI_Comm_rank(mpi_comm, &mpi_rank);

  int mpi_comm_size;
  MPI_Comm_size(mpi_comm, &mpi_comm_size);

  const size_t max_threads = omp_get_max_threads();

  // Index ranges delimited by MPI rank.
  const auto byte_batches = GroupIndices(0, count, mpi_comm_size);

  // Index ranges delimited by OpenMP.
  const auto byte_chunks = GroupIndices(
      byte_batches[mpi_rank],
      byte_batches[mpi_rank + 1],
      max_threads);

  // Concurrently copy the portions associated to this MPI process.
  #pragma omp parallel default(shared)
  {
    const auto thread_num = omp_get_thread_num();

    const size_t byte_chunk_first = byte_chunks[thread_num];
    const size_t byte_chunk_last = byte_chunks[thread_num + 1];

    // Pointer to the beginning of the source portion.
    auto s_first = static_cast<const char *>(src);
    std::advance(s_first, byte_chunk_first);

    // Pointer to the beginning of the destination portion.
    auto d_first = static_cast<char *>(dst);
    std::advance(d_first, byte_chunk_first);

    // Number of bytes in the portion.
    const size_t portion_size = byte_chunk_last - byte_chunk_first;

    // Copy the portion.
    memcpy(d_first, s_first, portion_size);
  }

  MPI_Barrier(mpi_comm);
}

} // namespace memory
} // namespace parallel
} // namespace tanuki
