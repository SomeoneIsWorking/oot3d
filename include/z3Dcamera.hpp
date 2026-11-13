#ifndef _Z3DCAMERA_H_
#define _Z3DCAMERA_H_

#include "z3Dvec.hpp"

#define CAM_STAT_CUT        0
#define CAM_STAT_WAIT       1
#define CAM_STAT_UNK3       3
#define CAM_STAT_ACTIVE     7
#define CAM_STAT_UNK100     0x100

#define NUM_CAMS 4
#define MAIN_CAM 0
#define SUBCAM_FIRST 1
#define SUBCAM_FREE 0
#define SUBCAM_NONE -1
#define SUBCAM_ACTIVE -1

typedef struct Camera {
    /* 0x000 */ char unk_0[0x080];
    /* 0x080 */ Vec3f at;
    /* 0x08C */ Vec3f eye;
    /* 0x098 */ Vec3f up;
    /* 0x0A4 */ Vec3f eyeNext;
    /* 0x0B0 */ Vec3f skyboxOffset;
    /* 0x0BC */ char unk_BC[0x018];
    /* 0x0D4 */ struct PlayState* play;
    /* 0x0D8 */ void* player; // Player*
    /* 0x0DC */ char playerPosRot[0x14]; // PosRot: Link's world position and rotation
    /* 0x0F0 */ char unk_F0[0x034];
    /* 0x124 */ f32 dist;
    /* 0x128 */ char unk_128[0x01C];
    /* 0x144 */ f32 fov;
    /* 0x148 */ char unk_148[0x034];
    /* 0x17C */ Vec3s inputDir;
    /* 0x182 */ Vec3s camDir;
    /* 0x188 */ s16 status;
    /* 0x18A */ s16 setting;
    /* 0x18C */ s16 mode;
    /* 0x18E */ char unk_18E[0x002];
    /* 0x190 */ s16 camDataIdx;
    /* 0x192 */ char unk_192[0x00A];
    /* 0x19C */ s16 prevSetting;
    /* 0x19E */ char unk_19E[0x01E];
} Camera; // size = 0x1BC

#endif
