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
** main.c: It illustrates how to program with libhirsvg.so and libhicairo.so.
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
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <libgen.h>
#include <fcntl.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <mgeff/mgeff.h>
#include <hibus.h>

#include "../include/sysconfig.h"
#include "../include/svgpaint.h"


// for svg file
HDC create_memdc_from_image_surface (cairo_surface_t* image_surface)
{
    MYBITMAP my_bmp = {
        flags: MYBMP_TYPE_RGB | MYBMP_FLOW_DOWN | MYBMP_ALPHA,
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

void loadSVGFromFile(const char* file, cairo_t ** cr, cairo_surface_t ** surface, \
                                                        char * color_style, RECT rect)
{
    HiSVGHandle *handle;
    HiSVGRect vbox;
    HiSVGDimension dimensions;
    GError *error = NULL;
    double factor_width = 0.0f;
    double factor_height = 0.0f;
    int width = RECTW(rect);
    int height = width;

    // read file from svg file
    handle = hisvg_handle_new_from_file(file, &error);
    if(error)
    {
        * surface = NULL;
        * cr = NULL;
        return;
    }
    hisvg_handle_get_dimensions(handle, &dimensions);

    // create cairo_surface_t and cairo_t for one picture
    * surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    vbox.x = 0;
    vbox.y = 0;
    vbox.width = width;
    vbox.height = height;

    factor_width = (double)width / (double)dimensions.w.length;
    factor_height = (double)height / (double)dimensions.h.length;
    *cr = cairo_create(*surface);

    cairo_save(*cr);
    factor_width = (factor_width > factor_height) ? factor_width : factor_height;
    cairo_scale(*cr, factor_width, factor_width);

    cairo_set_source_rgba(*cr,  0.0, 0.0, 0.0, 0.0);
    cairo_paint(*cr);
    hisvg_handle_set_stylesheet(handle, NULL, color_style, strlen(color_style), NULL);
    hisvg_handle_render_cairo(handle, *cr, &vbox, NULL, NULL);
    cairo_restore(*cr);

    hisvg_handle_destroy(handle);
}


void paint_svg(HWND hwnd, HDC hdc, RECT rect, cairo_surface_t * surface)
{
    int width = RECTW(rect);
    int height = width;

    HDC csdc = create_memdc_from_image_surface(surface);
    if (csdc != HDC_SCREEN && csdc != HDC_INVALID)
    {
        SetMemDCAlpha (csdc, MEMDC_FLAG_SRCPIXELALPHA, 0);
        BitBlt(csdc, 0, 0, width, height, hdc, rect.left, rect.top, 0);
    }
    DeleteMemDC(csdc);
    return;
}
