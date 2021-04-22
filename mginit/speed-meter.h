#ifndef _SPEED_METER_H_
#define _SPEED_METER_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct {
    int m_x;
    int m_y;
    unsigned int m_t;
} record_t;

typedef struct{
    int m_precision_ms;
    int m_size;
    int m_index;
    int m_count;
    record_t *m_records;
} mSpeedMeter_t;

typedef struct tagMoseSpeed
{
    int first;
    int direction;
    BOOL goon;
    int cli;

    DWORD m_timePressed;
    BOOL m_bPressed;
    BOOL m_bMouseMoved;
    unsigned int m_mouseFlag; /* 1: move, 2: cancel animation */ \
    BOOL m_bTimedout;
    POINT m_pressMousePos;
    POINT m_oldMousePos;
    mSpeedMeter_t * m_speedmeter;
} MoseSpeed;

mSpeedMeter_t * mSpeedMeter_create(int duration_ms, int precision_ms);
void mSpeedMeter_reset(mSpeedMeter_t * _handle);
void mSpeedMeter_append(mSpeedMeter_t * _handle, int x, int y, unsigned int t);
void mSpeedMeter_destroy(mSpeedMeter_t * _handle);
void mSpeedMeter_stop(mSpeedMeter_t * _handle);
int mSpeedMeter_velocity(mSpeedMeter_t * _handle, float *v_x, float *v_y);
int mSpeedMeter_query_velocity(mSpeedMeter_t * _handle, float *v_x, float *v_y);


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  // _SPEED_METER_H_
