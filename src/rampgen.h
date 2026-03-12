#ifndef RAMPGEN_H
#define RAMPGEN_H

typedef struct {
    CMapClass *ramp;
    float degrees;
    int segments;
    char direction;
} RampGenCmd;

void do_ramp_generator();

#endif // RAMPGEN_H
