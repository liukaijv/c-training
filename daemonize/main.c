#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

static void write_log(char *msg) {
    int fd = open("/tmp/daemon.log", O_CREAT | O_WRONLY | O_APPEND, 0600);
    if (fd > 0) {
        char buf[strlen(msg) + strlen("\n")];
        sprintf(buf, "%s\n", msg);
        write(fd, buf, strlen(buf) + 1);
        close(fd);
    }
}

static void daemonize() {

    int ret;
    pid_t pid;
    int fd;

    // 1
    // 创建进程
    pid = fork();

    // 创建进程出错
    if (pid < 0) {
        write_log("fork err");
        exit(EXIT_FAILURE);
    }

    // 退出父进程
    if (pid > 0) {
        write_log("exit parent");
        exit(EXIT_SUCCESS);
    }

    // 2
    // 子进程脱离从父进程继承而来的已打开的终端、隶属进程组和隶属的会话
    ret = setsid();
    if (ret < 0) {
        write_log("setsid err");
        exit(EXIT_FAILURE);
    }

    // 3
    // 忽略来自子进程的信号
    signal(SIGCHLD, SIG_IGN);

    // 4
    // 再次创建进程
    // 在setsid时，脱离terminal会重新分配session，子进程将会是一个session leader，
    // 它可以重新获得一个terminal，再fork一次就能保证不能获得terminal了
    pid = fork();

    // 创建进程出错
    if (pid < 0) {
        write_log("fork2 err");
        exit(EXIT_FAILURE);
    }

    // 退出父进程
    if (pid > 0) {
        write_log("exit parent2");
        exit(EXIT_SUCCESS);
    }

    // 5
    // 重置继承的文件权限
    umask(0);

    // 6
    // 改变工作目录
    ret = chdir("/tmp");
    if (ret < 0) {
        write_log("chdir err");
        exit(EXIT_FAILURE);
    }

    // 7
    // 关闭继承的文件描述符
    for (fd = 0; fd < getdtablesize(); fd++) {
        close(fd);
    }

    // 8
    // todo 上一次中文件描述都关了，是不是没必要了？
    // 把标准输入、输出、错误都到/dev/null
    stdin = fopen("/dev/null", "r");
    stdout = fopen("/dev/null", "w+");
    stderr = fopen("/dev/null", "w+");

    write_log("daemonize success");
}

int main(int argc, char **argv) {

    daemonize();

    char *buffer = "daemon test.";
    int running = 1;

    while (running) {
        write_log(buffer);
        sleep(10);
    }

    return 0;
}