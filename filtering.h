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
    //注：landmark_index序号从1开始
    int landmark_index;
    string landmark_name;
    float prob;
}landmark_info;

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
    while(fscanf(landmark_classification_file, "%d %d %f", &tmp1, &tmp2, &tmp3)!=EOF)
        if(tmp2 != -1)
        {
            landmark_info tmp;
            tmp.landmark_index = tmp1;
            tmp.landmark_name = to_string(tmp2);
            tmp.prob = tmp3;
            landmark_infos.push_back(tmp);
        }

    //去掉名字相同的，取概率大的
    for(int i=0; i<landmark_infos.size()-1; ++i)
        for(int j=i+1; j<landmark_infos.size(); ++j)
            if(landmark_infos[i].landmark_name == landmark_infos[j].landmark_name)
            {
                if(landmark_infos[j].prob <= landmark_infos[i].prob)
                {
                    landmark_infos.erase(landmark_infos.begin()+j);
                    --j;
                }
                else
                {
                    landmark_infos.erase(landmark_infos.begin()+i);
                    --i;
                    break;
                }
            }

    vector<string> landmark_raw;
    for(int i=0; i<landmark_infos.size(); ++i)
        landmark_raw.push_back(landmark_infos[i].landmark_name);

    vector<int> result_index;
    vector<int> final_index;
    result_index = LCS(clusters, landmark_raw);
    for(int i=0; i<result_index.size(); ++i)
        final_index.push_back(landmark_infos[result_index[i]].landmark_index);

    return final_index;
}


#endif //KEYFRAMESELECTION_FILTERING_H
