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

// for strength length
#define MAX_NAME_LENGTH     64
#define HISHELL_MAX_PATH    256

#define CAPTION_FONT        25 

// timer id
#define TIMER_QUIT_FULL     110
#define QUIT_FULL_PERIOD    300

// for drag
#define PRESS_TIMEOUT           10
#define CLICK_TIMEOUT           8
#define CLICK_MICRO_MOVEMENT    8

// for hibus
#define SOCKET_PATH                 "/var/tmp/hibus.sock"
#define HIBUS_HISHELL_NAME          "cn.fmsoft.hybridos.hishell"

typedef struct tag_Global_Param
{
    unsigned char caption[MAX_NAME_LENGTH];

    RECT         caption_rect;
    RECT         icon_rect[2];

    hibus_conn * hibus_context;
    HWND         main_hwnd;

    int          font_size;
} Global_Param;



#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _CONFIG_h */
