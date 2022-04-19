list(
  APPEND
  TEST_SRCS

  ${SRC_TEST_CPP_DIR}/tanuki/state/spin.cc
  ${SRC_TEST_CPP_DIR}/tanuki/state/spin_range.cc
  ${SRC_TEST_CPP_DIR}/tanuki/state/spin_range_transformer.cc
)

set(TEST_SRCS ${TEST_SRCS} PARENT_SCOPE)
