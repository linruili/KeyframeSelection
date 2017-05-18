//
// Created by Ruili on 17-5-17.
//

#ifndef KEYFRAMESELECTION_LCS_H
#define KEYFRAMESELECTION_LCS_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

vector<string> LCS(vector<vector<string>> clusters, vector<string> landmark_raw)
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

    //vector<string> cluster = clusters[max_match_index]; //模板序列
    vector<string> cluster;
    cluster.push_back("1");
    cluster.push_back("2");
    cluster.push_back("3");

    for(int i=0; i<cluster.size(); ++i)
        cout<<cluster[i]<<' ';
    cout<<endl;

    //landmark_raw.insert(landmark_raw.begin(), landmark_raw.begin(), landmark_raw.end());

    for(int i=0; i<landmark_raw.size(); ++i)
        cout<<landmark_raw[i]<<' ';
    cout<<endl;

    int len1 = cluster.size();
    int len2 = landmark_raw.size();
    if(len2==0)
        return landmark_raw;
    vector<vector<int>> dp(len1+1, vector<int>(len2+1,0));
    for(int i=1; i<=len1; ++i)
        for(int j=1; j<=len2; ++j)
            if(cluster[i-1] == landmark_raw[j-1])
                dp[i][j] = dp[i-1][j-1]+1;
            else
                dp[i][j] = max(dp[i][j-1], dp[i-1][j]);

    vector<string> result;
    int i=len1, j=len2;
    while(dp[i][j] != 0)
    {
        if(dp[i][j] == dp[i-1][j-1]+1)
        {
            result.push_back(cluster[i-1]);
            --i;--j;
        }
        else if(dp[i-1][j] > dp[i][j-1])
            --i;
        else
            --j;
    }
    reverse(result.begin(), result.end());
    return result;
}

#endif //KEYFRAMESELECTION_LCS_H
