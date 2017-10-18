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
    const static int tracker_jump_length = 25;
    const static int sampling = 5;//图片每隔sampling帧，取一帧。此量用于得到正确的罗盘角

    const static int rcnn_method = 2;
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

private:
    vector<int> testcaseNum;
    vector<int> totalFrameNum;
    vector<int> load_time;
    vector<int> rcnn_total_time;
    vector<int> rcnn_total_num;
    vector<int> kcf_total_time;
    vector<int> GoogLeNet_total_time;
    vector<int> locate_total_time;
    vector<int> total_time;
    vector<int> mean_error;  //去掉特别大的数据
    vector<int> not_nan_error_num;

};

Record::Record()
{
    testcaseNum.push_back(12);
    testcaseNum.push_back(9);
    testcaseNum.push_back(10);
    for(int i=0; i<3; ++i)
    {
        totalFrameNum.push_back(0);
        load_time.push_back(0);
        rcnn_total_num.push_back(0);
        rcnn_total_time.push_back(0);
        kcf_total_time.push_back(0);
        GoogLeNet_total_time.push_back(0);
        locate_total_time.push_back(0);
        total_time.push_back(0);
        mean_error.push_back(0);
        not_nan_error_num.push_back(0);
    }

}
#endif //KEYFRAMESELECTION_CONSTANT_H