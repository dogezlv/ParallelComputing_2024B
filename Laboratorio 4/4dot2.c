#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

long long int number_of_tosses;
long long int number_in_circle = 0;
int thread_count;

pthread_mutex_t mutex;

void* monte_carlo(void* rank) {
    long long int local_number_in_circle = 0;
    long long int local_tosses = number_of_tosses / thread_count;
    unsigned int seed = (unsigned int)time(NULL) + (unsigned int)rank;

    for (long long int toss = 0; toss < local_tosses; toss++) {
        double x = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
        double y = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
        double distance_squared = x * x + y * y;
        if (distance_squared <= 1) {
            local_number_in_circle++;
        }
    }

    pthread_mutex_lock(&mutex);
    number_in_circle += local_number_in_circle;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <number_of_tosses> <thread_count>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    number_of_tosses = atoll(argv[1]);
    thread_count = atoi(argv[2]);
    pthread_t* thread_handles = malloc(thread_count * sizeof(pthread_t));
    pthread_mutex_init(&mutex, NULL);
    for (int thread = 0; thread < thread_count; thread++) {
        pthread_create(&thread_handles[thread], NULL, monte_carlo, (void*)(long)thread);
    }
    for (int thread = 0; thread < thread_count; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }
    double pi_estimate = 4 * number_in_circle / ((double)number_of_tosses);
    printf("Estimated π = %lf\n", pi_estimate);
    pthread_mutex_destroy(&mutex);
    free(thread_handles);

    return 0;
}
