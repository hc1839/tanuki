list(
  APPEND
  TEST_SRCS

  ${SRC_TEST_CPP_DIR}/tanuki/interop/foreign_container.cc
  ${SRC_TEST_CPP_DIR}/tanuki/interop/foreign_iterator.cc
)

set(TEST_SRCS ${TEST_SRCS} PARENT_SCOPE)
