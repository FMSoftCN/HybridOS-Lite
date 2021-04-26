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
** hibus.c: It illustrates how to use libhibus.so.
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

static void quitApp(hibus_conn* conn, const char* from_endpoint, const char* bubble_name, \
                                                                const char* bubble_data)
{
    fprintf(stderr, "quit chgconfig: %s\n", bubble_data);
    if(global_param.main_hwnd)
        PostMessage(global_param.main_hwnd, MSG_CLOSE, 0, 0);
    return;
}

int start_hibus(hibus_conn ** context, const char * id)
{
    hibus_conn * hibus_context = NULL;
    int fd_socket = -1;
    int ret_code = 0;
    char runner_name[32] = {0};
    char * endpoint = NULL;
    
    sprintf(runner_name, "chgconfig%s", id);
    while(ret_code < 10)
    {
        // connect to hibus server
        fd_socket = hibus_connect_via_unix_socket(SOCKET_PATH, HIBUS_HISHELL_NAME, \
                                                        runner_name, &hibus_context);
        if(fd_socket <= 0)
        {
            fprintf(stderr, "chgconfig: connect to HIBUS server error!\n");
            sleep(1);
            ret_code ++;
        }
        else
            break;
    }

    if(ret_code == 10)
        return -1;

    // subscribe hibus event
    endpoint = hibus_assemble_endpoint_name_alloc(HIBUS_LOCALHOST, HIBUS_HISHELL_NAME, \
                                                                HIBUS_HISHELL_MGINIT_NAME);
    ret_code = hibus_subscribe_event(hibus_context, endpoint, HIBUS_EVENT_APP_QUIT, quitApp);
    if(ret_code)
    {
        fprintf(stderr, "chgconfig: Error for subscribe event %s, %s.\n", \
                    HIBUS_EVENT_APP_QUIT, hibus_get_err_message(ret_code));
        return -1;
    }

    if(endpoint)
        free(endpoint);

    *context = hibus_context;

    return fd_socket;
}


void end_hibus(hibus_conn * context)
{
    hibus_disconnect(context);
}
