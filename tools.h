//
// Created by Ruili on 17/5/3.
//

#ifndef EDGEDETECTION_TOOLS_H
#define EDGEDETECTION_TOOLS_H

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
#include <sys/timeb.h>


using namespace std;

typedef std::vector<double> vector_d;

typedef struct point
{
    double x;
    double y;

    point(double _x = 0, double _y = 0)
    {
        x = _x;
        y = _y;
    }
} point_t;

typedef struct model
{
    point_t from;
    point_t to;
    model(point_t _from, point_t _to):from(_from),to(_to){};
} model_t;

typedef struct homo_point
{
    double x;
    double y;
    double z;

    homo_point(double _x, double _y, double _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }
} homo_point_t;

typedef struct landmark_info
{
    //对应landmark_classification.txt的每一行
    //注：landmark_index序号从1开始
    int landmark_index;//landmark_classification.txt的第几行
    int landmark_ID;//1-55的那个序号
    int keyframe;
    float prob;
    double x=0, y=0, compass=0;
}landmark_info;


void load_landmark_x_y_compass(vector<landmark_info> &landmark_sequence);

void load_landmark_correct();

void split(const string &src, const string &separator, vector<string> &dest);

bool landmark_comp(Landmark a, Landmark b);

bool landmark_comp_reverse(Landmark a, Landmark b);

cv::Mat load_frame(char *testcase_dir, int frame_index);

string get_name_from_frame_index(int frame_index);

int frame_counter(char *path);

void show_image_with_rect(cv::Mat _image, cv::Rect rect);

void remove_dir(char *path);

double *load_image(const char *filename, int &rows, int &cols);

double *mat_to_array(cv::Mat image_mat, int &rows, int &cols);

void show_image(double *image, int rows, int cols);

void load_video(const char *video_name, const char *output_video, const char *main_frame_directory);

std::vector<std::vector<double> > load_points(const char *filename);

void print_points(std::vector<std::vector<double> > points);

void extract_videos(const char *video_filename);

int buffToInteger(char* buffer);




#endif //EDGEDETECTION_TOOLS_H
