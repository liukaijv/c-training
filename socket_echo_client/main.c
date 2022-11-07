#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define PORT 8888
#define HOST_ADDR "127.0.0.1"
#define EXIT_ERROR -1
#define BUFFER_SIZE 1024

int main(int argc, char **argv) {

    int conn_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (conn_fd < 0) {
        perror("socket error");
        exit(EXIT_ERROR);
    }

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(HOST_ADDR);

    // 发起连接
    int ret = connect(conn_fd, (struct sockaddr *) &server_address, sizeof(server_address));
    if (ret < 0) {
        perror("connect error");
        exit(EXIT_ERROR);
    }

    int n;
    char buffer[BUFFER_SIZE];
    ssize_t size;

    while (1) {
        // 每次重置到初值
        n = 0;
        bzero(buffer, sizeof(buffer));

        printf("input: ");
        // 接收键盘输入
        char ch = (char) getchar();
        while (ch != '\n') {
            buffer[n++] = ch;
            ch = (char) getchar();
        }

        printf("input data: %s, size: %lu\n", buffer, strlen(buffer));

        // 什么都没输入
        if (strncmp(buffer, "\n", 1) == 0 || strlen(buffer) == 0) {
            printf("please input something\n");
            continue;
        }

        // 输入exit退出
        if (strncmp(buffer, "exit", 4) == 0) {
            printf("exiting...\n");
            break;
        }

        size = send(conn_fd, buffer, strlen(buffer), 0);
        if (size < 0) {
            printf("send data error: ");
            break;
        }
        printf("send data: %s\n", buffer);

        bzero(buffer, sizeof(buffer));
        size = recv(conn_fd, buffer, BUFFER_SIZE, 0);
        if (size < 0) {
            printf("recv data error: ");
            break;
        }
        printf("receive data: %s\n", buffer);

    }

    shutdown(conn_fd, SHUT_RDWR);
    printf("client close.\n");

    return 0;
}