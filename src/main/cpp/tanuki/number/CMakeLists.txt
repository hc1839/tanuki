configure_file(
  ${SRC_MAIN_CPP_DIR}/tanuki/number/types.h.in
  ${BUILD_SRC_MAIN_CPP_DIR}/tanuki/number/types.h
  @ONLY
)

list(
  APPEND
  HDRS

  tanuki/number/number_cast.h
  tanuki/number/types.h
)

list(
  APPEND
  SRCS

  ${SRC_MAIN_CPP_DIR}/tanuki/number/number_cast.cc
)

set(HDRS ${HDRS} PARENT_SCOPE)
set(SRCS ${SRCS} PARENT_SCOPE)
