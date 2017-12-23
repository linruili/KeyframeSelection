//
// Created by Ruili on 17-5-3.
//

#ifndef KEYFRAMESELECTION_KEYFRAME_SELECTION_UPDATE_H
#define KEYFRAMESELECTION_KEYFRAME_SELECTION_UPDATE_H

#include "tools.h"
#include "run_rcnn_procedure.h"
#include "Myserver.h"
#include "classification.h"
#include "landmark_identification.h"
#include <cmath>
#include <cstdlib>
#include <sys/timeb.h>

int findKeyframe(int &minSpace,int minIndex, int curIndex, cv::Rect rect, int frame_width);

void keyframe_selection_update(char *result_dir, Myserver &myserver, Classifier &classifier,
                               char *landmark_classification_filename);

#endif //KEYFRAMESELECTION_KEYFRAME_SELECTION_UPDATE_H
