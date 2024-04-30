#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "lab04.h"

#define PORT 3000
#define MASTER 0
#define SLAVE 1

int main(){
  struct sockaddr_in server, client;
  int server_socket, client_socket;
  int status, size;

  printf("Status (0 - Master, 1 - Slave): ");
  scanf("%d", &status);
  getchar();

  if (status == MASTER) {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == 1) {
      perror("socket(): ");
      return EXIT_FAILURE;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    int bind_res = bind(server_socket, (struct sockaddr*) &server, sizeof(server));
    if (bind_res == -1){
      perror("bind()");
      close(server_socket);
      return EXIT_FAILURE;
    }

    printf("Size of the matrix: ");
    scanf("%d", &size);
    getchar();

    if (listen(server_socket, 5) == -1){
      perror("listen()");
      close(server_socket);
      return EXIT_FAILURE;
    }

    while (1) {
      socklen_t cli_len = sizeof(client);
      client_socket = accept(server_socket, (struct sockaddr*) &client, &cli_len);
      if (client_socket == -1){
        perror("accept()");
        exit(EXIT_FAILURE);
      }

      printf("Client connected!\n");

      int size_nw = htonl(size);
      int sent_bytes = send(server_socket, &size_nw, sizeof(size_nw), 0);
      if (sent_bytes != sizeof(size_nw)) {
        perror("send()");
        exit(EXIT_FAILURE);
      }
    }

    close(server_socket);
  }

  if (status == SLAVE) {
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1){
      perror("socket()");
      return EXIT_FAILURE;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr*) &server, sizeof(server)) == -1) {
      perror("connect()");
      return EXIT_FAILURE;
    }

    printf("Client connected to the server!"); 
    int size_nw;
    int recv_bytes = recv(client_socket, &size_nw, sizeof(size_nw), 0);
    if  (recv_bytes != sizeof(size_nw)) {
      perror("recv()");
      return EXIT_FAILURE;
    }
    size = ntohl(size_nw);
    printf("Received: %d\n", size);
    close(client_socket);
  }

  return EXIT_SUCCESS;
}
