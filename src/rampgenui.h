#ifndef RAMPGENUI_H
#define RAMPGENUI_H

#define IDD_RAMPGEN              696
#define IDC_SEGMENTS           50000
#define IDC_SEGMENTS_EDIT      50001
#define IDC_DEGREES            50002
#define IDC_DEGREES_EDIT       50003
#define IDC_SEGMENT_WIDTH      50004
#define IDC_SEGMENT_WIDTH_EDIT 50005
#define IDC_CURVE_LEFT         50006
#define IDC_CURVE_RIGHT        50007
#define IDC_CURVE_UP           50008
#define IDC_CURVE_DOWN         50009
#define IDC_DIRECTION_PLUS     50010
#define IDC_DIRECTION_MINUS    50011
#define IDC_SEGMENT_GAP        50012
#define IDC_SEGMENT_GAP_EDIT   50013

void do_ramp_generator();
void rampgen_close();

#endif // RAMPGENUI_H
