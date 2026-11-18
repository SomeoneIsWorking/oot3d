#ifndef _Z3DANIMATION_H_
#define _Z3DANIMATION_H_

#include "z3Dvec.hpp"

// A limb's pose relative to its parent, as the animation leaves it (docs/frame.md).
typedef struct SkelJoint {
    /* 0x00 */ MTX34 mtx;
    /* 0x30 */ u32 unk_30;
} SkelJoint; // size = 0x34

// A CMAB material animation's file: the animation's block at `animOffset` from its start.
typedef struct {
    /* 0x00 */ char magic[4]; // "cmab"
    /* 0x04 */ char unk_04[0x10];
    /* 0x14 */ u32 animOffset;
} CmabHeader;

typedef struct {
    /* 0x0 */ s32 unk_0;
    /* 0x4 */ s32 frameCount; // AnimPlayer_Update wraps or holds the frame here
} CmabAnim;

// A material animation playing on a model (docs/frame.md "Material animations"): AnimPlayer_Update
// adds `speed` to `frame`, wraps it at the animation's frame count when `loop` is set and holds it
// at the end otherwise, then applies the frame to each target.
typedef struct AnimPlayer {
    /* 0x00 */ void* model;
    /* 0x04 */ CmabHeader** anim;
    /* 0x08 */ f32 frame;
    /* 0x0C */ f32 speed;
    /* 0x10 */ u8 loop;
    /* 0x11 */ u8 paused; // no step while set
    /* 0x12 */ char unk_12[0x2];
    /* 0x14 */ void* targets[32];
    /* 0x94 */ s32 targetCount;
} AnimPlayer; // size = 0x98

typedef struct SkelAnime {
    /* 0x00 */ void* unk_00;
    /* 0x04 */ struct ZARInfo* zarInfo;
    /* 0x08 */ char unk_08[0x08];
    /* 0x10 */ struct PlayState* play;
    /* 0x14 */ char unk_14[0x14];
    /* 0x28 */ struct SkeletonAnimationModel* model; // what SkelAnime_DrawOpa queues
    /* 0x2C */ char unk_2C[0x4];
    /* 0x30 */ s32 animationType;
    /* 0x34 */ char unk_34[0x8];
    /* 0x3C */ f32 curFrame;
    /* 0x40 */ f32 playSpeed;
    /* 0x44 */ f32 startFrame;
    /* 0x48 */ f32 endFrame;
    /* 0x4C */ f32 animLength;
    /* 0x50 */ char unk_50[0x24];
    /* 0x74 */ u8 limbCount;
    /* 0x75 */ char unk_75[0x03];
    /* 0x78 */ struct SkelJoint* jointTable; // limbCount joints, posed by SkelAnime_Update
    /* 0x7C */ struct SkelJoint* morphTable;
    /* 0x80 */ char unk_80[0x02];
    /* 0x82 */ u8 tablesAllocated;
    /* 0x83 */ char unk_83[0x01];
} SkelAnime; // size = 0x84

class FaceAnimations {
public:
    void Destroy();
    void FUN_0035e3a4(s32 arg1, s32 arg2);
    void FUN_0035e330();

    s8 unk_00;
    s8 unk_01;
    s8 unk_02;
    SkeletonAnimationModel_unk_0C unk_04[3];
};

// Some animation related structure
// Temporary name just taken from OoT
typedef struct {
    /* 0x00 */ s32              animationIndex;
    /* 0x04 */ f32              playbackSpeed;
    /* 0x08 */ f32              startFrame;
    /* 0x0C */ f32              frameCount;
    /* 0x10 */ u8               mode;
    /* 0x14 */ f32              transitionRate;
} struct_80034EC0_Entry; // size = 0x18

typedef s32 (*OverrideLimbDrawOpa)(struct PlayState* play, s32 limbIndex, MTX34* mtx, void*);

typedef void (*PostLimbDrawOpa)(struct PlayState* play, s32 limbIndex, MTX34* mtx, void*);

#endif
