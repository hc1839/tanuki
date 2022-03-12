list(
  APPEND
  HDRS

  tanuki/eda/hamiltonian_energies.h
)

list(
  APPEND
  SRCS

  ${SRC_MAIN_CPP_DIR}/tanuki/eda/hamiltonian_energies.cc
)

set(HDRS ${HDRS} PARENT_SCOPE)
set(SRCS ${SRCS} PARENT_SCOPE)
