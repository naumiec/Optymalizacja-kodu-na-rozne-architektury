// gcc -mavx512f

// gcc -mavx512f $(pkg-config --cflags --libs papi) -lm ge8.c -o ge8a.o
// gcc -O2 -mavx512f $(pkg-config --cflags --libs papi) -lm ge8.c -o ge8a.o
// ./ge7a.o 1000
// ./ge7b.o 1000


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
// 512-bit AVX-512 vector intrinsics
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

int ge(double *A, int SIZE)
{
    register unsigned i, j, k;
    register double multiplier;
    register __m512d mm_multiplier;
    register __m512d tmp0, tmp1, tmp2, tmp3;

    for (k = 0; k < SIZE; k++) { 
        for (i = k + 1; i < SIZE; i++) { 
            multiplier = A[IDX(i, k, SIZE)] / A[IDX(k, k, SIZE)];
            mm_multiplier = _mm512_set1_pd(multiplier);

            for (j = k + 1; j < SIZE; ) { 
                if (j < (max(SIZE - BLKSIZE, 0))) {
                    tmp0 = _mm512_loadu_pd(A + IDX(i, j, SIZE));
                    tmp1 = _mm512_loadu_pd(A + IDX(k, j, SIZE));
                    tmp2 = _mm512_loadu_pd(A + IDX(i, j + 8, SIZE));
                    tmp3 = _mm512_loadu_pd(A + IDX(k, j + 8, SIZE));

                    tmp1 = _mm512_mul_pd(tmp1, mm_multiplier);
                    tmp3 = _mm512_mul_pd(tmp3, mm_multiplier);

                    tmp0 = _mm512_sub_pd(tmp0, tmp1);
                    tmp2 = _mm512_sub_pd(tmp2, tmp3);

                    _mm512_storeu_pd(A + IDX(i, j, SIZE), tmp0);
                    _mm512_storeu_pd(A + IDX(i, j + 8, SIZE), tmp2);

                    j += BLKSIZE;
                } 
                else {
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
    int SIZE = 1500;

    double *matrix = (double *)malloc(SIZE * SIZE * sizeof(double));

    srand(1);
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            matrix[IDX(i, j, SIZE)] = rand();
        }
    }
    printf("call GE\n");
    dtime = dclock();
    iret = ge(matrix, SIZE);
    dtime = dclock() - dtime;
    printf("Time: %le \n", dtime);

    double check = 0.0;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            check += matrix[IDX(i, j, SIZE)];
        }
    }
    printf("Check: %le \n", check);
    fflush(stdout);

    free(matrix);
    return iret;
}
