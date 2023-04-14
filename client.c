#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_LEN 1024

in_port_t ip_port = 5566;
char *ip_addr = (char *)"127.0.0.1";

char *data = (char *)"tcp interface test";
char buffer[MAX_LEN];

int main(int argc, char *argv[])
{
    struct sockaddr_in serv_addr;
    int sock;
    int len;

    len = strlen(data);
    strncpy(buffer, data, len);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("socket failed\r\n");
        return 0;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(ip_port);
    serv_addr.sin_addr.s_addr = inet_addr(ip_addr); 

    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
   
    send(sock, buffer, len, 0);
    printf("Clint Send(len:%d): %s\r\n", len, buffer);

    read(sock, buffer, len);
    buffer[len] = '\0';
    printf("Clint Recv(len:%d): %s\r\n", len, buffer);

    close(sock);
    return 0;
}