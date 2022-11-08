#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define EXIT_ERROR -1
#define THREAD_SIZE 5

static void *func1(void *arg) {
    // 编译器提示有问题
    int thread_arg = (int) arg;
    printf("func %d exec\n", thread_arg);
    return arg;
}

static void *func2(void *arg) {
    int thread_arg = *(int *) arg;
    printf("func %d exec\n", thread_arg);
    // 记得释放内存
    free(arg);
    // 传出线程的变量也是动态的
    int *res = malloc(sizeof(res));
    *res = thread_arg;
    return res;
}

int main(int argc, char **argv) {

    pthread_t threads[THREAD_SIZE];
    int ret;
    void *result;

    /**
     * 传参 int当作地址用
     * 编译提示有问题，但是结果是预期的
     * 我理解是编译提示有问题那应该就是错的，退后再查下这种方式到底行不行
     */
    for (int i = 0; i < THREAD_SIZE; ++i) {
        // 这里传地址&i，得不到结果，线程是异步的，循环是同步的，i变成4了
        // func1不用写成&func1，函数名相当于函数的首地址
        ret = pthread_create(&threads[i], NULL, (void *(*)(void *)) func1, (void *) i);
        if (ret < 0) {
            printf("thread create %d error\n", i);
            exit(EXIT_ERROR);
        }
    }

    for (int i = 0; i < THREAD_SIZE; ++i) {
        // 等待情况，线程执行完后不会清理干净，要等执行它的主线程调用pthread_join
        ret = pthread_join(threads[i], &result);
        if (ret < 0) {
            printf("thread join  %d error\n", i);
            exit(EXIT_ERROR);
        }
        printf("thread %d result: %d\n", i, (int) result);
    }

    sleep(2);

    printf("-------\n");

    /**
     * 传参 动态内存
     * 传入和传出线程的都是动态内存变量
     */
    for (int i = 0; i < THREAD_SIZE; ++i) {
        // 把i传给动态内存变量
        int *p = malloc(sizeof(p));
        if (p == NULL) {
            printf("%d malloc p error\n", i);
            exit(EXIT_ERROR);
        }
        *p = i;
        // 这里传地址&i，得不到结果，线程是异步的，循环是同步的，i变成5了
        ret = pthread_create(&threads[i], NULL, (void *(*)(void *)) func2, (void *) p);
        if (ret < 0) {
            printf("thread create %d error\n", i);
            exit(EXIT_ERROR);
        }
    }

    for (int i = 0; i < THREAD_SIZE; ++i) {
        // 等待情况，线程执行完后不会清理干净，要等执行它的主线程调用pthread_join
        ret = pthread_join(threads[i], &result);
        if (ret < 0) {
            printf("thread join  %d error\n", i);
            exit(EXIT_ERROR);
        }
        printf("thread %d result: %d\n", i, *(int *) result);
        // 记得释放内存
        free(result);
    }

    return 0;
}