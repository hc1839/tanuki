list(
  APPEND
  HDRS

  tanuki/math/combinatorics/combinations.h
  tanuki/math/combinatorics/round_robin_tourney.h
  tanuki/math/linear/cholesky_decomposition.h
  tanuki/math/linear/indexed_vector_pair.h
  tanuki/math/linear/iterated_gram_schmidt.h
  tanuki/math/linear/matrix_index_pair.h
  tanuki/math/linear/matrix_product.h
  tanuki/math/linear/qr_decomposition.h
  tanuki/math/linear/rotation_matrix_spec.h
  tanuki/math/linear/triangular_matrix.h
  tanuki/math/linear/weighted_orthogonalization.h
)

list(
  APPEND
  SRCS

  ${SRC_MAIN_CPP_DIR}/tanuki/math/combinatorics/combinations.cc
  ${SRC_MAIN_CPP_DIR}/tanuki/math/linear/rotation_matrix_spec.cc
)

set(HDRS ${HDRS} PARENT_SCOPE)
set(SRCS ${SRCS} PARENT_SCOPE)
