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
/**
 * \file sysconfig.h
 * \author Gengyue <gengyue@minigui.org>
 * \date 2020/09/16
 *
 * \brief This file includes some interfaces used by Hybrid OS.
 *
 \verbatim

    This file is part of hishell, a developing operating system based on
    MiniGUI. HybridOs will support embedded systems and smart IoT devices.

    Copyright (C) 2002~2020, Beijing FMSoft Technologies Co., Ltd.
    Copyright (C) 1998~2002, WEI Yongming

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/blog/minigui-licensing-policy/>.

 \endverbatim
 */

/*
 * $Id: sysconfig.h 13674 2020-09-16 06:45:01Z Gengyue $
 *
 *      HybridOS for Linux, VxWorks, NuCleus, OSE.
 */

#ifndef _SYSTEM_CONFIG
#define _SYSTEM_CONFIG

// for hibus
#define HIBUS_PROCEDURE_QUIT            "quitApp"
#define HIBUS_PROCEDURE_LAUNCHAPP       "launchApp"
#define HIBUS_PROCEDURE_LAUNCHRUNNER    "launchRunner"

// Customer Require Id
#define FORMAT_REQID                (MAX_SYS_REQID + 1)

// Customer sub require Id
#define REQ_SUBMIT_PID_HWND         0   // inform mginit, the pid and main hwnd of hijs
#define REQ_GET_TOPMOST_TITLE       1   // get topmost normal window title
#define REQ_SUBMIT_TOGGLE           2   // toggle the application
#define REQ_SHOW_PAGE               3   // show target page
#define REQ_SUBMIT_TOPMOST          4   // set the window to topmost

// user define message
#define MSG_MAINWINDOW_CHANGE       (MSG_USER + 1)
#define MSG_CONFIG_CHANGE           (MSG_USER + 2)






// configuration file
#define SYSTEM_CONFIG_FILE  "hybridos.cfg"  // configuration file

#define MSG_DYBKGND_DO_ANIMATION    (MSG_USER + 2)
#define MSG_BROWSER_SHOW            (MSG_USER + 3)
#define MSG_WIFI_CHANGED            (MSG_USER + 4)


// max number of targets
#define MAX_TARGET_NUMBER           9   // max number of targets
#define MAX_TARGET_NAME_LEN         64  // the max number of target name

// Customer compositor name
#define MIME_COMPOSITOR         "mine-compositor"


typedef struct tagRequestInfo 
{
    int id;                     // sub request ID
    HWND hwnd;                  // the window handle of the sending window
    pid_t pid;                  // the pid of process
    int znode;                  // znode index of bar
} RequestInfo;

typedef struct tagReplyInfo 
{
    int id;                     // sub request ID
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
