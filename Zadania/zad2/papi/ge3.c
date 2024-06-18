// gcc -mavx

// gcc -mavx $(pkg-config --cflags --libs papi) -lm ge3.c -o ge3.o
// ./ge3.o 1000


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <papi.h>

#define NUM_EVENT 4

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

int ge(double **A, int SIZE)
{
  register int i, j, k;
  double multiplier;
  for (k = 0; k < SIZE; k++) {
    for (i = k + 1; i < SIZE; i++) {
      multiplier = A[i][k] / A[k][k];
      for (j = k + 1; j < SIZE; j++) {
        A[i][j] = A[i][j] - A[k][j] * multiplier;
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

  register int i, j, k, iret;
  double dtime;
  int SIZE = atoi(argv[1]);

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

  // PAPI initialization
  int EventSet = PAPI_NULL;
  int event_codes[NUM_EVENT] = {PAPI_TOT_CYC, PAPI_TOT_INS, PAPI_L1_DCM, PAPI_L2_ICH};
  long long values[NUM_EVENT];
  int retval;

  if ((retval = PAPI_library_init(PAPI_VER_CURRENT)) != PAPI_VER_CURRENT) {
    fprintf(stderr, "PAPI library init error!\n");
    exit(1);
  }
  if ((retval = PAPI_create_eventset(&EventSet)) != PAPI_OK) {
    fprintf(stderr, "PAPI create event set error!\n");
    exit(1);
  }
  if ((retval = PAPI_add_events(EventSet, event_codes, NUM_EVENT)) != PAPI_OK) {
    fprintf(stderr, "PAPI add events error!\n");
    exit(1);
  }

  if ((retval = PAPI_start(EventSet)) != PAPI_OK) {
    fprintf(stderr, "PAPI start error!\n");
    exit(1);
  }

  dtime = dclock();
  iret = ge(matrix, SIZE);
  dtime = dclock() - dtime;

  if ((retval = PAPI_stop(EventSet, values)) != PAPI_OK) {
    fprintf(stderr, "PAPI stop error!\n");
    exit(1);
  }

  double check = 0.0;
  for (i = 0; i < SIZE; i++) {
    for (j = 0; j < SIZE; j++) {
      check = check + matrix[i][j];
    }
  }

  // Combined print statement as requested
  printf("%d,%le,%lld,%lld,%lld,%lld,%le\n", SIZE, dtime, values[0], values[1], values[2], values[3], check);
  fflush(stdout);

  for (i = 0; i < SIZE; i++) {
    free(matrix[i]);
  }
  free(matrix);

  return iret;
}
