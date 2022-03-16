list(
  APPEND
  TEST_SRCS

  ${SRC_TEST_CPP_DIR}/tanuki/avro_specific/array.cc
  ${SRC_TEST_CPP_DIR}/tanuki/avro_specific/map.cc
  ${SRC_TEST_CPP_DIR}/tanuki/avro_specific/pair.cc
  ${SRC_TEST_CPP_DIR}/tanuki/avro_specific/set.cc
  ${SRC_TEST_CPP_DIR}/tanuki/avro_specific/size_t.cc
)

set(TEST_SRCS ${TEST_SRCS} PARENT_SCOPE)
