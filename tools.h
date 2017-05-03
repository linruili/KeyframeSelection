//
// Created by Mingkuan Li on 16/9/10.
//

#ifndef EDGEDETECTION_TOOLS_H
#define EDGEDETECTION_TOOLS_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <string>
#include <ctime>
#include <kcftracker.hpp>
#include <recttools.hpp>
#include <cstdarg>
#include <fstream>
#include <algorithm>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <regex>
#include "constant.h"

using namespace std;

Constant constant;
int rcnn_counter = 0;

void split(const string &src, const string &separator, vector<string> &dest) {
    string str = src;
    string substring;
    string::size_type start = 0, index;

    do {
        index = str.find_first_of(separator, start);
        if (index != string::npos) {
            substring = str.substr(start, index - start);
            dest.push_back(substring);
            start = str.find_first_not_of(separator, index);
            if (start == string::npos) return;
        }
    } while (index != string::npos);

    //the last token
    substring = str.substr(start);
    dest.push_back(substring);
}

bool landmark_comp(Landmark a, Landmark b) {
    return a.rect.x + a.rect.width / 2 < b.rect.x + b.rect.width / 2;
}

bool landmark_comp_reverse(Landmark a, Landmark b) {
    return a.rect.x + a.rect.width / 2 > b.rect.x + b.rect.width / 2;
}

vector<cv::Mat> frames;
vector<string> frame_names;

cv::Mat load_frame(char *testcase_dir, int frame_index) {
    assert(frames.size() != 0);
    // printf("load frame #%d\n", frame_index);

    // frame index is begin with 1;
    return frames[frame_index - 1];
}

string get_name_from_frame_index(int frame_index) {
    assert(frame_names.size() != 0);

    char image_name[256];
    strcpy(image_name, frame_names[frame_index - 1].c_str());
    long length = strlen(image_name);
    image_name[length - 4] = '\0';

    string result = image_name;
    return result;
}

vector<Landmark> run_rcnn_procedure(char *testcase_dir, int frame_index) {
    // simulate rcnn procedure and return regions

    printf("run rcnn procedure...\n");
    rcnn_counter++;

    std::vector<Landmark> result;

    //???????
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
               EOF) {
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

int frame_counter(char *path) {
    DIR *dir;
    struct dirent *ent;
    int count = 0;

    dir = opendir(path);

    // printf("In frame counter %s\n", path);
    regex pattern(".*\\.jpg");

    while ((ent = readdir(dir)) != NULL) {
        // printf("Type %d\n", ent->d_type);
        // if (ent->d_type == DT_REG) {
        //     printf("count file %s\n", ent->d_name);
        string str(ent->d_name);
        if (regex_match(str, pattern)) {
            count++;
        }
        // }
    }

    return count;
}

/****
int calculate_rotation_direction(char *testcase_dir, int max, int &frame_width, int &frame_height) {
    KCFTracker tracker;

    frame_width = frame_height = 0;

    int init_frame_index = 20;
    vector<Landmark> regions;
    cv::Mat init;
    do {
        init_frame_index += 10;
        if(init_frame_index > max - 10) {
            return -1;
        }
        init = load_frame(testcase_dir, init_frame_index);
        regions = run_rcnn_procedure(testcase_dir, init_frame_index);
    } while(regions.size() == 0);

    int cur_next = 10;
    int result = 0;

    int next_frame_index = max > cur_next + init_frame_index ? cur_next + init_frame_index : max;

    cv::Mat final = load_frame(testcase_dir, next_frame_index);

    tracker.init(regions[0].rect, init);
    cv::Rect rect = tracker.update(final);

    if (rect.x > regions[0].rect.x) {
        result = 0;
    } else if (rect.x < regions[0].rect.x) {
        result = 1;
    }

    frame_width = init.cols;
    frame_height = init.rows;

    if(result == 0) {
        printf("left to right\n");
    } else {
        printf("right to left\n");
    }

    // 0 - left to right
    // 1 - right to left
    return result;
} */

void show_image_with_rect(cv::Mat _image, cv::Rect rect) {
    cv::Mat image = _image.clone();

    cv::rectangle(image, rect, cv::Scalar(0, 0, 255, 255));
    cv::imshow("something", image);
    cv::waitKey(0);

    return;
}

void remove_dir(char *path) {

    struct dirent *entry = NULL;
    DIR *dir = NULL;
    dir = opendir(path);
    if (dir) {
        printf("Remove %s\n", path);
        while (entry = readdir(dir)) {
            DIR *sub_dir = NULL;
            FILE *file = NULL;
            char abs_path[1024] = {0};
            if (*(entry->d_name) != '.') {
                sprintf(abs_path, "%s/%s", path, entry->d_name);
                if (sub_dir = opendir(abs_path)) {
                    closedir(sub_dir);
                    remove_dir(abs_path);
                } else {
                    if (file = fopen(abs_path, "r")) {
                        fclose(file);
                        remove(abs_path);
                    }
                }
            }
        }
        remove(path);
    }
}

/******
void keyframe_selection(char *testcase_dir_name) {
    int pre_frame_index = 1;
    int max_frame_index = 1;
    int cur_frame_index = 1;
    KCFTracker tracker;

    char front_dir[256];
    sprintf(front_dir, "%s/front", testcase_dir_name);
    int total_frame = frame_counter(front_dir);

    // mkdir store landmark list
    char landmark_dir[256];
    sprintf(landmark_dir, "%s/landmark", testcase_dir_name);
    remove_dir(landmark_dir);
    mkdir(landmark_dir, 0777);

    char landmark_list_filename[256];
    sprintf(landmark_list_filename, "%s/landmark.txt", landmark_dir);
    FILE *landmark_list_file = fopen(landmark_list_filename, "w");
    int landmark_count = 0;

    printf("front dir: %s \nlandmark dir: %s \nlandmark_list_filename: %s\n", front_dir, landmark_dir,
           landmark_list_filename);

    // 0 - left to right && 1 - right to left
    int frame_width, frame_height;
    int rotation_direction = calculate_rotation_direction(testcase_dir_name, total_frame, frame_width, frame_height);

    if (rotation_direction == -1) {
        fprintf(landmark_list_file, "Something error!!!\n");
        total_frame = 0;
    }

    // printf("in frame %d\n", total_frame);
    // process left to right situation
    while (pre_frame_index < total_frame) {
        // printf("in frame %d\n", pre_frame_index);
        vector<Landmark> regions = run_rcnn_procedure(testcase_dir_name, pre_frame_index);
        if (rotation_direction == 0) {
            sort(regions.begin(), regions.end(), landmark_comp);
        } else {
            sort(regions.begin(), regions.end(), landmark_comp_reverse);
        }
        cv::Mat pre_frame = load_frame(testcase_dir_name, pre_frame_index);

        // find the tracker regions
        int region_index = 0;
        bool is_process = false;

        while (region_index < regions.size()) {

            if (((regions[region_index].rect.x + regions[region_index].rect.width / 2
                  >= frame_width / 2 + constant.tracker_ignore_threshold
                  || regions[region_index].rect.y + regions[region_index].rect.height / 2
                     >= frame_height / 2
                  || regions[region_index].rect.x < constant.tracker_ignore_start_threshold) && rotation_direction == 0)
                || ((regions[region_index].rect.x + regions[region_index].rect.width / 2
                     < frame_width / 2 - constant.tracker_ignore_threshold
                     || regions[region_index].rect.y + regions[region_index].rect.height / 2
                        >= frame_height / 2
                     || regions[region_index].rect.x + regions[region_index].rect.width
                        > frame_width - constant.tracker_ignore_start_threshold) && rotation_direction == 1)) {
                region_index++;
                continue;
            }

            is_process = true;
            // track this region
            cur_frame_index = pre_frame_index;
            tracker.init(regions[region_index].rect, pre_frame);
            cv::Rect pre_rect, update_rect;
            pre_rect = regions[region_index].rect;

            // record landmark
            printf("find landmark %03d\n", ++landmark_count);
            fprintf(landmark_list_file, "%03d\n", landmark_count);

            char landmark_sub_dir[256];
            sprintf(landmark_sub_dir, "%s/%03d", landmark_dir, landmark_count);
            mkdir(landmark_sub_dir, 0777);

            char landmark_sub_list_filename[256];
            sprintf(landmark_sub_list_filename, "%s/landmark_detail.txt", landmark_sub_dir);
            FILE *landmark_sub_file = fopen(landmark_sub_list_filename, "w");

            do {
                // record landmark
                fprintf(landmark_sub_file, "%03d.jpg %03d %d %d %d %d\n",
                        cur_frame_index, cur_frame_index, pre_rect.x, pre_rect.y,
                        pre_rect.x + pre_rect.width, pre_rect.y + pre_rect.height);

                RectTools::limit(pre_rect, frame_width, frame_height);
                cv::Mat ROI = pre_frame(pre_rect);

                char landmark_region_filename[256];
                sprintf(landmark_region_filename, "%s/%03d.jpg", landmark_sub_dir, cur_frame_index);
                cv::imwrite(landmark_region_filename, ROI);

                printf("Processing frame %d\n", cur_frame_index);

                cur_frame_index++;

                cv::Mat next_frame = load_frame(testcase_dir_name, cur_frame_index);
                update_rect = tracker.update(next_frame);

                pre_rect = update_rect;
                pre_frame = next_frame;

                // show_image_with_rect(next_frame, update_rect);
            } while (cur_frame_index < total_frame
                     && (((update_rect.x + update_rect.width < frame_width - constant.tracker_threshold) &&
                          rotation_direction == 0)
                         || ((update_rect.x > constant.tracker_threshold) && rotation_direction == 1)));

            fclose(landmark_sub_file);

            if (max_frame_index < cur_frame_index) {
                max_frame_index = cur_frame_index;
            }
            region_index++;
        }

        if (is_process) {
            pre_frame_index = max_frame_index + 1;
        } else {
            pre_frame_index += 10;

        }
    }


    fprintf(landmark_list_file, "RCNN counter: %d\n", rcnn_counter);
    rcnn_counter = 0;
    fclose(landmark_list_file);
    return;
}
*/

void keyframe_selection_update(char *testcase_dir_name) {
    int pre_frame_index = 1;
    int max_frame_index = 1;
    int cur_frame_index = 1;

    KCFTracker tracker;

    char front_dir[256];
    sprintf(front_dir, "%s/front", testcase_dir_name);
    int total_frame = frame_counter(front_dir);

    // mkdir store landmark list
    char landmark_dir[256];
    sprintf(landmark_dir, "%s/landmark", testcase_dir_name);
    remove_dir(landmark_dir);
    mkdir(landmark_dir, 0777);

    char landmark_list_filename[256];
    sprintf(landmark_list_filename, "%s/landmark.txt", landmark_dir);
    FILE *landmark_list_file = fopen(landmark_list_filename, "w");
    int landmark_count = 0;

    printf("front dir: %s \nlandmark dir: %s \nlandmark_list_filename: %s\n", front_dir, landmark_dir,
           landmark_list_filename);

    while (pre_frame_index < total_frame) {
        vector<Landmark> regions = run_rcnn_procedure(testcase_dir_name, pre_frame_index);
        sort(regions.begin(), regions.end(), landmark_comp);

        cv::Mat pre_frame = load_frame(testcase_dir_name, pre_frame_index);

        int frame_width, frame_height;
        frame_width = pre_frame.cols;
        frame_height = pre_frame.rows;

        int region_index = 0;
        bool is_process = false;

        while (region_index < regions.size()) {
                //过滤掉region在图片下半部分的情况
            if ((regions[region_index].rect.x < constant.tracker_ignore_start_threshold
                 || regions[region_index].rect.y + regions[region_index].rect.height / 2
                    >= frame_height / 2
                 || regions[region_index].rect.x + regions[region_index].rect.width
                    > frame_width - constant.tracker_ignore_start_threshold)) {
                region_index++;
                continue;
            }

            is_process = true;

            cur_frame_index = pre_frame_index;
            tracker.init(regions[region_index].rect, pre_frame);

            cv::Rect pre_rect, update_rect;
            pre_rect = regions[region_index].rect;

            printf("find landmark %03d\n", ++landmark_count);
            fprintf(landmark_list_file, "%03d\n", landmark_count);

            char landmark_sub_dir[256];
            sprintf(landmark_sub_dir, "%s/%03d", landmark_dir, landmark_count);
            mkdir(landmark_sub_dir, 0777);

            char landmark_sub_list_filename[256];
            sprintf(landmark_sub_list_filename, "%s/landmark_detail.txt", landmark_sub_dir);
            FILE *landmark_sub_file = fopen(landmark_sub_list_filename, "w");

            // record landmark in pre_frame index
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

            printf("Processing frame %d\n", cur_frame_index);

            cur_frame_index--;
            //向前跟踪
            while (cur_frame_index > 0) {
                cv::Mat next_frame = load_frame(testcase_dir_name, cur_frame_index);
                update_rect = tracker.update(next_frame);

                pre_rect = update_rect;
                pre_frame = next_frame;

                if (update_rect.x > constant.tracker_ignore_threshold
                    && update_rect.x + update_rect.width
                       < frame_width - constant.tracker_ignore_threshold
                    && update_rect.y + update_rect.height / 2 < frame_height / 2) {
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

                    printf("Processing frame %d\n", cur_frame_index);
                } else {
                    break;
                }

                cur_frame_index--;
            }

            cur_frame_index = pre_frame_index + 1;
            pre_frame = load_frame(testcase_dir_name, pre_frame_index);
            tracker.init(regions[region_index].rect, pre_frame);

            //向后跟踪
            while (cur_frame_index <= total_frame) {
                cv::Mat next_frame = load_frame(testcase_dir_name, cur_frame_index);
                update_rect = tracker.update(next_frame);

                pre_rect = update_rect;
                pre_frame = next_frame;

                if (update_rect.x > constant.tracker_ignore_threshold
                    && update_rect.x + update_rect.width
                       < frame_width - constant.tracker_ignore_threshold
                    && update_rect.y + update_rect.height / 2 < frame_height / 2) {
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

                    printf("Processing frame %d\n", cur_frame_index);
                } else {
                    break;
                }

                cur_frame_index++;
            }

            fclose(landmark_sub_file);

            if (max_frame_index < cur_frame_index) {
                max_frame_index = cur_frame_index;
            }

            region_index++;
        }

        if (is_process) {
            pre_frame_index = max_frame_index + 1;
        } else {
            pre_frame_index += constant.tracker_jump_length;
        }
    }

    fprintf(landmark_list_file, "RCNN counter: %d\n", rcnn_counter);
    rcnn_counter = 0;
    frame_names.clear();
    frames.clear();
    fclose(landmark_list_file);
    return;
}

void keyframe_init(char *filename){
    rcnn_counter = 0;

    char front_dir_name[256];
    sprintf(front_dir_name, "%s/front/", filename);

    int frame_count = 0;

    vector<int> frames_index;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(front_dir_name)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") != 0 &&
                strcmp(ent->d_name, "..") != 0) {
                frame_count++;
                char image_name[256];
                strcpy(image_name, ent->d_name);
                long length = strlen(image_name);
                image_name[length - 4] = '\0';
                frames_index.push_back(atoi(image_name));
            }
        }
    }
    closedir(dir);

    sort(frames_index.begin(), frames_index.end());

    for(int index = 0; index < frame_count; index++) {
        int frame_index = frames_index[index];
        char image_name[256];
        sprintf(image_name, "%s/%03d.jpg", front_dir_name, frame_index);
        cv::Mat image = cv::imread(image_name, CV_LOAD_IMAGE_COLOR);
        frames.push_back(image);

        char filename[256];
        sprintf(filename, "%03d.jpg", frame_index);

        string name = filename;
        frame_names.push_back(name);

        printf("Loaded frame %d\n", frame_names.size());
    }

//    char rcnn_dir[256];
//    sprintf(rcnn_dir, "%s/rcnn_ground_truth", filename);
//    remove_dir(rcnn_dir);
//    mkdir(rcnn_dir, 0777);
}


double *load_image(const char *filename, int &rows, int &cols) {
    cv::Mat image_mat = cv::imread(filename, CV_LOAD_IMAGE_GRAYSCALE);

    rows = image_mat.rows;
    cols = image_mat.cols;

    double *image;
    image = new double[image_mat.rows * image_mat.cols];
    for (int i = 0; i < image_mat.rows; i++) {
        for (int j = 0; j < image_mat.cols; j++) {
            image[i * cols + j] = image_mat.at<uchar>(i, j);
        }
    }

    return image;
}

double *mat_to_array(cv::Mat image_mat, int &rows, int &cols) {
    rows = image_mat.rows;
    cols = image_mat.cols;

    double *image;
    image = new double[image_mat.rows * image_mat.cols];
    for (int i = 0; i < image_mat.rows; i++) {
        for (int j = 0; j < image_mat.cols; j++) {
            image[i * cols + j] = image_mat.at<uchar>(i, j);
        }
    }

    return image;
}

void show_image(double *image, int rows, int cols) {
    cv::Mat image_mat = cv::Mat(rows, cols, CV_64F, image);
    cv::Mat dst;
    cv::normalize(image_mat, dst, 0, 1, cv::NORM_MINMAX);
    cv::imshow("Image", dst);
}

void load_video(const char *video_name, const char *output_video, const char *main_frame_directory) {
    int main_frame_count = 0;

    KCFTracker tracker;

    cv::VideoCapture cap(video_name);
    if (!cap.isOpened()) {
        std::cout << "Unable to open the camera\n";
        std::exit(1);
    }

    int width = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_HEIGHT));

    double FPS = cap.get(CV_CAP_PROP_FPS);
    bool rotate = false;

    printf("width: %d, height: %d, FPS: %f\n", width, height, FPS);

    if (width > height) {
        // rotate image
        width = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_HEIGHT));
        height = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_WIDTH));
        rotate = true;
    }

    int target_width = 600, target_height = 600 * height / width;

    cv::VideoWriter out(output_video, CV_FOURCC('m', 'p', '4', 'v'), FPS, cv::Size(target_width, target_height));
    if (!out.isOpened()) {
        std::cout << "Error! Unable to open video file for output." << std::endl;
        std::exit(-1);
    }

    cv::Mat frame;
    int rows, cols;
    int counter = 0;

    cap >> frame;
    tracker.init(cv::Rect(385.2500, 500.7500, 180.0000, 375.0000), frame);

    while (true) {
        cap >> frame;

        if (frame.empty()) {
            std::cout << "Can't read frames from your camera\n";
            break;
        }

        cv::Rect result = tracker.update(frame);
        cv::rectangle(frame, result, cv::Scalar(0, 0, 255, 255));
        cv::imshow("something", frame);
        cv::waitKey(0);
        // out << frame;
    }
    return;
}

std::vector<std::vector<double> > load_points(const char *filename) {
    std::vector<std::vector<double> > points;
    FILE *fp = fopen(filename, "r");
    char line[50];
    while (fgets(line, sizeof(line), fp) != NULL) {
        double x, y;
        char *x_str = line;
        char *y_str = line;
        while (*y_str != '\0') {
            if (*y_str == ',') {
                *y_str++ = 0;
                x = atof(x_str);
                y = atof(y_str);
                std::vector<double> point;
                point.push_back(x);
                point.push_back(y);
                points.push_back(point);
                break;
            }
            ++y_str;
        }
    }
    fclose(fp);
    return points;
}

void print_points(std::vector<std::vector<double> > points) {
    for (int i = 0; i < points.size(); i++) {
        for (int dim = 0; dim < points[i].size(); dim++) {
            printf("%f ", points[i][dim]);
        }
        printf("\n");
    }
}

void extract_videos(const char *video_filename) {

    // read config file
    char testcase_config[256];
    sprintf(testcase_config, "%s/config.txt", video_filename);
    std::ifstream file(testcase_config);
    std::string str;
    std::getline(file, str);
    assert(str[0] == '#');
    std::string type;
    file >> type;
    std::getline(file, str);
    std::getline(file, str);
    assert(str[0] == '#');
    std::string filename;
    file >> filename;
    std::getline(file, str);
    std::getline(file, str);
    assert(str[0] == '#');
    std::string output;
    file >> output;
    std::getline(file, str);
    char output_name[256];
    sprintf(output_name, "%s/%s", video_filename, output.c_str());

    remove_dir(output_name);
    mkdir(output_name, 0777);
    char compass_name[256];
    sprintf(compass_name, "%s/compass.txt", video_filename);
    char video_name[256];
    sprintf(video_name, "%s/%s", video_filename, filename.c_str());
    assert(strcmp(type.c_str(), "video") == 0);
    printf("%s\n%s\n%s\n", video_name, output_name, compass_name);

    // process compass and video;
    FILE *compass_file = fopen(compass_name, "r");
    if (compass_file == NULL) {
        std::cout << "Faile to access compass in testcase " << video_filename << std::endl;
        return;
    }
    cv::VideoCapture cap(video_name);

    if (!cap.isOpened()) {
        std::cout << "Unable to open the camera." << std::endl;
        std::cout << "Faile to process testcase " << video_filename << std::endl;
        return;
    }

    int rotate;

    char rotation_file_name[256];
    sprintf(rotation_file_name, "%s/rotation.txt", video_filename);
    FILE *rotation_file = fopen(rotation_file_name, "r");

    fscanf(rotation_file, "%d", &rotate);
    fclose(rotation_file);

    int width, height;

    if (rotate == 2) {
        width = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_WIDTH));
        height = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_HEIGHT));
    } else if (rotate == 1) {
        width = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_HEIGHT));
        height = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_WIDTH));
    } else {
        printf("Error roation !!!\n");
    }

    double FPS = cap.get(CV_CAP_PROP_FPS);

    printf("width: %d, height: %d, FPS: %f\n", width, height, FPS);

    cv::Mat frame;
    std::vector<cv::Mat> frames_vec;
    std::vector<double> compass_vec;

    long compass_timestamp;
    double compass_angle;

    fscanf(compass_file, "%ld %lf", &compass_timestamp, &compass_angle);

    int counter = 0;
    while (true) {
        cap >> frame;

        // printf("processing frame %d\n", ++counter);

        if (frame.empty()) {
            std::cout << "Can't read frame from your camera" << std::endl;
            break;
        }

        int target_width, target_height;
        if (rotate == 1) {
            // rotate frame
            cv::transpose(frame, frame);
            cv::flip(frame, frame, 1);
            target_width = 300;
            target_height = target_width * height / width;
        } else {
            target_height = 300;
            target_width = target_height * width / height;
        }


        // resize frame
        cv::Size size(target_width, target_height);
        cv::Mat color_target;
        cv::resize(frame, color_target, size);

        double timestamp = cap.get(CV_CAP_PROP_POS_MSEC);

        double cur_compass_angle = compass_angle;
        double cur_compass_count = 1;

        while (compass_timestamp < timestamp) {
            if (fscanf(compass_file, "%ld %lf", &compass_timestamp, &compass_angle) == EOF) {
                break;
            }
            cur_compass_angle += compass_angle;
            cur_compass_count++;
        }

        compass_vec.push_back(cur_compass_angle / cur_compass_count);

        frames_vec.push_back(color_target);
    }

    // write frames and compass to file
    char compass_output_filename[256];
    sprintf(compass_output_filename, "%s/compass.txt", output_name);
    FILE *compass_output_file = fopen(compass_output_filename, "w");
    for (int i = 0; i < frames_vec.size(); i++) {
        char output_filename[256];
        sprintf(output_filename, "%s/%03d.jpg", output_name, i + 1);

        fprintf(compass_output_file, "%s %f\n", output_filename, compass_vec[i]);

        cv::imwrite(output_filename, frames_vec[i]);
    }

    fclose(compass_output_file);
    fclose(compass_file);
    file.close();
    cap.release();
}


#endif //EDGEDETECTION_TOOLS_H
