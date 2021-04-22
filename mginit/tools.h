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
