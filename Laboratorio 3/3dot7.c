#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define NUM_PING_PONGS 1000

int main(int argc, char** argv) {
    int rank, size;
    const int PING = 0;
    const int PONG = 1;
    double start_time, end_time;
    clock_t start_clock, end_clock;
    int msg = 0;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        fprintf(stderr, "This program is designed to run with exactly 2 processes.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    if (rank == 0) {
        start_time = MPI_Wtime();
        for (int i = 0; i < NUM_PING_PONGS; i++) {
            MPI_Send(&msg, 1, MPI_INT, 1, PING, MPI_COMM_WORLD);
            MPI_Recv(&msg, 1, MPI_INT, 1, PONG, MPI_COMM_WORLD);
        }
        end_time = MPI_Wtime();
        printf("MPI_Wtime: %.6f seconds\n", end_time - start_time);
    } else if (rank == 1) {
        for (int i = 0; i < NUM_PING_PONGS; i++) {
            MPI_Recv(&msg, 1, MPI_INT, 0, PING, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(&msg, 1, MPI_INT, 0, PONG, MPI_COMM_WORLD);
        }
    }
    if (rank == 0) {
        start_clock = clock();
        for (int i = 0; i < NUM_PING_PONGS; i++) {
            MPI_Send(&msg, 1, MPI_INT, 1, PING, MPI_COMM_WORLD);
            MPI_Recv(&msg, 1, MPI_INT, 1, PONG, MPI_COMM_WORLD);
        }
        end_clock = clock();
        double elapsed_time_clock = (double)(end_clock - start_clock) / CLOCKS_PER_SEC;
        printf("clock(): %.6f seconds\n", elapsed_time_clock);
    }

    MPI_Finalize();
    return 0;
}
