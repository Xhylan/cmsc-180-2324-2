/***
 * lab02.c
 * Written by: Michael Anthony B. Dollentes
 * Date:
 ***/

#include <ctype.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define TRUE 0xFF
#define FALSE 0x00

int SIZE, IS_MATRIX_TRANSPOSED;
int **MATRIX = NULL, *VECTOR = NULL, t;
double *RHO_VECTOR = NULL;
pthread_mutex_t RHO_VECTOR_MUTEX;
FILE *file;

/* collection of arguments to be passed to threads upon thread creation */
typedef struct ARG_OBJECT {
  int thread_index;
  int submatrix_size;
} arguments;

/* utility functions */
double *initialize_rho_vector();
int **initialize_matrix(int min, int max);
int *initialize_vector(int min, int max);
int initialize_mutex();
int destroy_mutex();
void check_progress(int current, int total);
void check_if_file_exists(FILE **file, char *filename);

/* thread function */
void *pearson_cor(void *args) {
  int summ_x, summ_x_sq, summ_y, summ_y_sq, summ_xy;
  double numerator, denominator;
  arguments *arg = (arguments *)args;

  // compute for the start index, end index and the remainder
  int remainder = SIZE % t;
  int start_index = arg->thread_index * arg->submatrix_size;
  int end_index = (arg->thread_index == t - 1)
                      ? (start_index + arg->submatrix_size + remainder) - 1
                      : (start_index + arg->submatrix_size) - 1;

  // preemptively compute summation of vector values
  summ_y = summ_y_sq = 0;
  for (int i = 0; i < SIZE; i++) {
    summ_y += VECTOR[i];
    summ_y_sq += (VECTOR[i] * VECTOR[i]);
  }

  for (int i = start_index; i < end_index; i++) {
    summ_x = summ_x_sq = summ_xy = 0;

    if(IS_MATRIX_TRANSPOSED){
      for (int j = 0; j < SIZE; j++) {
        summ_x += MATRIX[i][j];
        summ_x_sq += (MATRIX[i][j] * MATRIX[i][j]);
        summ_xy += (MATRIX[i][j] * VECTOR[j]);
      }
    }

    if(IS_MATRIX_TRANSPOSED == FALSE){
      for (int j = 0; j < SIZE; j++) {
        summ_x += MATRIX[j][i];
        summ_x_sq += (MATRIX[j][i] * MATRIX[j][i]);
        summ_xy += (MATRIX[j][i] * VECTOR[j]);
      } 
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

/***
* MAIN FUNCTION
***/
int main(int argc, char *argv[]) {
  int *thread_ids = NULL;
  struct timespec start, finish;
  double elapsed;

  pthread_t *threads = NULL;

  /* handle user input depending on usage */
  if (argc > 1) {
    SIZE = atoi(argv[1]);
    t = atoi(argv[2]);
  }

  if (argc == 1) {
    printf("Enter n: ");
    scanf("%d", &SIZE);
    getchar();

    printf("Enter t: ");
    scanf("%d", &t);
    getchar();
  }

  /* prompt user into transposing the matrix */
  printf("Transpose matrix? (y/n) ");
  char res = getchar();

  if (tolower(res) == 'y')
    IS_MATRIX_TRANSPOSED = TRUE;
  if (tolower(res) == 'n')
    IS_MATRIX_TRANSPOSED = FALSE;

  srand(time(NULL));

  check_if_file_exists(&file, "log/results_threaded.txt");
  fprintf(file, "SIZE: %d, THREADS: %d\n", SIZE, t);
  fclose(file);


  printf("Initializing matrix...\n");
  clock_gettime(CLOCK_MONOTONIC, &start);
  MATRIX = initialize_matrix(1, 10);
  clock_gettime(CLOCK_MONOTONIC, &finish);
  elapsed = (finish.tv_sec - start.tv_sec);
  elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
  printf("\nMatrix initialized. (%.6f seconds)\n", elapsed);

  printf("Initializing vector...\n");
  clock_gettime(CLOCK_MONOTONIC, &start);
  VECTOR = initialize_vector(1, 10);
  clock_gettime(CLOCK_MONOTONIC, &finish);
  elapsed = (finish.tv_sec - start.tv_sec);
  elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
  printf("\nVector initialized. (%.6f seconds)\n", elapsed);

  while (t > 0){
  /*
   * Display progress of the program. 
   * Shows the time taken for each runtime of the program.
   */
  fprintf(file, "SIZE: %d, THREADS: %d\n", SIZE, t);

  for (int i = 1; i <= 3; i++) {
    printf("\n[[RUN %d]]\n", i);

    printf("Initializing results vector...\n");
    RHO_VECTOR = initialize_rho_vector();
    printf("\nResults vector has been created and initialized.\n");

    /* create thread array and thread_id array */
    threads = (pthread_t *)malloc(t * sizeof(pthread_t));
    thread_ids = (int *)malloc(t * sizeof(int));

    if (initialize_mutex()) {
      printf("ERROR: Mutex failed to initialize.\n");
      return EXIT_FAILURE;
    }

    printf("Starting the computation...\n");
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < t; i++) {
      thread_ids[i] = i;
      arguments *arg = (arguments *)malloc(sizeof(arguments));

      arg->thread_index = thread_ids[i];
      arg->submatrix_size = SIZE / t;
      
      pthread_create(&threads[i], NULL, pearson_cor, (void *)arg); 
    }

    for (int i = 0; i < t; i++) {
      pthread_join(threads[i], NULL);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("Computation finished!\n");
    printf("Elapsed time: %.4f seconds\n", elapsed);

    check_if_file_exists(&file, "log/results_threaded.txt");

    if (i < 3)
      fprintf(file, "%.6f, ", elapsed);

    if (i == 3)
      fprintf(file, "%.6f\n", elapsed);

    fclose(file);

    free(RHO_VECTOR);
    free(threads);
    free(thread_ids);

    if (destroy_mutex()) {
      printf("ERROR: Failed to destroy mutex");
      return EXIT_FAILURE;
    }
    }

    printf("Enter t: ");
    scanf("%d", &t);
    getchar();
  }

  for (int j = 0; j < SIZE; j++)
    free(MATRIX[j]);
  free(MATRIX);
  free(VECTOR);
  return EXIT_SUCCESS;
}

/*
* UTILITY FUNCTIONS
*/

int **initialize_matrix(int min, int max) {
  int **matrix = (int **)malloc(SIZE * sizeof(int *));

  for (int i = 0; i < SIZE; i++)
    matrix[i] = (int *)malloc(SIZE * sizeof(int));

  if (IS_MATRIX_TRANSPOSED){
    for (int i = 0; i < SIZE; i++) {
     for (int j = 0; j < SIZE; j++) {
        matrix[j][i] = rand() % (min - max) + min;
        check_progress((i * SIZE) + j + 1, SIZE * SIZE);
      }
    }  
  }
  
  if (IS_MATRIX_TRANSPOSED == FALSE) {
    for (int i = 0; i < SIZE; i++) {
      for (int j = 0; j < SIZE; j++) {
        matrix[i][j] = rand() % (min - max) + min;
        check_progress((i * SIZE) + j + 1, SIZE * SIZE);
      }
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

void check_if_file_exists(FILE **file, char *filename) {
  if (access(filename, F_OK) == 0)
    *file = fopen(filename, "a");
  if (access(filename, F_OK) != 0)
    *file = fopen(filename, "w");
}
