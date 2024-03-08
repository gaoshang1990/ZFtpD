#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_PORT 21
#define SERVER_IP   "192.168.1.101"

int listen_fd = -1;

void signal_handler(int arg)
{
    printf("close listen_fd(signal = %d)\n", arg);
    close(listen_fd);
    exit(0);
}

int main(int argc, const char* argv[])
{
    int                new_fd = -1;
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t          saddrlen = sizeof(server);
    socklen_t          caddrlen = sizeof(client);

    signal(SIGINT, signal_handler);

    memset(&server, 0, sizeof(server));
    memset(&client, 0, sizeof(client));

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        printf("socket error!\n");
        return -1;
    }

    server.sin_family = AF_INET;
    server.sin_port   = htons(SERVER_PORT);
    // server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listen_fd, (struct sockaddr*)&server, saddrlen) < 0) {
        printf("bind error!\n");
        return -1;
    }

    if (listen(listen_fd, 5) < 0) {
        printf("listen error!\n");
        return -1;
    }

    char rbuf[256] = {0};
    int  read_size = 0;
    while (1) {
        /*
        socket()创建的套接字默认是阻塞的，所以accept()在该套接字上进行监听时，
        如果没有客户端连接请求过来，accept()函数会一直阻塞等待；换句话说，程序
        就停在accept()函数这里，不会继续往下执行，直到有新的连接请求发送过来，唤醒accept()。
        */
        new_fd = accept(listen_fd, (struct sockaddr*)&client, &caddrlen);
        if (new_fd < 0) {
            perror("accept");
            return -1;
        }

        printf("new client connected.IP:%s,port:%u\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        while (1) {
            read_size = read(new_fd, rbuf, sizeof(rbuf));
            if (read_size < 0) {
                printf("read error!\n");
                continue;
            }
            else if (read_size == 0) {
                printf("client (%d) is closed!\n", new_fd);
                close(new_fd);
                break;
            }

            printf("recv:%s\n", rbuf);
        }
    }

    close(listen_fd);

    return 0;
}
