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

vector<int> LCS(vector<string> cluster, vector<string> landmark_raw, int flag);

vector<int> find_cluster_and_LCS(vector<vector<string>> clusters, vector<string> landmark_raw);

#endif //KEYFRAMESELECTION_LCS_H
