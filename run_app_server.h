//
// Created by ruili on 17-12-20.
//��������̨�㷨�����ض�λ�������׿�ͻ���
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
