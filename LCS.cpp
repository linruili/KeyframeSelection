//
// Created by ruili on 17-12-23.
//

#include "LCS.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>


using namespace std;

vector<int> LCS(vector<string> cluster, vector<string> landmark_raw, int flag)
{
    //flag=1 正向   flag 0 反向
    if(flag == 0)
        reverse(cluster.begin(), cluster.end());
    vector<int> result_index;
    vector<string> tmp(cluster);
    cluster.insert(cluster.begin(), tmp.begin(), tmp.end());
    int len1 = cluster.size();
    int len2 = landmark_raw.size();
    if(len2==0)
        return result_index;
    vector<vector<int>> dp(len1+1, vector<int>(len2+1,0));
    for(int i=1; i<=len1; ++i)
        for(int j=1; j<=len2; ++j)
            if(cluster[i-1] == landmark_raw[j-1])
                dp[i][j] = dp[i-1][j-1]+1;
            else
                dp[i][j] = max(dp[i][j-1], dp[i-1][j]);

    int i=len1, j=len2;
    while(dp[i][j] != 0)
    {
        if(cluster[i-1] == landmark_raw[j-1])
        {
            result_index.push_back(j-1);
            --i;--j;
        }
        else if(dp[i-1][j] > dp[i][j-1])
            --i;
        else
            --j;
    }

    reverse(result_index.begin(), result_index.end());
    return result_index;

}

vector<int> find_cluster_and_LCS(vector<vector<string>> clusters, vector<string> landmark_raw)
{
    int match_num[clusters.size()];//landmark_raw和每个cluster匹配到的个数
    for(int i=0; i<clusters.size(); ++i)
    {
        match_num[i] = 0;
        for(int j=0; j<landmark_raw.size(); ++j)
            if(find(clusters[i].begin(), clusters[i].end(), landmark_raw[j]) != clusters[i].end())
                ++match_num[i];
    }

    int max_match_num = 0;
    int max_match_index = 0;
    for(int i=0; i<clusters.size(); ++i)
        if(match_num[i] > max_match_num)
        {
            max_match_num = match_num[i];
            max_match_index = i;
        }

    vector<string> cluster = clusters[max_match_index]; //模板序列

    cout<<"cluster sequence ";
    for(int i=0; i<cluster.size(); ++i)
        cout<<cluster[i]<<' ';
    cout<<endl;

    cout<<"landmark_raw sequence ";
    for(int i=0; i<landmark_raw.size(); ++i)
        cout<<landmark_raw[i]<<' ';
    cout<<endl;

    vector<int> result_index1 = LCS(cluster, landmark_raw, 1);
    vector<int> result_index0 = LCS(cluster, landmark_raw, 0);
    if(result_index0.size() > result_index1.size())
        return result_index0;
    else
        return result_index1;
}

