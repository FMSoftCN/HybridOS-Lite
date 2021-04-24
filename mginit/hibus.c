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
#include <libgen.h>

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
#include "tools.h"
#include "parsemanifest.h"
#include "hibus.h"

extern OS_Global_struct __os_global_struct;

static char * launchApp(hibus_conn* conn, const char* from_endpoint, \
        const char* to_method, const char* method_param, int *err_code)
{
    fprintf(stderr, "launch application: %s\n", method_param);
    return NULL;
}

static char * launchRunner(hibus_conn* conn, const char* from_endpoint, \
        const char* method_name, const char* method_param, int *err_code)
{
    fprintf(stderr, "launch runner: %s\n", method_param);
    return NULL;
}

static char * configChange(hibus_conn* conn, const char* from_endpoint, \
        const char* method_name, const char* method_param, int *err_code)
{
    char manifest_path[HISHELL_MAX_PATH] = {0};
    char new_path[HISHELL_MAX_PATH] = {0};
    char temp_path[HISHELL_MAX_PATH] = {0};
    page_struct * page = NULL;
    char layer_name[16];

    // change configure file
    readlink("/proc/self/exe", manifest_path, HISHELL_MAX_PATH);
    sprintf(new_path, "%s", manifest_path);
    sprintf(temp_path, "%s", manifest_path);
    sprintf(manifest_path, "%s/layout/manifest.json", dirname(manifest_path));

    sprintf(temp_path, "%s/layout/temp.json", dirname(temp_path));
    sprintf(new_path, "%s/layout/newconfig.json", dirname(new_path));

    rename(manifest_path, temp_path);
    rename(new_path, manifest_path);
    rename(temp_path, new_path);

    end_apps();
    parse_manifest();
    page = __os_global_struct.page;
    while(page)
    {
        sprintf(layer_name, "layer%d", page->id - 1);
        page->layer = ServerCreateLayer(layer_name, 0, 0);
        page = page->next;
    }
    page = find_page_by_id(__os_global_struct.current_page + 1);
    ServerSetTopmostLayer(page->layer);

    start_apps();

    if(__os_global_struct.hIndicatorBar)
        PostMessage(__os_global_struct.hIndicatorBar, MSG_CONFIG_CHANGE, 0, 0);

    if(__os_global_struct.hDescriptionBar)
        PostMessage(__os_global_struct.hDescriptionBar, MSG_CONFIG_CHANGE, 0, 0);

    if(__os_global_struct.hTitleBar)
        PostMessage(__os_global_struct.hTitleBar, MSG_CONFIG_CHANGE, 0, 0);

    return NULL;
}

int start_hibus(hibus_conn ** context)
{
    hibus_conn * hibus_context = NULL;
    int fd_socket = -1;
    int ret_code = 0;
    char * endpoint = NULL;
    
    while(ret_code < 10)
    {
        // connect to hibus server
        fd_socket = hibus_connect_via_unix_socket(SOCKET_PATH, HIBUS_HISHELL_NAME, \
                                        HIBUS_HISHELL_MGINIT_NAME, &hibus_context);
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

    // register hibus procedure
    ret_code = hibus_register_procedure(hibus_context, HIBUS_PROCEDURE_LAUNCHAPP, \
                                                            NULL, NULL, launchApp);
    if(ret_code)
    {
        fprintf(stderr, "mginit hibus: Error for register procedure %s, %s.\n", \
                        HIBUS_PROCEDURE_LAUNCHAPP, hibus_get_err_message(ret_code));
        return -1;
    }

    ret_code = hibus_register_procedure(hibus_context, HIBUS_PROCEDURE_LAUNCHRUNNER, \
                                                        NULL, NULL, launchRunner);
    if(ret_code)
    {
        fprintf(stderr, "mginit hibus: Error for register procedure %s, %s.\n", \
                    HIBUS_PROCEDURE_LAUNCHRUNNER, hibus_get_err_message(ret_code));
        return -1;
    }

    ret_code = hibus_register_procedure(hibus_context, HIBUS_PROCEDURE_CONFIG_CHANGE, \
                                                        NULL, NULL, configChange);
    if(ret_code)
    {
        fprintf(stderr, "mginit hibus: Error for register procedure %s, %s.\n", \
                    HIBUS_PROCEDURE_CONFIG_CHANGE, hibus_get_err_message(ret_code));
        return -1;
    }

    *context = hibus_context;

    if(endpoint)
        free(endpoint);

    return fd_socket;
}


void end_hibus(hibus_conn * context)
{
    hibus_revoke_procedure(context, HIBUS_PROCEDURE_LAUNCHRUNNER);
    hibus_revoke_procedure(context, HIBUS_PROCEDURE_LAUNCHAPP);
    hibus_revoke_procedure(context, HIBUS_PROCEDURE_CONFIG_CHANGE);

    hibus_disconnect(context);
}
