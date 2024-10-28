#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void calculate_histogram(float* data, int data_count, int bin_count, float min_meas, float max_meas, int* local_histogram, int local_data_count) {
    float bin_width = (max_meas - min_meas) / bin_count;

    for (int i = 0; i < local_data_count; i++) {
        int bin = (int)((data[i] - min_meas) / bin_width);
        if (bin >= bin_count) bin = bin_count - 1;
        local_histogram[bin]++;
    }
}

int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int data_count, bin_count;
    float min_meas, max_meas;
    float* data = NULL;
    int* histogram = NULL;
    
    if (rank == 0) {
        printf("Enter the number of measurements: ");
        scanf("%d", &data_count);
        
        printf("Enter the number of bins: ");
        scanf("%d", &bin_count);
        
        printf("Enter min and max measurements: ");
        scanf("%f %f", &min_meas, &max_meas);

        data = (float*)malloc(data_count * sizeof(float));
        histogram = (int*)calloc(bin_count, sizeof(int));
        
        printf("Enter data:\n");
        for (int i = 0; i < data_count; i++) {
            scanf("%f", &data[i]);
        }
    }

    MPI_Bcast(&data_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&bin_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&min_meas, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&max_meas, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

    int local_data_count = data_count / size;
    float* local_data = (float*)malloc(local_data_count * sizeof(float));
    
    MPI_Scatter(data, local_data_count, MPI_FLOAT, local_data, local_data_count, MPI_FLOAT, 0, MPI_COMM_WORLD);

    int* local_histogram = (int*)calloc(bin_count, sizeof(int));
    calculate_histogram(local_data, data_count, bin_count, min_meas, max_meas, local_histogram, local_data_count);

    MPI_Reduce(local_histogram, histogram, bin_count, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("\nHistogram:\n");
        for (int i = 0; i < bin_count; i++) {
            printf("Bin %d: %d\n", i, histogram[i]);
        }
    }

    if (rank == 0) {
        free(data);
        free(histogram);
    }
    free(local_data);
    free(local_histogram);

    MPI_Finalize();
    return 0;
}
