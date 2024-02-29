#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define SIZE 25000

int ** MATRIX = NULL, * VECTOR = NULL;

typedef struct ARG_OBJECT{
  int start;
}args;

int ** initialize_matrix(int min, int max);
int * initialize_vector(int min, int max);
void check_progress(int current, int total);

int * pearson_cor(){
  int * rho_vector = NULL;

  return rho_vector;
}

int main(int argc, char *argv[]){
  int t;
  clock_t start, end;
  double elapsed;

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


    for(int j = 0; j < SIZE; j++)
      free(matrix[j]);
    free(matrix);

    free(vector);
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
      check_progress(i + 1, SIZE*SIZE);
    }
  }

  return matrix;
}

int * initialize_vector(int min, int max){
  int * vector = (int *) malloc(SIZE * sizeof(int));

  for (int i = 0; i < SIZE; i++){
    vector[i] = rand() % (min - max) + min;
    check_progress(i+1, SIZE); 
  }

  return vector;
}

void check_progress(int current, int total){
  float progress = (float) current / total * 100;

  printf("\rProgress: %.2f%% - (%d out of %d)...");
  fflush(stdout);
}
