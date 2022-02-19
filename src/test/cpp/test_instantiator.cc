#include <cassert>

#include <gtest/gtest.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    assert(provided >= MPI_THREAD_FUNNELED);
  }

  int mpi_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

  if (mpi_rank != 0) {
    auto &listeners = ::testing::UnitTest::GetInstance()->listeners();
    delete listeners.Release(listeners.default_result_printer());
  }

  const int error_status = RUN_ALL_TESTS();

  MPI_Finalize();

  return error_status;
}
