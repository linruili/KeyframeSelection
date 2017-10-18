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
#include <sys/timeb.h>
#include <dirent.h>


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
    /*
     * 记得修改constant.h里的jump_length和sampling-----------------------------------------------
     */

    load_landmark_correct();
    char root_path[256] = "/home/TestSet/0209_gogo_dataset";
    vector<string> phones = {"GoPro-vedio", "Mate7-vedio", "Mi4-video"};
    int testcase_num[3] = {12,9,10};
    for(int i=0; i<phones.size(); ++i)
    {
        vector<double> errors;
        vector<long> time_indentify;
        vector<long> time_locate;
        double total_error = 0;
        int error_num = testcase_num[i];
        long total_time_indentify = 0;
        long total_time_locate = 0;
        rcnn_total_time = 0;
        char part_path[256];
        sprintf(part_path, "%s/%s/part", root_path, phones[i].c_str());
        for(int j=1; j<=testcase_num[i]; ++j)
        {
            char testcase_path[256];
            char landmark_classification_filename[256];
            timeb t1,t2,t3;
            sprintf(testcase_path, "%s/testcase%d", part_path, j);
            sprintf(landmark_classification_filename, "%s/landmark_classification.txt", testcase_path);
            printf("Processing testcase %s\n", testcase_path);
            keyframe_init(testcase_path);
            ftime(&t1);
            keyframe_selection_update(testcase_path, myserver, classifier, landmark_classification_filename);
            vector<landmark_info> landmark_sequence = filter(landmark_classification_filename);
            load_landmark_x_y_compass(landmark_sequence, testcase_path);
            ftime(&t2);
            vector_d result = ga_main(landmark_sequence);
            ftime(&t3);

            //compute time
            long t = (t2.time-t1.time)*1000 + t2.millitm-t1.millitm;
            total_time_indentify += t;
            time_indentify.push_back(t);
            cout<<"**time_indentify = "<<t<<endl;
            t = (t3.time-t2.time)*1000 + t3.millitm-t2.millitm;
            total_time_locate += t;
            time_locate.push_back(t);
            cout<<"**time_locate = "<<t<<endl;

            //compute error
            char ground_truth_fileName[256];
            sprintf(ground_truth_fileName, "%s/ground_truth.txt", testcase_path);
            double x,y;
            FILE *ground_truth_file = fopen(ground_truth_fileName, "r");
            fscanf(ground_truth_file, "%lf,%lf", &x, &y);
            fclose(ground_truth_file);
            cout<<"ground truth = "<<x<<" , "<<y<<endl;
            double error = pdist(point_t(x,y), point_t(result[0], result[1]));
            if(error<200)
                total_error += error;
            else
                --error_num;
            errors.push_back(error);
            cout<<"error = "<<error<<endl;
            cout<<"------------------------------------------------------------------------------------------------------------------"<<endl;

            printf("Processing testcase Done***** \n%s\n", testcase_path);
        }

        //write result to file
        cout<<"mean error = "<<total_error/error_num<<" = "<<total_error/error_num/18<<"m"<<endl;
        cout<<"mean time_rcnn = "<<rcnn_total_time/testcase_num[i]<<endl;
        cout<<"mean time_from_rcnn_to_GoogLeNet = "<<total_time_indentify/testcase_num[i]<<endl;
        cout<<"mean time_locate = "<<total_time_locate/testcase_num[i]<<endl;
        char result_filename[256];
        sprintf(result_filename, "%s/result.txt", part_path);
        FILE *result_file = fopen(result_filename, "w");
        for(int i=0; i<errors.size(); ++i)
            fprintf(result_file, "%f %ld %ld\n", errors[i], time_indentify[i], time_locate[i]);
        fclose(result_file);
    }


    return 0;
}
