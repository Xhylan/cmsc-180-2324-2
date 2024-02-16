#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "lab01.h"

double * pearson_cor(int ** matrix, int * vector, int n);

int main(int argc, char *argv[]){
  int ** matrix = NULL, * vector = NULL, n = 0;
  double * rho_vector = NULL, time_elapsed;
  clock_t start, end;

  if (argc < 2) {
    printf("Please enter n (n > 0): ");
    while(n < 1){
      scanf("%d", &n);
      getchar();
    }
  }
 
  if (argc == 2) {
    int n = (int) *argv[1];
  }

  srand(time(NULL));
 
  matrix = init_matrix(n, 1, 10);
  if(matrix == NULL){
    return EXIT_FAILURE;
  }
  
  vector = init_vector(n, 1, 10);
  if(vector == NULL){
    return EXIT_FAILURE;
  }

  sleep(1);

  start = clock(); 
  rho_vector = pearson_cor(matrix, vector, n);
  end = clock();
  time_elapsed = ((double) end - start) / CLOCKS_PER_SEC;
  
  if (rho_vector == NULL){
    printf("Error!\n");
  }

  printf("Elapsed time: %f\n", time_elapsed);
  //free matrix
  for (int i = 0; i < n; i++)
    free(matrix[i]);
  free(matrix);

  //free vector 
  free(vector);

  //free rho_vector
  free(rho_vector);
  return EXIT_SUCCESS;
}

double * pearson_cor(int ** matrix, int * vector, int n){
  double * rho_vector = NULL;
  int x, x_sq, y, y_sq, xy;
  double numerator, denominator;
  
  rho_vector = (double *) malloc(n * sizeof(double));
  if(vector == NULL)
      return NULL;

  for (int i = 0; i < n; i++){
    x = x_sq = y = y_sq = xy = 0;
    int * vector_col_i = get_vector_from_column(matrix, n, i); //get column as vector
    
    for (int j = 0; j < n; j++) {
      x += vector_col_i[j];
      y += vector[i];
      x_sq += vector_col_i[j] * vector_col_i[j];
      y_sq += vector[j] * vector[j];
      xy += vector_col_i[j] * vector[j];
    }

    numerator = (double) (n * xy) - (y*x);
    denominator = sqrt(((n*x_sq) - (x*x)) * ((n*y_sq) * (y*y)));
    
    if (denominator == 0) rho_vector[i] = NAN;
    if (denominator != 0) rho_vector[i] = numerator / denominator;

    free(vector_col_i);
  }

  return rho_vector;
}
