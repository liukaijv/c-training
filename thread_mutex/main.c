#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define EXIT_ERROR -1
#define THREAD_SIZE 1000
#define LOOP_SIZE 1000

int num = 0;
int num_mutex = 0;

void *func(void *arg) {
    for (int i = 0; i < LOOP_SIZE; ++i) {
        num++;
    }
    return NULL;
}

// 初始方式一
//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// 方式二 pthread_mutex_init(&mutex,NULL);
pthread_mutex_t mutex;

void *func_with_mutex(void *arg) {
    for (int i = 0; i < LOOP_SIZE; ++i) {
        int ret = pthread_mutex_lock(&mutex);
        if (ret != 0) {
            perror("pthread_mutex_lock error: ");
            exit(EXIT_ERROR);
        }
        num_mutex++;
        ret = pthread_mutex_unlock(&mutex);
        if (ret != 0) {
            perror("pthread_mutex_unlock error: ");
            exit(EXIT_ERROR);
        }
    }
    return NULL;
}

int main(int argc, char **argv) {

    int ret = 0;

    pthread_t threads[THREAD_SIZE];
    for (int i = 0; i < THREAD_SIZE; ++i) {
        ret = pthread_create(&threads[i], NULL, func, NULL);
        if (ret != 0) {
            perror("create thread error: ");
            exit(EXIT_ERROR);
        }
    }
    for (int i = 0; i < THREAD_SIZE; ++i) {
        ret = pthread_join(threads[i], NULL);
        if (ret != 0) {
            perror("join thread error: ");
            exit(EXIT_ERROR);
        }
    }
    printf("num should be: %d, num: %d\n", THREAD_SIZE * LOOP_SIZE, num);

//    这个是加了互斥锁的
    ret = pthread_mutex_init(&mutex, NULL);
    if (ret != 0) {
        perror("pthread_mutex_init error: ");
        exit(EXIT_ERROR);
    }
    pthread_t mutex_threads[THREAD_SIZE];
    for (int i = 0; i < THREAD_SIZE; ++i) {
        ret = pthread_create(&mutex_threads[i], NULL, func_with_mutex, NULL);
        if (ret != 0) {
            perror("create thread error: ");
            exit(EXIT_ERROR);
        }
    }
    for (int i = 0; i < THREAD_SIZE; ++i) {
        ret = pthread_join(mutex_threads[i], NULL);
        if (ret != 0) {
            perror("join thread error: ");
            exit(EXIT_ERROR);
        }
    }
    printf("num_mutex should be: %d, num_mutex: %d\n", THREAD_SIZE * LOOP_SIZE, num_mutex);

    ret = pthread_mutex_destroy(&mutex);
    if (ret != 0) {
        perror("pthread_mutex_destroy error: ");
        exit(EXIT_ERROR);
    }

    return 0;
}