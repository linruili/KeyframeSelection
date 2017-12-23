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

Prediction landmark_identify(vector<cv::Mat> imgs, Classifier &classifier);


#endif //KEYFRAMESELECTION_LANDMARK_IDENTIFICATION_H
