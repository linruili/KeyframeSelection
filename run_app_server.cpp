//
// Created by ruili on 17-12-23.
//

#include "run_app_server.h"
#include "Myserver.h"
#include "classification.h"
#include "tools.h"
#include "keyframe_selection_update.h"
#include "filtering.h"
#include "locator_optimize.h"
#include <sys/timeb.h>
#include <pthread.h>

using namespace std;

void* recv_img_compass(void* args)
{
    char recv_dir[256] = "/home/HAVIL/KeyframeSelection/output/receive";
    timeb t4, t5;
    Myserver app_server(8080);
    app_server.start();
    ftime(&t4);
    app_server.recv_img_compass(recv_dir);
    ftime(&t5);
    long t = (t5.time - t4.time) * 1000 + t5.millitm - t4.millitm;
    cout << "**time_app_tcp = " << t << endl;

    return 0;
}

void run_app_server(char* argv)
{
    ::google::InitGoogleLogging(argv);
    string model_file   = "/home/download/caffe-master/models/GoogleNet/deploy.prototxt";
    string trained_file = "/home/download/caffe-master/models/GoogleNet/googlenet_iter_10000.caffemodel";
    string mean_file    = "/home/download/caffe-master/models/GoogleNet/imagenet_mean.binaryproto";
    string label_file   = "/home/download/caffe-master/models/GoogleNet/lable.txt";
    Classifier classifier(model_file, trained_file, mean_file, label_file);

    char output_dir[256] = "/home/HAVIL/KeyframeSelection/output";
    char result_dir[256];
    sprintf(result_dir, "%s/result", output_dir);
    char recv_dir[256];
    sprintf(recv_dir, "%s/receive", output_dir);

    char landmark_classification_filename[256];
    sprintf(landmark_classification_filename, "%s/landmark_classification.txt", result_dir);
    remove_dir(output_dir);
    mkdir(output_dir, 0777);
    mkdir(result_dir, 0777);

    pthread_t thread = NULL;
    int ret;
    try
    {
        ret = pthread_create(&thread, NULL, recv_img_compass, NULL);
    }
    catch (const std::exception &exc)
    {
        std::cerr << exc.what();
    }
    if (ret != 0)
        cout << "pthread_create error: error_code=" << ret << endl;
//    sleep(15);

    cout<<"isReceiving: "<<isReceiving<<endl;
    Myserver myserver(3333);
    myserver.start();
    /*
     * 记得修改constant.h里的jump_length和sampling-----------------------------------------------
     */
    load_landmark_correct();
    timeb t1, t2, t3;
    ftime(&t1);
    keyframe_selection_update(output_dir, myserver, classifier, landmark_classification_filename);
    vector<landmark_info> landmark_sequence = filter(landmark_classification_filename);
    load_landmark_x_y_compass(landmark_sequence);
    ftime(&t2);
    vector_d result;

    if (landmark_sequence.size() != 0)
        result = simple_weighted(landmark_sequence);
    else
        result = vector_d(2, 0.0);
    ftime(&t3);

    //compute time
    long t = (t2.time - t1.time) * 1000 + t2.millitm - t1.millitm;
    cout << "**time_from_rcnn_to_GoogLeNet = " << t << endl;
    t = (t3.time - t2.time) * 1000 + t3.millitm - t2.millitm;
    t /= ROUND;
    cout << "**time_locate = " << t << endl;


    cout<<"x: "<<result[0]<<" y: "<<result[1]<<endl;

}