//
// Created by Ruili on 17-5-3.
//

#include <iostream>
#include "keyframe_init.h"
#include "keyframe_selection_update.h"
#include "Myserver.h"
#include "classification.h"

#include "LCS.h"



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
//
//    for (int i = 1; i <= 1; i++) //原来i=12
//    {
//    	char filename[256];
//    	sprintf(filename, "/home/TestSet/0209_gogo_dataset/GoPro-vedio/part/testcase%d", i);
//    	printf("Processing testcase %s\n", filename);
//        keyframe_init(filename);
//    	keyframe_selection_update(filename, myserver, classifier);
//        printf("Processing testcase Done***** \n%s\n", filename);
//	}


    FILE *cluster_file = fopen("cluster.txt", "r");
    vector<vector<string>> clusters;
    int num;
    while(fscanf(cluster_file, "%d", &num)!= EOF)
    {
        vector<string> cluster_line;
        int each_landmark;
        for(int i=1; i<=num; ++i)
        {
            fscanf(cluster_file, " %d", &each_landmark);
            cluster_line.push_back(to_string(each_landmark));
        }
        clusters.push_back(cluster_line);
    }

    vector<string> landmark_raw;
//    landmark_raw.push_back("2");
//    landmark_raw.push_back("4");
//    landmark_raw.push_back("50");
    landmark_raw.push_back("1");
    landmark_raw.push_back("3");

    vector<string> result;
    result = LCS(clusters, landmark_raw);
    for(int i=0; i<result.size(); ++i)
        cout<<result[i]<<" ";



    return 0;
}
