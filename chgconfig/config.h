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
 * \file config.h
 * \author Gengyue <gengyue@minigui.org>
 * \date 2020/09/16
 *
 * \brief This file includes some interfaces used by system manager process.
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
 * $Id: config.h 13674 2020-09-16 06:45:01Z Gengyue $
 *
 *      HybridOS for Linux, VxWorks, NuCleus, OSE.
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
#include <libhirsvg/rsvg.h>

// for strength length
#define MAX_NAME_LENGTH     64
#define HISHELL_MAX_PATH    256

#define CAPTION_FONT        25 

// for hibus
#define SOCKET_PATH                 "/var/tmp/hibus.sock"
#define HIBUS_HISHELL_NAME          "cn.fmsoft.hybridos.hishell"
#define HIBUS_HISHELL_MGINIT_NAME   "mginit"

typedef struct tag_Global_Param
{
    // caption
    unsigned char caption[MAX_NAME_LENGTH];

    // rect for element
    RECT         caption_rect;
    RECT         icon_rect[2];

    // for draw icon
    char button_color[2][32];
    RsvgStylePair color_pair [2];
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
