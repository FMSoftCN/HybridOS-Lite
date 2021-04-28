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
** tools.c: functions for all program.
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
#include "tools.h"

extern OS_Global_struct __os_global_struct;

void child_wait (int sig)
{
    int pid;
    int status;

    while ((pid = waitpid (-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED (status))
            printf ("--pid=%d--status=%x--rc=%d---\n", 
                            pid, status, WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            printf ("--pid=%d--signal=%d--\n", pid, WTERMSIG (status));
    }
}

pid_t exec_hishell_app(char * app)
{
    pid_t pid = 0;
    char buff [PATH_MAX + NAME_MAX + 1] = {0};
    char execPath[PATH_MAX + 1];

    if ((pid = vfork ()) > 0) {
        fprintf (stderr, "new child, pid: %d.\n", pid);
    }
    else if (pid == 0) {
        readlink("/proc/self/exe", execPath, PATH_MAX);
        sprintf(buff, "%s/%s", dirname(execPath), app);
        execl (buff, app, NULL);
        perror ("execl");
        _exit (1);
    }
    else {
        perror ("vfork");
    }
    return pid;
}

pid_t exec_runner(char * path, char * app, char * param)
{
    pid_t pid = 0;
    char buff [PATH_MAX + NAME_MAX + 1] = {0};
    char execPath[PATH_MAX + 1];

    if ((pid = vfork ()) > 0) {
        fprintf (stderr, "new child, pid: %d.\n", pid);
    }
    else if (pid == 0) {
        readlink("/proc/self/exe", buff, PATH_MAX + NAME_MAX + 1);
        sprintf(buff, "%s/%s", dirname(buff), path);
        execl (buff, app, param, NULL);
        perror ("execl");
        _exit (1);
    }
    else {
        perror ("vfork");
    }
    return pid;
}

page_struct * find_page_by_id(int page_id)
{
    int i = 0;
    page_struct * page = __os_global_struct.page;
    page_struct * ret_page = NULL;

    for(i = 0; i < __os_global_struct.page_number; i++)
    {
        if(page->id == page_id)
        {
            ret_page = page;
            break;
        }
        page = page->next;
    }

    return ret_page;
}

page_struct * find_next_page(page_struct * page, BOOL circle)
{
    page_struct * ret_page = NULL;

    ret_page = page->next;

    if(circle && (ret_page == NULL))
        ret_page = __os_global_struct.page;

    return ret_page;
}

page_struct * find_prev_page(page_struct * page, BOOL circle)
{
    page_struct * ret_page = NULL;
    page_struct * page_now = __os_global_struct.page;
    int i = 0;

    for(i = 0; i < __os_global_struct.page_number; i++)
    {
        if(page_now == page)
            break;

        ret_page = page_now;
        page_now = page_now->next;
    }

    if(i == __os_global_struct.page_number)
        ret_page = NULL;

    if(circle && (ret_page == NULL))
    {
        ret_page = __os_global_struct.page;
        for(i = 0; i < __os_global_struct.page_number - 1; i++)
            ret_page = ret_page->next;
    }

    return ret_page;
}

runner_struct * find_pid(pid_t pid)
{
    page_struct * page = __os_global_struct.page;
    runner_struct * runner = NULL;
    int bfind = 0;

    while(page)
    {
        runner = page->runner;
        while(runner)
        {
            if(runner->pid == pid)
            {
                bfind = 1;
                break;
            }
            runner = runner->next;
        }
        if(bfind)
            break;

        page = page->next;
    }
    if(bfind == 0)
        runner = NULL;

    return runner;
}

int find_clientId_by_pid(pid_t pid)
{
    int i = 0;
     MG_Client* client = NULL;

    for(i = 0; i < mgClientSize; i++)
    {
        client = mgClients + i;
        if(client->pid == pid)
            break;
    }

    if(i == mgClientSize)
        i = -1;

    return i;
}

static pid_t exec_lite_runner(char * entry, char* name, char * layer)
{
    pid_t pid = 0;
    char buff [PATH_MAX + NAME_MAX + 1] = {0};
    char execPath[PATH_MAX + 1];
    char * param[100];
    int length = 0;

    if((entry == NULL) || (name == NULL) || (layer == NULL))
        return -1;

    if ((pid = vfork ()) > 0) {
        fprintf (stderr, "new child, pid: %d.\n", pid);
    }
    else if (pid == 0) 
    {
        char * temp = NULL;
        char * temp1 = NULL;
        int n = 0;
        
        length = strlen(entry);
        temp1 = entry;
        temp = strchr(temp1, ' ');

        while(temp)
        {
            if(temp == temp1)       // it is space
            {
                if((temp - entry) >= length)
                    break;
                else
                    temp1 = temp + 1;
            }
            else
            {
                param[n] = malloc((temp - temp1) * sizeof(char) + 1);
                snprintf(param[n], (temp - temp1) + 1, "%s", temp1);
                n ++;

                if((temp - entry) >= length)
                    break;

                temp1 = temp + 1;
            }
            temp = strchr(temp1, ' ');
            if(temp == NULL)
            {
                param[n] = malloc(strlen(temp1) * sizeof(char) + 1);
                sprintf(param[n], "%s", temp1);
                n ++;
                break;
            }
        }
        param[n++] = "-b";
        param[n++] = name;
        param[n++] = "-l";
        param[n++] = layer;
        param[n] = NULL;

        readlink("/proc/self/exe", buff, PATH_MAX + NAME_MAX + 1);
        sprintf(buff, "%s/%s", dirname(buff), param[0]);
        execv (buff, param);
        perror ("execl");
        _exit (1);
    }
    else {
        perror ("vfork");
    }
    return pid;
}

static int quit_handler(hibus_conn* conn, const char* from_endpoint, const char* method_name, int ret_code, const char* ret_value)
{
    return 0;
}

void start_apps()
{
    int id = 0;
    page_struct * page = __os_global_struct.page;
    runner_struct * runner = NULL;
    char layer[16] = {0};

    while(page)
    {
        id = page->id;
        runner = page->runner;

        while(runner)
        {
            if(runner->name)
            {
                sprintf(layer, "layer%d", id - 1);
                runner->pid = exec_lite_runner(runner->entry, runner->name, layer);

                if(runner->pid > 0)
                    runner->status = RS_RUN;
                else
                    runner->status = RS_STOP;
            }
            runner = runner->next;
        }

        page = page->next;
    }
}

void end_apps(void)
{
    int id = 0;
    page_struct * page = __os_global_struct.page;
    page_struct * page_temp = NULL;
    runner_struct * runner = NULL;
    runner_struct * runner_temp = NULL;
    dep_struct * dep = NULL;
    dep_struct * dep_temp = NULL;
    char layer[16] = {0};
    char * endpoint = NULL;
    char runner_name[64] = {0};

    while(page)
    {
        id = page->id;
        runner = page->runner;

        sprintf(layer, "%d", id - 1);

        while(runner)
        {
            if(runner->pid >= 0)
            {
                sprintf(runner_name, "%s", runner->name);
                endpoint = hibus_assemble_endpoint_name_alloc(HIBUS_LOCALHOST, HIBUS_HISHELL_NAME, runner_name);
                hibus_fire_event(__os_global_struct.hibus_context, HIBUS_EVENT_APP_QUIT, "{\"command\":\"quit\"}");
            }

            if(runner->name)
                free(runner->name);
            if(runner->entry)
                free(runner->entry);
            if(runner->css_class)
                free(runner->css_class);
            if(runner->styles)
                free(runner->styles);
            if(runner->intent)
                free(runner->intent);
            if(runner->dep)
            {
                dep = runner->dep;
                while(dep)
                {
                    if(dep->dep_name)
                        free(dep->dep_name);

                    dep_temp = dep;
                    dep = dep->next;
                    free(dep_temp);
                }
            }

            if(runner->node)
                hilayout_element_node_destroy(runner->node);

            if(runner->font_name)
                free(runner->font_name);

            runner_temp = runner;
            runner = runner->next;
            free(runner_temp);
        }

        ServerDeleteLayer(page->layer);

        if(page->id_string)
            free(page->id_string);
        if(page->styles)
            free(page->styles);
        if(page->title_en)
            free(page->title_en);
        if(page->title_zh)
            free(page->title_zh);

        page_temp = page;
        page = page->next;
        free(page_temp);
    }

    __os_global_struct.page = NULL;

    if(__os_global_struct.css_file)
        free(__os_global_struct.css_file);
    __os_global_struct.css_file = NULL;

    if(__os_global_struct.tile_style)
        free(__os_global_struct.tile_style);
    __os_global_struct.tile_style = NULL;

    if(__os_global_struct.description_style)
        free(__os_global_struct.description_style);
    __os_global_struct.description_style = NULL;

    if(__os_global_struct.page_style)
        free(__os_global_struct.page_style);
    __os_global_struct.page_style = NULL;

    if(__os_global_struct.indicator_style)
        free(__os_global_struct.indicator_style);
    __os_global_struct.indicator_style = NULL;

    __os_global_struct.page_number = 0;
    __os_global_struct.current_page = 0;
    __os_global_struct.clientId_topmost = 0;
    __os_global_struct.page = NULL;
}

