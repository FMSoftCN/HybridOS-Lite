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
 * \file bar.c
 * \author Gengyue <gengyue@minigui.org>
 * \date 2020/09/16
 *
 * \brief This file implements status bar in system manager process.
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
 * $Id: bar.c 13674 2020-09-16 06:45:01Z Gengyue $
 *
 *      HybridOS for Linux, VxWorks, NuCleus, OSE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <mgeff/mgeff.h>

#include <cairo/cairo.h>
#include <cairo/cairo-minigui.h>

#ifdef CAIRO_HAS_DRM_SURFACE
#include <cairo/cairo-drm.h>
#endif

#include <hibus.h>

#include "../include/sysconfig.h"
#include "config.h"
#include "tools.h"
#include "title-description.h"

#define TIME_INFO_X         100
extern OS_Global_struct __os_global_struct;

// get current time
static char* mk_time(char* buff)
{
    time_t t;
    struct tm * tm;

    time (&t);
    tm = localtime (&t);
    sprintf(buff, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);

    return buff;
}

// the window proc of title bar
static LRESULT TitleBarWinProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    static PLOGFONT font_title;
    char buff [20];

    switch (message) 
    {
        case MSG_PAINT:
        {
            RECT rect;
            page_struct * page = find_page_by_id(__os_global_struct.current_page + 1);
            GetClientRect(hWnd, &rect);
            if(page && page->title_en)
            {
                hdc = BeginPaint (hWnd);
                SetTextColor (hdc, DWORD2Pixel (hdc, 0xFFFFFFFF));
                SetBkMode (hdc, BM_TRANSPARENT);
                SelectFont(hdc, font_title);
                rect.left += 10;
                rect.right -= 10;
                DrawText (hdc, page->title_en, strlen((char *)page->title_en), &rect, \
                                                DT_LEFT | DT_VCENTER | DT_SINGLELINE);
                EndPaint (hWnd, hdc);
            }
        }
            return 0;

        case MSG_CREATE:
            font_title = CreateLogFont (FONT_TYPE_NAME_SCALE_TTF, "ttf-Source", "UTF-8",
                        FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN, FONT_FLIP_NIL,
                        FONT_OTHER_AUTOSCALE, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                        15, 0);
            break;

        case MSG_MAINWINDOW_CHANGE:
            InvalidateRect(hWnd, NULL, TRUE);
            break;

        case MSG_DESTROY:
            DestroyLogFont(font_title);
            break;

        case MSG_CLOSE:
            DestroyAllControls (hWnd);
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
            return 0;
    }

    return DefaultMainWinProc (hWnd, message, wParam, lParam);
}

// the window proc of description bar
static LRESULT DescriptionBarWinProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    static PLOGFONT font_time;
    char buff [20];
    static RECT rect[2];

    switch (message) 
    {
        case MSG_PAINT:
        {
            page_struct * page = find_page_by_id(__os_global_struct.current_page + 1);
            hdc = BeginPaint (hWnd);
            SetTextColor (hdc, DWORD2Pixel (hdc, 0xFFFFFFFF));
            SetBkMode (hdc, BM_TRANSPARENT);
            SelectFont(hdc, font_time);

            if(page && page->description_zh)
                DrawText (hdc, page->description_zh, strlen((char *)page->description_zh), \
                                    rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

            mk_time(buff);
            DrawText (hdc, buff, strlen(buff), rect + 1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            EndPaint (hWnd, hdc);
            return 0;
        }

        case MSG_CREATE:
        {
            RECT window_rect;
            GetClientRect(hWnd, &window_rect);

            rect[0].left = 10;
            rect[0].top = 0;
            rect[0].right = window_rect.right - TIME_INFO_X - 10;
            rect[0].bottom = window_rect.bottom;

            rect[1].left = window_rect.right - TIME_INFO_X + 10;
            rect[1].top = 0;
            rect[1].right = window_rect.right - 10;
            rect[1].bottom = window_rect.bottom;

            font_time = CreateLogFont (FONT_TYPE_NAME_SCALE_TTF, "ttf-Source", "UTF-8",
                        FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN, FONT_FLIP_NIL,
                        FONT_OTHER_AUTOSCALE, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                        15, 0);

            SetTimer(hWnd, ID_TIMER_BAR, 100);
        }
            break;

        case MSG_TIMER:
            if(wParam == ID_TIMER_BAR)
                InvalidateRect(hWnd, rect + 1, TRUE);
            break;

        case MSG_MAINWINDOW_CHANGE:
            InvalidateRect(hWnd, rect, TRUE);
            break;

        case MSG_DESTROY:
            DestroyLogFont(font_time);
            break;

        case MSG_CLOSE:
            KillTimer (hWnd, ID_TIMER_BAR);
            DestroyAllControls (hWnd);
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
            return 0;
    }

    return DefaultMainWinProc (hWnd, message, wParam, lParam);
}

// create title bar
HWND create_title_bar(void)
{
    MAINWINCREATE CreateInfo;

    // create a main window
    CreateInfo.dwStyle = WS_ABSSCRPOS | WS_VISIBLE;
    CreateInfo.dwExStyle = 0;
    CreateInfo.spCaption = "";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor (0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = TitleBarWinProc;
    CreateInfo.lx = __os_global_struct.rect_title.left; 
    CreateInfo.ty = __os_global_struct.rect_title.top;
    CreateInfo.rx = __os_global_struct.rect_title.right;
    CreateInfo.by = __os_global_struct.rect_title.bottom;
    CreateInfo.iBkColor = RGBA2Pixel(HDC_SCREEN, 0xFF, 0xFF, 0xFF, 0xFF); 
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    __os_global_struct.hTitleBar = CreateMainWindowEx2 (&CreateInfo, 0L, \
                    NULL, NULL, ST_PIXEL_ARGB8888,
                    MakeRGBA (BK_COLOR_R, BK_COLOR_G, BK_COLOR_B, BK_TRANSPARENT),
                    CT_ALPHAPIXEL, 0xFF);
    if (__os_global_struct.hTitleBar == HWND_INVALID)
        return HWND_INVALID;

    return __os_global_struct.hTitleBar;
}

// create description bar
HWND create_description_bar(void)
{
    MAINWINCREATE CreateInfo;

    // create a main window
    CreateInfo.dwStyle = WS_ABSSCRPOS | WS_VISIBLE;
    CreateInfo.dwExStyle = 0;
    CreateInfo.spCaption = "";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor (0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = DescriptionBarWinProc;
    CreateInfo.lx = __os_global_struct.rect_description.left; 
    CreateInfo.ty = __os_global_struct.rect_description.top;
    CreateInfo.rx = __os_global_struct.rect_description.right;
    CreateInfo.by = __os_global_struct.rect_description.bottom;
    CreateInfo.iBkColor = RGBA2Pixel(HDC_SCREEN, 0xFF, 0xFF, 0xFF, 0xFF); 
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    __os_global_struct.hDescriptionBar = CreateMainWindowEx2 (&CreateInfo, 0L, \
                    NULL, NULL, ST_PIXEL_ARGB8888,
                    MakeRGBA (BK_COLOR_R, BK_COLOR_G, BK_COLOR_B, BK_TRANSPARENT),
                    CT_ALPHAPIXEL, 0xFF);
    if (__os_global_struct.hDescriptionBar == HWND_INVALID)
        return HWND_INVALID;

    return __os_global_struct.hDescriptionBar;
}
