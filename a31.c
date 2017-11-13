/*
 * Benjamin A. Slack
 * 11.12.17
 * CS5260
 * Assignment 3.1
 * Description:
 * Default OMP scheduling probe. This executable
 * will take two inputs from the command line:
 * # of iterations, # of threads
 * It will then tally the ranges of iterations
 * scheduled by each of the specified threads
 * under default scheduling conditions
 *
 * example:
 *
 * input:
 * a31 -i 16 -t 2
 *
 * output:
 * Thread 0 : 0-7
 * Thread 1: 8-15
 */

#define DEFAULT_ITER 16
#define DEFAULT_THREADS 2
#define TOK_ITER "-i"
#define TOK_THRD "-t"
#define BUF_SIZE 4096

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include <errno.h>

int main(int argc, char **argv) {
    int no_iter = DEFAULT_ITER;
    int no_threads = DEFAULT_THREADS;
    //parse input
    if (argc > 1){
        for (int i = 0; i < argc; i++){
            if (strcmp(argv[i], TOK_ITER)==0){
                no_iter = atoi(argv[i+1]);
                if (errno != 0){
                    perror("a31: ");
                    exit(-1);
                }
            }else if (strcmp(argv[i], TOK_THRD)==0){
                no_threads = atoi(argv[i+1]);
                if (errno != 0){
                    perror("a31: ");
                    exit(-2);
                }
            }
        }
    }
    //setup thread tracker
    int thread_tracker[no_iter];

    //schedule a loop
    int index = 0;
#pragma omp parallel for num_threads(no_threads) \
    default(none) private(index) shared(thread_tracker, no_iter, no_threads)
    for (index = 0; index < no_iter; index++){
        thread_tracker[index] = omp_get_thread_num();
    }
    //generate print statements from tracker
    char buf[BUF_SIZE];
    memset(&buf, '\0', BUF_SIZE);
    int start_range = -1;
    int end_range = -1;
    for (int i = 0; i < no_threads; i++) {
        memset(&buf, '\0', BUF_SIZE);
        sprintf(buf, "Thread %d: Iterations - ", i);
        for (int j = 0; j < no_iter; j++) {
            //found beginning a range
            if ((start_range < 0) && (i == thread_tracker[j])) {
                start_range = j;

            //found end of a range
            }
            if ((end_range < 0) && (start_range >= 0) && (i != thread_tracker[j])) {
                    end_range = j;
            }

            //ran out of iterations to check, so the range is done
            if ((j == no_iter-1) && (start_range >= 0)){
                end_range = j + 1;
            }

            //output a range to the buffer
            if ((start_range >= 0) && (end_range >= 0)) {
                // single item range
                if (start_range == end_range + 1) {
                    sprintf(buf+strlen(buf), "%d, ", start_range);
                    start_range = -1;
                    end_range = -1;
                // multi item range
                } else {
                    sprintf(buf+strlen(buf), "%d-%d, ", start_range, end_range-1);
                    start_range = -1;
                    end_range = -1;
                }
            }
        }
        //trim off the end of the line
        sprintf((buf + strlen(buf) - 2), ".\n");
        //output the buffer
        printf("%s", buf);
    }

    exit(0);
}