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
#define BK_COLOR_R      0
#define BK_COLOR_G      0
#define BK_COLOR_B      0
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

typedef void (* CC_TRANSIT_TO_LAYER) (CompositorCtxt* ctxt, MG_Layer* to_layer);

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
