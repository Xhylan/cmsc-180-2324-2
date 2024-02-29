/***
* lab02.c
* Written by: Michael Anthony B. Dollentes
* Date: 
***/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define SIZE 25000

int ** MATRIX = NULL, * VECTOR = NULL;
double ** RHO_VECTOR = NULL;

/* collection of arguments to be passed to threads upon thread creation */
typedef struct ARG_OBJECT{
  int start;
}args;

int ** initialize_matrix(int min, int max);
int * initialize_vector(int min, int max);
void check_progress(int current, int total);

/* thread function */
void * pearson_cor(void * args){
  //TODO: Threaded implementation of pearson_cor()
  int * thread_id = (int *) args;

  printf("%d\n", *thread_id);
  return NULL;
}

int main(int argc, char *argv[]){
  int t, *thread_ids = NULL;
  clock_t start, end;
  double elapsed;

  pthread_t *threads = NULL;

  if (argc == 2)
    t = atoi(argv[1]);

  if (argc < 2){
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
    elapsed = ((double) end - start) / CLOCKS_PER_SEC;

    printf("Initializing vector...\n");
    start = clock();
    VECTOR = initialize_vector(1, 10);
    end = clock();
    elapsed = ((double) end - start) / CLOCKS_PER_SEC;

    threads = (pthread_t *) malloc(t * sizeof(pthread_t));
    thread_ids = (int *) malloc(t * sizeof(int));

    //TODO: Threaded implementation of pearson_cor

    for (int i = 0; i < t; i++){
      thread_ids[i] = i;
      pthread_create(&threads[i], NULL, pearson_cor, (void *) &thread_ids[i]);
    }

    for (int i = 0; i < t; i++){
      pthread_join(threads[i], NULL);
    }

    for(int j = 0; j < SIZE; j++)
      free(MATRIX[j]);
    free(MATRIX);

    free(VECTOR);

    free(threads);
  }

  return EXIT_SUCCESS;
}

int ** initialize_matrix(int min, int max){
  int ** matrix = (int **) malloc(SIZE * sizeof(int *));

  for(int i = 0; i < SIZE; i++)
    matrix[i] = (int *) malloc(SIZE * sizeof(int));

  for(int i = 0; i < SIZE; i++){
    for(int j = 0; j < SIZE; j++){
      matrix[i][j] = rand() % (min - max) + min;
      check_progress((i * SIZE) + j + 1, SIZE*SIZE);
    }
  }

  return matrix;
}

int * initialize_vector(int min, int max){
  int * vector = (int *) malloc(SIZE * sizeof(int));

  for (int i = 0; i < SIZE; i++){
    vector[i] = (rand() % (min - max)) + min;
    check_progress(i+1, SIZE); 
  }

  return vector;
}

double * initialize_rho_vector(int min, int max){
  double * vector = (double *) malloc(SIZE * sizeof(double));

  for(int i = 0; i < SIZE; i++){
    vector[i] = 0;
    check_progress(i+1, SIZE);
  }

  return vector;
}

void check_progress(int current, int total){
  float progress = ((float) current / total) * 100;

  printf("\rProgress: %.2f%% - (%d out of %d)...", progress, current, total);
  fflush(stdout);
}
