configure_file(
  ${SRC_MAIN_CPP_DIR}/tanuki/math/linear/number_array.cc.in
  ${BUILD_SRC_MAIN_CPP_DIR}/tanuki/math/linear/number_array.cc
  @ONLY
)

list(
  APPEND
  HDRS

  tanuki/math/combinatorics/combinations.h
  tanuki/math/combinatorics/round_robin_tourney.h
  tanuki/math/comparison.h
  tanuki/math/linear/cholesky_decomposition.h
  tanuki/math/linear/equation_system.h
  tanuki/math/linear/indexed_vector_pair.h
  tanuki/math/linear/iterated_gram_schmidt.h
  tanuki/math/linear/matrix_index_pair.h
  tanuki/math/linear/matrix_product.h
  tanuki/math/linear/number_array.h
  tanuki/math/linear/operator_representation.h
  tanuki/math/linear/qr_decomposition.h
  tanuki/math/linear/rotation_matrix_spec.h
  tanuki/math/linear/triangular_matrix.h
  tanuki/math/linear/weighted_orthogonalization.h
)

list(
  APPEND
  SRCS

  ${BUILD_SRC_MAIN_CPP_DIR}/tanuki/math/linear/number_array.cc
  ${SRC_MAIN_CPP_DIR}/tanuki/math/combinatorics/combinations.cc
  ${SRC_MAIN_CPP_DIR}/tanuki/math/comparison.cc
  ${SRC_MAIN_CPP_DIR}/tanuki/math/linear/rotation_matrix_spec.cc
)

set(HDRS ${HDRS} PARENT_SCOPE)
set(SRCS ${SRCS} PARENT_SCOPE)
