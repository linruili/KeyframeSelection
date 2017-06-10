//
// Created by Ruili on 17-5-3.
//

#ifndef KEYFRAMESELECTION_KEYFRAME_INIT_H
#define KEYFRAMESELECTION_KEYFRAME_INIT_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <string>
#include <ctime>
#include <kcftracker.hpp>
#include <recttools.hpp>
#include <cstdarg>
#include <fstream>
#include <algorithm>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <regex>
#include "constant.h"
#include <sys/timeb.h>

using namespace std;

Constant constant;
int rcnn_counter = 0;
vector<cv::Mat> frames;
vector<string> frame_names;

void keyframe_init(char *filename)
{
    //读入图片，放入frames, name放入frame_names
    rcnn_counter = 0;

    char front_dir_name[256];
    sprintf(front_dir_name, "%s/front/", filename);

    int frame_count = 0;

    vector<int> frames_index;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(front_dir_name)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
            {
                frame_count++;
                char image_name[256];
                strcpy(image_name, ent->d_name);
                long length = strlen(image_name);
                image_name[length - 4] = '\0';
                frames_index.push_back(atoi(image_name));
            }
        }
    }
    closedir(dir);

    sort(frames_index.begin(), frames_index.end());

    timeb t1,t2;
    ftime(&t1);
    cout<<"Loaded frames... "<<endl;
    for(int index = 0; index < frame_count; index++)
    {
        int frame_index = frames_index[index];
        char image_name[256];
        sprintf(image_name, "%s/%03d.jpg", front_dir_name, frame_index);
        cv::Mat image = cv::imread(image_name, CV_LOAD_IMAGE_COLOR);
        frames.push_back(image);

        char filename[256];
        sprintf(filename, "%03d.jpg", frame_index);

        string name = filename;
        frame_names.push_back(name);
    }
    ftime(&t2);
    long t = (t2.time-t1.time)*1000+t2.millitm-t1.millitm;
    cout<<"frame_count = "<<frame_count<<endl;
    cout<<"**runtime for loading = "<<t<<" ms"<<endl;
    cout<<"load time per frame = "<<t/frame_count<<endl;
}

#endif //KEYFRAMESELECTION_KEYFRAME_INIT_H
