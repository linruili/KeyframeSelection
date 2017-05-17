//
// Created by Ruili on 17-5-16.
//

#ifndef KEYFRAMESELECTION_LANDMARK_IDENTIFICATION_H
#define KEYFRAMESELECTION_LANDMARK_IDENTIFICATION_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "classification.h"
#include <set>
#include <string>


using namespace std;

Prediction landmark_identify(vector<cv::Mat> imgs, Classifier &classifier)
{
    //读入ROI sequence，返回这应该对应哪个landmark

    using namespace cv;

    Prediction result;

    int imgs_size = imgs.size();
    vector<vector<Prediction>> predictions;
    set<string> candidates_tmp;
    vector<string> candidates;
    for(int i=0; i<imgs_size; ++i)
    {
        Mat predictImg = imgs[i];
        vector<Prediction> prediction = classifier.Classify(predictImg);
        predictions.push_back(prediction);
    }

    //candidates里存放的所有图片预测的top1,且出现过至少两次
    for(int i=0; i<imgs_size; ++i)
    {
        string landmark_name = predictions[i][0].first;
        if(candidates_tmp.count(landmark_name)==1)
            if(std::find(candidates.begin(), candidates.end(), landmark_name) == candidates.end())
                candidates.push_back(landmark_name);
        candidates_tmp.insert(landmark_name);
    }

    cout<<"candidates.size = "<<candidates.size()<<endl;

    //p为图片i判断为landmark j的概率
    float p[imgs_size][candidates.size()];
    for(int i=0; i<imgs_size; ++i)
        for(int k=0; k<candidates.size(); ++k)
        {
            p[i][k] = 0;
            for(int j=0; j<5; ++j)
                if(predictions[i][j].first == candidates[k])
                {
                    p[i][k] = predictions[i][j].second;
                    break;
                }
        }

    //n为fast特征点数
    int n[imgs_size];
    int keypoints_sum = 0;
    for(int i=0; i<imgs_size; ++i)
    {
        Mat img = imgs[i];
        vector<KeyPoint> keypoints;
        Ptr<FastFeatureDetector> detector = FastFeatureDetector::create(40);
        detector->detect(img,keypoints);
        n[i] = keypoints.size();
        keypoints_sum += n[i];
    }

    float phi[imgs_size];
    for(int i=0; i<imgs_size; ++i)
        phi[i] = n[i]/float(keypoints_sum);

    float eta[candidates.size()];
    for(int i=0; i<candidates.size(); ++i)
    {
        eta[i] = 0;
        for(int j=0; j<imgs_size; ++j)
            eta[i] += phi[j]*p[j][i];
    }

    int maxEta = 0;
    int res_index;

    for(int i=0; i<candidates.size(); ++i)
        if(eta[i]>maxEta)
        {
            maxEta = eta[i];
            res_index = i;
        }

    for(int i=0; i<candidates.size(); ++i)
        cout<<eta[i]<<endl;


    //待考查
    if(eta[res_index]>=0.5)
    {
        result.first = candidates[res_index];
        result.second = eta[res_index];
    }
    else
    {
        result.first = "-1";
        result.second = 0.0;
    }
    return result;


}


#endif //KEYFRAMESELECTION_LANDMARK_IDENTIFICATION_H
