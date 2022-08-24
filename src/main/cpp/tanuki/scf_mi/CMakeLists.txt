configure_file(
  ${SRC_MAIN_CPP_DIR}/tanuki/scf_mi/scf_mi_method.cc.in
  ${BUILD_SRC_MAIN_CPP_DIR}/tanuki/scf_mi/scf_mi_method.cc
  @ONLY
)

list(
  APPEND
  HDRS

  tanuki/scf_mi/lmo_projection_factors.h
  tanuki/scf_mi/projection_factors.h
  tanuki/scf_mi/scf_mi_method.h
)

list(
  APPEND
  SRCS

  ${BUILD_SRC_MAIN_CPP_DIR}/tanuki/scf_mi/scf_mi_method.cc
)

set(HDRS ${HDRS} PARENT_SCOPE)
set(SRCS ${SRCS} PARENT_SCOPE)
