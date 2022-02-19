list(
  APPEND
  HDRS

  tanuki/common/divider/group_delimiter.h
)

list(
  APPEND
  SRCS

  ${SRC_MAIN_CPP_DIR}/tanuki/common/divider/group_delimiter.cc
)

set(HDRS ${HDRS} PARENT_SCOPE)
set(SRCS ${SRCS} PARENT_SCOPE)
