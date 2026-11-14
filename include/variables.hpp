#ifndef _VARIABLES_H_
#define _VARIABLES_H_

#include "z3D.hpp"

extern SaveContext gSaveContext; //0x00587958
extern MotionState gMotionState; //0x0055B240

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
// The items and gear screens' state words: MENU_SCREEN_CLOSED, or MENU_SCREEN_UP while theirs is up.
extern s32 gItemMenuState; //0x0050672c
extern s32 gItemMenuCursorNode; //0x00506760
extern s32 gGearMenuState; //0x00504484
#define MENU_SCREEN_CLOSED 0
#define MENU_SCREEN_UP 2
// The ocarina screen (docs/ocarina.md): its state, and the entry of its song list on show, -1 for none.
extern s32 gOcarinaScreenState; //0x005093f8
#define OCARINA_SCREEN_CLOSED 0
#define OCARINA_SCREEN_SONG_LIST 4
#define OCARINA_SCREEN_PLAY 12
extern s32 gOcarinaSongListShown; //0x00509404
// The song list's entries in grid order, four to a row: each an ITEM_SONG_*.
extern u32 gOcarinaSongListItems[12]; //0x0050a3b0
// A song's notes: a count, then that many of the five notes' numbers, low to high.
extern u8 gOcarinaSongButtons[14][9]; //0x0054c222
#define OCARINA_SONG_BUTTONS_SIZE 9
// 1 while the audio's ocarina takes notes.
extern u8 gAudioOcarinaInputEnabled; //0x0054ac5c
extern TopHud gTopHud; //0x004fc648
extern Minimap gMinimap; //0x004fda6c
extern void* gMenuTextures[26]; //0x0055b490
extern MessageManager gMessageManager; //0x005c1878

extern GameInfo* gGameInfo;

#endif
