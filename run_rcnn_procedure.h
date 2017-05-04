//
// Created by Ruili on 17-5-4.
//

#ifndef KEYFRAMESELECTION_RUN_RCNN_PROCEDURE_H
#define KEYFRAMESELECTION_RUN_RCNN_PROCEDURE_H


#include "constant.h"
#include "keyframe_init.h"
#include "tools.h"

vector<Landmark> run_rcnn_procedure(char *testcase_dir, int frame_index) {
    // simulate rcnn procedure and return regions

    printf("run rcnn procedure...\n");
    rcnn_counter++;

    std::vector<Landmark> result;

    //直接从文件读region
    if(constant.rcnn_method == 1)
    {

        char keyframe_prob_filename[1024];
        sprintf(keyframe_prob_filename, "%s/cache_0.6/%s/prob.txt", testcase_dir,
                get_name_from_frame_index(frame_index).c_str());

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
            printf("landmark filename %s\n rect: %d %d %d %d\n", frame_name, rect.x, rect.y, rect.width, rect.height);
            result.push_back(landmark);
        }

        fclose(keyframe_prob_file);
    }
    else if(constant.rcnn_method == 2)
    {
        char rcnn_result_dir[1024];
        sprintf(rcnn_result_dir, "%s/rcnn_result", testcase_dir);
        mkdir(rcnn_result_dir, 0777);






        char prob_filename[1024];
        sprintf(prob_filename, "%s/rcnn_ground_truth/%s_prob.txt", testcase_dir,
                get_name_from_frame_index(frame_index).c_str());

        char command[1024];
        sprintf(command, "cd /data/UserData/Mingkuan/TestSet/0209_gogo_dataset && java -jar SignboardTag_RCNN.jar %s/front/%s %s",
                testcase_dir, frame_names[frame_index - 1].c_str(), prob_filename);

        printf("Running command %s\n", command);

        // run command
        system(command);

        FILE *keyframe_prob_file = fopen(prob_filename, "r");

        char frame_name[1024];
        char region_name[1024];
        double prob, x1, y1, x2, y2;
        int landmark_id;
        while (fscanf(keyframe_prob_file, "%s %s %lf %lf %lf %lf %lf %d", frame_name, region_name, &prob, &x1, &y1, &x2,
                      &y2, &landmark_id) !=
               EOF)
        {
            Landmark landmark;
            landmark.prob = prob;
            cv::Rect rect((int) x1, (int) y1, (int) (x2 - x1), (int) (y2 - y1));
            landmark.rect = rect;

            landmark.landmark_id = landmark_id;

            string region_name_str(region_name);
            vector<string> region_name_path;
            split(region_name_str, "/", region_name_path);
            strcpy(landmark.filename, region_name_path[region_name_path.size() - 1].c_str());
            printf("landmark filename %s\n rect: %d %d %d %d\n", frame_name, rect.x, rect.y, rect.width, rect.height);
            result.push_back(landmark);
        }

        fclose(keyframe_prob_file);
    }

    return result;
}


#endif //KEYFRAMESELECTION_RUN_RCNN_PROCEDURE_H
