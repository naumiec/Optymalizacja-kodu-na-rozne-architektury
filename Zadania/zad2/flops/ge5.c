#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <float.h>

#define IDX(i, j, n) (((j)+ (i)*(n)))
#define max(a,b) ((a) > (b) ? (a) : (b))
#define BLKSIZE 8

static double gtod_ref_time_sec = 0.0;

double dclock(){
    double the_time, norm_sec;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    if (gtod_ref_time_sec == 0.0)
        gtod_ref_time_sec = (double)tv.tv_sec;
    norm_sec = (double)tv.tv_sec - gtod_ref_time_sec;
    the_time = norm_sec + tv.tv_usec * 1.0e-6;
    return the_time;
}

double calculate_gflops(int size) {
    double flops;
    flops = 2.0 * size * size * size / 3;
    return flops * 1.0e-09;
}

int ge(double *A, int SIZE) {
    register int i, j, k;
    double multiplier;
    for (k = 0; k < SIZE; k++) {
        for (i = k + 1; i < SIZE; i++) {
            multiplier = A[IDX(i, k, SIZE)] / A[IDX(k, k, SIZE)];
            for (j = k + 1; j < SIZE;) {
                if (j < (max(SIZE - BLKSIZE, 0))) {
                    A[IDX(i, j, SIZE)] = A[IDX(i, j, SIZE)] - A[IDX(k, j, SIZE)] * multiplier;
                    A[IDX(i, j + 1, SIZE)] = A[IDX(i, j + 1, SIZE)] - A[IDX(k, j + 1, SIZE)] * multiplier;
                    A[IDX(i, j + 2, SIZE)] = A[IDX(i, j + 2, SIZE)] - A[IDX(k, j + 2, SIZE)] * multiplier;
                    A[IDX(i, j + 3, SIZE)] = A[IDX(i, j + 3, SIZE)] - A[IDX(k, j + 3, SIZE)] * multiplier;
                    A[IDX(i, j + 4, SIZE)] = A[IDX(i, j + 4, SIZE)] - A[IDX(k, j + 4, SIZE)] * multiplier;
                    A[IDX(i, j + 5, SIZE)] = A[IDX(i, j + 5, SIZE)] - A[IDX(k, j + 5, SIZE)] * multiplier;
                    A[IDX(i, j + 6, SIZE)] = A[IDX(i, j + 6, SIZE)] - A[IDX(k, j + 6, SIZE)] * multiplier;
                    A[IDX(i, j + 7, SIZE)] = A[IDX(i, j + 7, SIZE)] - A[IDX(k, j + 7, SIZE)] * multiplier;
                    j += BLKSIZE;
                } else {
                    A[IDX(i, j, SIZE)] = A[IDX(i, j, SIZE)] - A[IDX(k, j, SIZE)] * multiplier;
                    j++;
                }
            }
        }
    }
    return 0;
}

int main(int argc, const char *argv[]) {
    register int i, j, k, iret;
    double dtime;
    int SIZE;
    FILE *output_file = fopen("output_ge5.csv", "w");
    fprintf(output_file, "Matrix Size,GFLOPS,Check\n");

    for (SIZE = 100; SIZE <= 2500; SIZE += 100) {
        double *matrix = (double *)malloc(SIZE * SIZE * sizeof(double));

        srand(1);
        for (i = 0; i < SIZE; i++) {
            for (j = 0; j < SIZE; j++) {
                matrix[IDX(i, j, SIZE)] = rand();
            }
        }

        dtime = dclock();
        iret = ge(matrix, SIZE);
        dtime = dclock() - dtime;

        double check = 0.0;
        for (i = 0; i < SIZE; i++) {
            for (j = 0; j < SIZE; j++) {
                check = check + matrix[IDX(i, j, SIZE)];
            }
        }

        fprintf(output_file, "%d,%le,%le\n", SIZE, calculate_gflops(SIZE) / dtime, check);
        fflush(stdout);
        free(matrix);
    }

    fclose(output_file);
    return iret;
}
