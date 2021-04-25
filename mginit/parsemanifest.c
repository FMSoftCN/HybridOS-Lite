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
** parsemanifest.c: It illustrates how to interpret manifest file and layout.
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <libgen.h>
#include <hibus.h>
#include <hibox/json.h>
#include <hidomlayout.h>

#include "config.h"
#include "tools.h"

extern OS_Global_struct __os_global_struct;

#define FONT_SUBPIXEL_SIZE 20

static void create_font_name(runner_struct *runner, const HLUsedTextValues* text)
{
    int length = strlen(text->font_family);
    int size = text->font_size;
    char style[LEN_FONT_NAME + 1];
    static const char weight[] = {FONT_WEIGHT_THIN, FONT_WEIGHT_EXTRA_LIGHT, 
            FONT_WEIGHT_LIGHT, FONT_WEIGHT_REGULAR, FONT_WEIGHT_MEDIUM,
            FONT_WEIGHT_DEMIBOLD, FONT_WEIGHT_BOLD, FONT_WEIGHT_EXTRA_BOLD,
            FONT_WEIGHT_BLACK};

    runner->font_name = malloc(length + 32);

    if(size > 1024)
        size = 1024;
    else if(size < 0)
        size = 1;

    strcpy(style, "rrncnn");
    style[0] = weight[text->font_weight];

    if (text->font_size <= FONT_SUBPIXEL_SIZE) {
        style[5] = FONT_RENDER_SUBPIXEL;
    }
    else {
        style[5] = FONT_RENDER_GREY;
    }

    sprintf(runner->font_name, "ttf-%s-%s-*-%d-utf-8", text->font_family, style, size);
}

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

static int layout_page(page_struct * page, HLDomElementNode* root)
{
    runner_struct * runner = page->runner;

    while(runner)
    {
        if((runner->runas & RA_WINDOW) || (runner->runas & RA_ACTIVITY))
        {
            if(runner->type == RT_HIJS)
                runner->node = hilayout_element_node_create("hijs");
            else if(runner->type == RT_HIWEB)
                runner->node = hilayout_element_node_create("hiweb");
            else
                runner->node = hilayout_element_node_create("minigui");

            if(runner->node)
            {
                if(runner->css_class)
                    hilayout_element_node_set_class(runner->node, runner->css_class);
                if(runner->styles)
                    hilayout_element_node_set_style(runner->node, runner->styles);
                hilayout_element_node_append_as_last_child(runner->node, root);
            }
        }
        runner = runner->next;
    }

    return 0;
}

int layout_applications(void)
{
    int quit_layout = 0;                            // whether quit layout 
    int temp = 0;

    char default_path[HISHELL_MAX_PATH] = {0};      // get the css file path

    char * default_file = NULL;                     // for default_layout.css
    int default_length = 0;                         // length of default_layout.css
    char * css_file = NULL;                         // for user defined css file
    int css_length = 0;                             // length of user defined css file
    
    page_struct * page = __os_global_struct.page;
    runner_struct * runner = NULL;

    HLCSS* css = NULL;
    HLMedia hl_media;
    const HLUsedBoxValues* box = NULL;
    const HLUsedBackgroundValues* bg = NULL;
    const HLUsedTextValues* text = NULL;

    hl_media.width = g_rcScr.right;
    hl_media.height = g_rcScr.bottom;
    hl_media.dpi = GetGDCapability(HDC_SCREEN, GDCAP_DPI);
    hl_media.density = hl_media.dpi;

    while(!quit_layout)
    {
        css = hilayout_css_create();
        if (css == NULL)
            return HILAYOUT_INVALID;

        // step 1: read default css file
        if(default_file == NULL)
        {
            readlink("/proc/self/exe", default_path, HISHELL_MAX_PATH);
            sprintf(default_path, "%s/layout/%s", dirname(default_path), DEFAULT_CSS_PATH);

            default_file = get_file_content(default_path, &default_length);
            if(default_file)
                hilayout_css_append_data(css, default_file, default_length);
        }
        else
            hilayout_css_append_data(css, default_file, default_length);

        if(__os_global_struct.css_file)
        {
            if(css_file == NULL)
            {
                memset(default_path, 0, HISHELL_MAX_PATH);
                readlink("/proc/self/exe", default_path, HISHELL_MAX_PATH);
                sprintf(default_path, "%s/layout/%s", dirname(default_path), __os_global_struct.css_file);

                css_file = get_file_content(default_path, &css_length);
                if(css_file)
                    hilayout_css_append_data(css, css_file, css_length);
            }
            else
                hilayout_css_append_data(css, css_file, css_length);
        }

        HLDomElementNode* root = hilayout_element_node_create("div");

        HLDomElementNode* title = hilayout_element_node_create("div");
        hilayout_element_node_set_id(title, "title");
        if(__os_global_struct.tile_style)
            hilayout_element_node_set_style(title, __os_global_struct.tile_style);

        HLDomElementNode* description = hilayout_element_node_create("div");
        hilayout_element_node_set_id(description, "description");
        if(__os_global_struct.description_style)
            hilayout_element_node_set_style(description, __os_global_struct.description_style);

        HLDomElementNode* page_node = hilayout_element_node_create("div");
        hilayout_element_node_set_id(page_node, "page");
        if(__os_global_struct.page_style)
            hilayout_element_node_set_style(page_node, __os_global_struct.page_style);


        HLDomElementNode* indicator = hilayout_element_node_create("div");
        hilayout_element_node_set_id(indicator, "indicator");
        if(__os_global_struct.indicator_style)
            hilayout_element_node_set_style(indicator, __os_global_struct.indicator_style);

        hilayout_element_node_append_as_last_child(title, root);
        hilayout_element_node_append_as_last_child(description, root);
        hilayout_element_node_append_as_last_child(page_node, root);
        hilayout_element_node_append_as_last_child(indicator, root);

        if(page)
        {
            if(page->styles)
                hilayout_element_node_set_style(page_node, page->styles);
            layout_page(page, page_node);
        }

        hilayout_do_layout(&hl_media, css, root);

        // get the attribute of node
        box = hilayout_element_node_get_used_box_value(title);
        __os_global_struct.rect_title.left = (int)box->x;
        __os_global_struct.rect_title.top = (int)box->y;
        __os_global_struct.rect_title.right = (int)(box->x + box->w);
        __os_global_struct.rect_title.bottom = (int)(box->y + box->h);

        box = hilayout_element_node_get_used_box_value(description);
        __os_global_struct.rect_description.left = (int)box->x;
        __os_global_struct.rect_description.top = (int)box->y;
        __os_global_struct.rect_description.right = (int)(box->x + box->w);
        __os_global_struct.rect_description.bottom = (int)(box->y + box->h);

        box = hilayout_element_node_get_used_box_value(page_node);
        __os_global_struct.rect_page.left = (int)box->x;
        __os_global_struct.rect_page.top = (int)box->y;
        __os_global_struct.rect_page.right = (int)(box->x + box->w);
        __os_global_struct.rect_page.bottom = (int)(box->y + box->h);

        box = hilayout_element_node_get_used_box_value(indicator);
        __os_global_struct.rect_indicator.left = (int)box->x;
        __os_global_struct.rect_indicator.top = (int)box->y;
        __os_global_struct.rect_indicator.right = (int)(box->x + box->w);
        __os_global_struct.rect_indicator.bottom = (int)(box->y + box->h);

        if(page)
        {
            runner = page->runner;
            while(runner)
            {
                if((runner->runas & RA_WINDOW) || (runner->runas & RA_ACTIVITY))
                {
                    box = hilayout_element_node_get_used_box_value(runner->node);
                    runner->lx = (int)box->x;
                    runner->ty = (int)box->y;
                    runner->rx = (int)(box->x + box->w);
                    runner->by = (int)(box->y +box->h);

                    bg = hilayout_element_node_get_used_background_value(runner->node);
                    runner->bk_color = bg->color;

                    text = hilayout_element_node_get_used_text_value(runner->node);
                    create_font_name(runner, text);
                    runner->text_color = text->color;
                }
                runner = runner->next;
            }
            page = page->next;
        }

        hilayout_css_destroy(css);

        if(page == NULL)
            quit_layout = 1;
    }
    
    if(css_file)
        free(css_file);

    if(default_file)
        free(default_file);
    return 0;
}

static runner_struct * parseRunner(struct json_object * obj)
{
    int i = 0;
    int length = 0;
    hibus_json *jo_tmp = NULL;
    struct json_object *obj1 = NULL;
    struct array_list *array = NULL;
    const char * string = NULL;
    dep_struct ** dep_temp = NULL;

    runner_struct * runner = malloc(sizeof(runner_struct));
    if(runner == NULL)
        return NULL;

    memset(runner, 0, sizeof(runner_struct));

    // get the name 
    if(json_object_object_get_ex(obj, "name", &jo_tmp) != 0)
    {
        string = json_object_get_string(jo_tmp);
        if(string)
            runner->name = strdup(string);
    }

    // get the type 
    runner->type = RT_EXEC;
    if(json_object_object_get_ex(obj, "type", &jo_tmp) != 0)
    {
        string = json_object_get_string(jo_tmp);
        if(string)
        {
            if(strncasecmp(string, "hijs", strlen(string)) == 0)
                runner->type = RT_HIJS;
            else if(strncasecmp(string, "hiweb", strlen(string)) == 0)
                runner->type = RT_HIWEB;
        }
    }

    // get the entry 
    if(json_object_object_get_ex(obj, "entry", &jo_tmp) != 0)
    {
        string = json_object_get_string(jo_tmp);
        if(string)
            runner->entry = strdup(string);
    }

    // get the class
    if(json_object_object_get_ex(obj, "class", &jo_tmp) != 0)
    {
        string = json_object_get_string(jo_tmp);
        if(string)
            runner->css_class = strdup(string);
    }

    // get css file 
    if(json_object_object_get_ex(obj, "CSSFile", &jo_tmp) != 0)
    {
        string = json_object_get_string(jo_tmp);
        if(string)
            runner->css_file = strdup(string);
    }
    // get the windowBoxStyles
    if(json_object_object_get_ex(obj, "windowBoxStyles", &jo_tmp) != 0)
    {
        string = json_object_get_string(jo_tmp);
        if(string)
            runner->styles = strdup(string);
    }

    // get the intent
    if(json_object_object_get_ex(obj, "intent", &jo_tmp) != 0)
    {
        string = json_object_get_string(jo_tmp);
        if(string)
            runner->intent = strdup(string);
    }

    // get the runas
    if(json_object_object_get_ex(obj, "runas", &jo_tmp) != 0)
    {
        string = json_object_get_string(jo_tmp);
        if(string)
        {
            char tempstring[HISHELL_MAX_PATH];

            if(strncasecmp(string, "window", strlen(string)) >= 0)
            {
                runner->runas &= ~RA_ACTIVITY;
                runner->runas |= RA_WINDOW;
            }
            if(strncasecmp(string, "activity", strlen(string)) >= 0)
            {
                runner->runas &= ~RA_WINDOW;
                runner->runas |= RA_ACTIVITY;
            }
            if(strncasecmp(string, "default", strlen(string)) >= 0)
                runner->runas |= RA_DEFAULT;
            if(strncasecmp(string, "intializer", strlen(string)) >= 0)
                runner->runas |= RA_INTIALIZER;
            if(strncasecmp(string, "daemon", strlen(string)) >= 0)
                runner->runas |= RA_DAEMON;
            if(strncasecmp(string, "program", strlen(string)) >= 0)
                runner->runas |= RA_PROGRAM;
        }
    }

    // get the dependency
    dep_temp = &(runner->dep);
    if(json_object_object_get_ex(obj, "dependencies", &jo_tmp) != 0)
    {
        array = json_object_get_array(jo_tmp);
        for(i = 0; i < array_list_length(array); i++)
        {
            obj1 = array_list_get_idx(array, i);
            if(obj)
            {
                string = json_object_get_string(obj1);
                if(string)
                {
                    dep_struct * dep =  malloc(sizeof(dep_struct));
                    if(dep == NULL)
                        continue;

                    memset(dep, 0, sizeof(dep_struct));
                    dep->dep_name = strdup(string);

                    * dep_temp = dep;
                    dep_temp = &(dep->next);
                }
            }
        } 
    }

    return runner;
}

static page_struct * parsePage(struct json_object * obj)
{
    int i = 0;
    int length = 0;
    hibus_json *jo_tmp = NULL;
    hibus_json *jo_tmp1 = NULL;
    struct json_object *obj1 = NULL;
    struct array_list *array = NULL;
    const char * string = NULL;
    runner_struct ** runner_temp = NULL;
    runner_struct * runner = NULL;

    page_struct * page = malloc(sizeof(page_struct));
    if(page == NULL)
        return NULL;

    memset(page, 0, sizeof(page_struct));

    // get the page id
    if(json_object_object_get_ex(obj, "id", &jo_tmp) != 0)
    {
        string = json_object_get_string(jo_tmp);
        if(string)
            page->id_string = strdup(string);
        page->id = atoi(string);
    }

    // get the css 
    if(json_object_object_get_ex(obj, "windowLayoutStyles", &jo_tmp) != 0)
    {
        string = json_object_get_string(jo_tmp);
        if(string)
            page->styles = strdup(string);
    }

    // get the title 
    if(json_object_object_get_ex(obj, "title", &jo_tmp) != 0)
    {
        if(json_object_object_get_ex(jo_tmp, "en", &jo_tmp1) != 0)
        {
            string = json_object_get_string(jo_tmp1);
            if(string)
                page->title_en = strdup(string);
        }
        if(json_object_object_get_ex(jo_tmp, "zh_CN", &jo_tmp1) != 0)
        {
            string = json_object_get_string(jo_tmp1);
            if(string)
                page->title_zh = strdup(string);
        }
    }

    // get the description 
    if(json_object_object_get_ex(obj, "description", &jo_tmp) != 0)
    {
        if(json_object_object_get_ex(jo_tmp, "en", &jo_tmp1) != 0)
        {
            string = json_object_get_string(jo_tmp1);
            if(string)
                page->description_en = strdup(string);
        }
        if(json_object_object_get_ex(jo_tmp, "zh_CN", &jo_tmp1) != 0)
        {
            string = json_object_get_string(jo_tmp1);
            if(string)
                page->description_zh = strdup(string);
        }
    }

    // get the runners 
    runner_temp = &(page->runner);
    page->runner_number = 0;
    if(json_object_object_get_ex(obj, "runners", &jo_tmp) != 0)
    {
        array = json_object_get_array(jo_tmp);
        for(i = 0; i < array_list_length(array); i++)
        {
            obj1 = array_list_get_idx(array, i);
            if(obj)
            {
                runner = parseRunner(obj1);
                if(runner)
                {
                    * runner_temp = runner;
                    runner_temp = &(runner->next);
                    page->runner_number ++;
                }
            }
        } 
    }
    return page;
}

int parse_manifest(void)
{
    int i = 0;
    int fd = -1;
    struct stat buf;
    int length = 0;
    char * buffer = NULL;

    hibus_json *jo = NULL;
    hibus_json *jo_tmp = NULL;
    struct array_list *array = NULL;
    struct json_object *obj = NULL;
    const char * string = NULL;
    page_struct ** page_temp = NULL;
    page_struct * page = NULL;
    
    char manifest_path[HISHELL_MAX_PATH];

    readlink("/proc/self/exe", manifest_path, HISHELL_MAX_PATH);
    sprintf(manifest_path, "%s/layout/manifest.json", dirname(manifest_path));

    // whether manifest exists
    if((access(manifest_path, F_OK | R_OK)) != 0)
    {
        fprintf(stderr, "The manifest file %s does not exist!\n", manifest_path);
        return ERR_MANIFEST_EXIST;
    }

    // get the length
    if(stat(manifest_path, &buf) < 0)
    {
        fprintf(stderr, "Can not get the length of %s !\n", manifest_path);
        return ERR_MANIFEST_LENGTH;
    }
    if(buf.st_size == 0)
    {
        fprintf(stderr, "The length of %s is 0!\n", manifest_path);
        return ERR_MANIFEST_LENGTH;
    }

    // create buffer for manifest file
    length = buf.st_size;
    buffer = malloc(length + 1);
    if(buffer == NULL)
    {
        fprintf(stderr, "Can not create buffer for manifest file %s!\n", manifest_path);
        return ERR_MANIFEST_MALLOC;
    }

    // load manifest file
    fd = open(manifest_path, O_RDONLY);
    if(fd == -1)
    {
        fprintf(stderr, "Can not open manifest file %s!\n", manifest_path);
        free(buffer);
        return ERR_MANIFEST_OPEN;
    }

    memset(buffer, 0, length + 1);
    length = read(fd, buffer, buf.st_size);
    if(length == 0)
    {
        fprintf(stderr, "Can not read manifest file %s!\n", manifest_path);
        free(buffer);
        close(fd);
        return ERR_MANIFEST_READ;
    }

    close(fd);

    // parse the manifest file
    jo = hibus_json_object_from_string(buffer, strlen(buffer), 10);
    if(jo == NULL)
    {
        if(jo)
            json_object_put (jo);
        free(buffer);
        return ERR_MANIFEST_PARSE;
    }

    // get defaultCSSFile
    if(json_object_object_get_ex(jo, "defaultCSSFile", &jo_tmp) != 0)
    {
        string = json_object_get_string(jo_tmp);

        if(string)
            __os_global_struct.css_file = strdup(string);
    }

    // get direction 
    string = NULL;
    __os_global_struct.direction = SCREEN_ANIMATION_VERTICAL;
    if(json_object_object_get_ex(jo, "direction", &jo_tmp) != 0)
    {
        string = json_object_get_string(jo_tmp);

        if(string && strncasecmp(string, "horizental", strlen(string)) == 0)
            __os_global_struct.direction = SCREEN_ANIMATION_HORIZENTAL;
        else
            __os_global_struct.direction = SCREEN_ANIMATION_VERTICAL;
    }

    // get tileBoxStyle 
    string = NULL;
    if(json_object_object_get_ex(jo, "tileBoxStyle", &jo_tmp) != 0)
    {
        string = json_object_get_string(jo_tmp);
        if(string)
            __os_global_struct.tile_style = strdup(string);
    }

    // get pageBoxStyle
    string = NULL;
    if(json_object_object_get_ex(jo, "pageBoxStyle", &jo_tmp) != 0)
    {
        string = json_object_get_string(jo_tmp);
        if(string)
            __os_global_struct.page_style = strdup(string);
    }

    // get indicatorBoxStyle 
    string = NULL;
    if(json_object_object_get_ex(jo, "indicatorBoxStyle", &jo_tmp) != 0)
    {
        string = json_object_get_string(jo_tmp);
        if(string)
            __os_global_struct.indicator_style = strdup(string);
    }

    // get descriptionBoxStyle 
    string = NULL;
    if(json_object_object_get_ex(jo, "descriptionBoxStyle", &jo_tmp) != 0)
    {
        string = json_object_get_string(jo_tmp);
        if(string)
            __os_global_struct.description_style = strdup(string);
    }

    // get the pages
    page_temp = &(__os_global_struct.page);
    __os_global_struct.page_number = 0;
    if(json_object_object_get_ex(jo, "page", &jo_tmp) != 0)
    {
        array = json_object_get_array(jo_tmp);
        for(i = 0; i < array_list_length(array); i++)
        {
            obj = array_list_get_idx(array, i);
            if(obj)
            {
                page = parsePage(obj);
                if(page)
                {
                    * page_temp = page;
                    page_temp = &(page->next);
                    __os_global_struct.page_number ++;
                }
            }
        } 
    }

    __os_global_struct.current_page = 0;

    if(jo)
        json_object_put (jo);

    free(buffer);

    if(layout_applications())
        return ERR_LAYOUT;

#if 1 
    printf("defaultCSSFile:%s\n", __os_global_struct.css_file);
    printf("tileBoxStyle:%s, %d, %d, %d, %d\n", __os_global_struct.tile_style, \
            __os_global_struct.rect_title.left, __os_global_struct.rect_title.top, \
            __os_global_struct.rect_title.right, __os_global_struct.rect_title.bottom);
    printf("pageBoxStyle:%s\n", __os_global_struct.page_style);
    printf("indicatorBoxStyle:%s, %d, %d, %d, %d\n", __os_global_struct.indicator_style, \
            __os_global_struct.rect_indicator.left, __os_global_struct.rect_indicator.top, \
            __os_global_struct.rect_indicator.right, __os_global_struct.rect_indicator.bottom);
    printf("descriptionBoxStyle:%s, %d, %d, %d, %d\n", __os_global_struct.description_style, \
            __os_global_struct.rect_description.left, __os_global_struct.rect_description.top, \
            __os_global_struct.rect_description.right, __os_global_struct.rect_description.bottom);
    printf("direction: %d\n", (int)__os_global_struct.direction);

    page = __os_global_struct.page;
    int p = 0;
    while(page)
    {
        printf("page%d:\n", p);
        printf("\tid:%s\n", page->id_string);
        printf("\ttitle_en:%s\n", page->title_en);
        printf("\ttitle_zh:%s\n", page->title_zh);
        printf("\tdescription_en:%s\n", page->description_en);
        printf("\tdescription_zh:%s\n", page->description_zh);
        printf("\twindowLayoutStyles:%s\n", page->styles);

        runner_struct * runner = page->runner;
        int r = 0;
        while(runner)
        {
            printf("\trunner%d:\n", r);
            printf("\t\tname:%s\n", runner->name);
            printf("\t\ttype:%d\n", (int)runner->type);
            printf("\t\tentry:%s\n", runner->entry);
            printf("\t\trunas:0x%08x\n", runner->runas);
            printf("\t\tclass:%s\n", runner->css_class);
            printf("\t\twindowBoxStyles:%s\n", runner->styles);
            printf("\t\tintent:%s\n", runner->intent);
            printf("\t\tlx:%d\n", runner->lx);
            printf("\t\tty:%d\n", runner->ty);
            printf("\t\trx:%d\n", runner->rx);
            printf("\t\tby:%d\n", runner->by);

            dep_struct * dep = runner->dep;
            int d = 0;
            printf("\t\tdep\n");
            while(dep)
            {
                printf("\t\t\t%s\n", dep->dep_name);
                
                d ++;
                dep = dep->next;
            }

            r ++;
            runner = runner->next;
        }

        p ++;
        page = page->next;
    }
#endif


    return NO_ERR;
}
