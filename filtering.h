//
// Created by Ruili on 17-5-18.
//

#ifndef KEYFRAMESELECTION_FILTERING_H
#define KEYFRAMESELECTION_FILTERING_H

#include <iostream>
#include <vector>
#include "LCS.h"

using namespace std;

typedef struct landmark_info
{
    //对应landmark_classification.txt的每一行
    int landmark_index;
    string landmark_name;
    float prob;
};

vector<int> filter(char *landmark_classification_filename)
{
    FILE *cluster_file = fopen("cluster.txt", "r");
    vector<vector<string>> clusters;
    int num;
    while(fscanf(cluster_file, "%d", &num)!= EOF)
    {
        vector<string> cluster_line;
        int each_landmark;
        for(int i=1; i<=num; ++i)
        {
            fscanf(cluster_file, " %d", &each_landmark);
            cluster_line.push_back(to_string(each_landmark));
        }
        clusters.push_back(cluster_line);
    }

    FILE *landmark_classification_file = fopen(landmark_classification_filename, "r");
    int tmp1, tmp2;
    float tmp3;
    vector<landmark_info> landmark_infos;
    while(fscanf(landmark_classification_file, "%d %d %f", tmp1, tmp2, tmp3)!=EOF)
        if(tmp2 != -1)
        {
            landmark_info tmp;
            tmp.landmark_index = tmp1;
            tmp.landmark_name = to_string(tmp2);
            tmp.prob = tmp3;
            landmark_infos.push_back(tmp);
        }


    //注：landmark序号从1开始











    vector<string> landmark_raw;
    landmark_raw.push_back("2");
    landmark_raw.push_back("4");
    landmark_raw.push_back("50");
    landmark_raw.push_back("1");

    vector<int> result_index;
    result_index = LCS(clusters, landmark_raw);
    for(int i=0; i<result_index.size(); ++i)
        cout<<result_index[i]<<" ";

}


#endif //KEYFRAMESELECTION_FILTERING_H
