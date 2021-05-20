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
** sysconfig.h: It defines some strutures and micro for all applications.
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

#ifndef _SYSTEM_SVG_COMMON
#define _SYSTEM_SVG_COMMON

#include <cairo/cairo.h>
#include <cairo/cairo-minigui.h>

#ifdef CAIRO_HAS_DRM_SURFACE
#include <cairo/cairo-drm.h>
#endif

#include <hisvg.h>
#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

// svg fucntions
//HDC create_memdc_from_image_surface (cairo_surface_t *);
//void loadSVGFromFile(const char *, cairo_t **, cairo_surface_t **, char *, RECT);
//void paint_svg(HWND, HDC, RECT, cairo_surface_t *);


BOOL loadSVGFromFile(const char* file, HiSVGHandle ** svg_handle);
void paint_svg(HWND hwnd, HDC hdc, RECT rect, HiSVGHandle * svg_handle, char * color_style);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _SYSTEM_SVG_COMMON */
