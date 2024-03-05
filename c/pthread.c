#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

int ** matrix = NULL, * vector = NULL;
double * rho_vector;
pthread_t * threads_array = NULL;
pthread_mutex_t rho_vector_mutex;

int randomize(int min, int max);
int ** initialize_matrix(int size, int min, int max);
int * initialize_vector(int size, int min, int max);

int initialize_mutex(){
  int result = pthread_mutex_init(&rho_vector_mutex, NULL);

  if (result != 0) return 1;
  return 0;
}

int destroy_mutex(){
  int result = pthread_mutex_destroy(&rho_vector_mutex);

  if (result != 0) return 1;
  return 0;
}

void * pearson_cor(void * args) {
  int * thread_index = (int *) args;

  pthread_exit(NULL);
}

int main(int argc, char *argv[]){
  int size, threads;
  int * thread_indices;

  printf("Enter n: ");
  scanf("%d", &size);
  getchar();

  printf("Enter t: ");
  scanf("%d", &threads);
  getchar();

  if (initialize_mutex() != 0){
    printf("Error!\n");
    return EXIT_FAILURE;
  }

  matrix = initialize_matrix(size, 1, 10);
  vector = initialize_vector(size, 1, 10);
  rho_vector = (double *) malloc(size * sizeof(double));

  threads_array = (pthread_t *) malloc(size * sizeof(pthread_t));

  for (int i = 0; i < threads; i++){

  }

  for (int i = 0; i < size; i++)
    free(matrix[i]);
  free(matrix);
  return EXIT_SUCCESS;
}

int randomize(int min, int max){
  return (rand() % (max - min)) + min;
}

int ** initialize_matrix(int size, int min, int max){
  int ** matrix = (int **) malloc(size * sizeof(int *));

  for(int i = 0; i < size; i++)
    matrix[i] = (int *) malloc(size * sizeof(int));

  for(int i = 0; i < size; i++){
    for(int j = 0; j < size; j++)
      matrix[i][j] = randomize(min, max);
  }
  
  return matrix;
}

int * initialize_vector(int size, int min, int max){
  int * vector = (int *) malloc(size * sizeof(int));

  for(int i = 0; i < size; i++)
    vector[i] = randomize(min, max);

  return vector;
}
