#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    pthread_rwlock_t rwlock;
} LinkedList;

LinkedList list;

void init_list() {
    list.head = NULL;
    pthread_rwlock_init(&list.rwlock, NULL);
}

void insert(int data) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->data = data;
    new_node->next = NULL;

    pthread_rwlock_wrlock(&list.rwlock);
    new_node->next = list.head;
    list.head = new_node;
    pthread_rwlock_unlock(&list.rwlock);
}

void print_list() {
    pthread_rwlock_rdlock(&list.rwlock);
    Node* current = list.head;
    /*while (current != NULL) {
        printf("%d -> ", current->data);
        current = current->next;
    }
    printf("NULL\n");*/
    pthread_rwlock_unlock(&list.rwlock);
}

void* reader(void* arg) {
    for (int i = 0; i < 10; i++) {
        print_list();
    }
    return NULL;
}

void* writer(void* arg) {
    for (int i = 0; i < 10; i++) {
        insert(rand() % 100);
    }
    return NULL;
}

void benchmark(int num_readers, int num_writers) {
    struct timeval start, end;
    double elapsed;
    init_list();
    pthread_t read_threads[num_readers], write_threads[num_writers];
    gettimeofday(&start, NULL);

    for (int i = 0; i < num_writers; i++) {
        pthread_create(&write_threads[i], NULL, writer, NULL);
    }
    for (int i = 0; i < num_readers; i++) {
        pthread_create(&read_threads[i], NULL, reader, NULL);
    }
    for (int i = 0; i < num_writers; i++) {
        pthread_join(write_threads[i], NULL);
    }
    for (int i = 0; i < num_readers; i++) {
        pthread_join(read_threads[i], NULL);
    }
    gettimeofday(&end, NULL);
    elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    printf("Elapsed time with Pthreads read-write lock: %f seconds\n", elapsed);
    pthread_rwlock_destroy(&list.rwlock);
}

int main() {
    int num_readers = 5;
    int num_writers = 2;
    printf("Performance with Pthreads read-write lock:\n");
    benchmark(num_readers, num_writers);
    return 0;
}
