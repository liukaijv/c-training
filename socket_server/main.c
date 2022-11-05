#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define SERVER_IP "0.0.0.0"
#define PORT 8888
#define EXIT_ERROR -1
#define CLIENT_SIZE 20
#define BUFFER_SIZE 1024

int main(int argc, char **argv) {

    // 地址族 address family
    // AF_INET-ipv4  AF_INET-ipv6
    // 类型 SOCK_STREAM-tcp SOCK_DGRAM-udp
    // 协议 0-auto IPPROTO_TCP-tcp IPPROTO_UDP-udp
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("create socket error");
        exit(EXIT_ERROR);
    }

    struct sockaddr_in server_address;
    // 填充0
    memset(&server_address, 0, sizeof(server_address));
    // AF_INET-ipv4  AF_INET-ipv6
    server_address.sin_family = AF_INET;
    // ip转为整形表式
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    // htonl()--"Host to Network Long"
    // ntohl()--"Network to Host Long"
    // htons()--"Host to Network Short"
    // ntohs()--"Network to Host Short"
    server_address.sin_port = htons(PORT);

    // sockaddr_in->sockaddr 类型转换
    int ret = bind(server_fd, (struct sockaddr *) &server_address, sizeof(server_address));
    if (ret < 0) {
        perror("bind error");
        exit(EXIT_ERROR);
    }

    ret = listen(server_fd, CLIENT_SIZE);
    if (ret < 0) {
        perror("listen error");
        exit(EXIT_ERROR);
    }

    struct sockaddr_in client_address;
    socklen_t address_len = sizeof(client_address);
    // 第三个参数是socklen_t指针类型
    int client_fd = accept(server_fd, (struct sockaddr *) &client_address, &address_len);
    if (client_fd < 0) {
        perror("accept error");
        exit(EXIT_ERROR);
    }

    char client_ip[INET_ADDRSTRLEN];
    // 解析出ip inet_ntop inet_ntoa
    inet_ntop(AF_INET, &(client_address.sin_addr), client_ip, INET_ADDRSTRLEN);
    printf("client ip: %s\n", client_ip);

    char buffer[BUFFER_SIZE] = {0};
    // MSG_WAITALL 阻塞模式接收
    // MSG_DONTWAIT 非阻塞模式接收
    ssize_t read_len;
    read_len = recv(client_fd, buffer, BUFFER_SIZE, 0);
//    read_len = read(client_fd, buffer, BUFFER_SIZE);

    printf("receive data: %s, len: %zd\n", buffer, read_len);

    char *send_data = "Hello from server";
    ssize_t send_len = send(client_fd, send_data, strlen(send_data), 0);

    printf("send data: %s, len: %zd\n", send_data, send_len);

    // close 如果其它进程共享fd，仍然是打开的
    // shutdown 1、切断所有共享关联的；2、设置断开的方式
    close(client_fd);
//    close(server_fd);
    shutdown(server_fd, SHUT_RDWR);

    return 0;
}