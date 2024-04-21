int ** initialize_matrix(int size){
    int ** matrix = (int **) malloc(sizeof(int *) * size);

    for(int i = 0; i < size; i++){
        matrix[i] = (int *) malloc(sizeof(int) * size);
    }

    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            matrix[i][j] = rand() % (3 - 1) + 1;
        }
    }

    return matrix;
}

int * initialize_vector(int size){
    int * vector = (int *) malloc(sizeof(int) * size);

    for(int i = 0; i < size; i++)
        vector[i] = rand() % (3 - 1) + 1;

    return vector;
}

double * initialize_rho_vector(int size){
    double * rho_vector = (double *) malloc(sizeof(double) * size);

    return rho_vector;
}

void check_if_file_exists(FILE** fd, char* filename){
    if (access(filename, F_OK) == 0)
        *fd = fopen(filename, "r");
}