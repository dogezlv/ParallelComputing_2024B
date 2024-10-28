#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int main(int argc, char* argv[]) {
    int rank, size;
    long long int tosses_per_process;
    long long int total_tosses, local_hits = 0, global_hits = 0;
    double x, y, distance_squared;
    unsigned int seed;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        if (argc != 2) {MPI_Abort(MPI_COMM_WORLD, 1);}
        total_tosses = atoll(argv[1]);
    }
    MPI_Bcast(&total_tosses, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
    tosses_per_process = total_tosses / size;
    seed = (unsigned int)(time(NULL) + rank);
    for (long long int toss = 0; toss < tosses_per_process; toss++) {
        x = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
        y = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
        distance_squared = x * x + y * y;
        if (distance_squared <= 1) {
            local_hits++;
        }
    }
    MPI_Reduce(&local_hits, &global_hits, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        long double pi_estimate = 4.0 * global_hits / total_tosses;
        printf("Estimated value of π: %Lf\n", pi_estimate);
    }
    MPI_Finalize();
    return 0;
}
