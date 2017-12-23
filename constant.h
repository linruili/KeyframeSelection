//
// Created by Ruili on 2017/5/3.
//

#include <opencv2/opencv.hpp>
#include <vector>

#ifndef KEYFRAMESELECTION_CONSTANT_H
#define KEYFRAMESELECTION_CONSTANT_H

using namespace std;


class Constant {
public:
    const static int tracker_threshold = 50;
    const static int tracker_ignore_threshold = 30;
    const static int tracker_ignore_start_threshold = 50;
    const static int tracker_jump_length = 5;
    const static int sampling = 20;//ͼƬÿ��sampling֡��ȡһ֡���������ڵõ���ȷ�����̽�
    //const vector<double> kernel;

    const static int rcnn_method = 2;
    //Constant(vector<double> a):kernel(a){}

};

class Landmark {
public:
    cv::Rect rect;//Rect.x  Rectl.y���Ͻǵ�
    char filename[256];
    double prob;
    int landmark_id;
};



#endif //KEYFRAMESELECTION_CONSTANT_H