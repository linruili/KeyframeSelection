#include <iostream>
#include "tools.h"

using namespace std;

int main() {
    cout << "Hello, World!" << endl;
    for (int i = 1; i <= 1; i++) //原来i=12
    {
    	char filename[256];
    	sprintf(filename, "/home/TestSet/0209_gogo_dataset/GoPro-vedio/part/testcase%d", i);
    	printf("Processing testcase %s\n", filename);
        keyframe_init(filename);
    	keyframe_selection_update(filename);
        printf("Processing testcase Done***** \n%s\n", filename);
	}
    return 0;
}

/*
int main(int argc, char* argv[]) {
     two arguments
     argv[1] is testcase dir
     argv[2] is rcnn threshold

    if(argc != 3) {
        cout << "with two arguments: testcase_directory threshold\n" << endl;
        return 1;
    }
    读入图片，放入frames, name放入frame_names
    keyframe_init(argv[1], argv[2]);
    keyframe_selection_update(argv[1]);
    printf("Processing testcase Done***** \n%s\n", argv[1]);

    return 0;
}*/
