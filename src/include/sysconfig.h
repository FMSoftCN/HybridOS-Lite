///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
/*
** sysconfig.h: It defines some strutures and micro for all applications.
**
** Copyright (C) 2021 FMSoft (http://www.fmsoft.cn).
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#ifndef _SYSTEM_CONFIG
#define _SYSTEM_CONFIG

// for hibus
#define SOCKET_PATH                     "/var/tmp/hibus.sock"
#define HIBUS_HISHELL_NAME              "cn.fmsoft.hybridos.hishell"
#define HIBUS_HISHELL_MGINIT_NAME       "mginit"

// for hibus, remote procedure
#define HIBUS_PROCEDURE_CONFIG_CHANGE   "configChange"
#define HIBUS_PROCEDURE_LAUNCHAPP       "launchApp"
#define HIBUS_PROCEDURE_LAUNCHRUNNER    "launchRunner"

// for hibus, remote procedure
#define HIBUS_EVENT_APP_QUIT            "APPQUITEVENT"

// Customer Require Id
#define FORMAT_REQID                    (MAX_SYS_REQID + 1)

// Customer subscribe require Id
#define REQ_SUBMIT_PID_HWND             0   // inform mginit, the pid and main hwnd of hijs

// user define message
#define MSG_MAINWINDOW_CHANGE           (MSG_USER + 1)
#define MSG_CONFIG_CHANGE               (MSG_USER + 2)

// configuration file
#define SYSTEM_CONFIG_FILE              "hybridos.cfg"  // configuration file


typedef struct tagRequestInfo 
{
    int id;                             // sub request ID
    HWND hwnd;                          // the window handle of the sending window
    pid_t pid;                          // the pid of process
    int znode;                          // znode index of bar
} RequestInfo;

typedef struct tagReplyInfo 
{
    int id;                             // sub request ID
    unsigned int iData0;
    unsigned int request1;
    unsigned int request2;
    unsigned int request3;
} ReplyInfo;

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _SYSTEM_CONFIG */
