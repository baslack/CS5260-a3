/*
 * Benjamin A. Slack
 * 11.12.17
 * CS5260
 * Assignment 3.1
 * Description:
 * Parallel Count Sort. This
 * executable will print comparisons
 * of the timing of three different
 * sorting algorithms to stdout.
 * Usage
 * a33 -t <number_of_threads>
 *     -n <number_of_random_ints>
 *     -h <the high value for the ints>
 *     -s <the seed to use for generation>
 *     -i <number of iterations to average>
 *

 */

#define  _POSIX_C_SOURCE 199309L
#define ECLOCK -1
#define ECOMMAND -2
#define DEFAULT_ITER 10
#define DEFAULT_NUM_THREADS 4
#define DEFAULT_N 10
#define DEFAULT_HIGH 100
#define DEFAULT_SORT 1
#define TOK_THREAD "-t"
#define TOK_N "-n"
#define TOK_HIGH "-h"
#define TOK_SEED "-s"
#define TOK_ITER "-i"
#define TOK_QSORT "-q"
#define TOK_PARALLEL "-p"
#define TOK_SERIAL "-l"
#define BUFFER_SIZE 4096
#define MAX_FOR_OUTPUT 10

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include <math.h>

void dumpA(char *, int [], int);

int randint(int);

void count_sort(int[], int);

void parallel_count_sort(int[], int);

void qsort_wrapper(int[], int);

int cmp_int(const void *, const void *);

double timed_sort(void (*)(int [], int), int[], int);

struct timespec diff(struct timespec, struct timespec);

//global
int no_threads = DEFAULT_NUM_THREADS;
enum sort_type {SERIAL, PARALLEL, QSORT};

int main(int argc, char **argv) {
    int n = DEFAULT_N;
    int iter = DEFAULT_ITER;
    int seed = time(NULL);
    int high = DEFAULT_HIGH;
    enum sort_type sort = DEFAULT_SORT;

    //parse commands
    if (argc > 1) {
        for (int i = 0; i < argc; i++) {
            if (strcmp(argv[i], TOK_ITER) == 0) {
                iter = atoi(argv[i + 1]);
                if (errno < 0) {
                    perror("a33, main, iter");
                    exit(ECOMMAND);
                }
            } else if (strcmp(argv[i], TOK_HIGH) == 0) {
                high = atoi(argv[i + 1]);
                if (errno < 0) {
                    perror("a33, main, high");
                    exit(ECOMMAND);
                }
            } else if (strcmp(argv[i], TOK_N) == 0) {
                n = atoi(argv[i + 1]);
                if (errno < 0) {
                    perror("a33, main, n");
                    exit(ECOMMAND);
                }
            } else if (strcmp(argv[i], TOK_SEED) == 0) {
                seed = atoi(argv[i + 1]);
                if (errno < 0) {
                    perror("a33, main, seed");
                    exit(ECOMMAND);
                }
            } else if (strcmp(argv[i], TOK_THREAD) == 0) {
                no_threads = atoi(argv[i + 1]);
                if (errno < 0) {
                    perror("a33, main, threads");
                    exit(ECOMMAND);
                }
            } else if (strcmp(argv[i], TOK_QSORT) == 0) {
                sort = QSORT;
            }else if (strcmp(argv[i], TOK_PARALLEL) == 0) {
                sort = PARALLEL;
            }else if (strcmp(argv[i], TOK_SERIAL) == 0) {
                sort = SERIAL;
            }

        }
    }
    //create array, and copies
    srand(seed);
    int A[n];
    for (int i = 0; i < n; i++) {
        A[i] = randint(high);
    }

    //dump array contents
    if (n <= MAX_FOR_OUTPUT) {
        dumpA("A", A, n);
    }

    int A_1[n];
    memcpy(A_1, A, n * sizeof(int));


    double sum = 0.0;

    //iterate
    for (int i = 0; i < iter; i++) {
        //perform timed sort for the requested function
        switch (sort){
            case SERIAL:
                sum += timed_sort(&count_sort, A_1, n);
                break;
            case PARALLEL:
                sum += timed_sort(&parallel_count_sort, A_1, n);
                break;
            case QSORT:
                sum += timed_sort(&qsort_wrapper, A_1, n);
                break;
        }

        //reset the array for the next iteration
        if (i < iter - 1) {
            memcpy(A_1, A, n * sizeof(int));
        }
    }

    //dump sorted array contents
    if (n <= MAX_FOR_OUTPUT) {
        dumpA("A_1", A_1, n);
    }

    //average
    double avg = sum / iter;

    //output averages to std out
    switch (sort){
        case SERIAL:
            printf("serial count sort: n=%d, avg=%f\n", n, avg);
            break;
        case PARALLEL:
            printf("parallel count sort: n=%d, threads=%d, avg=%f\n", n, no_threads, avg);
            break;
        case QSORT:
            printf("qsort: n=%d, avg=%f\n", n, avg);
            break;
    }
    exit(0);
}

void dumpA(char *name, int A[], int n) {
    char *buf = calloc(BUFSIZ, sizeof(char));
    sprintf(buf, "%s: [", name);
    for (int i = 0; i < n; i++) {
        sprintf(buf + strlen(buf), "%d, ", A[i]);
    }
    sprintf(buf + strlen(buf) - 2, "]\n");
    printf("%s", buf);
    free(buf);
}

int randint(int n) {
// https://stackoverflow.com/questions/822323/how-to-generate-a-random-number-in-c
/* Returns an integer in the range [0, n).
 *
 * Uses rand(), and so is affected-by/affects the same seed.
 */

    if ((n - 1) == RAND_MAX) {
        return rand();
    } else {
        // Chop off all of the values that would cause skew...
        long end = RAND_MAX / n; // truncate skew
        assert (end > 0L);
        end *= n;

        // ... and ignore results from rand() that fall above that limit.
        // (Worst case the loop condition should succeed 50% of the time,
        // so we can expect to bail out of this loop pretty quickly.)
        int r;
        while ((r = rand()) >= end);

        return r % n;
    }
}

void count_sort(int a[], int n) {
    int i, j, count;
    int *temp = malloc(n * sizeof(int));
    for (i = 0; i < n; i++) {
        count = 0;
        for (j = 0; j < n; j++) {
            if (a[j] < a[i])
                count++;
            else if (a[j] == a[i] && j < i)
                count++;
        }
        temp[count] = a[i];
    }
    memcpy(a, temp, n * sizeof(int));
    free(temp);
} /* count_sort */


//disabled to try para memcpy
//void parallel_count_sort(int a[], int n) {
//    int i, j, count;
//    int *temp = malloc(n * sizeof(int));
//#pragma omp parallel for \
//    num_threads(no_threads) \
//    default(none) \
//    private(i, j, count) \
//    shared(a, n, temp)
//    for (i = 0; i < n; i++) {
//        count = 0;
//        for (j = 0; j < n; j++) {
//            if (a[j] < a[i])
//                count++;
//            else if (a[j] == a[i] && j < i)
//                count++;
//        }
//        temp[count] = a[i];
//    }
//    memcpy(a, temp, n * sizeof(int));
//    free(temp);
//}

void parallel_count_sort(int a[], int n) {
    int i, j, count;
    int *temp = malloc(n * sizeof(int));
#pragma omp parallel \
    num_threads(no_threads) \
    default(none) \
    private(i, j, count) \
    shared(a, n, temp)
#pragma omp for
    for (i = 0; i < n; i++) {
        count = 0;
        for (j = 0; j < n; j++) {
            if (a[j] < a[i])
                count++;
            else if (a[j] == a[i] && j < i)
                count++;
        }
        temp[count] = a[i];
    }
#pragma omp for
    for (i = 0; i < n; i++){
        memcpy(a + i, temp + i, sizeof(int));
    }
    free(temp);
}


int cmp_int(const void *pa, const void *pb) {
    int a = *(int *) pa;
    int b = *(int *) pb;
    if (a == b) {
        return 0;
    } else if (a > b) {
        return 1;
    } else {
        return -1;
    }
}

void qsort_wrapper(int a[], int n) {
    qsort((void *) a, n, sizeof(int), &cmp_int);
}

double timed_sort(void (*fn)(int [], int), int A[], int n) {
    struct timespec start;
    struct timespec end;
    int ret_val;
    ret_val = clock_gettime(CLOCK_REALTIME, &start);
    if (ret_val < 0) {
        perror("a33, timed_sort, clock_gettime");
        exit(ECLOCK);
    }
    (*fn)(A, n);
    ret_val = clock_gettime(CLOCK_REALTIME, &end);
    if (ret_val < 0) {
        perror("a33, timed_sort, clock_gettime");
        exit(ECLOCK);
    }
    struct timespec elaspsed = diff(start, end);

    return ((double) elaspsed.tv_sec) + ((double) elaspsed.tv_nsec) * pow(10.0, -9.0);
}

struct timespec diff(struct timespec start, struct timespec end) {
    struct timespec temp;
    if ((end.tv_nsec - start.tv_nsec) < 0) {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}