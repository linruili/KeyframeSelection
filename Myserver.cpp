//
// Created by Ruili on 17-5-4.
//

#include "Myserver.h"
#include <sys/stat.h>
#include "tools.h"

using namespace std;

Constant constant;
int rcnn_counter = 0;
int isReceiving = 1;
std::vector<cv::Mat> frames;
vector<string> frame_names;
std::vector<double> compass;
int total_frame;

Myserver::~Myserver()
{
    close_socket();
}

void Myserver::start(int SERVPORT)
{
    this->SERVPORT = SERVPORT;
    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket创建出错！\n");
        exit(1);
    }

    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag));

    my_addr.sin_family=AF_INET;
    my_addr.sin_port=htons(SERVPORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero),8);
    if(bind(sock_fd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind出错！\n");
        exit(1);
    }
    if(listen(sock_fd, BACKLOG) == -1)
    {
        perror("listen出错！\n");
        exit(1);
    }

    sin_size = sizeof(struct sockaddr_in);
    if((client_fd = accept(sock_fd, (struct sockaddr *)&remote_addr, (socklen_t*)&sin_size)) == -1) {
        perror("accept出错\n");
        //continue;
    }
    printf("received a connection from %s:%u\n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
}

void Myserver::send_mes(char *mes, int len)
{
    //if(!fork())
    {   // 子进程代码段
        if(send(client_fd, mes, len, 0) == -1)
            perror("send出错！\n");
        //exit(0);
    }
}

void Myserver::rec_mes(char *rec_chars)
{
    if ((recvbytes = recv(client_fd, buf, MAXDATASIZE, 0)) == -1)
    {
        perror("recv出错！\n");
    }
    if(buf=="exit")
        close_socket();
    buf[recvbytes] = '\0';
    printf("Received: %s\n",buf);
    strcpy(rec_chars, buf);
}

void Myserver::recv_img_compass()
{

    char recv_dir[256] = "../output/receive";
    char compass_filename[256] = "../ouput/receive/compass.txt";
    char img_name[256];

    mkdir(recv_dir, 0777);
    FILE *compass_file = fopen(compass_filename, "a");
    int doubleSize;
    int frame_count = 0;

    int imgSize = 640*480;
    uchar sockData[imgSize];
    while(frame_count >= 0)
    {
        char rec_int[4];
        if((recvbytes=recv(client_fd, rec_int, sizeof(int), 0)) == -1)
        {
            perror("recv出错！");
            return ;
        }
        imgSize = buffToInteger(rec_int);
        cout<<"imgSize: "<<imgSize<<endl;
        if(imgSize==0)//结束的标志
        {
            frame_count = -1;
            break;
        }

        ++frame_count;
        sprintf(img_name, "%s/%03d.jpg", recv_dir, frame_count);
        int total_rec_bytes = 0;
        for(int i=0; i<imgSize; i+=recvbytes)
        {
            recvbytes = recv(client_fd, sockData + i, imgSize - i, 0);
            //cout<<"receive bytes:"<<recvbytes<<endl;
            total_rec_bytes += recvbytes;
            if(recvbytes == -1)
            {
                perror("recv出错！");
                return ;
            }
        }
        sockData[total_rec_bytes] = '\0';
        cout<<"receive total bytes:"<<total_rec_bytes<<endl;

        vector<uchar> img_data(sockData, sockData+total_rec_bytes);
        cv::Mat img = cv::imdecode(img_data, CV_LOAD_IMAGE_COLOR);
        frames.push_back(img);

        char filename[256];
        sprintf(filename, "%03d.jpg", frame_count);
        string name = filename;
        frame_names.push_back(name);

        cv::imwrite(img_name, img);
        cout<<"saved image "<<frame_count<<"-----------------"<<endl;

        //receive compass
        if((recvbytes=recv(client_fd, rec_int, sizeof(int), 0)) == -1)
        {
            perror("recv出错！");
            return ;
        }
        doubleSize = buffToInteger(rec_int);
        //cout<<"doubleSize: "<<doubleSize<<endl;

        if((recvbytes=recv(client_fd, buf, doubleSize, 0)) == -1)
        {
            perror("recv出错！");
            return ;
        }
        buf[recvbytes] = '\0';
        double compass_each_frame = atof(buf);
        compass.push_back(compass_each_frame);
        cout<<"compass: "<<compass_each_frame<<endl;
        fprintf(compass_file, "%f\n", compass_each_frame);
    }
    fclose(compass_file);
    isReceiving = 0;
    total_frame = frame_count;

}

void Myserver::set_dir(char *dir)
{
    strcpy(this->rcnn_result_dir , dir);
}

void Myserver::close_socket()
{
    close(client_fd);
}

