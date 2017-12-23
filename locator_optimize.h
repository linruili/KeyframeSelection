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
const double north[] = {1, 0};
int landmark_count;
vector<point_t> landmark_location;
vector<model_t> map_model;//墙的线段
//vector_d compass;
double lambda_weight;

double dot_product(const point_t &veca, const point_t &vecb);
double norm(const point_t &vec);
point_t cross(const homo_point_t &from, const homo_point_t &to);
double pdist(const point_t &veca, const point_t &vecb);
bool line_intersection_side(const point_t A, const point_t B, const point_t C, const point_t D);
bool is_can_see(const point_t user_pos, const point_t store_pos);
void print_landmark_sequence_info(vector<landmark_info> &landmark_sequence);
void landmark_init(vector<landmark_info> &landmark_sequence);
vector_d calculate_location_result(const vector_d &delta_theta);


/*****************************
 ** begin genetic algorithm **/

float objective(GAGenome &);
vector_d genome2vector(GAGenome &);


vector_d
ga_main(vector<landmark_info> &landmark_sequence,
        const double lambda = 0.05, const int pop_size = 50, const int generation = 500, const double cross_ratio = 0.9,
        const double mutation_ratio = 0.1, const int ga_min_value = -10, const int ga_max_value = 10)
{
    print_landmark_sequence_info(landmark_sequence);

    printf("------GA VERSION: %s ------\n", GA_VERSION);
    printf("Config: lambda=%f, pop_size=%d, generation=%d, \ncross_ratio=%f, mutation_ratio=%f, min_value=%d, max_value=%d\n",
           lambda, pop_size, generation, cross_ratio, mutation_ratio, ga_min_value, ga_max_value);

    // begin ga config

    landmark_count = landmark_sequence.size();
    lambda_weight = lambda;
    landmark_init(landmark_sequence);

    // See if we've been given a seed to use (for testing purposes).  When you
    // specify a random seed, the evolution will be exactly the same each time
    // you use that seed number.

    unsigned int seed = time(NULL);
    GARandomSeed(seed);

    // Declare variables for the GA parameters and set them to some default values.
    //    int popsize = 30;
    //    int ngen = 200;
    //    float pmut = 0.08;
    //    float pcross = 0.9;

    // Create a phenotype for two variables.  The number of bits you can use to
    // represent any number is limited by the type of computer you are using.

    GABin2DecPhenotype map;
    for (int i = 0; i < landmark_count; ++i)
        map.add(16, ga_min_value, ga_max_value);


    // Create the template genome using the phenotype map we just made.
    //This genome is an implementation of the traditional method for converting binary strings to decimal values.
    GABin2DecGenome genome(map, objective);

    // Now create the GA using the genome and run it.  We'll use sigma truncation
    // scaling so that we can handle negative objective scores.

    GASimpleGA ga(genome);
    GASigmaTruncationScaling scaling;
//    GASharing scaling;
    ga.minimize();
    ga.populationSize(pop_size);
    ga.nGenerations(generation);
    ga.pMutation(mutation_ratio);
    ga.pCrossover(cross_ratio);
    ga.scaling(scaling);
    ga.scoreFilename("/dev/null");
    ga.scoreFrequency(50);
    ga.flushFrequency(100);
    ga.initialize();
    ga.evolve(seed);

    genome = ga.statistics().bestIndividual();
    cout << "\nthe ga found an optimum at the point (";
    for (int j = 0; j < genome.nPhenotypes() - 1; ++j)
        cout << genome.phenotype(j) << ", ";
    cout << genome.phenotype(genome.nPhenotypes() - 1) << ")\n";

    vector_d delta = genome2vector(genome);
    vector_d location_result = calculate_location_result(delta);

    printf("Location Result is:(%f, %f), cost is: %f, %f, %f\n\n", location_result[0], location_result[1],
           location_result[2], location_result[3], location_result[4]);

    return location_result;
}

vector_d genome2vector(GAGenome &ga)
{
    GABin2DecGenome &genome = (GABin2DecGenome &) ga;

    vector_d delta;
    for (int i = 0; i < genome.nPhenotypes(); ++i)
        delta.push_back(genome.phenotype(i));
    return delta;
}

float objective(GAGenome &c)
{
    GABin2DecGenome &genome = (GABin2DecGenome &) c;

    vector_d delta = genome2vector(genome);

    vector_d result = calculate_location_result(delta);

    if(isnan(result[2]))
        assert(false);

    return result[2];
}

//*****************************
//以下是非遗传算法相关

vector_d simple_weighted(vector<landmark_info> &landmark_sequence)
{
    print_landmark_sequence_info(landmark_sequence);
    landmark_init(landmark_sequence);
    vector_d delta(landmark_count, 0);
    vector_d location_result = calculate_location_result(delta);

    printf("Location Result is:(%f, %f), cost is: %f, %f, %f\n\n", location_result[0], location_result[1],
           location_result[2], location_result[3], location_result[4]);

    return location_result;
}


vector_d calculate_location_result(const vector_d &delta_theta)
{
    vector<point_t> cross_result;
    vector<double> weight;

    for (int i = 0; i < landmark_count - 1; ++i)
        for (int j = i + 1; j < landmark_count; ++j)
        {
            point_t landmark_a = landmark_location[i];
            point_t landmark_b = landmark_location[j];

            double theta_a = compass[i] + delta_theta[i];
            double theta_b = compass[j] + delta_theta[j];

            iBS::Matrix rotation_a(2), rotation_b(2);
            rotation_a.data[0][0] = cos(DEG2RAD(theta_a));
            rotation_a.data[0][1] = -sin(DEG2RAD(theta_a));
            rotation_a.data[1][0] = sin(DEG2RAD(theta_a));
            rotation_a.data[1][1] = cos(DEG2RAD(theta_a));

            rotation_b.data[0][0] = cos(DEG2RAD(theta_b));
            rotation_b.data[0][1] = -sin(DEG2RAD(theta_b));
            rotation_b.data[1][0] = sin(DEG2RAD(theta_b));
            rotation_b.data[1][1] = cos(DEG2RAD(theta_b));

            iBS::Matrix north_vec(2);
            north_vec.data[0][0] = north[0];
            north_vec.data[1][0] = north[1];

            iBS::Matrix direction_vec_a = rotation_a * north_vec;
            iBS::Matrix direction_vec_b = rotation_b * north_vec;

            point_t dir_a(direction_vec_a.data[0][0], direction_vec_a.data[1][0]);
            point_t dir_b(direction_vec_b.data[0][0], direction_vec_b.data[1][0]);

            homo_point_t line_a(dir_a.y, -dir_a.x, dir_a.x * landmark_a.y - dir_a.y * landmark_a.x);
            homo_point_t line_b(dir_b.y, -dir_b.x, dir_b.x * landmark_b.y - dir_b.y * landmark_b.x);

            point_t user_pos = cross(line_a, line_b);

            if (isnan(user_pos.x) || isnan(user_pos.y))
                assert(false);

            if (is_can_see(user_pos, landmark_a) && is_can_see(user_pos, landmark_b))
            {
                cross_result.push_back(user_pos);
                weight.push_back(1.0 / (pdist(landmark_a, landmark_b) * pdist(landmark_a, landmark_b)));
            }
            else
            {
                point_t error_point(INT_MAX, INT_MAX);
                cross_result.push_back(error_point);
                weight.push_back(0);
            }
        }


    // normalize weight
    double weight_sum = 0;
    for (int i = 0; i < weight.size(); i++)
        weight_sum += weight[i];


    point_t loc_result(INT_MAX, INT_MAX);
    double loc_cost = 0;

    if(weight_sum == 0)
    {
        weight_sum = INT_MAX;
        loc_cost = INT_MAX;
    }
    else
    {
        // calucuate average location result
        double average_x = 0;
        double average_y = 0;
        for (int i = 0; i < cross_result.size(); i++)
        {
            average_x += cross_result[i].x * (weight[i] / weight_sum);
            average_y += cross_result[i].y * (weight[i] / weight_sum);
        }

        loc_result.x = average_x;
        loc_result.y = average_y;

        for (int i = 0; i < cross_result.size(); ++i)
            loc_cost += pow(pdist(loc_result, cross_result[i]),2) * (weight[i] / weight_sum);

    }

    double delta_cost = 0;
    for (int i = 0; i < delta_theta.size(); i++)
        delta_cost += delta_theta[i] * delta_theta[i];

    double total_cost = loc_cost + lambda_weight * delta_cost;

    vector_d result;
    result.push_back(loc_result.x);
    result.push_back(loc_result.y);
    result.push_back(total_cost);
    result.push_back(loc_cost);
    result.push_back(delta_cost);
    return result;
}

double dot_product(const point_t &veca, const point_t &vecb)
{
    return veca.x * vecb.x + veca.y * vecb.y;
}

double norm(const point_t &vec)
{
    return sqrt(vec.x * vec.x + vec.y * vec.y);
}

point_t cross(const homo_point_t &from, const homo_point_t &to)
{
    double x = from.y * to.z - from.z * to.y;
    double y = from.z * to.x - from.x * to.z;
    double z = from.x * to.y - from.y * to.x;

    return point_t(x / z, y / z);
}

double pdist(const point_t &veca, const point_t &vecb)
{
    return sqrt((veca.x - vecb.x) * (veca.x - vecb.x) +
                (veca.y - vecb.y) * (veca.y - vecb.y));
}

bool line_intersection_side(const point_t A, const point_t B, const point_t C, const point_t D)
{
    double fC = (C.y - A.y) * (A.x - B.x) - (C.x - A.x) * (A.y - B.y);
    double fD = (D.y - A.y) * (A.x - B.x) - (D.x - A.x) * (A.y - B.y);

    return fC * fD <= 0;
}

bool is_can_see(const point_t user_pos, const point_t store_pos)
{
    double dist = pdist(user_pos, store_pos);
    if (dist > LINE_OF_SIGHT_MAX || dist < LINE_OF_SIGHT_MIN || isnan(dist))
        return false;

    for (int i = 0; i < map_model.size(); i++)
        if (line_intersection_side(user_pos, store_pos, map_model[i].from, map_model[i].to)
            && line_intersection_side(map_model[i].from, map_model[i].to, user_pos, store_pos))
        {
            return false;
        }
    return true;
}

void print_landmark_sequence_info(vector<landmark_info> &landmark_sequence)
{
    cout<<"landmark_sequence : ";
    for(int i=0; i<landmark_sequence.size(); ++i)
        cout<<landmark_sequence[i].landmark_ID<<" ";
    cout<<endl;

    cout<<"landmark_sequence.size() = "<<landmark_sequence.size()<<endl;
    printf("match_result_x: {");
    for (int i = 0; i < landmark_sequence.size() - 1; i++)
        printf("%f,", landmark_sequence[i].x);
    printf("%f}\n", landmark_sequence[landmark_sequence.size() - 1].x);

    printf("match_result_y: {");
    for (int i = 0; i < landmark_sequence.size() - 1; i++)
        printf("%f,", landmark_sequence[i].y);
    printf("%f}\n", landmark_sequence[landmark_sequence.size() - 1].y);

    printf("compass_reading: {");
    for (int i = 0; i < landmark_sequence.size() - 1; i++)
        printf("%f,", landmark_sequence[i].compass);
    printf("%f}\n", landmark_sequence[landmark_sequence.size() - 1].compass);
}

void landmark_init(vector<landmark_info> &landmark_sequence)
{
    //load map_model
    FILE *modelFile = fopen("./mapmodel_gogo.txt", "r");
    double from_x, from_y, to_x, to_y;
    while(fscanf(modelFile, "%lf %lf %lf %lf", &from_x, &from_y, &to_x, &to_y)!=EOF)
    {
        point_t from(from_x, from_y);
        point_t to(to_x, to_y);
        model_t from_to(from, to);
        map_model.push_back(from_to);
    }
    fclose(modelFile);

    landmark_location.clear();
    compass.clear();

    landmark_count = landmark_sequence.size();
    for (int i = 0; i < landmark_count; i++)
    {
        point_t p(landmark_sequence[i].x, landmark_sequence[i].y);
        landmark_location.push_back(p);
        compass.push_back(landmark_sequence[i].compass);
    }
}
#endif //KEYFRAMESELECTION_LOCATOR_OPTIMIZE_H
