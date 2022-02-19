#ifndef TANUKI_PARALLEL_MPI_MPI_BASIC_DATATYPE_H
#define TANUKI_PARALLEL_MPI_MPI_BASIC_DATATYPE_H

#include <mpi.h>

namespace tanuki {
namespace parallel {
namespace mpi {

using MpiDatatype = decltype(MPI_CHAR);

/**
 *  @brief MPI basic datatype corresponding to C/C++ type.
 *
 *  @tparam T
 *    C/C++ type. Types are supported through template specialization.
 *
 *  @return
 *    MPI datatype corresponding to <tt>T</tt>.
 */
template <typename T>
MpiDatatype MpiBasicDatatype();

} // namespace mpi
} // namespace parallel
} // namespace tanuki

#endif
