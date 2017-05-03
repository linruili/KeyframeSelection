//
// Created by Mingkuan Li on 2016/12/8.
//

#include <opencv2/opencv.hpp>

#ifndef KEYFRAMESELECTION_CONSTANT_H
#define KEYFRAMESELECTION_CONSTANT_H

#endif //KEYFRAMESELECTION_CONSTANT_H

class Constant {
public:
    const static int tracker_threshold = 50;
    const static int tracker_ignore_threshold = 30;
    const static int tracker_ignore_start_threshold = 50;
    const static int tracker_jump_length = 30;

    const static int rcnn_method = 1;
};

class Landmark {
public:
    cv::Rect rect;//Rect.x  Rectl.y×óÉÏ½Çµã
    char filename[256];
    double prob;
    int landmark_id;
};
