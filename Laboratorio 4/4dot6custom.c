#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t readers_ok;
    pthread_cond_t writers_ok;
    int active_readers;
    int waiting_readers;
    int active_writers;
    int waiting_writers;
    int prefer_writer;
} rwlock_t;

void rwlock_init(rwlock_t *lock, int prefer_writer) {
    pthread_mutex_init(&lock->mutex, NULL);
    pthread_cond_init(&lock->readers_ok, NULL);
    pthread_cond_init(&lock->writers_ok, NULL);
    lock->active_readers = 0;
    lock->waiting_readers = 0;
    lock->active_writers = 0;
    lock->waiting_writers = 0;
    lock->prefer_writer = prefer_writer;
}

void rwlock_acquire_readlock(rwlock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    while (lock->active_writers > 0 || (lock->prefer_writer && lock->waiting_writers > 0)) {
        lock->waiting_readers++;
        pthread_cond_wait(&lock->readers_ok, &lock->mutex);
        lock->waiting_readers--;
    }
    lock->active_readers++;
    pthread_mutex_unlock(&lock->mutex);
}

void rwlock_release_readlock(rwlock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    lock->active_readers--;
    if (lock->active_readers == 0) {
        pthread_cond_signal(&lock->writers_ok);
    }
    pthread_mutex_unlock(&lock->mutex);
}

void rwlock_acquire_writelock(rwlock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    while (lock->active_readers > 0 || lock->active_writers > 0) {
        lock->waiting_writers++;
        pthread_cond_wait(&lock->writers_ok, &lock->mutex);
        lock->waiting_writers--;
    }
    lock->active_writers++;
    pthread_mutex_unlock(&lock->mutex);
}

void rwlock_release_writelock(rwlock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    lock->active_writers--;
    if (lock->waiting_writers > 0) {
        pthread_cond_signal(&lock->writers_ok);
    } else {
        pthread_cond_broadcast(&lock->readers_ok);
    }
    pthread_mutex_unlock(&lock->mutex);
}

void rwlock_destroy(rwlock_t *lock) {
    pthread_mutex_destroy(&lock->mutex);
    pthread_cond_destroy(&lock->readers_ok);
    pthread_cond_destroy(&lock->writers_ok);
}

typedef struct {
    Node* head;
    rwlock_t lock; // Use the custom rwlock
} LinkedList;

LinkedList list;

void init_list(int prefer_writer) {
    list.head = NULL;
    rwlock_init(&list.lock, prefer_writer);
}

void insert(int data) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->data = data;
    new_node->next = NULL;

    rwlock_acquire_writelock(&list.lock);
    new_node->next = list.head;
    list.head = new_node;
    rwlock_release_writelock(&list.lock);
}

void print_list() {
    rwlock_acquire_readlock(&list.lock);
    Node* current = list.head;
    /*while (current != NULL) {
        printf("%d -> ", current->data);
        current = current->next;
    }
    printf("NULL\n");*/
    rwlock_release_readlock(&list.lock);
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

void benchmark(int prefer_writer, int num_readers, int num_writers) {
    struct timeval start, end;
    double elapsed;
    init_list(prefer_writer);
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
    printf("Elapsed time with %s preference: %f seconds\n", 
           prefer_writer ? "writer" : "reader", elapsed);
}

int main() {
    int num_readers = 5;
    int num_writers = 2;

    benchmark(0, num_readers, num_writers);
    benchmark(1, num_readers, num_writers);

    return 0;
}
