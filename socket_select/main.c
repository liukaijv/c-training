#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUFFER_SIZE 1024
#define EXIT_ERROR -1
#define CLIENT_SIZE 20

int main(int argc, char **argv) {

    int server_fd;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("create socket error: ");
        exit(EXIT_ERROR);
    }

    struct sockaddr_in server_address;
    // bzero和memset区别
    bzero(&server_address, sizeof(server_address));
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_family = AF_INET;

    int ret = bind(server_fd, (struct sockaddr *) &server_address, sizeof(server_address));
    if (ret < 0) {
        perror("bind error: ");
        exit(EXIT_ERROR);
    }

    ret = listen(server_fd, CLIENT_SIZE);
    if (ret < 0) {
        perror("listen error: ");
        exit(EXIT_ERROR);
    }

    printf("wait for client...\n");

    struct sockaddr_in client_address;
    socklen_t address_len = sizeof(client_address);

    // 被观察的fd集合
    fd_set read_fds;
    int max_fd = server_fd;
    // 所有客户端的fd
    int client_fds[CLIENT_SIZE];
    for (int i = 0; i < CLIENT_SIZE; ++i) {
        client_fds[i] = 0;
    }

    // 读缓冲
    char buffer[BUFFER_SIZE] = {0};

    while (1) {

        // 第次清空fds后重新加入
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);

        // 加入客户端的fd
        for (int i = 0; i < CLIENT_SIZE; ++i) {
            int fd = client_fds[i];
            if (fd > 0) {
                FD_SET(fd, &read_fds);
            }
            if (fd > max_fd) {
                max_fd = fd;
            }
        }

        // 取被观测的fd
        // 这是要+1，select是从0开始的
        ret = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (ret < 0) {
            perror("select error: ");
            exit(EXIT_ERROR);
        }

        // 服务器的fd
        if (FD_ISSET(server_fd, &read_fds)) {
            int client_fd = accept(server_fd, (struct sockaddr *) &client_address, &address_len);
            if (client_fd < 0) {
                perror("accept error: ");
                exit(EXIT_ERROR);
            }
            printf("accept client %d: ip: %s, port: %d\n", client_fd, inet_ntoa(client_address.sin_addr),
                   ntohs(client_address.sin_port));

            // 找个空的插入
            for (int i = 0; i < CLIENT_SIZE; ++i) {
                if (client_fds[i] == 0) {
                    client_fds[i] = client_fd;
                    break;
                }
            }
            char *connect_data = "Hello from server";
            send(client_fd, connect_data, strlen(connect_data), 0);

        } else {
            ssize_t read_size;
            for (int i = 0; i < CLIENT_SIZE; ++i) {
                int c_fd = client_fds[i];
                if (c_fd > 0 && FD_ISSET(c_fd, &read_fds)) {
                    read_size = recv(c_fd, buffer, BUFFER_SIZE, 0);
                    // 异常或者被关闭的
                    if (read_size == 0) {
                        close(c_fd);
                        client_fds[i] = 0;
                    } else if (read_size < 0) {
                        perror("accept error: ");
                    } else {
                        printf("receive data from %d: %s, size: %zd\n", c_fd, buffer, read_size);
                        bzero(buffer, sizeof(buffer));
                    }
                }
            }
        }

    }

    return 0;
}