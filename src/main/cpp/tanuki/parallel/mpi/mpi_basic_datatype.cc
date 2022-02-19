#include "tanuki/parallel/mpi/mpi_basic_datatype.h"

#include <complex>

namespace tanuki {
namespace parallel {
namespace mpi {

using std::complex;

template <>
MpiDatatype MpiBasicDatatype<float>() {
  return MPI_FLOAT;
}

template <>
MpiDatatype MpiBasicDatatype<double>() {
  return MPI_DOUBLE;
}

template <>
MpiDatatype MpiBasicDatatype<complex<float>>() {
  return MPI_CXX_FLOAT_COMPLEX;
}

template <>
MpiDatatype MpiBasicDatatype<complex<double>>() {
  return MPI_CXX_DOUBLE_COMPLEX;
}

} // namespace mpi
} // namespace parallel
} // namespace tanuki
