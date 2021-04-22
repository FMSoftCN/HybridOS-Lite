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

#include <hidomlayout.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define NO_ERR                      0
#define ERR_MANIFEST_EXIST          -1
#define ERR_MANIFEST_LENGTH         -2
#define ERR_MANIFEST_MALLOC         -3
#define ERR_MANIFEST_OPEN           -4
#define ERR_MANIFEST_READ           -5
#define ERR_MANIFEST_PARSE          -6
#define ERR_LAYOUT                  -7

// global setting for hishell
#define DEFAULT_CSS_PATH            "default_layout.css"
#define STRING_OS_NAME              "Hybrid OS Lite V1.1.0"

// background color for tile bar, indicator bar and description bar
#define BK_COLOR_R      140
#define BK_COLOR_G      140
#define BK_COLOR_B      140
#define BK_TRANSPARENT  0xF0

// for catch mouse event
#define PRESS_TIMEOUT               (5)
#define CLICK_TIMEOUT               (8)
#define CLICK_MICRO_MOVEMENT        (8)
#define LAYER_CIRCLE                (FALSE)
#define LIMIT_SPEED                 (300.0)

// for timer
#define ID_TIMER_BAR                (100)           // for time display
#define ID_TIMER_MOUSE              (210)           // for compositor

// for buffer length
#define APP_NAME_LENGTH             (128)
#define HISHELL_MAX_PATH            (256)
#define STYLE_LENGTH                (512)

// for hibus
#define SOCKET_PATH                 "/var/tmp/hibus.sock"
#define HIBUS_HISHELL_NAME          "cn.fmsoft.hybridos.hishell"
#define HIBUS_HISHELL_MGINIT_NAME   "mginit"

typedef enum
{
    SCREEN_ANIMATION_HORIZENTAL,
    SCREEN_ANIMATION_VERTICAL,
} screenDirection;

typedef enum
{
    RT_EXEC,
    RT_HIWEB,
    RT_HIJS,
} runnerType;

typedef enum
{
    RS_STOP,
    RS_RUN,
    RS_QUIT,
    RS_RESTART,
} runnerStatus;

#define RA_WINDOW               (0x01 << 0)
#define RA_ACTIVITY             (0x01 << 1)
#define RA_INTIALIZER           (0x01 << 2)
#define RA_DAEMON               (0x01 << 3)
#define RA_PROGRAM              (0x01 << 4)
#define RA_DEFAULT              (0x01 << 5)

typedef struct OS_Global_structure_tag
{
    char * css_file;            // css file name
    char * tile_style;          // css for title box
    char * page_style;          // css for page box
    char * indicator_style;     // css for indicator box
    char * description_style;   // css for description box
    screenDirection direction;  // direction of screen, which is for slide

    int clientId_topmost;       // Topmost client id
    int clientId_wallpaper;     // clientID of dynamic background process
    pid_t pid_wallpaper;        // pid of dynamic background process

    HWND hTitleBar;             // for title bar
    RECT rect_title;

    HWND hIndicatorBar;         // for indicator bar
    RECT rect_indicator; 

    RECT rect_page;             // for page area

    HWND hDescriptionBar;       // for description bar
    RECT rect_description;

    struct page_structure_tag * page;   // page list
    int page_number;            // page number
    int current_page;           // index of current page

    hibus_conn * hibus_context;
} OS_Global_struct;

typedef struct page_structure_tag
{
    char * id_string;           // page id
    char * styles;              // css for page
    char * title_en;            // title in english
    char * title_zh;            // title in chinese
    char * description_en;      // description in english
    char * description_zh;      // description in chinese
    struct runner_structure_tag * runner;   // runner list
    struct page_structure_tag * next;       // next page

    int id;                     // page id
    int runner_number;          // runner number in this page
    MG_Layer * layer;           // layer for this page
} page_struct;

typedef struct runner_structure_tag
{
    char * name;                // runner name
    char * entry;               // executive file
    char * css_class;           // css class of this runner
    char * styles;              // css for this runner
    char * intent;              // start parameter
    unsigned int runas;         // how to start
    runnerType type;            // executive type
    struct dep_structure_tag * dep;     // dependency list
    struct runner_structure_tag * next; // next runner

    HWND hwnd;                  // window handle
    int lx;                     // left
    int ty;                     // top
    int rx;                     // right
    int by;                     // bottom
    HLDomElementNode * node;    // for layout
    uint32_t bk_color;          // background color
    char * font_name;           // font name
    uint32_t text_color;        // font color

    pid_t pid;                  // pid for runner thread
    runnerStatus status;        // run status

    int hibus_id;               // id for hibus
} runner_struct;


typedef struct dep_structure_tag
{
    char * dep_name;
    struct dep_structure_tag * next;
} dep_struct;


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _CONFIG_h */
