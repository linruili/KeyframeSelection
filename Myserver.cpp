//
// Created by Ruili on 17-5-4.
//

#include "Myserver.h"

using namespace std;

Myserver::~Myserver()
{
    close_socket();
}

void Myserver::start()
{
    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket创建出错！");
        exit(1);
    }

    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag));

    my_addr.sin_family=AF_INET;
    my_addr.sin_port=htons(SERVPORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero),8);
    if(bind(sock_fd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind出错！");
        exit(1);
    }
    if(listen(sock_fd, BACKLOG) == -1)
    {
        perror("listen出错！");
        exit(1);
    }

    sin_size = sizeof(struct sockaddr_in);
    if((client_fd = accept(sock_fd, (struct sockaddr *)&remote_addr, (socklen_t*)&sin_size)) == -1) {
        perror("accept出错");
        //continue;
    }
    printf("received a connection from %s:%u\n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
}

void Myserver::send_mes(char *mes, int len)
{
    if(!fork())
    {   // 子进程代码段
        if(send(client_fd, mes, len, 0) == -1)
            perror("send出错！");
        exit(0);
    }
}

void Myserver::rec_mes(char *rec_chars)
{
    if ((recvbytes = recv(client_fd, buf, MAXDATASIZE, 0)) == -1)
    {
        perror("recv出错！");
    }
    if(buf=="exit")
        close_socket();
    buf[recvbytes] = '\0';
    printf("Received: %s\n",buf);
    strcpy(rec_chars, buf);
}

void Myserver::set_dir(char *dir)
{
    strcpy(this->rcnn_result_dir , dir);
}

void Myserver::close_socket()
{
    close(client_fd);
}

