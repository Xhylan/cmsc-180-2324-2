#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include "lab04.h"

int main(){
    int size = 0, divisions = 0;

    scanf("%d", &size);
    getchar();
    scanf("%d", &divisions);
    getchar();

    srand(time(NULL));
    int ** matrix = initialize_matrix(size);

     for(int j = 0; j < size; j++){
        for(int k = 0; k < size; k++){
            printf("%d ", matrix[j][k]);
        }
        printf("\n");
    }

    int *** submatrices = divide_into_submatrices(matrix, size, divisions);

    for(int i = 0; i < divisions; i++){
        printf("Submatrix %d: \n", i);
        for(int j = 0; j < size; j++){
            for(int k = 0; k < size/divisions; k++){
                printf("%d ", submatrices[i][j][k]);
            }
            printf("\n");
        }
        printf("\n\n");
    }
}