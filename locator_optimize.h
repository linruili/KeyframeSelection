//
// Created by Ruili on 17-5-26.
//

#ifndef KEYFRAMESELECTION_LOCATOR_OPTIMIZE_H
#define KEYFRAMESELECTION_LOCATOR_OPTIMIZE_H

#include <stdio.h>
#include <vector>
#include <cmath>
#include "ibs_matrix.h"
#include "include/ga/ga.h"
#include "include/ga/std_stream.h"
#include "include/ga/GAGenome.h"
#include "include/ga/gaversion.h"
#include "include/ga/garandom.h"
#include "include/ga/GABin2DecGenome.h"
#include "include/ga/GASimpleGA.h"
#include "tools.h"
#include "Myserver.h"
#include <stack>
#include <algorithm>
#include <climits>

#ifndef isnan
#if __cplusplus <= 199711L
#   define isnan(x) ::isnan(x)
#else
#   define isnan(x) std::isnan(x)
#endif
#endif

#define LINE_OF_SIGHT_MIN 10
#define LINE_OF_SIGHT_MAX 1000
#define GA_VERSION "4.0.2-final-fix"
//#define PI 3.14159265f
#define DEG2RAD(alpha) (alpha * M_PI / 180.0f)

using namespace std;

// global variables
extern const double north[];
extern int landmark_count;
extern vector<point_t> landmark_location;
extern vector<point_t> landmark_correct;
extern vector<model_t> map_model;//墙的线段
//vector_d compass;
extern double lambda_weight;


/*****************************
 ** begin genetic algorithm **/

float objective(GAGenome &);
vector_d genome2vector(GAGenome &);


vector_d
ga_main(vector<landmark_info> &landmark_sequence,
        const double lambda, const int pop_size, const int generation, const double cross_ratio,
        const double mutation_ratio, const int ga_min_value, const int ga_max_value);

vector_d genome2vector(GAGenome &ga);

float objective(GAGenome &c);

//*****************************
//以下是非遗传算法相关

vector_d simple_weighted(vector<landmark_info> &landmark_sequence);

vector_d calculate_location_result(const vector_d &delta_theta);

double dot_product(const point_t &veca, const point_t &vecb);

double norm(const point_t &vec);

point_t cross(const homo_point_t &from, const homo_point_t &to);

double pdist(const point_t &veca, const point_t &vecb);

bool line_intersection_side(const point_t A, const point_t B, const point_t C, const point_t D);

bool is_can_see(const point_t user_pos, const point_t store_pos);

void print_landmark_sequence_info(vector<landmark_info> &landmark_sequence);

void landmark_init(vector<landmark_info> &landmark_sequence);
#endif //KEYFRAMESELECTION_LOCATOR_OPTIMIZE_H
