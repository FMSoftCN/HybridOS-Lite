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
** speed-meter.c: It illustrates how to calculate mouse speed.
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
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <mgeff/mgeff.h>
#include <glib.h>
#include <hibus.h>

#include "../include/sysconfig.h"
#include "config.h"
#include "speed-meter.h"

static int s_mod(int a, int b) 
{
    assert(b > 0);
    while (a < 0) {
        a += b;
    }
    while (a > b) {
        a -= b;
    }
    assert(a >= 0 && a < b);
    return a;
}

mSpeedMeter_t * mSpeedMeter_create(int duration_ms, int precision_ms)
{
    mSpeedMeter_t * handle = NULL;

    handle = (mSpeedMeter_t *)calloc(1, sizeof(*handle));

    handle->m_precision_ms = precision_ms;
    assert(handle->m_precision_ms > 0);

    handle->m_size = duration_ms / precision_ms;
    assert(handle->m_size > 0);

    handle->m_records = (record_t *)calloc(handle->m_size, sizeof(handle->m_records[0]));

    return handle;
}

void mSpeedMeter_reset(mSpeedMeter_t * _handle)
{
    mSpeedMeter_t *handle = (mSpeedMeter_t *)_handle;
    assert(handle);
    handle->m_index = 0;
    handle->m_count = 0;
}

void mSpeedMeter_append(mSpeedMeter_t * _handle, int x, int y, unsigned int t)
{
    mSpeedMeter_t *handle = (mSpeedMeter_t *)_handle;
    int old_index = s_mod(handle->m_index - 1, handle->m_size);
    assert(handle->m_size > 0);
    assert(handle);

    if(handle->m_count > 0 && handle->m_records[old_index].m_t == t) 
    {
        handle->m_records[old_index].m_x = x;
        handle->m_records[old_index].m_y = y;
    }
    else
    {
        handle->m_records[handle->m_index].m_x = x;
        handle->m_records[handle->m_index].m_y = y;
        handle->m_records[handle->m_index].m_t = t;

        if(++handle->m_index >= handle->m_size) 
            handle->m_index = 0;
        if(handle->m_count < handle->m_size) 
        {
            assert(handle->m_index < handle->m_size);
            ++ handle->m_count;
        }
    }
}

void mSpeedMeter_destroy(mSpeedMeter_t * _handle)
{
    mSpeedMeter_t *handle = (mSpeedMeter_t *)_handle;
    assert(handle);

    if(handle->m_records) 
        free(handle->m_records);
    free(handle);
}

void mSpeedMeter_stop(mSpeedMeter_t * _handle)
{
    mSpeedMeter_t *handle = (mSpeedMeter_t *)_handle;
    assert(handle);
}

int mSpeedMeter_velocity(mSpeedMeter_t * _handle, float *v_x, float *v_y)
{
    mSpeedMeter_t *handle = (mSpeedMeter_t *)_handle;
    int step = 0;
    int I = 0;
    int i = 0;
    float WX = 0.0;
    float WY = 0.0;
    float W = 0.0;

    if(!handle) 
    {
        *v_x = *v_y = 0.0f;
        return -1;
    }

    if(handle->m_count <= 1) 
    {
        *v_x = 0.0f;
        *v_y = 0.0f;
        return -1;
    }

    WX = WY = W = 0;
    step = handle->m_count / 2;
    I = (handle->m_count / 2) + (handle->m_count % 2);

    for(i = 0; i < I; ++i) 
    {
        int i1, i2;
        unsigned int dT;
        float vx, vy, w;

        i2 = s_mod(handle->m_index - 1 - i, handle->m_size);
        i1 = s_mod(i2 - step, handle->m_size);

        dT = handle->m_records[i2].m_t - handle->m_records[i1].m_t;
        if(dT <= 0) 
        {
            fprintf(stderr, "[WARNING speedmeter]: step=%d, I=%d, t(%d)=%u t(%d)=%u\n",
                    step, I,
                    i1, handle->m_records[i1].m_t,
                    i2, handle->m_records[i2].m_t);
            *v_x = 0.0f;
            *v_y = 0.0f;
            return -1;
        }
        vx = 1.0f * (handle->m_records[i2].m_x - handle->m_records[i1].m_x) / dT;
        vy = 1.0f * (handle->m_records[i2].m_y - handle->m_records[i1].m_y) / dT;
        w = 1.0f; /* TODO */

        WX += vx * w;
        WY += vy * w;
        W += w;
    }
    *v_x = WX / W;
    *v_y = WY / W;

    return 0;
}

int mSpeedMeter_query_velocity(mSpeedMeter_t * _handle, float *v_x, float *v_y)
{
    float f_x = 0.0;
    float f_y = 0.0;

    if(mSpeedMeter_velocity (_handle, &f_x, &f_y) == 0) 
    {
        *v_x = 1000 * f_x;
        *v_y = 1000 * f_y;
        return 0;
    }

    return -1;
}
