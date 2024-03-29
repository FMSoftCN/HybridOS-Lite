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
** indicator.c: It illustrates how to program indicator bar.
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
#include <math.h>
#include <libgen.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <mgeff/mgeff.h>
#include <hibus.h>

#include <cairo/cairo.h>
#include <cairo/cairo-minigui.h>

#ifdef CAIRO_HAS_DRM_SURFACE
#include <cairo/cairo-drm.h>
#endif

#include <glib.h>
#include <hisvg.h>

#include "../include/sysconfig.h"
#include "../include/svgpaint.h"
#include "config.h"
#include "tools.h"
#include "title-description.h"

#define MAX_SQUARE      11
#define SQUARE_LENGTH   12 
#define SELECT_COLOR    "#FFFFFF"
#define UNSELECT_COLOR  "#C0C0C0"
#define UNSELECT_RATIO  1.5       // select raius / unselect radius

typedef void (* CC_TRANSIT_TO_LAYER) (CompositorCtxt* ctxt, MG_Layer* to_layer);
static int square_interval = 0;

typedef struct tagIndicator
{
    int start;          // indicate the index of start point
    int select;         // indicate the index of selected point
} Indicator;

extern OS_Global_struct __os_global_struct;
extern CompositorCtxt * cc_context;
static Indicator indicator;

static HiSVGHandle * icon_handle[2];
static char color_style[2][64];

static void my_transit_to_layer (CompositorCtxt* ctxt, MG_Layer* to_layer)
{
}

static void paint(HWND hwnd, HDC hdc, int square_number, RECT * rect)
{
    int i = 0;
    int select = 0;
    float alpha = BK_TRANSPARENT / 255.0;
    int index = 0;
    RECT draw_rect;

    for(i = 0; i < square_number; i++)
    {
        if(i == indicator.select)
            select = 0;
        else
            select = 1;

        SetBrushColor (hdc, RGB2Pixel (hdc, BK_COLOR_R, BK_COLOR_G, \
                    BK_COLOR_B));
        FillBox(hdc, (rect + i)->left, (rect + i)->top, \
                RECTWP(rect + i), RECTHP(rect + i));

        if(select == 0)
        {
            draw_rect.left = (rect + i)->left;
            draw_rect.top = (rect + i)->top;
            draw_rect.right = (rect + i)->right;
            draw_rect.bottom = (rect + i)->bottom;
        }
        else
        {
            draw_rect.left = (rect + i)->left + SQUARE_LENGTH  * (1.0 - (1.0 / (float)UNSELECT_RATIO)) / 2.0;
            draw_rect.top = (rect + i)->top + SQUARE_LENGTH *  (1.0 - (1.0 / (float)UNSELECT_RATIO)) / 2.0;
            draw_rect.right = draw_rect.left + SQUARE_LENGTH / UNSELECT_RATIO;
            draw_rect.bottom = draw_rect.top + SQUARE_LENGTH / UNSELECT_RATIO;
        }
        paint_svg(hwnd, hdc, draw_rect, icon_handle[select], color_style[select]);
    }

    return;
}
static void calculate_points(void)
{
    if(__os_global_struct.page_number <= MAX_SQUARE)
    {
        indicator.start = 0;
        indicator.select = __os_global_struct.current_page;
    }
    else
    {
        int head = __os_global_struct.current_page - MAX_SQUARE / 2;
        int tail = __os_global_struct.current_page + MAX_SQUARE / 2;

        if(head < 0)
        {
            indicator.start =  0;
            indicator.select = __os_global_struct.current_page;
        }
        else if(tail > (__os_global_struct.page_number - 1))
        {
            indicator.start = __os_global_struct.page_number - MAX_SQUARE;
            indicator.select = __os_global_struct.current_page - indicator.start;
        }
        else
        {
            indicator.start = __os_global_struct.current_page - MAX_SQUARE / 2;
            indicator.select = __os_global_struct.current_page - indicator.start;
        }
    }
}

static void calculate_next(BOOL add)
{
    if(__os_global_struct.page_number <= MAX_SQUARE)
    {
        indicator.start = 0;
        if(__os_global_struct.current_page < (__os_global_struct.page_number - 1))
        {
            if(add)
                __os_global_struct.current_page ++;
        }
        indicator.select = __os_global_struct.current_page;
    }
    else
    {
        if(indicator.select < (MAX_SQUARE - 1))
        {
            indicator.select ++;
            if(add)
                __os_global_struct.current_page ++;
        }
        else
        {
            if((indicator.start + MAX_SQUARE) >= __os_global_struct.page_number)
            {
            }
            else
            {
                indicator.start ++;
                if(add)
                    __os_global_struct.current_page ++;
            }
        }
    }
}


static void calculate_pre(BOOL sub)
{
    if(__os_global_struct.page_number <= MAX_SQUARE)
    {
        indicator.start = 0;
        if(__os_global_struct.current_page > 0)
        {
            if(sub)
                __os_global_struct.current_page --;
        }
        indicator.select = __os_global_struct.current_page;
    }
    else
    {
        if(indicator.select > 0)
        {
            indicator.select --;
            if(sub)
                __os_global_struct.current_page --;
        }
        else
        {
            if(indicator.start <= 0)
            {
            }
            else
            {
                indicator.start --;
                if(sub)
                    __os_global_struct.current_page --;
            }
        }
    }
}

// the window proc of indicator bar
static LRESULT IndicatorBarWinProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    static RECT rect[MAX_SQUARE];
    static int square_number = 0;
    static BOOL pressed = FALSE;

    switch (message) 
    {
        case MSG_PAINT:
            hdc = BeginPaint (hWnd);
            paint(hWnd, hdc, square_number, rect);
            EndPaint (hWnd, hdc);
            return 0;

        case MSG_CREATE:
        {
            int i = 0;
            int startx = 0;
            int starty = 0;
            int width = 0;
            char path[HISHELL_MAX_PATH] = {0};
            RECT window_rect;

            GetWindowRect(hWnd, &window_rect);

            square_interval = (RECTW(window_rect) * 6 / 10) / MAX_SQUARE;

            square_number = __os_global_struct.page_number < MAX_SQUARE ? \
                        __os_global_struct.page_number: MAX_SQUARE;
            width = square_number * SQUARE_LENGTH + (square_number - 1) * square_interval;

            startx = (RECTW(window_rect) - width) / 2;
            starty = (RECTH(window_rect) - SQUARE_LENGTH) / 2;

            // calculate rect array.
            for(i = 0; i < square_number; i++)
            {
                rect[i].left = startx + i * (SQUARE_LENGTH + square_interval);
                rect[i].top = starty;
                rect[i].right = rect[i].left + SQUARE_LENGTH;
                rect[i].bottom = rect[i].top + SQUARE_LENGTH;
            }

            readlink("/proc/self/exe", path, HISHELL_MAX_PATH);
            sprintf(path, "%s/res/circle.svg", dirname(path));

            strcpy(color_style[0], "svg { color:");
            strcat(color_style[0], SELECT_COLOR);
            strcat(color_style[0], "; } ");
            loadSVGFromFile(path, &icon_handle[0]);

            strcpy(color_style[1], "svg { color:");
            strcat(color_style[1], UNSELECT_COLOR);
            strcat(color_style[1], "; } ");
            loadSVGFromFile(path, &icon_handle[1]);
        }
            break;

        case MSG_CONFIG_CHANGE:
        {
            int i = 0;
            int startx = 0;
            int starty = 0;
            int width = 0;
            RECT window_rect;

            square_number = __os_global_struct.page_number < MAX_SQUARE ? \
                        __os_global_struct.page_number: MAX_SQUARE;
            width = square_number * SQUARE_LENGTH + (square_number - 1) * square_interval;

            GetWindowRect(hWnd, &window_rect);
            startx = (RECTW(window_rect) - width) / 2;
            starty = (RECTH(window_rect) - SQUARE_LENGTH) / 2;

            // calculate rect array.
            for(i = 0; i < square_number; i++)
            {
                rect[i].left = startx + i * (SQUARE_LENGTH + square_interval);
                rect[i].top = starty;
                rect[i].right = rect[i].left + SQUARE_LENGTH;
                rect[i].bottom = rect[i].top + SQUARE_LENGTH;
            }
            calculate_points();
            InvalidateRect(hWnd, NULL, TRUE);
        }
            break;

        case MSG_LBUTTONDOWN:
        {
            int i = 0;
            int j = 0;
            int x = LOWORD (lParam);
            int y = HIWORD (lParam);
            page_struct * page = NULL;
            CompositorOps* fallback_ops = (CompositorOps*)ServerGetCompositorOps (COMPSOR_NAME_FALLBACK);
            COMBPARAMS_FALLBACK cpf;
            MG_Layer * layers[2];

            if(square_number == 0)
                break;

            // whether current range 
            for(i = 0; i < square_number; i++)
            {
                if(PtInRect ((rect + i), x, y))
                    break;
            }

            if(i < square_number)   // in current range
            {
                pressed = TRUE;
                if(indicator.select == i)   // current page
                {
                    page = find_page_by_id(__os_global_struct.current_page + 1);
                    layers[0] = mgTopmostLayer;
                    layers[1] = page->layer;

                    cpf.method = FCM_HORIZONTAL | FCM_SCALE;
                    cpf.percent = 100.0;
                    cpf.scale = 0.95;

                    for(i = 0; i < 4; i++)
                    {
                        cpf.scale -= 0.05;
                        fallback_ops->composite_layers(cc_context, layers, 2, &cpf);
                        usleep (20 * 1000);
                    }
                }
                else                        // not current page
                {
                    CC_TRANSIT_TO_LAYER old_transit_to_lay = fallback_ops->transit_to_layer;

                    // zoom in current page
                    page = find_page_by_id(__os_global_struct.current_page + 1);
                    layers[0] = mgTopmostLayer;
                    layers[1] = page->layer;

                    cpf.method = FCM_HORIZONTAL | FCM_SCALE;
                    cpf.percent = 100.0;
                    cpf.scale = 0.95;

                    for(j = 0; j < 4; j++)
                    {
                        cpf.scale -= 0.05;
                        fallback_ops->composite_layers(cc_context, layers, 2, &cpf);
                        usleep (20 * 1000);
                    }

                    // switch current page
                    layers[0] = page->layer;

                    indicator.select = i;
                    __os_global_struct.current_page = indicator.start + i;
                    page = find_page_by_id(__os_global_struct.current_page + 1);

                    // animation，switch page
                    if(page)
                    {
                        layers[1] = page->layer;

                        cpf.method = FCM_HORIZONTAL | FCM_SCALE;
                        cpf.percent = 95.0;
                        cpf.scale = 0.80;

                        for(i = 0; i < 20; i++)
                        {
                            cpf.percent -= 5.0; 
                            fallback_ops->composite_layers(cc_context, layers, 2, &cpf);
                            usleep (20 * 1000);
                        }

                        fallback_ops->transit_to_layer = my_transit_to_layer;
                        ServerSetTopmostLayer(page->layer);
                        fallback_ops->transit_to_layer = old_transit_to_lay;

                        hdc = GetClientDC(hWnd);
                        paint(hWnd, hdc, square_number, rect);
                        ReleaseDC(hdc);

                        layers[0] = mgTopmostLayer;
                        layers[1] = page->layer;

                        cpf.method = FCM_HORIZONTAL | FCM_SCALE;
                        cpf.percent = 100.0;
                        cpf.scale = 0.80;
                        fallback_ops->composite_layers(cc_context, layers, 2, &cpf);
                    }

                    if(__os_global_struct.hTitleBar)
                        SendMessage(__os_global_struct.hTitleBar, MSG_MAINWINDOW_CHANGE, 0, 0);
                }
            }
        }
            break;

        case MSG_MOUSEMOVE:
        {
            int i = 0;
            int x = LOWORD (lParam);
            int y = HIWORD (lParam);
            page_struct * page = NULL;
            CompositorOps* fallback_ops = (CompositorOps*)ServerGetCompositorOps (COMPSOR_NAME_FALLBACK);
            COMBPARAMS_FALLBACK cpf;
            MG_Layer * layers[2];
            RECT field;

            if(square_number == 0)
                break;

            if(!pressed)
                break;

            field.left = (rect + 0)->left - 10;
            field.top = (rect + 0)->top;
            field.right = (rect + square_number - 1)->right + 10;
            field.bottom = (rect + square_number - 1)->bottom;
            
            if(!PtInRect (&field, x, y))
            {
                CC_TRANSIT_TO_LAYER old_transit_to_lay = fallback_ops->transit_to_layer;

                page = find_page_by_id(__os_global_struct.current_page + 1);
                layers[0] = mgTopmostLayer;
                layers[1] = page->layer;

                cpf.method = FCM_HORIZONTAL | FCM_SCALE;
                cpf.percent = 100.0;
                cpf.scale = 0.85;

                for(i = 0; i < 4; i++)
                {
                    cpf.scale += 0.05;
                    fallback_ops->composite_layers(cc_context, layers, 2, &cpf);
                    usleep (20 * 1000);
                }

                fallback_ops->transit_to_layer = my_transit_to_layer;
                ServerSetTopmostLayer(page->layer);
                fallback_ops->transit_to_layer = old_transit_to_lay;
                fallback_ops->refresh(cc_context);;
                
                pressed = FALSE;

                break;
            }

            for(i = 0; i < square_number; i++)
            {
                if(PtInRect ((rect + i), x, y))
                    break;
            }

            if(i < square_number)
            {
                if(indicator.select == i)
                {
                }
                else
                {
                    CC_TRANSIT_TO_LAYER old_transit_to_lay = fallback_ops->transit_to_layer;
                    page = find_page_by_id(__os_global_struct.current_page + 1);
                    layers[0] = page->layer;

                    indicator.select = i;
                    __os_global_struct.current_page = indicator.start + i;
                    page = find_page_by_id(__os_global_struct.current_page + 1);
                    if(page)
                    {
                        fallback_ops->transit_to_layer = my_transit_to_layer;
                        ServerSetTopmostLayer(page->layer);
                        fallback_ops->transit_to_layer = old_transit_to_lay;

                        hdc = GetClientDC(hWnd);
                        paint(hWnd, hdc, square_number, rect);
                        ReleaseDC(hdc);

                        layers[0] = mgTopmostLayer;
                        layers[1] = page->layer;

                        cpf.method = FCM_HORIZONTAL | FCM_SCALE;
                        cpf.percent = 100.0;
                        cpf.scale = 0.80;
                        fallback_ops->composite_layers(cc_context, layers, 2, &cpf);
                    }

                    if(__os_global_struct.hTitleBar)
                        SendMessage(__os_global_struct.hTitleBar, MSG_MAINWINDOW_CHANGE, 0, 0);
                }
            }
            else
            {
                if((x < rect[0].left) || (x > rect[square_number - 1].right))
                    break;

                for(i = 0; i < square_number - 1; i++)
                {
                    if((x > rect[i].right) && (x < rect[i + 1].left))
                        break;
                }

                if(i < (square_number - 1))
                {
                    int internal = rect[i + 1].left - rect[i].right;
                    float percent = (float)(x - rect[i].right) * 100.0 / (float)internal;

                    page = find_page_by_id(i + 1);
                    layers[0] = page->layer;

                    page = find_page_by_id(i + 1 + 1);
                    layers[1] = page->layer;

                    cpf.method = FCM_HORIZONTAL | FCM_SCALE;
                    cpf.percent = 100.0f - percent;
                    cpf.scale = 0.80;

                    fallback_ops->composite_layers(cc_context, layers, 2, &cpf);
                }
            }
        }   

            break;

        case MSG_LBUTTONUP:
        {
            int i = 0;
            int x = LOWORD (lParam);
            int y = HIWORD (lParam);
            page_struct * page = NULL;
            CompositorOps* fallback_ops = (CompositorOps*)ServerGetCompositorOps (COMPSOR_NAME_FALLBACK);
            COMBPARAMS_FALLBACK cpf;
            MG_Layer * layers[2];

            if(square_number == 0)
                break;

            if(!pressed)
                break;

            for(i = 0; i < square_number; i++)
            {
                if(PtInRect ((rect + i), x, y))
                    break;
            }

            if(i < square_number)
            {
                if(indicator.select == i)
                {
                    CC_TRANSIT_TO_LAYER old_transit_to_lay = fallback_ops->transit_to_layer;

                    page = find_page_by_id(__os_global_struct.current_page + 1);
                    layers[0] = mgTopmostLayer;
                    layers[1] = page->layer;

                    cpf.method = FCM_HORIZONTAL | FCM_SCALE;
                    cpf.percent = 100.0;
                    cpf.scale = 0.85;

                    for(i = 0; i < 4; i++)
                    {
                        cpf.scale += 0.05;
                        fallback_ops->composite_layers(cc_context, layers, 2, &cpf);
                        usleep (20 * 1000);
                    }

                    fallback_ops->transit_to_layer = my_transit_to_layer;
                    ServerSetTopmostLayer(page->layer);
                    fallback_ops->transit_to_layer = old_transit_to_lay;
                    fallback_ops->refresh(cc_context);;
                }
            }
            else
            {
                CC_TRANSIT_TO_LAYER old_transit_to_lay = fallback_ops->transit_to_layer;

                page = find_page_by_id(__os_global_struct.current_page + 1);
                layers[0] = mgTopmostLayer;
                layers[1] = page->layer;

                cpf.method = FCM_HORIZONTAL | FCM_SCALE;
                cpf.percent = 100.0;
                cpf.scale = 0.85;

                for(i = 0; i < 4; i++)
                {
                    cpf.scale += 0.05;
                    fallback_ops->composite_layers(cc_context, layers, 2, &cpf);
                    usleep (20 * 1000);
                }

                fallback_ops->transit_to_layer = my_transit_to_layer;
                ServerSetTopmostLayer(page->layer);
                fallback_ops->transit_to_layer = old_transit_to_lay;
                fallback_ops->refresh(cc_context);;
            }
            pressed = FALSE;
        }   
            break;

        case MSG_MAINWINDOW_CHANGE:
            if(wParam == 1)             // next
                calculate_next(FALSE);
            else
                calculate_pre(FALSE);
            InvalidateRect(hWnd, NULL, TRUE);
            break;

        case MSG_DESTROY:
            break;

        case MSG_CLOSE:
            if(icon_handle[0])
               hisvg_handle_destroy(icon_handle[0]);
            if(icon_handle[1])
               hisvg_handle_destroy(icon_handle[1]);

            DestroyAllControls (hWnd);
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
            return 0;
    }

    return DefaultMainWinProc (hWnd, message, wParam, lParam);
}

// create indicator bar
HWND create_indicator_bar(void)
{
    MAINWINCREATE CreateInfo;

    // create a main window
    CreateInfo.dwStyle = WS_ABSSCRPOS | WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_TOOLWINDOW;
    CreateInfo.spCaption = "Task Bar";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor (0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = IndicatorBarWinProc;
    CreateInfo.lx = __os_global_struct.rect_indicator.left; 
    CreateInfo.ty = __os_global_struct.rect_indicator.top;
    CreateInfo.rx = __os_global_struct.rect_indicator.right;
    CreateInfo.by = __os_global_struct.rect_indicator.bottom;
    CreateInfo.iBkColor = COLOR_black;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

#if 1 
    __os_global_struct.hIndicatorBar = CreateMainWindowEx2 (&CreateInfo, \
                                        0L, NULL, NULL, ST_PIXEL_ARGB8888,
                                        MakeRGBA (BK_COLOR_R, BK_COLOR_G, \
                                                  BK_COLOR_B, BK_TRANSPARENT),\
                                        CT_ALPHAPIXEL, COLOR_BLEND_LEGACY);
#else
    __os_global_struct.hIndicatorBar = CreateMainWindow(&CreateInfo);
#endif

    if (__os_global_struct.hIndicatorBar == HWND_INVALID)
        return HWND_INVALID;

    return __os_global_struct.hIndicatorBar;
}

