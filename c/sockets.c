#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MASTER 0
#define SLAVE 1
#define PORT 8080

int main() {
    int sockfd, new_socket, status, size;
    struct sockaddr_in server, client;
    int opt = 1;

    //Socket creation and error handling. 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("[-] Socket creation failed: ");
        exit(EXIT_FAILURE);
    }

    //Setting socket options.
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){
        perror("[-] Failed to set socket options: ");
        exit(EXIT_FAILURE);
    }

    printf("Status (0 for MASTER, 1 for SLAVE): ");
    scanf("%d", &status);
    getchar();

    if (status == MASTER) {
        socklen_t addrlen = sizeof(server);
        printf("[+] This program is the master program.\n");

        printf("Size: ");
        scanf("%d", &size);
        getchar();

        /*
         * Forcefully binding server socket to a specified address.
         */
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(8080);

        if(bind(sockfd, (struct sockaddr*) &server, sizeof(server)) < 0){
            perror("[-] Binding to address failed: ");
            exit(EXIT_FAILURE);
        }

        if(listen(sockfd, 5) < 0) {
            perror("[-] Cannot listen to specified port: ");
            exit(EXIT_FAILURE);
        }

        while (1) {
            new_socket = accept(sockfd, (struct sockaddr*) &server, &addrlen);
            if(new_socket < 0) {
                perror("[-] Failed to accept the connection: ");
                exit(EXIT_FAILURE);
            }

            int size_nw = htonl(size);
            int send_bytes = send(new_socket, &size_nw, sizeof(size_nw), 0);
            if(send_bytes != sizeof(size_nw)){
                perror("[-] Failed to send data: ");
                exit(EXIT_FAILURE);
            }

            for(int i = 0; i < size; i++){
                size_nw = htonl(i+1);
                send_bytes = send(new_socket, &size_nw, sizeof(size_nw), 0);
                if(send_bytes != sizeof(size_nw)){
                    perror("[-] Failed to send data: ");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    if (status == SLAVE) {
        int size_nw, recv_bytes, *array;

        printf("[+] This program is the slave program.\n");

        server.sin_family = AF_INET;
        server.sin_port = htons(PORT);
        
        if(inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) < 0) {
            perror("[-] Failed to bind address: ");
            exit(EXIT_FAILURE);
        }

        if(connect(sockfd, (struct sockaddr*) &server, sizeof(server)) < 0){
            perror("[-] Failed to connect to server: ");
            exit(EXIT_FAILURE); 
        }

        recv_bytes = recv(sockfd, &size_nw, sizeof(size_nw), 0);
        if(recv_bytes != sizeof(size_nw)){
            perror("[-] Failed to receive data: ");
            exit(EXIT_FAILURE);
        }
        size = ntohl(size_nw);

        printf("Received the following from the server: %d\n", size);

        array = (int*) malloc(sizeof(int) * size);
        for(int i = 0; i < size; i++){
            recv_bytes = recv(sockfd, &size_nw, sizeof(size_nw), 0);
            if(recv_bytes != sizeof(size_nw)){
                perror("[-] Failed to receive data: ");
                exit(EXIT_FAILURE);
            }
            array[i] = ntohl(size_nw);
        }

        printf("Received the following from the server: \n");

        for(int i = 0; i < size; i++){
            printf("%d ", array[i]);
        }
    }

    close(sockfd);

    return 0;
}