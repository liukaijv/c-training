#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <poll.h>

#define PORT 8888
#define EXIT_ERROR -1
#define CLIENT_MAX 20

int main(int argc, char **argv) {

    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("create socket error: ");
        exit(EXIT_ERROR);
    }

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    int ret = bind(server_fd, (struct sockaddr *) &server_address, sizeof(server_address));
    if (ret < 0) {
        perror("bind socket error: ");
        exit(EXIT_ERROR);
    }

    ret = listen(server_fd, CLIENT_MAX);
    if (ret < 0) {
        perror("listen socket error: ");
        exit(EXIT_ERROR);
    }

    printf("serve at: %s:%d\n", inet_ntoa(server_address.sin_addr), PORT);

//    初始化fds
//    struct pollfd {
//        int fd;        //文件描述符
//        short events;  //要求查询的事件掩码
//        short revents; //返回的事件掩码
//    };
    struct pollfd fds[CLIENT_MAX];
    for (int i = 0; i < CLIENT_MAX; ++i) {
        fds[i].fd = 0;
        fds[i].events = 0;
    }

    // 服务端socket_fd加进去
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    struct sockaddr_in client_address;
    socklen_t address_len = sizeof(client_address);

    char buffer[1024];
    bzero(buffer, sizeof(buffer));

    while (1) {

        ret = poll(fds, CLIENT_MAX + 1, -1);
//        printf("poll ret: %d\n", ret);
        if (ret < 0) {
            perror("poll error: ");
            exit(EXIT_ERROR);
        }

        // 服务端的fd
        // 是用revents作比较
        if (fds[0].revents & POLLIN) {
            int client_fd = accept(server_fd, (struct sockaddr *) &client_address, &address_len);
            if (client_fd < 0) {
                perror("accept error: ");
                exit(EXIT_ERROR);
            }
            printf("client connect ip: %s, port: %hu, fd: %d\n", inet_ntoa(client_address.sin_addr),
                   ntohs(client_address.sin_port), client_fd);

            int i = 1;
            for (i; i < CLIENT_MAX; ++i) {
                // 加入观测数组中
                if (fds[i].fd == 0) {
                    fds[i].fd = client_fd;
                    fds[i].events = POLLIN;
                    break;
                }
            }
            if (i == CLIENT_MAX) {
                printf("max client limit: %d", CLIENT_MAX);
            }
        } else {
            for (int i = 1; i < CLIENT_MAX; ++i) {
                if (fds[i].fd > 0 && fds[i].revents & POLLIN) {
                    int fd = fds[i].fd;
                    ssize_t read_size = recv(fd, buffer, sizeof(buffer), 0);
                    if (read_size <= 0) {
                        close(fd);
                        fds[i].fd = 0;
                        fds[i].events = 0;
                        break;
                    } else {
                        printf("receive client %d data: %s\n", fd, buffer);
                        char *data = "Hello from server";
                        send(fd, data, strlen(data), 0);
                    }
                    bzero(buffer, sizeof(buffer));
                }
            }
        }

    }

    for (int i = 0; i < CLIENT_MAX; ++i) {
        if (fds[i].fd > 0) {
            close(fds[i].fd);
        }
    }
    return 0;
}