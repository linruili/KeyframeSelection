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
#include <opencv2/core/mat.hpp>
#include "constant.h"


#define BACKLOG 10  // ���ͬʱ����������
#define MAXDATASIZE 1000

extern Constant constant;
extern int rcnn_counter;
extern int isReceiving;
extern std::vector<cv::Mat> frames;
extern vector<string> frame_names;
extern std::vector<double> compass;
extern int total_frame;

class Myserver
{
private:
    int SERVPORT;
    char buf[MAXDATASIZE];
    int recvbytes;
    int sock_fd,client_fd;  // sock_fd������socket��client_fd�����ݴ���socket
    int sin_size;
    struct sockaddr_in my_addr; // ������ַ��Ϣ
    struct sockaddr_in remote_addr; // �ͻ��˵�ַ��Ϣ
    long flag = 1;
    char rcnn_result_dir[1024];
    int mes_index=0;


public:
    ~Myserver();
    void start(int SERVPORT);
    void send_mes(char *mes, int len);
    void set_dir(char *dir);
    void rec_mes(char *rec_chars);
    void close_socket();
    void recv_img_compass(char *recv_dir);


};


#endif //KEYFRAMESELECTION_MYSERVER_H
