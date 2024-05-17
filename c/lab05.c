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
        int expected, total_addr = 0, expected;
        int * sockets;
        struct sockaddr_in server;
        char netAddressString[256], client_IP[INET_ADDRSTRLEN];
        char *token;

        printf("Enter expected number of clients: ");
        scanf("%d", &expected);
        getchar(); 
        
        sockets = (int *) malloc(sizeof(int) * expected);

        check_if_file_exists(&fd, "clients.cfg");
        if(fd == NULL){
            printf("[-] Cannot find the client configuration file. Exiting...\n");
            exit(EXIT_FAILURE);
        }

        addr = (net_address *) malloc(sizeof(net_address) * MAX_ADDR);
        while(fgets(netAddressString, 256, fd)){
            token = strtok(netAddressString, ":");
            addr[total_addr].IP_addr = token;
            token = strtok(NULL, ":");
            addr[total_addr].port = atoi(token);

            total_addr++;
        }

        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(p);

        if(bind(SOCKFD, (struct sockaddr*) &server, sizeof(server)) == -1) {
            perror("[-] Error binding address. ERR_DESC");
            exit(EXIT_FAILURE);
        }

        if(listen(SOCKFD, 5) == -1) {
            perror("[-] Can't listen to port. ERR_DESC");
            exit(EXIT_FAILURE);
        }

        printf("[+] Server listening at port %d.\n", p);

        for(int i = 0; i < expected; i++){
            struct sockaddr_in client;
            socklen_t client_size = sizeof(client);

            sockets[i] = accept(sockfd, (struct sockaddr*) &client, client_size);
            if (sockets[i] == -1) {
                perror("[-] Failed to accept connection. ERR_DESC");
                exit(EXIT_FAILURE);
            }
        }
    }

    /*
    * Code for when the program is the client.
    */
    if(s == SLAVE){
        struct sockaddr_in server, client;
    }
}