// gcc -mavx

// gcc -mavx $(pkg-config --cflags --libs papi) -lm ge6.c -o ge6.o
// ./ge6.o 1000


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
// SSE3 vector intrinsics
#include <x86intrin.h>
#include <papi.h>

// max macro
#define max(a,b) ((a) > (b) ? (a) : (b))
// block size
#define BLKSIZE 8
// macro to index a 1D array as a 2D array
#define IDX(i,j,n) ((i)*(n) + (j))

#define NUM_EVENT 4
#define ERROR_RETURN(retval) { fprintf(stderr, "Error %d %s:line %d: \n", retval,__FILE__,__LINE__);  exit(retval); }

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
  register __m128d mm_multiplier;
  register __m128d tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

  for (k = 0; k < SIZE; k++) { 
    for (i = k+1; i < SIZE; i++) { 
            multiplier = A[IDX(i, k, SIZE)] / A[IDX(k, k, SIZE)];
            mm_multiplier[0] = multiplier;
            mm_multiplier[1] = multiplier;
      for (j = k+1; j < SIZE; ) { 
        if (j < (max(SIZE - BLKSIZE, 0))) {
          mm_multiplier = _mm_set1_pd(multiplier);
          
          tmp0 = _mm_loadu_pd(A + IDX(i, j, SIZE));
          tmp1 = _mm_loadu_pd(A + IDX(k, j, SIZE));
          tmp2 = _mm_loadu_pd(A + IDX(i, j + 2, SIZE));
          tmp3 = _mm_loadu_pd(A + IDX(k, j + 2, SIZE));
          tmp4 = _mm_loadu_pd(A + IDX(i, j + 4, SIZE));
          tmp5 = _mm_loadu_pd(A + IDX(k, j + 4, SIZE));
          tmp6 = _mm_loadu_pd(A + IDX(i, j + 6, SIZE));
          tmp7 = _mm_loadu_pd(A + IDX(k, j + 6, SIZE));

          tmp1 = _mm_mul_pd(tmp1, mm_multiplier);
          tmp3 = _mm_mul_pd(tmp3, mm_multiplier);
          tmp5 = _mm_mul_pd(tmp5, mm_multiplier);
          tmp7 = _mm_mul_pd(tmp7, mm_multiplier);

          tmp0 = _mm_sub_pd(tmp0, tmp1);
          tmp2 = _mm_sub_pd(tmp2, tmp3);
          tmp4 = _mm_sub_pd(tmp4, tmp5);
          tmp6 = _mm_sub_pd(tmp6, tmp7);

          _mm_storeu_pd(A + IDX(i, j, SIZE), tmp0);
          _mm_storeu_pd(A + IDX(i, j + 2, SIZE), tmp2);
          _mm_storeu_pd(A + IDX(i, j + 4, SIZE), tmp4);
          _mm_storeu_pd(A + IDX(i, j + 6, SIZE), tmp6);

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
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <matrix_size>\n", argv[0]);
    return EXIT_FAILURE;
  }

  /* PAPI counters variables */
  int EventSet = PAPI_NULL;
  int event_codes[NUM_EVENT] = {PAPI_TOT_CYC, PAPI_TOT_INS, PAPI_L1_DCM, PAPI_L2_ICH};
  long long values[NUM_EVENT];
  int retval;

  register int i, j, k, iret;
  double dtime;
  int SIZE = atoi(argv[1]);

  double *matrix = (double *)malloc(SIZE * SIZE * sizeof(double));

  srand(1);
  for (i = 0; i < SIZE; i++) {
    for (j = 0; j < SIZE; j++) {
      matrix[IDX(i, j, SIZE)] = rand();
    }
  }

  /* initializing library */
  if ((retval = PAPI_library_init(PAPI_VER_CURRENT)) != PAPI_VER_CURRENT) {
    fprintf(stderr, "PAPI library init error!\n");
    exit(1);
  }
  /* Creating event set */
  if ((retval = PAPI_create_eventset(&EventSet)) != PAPI_OK) {
    ERROR_RETURN(retval);
  }
  /* Add the array of events PAPI_TOT_CYC, PAPI_TOT_INS, PAPI_L1_DCM, PAPI_L2_ICH to the eventset */
  if ((retval = PAPI_add_events(EventSet, event_codes, NUM_EVENT)) != PAPI_OK) {
    ERROR_RETURN(retval);
  }
  /* Start counting */
  if ((retval = PAPI_start(EventSet)) != PAPI_OK) {
    ERROR_RETURN(retval);
  }

  dtime = dclock();
  iret = ge(matrix, SIZE);
  dtime = dclock() - dtime;

  /* Stop counting, this reads from the counter as well as stop it. */
  if ((retval = PAPI_stop(EventSet, values)) != PAPI_OK) {
    ERROR_RETURN(retval);
  }

  double check = 0.0;
  for (i = 0; i < SIZE; i++) {
    for (j = 0; j < SIZE; j++) {
      check += matrix[IDX(i, j, SIZE)];
    }
  }

  // Combined print statement as requested
  printf("%d,%le,%lld,%lld,%lld,%lld,%le\n", SIZE, dtime, values[0], values[1], values[2], values[3], check);
  fflush(stdout);

  free(matrix);

  return iret;
}
