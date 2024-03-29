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
** wallpaper.c: It illustrates how to make a dynamic wallpaper.
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
#include <string.h>
#include <math.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <cairo/cairo.h>

#include <cairo/cairo-minigui.h>

#ifdef CAIRO_HAS_DRM_SURFACE
#include <cairo/cairo-drm.h>
#endif

#include "../include/sysconfig.h"

#define ID_TIMER    200
#define TIMER_INTERVAL  100
#define MAX_NUMBER  10 
#define MAX_STEP    100
#define RGBA_R      100
#define RGBA_G      0
#define RGBA_B      100
#define RGBA_A      255

#ifdef PLATFORM_ANYKA_UCLINUX
    #define M_PI        3.1415926
#endif

typedef struct tagNodeInfo
{
    cairo_t *cr;
    cairo_surface_t *surface;
    int x;
    int y;
    int step;
    int direction;
    RGB rgb;
} NodeInfo;

cairo_t* global_cr = NULL;
NodeInfo node[MAX_NUMBER];
static int fixed_radius = 0;

static cairo_surface_t *create_cairo_surface (HDC hdc, int width, int height)
{
    static cairo_device_t* cd = NULL;

#ifdef CAIRO_HAS_DRM_SURFACE
    if (cd == NULL) {
        cd = cairo_drm_device_default ();
        if (cairo_device_status (cd)) 
        {
            cairo_device_destroy (cd);
            cd = NULL;
        }
    }
#endif

    if (hdc == HDC_INVALID) 
        return cairo_minigui_surface_create_with_memdc (cd, CAIRO_FORMAT_RGB24, width, height);
    else if (hdc != HDC_SCREEN && width > 0 && height > 0) 
        return cairo_minigui_surface_create_with_memdc_similar (cd, hdc, width, height);

    return cairo_minigui_surface_create (cd, hdc);
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

#ifdef CAIRO_HAS_DRM_SURFACE
static HDC create_memdc_from_drm_surface(cairo_surface_t* drm_surface,
        cairo_surface_t** image_surface)
{
    cairo_status_t status;
    MYBITMAP my_bmp = {
        flags: MYBMP_TYPE_RGB | MYBMP_FLOW_DOWN,
        frames: 1,
        depth: 32,
    };

    *image_surface = cairo_drm_surface_map_to_image(drm_surface);
    status = cairo_surface_status (*image_surface);

    my_bmp.w = cairo_image_surface_get_width (*image_surface);
    my_bmp.h = cairo_image_surface_get_height (*image_surface);
    my_bmp.pitch = cairo_image_surface_get_stride (*image_surface);
    my_bmp.bits = cairo_image_surface_get_data (*image_surface);
    my_bmp.size = my_bmp.pitch * my_bmp.h;

    return CreateMemDCFromMyBitmap(&my_bmp, NULL);
}
#endif

static HDC destroy_memdc_for_image_surface(HDC hdc,
        cairo_surface_t* image_surface)
{
    DeleteMemDC (hdc);
}

static void get_coordinate(int index)
{
    int i = 0;
    int x = 0;
    int y = 0;
    int quit = 1;
    int times = 0;

    while(quit)
    {
        times ++;
        node[index].x = rand() % g_rcScr.right;
        node[index].y = rand() % g_rcScr.bottom;
 
        for(i = 0; i < MAX_NUMBER; i++)
        {
            if(i == index)
                continue;
            x = node[index].x - node[i].x;
            y = node[index].y - node[i].y;

            if((abs(x) < fixed_radius / 2) || (abs(y) < fixed_radius / 2))
                break;
        }
        
        if(i == MAX_NUMBER)
            quit = 0;

        if(times > 10)
            break;
    }
}

static void paint(int width, int height)
{
    HDC csdc = HDC_INVALID;
    int i = 0;
    HDC hdc = HDC_SCREEN;

    if (global_cr == NULL) 
        return;

    // draw 
    cairo_save(global_cr);

    // clear surface
    cairo_set_source_rgb(global_cr, 0, 0,  0);
    cairo_rectangle(global_cr, 0, 0, g_rcScr.right, g_rcScr.bottom);
    cairo_fill(global_cr);

    srand(time(NULL));

    i = g_rcScr.right > g_rcScr.bottom ? g_rcScr.bottom : g_rcScr.right;
    fixed_radius = i / 6;
    
    for(i = 0; i < MAX_NUMBER; i++)
    {
        if(node[i].direction)
        {
            if(node[i].step == MAX_STEP)
            {
                node[i].direction = 0;
                node[i].step = MAX_STEP - 1;
            }
            else
                node[i].step ++;
        }
        else
        {
            if(node[i].step == 1)
            {
                get_coordinate(i);
                node[i].step = 1;
                node[i].direction = 1;
                node[i].rgb.r = RGBA_R;
                node[i].rgb.g = RGBA_G;
                node[i].rgb.b = RGBA_B;
                node[i].rgb.a = RGBA_A;
            }
            else
                node[i].step --;
        }

        node[i].surface = cairo_surface_create_similar(cairo_get_target(global_cr), CAIRO_CONTENT_COLOR, g_rcScr.right, g_rcScr.bottom);
        node[i].cr = cairo_create(node[i].surface);

        cairo_set_source_rgb(node[i].cr, node[i].rgb.r / 255.0, node[i].rgb.g / 255.0,  node[i].rgb.b / 255.0);
        cairo_arc(node[i].cr, node[i].x, node[i].y, fixed_radius * node[i].step / MAX_STEP, 0, 2 * M_PI);
        cairo_fill(node[i].cr);

        cairo_set_operator(global_cr, CAIRO_OPERATOR_ADD);
        cairo_set_source_surface(global_cr, node[i].surface, 0, 0);
        cairo_paint(global_cr);

        cairo_surface_destroy(node[i].surface);
        cairo_destroy(node[i].cr);
    }
    cairo_restore(global_cr);

    // copy to minigui
    {
        cairo_surface_t* image_surface = NULL;
        cairo_surface_t* target_surface = cairo_get_target(global_cr);
        //cairo_surface_flush (target_surface);
        cairo_surface_type_t cst = cairo_surface_get_type (target_surface);
        if (cst == CAIRO_SURFACE_TYPE_MINIGUI) 
            csdc = cairo_minigui_surface_get_dc (target_surface);
        else if (cst == CAIRO_SURFACE_TYPE_IMAGE) 
            csdc = create_memdc_from_image_surface (target_surface);
#ifdef CAIRO_HAS_DRM_SURFACE
        else if (cst == CAIRO_SURFACE_TYPE_DRM) 
        {
            csdc = (HDC)cairo_drm_surface_get_minigui_dc (target_surface);
            if (csdc == HDC_INVALID) 
                csdc = create_memdc_from_drm_surface (target_surface, &image_surface);
        }
#endif

        if (csdc == HDC_INVALID) 
            return;

        if (csdc != HDC_SCREEN && csdc != HDC_INVALID) 
            BitBlt(csdc, 0, 0, width, height, hdc, 0, 0, 0);

        if (image_surface) 
        {
#ifdef CAIRO_HAS_DRM_SURFACE
            destroy_memdc_for_drm_surface (csdc, target_surface, image_surface);
#endif
        }
        else if (cst == CAIRO_SURFACE_TYPE_IMAGE) {
            destroy_memdc_for_image_surface (csdc, target_surface);
        }
    }
    SyncUpdateDC(HDC_SCREEN);

    return;
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG     Msg;
    int i = 0;
    cairo_surface_t* surface = NULL;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER, "wallpaper", 0, 0);
#endif

    surface = create_cairo_surface (HDC_INVALID, g_rcScr.right, g_rcScr.bottom);
    if (surface == NULL) 
        goto FAIL;

    global_cr = cairo_create (surface);
    if (global_cr == NULL) 
        goto FAIL;

    memset(node, 0, MAX_NUMBER * sizeof(NodeInfo));

    srand(time(0));
    for(i = 0; i < MAX_NUMBER; i++)
    {
        get_coordinate(i);
        node[i].step = rand() % MAX_STEP;
        node[i].direction = node[i].step & 0x01;
        node[i].rgb.r = RGBA_R;
        node[i].rgb.g = RGBA_G;
        node[i].rgb.b = RGBA_B;
        node[i].rgb.a = RGBA_A;
    }

    SetTimer(HWND_DESKTOP, ID_TIMER, TIMER_INTERVAL);

    while (GetMessage(&Msg, HWND_DESKTOP)) 
    {
        if (Msg.message == MSG_TIMER && Msg.wParam == ID_TIMER)
        {
            paint(g_rcScr.right, g_rcScr.bottom);
        }

        DispatchMessage(&Msg);
    }

FAIL:
    if (global_cr) 
        cairo_destroy(global_cr);

    if (surface) 
    {
        cairo_surface_finish(surface);
        cairo_surface_destroy(surface);
    }

    return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

