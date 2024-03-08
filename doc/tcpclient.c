
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

int main(int argc, const char* argv[])
{
    int                connect_fd = -1;
    struct sockaddr_in server;
    socklen_t          saddrlen = sizeof(server);

    memset(&server, 0, sizeof(server));

    connect_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect_fd < 0) {
        printf("socket error!\n");
        return -1;
    }

    server.sin_family      = AF_INET;
    server.sin_port        = htons(SERVER_PORT);
    server.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(connect_fd, (struct sockaddr*)&server, saddrlen) < 0) {
        printf("connect failed!\n");
        return -1;
    }

    char buf[256] = {0};
    while (1) {
        printf(">");
        fgets(buf, sizeof(buf), stdin);
        if (strcmp(buf, "quit\n") == 0) {
            printf("client will quit!\n");
            break;
        }
        write(connect_fd, buf, sizeof(buf));
    }
    close(connect_fd);

    return 0;
}
