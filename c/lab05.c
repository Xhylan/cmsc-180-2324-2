/*
* lab05.c
* Author: Michael Anthony B. Dollentes, CMSC 180 T-6L
*/

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
#include "lab05.h"

//constants for the program
#define MASTER 0 //the program is the server
#define SLAVE 1 //the program is a client
#define MAX_ADDR 50

int SOCKFD, **MATRIX, *VECTOR;
double *RHO_VECTOR;

int main(int argc, char *argv[]) {
    int n, p, s, opt = 1;
    FILE * fd;
    net_address *addr;
    struct sockaddr_in server;

    SOCKFD = socket(AF_INET, SOCK_STREAM, 0);
    if (SOCKFD < 0) {
        perror("[-] Cannot create socket. ERR_DESC: ");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(SOCKFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){
        perror("[-] Failed to set socket options: ");
        exit(EXIT_FAILURE);
    }

    printf("[o] N: ");
    scanf("%d", &n);
    getchar();

    printf("[o] Port: ");
    scanf("%d", &p);
    getchar();

    printf("[o] Status (0 - Master, 1 - Slave): ");
    scanf("%d", &s);
    getchar();

    /*
    * Code for when the program is the server.
    */
    if(s == MASTER){
        int expected, current = 0;
        int *sockets = NULL;

        printf("[o] Expected clients: ");
        scanf("%d", &expected);
        getchar();

        sockets = (int *) malloc(sizeof(int) * expected);

        /*
         * Initialize all data structures.
         */
        srand(time(NULL));
        MATRIX = initialize_matrix(n);
        VECTOR = initialize_vector(n);
        RHO_VECTOR = (double *) malloc(sizeof(double) * n);

        check_if_file_exists(&fd, "clients.cfg");

        if(fd == NULL){
            printf("[-] Can't find client configuration file!\n");
            return EXIT_FAILURE;
        }

        addr = (net_address *) malloc(sizeof(net_address) * MAX_ADDR);
        
        char netAddressString[256];
        while (fgets(netAddressString, 256, fd)){
            char *token = strtok(netAddressString, ":");
            addr[current].IP_addr = token;
            token = strtok(NULL, ":");
            addr[current].port = atoi(token);
            
            current++;
        }

        /*
         * Setting the server.
         */
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(p);

        if(bind(SOCKFD, (struct sockaddr *) &server, sizeof(server) < 0)) {
            perror("[-] Binding failed! ERR_DESC: ");
            exit(EXIT_FAILURE);
        }

        /*
         * Listen for client connections.
         */
        if(listen(SOCKFD, 5) < 0){
            perror("[-] Can't listen to port! ERR_DESC: ");
            exit(EXIT_FAILURE);
        }
    
        for (int i = 0; i < expected; i++){
            struct sockaddr_in client;
            socklen_t client_size = sizeof(client);
        }
    }

    /*
    * Code for when the program is the client.
    */
    if(s == SLAVE){

    }
}