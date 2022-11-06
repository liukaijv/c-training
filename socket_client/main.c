#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "0.0.0.0"
#define PORT 8888
#define EXIT_ERROR -1
#define BUFFER_SIZE 1024

int main(int argc, char **argv) {

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("create socket error");
        exit(EXIT_ERROR);
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_address.sin_port = htons(PORT);

    int ret = connect(client_fd, (struct sockaddr *) &server_address, sizeof(server_address));
    if (ret < 0) {
        perror("connect socket error");
        exit(EXIT_ERROR);
    }
    printf("connect to server\n");

    char *send_data = "Hello from client";
    ssize_t send_len;
    send_len = send(client_fd, send_data, strlen(send_data), 0);

    printf("send data: %s, len: %zd\n", send_data, send_len);

    char buffer[BUFFER_SIZE] = {0};
    ssize_t read_len;
    read_len = recv(client_fd, buffer, BUFFER_SIZE, 0);
//    read_len = read(client_fd, buffer, BUFFER_SIZE);

    printf("receive data: %s, len: %zd\n", buffer, read_len);

//    close(client_fd);
    shutdown(client_fd, SHUT_RDWR);

    return 0;
}