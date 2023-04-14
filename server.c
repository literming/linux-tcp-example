#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>

#define MAX_LEN 1024

in_port_t ip_port = 5566;
char *ip_addr = (char *)"127.0.0.1";

char buffer[MAX_LEN];

void sig_fork(int signo) {
    int stat;
    waitpid(0, &stat, WNOHANG);
    return;
}

int main()
{
    struct sockaddr_in clnt_addr;
    struct sockaddr_in serv_addr;
    struct timeval timeout;
    socklen_t clnt_addr_size;
    int clnt_sock;
    int sock;
    int pid;
    int len;
    
    signal(SIGCHLD, sig_fork);

    timeout.tv_sec = 5; // sec
    timeout.tv_usec = 0; // ms

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        printf("socket failed\r\n");
        return 0;
    } 

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;    
    serv_addr.sin_port = htons(ip_port);
    serv_addr.sin_addr.s_addr = inet_addr(ip_addr);

    bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(sock, 20);

    clnt_addr_size = sizeof(clnt_addr);
    while (1) {
      clnt_sock = accept(sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
      if (clnt_sock != -1) {
        //printf("accept success, id: %d\r\n", clnt_sock);
      }
      // set recv timeout
      if( setsockopt (clnt_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0 ) {
        printf( "setsockopt fail\n");
      }
      // set send timeout
      if( setsockopt (clnt_sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0 ) {
        printf( "setsockopt fail\n" );
      }

      pid = fork();
      if (pid == -1) {
        printf( "fork failed\n" );
      } else if (pid == 0) {
        //printf("%d\n", getpid()) ;
        recv(clnt_sock, buffer, MAX_LEN, 0);
        len = strlen(buffer);
        printf("Server Recv(len:%d):%s\n", len, buffer);

        send(clnt_sock, buffer, len, 0);
        printf("Server Secv(len:%d):%s\n", len, buffer);

        close(clnt_sock);
        close(sock);

        return 0 ;
      } else {
        close(clnt_sock);
      }
    }    
 
    close(sock);
    return 0;
}