#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 8080
#define MASTER 0
#define SLAVE 1

int main(){
    int status, size;
    int matrix[2][2] = {{1,2},{3,4}};
    int server_socket, client_socket;
    int recv_val, send_val;
    int addrlen = sizeof(server_socket);
    struct sockaddr_in server_address, client_address;

    printf("Status: (Master - 0; Slave - 1) ");
    scanf("%d", &status);

    if(status == MASTER){
        server_socket = socket(AF_INET, SOCK_STREAM, 0);

        if (server_socket == -1) {
            perror("Socket creation failed");
            return EXIT_FAILURE; 
        }

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

        while(1){
            client_socket = accept(server_socket, (struct sockaddr*) &client_socket, (socklen_t*)&addrlen);
            if(client_socket == -1){
                perror("Connection failed!");
                continue;
            }
        }
    }

}