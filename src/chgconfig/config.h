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
** config.h: It defines some strutures and micro for program.
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

#ifndef _CONFIG_H
#define _CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include <cairo/cairo.h>
#include <cairo/cairo-minigui.h>

#ifdef CAIRO_HAS_DRM_SURFACE
#include <cairo/cairo-drm.h>
#endif

#include <glib.h>
#include <hisvg.h>

// for strength length
#define MAX_NAME_LENGTH     128 
#define HISHELL_MAX_PATH    256

#define CAPTION_FONT        25 


typedef struct tag_Global_Param
{
    char config_file[MAX_NAME_LENGTH];
    char css_file[MAX_NAME_LENGTH];

    // caption
    unsigned char caption[MAX_NAME_LENGTH];

    // rect for element
    RECT         caption_rect;
    RECT         icon_rect[2];

    // for draw icon
    char button_color[2][32];
    char color_style[2][64];

    cairo_t * icon_cr[2];
    cairo_surface_t * icon_surface[2];

    // for hibus
    hibus_conn * hibus_context;
    HWND         main_hwnd;

    // font size
    int          font_size;
} Global_Param;



#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _CONFIG_h */
