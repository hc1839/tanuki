if(TANUKI_REAL_T STREQUAL "double")
  set(TANUKI_AVRO_REAL_SCHEMA_TYPENAME "DoubleSchema")
elseif(TANUKI_REAL_T STREQUAL "float")
  set(TANUKI_AVRO_REAL_SCHEMA_TYPENAME "FloatSchema")
endif()

configure_file(
  ${SRC_MAIN_CPP_DIR}/tanuki/avro_specific/real_schema.h.in
  ${BUILD_SRC_MAIN_CPP_DIR}/tanuki/avro_specific/real_schema.h
  @ONLY
)

list(
  APPEND
  HDRS

  tanuki/avro_specific/armadillo.h
  tanuki/avro_specific/array.h
  tanuki/avro_specific/map.h
  tanuki/avro_specific/pair.h
  tanuki/avro_specific/real_schema.h
  tanuki/avro_specific/set.h
  tanuki/avro_specific/size_t.h
)

list(
  APPEND
  SRCS

  ${SRC_MAIN_CPP_DIR}/tanuki/avro_specific/size_t.cc
)

set(HDRS ${HDRS} PARENT_SCOPE)
set(SRCS ${SRCS} PARENT_SCOPE)
