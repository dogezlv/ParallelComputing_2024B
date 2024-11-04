#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

void *thread_function(void *arg) {
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_threads>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int num_threads = atoi(argv[1]);
    if (num_threads <= 0) {
        fprintf(stderr, "Number of threads must be a positive integer.\n");
        exit(EXIT_FAILURE);
    }
    pthread_t threads[num_threads];
    struct timeval start, end;
    double total_time = 0.0;
    for (int i = 0; i < num_threads; i++) {
        gettimeofday(&start, NULL);
        if (pthread_create(&threads[i], NULL, thread_function, NULL) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
        gettimeofday(&end, NULL);
        total_time += (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec) / 1e6;
    }
    printf("Average time to create and terminate a thread: %f seconds\n", total_time / num_threads);
    return 0;
}
