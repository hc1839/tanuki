cmake_minimum_required(VERSION 3.14)
project(tanuki)

set(TANUKI_VERSION 0.1.0)

# User options.

set(
  BUILD_TEST
  OFF
  CACHE
  BOOL
  "Whether to build the test executable."
)

set(
  TANUKI_NO_DEBUG
  ON
  CACHE
  BOOL
  "Whether to disable debugging."
)

set(
  TANUKI_REAL_T
  "double"
  CACHE
  STRING
  "C/C++ type of real number. It can be 'float' or 'double' (default). Other types are undefined."
)

if(TANUKI_NO_DEBUG)
  add_definitions(-DTANUKI_NO_DEBUG)
endif()

# Language settings.

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Includes.

include(FetchContent)
include(GNUInstallDirs)

# Global values.

set(SRC_MAIN_CPP_DIR ${PROJECT_SOURCE_DIR}/src/main/cpp)
set(SRC_MAIN_RESOURCES_DIR ${PROJECT_SOURCE_DIR}/src/main/resources)
set(SRC_TEST_CPP_DIR ${PROJECT_SOURCE_DIR}/src/test/cpp)

set(BUILD_SRC_MAIN_CPP_DIR ${PROJECT_BINARY_DIR}/src/main/cpp)
set(BUILD_SRC_MAIN_RESOURCES_DIR ${PROJECT_BINARY_DIR}/src/main/resources)
set(BUILD_SRC_TEST_CPP_DIR ${PROJECT_BINARY_DIR}/src/test/cpp)

set(CPATH $ENV{CPATH})
string(REPLACE ":" ";" CPATH "${CPATH}")

set(C_INCLUDE_PATH $ENV{C_INCLUDE_PATH})
string(REPLACE ":" ";" C_INCLUDE_PATH "${C_INCLUDE_PATH}")

set(CPLUS_INCLUDE_PATH $ENV{CPLUS_INCLUDE_PATH})
string(REPLACE ":" ";" CPLUS_INCLUDE_PATH "${CPLUS_INCLUDE_PATH}")

set(LIBRARY_PATH $ENV{LIBRARY_PATH})
string(REPLACE ":" ";" LIBRARY_PATH "${LIBRARY_PATH}")

# Set up definitions.

add_definitions(
  -DARMA_ALLOW_FAKE_GCC
)

# Find third-party packages and libraries.

find_library(
  ARMA_LIB
  armadillo
  PATHS
    ${LIBRARY_PATH}
  REQUIRED
)

find_library(
  AVRO_LIB
  avrocpp
  PATHS
    ${LIBRARY_PATH}
  REQUIRED
)

find_package(
  Boost
  REQUIRED
  COMPONENTS
    filesystem
    system
)

find_package(MPI REQUIRED)
find_package(OpenMP REQUIRED)

set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS})

include_directories(
  ${CPATH}
  ${CPLUS_INCLUDE_PATH}
  ${C_INCLUDE_PATH}
  ${MPI_CXX_INCLUDE_DIRS}
  ${OpenMP_CXX_INCLUDE_DIRS}
)

# Add subdirectories, and configure files.

add_subdirectory(${SRC_MAIN_CPP_DIR}/tanuki/algorithm)
add_subdirectory(${SRC_MAIN_CPP_DIR}/tanuki/avro_specific)
add_subdirectory(${SRC_MAIN_CPP_DIR}/tanuki/common)
add_subdirectory(${SRC_MAIN_CPP_DIR}/tanuki/eda)
add_subdirectory(${SRC_MAIN_CPP_DIR}/tanuki/enumeration)
add_subdirectory(${SRC_MAIN_CPP_DIR}/tanuki/interop)
add_subdirectory(${SRC_MAIN_CPP_DIR}/tanuki/math)
add_subdirectory(${SRC_MAIN_CPP_DIR}/tanuki/memory)
add_subdirectory(${SRC_MAIN_CPP_DIR}/tanuki/number)
add_subdirectory(${SRC_MAIN_CPP_DIR}/tanuki/parallel)
add_subdirectory(${SRC_MAIN_CPP_DIR}/tanuki/scf_mi)
add_subdirectory(${SRC_MAIN_CPP_DIR}/tanuki/state)
add_subdirectory(${SRC_MAIN_RESOURCES_DIR})

# Build include directives.
set(HDR_INCS ${HDRS})
list(TRANSFORM HDR_INCS PREPEND "#include \"")
list(TRANSFORM HDR_INCS APPEND "\"")
string(REPLACE ";" "\n" HDR_INCS "${HDR_INCS}")

# Configure the all-inclusive header file.
configure_file(
  ${SRC_MAIN_CPP_DIR}/tanuki.h.in
  ${BUILD_SRC_MAIN_CPP_DIR}/tanuki.h
  @ONLY
)

# Main libraries.

add_library(
  tanuki_s
  STATIC
  ${SRCS}
)

target_include_directories(
  tanuki_s
  PUBLIC
  ${BUILD_SRC_MAIN_CPP_DIR}
  ${Boost_INCLUDE_DIRS}
  ${SRC_MAIN_CPP_DIR}
)

# Set the file name of the static library.
set(
  TANUKI_STATIC_LIB_FILE_NAME
  "${CMAKE_STATIC_LIBRARY_PREFIX}tanuki_s${CMAKE_STATIC_LIBRARY_SUFFIX}"
)

add_library(
  tanuki
  SHARED
  ${SRCS}
)

target_include_directories(
  tanuki
  PUBLIC
  ${BUILD_SRC_MAIN_CPP_DIR}
  ${Boost_INCLUDE_DIRS}
  ${SRC_MAIN_CPP_DIR}
)

target_link_libraries(
  tanuki
  ${ARMA_LIB}
  ${AVRO_LIB}
  ${Boost_LIBRARIES}
  MPI::MPI_CXX
)

# Set the file name of the shared library.
set(
  TANUKI_SHARED_LIB_FILE_NAME
  "${CMAKE_SHARED_LIBRARY_PREFIX}tanuki${CMAKE_SHARED_LIBRARY_SUFFIX}"
)

install(
  TARGETS tanuki_s tanuki
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
  DIRECTORY
    ${BUILD_SRC_MAIN_CPP_DIR}/
    ${SRC_MAIN_CPP_DIR}/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  OPTIONAL
  FILES_MATCHING
  PATTERN "*.h*"
  PATTERN "*.in" EXCLUDE
)

install(
  DIRECTORY
    ${BUILD_SRC_MAIN_RESOURCES_DIR}/
    ${SRC_MAIN_RESOURCES_DIR}/
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}
  OPTIONAL
  PATTERN "*.in" EXCLUDE
  PATTERN "CMakeLists.txt" EXCLUDE
)

# Create configuration files for this package.

include(CMakePackageConfigHelpers)

configure_package_config_file(
  TanukiConfig.cmake.in
  TanukiConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/Tanuki
  PATH_VARS
    CMAKE_INSTALL_INCLUDEDIR
    CMAKE_INSTALL_LIBDIR
    CMAKE_INSTALL_DATAROOTDIR
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/TanukiConfigVersion.cmake
  VERSION ${TANUKI_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/TanukiConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/TanukiConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/Tanuki
)

if(BUILD_TEST)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/e2239ee6043f73722e7aa812a459f54a28552929.zip
  )
  FetchContent_MakeAvailable(googletest)

  include(CTest)
  include(GoogleTest)

  add_subdirectory(${SRC_TEST_CPP_DIR}/tanuki/algorithm)
  add_subdirectory(${SRC_TEST_CPP_DIR}/tanuki/avro_specific)
  add_subdirectory(${SRC_TEST_CPP_DIR}/tanuki/eda)
  add_subdirectory(${SRC_TEST_CPP_DIR}/tanuki/interop)
  add_subdirectory(${SRC_TEST_CPP_DIR}/tanuki/math)
  add_subdirectory(${SRC_TEST_CPP_DIR}/tanuki/scf_mi)
  add_subdirectory(${SRC_TEST_CPP_DIR}/tanuki/state)

  enable_testing()

  add_executable(
    test_instantiator
    ${SRC_TEST_CPP_DIR}/test_instantiator.cc
    ${TEST_SRCS}
  )

  target_include_directories(
    test_instantiator
    PUBLIC
    ${BUILD_SRC_MAIN_CPP_DIR}
    ${BUILD_SRC_TEST_CPP_DIR}
    ${Boost_INCLUDE_DIRS}
    ${SRC_MAIN_CPP_DIR}
    ${SRC_TEST_CPP_DIR}
  )

  target_link_libraries(
    test_instantiator
    ${ARMA_LIB}
    ${AVRO_LIB}
    ${Boost_LIBRARIES}
    MPI::MPI_CXX
    gtest_main
    tanuki_s
  )

  add_test(
    NAME test_instantiator
    COMMAND test_instantiator
  )

  gtest_discover_tests(test_instantiator)
endif()
