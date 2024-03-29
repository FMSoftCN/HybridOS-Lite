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
** title-description.h: It declares some functions for bar windows.
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

#ifndef _MGINIT_BAR_H_
#define _MGINIT_BAR_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


HWND create_title_bar(void);
HWND create_indicator_bar(void);
HWND create_description_bar(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  // _MGINIT_BAR_H_
