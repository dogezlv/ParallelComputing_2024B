#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    int rank, comm_sz;
    int local_value, global_sum;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    local_value = rank;
    global_sum = local_value;
    int step = 1;
    while (step < comm_sz) {
        if (rank % (2 * step) == 0) {
            int received_value;
            if (rank + step < comm_sz) {
                MPI_Recv(&received_value, 1, MPI_INT, rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                global_sum += received_value;
            }
        } else if (rank % step == 0) {
            if (rank - step >= 0) {
                MPI_Send(&global_sum, 1, MPI_INT, rank - step, 0, MPI_COMM_WORLD);
                break;
            }
        }
        step *= 2;
    }
    if (rank == 0) {
        printf("The global sum is %d\n", global_sum);
    }
    MPI_Finalize();
    return 0;
}
