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
#include "config.h"
#include "hibus.h"
#include "hidomlayout.h"


Global_Param global_param;

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


// get user data and layout
static BOOL parse_config(int width, int height)
{
    char file_path[HISHELL_MAX_PATH] = {0};
    int default_length = 0;
    char * css_content = NULL;

    // get user date from file
    if(global_param.config_file[0])
    {
        readlink("/proc/self/exe", file_path, HISHELL_MAX_PATH);
        sprintf(file_path, "%s/config/%s", dirname(file_path), global_param.config_file);

        if((access(file_path, F_OK | R_OK)) != 0)
            return FALSE;

        if(global_param.caption[0] == 0)
            GetValueFromEtcFile(file_path, "AppConfigData", "caption", \
                                        global_param.caption, MAX_NAME_LENGTH);

        memset(global_param.button_color, 0, 32);
        if(GetValueFromEtcFile(file_path, "AppConfigData", "icon_color", \
                                    global_param.button_color, 32) == ETC_OK)
        {
            global_param.color_pair.name = "color";
            global_param.color_pair.value = global_param.button_color;
            global_param.color_pair.important = 0;
        }
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


    hilayout_element_node_append_as_last_child(caption, root);
    hilayout_element_node_append_as_last_child(icon, root);

    hilayout_do_layout(&hl_media, css, root);

    // caption
    box = hilayout_element_node_get_used_box_value(caption);
    global_param.caption_rect.left = (int)box->x;
    global_param.caption_rect.top = (int)box->y;
    global_param.caption_rect.right = (int)(box->x + box->w);
    global_param.caption_rect.bottom = (int)(box->y +box->h);

    text = hilayout_element_node_get_used_text_value(caption);
    global_param.font_size = text->font_size;

    // icon 
    box = hilayout_element_node_get_used_box_value(icon);
    global_param.icon_rect.left = (int)box->x;
    global_param.icon_rect.top = (int)box->y;
    global_param.icon_rect.right = (int)(box->x + box->w);
    global_param.icon_rect.bottom = (int)(box->y +box->h);

    hilayout_css_destroy(css);

    if(root)
        hilayout_element_node_destroy(root);
    if(caption)
        hilayout_element_node_destroy(caption);
    if(icon)
        hilayout_element_node_destroy(icon);

    if(css_content)
        free(css_content);

    return TRUE;
}

// for cairo
static cairo_surface_t *create_cairo_surface (HDC hdc, int width, int height)
{
    static cairo_device_t* cd = NULL;

    if (hdc == HDC_INVALID) 
        return cairo_minigui_surface_create_with_memdc (cd, CAIRO_FORMAT_RGB24, width, height);
    else if (hdc != HDC_SCREEN && width > 0 && height > 0) 
        return cairo_minigui_surface_create_with_memdc_similar (cd, hdc, width, height);
    return cairo_minigui_surface_create (cd, hdc);
}

// for svg file
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

static void loadSVGFromFile(const char* file)
{
    RsvgHandle *handle;
    GError *error = NULL;
    RsvgDimensionData dimensions;
    double factor_width = 0.0f;
    double factor_height = 0.0f;
    int width = RECTW(global_param.icon_rect);
    int height = width;

    // read file from svg file
    handle = rsvg_handle_new_from_file(file, &error);
    if(error)
    {
        global_param.icon_surface = NULL;
        global_param.icon_cr = NULL;
        return;
    }
    rsvg_handle_get_dimensions(handle, &dimensions);

    // create cairo_surface_t and cairo_t for one picture
    global_param.icon_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, \
                                                                        width, height);
    factor_width = (double)width / (double)dimensions.width;
    factor_height = (double)height / (double)dimensions.height;
    global_param.icon_cr = cairo_create(global_param.icon_surface);

    cairo_save(global_param.icon_cr);
    factor_width = (factor_width > factor_height) ? factor_width : factor_height;
    cairo_scale(global_param.icon_cr, factor_width, factor_width);

    float r = 0.0;
    float g = 0.0;
    float b = 0.0;
    float alpha = 1.0;

    cairo_set_source_rgb(global_param.icon_cr,  r * alpha, g * alpha, b * alpha);
    cairo_paint(global_param.icon_cr);
    rsvg_handle_render_cairo_style(handle, global_param.icon_cr, \
                                            &global_param.color_pair, 1);
    cairo_restore(global_param.icon_cr);

    g_object_unref (handle);
}


static void paint_svg(HWND hwnd, HDC hdc)
{
    float alpha = 1.0;
    int width = RECTW(global_param.icon_rect);
    int height = width;

    HDC csdc = create_memdc_from_image_surface(global_param.icon_surface);
    if (csdc != HDC_SCREEN && csdc != HDC_INVALID)
    {
        SetMemDCColorKey(csdc, MEMDC_FLAG_SRCCOLORKEY,
                MakeRGB(255 * alpha, 255 * alpha, 255 * alpha));
        BitBlt(csdc, 0, 0, width, height, hdc, \
                    global_param.icon_rect.left, global_param.icon_rect.top, 0);
    }
    DeleteMemDC(csdc);
    return;
}

static LRESULT GearWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    char string[MAX_NAME_LENGTH];
    static PLOGFONT font_caption;
    static HDC hmapdc;

    switch (message) {
        case MSG_PAINT:
            hdc = BeginPaint (hWnd);

            // caption
            SetTextColor (hdc, DWORD2Pixel (hdc, 0xFFFFFFFF));
            SetBkMode (hdc, BM_TRANSPARENT);
            SelectFont(hdc, font_caption);
            if(global_param.caption[0])
                DrawText (hdc, global_param.caption, strlen((char *)global_param.caption), \
                        &global_param.caption_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            paint_svg(hWnd, hdc);

            EndPaint (hWnd, hdc);
            return 0;

        case MSG_CREATE:
        {
            char path[HISHELL_MAX_PATH] = {0};
            RECT window_rect;

            GetWindowRect(hWnd, &window_rect);
            parse_config(RECTW(window_rect), RECTH(window_rect));

            // load svg file
            readlink("/proc/self/exe", path, HISHELL_MAX_PATH);
            if(global_param.svg_file[0])
                sprintf(path, "%s/res/%s", dirname(path), global_param.svg_file);
            else
                sprintf(path, "%s/res/gear.svg", dirname(path));
            loadSVGFromFile(path);

            if(global_param.font_size == 0)
                global_param.font_size = CAPTION_FONT;

            // create font
            font_caption = CreateLogFont (FONT_TYPE_NAME_SCALE_TTF, "ttf-Source", "UTF-8",
                        FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN, FONT_FLIP_NIL,
                        FONT_OTHER_AUTOSCALE, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                        global_param.font_size, 0);

            // do not use getsture
            SetMainWindowGestureFlags(hWnd, 0);
        }
            break;

        case MSG_CLOSE:
            if(hmapdc)
                DeleteMemDC(hmapdc);

            cairo_surface_destroy(global_param.icon_surface);
            cairo_destroy(global_param.icon_cr);

            DestroyLogFont(font_caption);

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

#ifdef _MGRM_PROCESSES
    JoinLayer(layer , "svgshow" , 0 , 0);
#endif

    CreateInfo.dwStyle = WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_AUTOPOSITION;
    CreateInfo.spCaption = "";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = GearWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = g_rcScr.right;
    CreateInfo.by = g_rcScr.bottom;
    CreateInfo.iBkColor = COLOR_black;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
    
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

    return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

