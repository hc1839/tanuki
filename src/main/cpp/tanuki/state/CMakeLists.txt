configure_file(
  ${SRC_MAIN_CPP_DIR}/tanuki/state/spin.cc.in
  ${BUILD_SRC_MAIN_CPP_DIR}/tanuki/state/spin.cc
  @ONLY
)

list(
  APPEND
  HDRS

  tanuki/state/spin.h
)

list(
  APPEND
  SRCS

  ${BUILD_SRC_MAIN_CPP_DIR}/tanuki/state/spin.cc
)

set(HDRS ${HDRS} PARENT_SCOPE)
set(SRCS ${SRCS} PARENT_SCOPE)
