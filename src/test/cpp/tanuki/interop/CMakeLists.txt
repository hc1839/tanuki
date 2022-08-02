list(
  APPEND
  TEST_SRCS

  ${SRC_TEST_CPP_DIR}/tanuki/interop/foreign_iterator.cc
  ${SRC_TEST_CPP_DIR}/tanuki/interop/foreign_multi_array.cc
)

set(TEST_SRCS ${TEST_SRCS} PARENT_SCOPE)
