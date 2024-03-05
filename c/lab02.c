/***
 * lab02.c
 * Written by: Michael Anthony B. Dollentes
 * Date:
 ***/

#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SIZE 25000

int **MATRIX = NULL, *VECTOR = NULL, t;
double *RHO_VECTOR = NULL;
pthread_mutex_t RHO_VECTOR_MUTEX;

/* collection of arguments to be passed to threads upon thread creation */
typedef struct ARG_OBJECT {
  int thread_index;
  int submatrix_size;
} arguments;

double *initialize_rho_vector();
int **initialize_matrix(int min, int max);
int *initialize_vector(int min, int max);
int initialize_mutex();
int destroy_mutex();
void check_progress(int current, int total);

/* thread function */
void *pearson_cor(void *args) {
  // TODO: Threaded implementation of pearson_cor()
  int summ_x, summ_x_sq, summ_y, summ_y_sq, summ_xy;
  double numerator, denominator;
  arguments *arg = (arguments *)args;

  int remainder = SIZE % t;
  int start_index = arg->thread_index * arg->submatrix_size;
  int end_index = (arg->thread_index == t - 1)
                      ? (start_index + arg->submatrix_size + remainder) - 1
                      : (start_index + arg->submatrix_size) - 1;

  summ_y = summ_y_sq = 0;
  for (int i = 0; i < SIZE; i++) {
    summ_y += VECTOR[i];
    summ_y_sq += (VECTOR[i] * VECTOR[i]);
  }

  for (int i = start_index; i < end_index; i++) {
    summ_x = summ_x_sq = summ_xy = 0;

    for (int j = 0; j < SIZE; j++) {
      summ_x += MATRIX[j][i];
      summ_x_sq += (MATRIX[j][i] * MATRIX[j][i]);
      summ_xy += (MATRIX[j][i] * VECTOR[j]);
    }

    numerator = (double)(SIZE * summ_xy) - (summ_y * summ_x);
    denominator = sqrt(((SIZE * summ_x_sq) - (summ_x * summ_x)) *
                       ((SIZE * summ_y_sq) - (summ_y * summ_y)));
    pthread_mutex_lock(&RHO_VECTOR_MUTEX);
    RHO_VECTOR[i] = (denominator == 0) ? NAN : numerator / denominator;
    pthread_mutex_unlock(&RHO_VECTOR_MUTEX);
  }

  return NULL;
}

int main(int argc, char *argv[]) {
  int *thread_ids = NULL;
  clock_t start, end;
  double elapsed;

  pthread_t *threads = NULL;

  if (argc == 2)
    t = atoi(argv[1]);

  if (argc < 2) {
    printf("Enter t: ");
    scanf("%d", &t);
    getchar();
  }

  srand(time(NULL));

  for (int i = 1; i <= 3; i++) {
    printf("\n[[RUN %d]]\n", i);

    printf("Initializing matrix...\n");
    start = clock();
    MATRIX = initialize_matrix(1, 10);
    end = clock();
    elapsed = ((double)end - start) / CLOCKS_PER_SEC;

    printf("Initializing vector...\n");
    start = clock();
    VECTOR = initialize_vector(1, 10);
    end = clock();
    elapsed = ((double)end - start) / CLOCKS_PER_SEC;

    printf("Initializing results vector...\n");
    RHO_VECTOR = initialize_rho_vector();
    printf("Results vector has been created and initialized.\n");

    threads = (pthread_t *)malloc(t * sizeof(pthread_t));
    thread_ids = (int *)malloc(t * sizeof(int));

    start = clock();
    // TODO: Threaded implementation of pearson_cor
    if (initialize_mutex()) {
      printf("ERROR: Mutex failed to initialize.\n");
      return EXIT_FAILURE;
    }

    for (int i = 0; i < t; i++) {
      thread_ids[i] = i;
      pthread_create(&threads[i], NULL, pearson_cor, (void *)&thread_ids[i]);
    }

    for (int i = 0; i < t; i++) {
      pthread_join(threads[i], NULL);
    }
    end = clock();
    elapsed = ((double)end - start) / CLOCKS_PER_SEC;
    printf("Elapsed time: %.4f\n", elapsed);

    for (int j = 0; j < SIZE; j++)
      free(MATRIX[j]);
    free(MATRIX);
    free(VECTOR);
    free(RHO_VECTOR);

    free(threads);
    free(thread_ids);
    if (destroy_mutex()) {
      printf("ERROR: Failed to destroy mutex");
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}

int **initialize_matrix(int min, int max) {
  int **matrix = (int **)malloc(SIZE * sizeof(int *));

  for (int i = 0; i < SIZE; i++)
    matrix[i] = (int *)malloc(SIZE * sizeof(int));

  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      matrix[i][j] = rand() % (min - max) + min;
      check_progress((i * SIZE) + j + 1, SIZE * SIZE);
    }
  }

  return matrix;
}

int *initialize_vector(int min, int max) {
  int *vector = (int *)malloc(SIZE * sizeof(int));

  for (int i = 0; i < SIZE; i++) {
    vector[i] = (rand() % (min - max)) + min;
    check_progress(i + 1, SIZE);
  }

  return vector;
}

double *initialize_rho_vector() {
  double *vector = (double *)malloc(SIZE * sizeof(double));

  for (int i = 0; i < SIZE; i++) {
    vector[i] = 0;
    check_progress(i + 1, SIZE);
  }

  return vector;
}

void check_progress(int current, int total) {
  float progress = ((float)current / total) * 100;

  printf("\rProgress: %.2f%% - (%d out of %d)...", progress, current, total);
  fflush(stdout);
}

int initialize_mutex() {
  int result = pthread_mutex_init(&RHO_VECTOR_MUTEX, NULL);
  if (result != 0)
    return 1;
  return 0;
}

int destroy_mutex() {
  int result = pthread_mutex_destroy(&RHO_VECTOR_MUTEX);
  if (result != 0)
    return 1;
  return 0;
}
