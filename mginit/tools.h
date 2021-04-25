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
** tools.h: It declares some functions for whole program.
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


#ifndef _TOOL_H_
#define _TOOL_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

void child_wait (int sig);
pid_t exec_hishell_app(char * app);
pid_t exec_runner(char *path, char * app, char * param);
void start_apps(void);
void end_apps(void);

page_struct * find_page_by_id(int page_id);
page_struct * find_next_page(page_struct * page, BOOL circle);
page_struct * find_prev_page(page_struct * page, BOOL circle);
runner_struct * find_pid(pid_t pid);
int find_clientId_by_pid(pid_t pid);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  // _TOOL_H_
