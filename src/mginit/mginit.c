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
** mginit.c: It illustrates how to program mginit in HybridOS Lite.
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
#include <signal.h>
#include <time.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/fixedmath.h>
#include <mgeff/mgeff.h>
#include <hibus.h>

#include "../include/sysconfig.h"
#include "config.h"
#include "hibus.h"
#include "tools.h"
#include "parsemanifest.h"
#include "title-description.h"
#include "compsor-mine.h"

OS_Global_struct __os_global_struct;

static void on_znode_operation(int op, int cli, int idx_znode)
{
    MSG msg;
    int clientId = 0;
    int idx_topmost = 0;
    MG_Client* client = mgClients + cli;

    if(__os_global_struct.pid_wallpaper == client->pid)
        __os_global_struct.clientId_wallpaper = cli;

    idx_topmost = ServerGetTopmostZNodeOfType(NULL, ZOF_TYPE_NORMAL, &clientId);

    if((__os_global_struct.clientId_topmost == 0) && (clientId == 0))
        return;

    __os_global_struct.clientId_topmost = clientId;


    if((op == ZNOP_CHANGECAPTION) && (clientId != cli))
    {
        return;
    }
}

static int format_request(int cli, int clifd, void* buff, size_t len)
{
    RequestInfo * requestInfo = (RequestInfo *)buff;
    ReplyInfo replyInfo;
    page_struct * page = __os_global_struct.page;
    runner_struct * runner = NULL;
    int bfind = 0;

    if(requestInfo->id == REQ_SUBMIT_PID_HWND)
    {
        replyInfo.id = REQ_SUBMIT_PID_HWND;
        ServerSendReply(clifd, &replyInfo, sizeof(replyInfo));

        while(page)
        {
            runner = page->runner;
            while(runner)
            {
                if(runner->pid == requestInfo->pid)
                {
                    runner->hwnd = requestInfo->hwnd;
                    bfind = 1;
                    break;
                }
                runner = runner->next;
            }
            if(bfind)
                break;
            page = page->next;
        }
    }

    return 0;
}

int MiniGUIMain (int args, const char* arg[])
{
    struct sigaction siga;                      // for process quit
    MSG msg;

    char layer_name[16];
    page_struct * page = NULL;

    int fd_hibus = -1;
    CompositorOps* fallback_ops = (CompositorOps*)ServerGetCompositorOps (COMPSOR_NAME_FALLBACK);
    CC_TRANSIT_TO_LAYER old_transit_to_lay = fallback_ops->transit_to_layer;

#ifdef PLATFORM_R818
    int fd = -1;
    fd = open("/dev/fb0", O_RDWR, 0);
    if (fd < 0) 
    {
        fprintf(stderr, "Unable to open /dev/fb0\n");
        return(-1);
    }

    uintptr_t args1[2] = { 1, 0 };
    if (ioctl(fd, FBIO_ENABLE_CACHE, args1) < 0) 
    {
        fprintf(stderr, "Unable to ioctl /dev/fb0\n");
        return -1;
    }
    close(fd);
#endif

    // clean global structure
    memset(&__os_global_struct, 0, sizeof(OS_Global_struct));

    // step 1: for prcess exception
    siga.sa_handler = child_wait;
    siga.sa_flags  = 0;
    memset (&siga.sa_mask, 0, sizeof(sigset_t));
    sigaction (SIGCHLD, &siga, NULL);


    // step 2: start MiniGUI server
    mGEffInit();
    if (!ServerStartup (0, 0, 0)) 
    {
        fprintf (stderr, "Can not start the server of MiniGUI-Processes: mginit.\n");
        return 1;
    }
    // register request handler
    if(!RegisterRequestHandler(FORMAT_REQID, format_request))
    {
        return 2;
    }
    // register global function
    // to get focused app znode and display title
    OnZNodeOperation = on_znode_operation;


    // step 3: override compositor functions 
    start_my_compositor();
    

    // step 4: parse manifest.json
    parse_manifest();


    // step5: create layer for application 
    page = __os_global_struct.page;
    while(page)
    {
        sprintf(layer_name, "layer%d", page->id - 1);
        page->layer = ServerCreateLayer(layer_name, 0, 0);
        page = page->next;
    }
    page = find_page_by_id(__os_global_struct.current_page + 1);
    // no animation
    fallback_ops->transit_to_layer = NULL;
    ServerSetTopmostLayer(page->layer);
    fallback_ops->transit_to_layer = old_transit_to_lay;

    
    // step 6: start wallpaper and 3 windows
    __os_global_struct.pid_wallpaper = exec_hishell_app("wallpaper");
    if(__os_global_struct.pid_wallpaper < 0)
    {
        return 3;
    }

    // create title, description, indicator bar
    if(create_title_bar () == HWND_INVALID)
    {
        fprintf (stderr, "Can not create title bar.\n");
        return 2;
    }
    if(create_description_bar() == HWND_INVALID)
    {
        fprintf (stderr, "Can not create description bar.\n");
        return 2;
    }
    if(create_indicator_bar() == HWND_INVALID)
    {
        fprintf (stderr, "Can not create indicator bar.\n");
        return 2;
    }

    // step 7: start process according to page and daemon
    start_apps();

    // step 8: connect to hibus
    fd_hibus = start_hibus(&(__os_global_struct.hibus_context));
    if(fd_hibus <= 0)
    {
        fprintf (stderr, "Can not connect to hibus.\n");
        return 3;
    }
    RegisterListenFD (fd_hibus, POLLIN, HWND_DESKTOP, NULL);


    while (GetMessage(&msg, HWND_DESKTOP)) {
        if(msg.message == MSG_FDEVENT)
        {
            hibus_read_and_dispatch_packet(__os_global_struct.hibus_context);
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    mGEffDeinit();
    end_hibus(__os_global_struct.hibus_context);

    return 0;
}
