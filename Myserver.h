//
// Created by Ruili on 17-5-4.
//

#ifndef KEYFRAMESELECTION_MYSERVER_H
#define KEYFRAMESELECTION_MYSERVER_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


#define SERVPORT 3333   // 服务器监听端口号
#define BACKLOG 10  // 最大同时连接请求数
#define MAXDATASIZE 1000

class Myserver
{
private:
    char buf[MAXDATASIZE];
    int recvbytes;
    int sock_fd,client_fd;  // sock_fd：监听socket；client_fd：数据传输socket
    int sin_size;
    struct sockaddr_in my_addr; // 本机地址信息
    struct sockaddr_in remote_addr; // 客户端地址信息
    long flag = 1;
    char rcnn_result_dir[1024];

public:
    void start();
    void send_mes(char *mes, int len);
    void set_dir(char *dir);


};


#endif //KEYFRAMESELECTION_MYSERVER_H
