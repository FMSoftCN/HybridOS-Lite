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


static cairo_surface_t *create_direct_image_surface (HDC hdc, const RECT* rc)
{
    cairo_surface_t *surface;
    Uint8* bits;
    int width, height, pitch;

    bits = LockDC (hdc, rc, &width, &height, &pitch);

    if (bits == NULL || width <= 0 || height <= 0 || pitch <= 0) {
        _ERR_PRINTF("hicairo: failed LockDC()\n");
        goto FAIL;
    }

    // format here must be CAIRO_FORMAT_RGB16_565.
    // See call of CreateMainWindowEx2.
    surface = cairo_image_surface_create_for_data (bits,
            CAIRO_FORMAT_RGB16_565, width, height, pitch);
    if (surface == NULL) {
        _ERR_PRINTF("hicairo: failed when creating direct image surface\n");
        goto FAIL;
    }

    return surface;

FAIL:
    return NULL;
}

static cairo_surface_t *finish_direct_image_surface (cairo_surface_t *surface, HDC hdc)
{
    _DBG_PRINTF("calling finish_direct_image_surface\n");

    cairo_surface_finish(surface);
    cairo_surface_destroy(surface);

    UnlockDC (hdc);
    return NULL;
}

BOOL loadSVGFromFile(const char* file, HiSVGHandle ** svg_handle)
{
    GError *error = NULL;
    struct stat buf;
    int fd = 0;
    int length = 0;
    BOOL ret = FALSE;

    * svg_handle = NULL;

    if((access(file, F_OK | R_OK)) != 0)
        goto FAIL;

    if(stat(file, &buf) < 0)
        goto FAIL;

    fd = open(file, O_RDONLY);
    if(fd == -1)
        goto FAIL;

    if(buf.st_size <= 4096)
    {
        unsigned char buffer[4096];
        length = read(fd, buffer, buf.st_size);
        close(fd);

        if(length == 0)
            goto FAIL;

        *svg_handle = hisvg_handle_new_from_data(buffer, buf.st_size, &error);
        if(error)
        {
            *svg_handle = NULL;
            goto FAIL;
        }
        ret = TRUE;
    }
    else
    {
        char *buffer = malloc(buf.st_size);
        if(buffer)
        {
            length = read(fd, buffer, buf.st_size);
            close(fd);

            if(length == 0)
                goto FAIL;

            *svg_handle = hisvg_handle_new_from_data(buffer, buf.st_size, &error);
            free(buffer);
            if(error)
            {
                *svg_handle = NULL;
                goto FAIL;
            }
            ret = TRUE;
        }
    }

FAIL:
    return ret;
}

void paint_svg(HWND hwnd, HDC hdc, RECT rect, HiSVGHandle * svg_handle, char * color_style)
{
    cairo_t *cr = NULL;
    cairo_surface_t * surface = NULL;
    HiSVGRect vbox;
    HiSVGDimension dimensions;
    GError *error = NULL;

    double factor_width = 0.0f;
    double factor_height = 0.0f;
    int width = RECTW(rect);
    //int height = RECTH(rect);
    int height = width;
    RECT draw_rect;

    if(svg_handle == NULL)
        return;

    draw_rect.left = rect.left;
    draw_rect.top = rect.top;
    draw_rect.right = draw_rect.left + width;
    draw_rect.bottom = draw_rect.top + height;

    hisvg_handle_get_dimensions(svg_handle, &dimensions);

    // create cairo_surface_t and cairo_t for one picture
    surface = create_direct_image_surface(hdc, &draw_rect);
    vbox.x = 0;
    vbox.y = 0;
    vbox.width = width;
    vbox.height = height;

    factor_width = (double)width / (double)dimensions.w.length;
    factor_height = (double)height / (double)dimensions.h.length;
    cr = cairo_create(surface);

    cairo_save(cr);
    factor_width = (factor_width > factor_height) ? factor_width : factor_height;
    cairo_scale(cr, factor_width, factor_width);

    cairo_set_source_rgba(cr,  0.0, 0.0, 0.0, 0.0);
    cairo_paint(cr);
    hisvg_handle_set_stylesheet(svg_handle, NULL, color_style, strlen(color_style), NULL);
    hisvg_handle_render_cairo(svg_handle, cr, &vbox, NULL, NULL);

    cairo_restore(cr);

    if(cr)
    {
        cairo_destroy(cr);
        finish_direct_image_surface(surface, hdc);
    }

    return;
}

