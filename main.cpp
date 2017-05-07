//
// Created by Ruili on 17-5-3.
//

#include <iostream>
#include "keyframe_init.h"
#include "keyframe_selection_update.h"
#include "Myserver.h"

using namespace std;

int main()
{
    Myserver myserver;
    myserver.start();
//    std::vector<Landmark> result;
//    sleep(5);
//    char image_name[1024] = "/home/TestSet/0209_gogo_dataset/GoPro-vedio/part/testcase1/front/001.jpg#1";
//    string str = image_name;
//    myserver.send_mes(image_name, str.size());
//
//    char keyframe_prob_filename[1024];
//    myserver.rec_mes(keyframe_prob_filename);
//    readFromFile(keyframe_prob_filename, result);
//
//    sleep(5);
//    char image_name2[1024] = "/home/TestSet/0209_gogo_dataset/GoPro-vedio/part/testcase1/front/031.jpg#2";
//    str = image_name;
//    myserver.send_mes(image_name2, str.size());
//
//    myserver.rec_mes(keyframe_prob_filename);
//    readFromFile(keyframe_prob_filename, result);


    for (int i = 1; i <= 2; i++) //Ô­À´i=12
    {
    	char filename[256];
    	sprintf(filename, "/home/TestSet/0209_gogo_dataset/GoPro-vedio/part/testcase%d", i);
    	printf("Processing testcase %s\n", filename);
        keyframe_init(filename);
    	keyframe_selection_update(filename,myserver);
        printf("Processing testcase Done***** \n%s\n", filename);
	}
    return 0;
}
/*
int main(int argc, char* argv[])
{
     two arguments
     argv[1] is testcase dir
     argv[2] is rcnn threshold

    if(argc != 3)
    {
        cout << "with two arguments: testcase_directory threshold\n" << endl;
        return 1;
    }
    keyframe_init(argv[1], argv[2]);
    keyframe_selection_update(argv[1]);
    printf("Processing testcase Done***** \n%s\n", argv[1]);

    return 0;
}*/