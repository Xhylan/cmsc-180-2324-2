#define _GNU_SOURCE

#include <arpa/inet.h>
#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <unistd.h>
#include "lab04.h"

#define MASTER 0
#define SLAVE 1
#define TRUE 10
#define FALSE 11
#define ACK_MSG "ACK"

int sockfd;
pthread_mutex_t mutex; 

void *send_data(void *args){
    int size, to_send, sent_bytes, recv_bytes;
    int ** submatrix = NULL, *vector = NULL;
    char buffer[1024];
    arg *temp = args;

    size =  temp -> n / temp -> expected_client;
    to_send = htonl(size);
    pthread_mutex_lock(&mutex);
    sent_bytes = send(temp -> socket, &to_send, sizeof(to_send), 0);
    pthread_mutex_unlock(&mutex);

    if(sent_bytes != sizeof(to_send)){
        perror("[-] Failed to send: ");
        exit(EXIT_FAILURE);
    }

    submatrix = temp -> submatrix;
    for (int i = 0; i < temp -> n; i++){
        for (int j = 0; j < size; j++){
            to_send = htonl(submatrix[i][j]);
            pthread_mutex_lock(&mutex);
            sent_bytes = send(temp -> socket, &to_send, sizeof(to_send), 0);
            pthread_mutex_unlock(&mutex);
            if(sent_bytes != sizeof(to_send)){
                perror("[-] Failed to send data: ");
                exit(EXIT_FAILURE);
            }
        }
    }

    vector = temp -> vector;
    for (int j = 0; j < temp -> n; j++) {
        to_send = htonl(vector[j]);
        pthread_mutex_lock(&mutex);
        sent_bytes = send(temp -> socket, &to_send, sizeof(to_send), 0);
        pthread_mutex_unlock(&mutex);
        if(sent_bytes != sizeof(to_send)){
            perror("[-] Failed to send data: ");
            exit(EXIT_FAILURE);
        }
    }

    pthread_mutex_lock(&mutex);
    recv_bytes = recv(temp -> socket, buffer, sizeof(buffer), 0);
    pthread_mutex_unlock(&mutex);
    if(recv_bytes > 0){
        printf("[+] Acknowledgment received.\n");
    } 

    if(recv_bytes == 0){
        printf("[o] Connection closed by client.\n");
    }

    if(recv_bytes < 0){
        perror("[-] Cannot receive the data: ");
    }

    return NULL;
}


void send_over_socket(int expected_client, int n, int *sockets, 
                        int * vector, int *** submatrices);
void send_over_socket_thread_affine(int expected_client, int n, int *sockets,
                                    int * vector, int *** submatrices);

int main(int argc, char *argv[]){
    int n, p, s, opt = 1; 
    int ** matrix, * vector, ***submatrices;
    double * rho_vector, elapsed;
    struct sockaddr_in server, client;
    struct timespec start, end;
    socklen_t client_addr_size;
    cpu_set_t cpuset;
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

    //Define a socket.
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
        int expected_clients = 0, connected = 0, size, choice, client_port;
        int *sockets;
        char netAddressString[256], client_IP[INET_ADDRSTRLEN]; ;
        char *token;
        net_address * head = NULL;

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

        /*
         * Obtain all addresses from server.cfg.
         */
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


        /*
         * Bind socket to specified server address.
         */
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(p); 

        if (bind(sockfd, (struct sockaddr*)&server, sizeof(server)) == -1) {
            perror("[-] Error binding to address!");
            return 1;
        }

        if (listen(sockfd, 5) == -1) {
            perror("[-] Can't listen to specified port!");
            return 1;
        }

        printf("[+] Server listening on port %d\n", ntohs(server.sin_port));

        sockets = (int*) malloc(expected_clients * sizeof(int));

        while(connected < expected_clients){
            struct sockaddr_in client;
            socklen_t client_addr_size = sizeof(client);

            sockets[connected] = accept(sockfd, (struct sockaddr*) &client, &client_addr_size);
            if (sockets[connected] < 0) {
                perror("[-] Failed to accept client connection!");
                continue;
            }   

            getpeername(sockets[connected], (struct sockaddr *) &client, &client_addr_size);
            inet_ntop(AF_INET, &(client.sin_addr), client_IP, client_addr_size);
            client_port = ntohs(client.sin_port);

            if (is_address_found(client_IP, client_port, head) == FALSE){
                printf("[-] The server cannot accept connections from this address.\n ");
                close(sockets[connected]);
                continue;
            }

        connected++;
    }

        choice = printMenu();
        switch(choice){
            case 1: send_over_socket(expected_clients, n, sockets, vector, submatrices);
                    break;
            case 2:
                    if(pthread_mutex_init(&mutex, NULL) != 0){
                        perror("[-] Cannot initialize mutex!");
                        exit(EXIT_FAILURE);
                    }

                    send_over_socket_thread_affine(expected_clients, n, sockets, vector, submatrices);

                    if(pthread_mutex_destroy(&mutex) != 0){
                        perror("[-] Cannot destroy mutex!");
                        exit(EXIT_FAILURE);
                    }
                    break;
            case 0:
                    printf("[o] Canceling...\n");
                    break;
            default:
                    printf("[-] Invalid option!");
                    break;
        }

        for(int i = 0; i < expected_clients; i++){
            close(sockets[i]);
        }

        free(sockets);
    }




    if (s == SLAVE){
        int bytes_to_recv, recv_bytes, cols;
        int *vector, **submatrix;
        char *token, netAddressString[256];
        const char* ack_msg = ACK_MSG;
        net_address * server_addr;

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

        fgets(netAddressString, 256, fd);
        server_addr = (net_address *) malloc(sizeof(net_address));
        token = strtok(netAddressString, ":");
        server_addr -> IP_addr = token;
        token = strtok(NULL, ":");
        server_addr -> port = atoi(token);
        server_addr -> next = NULL;

        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr(server_addr -> IP_addr);
        server.sin_port = htons(server_addr -> port); 

        if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) == -1) {
            perror("[-] Cannot connect to the server: ");
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

        vector = (int *) malloc(sizeof(int) * n);
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

        for(int i = 0; i < n; i++){
            recv_bytes = recv(sockfd, &bytes_to_recv, sizeof(bytes_to_recv), 0);
            if(recv_bytes != sizeof(bytes_to_recv)){
                perror("[-] Failed to receive data: ");
                exit(EXIT_FAILURE);
            }
            vector[i] = ntohl(bytes_to_recv);
        }

        send(sockfd, ack_msg, strlen(ack_msg), 0);
    }

    close(sockfd);
    return EXIT_SUCCESS;
}

void send_over_socket(int expected_client, int n, int * sockets, int * vector, int *** submatrices){
    int size;
    int to_send, sent_bytes, recv_bytes;
    char buffer[1024];

    for (int i = 0; i < expected_client; i++){
        size = n/expected_client;
        to_send = htonl(size);
        sent_bytes = send(sockets[i], &to_send, sizeof(to_send), 0);
        if(sent_bytes != sizeof(to_send)){
            perror("[-] Failed to send data: ");
            exit(EXIT_FAILURE);
        }

        for (int j = 0; j < n; j++) {
            for (int k = 0; k < size; k++) {
                to_send = htonl(submatrices[i][j][k]);
                sent_bytes = send(sockets[i], &to_send, sizeof(to_send), 0);
                if(sent_bytes != sizeof(to_send)){
                    perror("[-] Failed to send data: ");
                    exit(EXIT_FAILURE);
                }
            }
        }

        for (int j = 0; j < n; j++) {
            to_send = htonl(vector[j]);
            sent_bytes = send(sockets[i], &to_send, sizeof(to_send), 0);
            if(sent_bytes != sizeof(to_send)){
                perror("[-] Failed to send data: ");
                exit(EXIT_FAILURE);
            }
        }

        int recv_bytes = recv(sockets[i], buffer, sizeof(buffer), 0);

        if(recv_bytes > 0){
            printf("[+] Acknowledgment received.\n");
        } 

        if(recv_bytes == 0){
            printf("[o] Connection closed by client.\n");
        }

        if(recv_bytes < 0){
            perror("[-] Cannot receive the data: ");
        }
    }
}


void send_over_socket_thread_affine(int expected_client, int n, int * sockets, int * vector, int *** submatrices){
    int core = 0, nprocs = sysconf(_SC_NPROCESSORS_CONF);
    pthread_t *threads = NULL;
    cpu_set_t cpuset;
    threads = (pthread_t *) malloc(sizeof(pthread_t) * expected_client);

    for (int i = 0; i < expected_client; i++){
        arg *args = (arg*) malloc(sizeof(arg));

        args -> socket = sockets[i];
        args -> n = n;
        args -> expected_client = expected_client;
        args -> submatrix = submatrices[i];
        args -> vector = vector;

        pthread_create(&threads[i], NULL, send_data, (void*) args);

        CPU_ZERO(&cpuset);
        CPU_SET(core, &cpuset);

        if (pthread_setaffinity_np(threads[i], sizeof(cpu_set_t), &cpuset) != 0){
            perror("[-] Unable to set core affinity: ");
            exit(EXIT_FAILURE);
        } else {
            if (core == nprocs-1) core = 0;
            else core++;
        }
    }

    for (int i = 0; i < expected_client; i++){
        pthread_join(threads[i], NULL);
    }

    free(threads);
}
