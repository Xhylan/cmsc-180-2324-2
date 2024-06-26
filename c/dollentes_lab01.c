/*
 * dollentes_lab01.c
 * Author: Michael Anthony B. Dollentes
 * Written for CMSC 180 T-6L.
 */

#include "dollentes_lab01.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

double *pearson_cor(int **matrix, int *vector, int n);

int main(int argc, char *argv[]) {
  int **matrix = NULL, *vector = NULL, n = 0;
  double *rho_vector = NULL, time_elapsed;
  clock_t start, end;

  FILE *fptr;

  if (argc < 2) {
    printf("Please enter n (n > 0): ");
    while (n < 1) {
      scanf("%d", &n);
      getchar();
    }
  }

  if (argc == 2) {
    int n = atoi(argv[1]);
  }

  check_if_file_exists(&fptr, "log/results.txt");
  fprintf(fptr, "%d: ", n);
  fclose(fptr);

  srand(time(NULL));

  printf("\nInitializing matrix...\n");
  start = clock();
  matrix = init_matrix(n, 1, 10);
  end = clock();
  time_elapsed = ((double)end - start) / CLOCKS_PER_SEC;
  if (matrix == NULL) {
    return EXIT_FAILURE;
  }

  printf("\nInitialization done. (Completed in %.4f seconds)\n", time_elapsed);

  printf("\nInitializing vector...\n");
  start = clock();
  vector = init_vector(n, 1, 10);
  end = clock();
  time_elapsed = ((double)end - start) / CLOCKS_PER_SEC;
  if (vector == NULL) {
    return EXIT_FAILURE;
  }
  printf("\nInitialization done. (Completed in %.4f seconds)\n", time_elapsed);

  for (int i = 0; i < 3; i++) {
    printf("\n\n[RUN #%d]\n", (i + 1));

    printf("\nRunning function pearson_cor()...\n");
    start = clock();
    rho_vector = pearson_cor(matrix, vector, n);
    end = clock();
    time_elapsed = ((double)end - start) / CLOCKS_PER_SEC;
    if (rho_vector == NULL) {
      printf("Error!\n");
      return EXIT_FAILURE;
    }
    printf("\nElapsed time: %.4f seconds\n", time_elapsed);

    check_if_file_exists(&fptr, "log/results.txt");
    fprintf(fptr, "%.4f, ", time_elapsed);
    fclose(fptr);

    free(rho_vector);
  }

  for (int i = 0; i < n; i++)
    free(matrix[i]);
  free(matrix);
  free(vector);

  // print_results(rho_vector, n);
  check_if_file_exists(&fptr, "log/results.txt");
  fprintf(fptr, "\n");
  fclose(fptr);
  return EXIT_SUCCESS;
}

double *pearson_cor(int **matrix, int *vector, int n) {
  double *rho_vector = NULL;
  int x, x_sq, y, y_sq, xy;
  double numerator, denominator;

  rho_vector = (double *)malloc(n * sizeof(double));
  if (vector == NULL)
    return NULL;

  y = y_sq = 0;

  for (int i = 0; i < n; i++) {
    y += vector[i];
    y_sq += vector[i] * vector[i];
  }

  for (int i = 0; i < n; i++) {
    x = x_sq = xy = 0;

    for (int j = 0; j < n; j++) {
      x += matrix[j][i];
      x_sq += matrix[j][i] * matrix[j][i];
      xy += matrix[j][i] * vector[j];
    }

    numerator = (double)((n * xy) - (y * x));
    denominator = sqrt(((n * x_sq) - (x * x)) * ((n * y_sq) - (y * y)));

    rho_vector[i] = (denominator == 0) ? NAN : numerator / denominator;
    //track_progress(i + 1, n);
  }

  return rho_vector;
}
