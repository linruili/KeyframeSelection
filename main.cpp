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

#define ROUND 10

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

    for (int i = 0; i < i<phones.size(); ++i)
    {
        char part_path[256];
        sprintf(part_path, "%s/%s/part", root_path, phones[i].c_str());
        for (int j = 1; j <= final_result.testcaseNum[i]; ++j) {
            char testcase_path[256];
            char landmark_classification_filename[256];
            timeb t1, t2, t3;
            sprintf(testcase_path, "%s/testcase%d", part_path, j);
            sprintf(landmark_classification_filename, "%s/landmark_classification.txt", testcase_path);
            printf("Processing testcase %s\n", testcase_path);
            keyframe_init(testcase_path, i);
            ftime(&t1);
            keyframe_selection_update(testcase_path, myserver, classifier, landmark_classification_filename, i);
            vector<landmark_info> landmark_sequence = filter(landmark_classification_filename);
            load_landmark_x_y_compass(landmark_sequence, testcase_path);
            ftime(&t2);
            vector_d result;

            if (landmark_sequence.size() != 0)
            {
                /*
                vector< vector_d > tmp(ROUND,vector<double>());//-----------------------------------
                for(int k=0; k<ROUND; ++k)
                    tmp[k] = ga_main(landmark_sequence);
                result = tmp[0];
                for(int k=1; k<ROUND; ++k)
                    for(int w=0; w<result.size(); ++w)
                        result[w] += tmp[k][w];
                for(int w=0; w<result.size(); ++w)
                    result[w] /= ROUND;*/

                result = simple_weighted(landmark_sequence);
            }
            else
                result = vector_d(2, 0.0);
            ftime(&t3);
                    
            //compute time
            long t = (t2.time - t1.time) * 1000 + t2.millitm - t1.millitm;
            cout << "**time_from_rcnn_to_GoogLeNet = " << t << endl;
            t = (t3.time - t2.time) * 1000 + t3.millitm - t2.millitm;
            t /= ROUND; //---------------------------------------------------------
            final_result.locate_total_time[i] += t;
            cout << "**time_locate = " << t << endl;
            t += (t2.time - t1.time) * 1000 + t2.millitm - t1.millitm;
            final_result.total_time[i] += t;
                    
            //compute error
            char ground_truth_fileName[256];
            sprintf(ground_truth_fileName, "%s/ground_truth.txt", testcase_path);
            double x, y;
            FILE *ground_truth_file = fopen(ground_truth_fileName, "r");
            fscanf(ground_truth_file, "%lf,%lf", &x, &y);
            fclose(ground_truth_file);
            cout << "ground truth = " << x << " , " << y << endl;
            double error = pdist(point_t(x, y), point_t(result[0], result[1])) / 18;
            final_result.errors[i].push_back(error);
            if (error < 10)
            {
                final_result.not_nan_error_num[i]++;
                final_result.total_error[i] += error;
            } else
                cout << "error>10!" << endl;
            cout << "error = " << error << endl;
            cout << "----------------------------------------------------------------------------------"
                    << endl;
                    
            printf("Processing testcase Done***** \n%s\n", testcase_path);
        }
                    
        //write error result to file
        char result_filename[256];
        sprintf(result_filename, "%s/result.txt", part_path);
        FILE *result_file = fopen(result_filename, "w");
        for (int j = 0; j < final_result.testcaseNum[i]; ++j)
            fprintf(result_file, "%ld\n", final_result.errors[i][j]);
        fclose(result_file);
    }
    for (int i = 0; i < i<phones.size(); ++i)
    {
        cout << "result of " << phones[i] << endl;
        cout << "error<10m number = " << final_result.not_nan_error_num[i] << endl;
        cout << "mean error = " << final_result.total_error[i] / final_result.not_nan_error_num[i] << endl;
        cout << "mean frame number = " << final_result.totalFrameNum[i] / final_result.testcaseNum[i] << endl;
        cout << "mean load time = " << final_result.load_time[i] / final_result.testcaseNum[i] << endl;
        cout << "mean kcf time = " << final_result.kcf_total_time[i] / final_result.testcaseNum[i] << endl;
        cout << "mean time_rcnn = " << final_result.rcnn_total_time[i] / final_result.testcaseNum[i] << endl;
        cout << "mean rcnn number = " << (double) final_result.rcnn_total_num[i] / final_result.testcaseNum[i]
             << endl;
        cout << "mean time GoogLeNet = " << final_result.GoogLeNet_total_time[i] / final_result.testcaseNum[i]
             << endl;
        cout << "mean time locate = " << final_result.locate_total_time[i] / final_result.testcaseNum[i] << endl;
        cout << "mean time for each testcase = " << final_result.total_time[i] / final_result.testcaseNum[i]
             << endl;
        cout << endl;
    }

    return 0;
}
