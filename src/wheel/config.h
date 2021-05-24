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

#include <hisvg.h>
#include <glib.h>

// for strength length
#define MAX_NAME_LENGTH     64
#define HISHELL_MAX_PATH    256

// for font
#define CAPTION_FONT        25 
#define NAME_FONT           30
// for title
//#define CAPTION_LEFT        20
//#define CAPTION_TOP         60
//#define CAPTION_RIGHT       150
//#define CAPTION_BOTTOM      90

// for icon 
#define ICON_WIDTH          48
#define ICON_HEIGHT         48

// timer id
#define TIMER_LONG_PRESS    100
#define TIMER_QUIT_FULL     110
#define LONG_PRESS_PERIOD   100         // 1s
#define QUIT_FULL_PERIOD    300

// for drag
#define PRESS_TIMEOUT           10
#define CLICK_TIMEOUT           8
#define CLICK_MICRO_MOVEMENT    8

// for list
#define LIST_LINE           5           // must be odd
#define LIST_DELT           30
#define LIST_UNSELECT_R     1.0
#define LIST_UNSELECT_G     1.0
#define LIST_UNSELECT_B     1.0
#define LIST_FONT_SIZE      30
#define LIST_MARGIN         30
#define LIST_NAME_HEIGHT    20
#define LIST_FACTOR         0.5     // 第一行长度和中间行长度的比值
#define LIST_LIMIT_SPEED    200.0   // 速度超过此，滑动后要进行动画
#define LIST_ANIMATION_TIME 300

// for hibus
#define SOCKET_PATH                 "/var/tmp/hibus.sock"
#define HIBUS_HISHELL_NAME          "cn.fmsoft.hybridos.hishell"

// for transparent
#define BK_TRANSPARENT      0xA0

typedef struct tag_Global_Param
{
    // for file
    char config_file[MAX_NAME_LENGTH];
    char css_file[MAX_NAME_LENGTH];
    char svg_file[MAX_NAME_LENGTH];

    // caption
    unsigned char caption[MAX_NAME_LENGTH];

    // for draw icon
    char color_style[64];
    HiSVGHandle *svg_handle;

    // for draw select
    unsigned int text_color;

    // for font size
    int          caption_font_size;
    int          name_font_size;

    // for list
    int          list_number;       // 列表条目数
    unsigned char * member;         // 所有条目的数组
    int          select;            // 当前选择序号
    int          move_select;       // 鼠标移动时，选择序号

    RECT         caption_rect;      // caption区域
    RECT         icon_rect;         // 图标区域
    RECT         name_rect;         // 名字区域
    RECT         press_rect;        // 长按区域
    RECT         full_rect;         // 弹出区域
    RECT         list_area;         // list区域
    RECT         list_rect[LIST_LINE];  // list中的每个条目的区域，用来算坐标

    BOOL         pressed_long;      // 是否是长按
    BOOL         full_screen;       // 是否是全屏

    float        internal;          // list内，条目之间的间距
    int          mouse_delt;        // 鼠标y方向的移动量
    
    int *        rect_y;            // list中，每个条目的y坐标

    float        speed;             // 动画速度

    // for hibus
    hibus_conn * hibus_context;
    HWND         main_hwnd;
} Global_Param;



#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _CONFIG_h */
