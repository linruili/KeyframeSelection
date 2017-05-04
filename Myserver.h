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


#define SERVPORT 3333   // �����������˿ں�
#define BACKLOG 10  // ���ͬʱ����������
#define MAXDATASIZE 1000

class Myserver
{
private:
    char buf[MAXDATASIZE];
    int recvbytes;
    int sock_fd,client_fd;  // sock_fd������socket��client_fd�����ݴ���socket
    int sin_size;
    struct sockaddr_in my_addr; // ������ַ��Ϣ
    struct sockaddr_in remote_addr; // �ͻ��˵�ַ��Ϣ
    long flag = 1;
    char rcnn_result_dir[1024];

public:
    void start();
    void send_mes(char *mes, int len);
    void set_dir(char *dir);


};


#endif //KEYFRAMESELECTION_MYSERVER_H
