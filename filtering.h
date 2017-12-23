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

vector<landmark_info> filter(char *landmark_classification_filename);

#endif //KEYFRAMESELECTION_FILTERING_H
