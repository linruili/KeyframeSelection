//
// Created by Ruili on 17-5-3.
//

#include <iostream>
#include "keyframe_init.h"
#include "keyframe_selection_update.h"
#include "Myserver.h"
#include "classification.h"
#include "filtering.h"



int main(int argc, char** argv)
{
    ::google::InitGoogleLogging(argv[0]);
    string model_file   = "/home/download/caffe-master/models/GoogleNet/deploy.prototxt";
    string trained_file = "/home/download/caffe-master/models/GoogleNet/googlenet_iter_10000.caffemodel";
    string mean_file    = "/home/download/caffe-master/models/GoogleNet/imagenet_mean.binaryproto";
    string label_file   = "/home/download/caffe-master/models/GoogleNet/lable.txt";
    Classifier classifier(model_file, trained_file, mean_file, label_file);


    Myserver myserver;
    myserver.start();

    for (int i = 1; i <= 1; i++) //原来i=12
    {
    	char filename[256];
        char landmark_classification_filename[256];
        vector<int> landmark_index2;
    	sprintf(filename, "/home/TestSet/0209_gogo_dataset/GoPro-vedio/part/testcase%d", i);
        sprintf(landmark_classification_filename, "%s/landmark_classification.txt", filename);
    	printf("Processing testcase %s\n", filename);
        keyframe_init(filename);
    	keyframe_selection_update(filename, myserver, classifier, landmark_classification_filename);
        landmark_index2 = filter(landmark_classification_filename);
        printf("Processing testcase Done***** \n%s\n", filename);
	}


    return 0;
}
