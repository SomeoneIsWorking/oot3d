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

#define CAM_MODE_NORMAL 0
#define CAM_MODE_Z_PARALLEL 1

// Indices into sCameraFunctions, in the N64 order.
#define CAM_FUNC_NORM1 2
#define CAM_FUNC_NORM3 4
#define CAM_FUNC_JUMP1 22
#define CAM_FUNC_SPEC9 0x46 // Camera_Special9, the room door camera

// Camera_Special9's animState when it has moved the eye to the door's far side.
#define CAM_SPEC9_ANIM_FAR_SIDE 2

// Camera_Update runs sCameraFunctions[sCameraSettings[setting & 0xFF].cameraModes[mode & 0xFF].funcIdx].
typedef struct {
    /* 0x00 */ s16 funcIdx;
    /* 0x02 */ s16 valueCnt;
    /* 0x04 */ void* values;
} CameraMode; // size = 0x8

typedef struct {
    /* 0x00 */ u32 validModes; // bit n: cameraModes[n] exists
    /* 0x04 */ CameraMode* cameraModes;
} CameraSetting; // size = 0x8

// Camera_BGCheckInfo(camera, from, colChk) line-tests from `from` to pos; on a hit it moves pos
// in front of the wall and returns bgId + 1, otherwise 0.
typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f norm;
    /* 0x18 */ void* poly;
    /* 0x1C */ char unk_1C[0x08];
    /* 0x24 */ s32 bgId;
} CamColChk; // size = 0x28

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
    /* 0x192 */ char unk_192[0x002];
    /* 0x194 */ s16 stateFlags; // the N64's, at its offset + 0x48; Camera_Special9 clears 0x2 and 0x4
    /* 0x196 */ char unk_196[0x006];
    /* 0x19C */ s16 prevSetting;
    /* 0x19E */ char unk_19E[0x008];
    /* 0x1A6 */ s16 animState; // the N64's, at its offset + 0x48; Camera_Special9 steps it
    /* 0x1A8 */ char unk_1A8[0x014];
} Camera; // size = 0x1BC

#endif
