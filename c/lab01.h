void track_progress(int current, int total){
  double progress = (double) current / total * 100; 
  printf("\rProgress: %.2f%% - (%d out of %d)", progress, current, total);
  fflush(stdout);
}

int ** init_matrix(int n, int min, int max){
    int ** matrix = NULL;

    matrix = (int **) malloc(sizeof(int *) * n);

    for (int i = 0; i < n; i++)
        matrix[i] = (int *) malloc(sizeof(int) * n);

    for (int i = 0; i < n; i++){
      for (int j = 0; j < n; j++){
        matrix[i][j] = (rand() % (max - min)) + min;
        track_progress(((i * n) + j + 1), n * n);
      }
    }

    return matrix;
}

int * init_vector(int n, int min, int max){
    int * vector = (int *) malloc(n * sizeof(int));

    for(int i = 0; i < n; i++){
      vector[i] = (rand() % (max - min)) + min;
      track_progress(i+1, n);
    }

    return vector;
}

int * get_vector_from_column(int ** matrix, int n, int col){
  int * vector = (int *) malloc(n * sizeof(int));

  for (int i = 0; i < n; i++)
    vector[i] = matrix[i][col];

  return vector;
}

