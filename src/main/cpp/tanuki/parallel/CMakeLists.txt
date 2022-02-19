list(
  APPEND
  HDRS

  tanuki/parallel/concurrent_actuator.h
  tanuki/parallel/grs/actuator.h
  tanuki/parallel/grs/jacobi_sidedness.h
  tanuki/parallel/memory/copy.h
  tanuki/parallel/mpi/mpi_basic_datatype.h
  tanuki/parallel/mpi/mpi_host_based_comms.h
  tanuki/parallel/mpi/mpi_hosts.h
  tanuki/parallel/mpi/mpi_shared_memory.h
)

list(
  APPEND
  SRCS

  ${SRC_MAIN_CPP_DIR}/tanuki/parallel/grs/actuator.cc
  ${SRC_MAIN_CPP_DIR}/tanuki/parallel/memory/copy.cc
  ${SRC_MAIN_CPP_DIR}/tanuki/parallel/mpi/mpi_basic_datatype.cc
  ${SRC_MAIN_CPP_DIR}/tanuki/parallel/mpi/mpi_host_based_comms.cc
  ${SRC_MAIN_CPP_DIR}/tanuki/parallel/mpi/mpi_hosts.cc
  ${SRC_MAIN_CPP_DIR}/tanuki/parallel/mpi/mpi_shared_memory.cc
)

set(HDRS ${HDRS} PARENT_SCOPE)
set(SRCS ${SRCS} PARENT_SCOPE)
