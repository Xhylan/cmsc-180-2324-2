#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "lab04.h"

#define MASTER 0
#define SLAVE 1

typedef struct ADDR_COLL{
    char * IP_addr;
    int port;
    struct ADDR_COLL * next;
}net_address;

int main(int argc, char *argv[]){
    int n, p, s, sockfd;
    int ** matrix, * vector;
    double * rho_vector;
    struct sockaddr_in server, client;
    socklen_t client_addr_size;
    FILE * fd;

    printf("N: ");
    scanf("%d", &n);
    getchar();

    printf("Port: ");
    scanf("%d", &p);
    getchar();

    printf("Status (0 - Master, 1 - Slave): ");
    scanf("%d", &s);
    getchar();

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("socket()");
        return EXIT_FAILURE;
    }

    if (s == MASTER) {
        int clients_connected = 0;
        net_address * head = NULL;

        matrix = initialize_matrix(n);
        vector = initialize_vector(n);
        rho_vector = initialize_rho_vector(n);

        check_if_file_exists(&fd, "clients.cfg");

        if(fd == NULL){
            printf("Can't find client configuration file!\n");
            return EXIT_FAILURE;
        }

        char netAddressString[256];
        char *token;

        while (fgets(netAddressString, 256, fd)){
            net_address * newAddress = (net_address *) malloc(sizeof(net_address));

            token = strtok(netAddressString, ":");
            newAddress -> IP_addr = token;
            token = strtok(NULL, ":");
            newAddress -> port = atoi(token);
            token = strtok(NULL, ":");
            
            if (head == NULL) head = newAddress;
            if (head != NULL) {
                net_address * temp = NULL;

                while (temp -> next != NULL) temp = temp -> next;

                temp -> next = newAddress;
            }
        }


        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(p); 

        if (bind(sockfd, (struct sockaddr*)&server, sizeof(server)) == -1) {
            perror("bind");
            return 1;
        }

        if (listen(sockfd, 5) == -1) {
            perror("listen");
            return 1;
        }

        printf("Server listening on port %d\n", ntohs(server.sin_port));
    
        while (1) {
            int new_socket = accept(sockfd, (struct sockaddr*) &client, &client_addr_size);
            struct sockaddr_in client;
            socklen_t client_addr_size = sizeof(client);

            if (new_socket < 0){
                perror("accept()");
                continue;
            }

            getpeername(new_socket, (struct sockaddr *) &client, &client_addr_size);

            char client_IP[INET_ADDRSTRLEN]; 
            inet_ntop(AF_INET, &(client.sin_addr), client_IP, client_addr_size);

            int client_port = ntohs(client.sin_port);

            printf("Client connected from: %s:%d\n", client_IP, client_port);
            clients_connected++;
        }

    }

    if (s == SLAVE){
        check_if_file_exists(&fd, "server.cfg");
        struct sockaddr_in client_addr;

        client_addr.sin_family = AF_INET;
        client_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
        client_addr.sin_port = htons(p);

         if (bind(sockfd, (struct sockaddr*)&client_addr, sizeof(client_addr)) == -1) {
            perror("bind");
        } else {
            printf("Client socket bound to port %d\n", p);
        }

        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr("127.0.0.1");
        server.sin_port = htons(8000); 

        if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) == -1) {
            perror("connect()");
            return 1;
        }

        printf("Connected");
    }

    close(sockfd);
    return EXIT_SUCCESS;
}
