//
// Created by ruili on 17-12-20.
//跑整个后台算法，返回定位结果给安卓客户端
//

#ifndef KEYFRAMESELECTION_RUN_APP_SERVER_H
#define KEYFRAMESELECTION_RUN_APP_SERVER_H

#include "Myserver.h"
#include "classification.h"
#include "tools.h"
#include "keyframe_selection_update.h"
#include "filtering.h"
#include "locator_optimize.h"
#include <sys/timeb.h>

#define ROUND 10
using namespace std;

void run_app_server(char* argv);

#endif //KEYFRAMESELECTION_RUN_APP_SERVER_H
