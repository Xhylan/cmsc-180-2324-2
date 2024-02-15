int ** init_matrix(int n, int min, int max){
    int ** matrix = NULL;

    matrix = (int **) malloc(sizeof(int *) * n);

    for (int i = 0; i < n; i++)
        matrix[i] = (int *) malloc(sizeof(int) * n);

    for (int i = 0; i < n; i++){
      for (int j = 0; j < n; i++){
        matrix[i][j] = (rand() % (max - min)) + min;
      }
    }

    return matrix;
}

int * init_empty_vector(int n){
    return (int *) malloc(sizeof(int) * n);
}

int * init_vector(int n, int min, int max){
    int * vector = init_empty_vector(n);

    for(int i = 0; i < n; i++){
      vector[i] = (rand() % (max - min)) + min;
    }

    return vector;
}

int get_vector_sum(int *vector, int n){
  int sum = 0;

  for (int i = 0; i < n; i++)
    sum = sum + vector[i];

  return sum;
}

int get_vector_sum_sq(int *vector, int n){
  int sum = 0;

  for (int i = 0; i < n; i++)
    sum = sum + (vector[i] * vector[i]);

  return sum;
}

int get_sum_of_products(int *vectorA, int *vectorB, int n){
  int sum = 0;

  for (int i = 0; i < n; i++)
    sum = sum + (vectorA[i] * vectorB[i]);

  return sum;
}

int * get_vector_from_column(int ** matrix, int n, int col){
  int * vector = init_empty_vector(n);

  for (int i = 0; i < n; i++)
    vector[i] = matrix[i][col];

  return vector;
}
