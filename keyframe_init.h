//
// Created by Ruili on 17-5-3.
//

#ifndef KEYFRAMESELECTION_KEYFRAME_INIT_H
#define KEYFRAMESELECTION_KEYFRAME_INIT_H

#include "tools.h"

//#include <iostream>
//#include <vector>
//#include "constant.h"
//#include <string>
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>



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

        printf("Loaded frame %d\n", frame_names.size());
    }
}

#endif //KEYFRAMESELECTION_KEYFRAME_INIT_H
