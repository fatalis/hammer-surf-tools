#ifndef RAMPGEN_H
#define RAMPGEN_H

#include "hammerfuncs.h"

// #define RAMPGEN_DEBUG
// #define CONVEX_DEBUG

typedef enum {
    SW,
    SE,
    NE,
    NW
} BoxCorner;

typedef enum {
    DIR_MINUS = 0,
    DIR_PLUS = 1
} AppendDirection;

typedef struct {
    // ui
    CMapSolid *ramp;
    float ui_degrees;
    int ui_segments;
    char curve;
    AppendDirection direction;
    float segment_width;
    float segment_gap;

    // overidden from ui
    float degrees;
    int segments;

    // ramp data
    Angle rotate_angle;
    Axis axis;
    FaceOrientation orientation; // surfing direction
    bool convex;
    int sign; // dir * facing * axis_sign
    bool flip_edge; // whether to swap pivot/pivot_end when building the cut plane
    BoxCorner pivot;
    BoxCorner pivot_end;
    BoxCorner pivot_opposite;
    BoxCorner pivot_opposite_end;

    CMapSolid **segment_list;
} RampGenState;

void rampgen(RampGenState *ramp);

#endif // RAMPGEN_H
