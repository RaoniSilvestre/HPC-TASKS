#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
  // 1. Initialize the MPI environment
  MPI_Init(&argc, &argv);

  // 2. Get the number of processes
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // 3. Get the rank of the individual process
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  // Get the name of the processor
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);

  // Print a hello from this specific rank
  printf("Hello from processor %s, rank %d out of %d processors\n",
         processor_name, world_rank, world_size);

  // 4. Finalize the MPI environment
  MPI_Finalize();
  return 0;
}
