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
 * \file hibus.c
 * \author Gengyue <gengyue@minigui.org>
 * \date 2020/09/16
 *
 * \brief This file implements status bar in system manager process.
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
 * $Id: hibus.c 13674 2021-03-11 11:40:00Z Gengyue $
 *
 *      HybridOS for Linux, VxWorks, NuCleus, OSE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <mgeff/mgeff.h>
#include <hibus.h>
#include <hibox/json.h>

#include "../include/sysconfig.h"
#include "config.h"

extern Global_Param global_param;

static char * quitApp(hibus_conn* conn, const char* from_endpoint, const char* to_method, const char* method_param, int *err_code)
{
    if(global_param.main_hwnd)
        PostMessage(global_param.main_hwnd, MSG_CLOSE, 0, 0);
    return NULL;
}

int start_hibus(hibus_conn ** context, const char * id)
{
    hibus_conn * hibus_context = NULL;
    int fd_socket = -1;
    int ret_code = 0;
    char runner_name[32] = {0};
    
    sprintf(runner_name, "gear%s", id);
    while(ret_code < 10)
    {
        // connect to hibus server
        fd_socket = hibus_connect_via_unix_socket(SOCKET_PATH, HIBUS_HISHELL_NAME, runner_name, &hibus_context);
        if(fd_socket <= 0)
        {
            fprintf(stderr, "mginit hibus: connect to HIBUS server error!\n");
            sleep(1);
            ret_code ++;
        }
        else
            break;
    }

    if(ret_code == 10)
        return -1;

    *context = hibus_context;

    // register procedure
    ret_code = hibus_register_procedure(hibus_context, HIBUS_PROCEDURE_QUIT, NULL, NULL, quitApp);
    if(ret_code)
    {
        fprintf(stderr, "mginit hibus: Error for register procedure %s, %s.\n", HIBUS_PROCEDURE_LAUNCHAPP, hibus_get_err_message(ret_code));
        return -1;
    }

    return fd_socket;
}


void end_hibus(hibus_conn * context)
{
    hibus_revoke_procedure(context, HIBUS_PROCEDURE_QUIT);

    hibus_disconnect(context);
}
