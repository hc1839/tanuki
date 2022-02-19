#ifndef TANUKI_PARALLEL_MEMORY_COPY_H
#define TANUKI_PARALLEL_MEMORY_COPY_H

#include <cstddef>

#include <mpi.h>

namespace tanuki {
namespace parallel {
namespace memory {

/**
 *  @brief Copies bytes with OpenMP and MPI parallelization.
 *
 *  @param mpi_comm
 *    MPI communicator for further parallization with MPI, or
 *    <tt>MPI_COMM_NULL</tt> for OpenMP only. If not <tt>MPI_COMM_NULL</tt>,
 *    behavior is defined only when <tt>src</tt> points to a memory location
 *    (not necessarily shared) that has the same data contents across the
 *    processes and <tt>dst</tt> points to shared memory.
 *
 *  @param src
 *    Pointer to the memory location to copy from. It is reinterpreted as an
 *    array of <tt>char</tt>.
 *
 *  @param dst
 *    Pointer to the memory location to copy to. It is reinterpreted as an
 *    array of <tt>char</tt>.
 *
 *  @param count
 *    Number of bytes to copy.
 */
void Copy(MPI_Comm mpi_comm, const void *src, void *dst, size_t count);

} // namespace memory
} // namespace parallel
} // namespace tanuki

#endif
