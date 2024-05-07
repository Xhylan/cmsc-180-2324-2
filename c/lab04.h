#define TRUE 10
#define FALSE 11

typedef struct ADDR_COLL{
    char * IP_addr;
    int port;
    struct ADDR_COLL * next;
}net_address;

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

int ** init_empty_matrix(int rows, int cols){
    int ** matrix = (int **) malloc(sizeof(int *) * rows);

    for(int i = 0; i < rows; i++){
        matrix[i] = (int *) malloc(sizeof(int) * cols);
    }

    return matrix;
}

int *** divide_into_submatrices(int ** matrix, int size, int div){
    int cols = size/div;
    int *** submatrices = (int ***) malloc(sizeof(int **) * div);

    if (submatrices == NULL){
        perror("malloc()");
        free(submatrices);
        return NULL;
    }

    for (int i = 0; i < div; i++){
        submatrices[i] = init_empty_matrix(size, cols);
    }

    for (int i = 0; i < div; i++){
        for(int j = 0; j < size; j++){
            for(int k = 0; k < cols; k++){
                submatrices[i][j][k] = matrix[j][(i * cols) + k];
            }
        }
    }

    return submatrices;
}

int is_address_found(char* IP_addr, int port, net_address *head){
    net_address *temp = head;

    while(temp != NULL){
        if(strcmp(IP_addr, head -> IP_addr) == 0){
            if(temp -> port == port) return TRUE;
        } 

        temp = temp -> next;
    }
    
    return FALSE;
}

int printMenu(){
    int choice;

    printf("Choose how to send:\n");
    printf("[1] Send normally.\n");
    printf("[2] Send with threads.\n");
    printf("[3] Send with core-affine threads.\n");
    printf("[0] Exit.\n");
    scanf("%d", &choice);
    getchar();

    return choice;
}

double * initialize_rho_vector(int size){
    double * rho_vector = (double *) malloc(sizeof(double) * size);

    return rho_vector;
}

void check_if_file_exists(FILE** fd, char* filename){
    if (access(filename, F_OK) == 0)
        *fd = fopen(filename, "r");
}