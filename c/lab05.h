typedef struct ADDR_COLL{
    char * IP_addr;
    int port;
}net_address;

typedef struct ARG_STRUCT{
    int socket;
    int n;
    int expected_client;
    int **submatrix;
}arg;


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

    for(int i = 0; i < size; i++){
        vector[i] = rand() % (3 - 1) + 1;
    }
    
    return vector;
}

/*
 * FILE CHECKING UTILITY
 */

/*
 * for reading files
 */
void check_if_file_exists(FILE** fd, char* filename){
    if (access(filename, F_OK) == 0)
        *fd = fopen(filename, "r");
}

/*
 * for writing into files
 */
void check_if_file_exists_w(FILE **file, char *filename) {
  if (access(filename, F_OK) == 0)
    *file = fopen(filename, "a");
  if (access(filename, F_OK) != 0)
    *file = fopen(filename, "w");
}