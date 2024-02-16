int ** init_matrix(int n, int min, int max){
    int ** matrix = NULL;

    matrix = (int **) malloc(sizeof(int *) * n);

    for (int i = 0; i < n; i++)
        matrix[i] = (int *) malloc(sizeof(int) * n);

    for (int i = 0; i < n; i++){
      for (int j = 0; j < n; j++){
        matrix[i][j] = (rand() % (max - min)) + min;
      }
    }

    return matrix;
}

int * init_vector(int n, int min, int max){
    int * vector = (int *) malloc(n * sizeof(int));

    for(int i = 0; i < n; i++){
      vector[i] = (rand() % (max - min)) + min;
    }

    return vector;
}

int * get_vector_from_column(int ** matrix, int n, int col){
  int * vector = init_empty_vector(n);

  for (int i = 0; i < n; i++)
    vector[i] = matrix[i][col];

  return vector;
}
