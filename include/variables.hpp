#ifndef _VARIABLES_H_
#define _VARIABLES_H_

#include "z3D.hpp"

extern SaveContext gSaveContext; //0x00587958

extern f32 FLOAT_0054ac20;

struct DAT_0054ac24_struct {
    s8 unk_0;
    s8 unk_1;
    s16 unk_2;
};

extern DAT_0054ac24_struct DAT_0054ac24;

extern u32 gBitFlags[32];
extern u16 gEquipMasks[4];
extern u16 gEquipNegMasks[4];
extern u32 gUpgradeMasks[8]; //0x0053cbc4
extern u32 gUpgradeNegMasks[8];
extern u8 gEquipShifts[4];
extern u8 gUpgradeShifts[8]; //0x0053cb1c
extern u16 gUpgradeCapacities[8][4]; //0x0053cbe4
extern u32 gGsFlagsMask[4];
extern u32 gGsFlagsShift[4];
extern u8 gItemSlots[56];

extern CameraSetting sCameraSettings[78]; //0x00516ff0

extern Hud gHud; //0x0050af34
extern TopHud gTopHud; //0x004fc648
extern Minimap gMinimap; //0x004fda6c
extern void* gMenuTextures[26]; //0x0055b490
extern MessageManager gMessageManager; //0x005c1878

extern GameInfo* gGameInfo;

#endif
