//
// Created by Ruili on 17-5-18.
//

#ifndef KEYFRAMESELECTION_FILTERING_H
#define KEYFRAMESELECTION_FILTERING_H

#include <iostream>
#include <vector>
#include "LCS.h"
#include <sys/timeb.h>

using namespace std;


vector<landmark_info> filter(char *landmark_classification_filename)
{
    timeb t1,t2;
    ftime(&t1);
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
    fclose(cluster_file);

    FILE *landmark_classification_file = fopen(landmark_classification_filename, "r");
    int tmp1, tmp2, tmp4;
    float tmp3;
    vector<landmark_info> landmark_infos;//过滤前的
    vector<landmark_info> landmark_sequence;//过滤后的
    while(fscanf(landmark_classification_file, "%d %d %f %d", &tmp1, &tmp2, &tmp3, &tmp4)!=EOF)
        if(tmp2 != -1)
        {
            landmark_info tmp;
            tmp.landmark_index = tmp1;
            tmp.landmark_ID = tmp2;
            tmp.prob = tmp3;
            tmp.keyframe = tmp4;
            landmark_infos.push_back(tmp);
        }
    fclose(landmark_classification_file);

    //去掉名字相同的，取概率大的
    for(int i=0; i<landmark_infos.size()-1; ++i)
        for(int j=i+1; j<landmark_infos.size(); ++j)
            if(landmark_infos[i].landmark_ID == landmark_infos[j].landmark_ID)
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
        landmark_raw.push_back(to_string(landmark_infos[i].landmark_ID));

    vector<int> result_index;
    vector<int> final_index;

    //LCS过滤
    result_index = find_cluster_and_LCS(clusters, landmark_raw);
    for(int i=0; i<result_index.size(); ++i)
        landmark_sequence.push_back(landmark_infos[result_index[i]]);

    ftime(&t2);
    long t = (t2.time-t1.time)*1000 + t2.millitm-t1.millitm;
    cout<<"**time for filtering = "<<t<<endl;
    return landmark_sequence;
}


#endif //KEYFRAMESELECTION_FILTERING_H
