//
// Created by ruili on 17-12-23.
//

#include "run_rcnn_procedure.h"
#include "constant.h"
#include "tools.h"
#include "Myserver.h"
#include <sys/wait.h>
#include <sys/timeb.h>


void readFromFile(char *keyframe_prob_filename, std::vector<Landmark> &result)
{
    FILE *keyframe_prob_file = fopen(keyframe_prob_filename, "r");
    char frame_name[1024];
    char region_name[1024];
    double prob, x1, y1, x2, y2;
    while (fscanf(keyframe_prob_file, "%s %s %lf %lf %lf %lf %lf ", frame_name, region_name, &prob, &x1, &y1, &x2, &y2) != EOF)
    {
        Landmark landmark;
        landmark.prob = prob;
        cv::Rect rect((int) x1, (int) y1, (int) (x2 - x1), (int) (y2 - y1));
        landmark.rect = rect;
        string region_name_str(region_name);
        vector<string> region_name_path;
        split(region_name_str, "/", region_name_path);
        strcpy(landmark.filename, region_name_path[region_name_path.size() - 1].c_str());
        //printf("landmark filename %s\n rect: %d %d %d %d\n", frame_name, rect.x, rect.y, rect.width, rect.height);
        result.push_back(landmark);
    }
    fclose(keyframe_prob_file);
}

vector<Landmark> run_rcnn_procedure(char *recv_dir, int frame_index, Myserver &myserver)
{
    // simulate rcnn procedure and return regions

    rcnn_counter++;
    printf("run rcnn procedure...rcnn_counter = %d\n", rcnn_counter);


    std::vector<Landmark> result;
    if(constant.rcnn_method == 1)
    {
        //直接从文件读region
    }
    else if(constant.rcnn_method == 2)
    {
        char image_name[1024];
        sprintf(image_name, "%s/%03d.jpg#%d", recv_dir, frame_index, rcnn_counter);
        string str = image_name;

        cout<<"send string = "<<str<<endl;
        timeb t1,t2;
        ftime(&t1);
        myserver.send_mes(image_name, str.size());
        //wait(NULL);
        char keyframe_prob_filename[1024];
        myserver.rec_mes(keyframe_prob_filename);
        ftime(&t2);
        long t = (t2.time-t1.time)*1000+t2.millitm-t1.millitm;
        cout<<"**RCNN runtime = "<<t<<" ms"<<endl;

        readFromFile(keyframe_prob_filename, result);

        //sleep(5);
    }

    return result;
}