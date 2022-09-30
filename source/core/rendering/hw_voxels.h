#pragma once

#include <stdint.h>
#include "model.h"
#include "model_kvx.h"
#include "intvec.h"

constexpr int MAXVOXELS = 1024;

struct voxmodel_t // : public mdmodel_t
{
    FVoxelModel* model = nullptr;
    float scale, bscale, zadd, yoffset;
    vec3_t siz;
    FVector3 piv;
    int32_t is8bit;
};



extern int16_t tiletovox[];
extern float voxscale[];
extern voxmodel_t* voxmodels[MAXVOXELS];
extern FixedBitArray<MAXVOXELS> voxrotate;

inline int32_t nextvoxid;
inline FixedBitArray<MAXVOXELS>voxreserve;

void voxInit();
void voxClear();
int voxDefine(int voxindex, const char* filename);
