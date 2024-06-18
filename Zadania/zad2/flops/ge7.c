#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
// 256-bit AVX vector intrinsics
#include <immintrin.h>

// max macro
#define max(a,b) ((a) > (b) ? (a) : (b))
// block size
#define BLKSIZE 16
// macro to index a 1D array as a 2D array
#define IDX(i,j,n) ((i)*(n) + (j))

static double gtod_ref_time_sec = 0.0;

/* Adapted from the bl2_clock() routine in the BLIS library */

double dclock()
{
    double the_time, norm_sec;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    if (gtod_ref_time_sec == 0.0)
        gtod_ref_time_sec = (double)tv.tv_sec;
    norm_sec = (double)tv.tv_sec - gtod_ref_time_sec;
    the_time = norm_sec + tv.tv_usec * 1.0e-6;
    return the_time;
}

double calculate_gflops(int size)
{
    double flops;
    flops = 2.0 * size * size * size / 3;
    return flops * 1.0e-09;
}

int ge(double *A, int SIZE)
{
    register unsigned i, j, k;
    register double multiplier;
    register __m256d mm_multiplier;
    register __m256d tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

    for (k = 0; k < SIZE; k++)
    {
        for (i = k + 1; i < SIZE; i++)
        {
            multiplier = A[IDX(i, k, SIZE)] / A[IDX(k, k, SIZE)];
            mm_multiplier = _mm256_set1_pd(multiplier);

            for (j = k + 1; j < SIZE;)
            {
                if (j < (max(SIZE - BLKSIZE, 0)))
                {
                    tmp0 = _mm256_loadu_pd(&A[IDX(i, j, SIZE)]);
                    tmp1 = _mm256_loadu_pd(&A[IDX(k, j, SIZE)]);
                    tmp2 = _mm256_loadu_pd(&A[IDX(i, j + 4, SIZE)]);
                    tmp3 = _mm256_loadu_pd(&A[IDX(k, j + 4, SIZE)]);
                    tmp4 = _mm256_loadu_pd(&A[IDX(i, j + 8, SIZE)]);
                    tmp5 = _mm256_loadu_pd(&A[IDX(k, j + 8, SIZE)]);
                    tmp6 = _mm256_loadu_pd(&A[IDX(i, j + 12, SIZE)]);
                    tmp7 = _mm256_loadu_pd(&A[IDX(k, j + 12, SIZE)]);

                    tmp1 = _mm256_mul_pd(tmp1, mm_multiplier);
                    tmp3 = _mm256_mul_pd(tmp3, mm_multiplier);
                    tmp5 = _mm256_mul_pd(tmp5, mm_multiplier);
                    tmp7 = _mm256_mul_pd(tmp7, mm_multiplier);

                    tmp0 = _mm256_sub_pd(tmp0, tmp1);
                    tmp2 = _mm256_sub_pd(tmp2, tmp3);
                    tmp4 = _mm256_sub_pd(tmp4, tmp5);
                    tmp6 = _mm256_sub_pd(tmp6, tmp7);

                    _mm256_storeu_pd(&A[IDX(i, j, SIZE)], tmp0);
                    _mm256_storeu_pd(&A[IDX(i, j + 4, SIZE)], tmp2);
                    _mm256_storeu_pd(&A[IDX(i, j + 8, SIZE)], tmp4);
                    _mm256_storeu_pd(&A[IDX(i, j + 12, SIZE)], tmp6);

                    j += BLKSIZE;
                }
                else
                {
                    A[IDX(i, j, SIZE)] = A[IDX(i, j, SIZE)] - A[IDX(k, j, SIZE)] * multiplier;
                    j++;
                }
            }
        }
    }
    return 0;
}

int main(int argc, const char *argv[])
{
    register int i, j, k, iret;
    double dtime;
    int SIZE;
    FILE *output_file = fopen("output_ge7.csv", "w");
    fprintf(output_file, "Matrix Size,GFLOPS,Check\n");

    for (SIZE = 100; SIZE <= 2500; SIZE += 100)
    {
        double *matrix = (double *)malloc(SIZE * SIZE * sizeof(double));

        srand(1);
        for (i = 0; i < SIZE; i++)
        {
            for (j = 0; j < SIZE; j++)
            {
                matrix[IDX(i, j, SIZE)] = rand();
            }
        }

        dtime = dclock();
        iret = ge(matrix, SIZE);
        dtime = dclock() - dtime;

        double check = 0.0;
        for (i = 0; i < SIZE; i++)
        {
            for (j = 0; j < SIZE; j++)
            {
                check += matrix[IDX(i, j, SIZE)];
            }
        }

        fprintf(output_file, "%d,%le,%le\n", SIZE, calculate_gflops(SIZE) / dtime, check);
        fflush(stdout);
        free(matrix);
    }

    fclose(output_file);
    return iret;
}
