//
// Created by Ruili on 17-5-4.
//

#ifndef KEYFRAMESELECTION_RUN_RCNN_PROCEDURE_H
#define KEYFRAMESELECTION_RUN_RCNN_PROCEDURE_H


#include "constant.h"
#include "tools.h"
#include "Myserver.h"
#include <sys/wait.h>
#include <sys/timeb.h>


void readFromFile(char *keyframe_prob_filename, std::vector<Landmark> &result);
vector<Landmark> run_rcnn_procedure(char *recv_dir, int frame_index, Myserver &myserver);

#endif //KEYFRAMESELECTION_RUN_RCNN_PROCEDURE_H
