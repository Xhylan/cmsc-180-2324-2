#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "lab01.h"

int * pearson_cor(int ** matrix, int * vector, int n);

int main(int argc, char *argv[]){
  int ** matrix = NULL, * vector = NULL, * rho_vector = NULL;
  //clock_t start, end;

  if (argc < 2) {
    printf("Usage: ./lab01 [n]");
    return EXIT_FAILURE;
  }
  
  int n = (int) *argv[1];
  srand(time(NULL));
 
  matrix = init_matrix(n, 1, 10);
  if(matrix == NULL){
    return EXIT_FAILURE;
  }
  
  vector = init_vector(n, 1, 10);
  if(vector == NULL){
    return EXIT_FAILURE;
  }

  /*
  if (pearson_cor(matrix, vector, n) == NULL){
    printf("Error!\n");
  }*/

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

int * pearson_cor(int ** matrix, int * vector, int n){
  int * rho_vector = NULL;
  int x, x_sq, y, y_sq, xy; 

  rho_vector = init_empty_vector(n);
  if(vector == NULL)
      return NULL;

  for (int i = 0; i < n; i++){
    int * vector_col_i = get_vector_from_column(matrix, n, i); //get column as vector
    
    y = get_vector_sum(vector, n);
    x = get_vector_sum(vector_col_i, n);
    x_sq = get_vector_sum_sq(vector_col_i, n);
    y_sq = get_vector_sum_sq(vector, n);
    xy = get_sum_of_products(vector_col_i, vector, n);

    /* TODO: Write the code snippet that finds the Pearson Correlation
    * Coefficient of two vectors. 
    * Guide: rho_numerator = (n * summ(Xcoly * vecy) - (summX * summY)) 
    * rho_denominator = sqrt((n * sum(X^2) - summX^2) * (n * sum(Y^2) - summY^2)) 
    * rho = rho_numerator / rho_denominator
    */

    free(vector_col_i);
  }

  return rho_vector;
}
