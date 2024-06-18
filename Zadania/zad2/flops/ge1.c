#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <float.h>

#define IDX(i, j, n) (((j)+ (i)*(n)))

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

int ge(double **A, int SIZE) {
    int i, j, k;
    for (k = 0; k < SIZE; k++) { 
        for (i = k + 1; i < SIZE; i++) { 
            for (j = k + 1; j < SIZE; j++) { 
                A[i][j] = A[i][j] - A[k][j] * (A[i][k] / A[k][k]);
            } 
        }
    }
    return 0;
}

int main(int argc, const char *argv[]) {
    int i, j, k, iret;
    double dtime;
    int SIZE;
    FILE *output_file = fopen("output_ge1.csv", "w");
    fprintf(output_file, "Matrix Size,GFLOPS,Check\n");

    for (SIZE = 100; SIZE <= 2500; SIZE += 100) {
        double **matrix = (double **)malloc(SIZE * sizeof(double *));
        for (i = 0; i < SIZE; i++) {
            matrix[i] = (double *)malloc(SIZE * sizeof(double));
        }

        srand(1);
        for (i = 0; i < SIZE; i++) {
            for (j = 0; j < SIZE; j++) {
                matrix[i][j] = rand();
            }
        }

        dtime = dclock();
        iret = ge(matrix, SIZE);
        dtime = dclock() - dtime;

        double check = 0.0;
        for (i = 0; i < SIZE; i++) {
            for (j = 0; j < SIZE; j++) {
                check = check + matrix[i][j];
            }
        }

        fprintf(output_file, "%d,%le,%le\n", SIZE, calculate_gflops(SIZE) / dtime, check);
        fflush(stdout);
        for (i = 0; i < SIZE; i++) {
            free(matrix[i]);
        }
        free(matrix);
    }

    fclose(output_file);
    return iret;
}
