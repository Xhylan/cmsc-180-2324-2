#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include "lab04.h"

#define MASTER 0
#define SLAVE 1
#define TRUE 10
#define FALSE 11

typedef struct ADDR_COLL{
    char * IP_addr;
    int port;
    struct ADDR_COLL * next;
}net_address;

net_address * head = NULL;

int is_address_found(char* IP_addr, int port){
    net_address *temp = head;

    while(temp != NULL){
        if(strcmp(IP_addr, head -> IP_addr) == 0){
            if(temp -> port == port) return TRUE;
        } 

        temp = temp -> next;
    }
    
    return FALSE;
}

void send_over_socket();
void send_over_socket_threaded();
void send_over_socket_thread_affine();

int main(int argc, char *argv[]){
    int n, p, s, sockfd, opt = 1;
    int ** matrix, * vector, ***submatrices;
    double * rho_vector, elapsed;
    struct sockaddr_in server, client;
    struct timespec start, end;
    socklen_t client_addr_size;
    FILE * fd;

    printf("[o] N: ");
    scanf("%d", &n);
    getchar();

    printf("[o] Port: ");
    scanf("%d", &p);
    getchar();

    printf("[o] Status (0 - Master, 1 - Slave): ");
    scanf("%d", &s);
    getchar();

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("[-] Error creating socket ");
        return EXIT_FAILURE;
    }

    //Setting socket options.
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){
        perror("[-] Failed to set socket options: ");
        exit(EXIT_FAILURE);
    }

    if (s == MASTER) {
        int expected_clients = 0, clients_connected = 0, size;

        printf("[o] Expected connections: ");
        scanf("%d", &expected_clients);
        getchar();

        srand(time(NULL));
        matrix = initialize_matrix(n);
        vector = initialize_vector(n);
        submatrices = divide_into_submatrices(matrix, n, expected_clients);

        check_if_file_exists(&fd, "clients.cfg");

        if(fd == NULL){
            printf("[-] Can't find client configuration file!\n");
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
            newAddress -> next = NULL;
            
            if (head != NULL) {
                net_address * temp = head;
                while (temp -> next != NULL) temp = temp -> next;
                temp -> next = newAddress;
            }

            if (head == NULL) {
                head = newAddress;
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

        printf("[+] Server listening on port %d\n", ntohs(server.sin_port));
    
        while (clients_connected < expected_clients) {
            int bytes_to_send, sent_bytes, size;
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

            if(is_address_found(client_IP, client_port) == FALSE){
                printf("[-] The server cannot accept connections from this address.\n ");
                close(new_socket);
                continue;
            }

            size = n/expected_clients;
            bytes_to_send = htonl(size);
            sent_bytes = send(new_socket, &bytes_to_send, sizeof(bytes_to_send), 0);
            if(sent_bytes != sizeof(bytes_to_send)){
                perror("[-] Failed to send data: ");
                exit(EXIT_FAILURE);
            }

            for(int i = 0; i < n; i++){
                for(int j = 0; j < size; j++){
                    bytes_to_send = htonl(submatrices[clients_connected][i][j]);
                    sent_bytes = send(new_socket, &bytes_to_send, sizeof(bytes_to_send), 0);
                    if(sent_bytes != sizeof(bytes_to_send)){
                        perror("[-] Failed to send data: ");
                        exit(EXIT_FAILURE);
                    }
                }
            }

            clients_connected++;
        }
    }

    if (s == SLAVE){
        int bytes_to_recv, recv_bytes, cols;
        int **submatrix;

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
        
        printf("[+] Connected\n");

        recv_bytes = recv(sockfd, &bytes_to_recv, sizeof(bytes_to_recv), 0);
        if(recv_bytes != sizeof(bytes_to_recv)){
            perror("[-] Failed to receive data: ");
            exit(EXIT_FAILURE);
        }
        cols = ntohl(bytes_to_recv);
    
        printf("[+] Received: %d", cols); 

        submatrix = init_empty_matrix(n, cols);
        for(int i = 0; i < n; i++){
            for(int j = 0; j < cols; j++){
                recv_bytes = recv(sockfd, &bytes_to_recv, sizeof(bytes_to_recv), 0);
                if(recv_bytes != sizeof(bytes_to_recv)){
                    perror("[-] Failed to receive data: ");
                    exit(EXIT_FAILURE);
                }
                submatrix[i][j] = ntohl(bytes_to_recv);
            }
        }

    }

    close(sockfd);
    return EXIT_SUCCESS;
}

void send_over_socket(){

}

void send_over_socket_threaded(){

}

void send_over_socket_thread_affine(){
    
}