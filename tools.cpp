//
// Created by ruili on 17-12-23.
//
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <string>
#include <ctime>
#include <cmath>
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
#include "tools.h"
#include "Myserver.h"
#include <sys/timeb.h>

int buffToInteger(char* buffer)
{
    int a = (int)(buffer[0]<<24 | (buffer[1]&0xFF)<<16 | (buffer[2]&0xFF)<<8 | (buffer[3]&0xFF));
    return a;
}

void load_landmark_compass(vector<landmark_info> &landmark_sequence)
{
    timeb t1,t2;
    ftime(&t1);

    //高斯核生成
    vector_d kernel;
    double tmp_sum=0;
    for(int i=0; i<2*constant.sampling+1; ++i)
    {
        int x = i-constant.sampling;
        double kernel_num = 1.0/sqrt(2*M_PI)*exp(-x*x/(2*pow(constant.sampling/2,2)));
        kernel.push_back(kernel_num);
        tmp_sum += kernel_num;
    }
    for(int i=0; i<2*constant.sampling+1; ++i)
        kernel[i] /= tmp_sum;
    for(int i=0; i<2*constant.sampling+1; ++i)
        cout<<kernel[i]<<' ';
    cout<<endl;
    //把landmark_sequence的compass读进来
    for(int i=0; i<landmark_sequence.size(); ++i)
    {
        int landmark_ID = landmark_sequence[i].landmark_ID;
        //非滤波方法
        landmark_sequence[i].compass = compass[(landmark_sequence[i].keyframe-1)*constant.sampling];

        //读关键帧compass的时候加上size=2*constant.sampling均值滤波
        /*if(landmark_sequence[i].keyframe==1)//帧数太靠前，无法做滤波
        {
            double compass1 = 0;
            for(int j=0; j<2*constant.sampling; ++j)
                compass1 += compass[j];
            compass1 /= 2*constant.sampling;
            double compass2 = 0;
            for(int j=1; j<3*constant.sampling; ++j)
                compass2 += compass[j];
            compass2 /= 2*constant.sampling;
            landmark_sequence[i].compass = compass1 - (compass2-compass1);
        }
        else if(landmark_sequence[i].keyframe*constant.sampling>compass.size())//帧数太靠后，无法做滤波
        {
            int start_keyframe = landmark_sequence[i].keyframe*constant.sampling-4*constant.sampling;
            double compass1 = 0;
            for(int j=start_keyframe; j<start_keyframe+2*constant.sampling; ++j)
                compass1 += compass[j];
            compass1 /= 2*constant.sampling;
            double compass2 = 0;
            for(int j=start_keyframe+constant.sampling; j<start_keyframe+3*constant.sampling; ++j)
                compass2 += compass[j];
            compass2 /= 2*constant.sampling;
            landmark_sequence[i].compass = compass2 + (compass2-compass1);
        }
        else
        {
            //均值滤波
//            double tmp_compass_sum = 0;
//            for(int j=(landmark_sequence[i].keyframe-2)*constant.sampling;
//                j<landmark_sequence[i].keyframe*constant.sampling; ++j)
//                tmp_compass_sum += compass[j];
//            landmark_sequence[i].compass = tmp_compass_sum/(2*constant.sampling);

            //高斯滤波
            landmark_sequence[i].compass = 0;
            for(int j=(landmark_sequence[i].keyframe-2)*constant.sampling;
                j<landmark_sequence[i].keyframe*constant.sampling+1; ++j)
                landmark_sequence[i].compass +=
                        compass[j]*kernel[j-(landmark_sequence[i].keyframe-2)*constant.sampling];

        }*/
    }

    ftime(&t2);
    long t = (t2.time-t1.time)*1000 + t2.millitm-t1.millitm;
    cout<<"**time for load_landmark_x_y_compass = "<<t<<endl;
}



void split(const string &src, const string &separator, vector<string> &dest)
{
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



cv::Mat load_frame(char *testcase_dir, int frame_index) {
    assert(frames.size() != 0);
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


int frame_counter(char *path) {
    DIR *dir;
    struct dirent *ent;
    int count = 0;

    dir = opendir(path);
    regex pattern(".*\\.jpg");

    while ((ent = readdir(dir)) != NULL)
    {
        string str(ent->d_name);
        if (regex_match(str, pattern))
            count++;
    }

    return count;
}

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

