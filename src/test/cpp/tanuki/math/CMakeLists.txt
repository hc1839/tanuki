list(
  APPEND
  TEST_SRCS

  ${SRC_TEST_CPP_DIR}/tanuki/math/linear/iterated_gram_schmidt.cc
  ${SRC_TEST_CPP_DIR}/tanuki/math/linear/matrix_product.cc
)

set(TEST_SRCS ${TEST_SRCS} PARENT_SCOPE)
