#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>

#define EXIT_ERROR -1
#define PORT 8888
#define BUFFER_SIZE 1024
#define CLIENT_MAX 1024
#define EVENT_SIZE 100

int main(int argc, char **argv) {

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("create socket error: ");
        exit(EXIT_ERROR);
    }

    // 每次感觉和其它语言相比好麻烦
    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    int ret = bind(server_fd, (struct sockaddr *) &server_address, sizeof(server_address));
    if (ret < 0) {
        perror("bind error: ");
        exit(EXIT_ERROR);
    }

    ret = listen(server_fd, CLIENT_MAX);
    if (ret < 0) {
        perror("listen error: ");
        exit(EXIT_ERROR);
    }

    printf("serve at: %s:%d\n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));

    struct epoll_event ev, events[EVENT_SIZE];

    // 创建epoll fd
    int ep_fd = epoll_create(CLIENT_MAX + 1);
    if (ep_fd < 0) {
        perror("epoll create error: ");
        exit(EXIT_ERROR);
    }

    // todo 所有的都用一个ev有点疑惑
    // 服务端fd先加入
    ev.data.fd = server_fd;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(ep_fd, EPOLL_CTL_ADD, server_fd, &ev);

    int ev_size;
    int client_fd;
    struct sockaddr_in client_address;
    socklen_t address_len = sizeof(client_address);
    char buffer[BUFFER_SIZE];
    ssize_t data_size = 0;

    while (1) {

        ev_size = epoll_wait(ep_fd, events, EVENT_SIZE, -1);
        if (ev_size < 0) {
            perror("epoll wait error: ");
            break;
        }

        for (int i = 0; i < ev_size; ++i) {
            // 客户端连接进来
            int fd = events[i].data.fd;
            if (fd == server_fd) {
                client_fd = accept(server_fd, (struct sockaddr *) &client_address, &address_len);
                if (client_fd < 0) {
                    perror("accept error: ");
                    continue;
                }

                printf("accept client: fd=%d, ip=%s, port=%d\n", client_fd, inet_ntoa(client_address.sin_addr),
                       ntohs(client_address.sin_port));

                // 客户端fd加入进去
                ev.data.fd = client_fd;
                ev.events = EPOLLIN | EPOLLET;
                ret = epoll_ctl(ep_fd, EPOLL_CTL_ADD, client_fd, &ev);
                if (ret < 0) {
                    perror("epoll add error: ");
                    continue;
                }
            } else if (events[i].events & EPOLLIN) {
                // 有数据进来
                bzero(buffer, BUFFER_SIZE);
                data_size = recv(fd, buffer, BUFFER_SIZE, 0);
                // todo 当客户端关闭了有待研究
                if (data_size == 0) {
                    printf("fd %d closed\n", fd);
                    // 这里使用shutdown会死循环
//                    shutdown(fd, SHUT_RDWR);
                    close(fd);
                    events[i].data.fd = -1;
                } else if (data_size < 0) {
                    perror("recv data error: ");
                    if (errno == ECONNRESET) {
                        // 这里使用shutdown会死循环
//                        shutdown(fd, SHUT_RDWR);
                        close(fd);
                        events[i].data.fd = -1;
                    }
                } else {

                    printf("receive data: %s, from fd: %d\n", buffer, fd);

                    // 修改epoll状态写可用
                    ev.data.fd = fd;
                    ev.events = EPOLLOUT | EPOLLET;
                    ret = epoll_ctl(ep_fd, EPOLL_CTL_MOD, fd, &ev);
                    if (ret < 0) {
                        perror("epoll mod error: ");
                    }
                }
            } else if (events[i].events & EPOLLOUT) {

                char *send_data = "hello from server";
                data_size = send(fd, send_data, strlen(send_data), 0);
                if (data_size < 0) {
                    perror("send data error: ");
                    continue;
                }

                // 修改epoll状态读可用
                ev.data.fd = fd;
                ev.events = EPOLLIN | EPOLLET;
                ret = epoll_ctl(ep_fd, EPOLL_CTL_MOD, fd, &ev);
                if (ret < 0) {
                    perror("epoll mod error: ");
                }
            }
        }

    }

    shutdown(server_fd, SHUT_RDWR);
    printf("server exit");

    return 0;
}