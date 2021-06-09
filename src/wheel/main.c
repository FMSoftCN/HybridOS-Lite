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
** $Id: helloworld.c 767 2009-12-08 06:42:19Z houhuihua $
**
** Listing 1.1
**
** helloworld.c: Sample program for MiniGUI Programming Guide
**      The first MiniGUI application.
**
** Copyright (C) 2003 ~ 2017 FMSoft (http://www.fmsoft.cn).
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
#include <libgen.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef PLATFORM_ANYKA_UCLINUX
    #include <bits/getopt.h>
#endif

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <mgeff/mgeff.h>
#include <hibus.h>
#include <hidomlayout.h>

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
#include "speed-meter.h"
#include "hibus.h"


Global_Param global_param;
static MoseSpeed mouseinfo;                 // for drag screen
static HiSVGHandle * image_handle = NULL;

// get css file content
static char * get_file_content(char * path, int *length)
{
    int fd = -1;
    struct stat buf;
    char * buffer = NULL;

    * length = 0;

    if((access(path, F_OK | R_OK)) != 0)
        fprintf(stderr, "The  %s does not exist!\n", path);
    else
    {
        if(stat(path, &buf) < 0)
            fprintf(stderr, "Can not get the length of %s !\n", path);
        else
        {
            if(buf.st_size == 0)
                fprintf(stderr, "The length of %s is 0!\n", path);
            else
            {
                * length = buf.st_size;
                buffer = malloc(* length + 1);
                if(buffer == NULL)
                    fprintf(stderr, "Can not create buffer for manifest file %s!\n", path);
                else
                {
                    fd = open(path, O_RDONLY);
                    if(fd == -1)
                    {
                        fprintf(stderr, "Can not open manifest file %s!\n", path);
                        free(buffer);
                    }
                    else
                    {
                        memset(buffer, 0, * length + 1);
                        * length = read(fd, buffer, buf.st_size);
                        if(* length == 0)
                        {
                            fprintf(stderr, "Can not read manifest file %s!\n", path);
                            free(buffer);
                        }
                        close(fd);
                    }
                }
            }
        }
    }
    return buffer;
}

static BOOL parse_config(int width, int height)
{
    char file_path[256] = {0};
    int i = 0;
    char number_index[32] = {0};
    char icon_color[32] = {0};
    int default_length = 0;
    char * css_content = NULL;

    // get user date from file
    if(global_param.config_file[0])
    {
        readlink("/proc/self/exe", file_path, HISHELL_MAX_PATH);
        sprintf(file_path, "%s/config/%s", dirname(file_path), global_param.config_file);

        if((access(file_path, F_OK | R_OK)) != 0)
            return FALSE;

        // get the caption
        if(global_param.caption[0] == 0)
            GetValueFromEtcFile(file_path, "AppConfigData", "caption", \
                                        global_param.caption, MAX_NAME_LENGTH);

        // get icon color
        if(GetValueFromEtcFile(file_path, "AppConfigData", "icon_color", \
                                    icon_color, 32) == ETC_OK)
        {
            strcpy(global_param.color_style, "svg { color:");
            strcat(global_param.color_style, icon_color);
            strcat(global_param.color_style, "; } ");
        }

        // get list item number
        GetIntValueFromEtcFile(file_path, "AppConfigData", "ListNumber", \
                                            &global_param.list_number);
        // get the list items
        if(global_param.list_number)
        {
            global_param.member = malloc(global_param.list_number * MAX_NAME_LENGTH);
            memset(global_param.member, 0, global_param.list_number * MAX_NAME_LENGTH);
            for(i = 0; i < global_param.list_number; i++)
            {
                sprintf(number_index, "item%d", i + 1);
                GetValueFromEtcFile(file_path, "ListContent", number_index, \
                        (char *)global_param.member + i * MAX_NAME_LENGTH, \
                        MAX_NAME_LENGTH);
            }
        }

        // get text color
        GetIntValueFromEtcFile(file_path, "AppConfigData", "TextColor", \
                                                &global_param.text_color);
    }


    // layout now
    HLCSS* css = NULL;
    HLMedia hl_media;
    const HLUsedBoxValues* box = NULL;
    const HLUsedBackgroundValues* bg = NULL;
    const HLUsedTextValues* text = NULL;

    // for normal screen
    hl_media.width = width;
    hl_media.height = height;
    hl_media.dpi = GetGDCapability(HDC_SCREEN, GDCAP_DPI);
    hl_media.density = hl_media.dpi;

    css = hilayout_css_create();
    if (css == NULL)
        return HILAYOUT_INVALID;

    // get css file
    if(global_param.css_file[0])
    {
        memset(file_path, 0, HISHELL_MAX_PATH);
        readlink("/proc/self/exe", file_path, HISHELL_MAX_PATH);
        sprintf(file_path, "%s/layout/%s", dirname(file_path), global_param.css_file);

        css_content = get_file_content(file_path, &default_length);
        if(css_content)
            hilayout_css_append_data(css, css_content, default_length);
    }

    HLDomElementNode* root = hilayout_element_node_create("div");

    HLDomElementNode* caption = hilayout_element_node_create("div");
    hilayout_element_node_set_id(caption, "caption");

    HLDomElementNode* icon = hilayout_element_node_create("div");
    hilayout_element_node_set_id(icon, "icon");

    HLDomElementNode* name = hilayout_element_node_create("div");
    hilayout_element_node_set_id(name, "name");

    hilayout_element_node_append_as_last_child(caption, root);
    hilayout_element_node_append_as_last_child(icon, root);
    hilayout_element_node_append_as_last_child(name, root);

    hilayout_do_layout(&hl_media, css, root);

    // caption
    box = hilayout_element_node_get_used_box_value(caption);
    global_param.caption_rect.left = (int)box->x;
    global_param.caption_rect.top = (int)box->y;
    global_param.caption_rect.right = (int)(box->x + box->w);
    global_param.caption_rect.bottom = (int)(box->y +box->h);

    text = hilayout_element_node_get_used_text_value(caption);
    global_param.caption_font_size = text->font_size;

    // icon 
    box = hilayout_element_node_get_used_box_value(icon);
    global_param.icon_rect.left = (int)box->x;
    global_param.icon_rect.top = (int)box->y;
    global_param.icon_rect.right = (int)(box->x + box->w);
    global_param.icon_rect.bottom = (int)(box->y +box->h);

    // name 
    box = hilayout_element_node_get_used_box_value(name);
    global_param.name_rect.left = (int)box->x;
    global_param.name_rect.top = (int)box->y;
    global_param.name_rect.right = (int)(box->x + box->w);
    global_param.name_rect.bottom = (int)(box->y +box->h);

    text = hilayout_element_node_get_used_text_value(name);
    global_param.name_font_size = text->font_size;

    // for press_rect
    GetBoundRect(&global_param.press_rect, &global_param.icon_rect, \
                    &global_param.name_rect);

    hilayout_css_destroy(css);

    if(root)
        hilayout_element_node_destroy(root);
    if(caption)
        hilayout_element_node_destroy(caption);
    if(icon)
        hilayout_element_node_destroy(icon);
    if(name)
        hilayout_element_node_destroy(name);

    // for full screen
    css = hilayout_css_create();
    if (css == NULL)
        return HILAYOUT_INVALID;

    // get css file
    if(global_param.css_file[0])
    {
        if(css_content)
            hilayout_css_append_data(css, css_content, default_length);
    }

    root = hilayout_element_node_create("div");

    HLDomElementNode* full = hilayout_element_node_create("div");
    hilayout_element_node_set_id(full, "list");

    hilayout_element_node_append_as_last_child(full, root);

    hilayout_do_layout(&hl_media, css, root);

    // full rect for mouse move
    box = hilayout_element_node_get_used_box_value(full);
    global_param.full_rect.left = (int)box->x;
    global_param.full_rect.top = (int)box->y;
    global_param.full_rect.right = (int)(box->x + box->w);
    global_param.full_rect.bottom = (int)(box->y + box->h);

    hilayout_css_destroy(css);
    if(root)
        hilayout_element_node_destroy(root);
    if(full)
        hilayout_element_node_destroy(full);

    if(css_content)
        free(css_content);

    return TRUE;
}

// for cairo
static cairo_surface_t *create_cairo_surface (HDC hdc, int width, int height)
{
    static cairo_device_t* cd = NULL;

    if (hdc == HDC_INVALID) 
        return cairo_minigui_surface_create_with_memdc (cd, CAIRO_FORMAT_RGB24, \
                                                                width, height);
    else if (hdc != HDC_SCREEN && width > 0 && height > 0) 
        return cairo_minigui_surface_create_with_memdc_similar (cd, hdc, width, \
                                                                        height);

    return cairo_minigui_surface_create (cd, hdc);
}

static void draw_rect(cairo_t * cr)
{
    char * name = NULL;
    cairo_text_extents_t te; 
    int start_x = 0;
    float factor = 0.0;
    int middle = LIST_LINE / 2;
    // 线性系数变化斜率
    float slope = (LIST_FACTOR - 1) / (global_param.list_rect[middle].bottom -\
                                            global_param.list_rect[0].bottom);
    int * rect = global_param.rect_y;
    int i = 0;
    int select = 0;
    int start_y = 0;
    float r = (float)GetRValue(global_param.text_color) / 255.0;
    float g = (float)GetGValue(global_param.text_color) / 255.0;
    float b = (float)GetBValue(global_param.text_color) / 255.0;
    float calculate = 0.0;

    if(rect == NULL)
        return;

    // 此时的select是为了算坐标用的 
    select = global_param.select - global_param.mouse_delt / (int)(LIST_NAME_HEIGHT + \
                                                                global_param.internal);
    if(select < 0)
        select = 0;
    if(select >= global_param.list_number)
        select = global_param.list_number - 1;

    start_y = global_param.select * \
                (LIST_NAME_HEIGHT + global_param.internal) + LIST_NAME_HEIGHT;
    start_y -= global_param.mouse_delt;
    start_y -= global_param.list_rect[middle].bottom;

    // 这里算哪个是select，谁靠近中线谁是
    calculate = (float)global_param.mouse_delt / \
            (float)((float)LIST_NAME_HEIGHT + global_param.internal);
    calculate = calculate - (int)calculate;
    if(calculate < 0.4)
        select = select;
    else
        select --;

    if(select < 0)
        select = 0;

    global_param.move_select = select;

    if(select == 0)
    {
        if(start_y < (-1 * global_param.list_rect[middle].bottom - \
                            LIST_NAME_HEIGHT * 1 / 8))
            start_y = -1 * global_param.list_rect[middle].bottom - \
                            LIST_NAME_HEIGHT * 1 / 8;
    }

    if(select == (global_param.list_number - 1))
    {
        if(start_y > (rect[select] - global_param.list_rect[middle].bottom + \
                                        LIST_NAME_HEIGHT))
            start_y = rect[select] - global_param.list_rect[middle].bottom + \
                                        LIST_NAME_HEIGHT;
    }

    for(i = 0; i < global_param.list_number; i++)
    {
        if(rect[i] > start_y)
        {
            if(i == select)
                cairo_set_source_rgb(cr, r, g, b);
            else
                cairo_set_source_rgb(cr, LIST_UNSELECT_R, LIST_UNSELECT_G, LIST_UNSELECT_B);

            name = global_param.member + i * MAX_NAME_LENGTH;
            cairo_text_extents (cr, name, &te);
            start_x = te.x_bearing + te.width;

            if(rect[i] <= (start_y + global_param.list_rect[middle].bottom))
                factor = 1.0 + slope * (start_y + global_param.list_rect[middle].bottom - rect[i]);
            else
                factor = 1.0 + slope * (rect[i] - (start_y + global_param.list_rect[middle].bottom));

            cairo_move_to(cr, (float)(RECTWP(&global_param.list_area) - \
                                    (float)start_x * factor) / 2, rect[i] - start_y);


            cairo_scale(cr, factor, factor);
            cairo_show_text(cr, name);
            factor = 1.0 / factor;
            cairo_scale(cr, factor, factor);
        }
    }

    return;
}

static void paint_list(cairo_t * cr, HDC hdc, int width, int height)
{
    HDC csdc = HDC_INVALID;
    cairo_surface_t * list_surface = NULL;
    cairo_t* list_cr = NULL;

    if(cr == NULL) 
        return;

    // draw
    cairo_save(cr);

    // paint background
    cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);
    cairo_rectangle(cr, 0, 0, width, height);
    cairo_fill(cr);

    list_surface = cairo_surface_create_similar(cairo_get_target(cr), CAIRO_CONTENT_COLOR, \
                            RECTWP(&global_param.list_area), RECTHP(&global_param.list_area));
    list_cr = cairo_create(list_surface);

    // draw rectangle
    cairo_select_font_face(list_cr, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    // 这个字体大小，需要用户最终根据窗口大小调节下
//    cairo_set_font_size(list_cr, LIST_FONT_SIZE);
    cairo_set_font_size(list_cr, global_param.name_font_size);
    draw_rect(list_cr);

    // draw rect0, rect1, rect2 to cr
    cairo_set_operator(cr, CAIRO_OPERATOR_ADD);
    cairo_set_source_surface(cr, list_surface, LIST_MARGIN, LIST_MARGIN);
    cairo_paint(cr);

    // destroy temp surface and cr
    cairo_destroy(list_cr);
    cairo_surface_destroy(list_surface);

    cairo_restore(cr);
    return;
}

static void animated_speed(MGEFF_ANIMATION handle, HWND hWnd, int id, float *value)
{
    float speed = *value;
    HDC hdc = NULL;

    if(abs(speed - global_param.speed) < 1.0)
        return;

    global_param.speed = speed;

    hdc = GetClientDC(hWnd);

    if(global_param.full_screen)
    {
        cairo_t * full_cr = NULL;
        cairo_surface_t * full_surface = NULL;

        full_surface = create_direct_image_surface(hdc, &global_param.full_rect);
        full_cr = cairo_create(full_surface);

        global_param.mouse_delt += speed / 100;

        paint_list(full_cr, hdc, RECTWP(&global_param.full_rect), RECTHP(&global_param.full_rect));

        if(full_cr)
        {
            cairo_destroy(full_cr);
            finish_direct_image_surface(full_surface, hdc);
        }
    }

    ReleaseDC(hdc);
}

static LRESULT WheelWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static PLOGFONT font_caption;
    static PLOGFONT font_name;

    switch (message) {
        case MSG_PAINT:
        {
            HDC hdc = BeginPaint (hWnd);
            char *name = NULL;

            SetTextColor (hdc, DWORD2Pixel (hdc, 0xFFFFFFFF));
            SetBkMode (hdc, BM_TRANSPARENT);
            SelectFont(hdc, font_caption);
            if(global_param.caption)
                DrawText (hdc, global_param.caption, strlen((char *)global_param.caption), \
                            &global_param.caption_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            if(global_param.full_screen)
            {
                cairo_t * full_cr = NULL;
                cairo_surface_t * full_surface = NULL;
                full_surface = create_direct_image_surface(hdc, &global_param.full_rect);
                full_cr = cairo_create(full_surface);

                paint_list(full_cr, hdc, RECTWP(&global_param.full_rect), RECTHP(&global_param.full_rect));

                if(full_cr)
                {
                    cairo_destroy(full_cr);
                    finish_direct_image_surface(full_surface, hdc);
                }
            }
            else
            {
                paint_svg(hWnd, hdc, global_param.icon_rect, global_param.svg_handle, global_param.color_style);

                SetTextColor (hdc, DWORD2Pixel (hdc, global_param.text_color));
                SetBkMode (hdc, BM_TRANSPARENT);
                SelectFont(hdc, font_name);
                name = global_param.member + global_param.select  * MAX_NAME_LENGTH;
                if(name[0])
                    DrawText (hdc, name, strlen(name), \
                            &global_param.name_rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            }

            EndPaint (hWnd, hdc);

            return 0;
        }
        case MSG_CREATE:
        {
            char path[HISHELL_MAX_PATH] = {0};
            RECT window_rect;
            int i = 0;

            GetWindowRect(hWnd, &window_rect);
            parse_config(RECTW(window_rect), RECTH(window_rect));

            global_param.list_area.left = global_param.full_rect.left + LIST_MARGIN;
            global_param.list_area.top = global_param.full_rect.top + LIST_MARGIN;
            global_param.list_area.right = global_param.full_rect.right - LIST_MARGIN;
            global_param.list_area.bottom = global_param.full_rect.bottom - LIST_MARGIN;

            // load svg file
            readlink("/proc/self/exe", path, HISHELL_MAX_PATH);
            if(global_param.svg_file[0])
                sprintf(path, "%s/res/%s", dirname(path), global_param.svg_file);
            else
                sprintf(path, "%s/res/list.svg", dirname(path));
            loadSVGFromFile(path, &global_param.svg_handle);

            // create the fonts
            if(global_param.caption_font_size == 0)
                global_param.caption_font_size = CAPTION_FONT;
            font_caption = CreateLogFontEx (FONT_TYPE_NAME_SCALE_TTF, "Serif", "UTF-8",
                        FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN, FONT_FLIP_NIL,
                        FONT_OTHER_AUTOSCALE, FONT_UNDERLINE_NONE, FONT_RENDER_SUBPIXEL,
                        global_param.caption_font_size, 0);

            if(global_param.name_font_size == 0)
                global_param.name_font_size = NAME_FONT;
            font_name = CreateLogFontEx (FONT_TYPE_NAME_SCALE_TTF, "Serif", "UTF-8",
                        FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN, FONT_FLIP_NIL,
                        FONT_OTHER_AUTOSCALE, FONT_UNDERLINE_NONE, FONT_RENDER_SUBPIXEL,
                        global_param.name_font_size, 0);

            global_param.internal = (RECTHP(&global_param.list_area) - \
                                    LIST_LINE * LIST_NAME_HEIGHT) / (LIST_LINE - 1);
            for(i = 0; i < LIST_LINE; i++)
            {
                global_param.list_rect[i].left = 0;
                global_param.list_rect[i].top = i * (LIST_NAME_HEIGHT + global_param.internal);
                global_param.list_rect[i].right = RECTHP(&global_param.list_area);
                global_param.list_rect[i].bottom = global_param.list_rect[i].top + LIST_NAME_HEIGHT;
            }

            // 计算每条记录的y坐标，相对于selected
            if(global_param.list_number)
            {
                global_param.rect_y = malloc(global_param.list_number * sizeof(int));
                for(i = 0; i < global_param.list_number; i++)
                    global_param.rect_y[i] = i * (LIST_NAME_HEIGHT + global_param.internal) + LIST_NAME_HEIGHT;
            }
        }
            break;

        case MSG_TIMER:
            if(wParam == TIMER_LONG_PRESS)
            {
                if(global_param.full_screen)
                {
 //                   mouseinfo.m_bPressed = TRUE;
                }
                else
                {
                    if(global_param.pressed_long)
                    {
                        SetTimer(hWnd, TIMER_QUIT_FULL, QUIT_FULL_PERIOD);
                        global_param.full_screen = TRUE;
                        SetMainWindowGestureFlags(hWnd, ZOF_GF_SWIPE_HORZ | ZOF_GF_SWIPE_VERT);
                        InvalidateRect(hWnd, NULL, TRUE);
                    }
                }
                KillTimer(hWnd, TIMER_LONG_PRESS);
                global_param.pressed_long = FALSE;
            }
            else if(wParam == TIMER_QUIT_FULL)
            {
                if(global_param.full_screen)
                {
                    global_param.full_screen = FALSE;
                    SetMainWindowGestureFlags(hWnd, 0);
                    InvalidateRect(hWnd, NULL, TRUE);
                }
                KillTimer(hWnd, TIMER_QUIT_FULL);
            }
            break;
            
        case MSG_LBUTTONDOWN:
        {
            int x = LOSWORD(lParam);
            int y = HISWORD(lParam);
            BOOL in_rect = FALSE;

            if(global_param.full_screen)
            {
                in_rect = PtInRect (&global_param.full_rect, x, y);
                if(in_rect)                                     // 全屏，且在全屏区域，全屏时的鼠标操作
                {
                    if(mouseinfo.m_speedmeter)
                    {
                        mSpeedMeter_destroy(mouseinfo.m_speedmeter);
                        mouseinfo.m_speedmeter = NULL;
                    }

                    memset(&mouseinfo, 0, sizeof(MoseSpeed));

                    mouseinfo.m_bPressed = TRUE;
                    mouseinfo.m_mouseFlag = 0;
                    mouseinfo.m_bTimedout = FALSE;
                    mouseinfo.m_timePressed = GetTickCount();
                    mouseinfo.m_oldMousePos.x = x;
                    mouseinfo.m_oldMousePos.y = y;
                    mouseinfo.m_pressMousePos = mouseinfo.m_oldMousePos;
                }
                else                                            // 全屏，且不在全屏区域，退出全屏区域
                {
                    KillTimer(hWnd, TIMER_QUIT_FULL);
                    KillTimer(hWnd, TIMER_LONG_PRESS);
                    global_param.pressed_long = FALSE;
                    global_param.full_screen = FALSE;
                    SetMainWindowGestureFlags(hWnd, 0);
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }
            else
            {
                in_rect = PtInRect (&global_param.press_rect, x, y);
                if(in_rect)                                     // 不是全屏，且在长按有效区域，显示
                {
                    KillTimer(hWnd, TIMER_LONG_PRESS);
                    global_param.pressed_long = TRUE;
                    SetTimer(hWnd, TIMER_LONG_PRESS, LONG_PRESS_PERIOD);
                }
                else                                            // 不是全屏，且不在长按区有效，不做处理
                {
                }
            }
        }
            break;

        case MSG_LBUTTONUP:
        {
            int x = LOSWORD(lParam);
            int y = HISWORD(lParam);

            if(global_param.full_screen)
            {
                if(mouseinfo.m_bMouseMoved && mouseinfo.m_bPressed)
                {
                    float v_x = 0.0;
                    float v_y = 0.0;

                    mSpeedMeter_append(mouseinfo.m_speedmeter, x, y, GetTickCount() * 10);
                    mSpeedMeter_stop(mouseinfo.m_speedmeter);
                    mSpeedMeter_query_velocity(mouseinfo.m_speedmeter, &v_x, &v_y);

                    // do animation
                    if(abs(v_y) < LIST_LIMIT_SPEED)
                    {
                        global_param.select = global_param.move_select;
                        InvalidateRect(hWnd, NULL, TRUE);
                    }
                    else
                    {
                        MGEFF_ANIMATION animation = NULL;
                        animation = mGEffAnimationCreate(hWnd, (void *)animated_speed, 1, MGEFF_FLOAT);
                        if (animation)
                        {
                            enum EffMotionType motionType = OutQuad;
                            int duration = LIST_ANIMATION_TIME;
                            float speed_end = 0.0;

                            global_param.speed = v_y;

                            mGEffAnimationSetStartValue(animation, &v_y);
                            mGEffAnimationSetEndValue(animation, &speed_end);
                            mGEffAnimationSetDuration(animation, duration);
                            mGEffAnimationSetCurve(animation, motionType);
                            mGEffAnimationSyncRun(animation);
                            mGEffAnimationDelete(animation);
                        }

                        global_param.select = global_param.move_select;
                        InvalidateRect(hWnd, NULL, TRUE);
                        global_param.select = global_param.move_select;
                    }
                }
                KillTimer(hWnd, TIMER_QUIT_FULL);
                SetTimer(hWnd, TIMER_QUIT_FULL, QUIT_FULL_PERIOD);
            }
            else
            {
            }

            global_param.mouse_delt = 0;
            mouseinfo.m_bPressed = FALSE;
            mouseinfo.m_bMouseMoved = FALSE;

            KillTimer(hWnd, TIMER_LONG_PRESS);
            global_param.pressed_long = FALSE;
        }
            break;

        case MSG_MOUSEMOVE:
            if(global_param.full_screen)
            {
                int x = LOSWORD(lParam);
                int y = HISWORD(lParam);

                if(mouseinfo.m_bPressed)
                {
                    mouseinfo.m_bMouseMoved = TRUE;

                    if(PtInRect (&global_param.full_rect, x, y))
                    {
                        KillTimer(hWnd, TIMER_QUIT_FULL);

                        if (mouseinfo.first)
                        {
                            lParam = MAKELONG(x, y);
                            assert(mouseinfo.m_speedmeter);
                            mSpeedMeter_append(mouseinfo.m_speedmeter, x, y, GetTickCount() * 10);
                        }
                        else
                        {
                            assert(mouseinfo.m_speedmeter == NULL);
                            mouseinfo.m_speedmeter = mSpeedMeter_create (1000, 10);
                            mSpeedMeter_reset(mouseinfo.m_speedmeter);
                            mSpeedMeter_append(mouseinfo.m_speedmeter, mouseinfo.m_oldMousePos.x,
                                    mouseinfo.m_oldMousePos.y, GetTickCount() * 10);
                            mouseinfo.first = 1;
                        }

                        // do animation
                        global_param.mouse_delt = y - mouseinfo.m_oldMousePos.y;
                        InvalidateRect(hWnd, NULL, TRUE);
                    }
                    else
                    {
                        float v_x = 0.0;
                        float v_y = 0.0;

                        mouseinfo.m_bPressed = FALSE;
                        mouseinfo.m_bMouseMoved = FALSE;
                        mSpeedMeter_append(mouseinfo.m_speedmeter, x, y, GetTickCount() * 10);
                        mSpeedMeter_stop(mouseinfo.m_speedmeter);
                        mSpeedMeter_query_velocity(mouseinfo.m_speedmeter, &v_x, &v_y);

                        // do animation
                        if(abs(v_y) < LIST_LIMIT_SPEED)
                        {
                            global_param.select = global_param.move_select;
                            global_param.mouse_delt = 0;
                            InvalidateRect(hWnd, NULL, TRUE);
                        }
                        else
                        {
                            MGEFF_ANIMATION animation = NULL;
                            animation = mGEffAnimationCreate(hWnd, (void *)animated_speed, 1, MGEFF_FLOAT);
                            if (animation)
                            {
                                enum EffMotionType motionType = OutQuad;
                                int duration = LIST_ANIMATION_TIME;
                                float speed_end = 0.0;

                                global_param.speed = v_y;

                                mGEffAnimationSetStartValue(animation, &v_y);
                                mGEffAnimationSetEndValue(animation, &speed_end);
                                mGEffAnimationSetDuration(animation, duration);
                                mGEffAnimationSetCurve(animation, motionType);
                                mGEffAnimationSyncRun(animation);
                                mGEffAnimationDelete(animation);
                            }

                            global_param.select = global_param.move_select;
                            InvalidateRect(hWnd, NULL, TRUE);
                            global_param.select = global_param.move_select;
                            global_param.mouse_delt = 0;
                        }
                        KillTimer(hWnd, TIMER_QUIT_FULL);
                        SetTimer(hWnd, TIMER_QUIT_FULL, QUIT_FULL_PERIOD);
                    }
                }
            }
            break;

        case MSG_GESTURETEST:
            if(global_param.full_screen)
                SetMainWindowGestureFlags(hWnd, ZOF_GF_SWIPE_HORZ | ZOF_GF_SWIPE_VERT);
            else
                SetMainWindowGestureFlags(hWnd, 0);
            break;

        case MSG_CLOSE:
            if(global_param.svg_handle)
                hisvg_handle_destroy(global_param.svg_handle);
            DestroyLogFont(font_caption);
            DestroyLogFont(font_name);

            if(global_param.rect_y)
                free(global_param.rect_y);
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
            return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;
    char layer[16] = {0};
    char hibus_name[MAX_NAME_LENGTH] = {0};
    int fd_hibus = -1;
    int opt;
    int compos_type = CT_OPAQUE;
    DWORD bkgnd_color = MakeRGBA (0x00, 0x00, 0x00, 0xFF); 

    if(argc < 3)
        return 1;

    memset(&global_param, 0, sizeof(Global_Param));

    while((opt = getopt(argc, (char *const *)argv, "l:b:d:c:p:t:"))!= -1)
    {
        switch(opt)
        {
            case 'l':           // layer name
                sprintf(layer, "%s", optarg);
                break;
            case 'b':           // hibus name
                sprintf(hibus_name, "%s", optarg);
                break;
            case 'd':           // user defined config file
                sprintf(global_param.config_file, "%s", optarg);
                break;
            case 'c':           // css file
                sprintf(global_param.css_file, "%s", optarg);
                break;
            case 'p':           // picture file
                sprintf(global_param.svg_file, "%s", optarg);
                break;
            case 't':
                sprintf(global_param.caption, "%s", optarg);
                break;
        }
    }

    mGEffInit();

#ifdef _MGRM_PROCESSES
    JoinLayer(layer , "wheel" , 0 , 0);
#endif

    CreateInfo.dwStyle = WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_AUTOPOSITION;
    CreateInfo.spCaption = "";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = WheelWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = g_rcScr.right;
    CreateInfo.by = g_rcScr.bottom;
    CreateInfo.iBkColor = COLOR_black;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
#ifdef TRANSPARENT_BK
    compos_type = CT_ALPHAPIXEL;
    bkgnd_color = MakeRGBA (0x00, 0x00, 0x00, BK_TRANSPARENT); 
#endif

    hMainWnd = CreateMainWindowEx2 (&CreateInfo, 0L, NULL, NULL, ST_PIXEL_ARGB8888,
                                    bkgnd_color, compos_type, COLOR_BLEND_LEGACY);
    
    if (hMainWnd == HWND_INVALID)
        return -1;

    REQUEST request;
    RequestInfo requestinfo;
    ReplyInfo replyInfo;

    // submit zNode index to server
    requestinfo.id = REQ_SUBMIT_PID_HWND;
    requestinfo.hwnd = hMainWnd;
    requestinfo.pid = getpid();

    request.id = FORMAT_REQID;
    request.data = (void *)&requestinfo;
    request.len_data = sizeof(requestinfo);

    memset(&replyInfo, 0, sizeof(ReplyInfo));
    ClientRequest(&request, &replyInfo, sizeof(ReplyInfo));

    ShowWindow(hMainWnd, SW_SHOWNORMAL);

    global_param.main_hwnd = hMainWnd;
    fd_hibus = start_hibus(&(global_param.hibus_context), hibus_name);
    if(fd_hibus <= 0)
    {
        fprintf (stderr, "Can not connect to hibus.\n");
        return 3;
    }
    RegisterListenFD (fd_hibus, POLLIN, hMainWnd, NULL);

    while (GetMessage(&Msg, hMainWnd)) {
        if(Msg.message == MSG_FDEVENT)
            hibus_read_and_dispatch_packet(global_param.hibus_context);

        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);
    end_hibus(global_param.hibus_context);
    mGEffDeinit();

    return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

