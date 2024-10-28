#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void butterfly_sum(int local_value, int rank, int comm_size) {
    int step;
    int partner;
    for (step = 1; step < comm_size; step *= 2) {
        if (rank % (2 * step) == 0) {
            partner = rank + step;
            if (partner < comm_size) {
                int recv_value;
                MPI_Recv(&recv_value, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                local_value += recv_value;
            }
        } else {
            partner = rank - step;
            MPI_Send(&local_value, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
            break;
        }
    }
    if (rank == 0) {
        printf("Global Sum: %d\n", local_value);
    }
}

int main(int argc, char** argv) {
    int rank, comm_size;
    int local_value;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    srand(rank + 1);
    local_value = rand() % 10 + 1;
    printf("Process %d has local value: %d\n", rank, local_value);
    butterfly_sum(local_value, rank, comm_size);
    MPI_Finalize();
    return 0;
}
