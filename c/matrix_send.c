#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "lab04.h"

#define PORT 8080
#define MASTER 0
#define SLAVE 1

int main(){
    struct sockaddr_in server_address, client_address;
    int status, size;
    int ** matrix;
    int server_socket, client_socket;
    int recv_val, send_val;
    int addrlen = sizeof(server_address);

    printf("Status: (Master - 0; Slave - 1) ");
    scanf("%d", &status);

    if(status == MASTER){
        server_socket = socket(AF_INET, SOCK_STREAM, 0);

        if (server_socket == -1) {
            perror("Socket creation failed");
            return EXIT_FAILURE; 
        }

        scanf("%d", &size);
        getchar();

        srand(time(NULL));
        matrix = initialize_matrix(size);

        memset(&server_address, 0, sizeof(server_address));
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = INADDR_ANY; 
        server_address.sin_port = htons(PORT);

        if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
            perror("Bind failed");
            return EXIT_FAILURE;
        }

        if (listen(server_socket, 5) < 0) {
            perror("Listen failed");
            return EXIT_FAILURE;
        }

        printf("Size to be sent: %d\n", size);
        int size_to_send = htonl(size);

        client_socket = accept(server_socket, (struct sockaddr*) &client_socket, (socklen_t*)&addrlen);
        if(client_socket == -1){
            perror("Connection failed!");
            return EXIT_FAILURE;
        }
        printf("Client connected!\n");

        int sent_bytes = send(client_socket, &size_to_send, sizeof(size_to_send), 0);
        if(sent_bytes != sizeof(size_to_send)){
            perror("Failed! (size) ");
            close(client_socket);
            return EXIT_FAILURE;
        }

        for(int i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                int to_send = htonl(matrix[i][j]);
                sent_bytes = send(client_socket, &to_send, sizeof(to_send), 0);
                if(sent_bytes != sizeof(to_send)){
                    perror("Failed! (matrix) ");
                    close(client_socket);
                    return EXIT_FAILURE;
                }
            }
        }

        close(client_socket);
        close(server_socket);
    }

    if (status == SLAVE) {
        client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket == -1) {
            perror("Failed to create socket!");
            return EXIT_FAILURE;
        }

        memset(&server_address, 0, addrlen);
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
        server_address.sin_port = htons(PORT);

        if (connect(client_socket, (struct sockaddr*)&server_address, addrlen) == -1) {
            perror("Failed to connect");
            return EXIT_FAILURE;
        }

        printf("Connected successfully!");

        int received_bytes = recv(client_socket, &size, sizeof(size), 0);
        if (received_bytes !=  sizeof(size)){
            perror("Failed to receive");
            close(client_socket);
            return EXIT_FAILURE;
        }

        size = ntohl(size);
        printf("Matrix size: %d\n", size);

        int** matrix = (int**)malloc(size * sizeof(int*));
         for (int i = 0; i < size; i++) {
            matrix[i] = (int*)malloc(size * sizeof(int));
        }

        for (int i = 0; i < size; i++){
            for (int j = 0; j < size; j++) {
                int received;
                received_bytes = recv(client_socket, &received, sizeof(received), 0);
                if (received_bytes != sizeof(matrix[i][j])){
                    perror("Failed to receive matrix data!");
                    close(client_socket);
                    for (int k = 0; k < size; k++) free(matrix[k]);
                    free(matrix);
                    exit(EXIT_FAILURE);
                }
                matrix[i][j] = ntohl(received);
            }
        }

        // printf("Matrix Received: \n");
        // for (int i = 0; i < size; i++){
        //     for (int j = 0; j < size; j++){
        //         printf("%d ", matrix[i][j]);
        //     }
        //     printf("\n");
        // }

        for (int k = 0; k < size; k++) free(matrix[k]);
        free(matrix);

        close(client_socket);
    }
}
