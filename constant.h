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
    const static int sampling = 20;//图片每隔sampling帧，取一帧。此量用于得到正确的罗盘角
    //const vector<double> kernel;

    const static int rcnn_method = 2;
    //Constant(vector<double> a):kernel(a){}

};

class Landmark {
public:
    cv::Rect rect;//Rect.x  Rectl.y左上角点
    char filename[256];
    double prob;
    int landmark_id;
};


class Record{
public:
    Record();

    vector<int> testcaseNum;
    vector<int> totalFrameNum;
    vector<int> load_time;
    vector<int> rcnn_total_time;
    vector<int> rcnn_total_num;
    vector<int> kcf_total_time;
    vector<int> GoogLeNet_total_time;
    vector<int> locate_total_time;
    vector<int> total_time;  //不算load时间
    vector<double> total_error;  //去掉特别大的数据
    vector<int> not_nan_error_num;
    vector< vector<double> > errors;

};

Record::Record():load_time(vector<int>(3,0)),rcnn_total_time(vector<int>(3,0)),
                 rcnn_total_num(vector<int>(3,0)),kcf_total_time(vector<int>(3,0)),
                 GoogLeNet_total_time(vector<int>(3,0)),locate_total_time(vector<int>(3,0)),
                 total_time(vector<int>(3,0)),total_error(vector<double >(3,0)),
                 not_nan_error_num(vector<int>(3,0)),totalFrameNum(vector<int>(3,0)),
                 errors(std::vector<vector<double>>(3,vector<double>()))
{
    testcaseNum.push_back(12);
    testcaseNum.push_back(9);
    testcaseNum.push_back(10);
}

Record final_result;
#endif //KEYFRAMESELECTION_CONSTANT_H