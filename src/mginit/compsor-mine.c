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
** compsor-mime.c: It illustrates how to modify compositor functions for user.
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
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <mgeff/mgeff.h>
#include <glib.h>
#include <hibus.h>

#include "../include/sysconfig.h"
#include "config.h"
#include "tools.h"
#include "speed-meter.h"
#include "compsor-mine.h"


extern OS_Global_struct __os_global_struct;                     // global variable for system
CompositorCtxt * cc_context = NULL;                             // CompositorCtxt for composite_layers
static MSGHOOK m_OldMouseHook = NULL;                           // mouse hook function
static MoseSpeed mouseinfo;                                     // for drag screen
static MGEFF_ANIMATION animation = NULL;                        // handle of animation

static BOOL check_runner_flag(void)
{
    BOOL ret = FALSE;
    int cur_clientId = 0;
    int idx_topmost = 0;
    int znode_index = 0;
    page_struct * page = find_page_by_id(__os_global_struct.current_page + 1);

    idx_topmost = ServerGetTopmostZNodeOfType(page->layer, ZOF_TYPE_NORMAL, &cur_clientId);
    if(idx_topmost > 0)
    {
        // get the number of app in normal level
        znode_index = ServerGetNextZNode(page->layer, 0, &cur_clientId);
        while(znode_index > 0)
        {
            ZNODEINFO znodeinfo;

            if(ServerGetZNodeInfo(page->layer, znode_index, &znodeinfo))
            {
                if((znodeinfo.type & ZOF_TYPE_MASK) == ZOF_TYPE_NORMAL)     // it the main window in normal level
                {
                    if(znodeinfo.cli = mouseinfo.cli)
                    {
                        if((znodeinfo.flags & ZOF_GF_SWIPE_VERT) || \
                                (znodeinfo.flags & ZOF_GF_SWIPE_HORZ))
                            ret = TRUE;
                        break;
                    }
                }
            }
            znode_index = ServerGetNextZNode(NULL, znode_index, &cur_clientId);
        }
    }

    return ret;
}

static BOOL check_timeout(HWND hWnd, LINT id, DWORD tickCount) 
{
    int cur_clientId = 0;
    int idx_topmost = 0;
    int znode_index = 0;
    page_struct * page = find_page_by_id(__os_global_struct.current_page + 1);

    idx_topmost = ServerGetTopmostZNodeOfType(page->layer, ZOF_TYPE_NORMAL, &cur_clientId);
    if(idx_topmost > 0)
    {
        // get the number of app in normal level
        znode_index = ServerGetNextZNode(page->layer, 0, &cur_clientId);
        while(znode_index > 0)
        {
            ZNODEINFO znodeinfo;

            if(ServerGetZNodeInfo(page->layer, znode_index, &znodeinfo))
            {
                if((znodeinfo.type & ZOF_TYPE_MASK) == ZOF_TYPE_NORMAL)     // it the main window in normal level
                {
                    if(znodeinfo.cli = mouseinfo.cli)
                    {
                        if((znodeinfo.flags & ZOF_GF_SWIPE_VERT) || \
                                (znodeinfo.flags & ZOF_GF_SWIPE_HORZ))
                            mouseinfo.goon = TRUE;
                        else
                            mouseinfo.goon = FALSE;
                        break;
                    }
                }
            }
            znode_index = ServerGetNextZNode(NULL, znode_index, &cur_clientId);
        }
    }

    if (!mouseinfo.m_bTimedout) 
    {
        mouseinfo.m_bTimedout = ((GetTickCount() - mouseinfo.m_timePressed) >= PRESS_TIMEOUT);
        if(mouseinfo.m_bTimedout) 
        {
            LPARAM lParam = MAKELONG(mouseinfo.m_oldMousePos.x, mouseinfo.m_oldMousePos.y);
            PostMessage(HWND_DESKTOP, MSG_LBUTTONDOWN, 0, lParam);
        }
    }
    KillTimer(HWND_DESKTOP, ID_TIMER_MOUSE);

    return TRUE;
}

static void my_calc_mainwin_pos (CompositorCtxt* ctxt, MG_Layer* layer,
        DWORD zt_type, int first_for_type, int cli, CALCPOSINFO* info)
{
    MG_Client* client = mgClients + cli;

    cc_context = ctxt;

    runner_struct * runner = NULL;
    runner = find_pid(client->pid);

    if(runner)
    {
        info->rc.left = runner->lx;
        info->rc.top = runner->ty;
        info->rc.right = runner->rx;
        info->rc.bottom = runner->by;
    }
    return;
}

static void my_transit_to_layer (CompositorCtxt* ctxt, MG_Layer* to_layer)
{
}

// callback function of animation
static void animated_cb(MGEFF_ANIMATION handle, HWND hWnd, int id, int *value)
{
    int onetime = 0;
    int percent = 0;
    animation_param * param = (animation_param *)mGEffAnimationGetTarget(handle);

    percent =  *value * 100 / param->end;

    if((float)percent != param->cpf.percent)
    {
        param->cpf.percent = (float)percent;
        onetime = param->fallback_ops->composite_layers(cc_context, param->layers, 2, &(param->cpf));
    }
}

// the function which will be invoked at the end of animation
static void animated_end(MGEFF_ANIMATION handle)
{
    animation_param * param = (animation_param *)mGEffAnimationGetTarget(handle);
    mGEffAnimationDelete(animation);
    animation = NULL;

    if(param)
        free(param);
}

static void create_animation(animation_param * param, int start, int end)
{
    if(animation)
    {
        mGEffAnimationDelete(animation);
        animation = NULL;
    }

    animation = mGEffAnimationCreate((void *)param, (void *)animated_cb, 1, MGEFF_INT);
    if (animation)
    {
        int duration = 0;
        enum EffMotionType motionType = OutQuad;

        duration = 500;

        mGEffAnimationSetStartValue(animation, &start);
        mGEffAnimationSetEndValue(animation, &end);
        mGEffAnimationSetDuration(animation, duration);
        mGEffAnimationSetCurve(animation, motionType);
        mGEffAnimationSetFinishedCb(animation, animated_end);
        mGEffAnimationSyncRun(animation);
    }
}

static void show_page(BOOL next, MG_Layer ** layers, int x, int y)
{
    int step = 0;
    int start = 0;
    int end = 0;
    int page = 0;
    CompositorOps* fallback_ops = (CompositorOps*)ServerGetCompositorOps (COMPSOR_NAME_FALLBACK);
    CC_TRANSIT_TO_LAYER old_transit_to_lay = fallback_ops->transit_to_layer;
    COMBPARAMS_FALLBACK cpf;
    animation_param * param = NULL;

    if(__os_global_struct.direction == SCREEN_ANIMATION_HORIZENTAL)
    {       
        cpf.method = FCM_HORIZONTAL;
        step = 5 * g_rcScr.right / 100;
        end = g_rcScr.right;
        if(next)
        {
            page = 1;
            start = g_rcScr.right - (mouseinfo.m_oldMousePos.x - x);
        }
        else
        {
            page = 0;
            start = x - mouseinfo.m_oldMousePos.x;
        }
    }
    else
    {
        cpf.method = FCM_VERTICAL;
        step = 5 * g_rcScr.bottom / 100;
        end = g_rcScr.bottom;
        if(next)
        {
            page = 1;
            start = g_rcScr.bottom - (mouseinfo.m_oldMousePos.y - y);
        }
        else
        {
            page = 0;
            start = y - mouseinfo.m_oldMousePos.y;
        }
    }
        
    param = malloc(sizeof(animation_param));
    param->layers = layers;
    param->cpf = cpf;
    param->end = end;
    param->fallback_ops = fallback_ops;

    if(next)                 // next layer
    {       
        create_animation(param, start, 0);
#if 0
        for(; start > 0; start -= step)
        {   
            cpf.percent = start * 100 / end;
            fallback_ops->composite_layers(cc_context, layers, 2, &cpf);
            usleep (20 * 1000);
        }
#endif
        __os_global_struct.current_page ++;
    }
    else                        // prev layer
    {
        create_animation(param, start, end);
#if 0
        for(; start < end; start += step)
        {
            cpf.percent = start * 100 / end;
            fallback_ops->composite_layers(cc_context, layers, 2, &cpf);
            usleep (20 * 1000);
        }
#endif
        __os_global_struct.current_page --;
    }

    fallback_ops->transit_to_layer = my_transit_to_layer;
    ServerSetTopmostLayer(layers[page]);
    fallback_ops->transit_to_layer = old_transit_to_lay;

    if(__os_global_struct.hTitleBar)
        SendMessage(__os_global_struct.hTitleBar, MSG_MAINWINDOW_CHANGE, 0, 0);

    if(__os_global_struct.hIndicatorBar)
        SendMessage(__os_global_struct.hIndicatorBar, MSG_MAINWINDOW_CHANGE, (WPARAM)page, 0);
}

static void show_current_page(BOOL next, MG_Layer ** layers, int x, int y)
{
    int step = 0;
    int start = 0;
    int end = 0;
    int page = 0;
    CompositorOps* fallback_ops = (CompositorOps*)ServerGetCompositorOps (COMPSOR_NAME_FALLBACK);
    CC_TRANSIT_TO_LAYER old_transit_to_lay = fallback_ops->transit_to_layer;
    COMBPARAMS_FALLBACK cpf;
    animation_param * param = NULL;

    if(__os_global_struct.direction == SCREEN_ANIMATION_HORIZENTAL)
    {       
        cpf.method = FCM_HORIZONTAL;
        step = 5 * g_rcScr.right / 100;
        end = g_rcScr.right;
        if(next)
        {
            page = 0;
            start = g_rcScr.right - (mouseinfo.m_oldMousePos.x - x);
        }
        else
        {
            page = 1;
            start = x - mouseinfo.m_oldMousePos.x;
        }
    }
    else
    {
        cpf.method = FCM_VERTICAL;
        step = 5 * g_rcScr.bottom / 100;
        end = g_rcScr.bottom;
        if(next)
        {
            page = 1;
            start = g_rcScr.bottom - (mouseinfo.m_oldMousePos.y - y);
        }
        else
        {
            page = 0;
            start = y - mouseinfo.m_oldMousePos.y;
        }
    }
        
    param = malloc(sizeof(animation_param));
    param->layers = layers;
    param->cpf = cpf;
    param->end = end;
    param->fallback_ops = fallback_ops;

    if(next)                 // next layer
    {       
        create_animation(param, start, end);
#if 0
        for(; start < end; start += step)
        {   
            cpf.percent = start * 100 / end;
            fallback_ops->composite_layers(cc_context, layers, 2, &cpf);
            usleep (20 * 1000);
        }
        cpf.percent = 100;
#endif
    }
    else                        // prev layer
    {
        create_animation(param, start, 0);
#if 0
        for(; start > 0; start -= step)
        {
            cpf.percent = start * 100 / end;
            fallback_ops->composite_layers(cc_context, layers, 2, &cpf);
            usleep (20 * 1000);
        }
        cpf.percent = 0;
#endif
    }

//    fallback_ops->composite_layers(cc_context, layers, 2, &cpf);
    fallback_ops->refresh(cc_context);

}

static runner_struct * get_point_hwnd(int x, int y)
{
    runner_struct * ret_runner = NULL;
    page_struct * page = find_page_by_id(__os_global_struct.current_page + 1);
    RECT rect;

    if(page)
    {
        runner_struct * runner = page->runner;

        while(runner)
        {
            rect.left = runner->lx;
            rect.top = runner->ty;
            rect.right = runner->rx;
            rect.bottom = runner->by;

            if(PtInRect (&rect, x, y))
            {
                ret_runner = runner;
                break;
            }
            
            runner = runner->next;
        }
    }

    return ret_runner;
}

static int MouseHook(void* context, HWND dst_wnd, UINT msg, WPARAM wparam, LPARAM lParam)
{
//    runner_struct * runner = NULL;

    if(msg == MSG_LBUTTONDOWN)
    {
        int x = LOSWORD(lParam);
        int y = HISWORD(lParam);
//        runner_struct * runner = NULL;
        int cli = 0;

        // title 和 indicatior 中的消息，自行处理
        if(PtInRect (&__os_global_struct.rect_title, x, y))
        {
            mouseinfo.goon = TRUE;
            return HOOK_GOON;
        }

        if(PtInRect (&__os_global_struct.rect_indicator, x, y))
        {
            mouseinfo.goon = TRUE;
            return HOOK_GOON;
        }

        if(PtInRect (&__os_global_struct.rect_description, x, y))
        {
            mouseinfo.goon = TRUE;
            return HOOK_GOON;
        }

        if(check_runner_flag())
        {
            mouseinfo.goon = TRUE;
            return HOOK_GOON;
        }

#if 0
        // 在 page 中的消息
        runner = get_point_hwnd(x, y);
        if(runner)
        {
            MSG msg;

            cli = find_clientId_by_pid(runner->pid);
            msg.message = MSG_GESTURETEST;
            msg.hwnd = runner->hwnd;
            msg.wParam = 0;
            Send2Client(&msg, cli);
        }
#endif

        if(mouseinfo.m_speedmeter) 
        {
            mSpeedMeter_destroy(mouseinfo.m_speedmeter);
            mouseinfo.m_speedmeter = NULL;
        }

        memset(&mouseinfo, 0, sizeof(MoseSpeed));
       
        mouseinfo.m_bPressed = TRUE;
        mouseinfo.m_mouseFlag = 0;
        mouseinfo.m_bTimedout = FALSE;
        mouseinfo.m_timePressed = GetTickCount();
        mouseinfo.m_oldMousePos.x = x;
        mouseinfo.m_oldMousePos.y = y;
        mouseinfo.m_pressMousePos = mouseinfo.m_oldMousePos;
        mouseinfo.cli = cli;
        
        KillTimer(HWND_DESKTOP, ID_TIMER_MOUSE);
        SetTimerEx(HWND_DESKTOP, ID_TIMER_MOUSE, PRESS_TIMEOUT, check_timeout);

        return HOOK_STOP;
    }
    else if(msg == MSG_LBUTTONUP)
    {
        if(mouseinfo.goon)
            return HOOK_GOON;
        if (!mouseinfo.m_bPressed) {
            return HOOK_GOON;
        }

        if((mouseinfo.m_mouseFlag & 0x02) == 0) 
        {
            int flag;
            if ((mouseinfo.m_mouseFlag & 0x01) == 0) 
            {
                flag = 0;
            }
            else 
            {
                if (GetTickCount() < mouseinfo.m_timePressed + CLICK_TIMEOUT
                        && (ABS(LOSWORD(lParam) - mouseinfo.m_pressMousePos.x)
                            + ABS(HISWORD(lParam) - mouseinfo.m_pressMousePos.y) <
                            CLICK_MICRO_MOVEMENT)) 
                    flag = 0;
                else 
                    flag = 1;
            }

            if(flag == 0) 
            {
                mouseinfo.m_timePressed -= PRESS_TIMEOUT;
                check_timeout(HWND_DESKTOP, ID_TIMER_MOUSE, PRESS_TIMEOUT);
            }
        }
        if(mouseinfo.m_bTimedout || mouseinfo.goon) 
            return HOOK_GOON;
        else if(mouseinfo.m_bMouseMoved) 
        {
            // do animation
            int x = LOSWORD(lParam);
            int y = HISWORD(lParam);
            float v_x = 0.0;
            float v_y = 0.0;
            MG_Layer * layers[2];
            page_struct * page = NULL;
            float speed = 0.0;
            int start = 0;
            int end = 0;

            lParam = MAKELONG (x, y);

            mouseinfo.m_bPressed = FALSE;
            mSpeedMeter_append(mouseinfo.m_speedmeter, x, y, GetTickCount() * 10);
            mSpeedMeter_stop(mouseinfo.m_speedmeter);
            mSpeedMeter_query_velocity(mouseinfo.m_speedmeter, &v_x, &v_y);

            // do animation
            page = find_page_by_id(__os_global_struct.current_page + 1);
            if(page == NULL)
                return HOOK_GOON;
                
            if(__os_global_struct.direction == SCREEN_ANIMATION_HORIZENTAL)
            {
                speed = v_x;
                start = x;
                end = mouseinfo.m_oldMousePos.x;
            }
            else
            {
                speed = v_y;
                start = y;
                end = mouseinfo.m_oldMousePos.y;
            }

            if((abs(speed) <= LIMIT_SPEED) || ((speed * mouseinfo.direction) <= 0)) // rollback
            {
                if((start - end) < 0)
                {
                    layers[0] = page->layer;
                    page = find_next_page(page, LAYER_CIRCLE);
                    if(page)
                    {
                        layers[1] = page->layer;
                        if(layers[0] == layers[1])    // when in circle mode, maybe one layer
                            return HOOK_GOON;
                    }
                    else
                        return HOOK_GOON;

                    show_current_page(TRUE, layers, x, y);
                }
                else                        // prev layer
                {
                    layers[1] = page->layer;
                    page = find_prev_page(page, LAYER_CIRCLE);
                    if(page)
                    {
                        layers[0] = page->layer;
                        if(layers[0] == layers[1])    // when in circle mode, maybe one layer
                            return HOOK_GOON;
                    }
                    else
                        return HOOK_GOON;

                    show_current_page(FALSE, layers, x, y);
                }
            }
            else
            {
                if(speed < 0)                // next layer
                {
                    layers[0] = page->layer;
                    page = find_next_page(page, LAYER_CIRCLE);
                    if(page)
                    {
                        layers[1] = page->layer;
                        if(layers[0] == layers[1])    // when in circle mode, maybe one layer
                            return HOOK_GOON;
                    }
                    else
                        return HOOK_GOON;

                    show_page(TRUE, layers, x, y);
                }
                else                        // prev layer
                {
                    layers[1] = page->layer;
                    page = find_prev_page(page, LAYER_CIRCLE);
                    if(page)
                    {
                        layers[0] = page->layer;
                        if(layers[0] == layers[1])    // when in circle mode, maybe one layer
                            return HOOK_GOON;
                    }
                    else
                        return HOOK_GOON;

                    show_page(FALSE, layers, x, y);
                }
            }
        }

        if(mouseinfo.m_speedmeter) 
        {
            mSpeedMeter_destroy(mouseinfo.m_speedmeter);
            mouseinfo.m_speedmeter = NULL;
        }
        return HOOK_STOP;
    }
    else if(msg == MSG_MOUSEMOVE)
    {
        if(mouseinfo.goon)
            return HOOK_GOON;

        if(!mouseinfo.m_bPressed) 
            return HOOK_GOON;

        mouseinfo.m_bMouseMoved = TRUE;

        if(mouseinfo.m_mouseFlag == 0) 
        {
            mouseinfo.m_mouseFlag |= 0x01;
            check_timeout(HWND_DESKTOP, ID_TIMER_MOUSE, PRESS_TIMEOUT);
        }

        if (mouseinfo.m_bTimedout || mouseinfo.goon) 
            return HOOK_GOON;
        else 
        {
            COMBPARAMS_FALLBACK cpf;
            int x = LOSWORD(lParam);
            int y = HISWORD(lParam);
            int delt = 0;
            MG_Layer * layers[2];
            page_struct * page = NULL;
            int end = 0;

            if (mouseinfo.first) 
            {
                lParam = MAKELONG(x, y);
                assert(mouseinfo.m_speedmeter);
                mSpeedMeter_append(mouseinfo.m_speedmeter, x, y, GetTickCount() * 10);
            }
            else 
            {
                assert(mouseinfo.m_speedmeter == NULL);
                mouseinfo.m_speedmeter = mSpeedMeter_create (1000, 10);
                mSpeedMeter_reset(mouseinfo.m_speedmeter);
                mSpeedMeter_append(mouseinfo.m_speedmeter, mouseinfo.m_oldMousePos.x,
                        mouseinfo.m_oldMousePos.y, GetTickCount() * 10);
                mouseinfo.first = 1;
            }

            // do animation
            page = find_page_by_id(__os_global_struct.current_page + 1);
            if(page)
            {
                if(__os_global_struct.direction == SCREEN_ANIMATION_HORIZENTAL)
                {
                    delt = x - mouseinfo.m_oldMousePos.x;
                    cpf.method = FCM_HORIZONTAL;
                    end = g_rcScr.right;
                }
                else
                {
                    delt = y - mouseinfo.m_oldMousePos.y;
                    cpf.method = FCM_VERTICAL;
                    end = g_rcScr.bottom;
                }

                if(delt < 0)
                {
                    cpf.percent = -1 * delt * 100 / end;
                    cpf.percent = 100 - cpf.percent;

                    layers[0] = page->layer;
                    page = find_next_page(page, LAYER_CIRCLE);
                    if(page)
                    {
                        layers[1] = page->layer;
                        if(layers[0] == layers[1])    // when in circle mode, maybe one layer
                            return HOOK_GOON;
                    }
                    else
                        return HOOK_GOON;
                }
                else
                {
                    cpf.percent = delt * 100 / end;

                    layers[1] = page->layer;
                    page = find_prev_page(page, LAYER_CIRCLE);
                    if(page)
                    {
                        layers[0] = page->layer;
                        if(layers[0] == layers[1])    // when in circle mode, maybe one layer
                            return HOOK_GOON;
                    }
                    else
                        return HOOK_GOON;
                }

                if(mouseinfo.direction == 0)
                    mouseinfo.direction = delt;

                if((cpf.percent >= 0) && (mouseinfo.percent != cpf.percent))
                {
                    CompositorOps* fallback_ops = NULL;
                    fallback_ops = (CompositorOps*)ServerGetCompositorOps (COMPSOR_NAME_FALLBACK);
                    fallback_ops->composite_layers(cc_context, layers, 2, &cpf);
                    mouseinfo.percent = cpf.percent;
                }
            }
            else
                return HOOK_GOON;
        }

        return HOOK_STOP;
    }

    return HOOK_GOON;
}

void start_my_compositor(void)
{
    CompositorOps* fallback_ops = NULL;
    fallback_ops = (CompositorOps*)ServerGetCompositorOps (COMPSOR_NAME_FALLBACK);
    fallback_ops->calc_mainwin_pos = my_calc_mainwin_pos;

    m_OldMouseHook = RegisterEventHookFunc(HOOK_EVENT_MOUSE, MouseHook, NULL);
}
