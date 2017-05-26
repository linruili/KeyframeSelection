//
// Created by Ruili on 17-5-3.
//

#include <iostream>
#include "keyframe_init.h"
#include "keyframe_selection_update.h"
#include "Myserver.h"
#include "classification.h"
#include "filtering.h"
#include "locator_optimize.h"



int main(int argc, char** argv)
{
//    ::google::InitGoogleLogging(argv[0]);
//    string model_file   = "/home/download/caffe-master/models/GoogleNet/deploy.prototxt";
//    string trained_file = "/home/download/caffe-master/models/GoogleNet/googlenet_iter_10000.caffemodel";
//    string mean_file    = "/home/download/caffe-master/models/GoogleNet/imagenet_mean.binaryproto";
//    string label_file   = "/home/download/caffe-master/models/GoogleNet/lable.txt";
//    Classifier classifier(model_file, trained_file, mean_file, label_file);
//
//
//    Myserver myserver;
//    myserver.start();

//    for (int i = 1; i <= 1; i++) //原来i=12
//    {
//    	char filename[256];
//        char landmark_classification_filename[256];
//        vector<int> final_index;
//    	sprintf(filename, "/home/TestSet/0209_gogo_dataset/GoPro-vedio/part/testcase%d", i);
//        sprintf(landmark_classification_filename, "%s/landmark/landmark_classification.txt", filename);
//    	printf("Processing testcase %s\n", filename);
//        //keyframe_init(filename);
//    	//keyframe_selection_update(filename, myserver, classifier, landmark_classification_filename);
//        final_index = filter(landmark_classification_filename);
//
//        for(int j=0; j<final_index.size(); ++j)
//            cout<<final_index[j]<<' ';
//        printf("Processing testcase Done***** \n%s\n", filename);
//	}

    vector_d match_result_x {100.0, 100.0, 0.0};
    vector_d match_result_y {0.0, 100.0, 100.0};
    vector_d compass_reading {0.0, 45.0, 90.0};
    vector_d result = ga_main(match_result_x,match_result_y,compass_reading);


    return 0;
}
