#ifndef RAMPGEN_H
#define RAMPGEN_H

#define IDD_RAMPGEN           696
#define IDC_SEGMENTS        50000
#define IDC_DEGREES         50001
#define IDC_SEGMENT_WIDTH   50002
#define IDC_DIRECTION_LEFT  50003
#define IDC_DIRECTION_RIGHT 50004

typedef struct {
    CMapSolid *ramp;
    float degrees;
    int segments;
    char direction;
    float segment_width;
} RampGenCmd;

void do_ramp_generator();
void rampgen_close();

#endif // RAMPGEN_H
