//
// Created by ruili on 17-12-23.
//

#include "keyframe_selection_update.h"
#include "tools.h"
#include "run_rcnn_procedure.h"
#include "Myserver.h"
#include "classification.h"
#include "landmark_identification.h"
#include <cmath>
#include <cstdlib>
#include <sys/timeb.h>

int findKeyframe(int &minSpace,int minIndex, int curIndex, cv::Rect rect, int frame_width)
{
    if( abs((rect.x+(rect.x+rect.width))/2 - frame_width/2) < minSpace)
    {
        minSpace = abs((rect.x+(rect.x+rect.width))/2 - frame_width/2);
        return curIndex;
    }
    return minIndex;
}

void keyframe_selection_update(char *output_dir, Myserver &myserver, Classifier &classifier,
                               char *landmark_classification_filename)
{
    /*
     * 所涉及到的目录
     * landmark_dir = result_dir + /landmark
     * landmark_sub_dir = landmark_dir + landmark_count
     *
     * 文件
     * landmark_list_file = landmark_dir + /landmark.txt
     * andmark_classification_filename = landmark_dir + /landmark_classification.txt  [landmarkcount, landmarkID, probability, keyframe]
     * landmark_sub_list_filename = landmark_sub_dir + /landmark_detail.txt
     * landmark_region_filename = landmark_sub_dir + /region_index.jpg
     * */
    cout<<"start keyframe selection update"<<endl;
    timeb t1,t2,t3,t4,t5,t6;
    long identify_total_time = 0;
    ftime(&t1);

    int pre_frame_index = 1;
    int max_frame_index = 1;
    int cur_frame_index = 1;
    int frame_index_begin;
    int frame_index_end;

    KCFTracker tracker;

    //*********************初始化路径和文件*****************************


    char result_dir[256];
    sprintf(result_dir, "%s/result", output_dir);

    char recv_dir[256];
    sprintf(recv_dir, "%s/receive", output_dir);

    char landmark_dir[256];
    sprintf(landmark_dir, "%s/landmark", result_dir);
    remove_dir(landmark_dir);
    mkdir(landmark_dir, 0777);

    char rcnn_result_dir[256];
    sprintf(rcnn_result_dir, "%s/rcnn_result", result_dir);
    remove_dir(rcnn_result_dir);
    mkdir(rcnn_result_dir, 0777);

    char landmark_list_filename[256];
    sprintf(landmark_list_filename, "%s/landmark.txt", landmark_dir);
    FILE *landmark_list_file = fopen(landmark_list_filename, "w");

    FILE *landmark_classification_file = fopen(landmark_classification_filename, "w");

    int landmark_count = 0;

    //^********************初始化路径和文件*****************************

    cout<<"pre_frame_index: "<<pre_frame_index<<endl;
    cout<<"total_frame: "<<total_frame<<endl;
    int tmp = 1;
    while (isReceiving || pre_frame_index < total_frame)
    {
        if(isReceiving && pre_frame_index >= total_frame)
        {
            ++tmp;
            if(tmp%20==0)
            {
                cout<<"pre_frame_index: "<<pre_frame_index<<endl;
                cout<<"total_frame: "<<total_frame<<endl;
            }

            sleep(0.1);
            continue;
        }



        ftime(&t2);

        vector<Landmark> regions = run_rcnn_procedure(recv_dir, pre_frame_index,myserver);
        cout<<"rcnn_frame_index = "<<pre_frame_index<<endl;
        cout<<"rcnn_regions.size() = "<<regions.size()<<endl;
        sort(regions.begin(), regions.end(), landmark_comp);

        cv::Mat pre_frame = frames[pre_frame_index-1];

        int frame_width, frame_height;
        frame_width = pre_frame.cols;
        frame_height = pre_frame.rows;

        int region_index = 0;
        bool is_process = false;

        ftime(&t3);

        while (region_index < regions.size())
        {
            ftime(&t3);
            pre_frame = frames[pre_frame_index-1];
            //过滤掉region在图片下半部分的情况或region在图片最左或最右的
            if (regions[region_index].rect.x < constant.tracker_ignore_start_threshold
                || regions[region_index].rect.y + regions[region_index].rect.height / 2
                   >= frame_height / 2
                || regions[region_index].rect.x + regions[region_index].rect.width
                   > frame_width - constant.tracker_ignore_start_threshold)
            {
                cout<<"rcnn region filtered -_-    rcnn_counter="<<rcnn_counter
                    <<"  region_index="<<region_index<<endl;
                region_index++;
                continue;
            }

            is_process = true;

            cur_frame_index = pre_frame_index;
            tracker.init(regions[region_index].rect, pre_frame);

            cv::Rect pre_rect, update_rect;
            pre_rect = regions[region_index].rect;

            printf("find landmark %03d, region %d\n", ++landmark_count, region_index);
            fprintf(landmark_list_file, "%03d\n", landmark_count);

            char landmark_sub_dir[256];
            sprintf(landmark_sub_dir, "%s/%03d", landmark_dir, landmark_count);
            mkdir(landmark_sub_dir, 0777);

            char landmark_sub_list_filename[256];
            sprintf(landmark_sub_list_filename, "%s/landmark_detail.txt", landmark_sub_dir);
            FILE *landmark_sub_file = fopen(landmark_sub_list_filename, "w");

            int minSpace = INT_MAX;//每帧和正中的距离
            int minIndex = findKeyframe(minSpace, 0, pre_frame_index, pre_rect, frame_width);//keyframe_index


            // record landmark in pre_frame index
            fprintf(landmark_sub_file, "%s.jpg %s %d %d %d %d\n",
                    get_name_from_frame_index(cur_frame_index).c_str(),
                    get_name_from_frame_index(cur_frame_index).c_str(),
                    pre_rect.x, pre_rect.y,
                    pre_rect.x + pre_rect.width, pre_rect.y + pre_rect.height);

            RectTools::limit(pre_rect, frame_width, frame_height);
            //convert Rect to Mat
            cv::Mat ROI = pre_frame(pre_rect);

            char landmark_region_filename[256];
            sprintf(landmark_region_filename, "%s/%s.jpg", landmark_sub_dir,
                    get_name_from_frame_index(cur_frame_index).c_str());
            cv::imwrite(landmark_region_filename, ROI);

            printf("Processing frame %d, start KCF...\n", cur_frame_index);

            cur_frame_index--;

            //向后跟踪
            while (cur_frame_index > 0)
            {
                cv::Mat next_frame = frames[cur_frame_index];
                update_rect = tracker.update(next_frame);

                pre_rect = update_rect;
                pre_frame = next_frame;

                minIndex = findKeyframe(minSpace, minIndex, cur_frame_index, pre_rect, frame_width);

                if (update_rect.x > constant.tracker_ignore_threshold
                    && update_rect.x + update_rect.width
                       < frame_width - constant.tracker_ignore_threshold
                    && update_rect.y + update_rect.height / 2 < frame_height / 2)
                {
                    // record landmark
                    fprintf(landmark_sub_file, "%s.jpg %s %d %d %d %d\n",
                            get_name_from_frame_index(cur_frame_index).c_str(),
                            get_name_from_frame_index(cur_frame_index).c_str(),
                            pre_rect.x, pre_rect.y,
                            pre_rect.x + pre_rect.width, pre_rect.y + pre_rect.height);

                    RectTools::limit(pre_rect, frame_width, frame_height);
                    cv::Mat ROI = pre_frame(pre_rect);

                    char landmark_region_filename[256];
                    sprintf(landmark_region_filename, "%s/%s.jpg", landmark_sub_dir,
                            get_name_from_frame_index(cur_frame_index).c_str());
                    cv::imwrite(landmark_region_filename, ROI);

                } else
                {
                    break;
                }
                cur_frame_index--;
            }
            frame_index_begin = cur_frame_index+1;
            printf("backward to frame %d\n", cur_frame_index+1);

            cur_frame_index = pre_frame_index + 1;
            pre_frame = frames[pre_frame_index];
            tracker.init(regions[region_index].rect, pre_frame);

            //向前跟踪
            while (isReceiving || cur_frame_index < total_frame)
            {
                if(isReceiving && cur_frame_index >= total_frame)
                {
                    sleep(0.01);
                    continue;
                }

                cv::Mat next_frame = frames[cur_frame_index];
                update_rect = tracker.update(next_frame);

                pre_rect = update_rect;
                pre_frame = next_frame;

                minIndex = findKeyframe(minSpace, minIndex, cur_frame_index, pre_rect, frame_width);

                if (update_rect.x > constant.tracker_ignore_threshold
                    && update_rect.x + update_rect.width
                       < frame_width - constant.tracker_ignore_threshold
                    && update_rect.y + update_rect.height / 2 < frame_height / 2)
                {
                    // record landmark
                    fprintf(landmark_sub_file, "%s.jpg %s %d %d %d %d\n",
                            get_name_from_frame_index(cur_frame_index).c_str(),
                            get_name_from_frame_index(cur_frame_index).c_str(),
                            pre_rect.x, pre_rect.y,
                            pre_rect.x + pre_rect.width, pre_rect.y + pre_rect.height);

                    RectTools::limit(pre_rect, frame_width, frame_height);
                    cv::Mat ROI = pre_frame(pre_rect);

                    char landmark_region_filename[256];
                    sprintf(landmark_region_filename, "%s/%s.jpg", landmark_sub_dir,
                            get_name_from_frame_index(cur_frame_index).c_str());
                    cv::imwrite(landmark_region_filename, ROI);

                } else
                {
                    break;
                }
//                cout<<"start forward to frame"<<endl;
//                cout<<"cur_frame_index: "<<cur_frame_index<<endl;
                cur_frame_index++;
            }
            frame_index_end = cur_frame_index-1;
            printf("forward to frame %d\n", cur_frame_index-1);

            fclose(landmark_sub_file);

            if (max_frame_index < cur_frame_index)
                max_frame_index = cur_frame_index;

            region_index++;

            ftime(&t4);
            long t;
            t = (t4.time-t3.time)*1000 + t4.millitm-t3.millitm;
            cout<<"**time for KCF = "<<t<<endl;
            if(frame_index_end-frame_index_begin != 0)
                cout<<"**time for KCF per frame = "<<t / (frame_index_end-frame_index_begin)<<endl;

            //start landmark identify

            cout<<"frame_index_beigin "<<frame_index_begin<<" "<<"frame_index_end "<<frame_index_end<<endl;
            int load_index;
            vector<cv::Mat> imgs;
            char predictFile[256];
            for(int i=1; i<=10; ++i)
            {
                load_index = (frame_index_end - frame_index_begin)/10*i+frame_index_begin;
                sprintf(predictFile, "%s/%s.jpg", landmark_sub_dir, get_name_from_frame_index(load_index).c_str());
                cv::Mat predictImg = cv::imread(predictFile, -1);
                CHECK(!predictImg.empty()) << "Unable to decode image " << predictFile;
                imgs.push_back(predictImg);
            }
            Prediction predict_landmark = landmark_identify(imgs, classifier);

            ftime(&t5);
            t = (t5.time-t4.time)*1000 + t5.millitm-t4.millitm;
            cout<<"**time for identifying = "<<t<<endl;
            identify_total_time += t;

            // record landmark_classification
            printf("------predict for landmark %d -- %d -- %f\n", landmark_count, atoi(predict_landmark.first.c_str())+1, predict_landmark.second);

            int predict_landmark_ID = atoi(predict_landmark.first.c_str())+1;
            if(predict_landmark_ID==0)
                predict_landmark_ID = -1;
            fprintf(landmark_classification_file, "%d %d %f %d\n",
                    landmark_count, predict_landmark_ID, predict_landmark.second, minIndex);

        }

        if (is_process)
            pre_frame_index = max_frame_index + 1;
        else
            pre_frame_index += constant.tracker_jump_length;

    }

    ftime(&t6);
    cout<<"**time for keyframe_selection_update = "<<(t6.time-t1.time)*1000 + t6.millitm-t1.millitm<<endl;
    cout<<"final RCNN counter = "<<rcnn_counter<<endl;
    cout<<"**identify_total_time = "<<identify_total_time<<endl;

    fprintf(landmark_list_file, "RCNN counter: %d\n", rcnn_counter);
    rcnn_counter = 0;
    frame_names.clear();
    frames.clear();
    fclose(landmark_list_file);
    fclose(landmark_classification_file);
    return;
}