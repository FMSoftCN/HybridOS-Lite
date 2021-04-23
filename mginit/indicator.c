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
 * \file bar.c
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
 * $Id: indicator.c 13674 2020-09-16 06:45:01Z Gengyue $
 *
 *      HybridOS for Linux, VxWorks, NuCleus, OSE.
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
#include <libhirsvg/rsvg.h>

#include "../include/sysconfig.h"
#include "config.h"
#include "tools.h"
#include "bar.h"

#define MAX_SQUARE      11
#define SQUARE_INTERVAL 12
#define SQUARE_LENGTH   12 
#define SELECT_COLOR    "#FFFFFF"
#define UNSELECT_COLOR  "#C0C0C0"
#define UNSELECT_RATIO  1.5       // select raius / unselect radius

typedef void (* CC_TRANSIT_TO_LAYER) (CompositorCtxt* ctxt, MG_Layer* to_layer);

typedef struct tagIndicator
{
    int start;          // indicate the index of start point
    int select;         // indicate the index of selected point
} Indicator;

extern OS_Global_struct __os_global_struct;
extern CompositorCtxt * cc_context;
static Indicator indicator;

static cairo_t *cr[2];
static cairo_surface_t *surface[2];
static RsvgStylePair button_color_pair[2];

static void my_transit_to_layer (CompositorCtxt* ctxt, MG_Layer* to_layer)
{
}

static HDC create_memdc_from_image_surface (cairo_surface_t* image_surface)
{
    MYBITMAP my_bmp = {
        flags: MYBMP_TYPE_RGB | MYBMP_FLOW_DOWN,
        frames: 1,
        depth: 32,
    };

    my_bmp.w = cairo_image_surface_get_width (image_surface);
    my_bmp.h = cairo_image_surface_get_height (image_surface);
    my_bmp.pitch = cairo_image_surface_get_stride (image_surface);
    my_bmp.bits = cairo_image_surface_get_data (image_surface);
    my_bmp.size = my_bmp.pitch * my_bmp.h;

    return CreateMemDCFromMyBitmap(&my_bmp, NULL);
}

static HDC destroy_memdc_for_image_surface(HDC hdc,
        cairo_surface_t* image_surface)
{
    DeleteMemDC (hdc);
}

static void paint(HWND hwnd, HDC hdc, int square_number, RECT * rect)
{
    int i = 0;
    int select = 0;
    float alpha = BK_TRANSPARENT / 255.0;

    for(i = 0; i < square_number; i++)
    {
        if(i == indicator.select)
            select = 0;
        else
            select = 1;

        HDC csdc = create_memdc_from_image_surface(surface[select]);
        if (csdc != HDC_SCREEN && csdc != HDC_INVALID)
        {
            SetBrushColor (hdc, RGB2Pixel (hdc, BK_COLOR_R, BK_COLOR_G, BK_COLOR_B));
            FillBox(hdc, (rect + i)->left, (rect + i)->top, \
                           RECTWP(rect + i), RECTHP(rect + i));

            if(select == 0)
            {
                SetMemDCColorKey(csdc, MEMDC_FLAG_SRCCOLORKEY,
                        MakeRGB(BK_COLOR_R * alpha, BK_COLOR_G * alpha, BK_COLOR_B * alpha));
                BitBlt(csdc, 0, 0, SQUARE_LENGTH, SQUARE_LENGTH, hdc, (rect + i)->left, (rect + i)->top, 0);
            }
            else
            {
                SetMemDCColorKey(csdc, MEMDC_FLAG_SRCCOLORKEY,
                        MakeRGB(BK_COLOR_R * alpha, BK_COLOR_G * alpha, BK_COLOR_B * alpha));
                BitBlt(csdc, 0, 0, SQUARE_LENGTH / UNSELECT_RATIO, SQUARE_LENGTH / UNSELECT_RATIO, hdc, \
                                (rect + i)->left + SQUARE_LENGTH  * (1.0 - (1.0 / (float)UNSELECT_RATIO)) / 2.0, \
                                (rect + i)->top + SQUARE_LENGTH *  (1.0 - (1.0 / (float)UNSELECT_RATIO)) / 2.0, 0);
            }
        }
        DeleteMemDC(csdc);
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

static void loadSVGFromFile(const char* file, int index)
{
    RsvgHandle *handle;
    GError *error = NULL;
    RsvgDimensionData dimensions;
    double factor_width = 0.0f;
    double factor_height = 0.0f;

    // read file from svg file
    handle = rsvg_handle_new_from_file(file, &error);
    if(error)
    {
        surface[index] = NULL;
        cr[index] = NULL;
        return;
    }
    rsvg_handle_get_dimensions(handle, &dimensions);

    // create cairo_surface_t and cairo_t for one picture
    if(index == 0)
    {
        surface[index] = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, SQUARE_LENGTH, SQUARE_LENGTH);
        factor_width = (double)SQUARE_LENGTH / (double)dimensions.width;
        factor_height = (double)SQUARE_LENGTH / (double)dimensions.height;
    }
    else
    {
        surface[index] = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, SQUARE_LENGTH / UNSELECT_RATIO, SQUARE_LENGTH / UNSELECT_RATIO);
        factor_width = (double)SQUARE_LENGTH / (double)dimensions.width / (double)UNSELECT_RATIO;
        factor_height = (double)SQUARE_LENGTH / (double)dimensions.height / (double)UNSELECT_RATIO;
    }
    cr[index] = cairo_create(surface[index]);

    cairo_save(cr[index]);
    factor_width = (factor_width > factor_height) ? factor_width : factor_height;
    cairo_scale(cr[index], factor_width, factor_width);

    float r = (float)BK_COLOR_R / 255.0;
    float g = (float)BK_COLOR_G / 255.0;
    float b = (float)BK_COLOR_B / 255.0;
    float alpha = BK_TRANSPARENT / 255.0;

    cairo_set_source_rgb (cr[index],  r * alpha, g * alpha, b * alpha);
    cairo_paint (cr[index]);
    rsvg_handle_render_cairo_style (handle, cr[index], &button_color_pair[index], 1);
    cairo_restore (cr[index]);

    g_object_unref (handle);
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

            square_number = __os_global_struct.page_number < MAX_SQUARE ? \
                        __os_global_struct.page_number: MAX_SQUARE;
            width = square_number * SQUARE_LENGTH + (square_number - 1) * SQUARE_INTERVAL;

            GetWindowRect(hWnd, &window_rect);
            startx = (RECTWP(&window_rect) - width) / 2;
            starty = (RECTHP(&window_rect) - SQUARE_LENGTH) / 2;

            // calculate rect array.
            for(i = 0; i < square_number; i++)
            {
                rect[i].left = startx + i * (SQUARE_LENGTH + SQUARE_INTERVAL);
                rect[i].top = starty;
                rect[i].right = rect[i].left + SQUARE_LENGTH;
                rect[i].bottom = rect[i].top + SQUARE_LENGTH;
            }

            readlink("/proc/self/exe", path, HISHELL_MAX_PATH);
            sprintf(path, "%s/res/circle.svg", dirname(path));

            surface[0] = cairo_image_surface_create (CAIRO_FORMAT_RGB24, (int)SQUARE_LENGTH, (int)SQUARE_LENGTH);
            cr[0] = cairo_create (surface[0]);
            button_color_pair[0].name = "color";
            button_color_pair[0].value = SELECT_COLOR;
            button_color_pair[0].important = 0;
            loadSVGFromFile(path, 0);

            surface[1] = cairo_image_surface_create (CAIRO_FORMAT_RGB24, (int)SQUARE_LENGTH, (int)SQUARE_LENGTH);
            cr[1] = cairo_create (surface[1]);
            button_color_pair[1].name = "color";
            button_color_pair[1].value = UNSELECT_COLOR;
            button_color_pair[1].important = 0;
            loadSVGFromFile(path, 1);
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
            width = square_number * SQUARE_LENGTH + (square_number - 1) * SQUARE_INTERVAL;

            GetWindowRect(hWnd, &window_rect);
            startx = (RECTWP(&window_rect) - width) / 2;
            starty = (RECTHP(&window_rect) - SQUARE_LENGTH) / 2;

            // calculate rect array.
            for(i = 0; i < square_number; i++)
            {
                rect[i].left = startx + i * (SQUARE_LENGTH + SQUARE_INTERVAL);
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

            // 是否是当前范围
            for(i = 0; i < square_number; i++)
            {
                if(PtInRect ((rect + i), x, y))
                    break;
            }

            if(i < square_number)   // 在当前范围内
            {
                pressed = TRUE;
                if(indicator.select == i)   // 是当前页
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
                else                        // 不是当前页
                {
                    CC_TRANSIT_TO_LAYER old_transit_to_lay = fallback_ops->transit_to_layer;

                    // 当前页缩小
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

                    // 切换当前页
                    layers[0] = page->layer;

                    indicator.select = i;
                    __os_global_struct.current_page = indicator.start + i;
                    page = find_page_by_id(__os_global_struct.current_page + 1);

                    // 动画，page切换，但是page不能恢复原来大小
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

                        // 要变化成 0.8 的倍率

                        layers[0] = mgTopmostLayer;
                        layers[1] = page->layer;

                        cpf.method = FCM_HORIZONTAL | FCM_SCALE;
                        cpf.percent = 100.0;
                        cpf.scale = 0.80;
                        fallback_ops->composite_layers(cc_context, layers, 2, &cpf);
                    }

                    // 改变点点，并切换标题
//                    InvalidateRect(hWnd, NULL, TRUE);
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

            // 如故考虑按下动画操作，此处要判断是否在动画状态
            if(!pressed)
                break;

            field.left = (rect + 0)->left - 10;
            field.top = (rect + 0)->top;
            field.right = (rect + square_number - 1)->right + 10;
            field.bottom = (rect + square_number - 1)->bottom;
            
            // 超出界限，按左键抬起处理
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

            // 判断在哪个点点里
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

                        // 要变化成 0.8 的倍率，没有下面的，就不对
                        layers[0] = mgTopmostLayer;
                        layers[1] = page->layer;

                        cpf.method = FCM_HORIZONTAL | FCM_SCALE;
                        cpf.percent = 100.0;
                        cpf.scale = 0.80;
                        fallback_ops->composite_layers(cc_context, layers, 2, &cpf);
                    }

//                    InvalidateRect(hWnd, NULL, TRUE);
                    if(__os_global_struct.hTitleBar)
                        SendMessage(__os_global_struct.hTitleBar, MSG_MAINWINDOW_CHANGE, 0, 0);
                }
            }
            else
            {
                // 最左边和最右边的都不用管
                if((x < rect[0].left) || (x > rect[square_number - 1].right))
                    break;

                // 判断在哪两个点之间
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

            // 如故考虑按下动画操作，此处要判断是否在动画状态
            if(!pressed)
                break;

            // 判断在哪个点点里
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
#if 0
                else
                {

                    indicator.select = i;
                    __os_global_struct.current_page = indicator.start + i;
                    page = find_page_by_id(__os_global_struct.current_page + 1);

                    if(page)
                    {
                    }
                    InvalidateRect(hWnd, NULL, TRUE);
                    if(__os_global_struct.hTitleBar)
                        SendMessage(__os_global_struct.hTitleBar, MSG_MAINWINDOW_CHANGE, 0, 0);
                }
#endif
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
            cairo_surface_destroy(surface[0]);
            cairo_destroy(cr[0]);
            cairo_surface_destroy(surface[1]);
            cairo_destroy(cr[1]);

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
    CreateInfo.iBkColor = RGBA2Pixel(HDC_SCREEN, 0xFF, 0xFF, 0xFF, 0xFF); 
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    __os_global_struct.hIndicatorBar = CreateMainWindowEx2 (&CreateInfo, 0L, NULL, NULL, ST_PIXEL_ARGB8888,
                                MakeRGBA (BK_COLOR_R, BK_COLOR_G, BK_COLOR_B, BK_TRANSPARENT),
                                CT_ALPHAPIXEL, 0xFF);
    if (__os_global_struct.hIndicatorBar == HWND_INVALID)
        return HWND_INVALID;

    return __os_global_struct.hIndicatorBar;
}
