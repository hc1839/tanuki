list(
  APPEND
  HDRS

  tanuki/interop/c_complex_number.h
  tanuki/interop/c_multi_array.h
  tanuki/interop/c_opaque_container.h
  tanuki/interop/c_poly_pointer.h
  tanuki/interop/c_sequence.h
  tanuki/interop/foreign_container.h
  tanuki/interop/foreign_container_ref.h
  tanuki/interop/foreign_forwarder.h
  tanuki/interop/foreign_iterator.h
  tanuki/interop/foreign_pointer.h
)

list(
  APPEND
  SRCS

  ${SRC_MAIN_CPP_DIR}/tanuki/interop/c_multi_array.cc
  ${SRC_MAIN_CPP_DIR}/tanuki/interop/foreign_pointer.cc
)

set(HDRS ${HDRS} PARENT_SCOPE)
set(SRCS ${SRCS} PARENT_SCOPE)
