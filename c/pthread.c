#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef struct ARG_COLL {
  int index;
  int submatrix_size;
} arguments;

int size, threads;
int **matrix = NULL, *vector = NULL;
double *rho_vector;
pthread_t *threads_array = NULL;
pthread_mutex_t rho_vector_mutex;

int randomize(int min, int max);
int **initialize_matrix(int size, int min, int max);
int *initialize_vector(int size, int min, int max);

int initialize_mutex() {
  int result = pthread_mutex_init(&rho_vector_mutex, NULL);

  if (result != 0)
    return 1;
  return 0;
}

int destroy_mutex() {
  int result = pthread_mutex_destroy(&rho_vector_mutex);

  if (result != 0)
    return 1;
  return 0;
}

void *pearson_cor(void *args) {
  arguments *arg = (arguments *)args;
  int summ_x, summ_x_sq, summ_y, summ_y_sq, summ_xy;
  double numerator, denominator;

  printf("%d\n", arg->index);
  int remainder = size % threads;
  int start = arg->index * arg->submatrix_size;
  int end = (arg->index == threads - 1)
                ? (start + arg->submatrix_size + remainder) - 1
                : (start + arg->submatrix_size) - 1;

  summ_y = summ_y_sq = 0;
  for (int i = 0; i < size; i++) {
    summ_y += vector[i];
    summ_y_sq += vector[i] * vector[i];
  }

  for (int i = start; i <= end; i++) {
    summ_x = summ_x_sq = summ_xy = 0;

    for (int j = 0; j < size; j++) {
      summ_x += matrix[j][i];
      summ_x_sq += matrix[j][i] * matrix[j][i];
      summ_xy += matrix[j][i] * vector[j];
    }

    numerator = (double)(size * summ_xy) - (summ_y * summ_x);
    int denominator_sq = ((size * summ_x_sq) - (summ_x * summ_x)) *
                         ((size * summ_y_sq) - (summ_y * summ_y));
    denominator = sqrt((double)denominator_sq);
    pthread_mutex_lock(&rho_vector_mutex);
    rho_vector[i] = (denominator == 0) ? NAN : numerator / denominator;
    pthread_mutex_unlock(&rho_vector_mutex);
  }

  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  int *thread_indices;

  printf("Enter n: ");
  scanf("%d", &size);
  getchar();

  printf("Enter t: ");
  scanf("%d", &threads);
  getchar();

  if (initialize_mutex() != 0) {
    printf("Error!\n");
    return EXIT_FAILURE;
  }

  srand(time(NULL));

  matrix = initialize_matrix(size, 1, 10);
  vector = initialize_vector(size, 1, 10);
  rho_vector = (double *)malloc(size * sizeof(double));

  threads_array = (pthread_t *)malloc(size * sizeof(pthread_t));
  thread_indices = (int *)malloc(size * sizeof(int));

  for (int i = 0; i < threads; i++) {
    thread_indices[i] = i;

    arguments *args = (arguments *)malloc(sizeof(arguments));
    args->index = thread_indices[i];
    args->submatrix_size = size / threads;

    pthread_create(&threads_array[i], NULL, pearson_cor, (void *)args);
  }

  for (int i = 0; i < threads; i++) {
    pthread_join(threads_array[i], NULL);
  }

  printf("Resulting vector: \n");
  for (int i = 0; i < size; i++)
    printf("%.2f\t", rho_vector[i]);
  printf("\n");

  for (int i = 0; i < size; i++)
    free(matrix[i]);
  free(matrix);
  free(vector);
  free(rho_vector);
  free(threads_array);
  free(thread_indices);

  if (destroy_mutex() != 0)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

int randomize(int min, int max) { return (rand() % (max - min)) + min; }

int **initialize_matrix(int size, int min, int max) {
  int **matrix = (int **)malloc(size * sizeof(int *));

  for (int i = 0; i < size; i++)
    matrix[i] = (int *)malloc(size * sizeof(int));

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++)
      matrix[i][j] = randomize(min, max);
  }

  return matrix;
}

int *initialize_vector(int size, int min, int max) {
  int *vector = (int *)malloc(size * sizeof(int));

  for (int i = 0; i < size; i++)
    vector[i] = randomize(min, max);

  return vector;
}
