#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "lab01.h"

double * pearson_cor(int ** matrix, int * vector, int n);

int main(int argc, char *argv[]){
  int ** matrix = NULL, * vector = NULL, n = 0;
  double * rho_vector = NULL;
  //clock_t start, end;

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

  rho_vector = pearson_cor(matrix, vector, n);
  if (rho_vector == NULL){
    printf("Error!\n");
  }else{
    for (int i = 0; i < n; i++) {
      printf("%.2f ", rho_vector[i]);
    }
  }

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
    int * vector_col_i = get_vector_from_column(matrix, n, i); //get column as vector
    
    y = get_vector_sum(vector, n);
    x = get_vector_sum(vector_col_i, n);
    x_sq = get_vector_sum_sq(vector_col_i, n);
    y_sq = get_vector_sum_sq(vector, n);
    xy = get_sum_of_products(vector_col_i, vector, n);
    numerator = (double) (n * xy) - (y*x);
    denominator = sqrt(((n*x_sq) - (x*x)) * ((n*y_sq) * (y*y)));
    rho_vector[i] = numerator / denominator;

    free(vector_col_i);
  }

  return rho_vector;
}
