#ifndef _Z3D_H_
#define _Z3D_H_

#include "z3Dactor.hpp"
#include "z3Dvec.hpp"
#include "z3Dcutscene.hpp"
#include "z3Ditem.hpp"
#include "z3Dobject.hpp"
#include "z3Danimation.hpp"
#include "z3Dcamera.hpp"
#include "z3Dcollision_check.hpp"
#include "z3Dbgcheck.hpp"
#include "z3Dsave.hpp"
#include "z3Dscene.hpp"
#include "color.hpp"
#include "math.hpp"
#include "ichain.hpp"
#include "stddef.hpp"
#include "regs.hpp"

typedef struct GraphicsContext GraphicsContext; //TODO

typedef struct {
    /* 0x0000 */ u32 size;
    /* 0x0004 */ void* bufp;
    /* 0x0008 */ void* head;
    /* 0x000C */ void* tail;
} TwoHeadArena; // size = 0x10

typedef struct {
    /* 0x000 */ s32    topyY;
    /* 0x004 */ s32    bottomY;
    /* 0x008 */ s32    leftX;
    /* 0x00C */ s32    rightX;
    /* 0x010 */ f32    fovy;  // vertical field of view in degrees
    /* 0x014 */ f32    zNear; // distance to near clipping plane
    /* 0x018 */ f32    zFar;  // distance to far clipping plane
    /* 0x01C */ char   unk_1C[0x014];
    /* 0x030 */ Vec3f  eye;
    /* 0x03C */ Vec3f  lookAt;
    /* 0x048 */ Vec3f  up;
    /* 0x054 */ char   unk_54[0x150];
    /* 0x1A4 */ Vec3f  unk_1A4;
    /* 0x1B0 */ Vec3f  unk_1B0;
    /* 0x1BC */ Vec3f  unk_1BC;
    /* 0x1C8 */ Vec3f  unk_1C8;
    /* 0x1D4 */ f32    unk_1D4;
    /* 0x1D8 */ s8     flags;
    /* 0x1D9 */ char   unk_1D9[0x003];
} View; // size = 0x1DC

typedef struct {
    /* 0x00 */ s32 enabled;
    /* 0x04 */ s32 timer;
} FrameAdvanceContext; // size = 0x8

typedef struct {
    /* 0x00 */ u8*  texture;
    /* 0x04 */ s16  unk_04;
    /* 0x06 */ s16  unk_06;
    /* 0x08 */ s16  unk_08;
    /* 0x0A */ s16  unk_0A;
    /* 0x0C */ s32  unk_0C;
    /* 0x10 */ u8   delayA;
    /* 0x11 */ u8   delayB;
    /* 0x12 */ s16  unk_12;
    /* 0x14 */ s16  unk_14;
    /* 0x16 */ s16  unk_16;
} TitleCardContext; // size = 0x18

typedef struct {
    /* 0x00 */ u32    length; // number of actors loaded of this type
    /* 0x04 */ Actor* head;  // pointer to first actor of this type
} ActorListEntry; // size = 0x08

typedef struct {
    /* 0x0000 */ u8     unk_00;
    /* 0x0001 */ char   unk_01[0x01];
    /* 0x0002 */ u8     unk_02;
    /* 0x0003 */ u8     unk_03;
    /* 0x0004 */ char   unk_04[0x04];
    /* 0x0008 */ u8     total; // total number of actors loaded
    /* 0x0009 */ char   unk_09[0x03];
    /* 0x000C */ ActorListEntry actorLists[12];
    // /* 0x006C */ TargetContext targetCtx;
    /* 0x006C */ char   unk_6C[0x130];
    struct {
        /* 0x019C */ u32    swch;
        /* 0x01A0 */ u32    tempSwch;
        /* 0x01A4 */ u32    unk0;
        /* 0x01A8 */ u32    unk1;
        /* 0x01AC */ u32    chest;
        /* 0x01B0 */ u32    clear;
        /* 0x01B4 */ u32    tempClear;
        /* 0x01B8 */ u32    collect;
        /* 0x01BC */ u32    tempCollect;
    }                   flags;
    /* 0x01C0 */ TitleCardContext titleCtx;
} ActorContext; // TODO: size = 0x1D8

typedef struct CutsceneContext {
    /* 0x00 */ char  unk_00[0x4];
    /* 0x04 */ void* segment;
    /* 0x08 */ u8    state;
    /* 0x09 */ char  unk_09[0x0B];
    /* 0x14 */ s32   unk_14;
    /* 0x18 */ s32   endFrame; // copied from the script's end frame each frame
    /* 0x1C */ f32   unk_1C;
    /* 0x20 */ u16   frames;
    /* 0x22 */ u16   unk_22;
    /* 0x24 */ s32   unk_24;
    /* 0x28 */ u16   unk_28;
    /* 0x2A */ u16   unk_2A;
    /* 0x2C */ u16   unk_2C;
    /* 0x2E */ u16   unk_2E;
    /* 0x30 */ char  unk_30[0x04];
    /* 0x34 */ s8    unk_34;
    /* 0x35 */ s8    unk_35;
    /* 0x36 */ char  unk_36[0x0A];
    /* 0x40 */ CsCmdActorAction* linkAction;
    /* 0x44 */ CsCmdActorAction* npcActions[10]; // "npcdemopnt"
} CutsceneContext; // size = 0x6C

typedef struct OcLine OcLine; //TODO
#define COLLISION_CHECK_AT_MAX 50
#define COLLISION_CHECK_AC_MAX 60
#define COLLISION_CHECK_OC_MAX 50
#define COLLISION_CHECK_OC_LINE_MAX 3

typedef struct {
    /* 0x000 */ s16 colAtCount;
    /* 0x002 */ u16 sacFlags;
    /* 0x004 */ Collider* colAt[COLLISION_CHECK_AT_MAX];
    /* 0x0CC */ s32 colAcCount;
    /* 0x0D0 */ Collider* colAc[COLLISION_CHECK_AC_MAX];
    /* 0x1C0 */ s32 colOcCount;
    /* 0x1C4 */ Collider* colOc[COLLISION_CHECK_OC_MAX];
    /* 0x28C */ s32 colOcLineCount;
    /* 0x290 */ OcLine* colOcLine[COLLISION_CHECK_OC_LINE_MAX];
} CollisionCheckContext; // size = 0x29C

struct ZARFileTypeEntry {
    /* 0x00 */ u32 numFiles;
    /* 0x04 */ u32 filesListOffset;
    /* 0x08 */ u32 typeNameOffset;
    /* 0x0C */ u32 unk_0C;
};

struct ZARInfo {
    void* FUN_003532c0(s32 index);
    void* GetQDBByIndex(s32 index);
    void* GetCMABByIndex(s32 index);

    /* 0x00 */ void* buf;
    /* 0x04 */ char unk_04[0x08];
    /* 0x0C */ ZARFileTypeEntry* fileTypes;
    /* 0x10 */ char unk_10[0xC];
    /* 0x1C */ s32 filetypeMap[11];
    /* 0x48 */ char unk_48[0x04];
    /* 0x4C */ void*** cmbPtrs;  /* Really, this is a pointer to an array of pointers to CMB managers,
                                    the first member of which is a pointer to the CMB data */
    /* 0x50 */ void*** csabPtrs; /* Same as above but for CSAB */
    /* 0x54 */ char unk_54[0x04];
    /* 0x58 */ void*** cmabPtrs; /* Same as above but for CMAB */
    /* 0x5C */ void*** unk_5C;
    /* 0x60 */ void*** qdbPtrs;
    /* 0x64 */ char unk_64[0x0C];
}; // size = 0x70

typedef struct {
    /* 0x00 */ s16 id;
    /* 0x02 */ char unk_02[0x06];
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ char unk_0C[0x4];
    /* 0x10 */ ZARInfo zarInfo;
} ObjectStatus; // size = 0x80

typedef struct {
    /* 0x000 */ u8 num;
    /* 0x001 */ char unk_01[0x3];
    /* 0x004 */ ObjectStatus status[OBJECT_EXCHANGE_BANK_MAX];
} ObjectContext; // size = 0x984

typedef struct {
    /* 0x00 */ char filename[0x40];
    /* 0x40 */ u32 size;
} ObjectFile;

typedef struct {
    /* 0x00 */ s16   id;
    /* 0x02 */ Vec3s pos;
    /* 0x08 */ Vec3s rot;
    /* 0x0E */ s16   params;
} ActorEntry; // size = 0x10

typedef struct GameAllocEntry {
    /* 0x00 */ struct GameAllocEntry* next;
    /* 0x04 */ struct GameAllocEntry* prev;
    /* 0x08 */ u32 size;
    /* 0x0C */ u32 unk_0C;
} GameAllocEntry; // size = 0x10

typedef struct {
    /* 0x00 */ GameAllocEntry base;
    /* 0x10 */ GameAllocEntry* head;
} GameAlloc; // size = 0x14

// GameState.buttonsHeld bit 12: no button of the console; Azahar's Debug button sets it.
#define PAD_DEBUG (1 << 12)

// The A button's label (InterfaceContext.doAction): the N64's numbers, the word cells of hud_all00.ctxb
// in column order (docs/hud.md). CHECK, RETURN, DROP, SPEAK, NEXT and NONE were seen live.
typedef enum {
    /* 0x00 */ DO_ACTION_ATTACK,
    /* 0x01 */ DO_ACTION_CHECK,
    /* 0x02 */ DO_ACTION_ENTER,
    /* 0x03 */ DO_ACTION_RETURN,
    /* 0x04 */ DO_ACTION_OPEN,
    /* 0x05 */ DO_ACTION_JUMP,
    /* 0x06 */ DO_ACTION_DECIDE,
    /* 0x07 */ DO_ACTION_DIVE,
    /* 0x08 */ DO_ACTION_FASTER,
    /* 0x09 */ DO_ACTION_THROW,
    /* 0x0A */ DO_ACTION_NONE, // no label; the sheet's cell is "Navi"
    /* 0x0B */ DO_ACTION_CLIMB,
    /* 0x0C */ DO_ACTION_DROP,
    /* 0x0D */ DO_ACTION_DOWN,
    /* 0x0E */ DO_ACTION_SAVE,
    /* 0x0F */ DO_ACTION_SPEAK,
    /* 0x10 */ DO_ACTION_NEXT,
    /* 0x11 */ DO_ACTION_GRAB,
    /* 0x12 */ DO_ACTION_STOP,
    /* 0x13 */ DO_ACTION_PUTAWAY,
    /* 0x14 */ DO_ACTION_REEL
} DoAction;
// The sheet's word cells: Swap and Equip follow Reel.
#define DO_ACTION_WORDS 23

typedef struct InterfaceContext {
    /* 0x000 */ char unk_000[0x226];
    /* 0x226 */ u16 doAction; // DO_ACTION_*: TopHud_Update shows its label on the A button
    /* 0x228 */ u16 unk_228; // equal to doAction when read
    /* 0x22A */ char unk_22A[0x5A];
    /* 0x284 */ u16 buttonAlphas[4]; // 0xFF with the item buttons enabled, 0x46 disabled, 0 in a text box or cutscene
    /* 0x28C */ u16 topHudAlpha; // TopHud_Update hides the top screen's HUD at 0: 0xFF in play and text, 0 in a cutscene
    /* 0x28E */ char unk_28E[0x04];
} InterfaceContext; // size = 0x292

// A menu texture's slot in gMenuTextures (the order of the name table at 0x4D4010).
#define MENU_TEXTURE_HUD 1 // hud_all00.ctxb
#define MENU_TEXTURE_TOP_PARTS 2 // menu_top_parts00.ctxb
#define MENU_TEXTURE_ITEM_ICONS 10 // icon_item_menu00.ctxb
// icon_item_menu00.ctxb: item id n is the cell at column n % 12, row n / 12 (HudItemButtons_SetIcon).
#define ITEM_ICON_SIZE 42
#define ITEM_ICON_COLUMNS 12

// The top screen's HUD (gTopHud): the A button's label, the map markers and the minimap.
typedef struct TopHud {
    /* 0x00 */ u8 drawNode78; // TopHud_Draw draws node78 while not 0
    /* 0x01 */ char unk_01[0x17];
    /* 0x18 */ void* quads; // the label's two quads, then the map's player and entrance markers
    /* 0x1C */ char unk_1C[0x30];
    /* 0x4C */ s32 doAction; // the label shown: InterfaceContext.doAction, -1 once DO_ACTION_NONE has gone
    /* 0x50 */ s32 prevDoAction;
    /* 0x54 */ char unk_54[0x04];
    /* 0x58 */ s32 hidden; // TopHud_Draw draws nothing while not 0
    /* 0x5C */ char unk_5C[0x0C];
    /* 0x68 */ s32 minimapOn; // the player's switch (TopHud_ToggleMinimap); the map and its markers draw while 1
    /* 0x6C */ char unk_6C[0x0C];
    /* 0x78 */ void* node78;
} TopHud;

// The minimap (gMinimap).
typedef struct Minimap {
    /* 0x00 */ char unk_00[0x18];
    /* 0x18 */ void* layout; // NULL in a scene without a map
    /* 0x1C */ char unk_1C[0x14];
    /* 0x30 */ s32 sceneNum;
} Minimap;

// The message manager (gMessageManager), the object Message_GetState reads.
typedef struct MessageManager {
    /* 0x000 */ char unk_000[0xF38];
    /* 0xF38 */ u8 boxMode; // 0 with no text box up
} MessageManager;

// The gameplay HUD (gHud).
typedef struct Hud {
    /* 0x00 */ struct PlayState* play;
    /* 0x04 */ void* layout; // the touch screen HUD's quads: 59 to 62 the magic meter, 63 on the hearts
    /* 0x08 */ char unk_08[0x0C];
    /* 0x14 */ void* itemButtons; // the layout Hud_SetItemButton draws an item's icon into
    /* 0x18 */ char unk_18[0x1C];
    /* 0x34 */ u32 state; // HUD_STATE_*
} Hud;

#define HUD_STATE_COVERED 0 // the items, gear, map or system screen is up
#define HUD_STATE_GAMEPLAY 2

struct GameState;

typedef void (*GameStateFunc)(struct GameState* gameState);

typedef struct GameState {
    /* 0x000 */ GraphicsContext* gfxCtx;
    /* 0x004 */ GameStateFunc main;
    /* 0x008 */ GameStateFunc destroy; // "cleanup"
    /* 0x00C */ GameStateFunc init;
    /* 0x010 */ u32 size;
    /* 0x014 */ u32 buttonsHeld; // the HID pad word: bit 3 is Start, bits 12 and 13 no button of the console
    /* 0x018 */ u32 buttonsPressed; // pressed this frame
    /* 0x01C */ char unk_1C[0xB8];
    /* 0x0D4 */ TwoHeadArena tha;
    /* 0x0E4 */ GameAlloc alloc;
    /* 0x0F8 */ u32 frames;
    /* 0x0FC */ char unk_FC[0x5];
    /* 0x101 */ u8 running;
    /* 0x102 */ char unk_102[0x2];
} GameState; // size = 0x104

typedef struct {
    /* 0x000 */ GameState state;
    /* 0x104 */ char unk_104[0x1F0];
    /* 0x2F4 */ u8 exit;
    /* 0x2F5 */ char unk_2F5[0x3];
} TitleContext; // size = 0x2F8

typedef struct {
    /* 0x00 */ char unk_00[0x40];
} subPlayState_5FCC; // size at least 0x40

typedef struct {
    /* 0x000 */ char unk_00[0x7C0];
    /* 0x7C0 */ u16 unk_7C0;
    /* 0x7C2 */ char unk_7C2[0x796];
} RoomContext; // size 0xF58

typedef struct {
    /* 0x000 */ char unk_000[0x2E0]; // Likely tons left
} MessageContext;

// Play State (ram start: 0871E840)
typedef struct PlayState {
    /* 0x0000 */ GameState state;
    /* 0x0104 */ s16 sceneNum;
    /* 0x0106 */ u8 sceneConfig;
    /* 0x0107 */ char unk_107[0x0009];
    /* 0x0110 */ void* sceneSegment;
    /* 0x0114 */ void* sceneZAR;
    /* 0x0118 */ ZARInfo sceneZARInfo;
    /* 0x0188 */ View view;
    /* 0x0364 */ Camera mainCamera;
    /* 0x0520 */ Camera subCameras[3];
    /* 0x0A54 */ Camera*               cameraPtrs[4];
    /* 0x0A64 */ s16                   activeCamera;
    /* 0x0A66 */ char                  unk_A66[0x002A];
    /* 0x0A90 */ FrameAdvanceContext   frameAdvCtx;
    /* 0x0A98 */ CollisionContext      colCtx;
    /* 0x208C */ ActorContext          actorCtx;
    /* 0x2264 */ char                  unk_2264[0x0034];
    /* 0x2298 */ CutsceneContext       csCtx; // "demo_play"
    /* 0x2304 */ char                  unk_2304[0x059C];
    /* 0x28A0 */ MessageContext        msgCtx;
    /* 0x2B80 */ char                  unk_2B80[0x0024];
    /* 0x2BA4 */ InterfaceContext      interfaceCtx;
    /* 0x2E36 */ char                  unk_2E36[0x07CA];
    /* 0x3600 */ f32                   unk_3600;
    /* 0x3604 */ char                  unk_3604[0x0454];
    /* 0x3A58 */ ObjectContext         objectCtx;
    /* 0x43DC */ char                  unk_43DC[0x0854];
    /* 0x4C30 */ RoomContext           roomCtx;
    /* 0x5B88 */ char                  unk_5B88[0x0078];
    /* 0x5C00 */ u8                    linkAgeOnLoad;
    /* 0x5C01 */ char                  unk_5C01[0x001F];
    /* 0x5C20 */ Path*                 setupPathList;
    /* 0x5C24 */ char                  unk_5C24[0x0009];
    /* 0x5C2D */ s8                    sceneLoadFlag; // "fade_direction"
    /* 0x5C2E */ char                  unk_5C2E[0x0004];
    /* 0x5C32 */ s16                   nextEntranceIndex;
    /* 0x5C34 */ char                  unk_5C34[0x0042];
    /* 0x5C76 */ u8                    fadeOutTransition;
    /* 0x5C78 */ CollisionCheckContext colChkCtx;
    /* 0x5F14 */ char                  unk_5F14[0x0084];
    /* 0x5F98 */ u16                   envFlags[20];
    /* 0x5FC0 */ char                  unk_5FC0[12];
    /* 0x5FCC */ subPlayState_5FCC unk_5FCC;
    //TODO

    ObjectStatus* GetObjectStatus(u8 objBankIdx) {
        if ((objBankIdx < OBJECT_EXCHANGE_BANK_MAX) && (objectCtx.status[objBankIdx].unk_08 != 0)) {
            return &objectCtx.status[objBankIdx];
        } else {
            return NULL;
        }
    }

    void FUN_0037573c(void* csSegment);
    u8 GetCutsceneState();
    void SetCutsceneState(u8 csState);
} PlayState; // size = 0x5F14 TODO

extern PlayState* gPlayState; //0x0050AF34

typedef struct StaticContext {
    /* 0x0000 */ char unk_0[0x0E72];
    /* 0x0E72 */ u16 collisionDisplay;
    /* 0x0E74 */ char unk_E74[0x015C];
    /* 0x0FD0 */ u16 renderGeometryDisable;
    /* 0x0FD2 */ char unk_FD2[0x0602];
} StaticContext; //size 0x15D4
// _Static_assert(sizeof(StaticContext) == 0x15D4, "Static Context size");

typedef enum {
    DPM_UNK = 0,
    DPM_PLAYER = 1,
    DPM_ENEMY = 2,
    DPM_UNK3 = 3
} DynaPolyMoveFlag;

typedef struct {
    /* 0x00 */ s8  scene;
    /* 0x01 */ s8  spawn;
    /* 0x02 */ u16 field;
} EntranceInfo; // size = 0x4

typedef struct {
    /* 0x00 */ char infoFilename[0x44];
    /* 0x44 */ char filename[0x44];
    /* 0x88 */ char unk_88[0x01];
    /* 0x89 */ u8   config;
    /* 0x8A */ char unk_8A[0x02];
} Scene; // size = 0x8C

typedef struct {
    /* 0x00 */ s16 objectId;
    /* 0x02 */ u8 objectModelIdx;
    /* 0x03 */ char unk_03[0x3];
} DrawItemTableEntry;

typedef struct {
    /* 0x00 */ u8 scene;
    /* 0x01 */ u8 flags1;
    /* 0x02 */ u8 flags2;
    /* 0x03 */ u8 flags3;
} RestrictionFlags;

// Game Info aka. Static Context
// Data normally accessed through REG macros (see regs.h)
typedef struct {
    /* 0x00 */ s32  regPage;   // 1 is first page
    /* 0x04 */ s32  regGroup;  // "register" group (R, RS, RO, RP etc.)
    /* 0x08 */ s32  regCur;    // selected register within page
    /* 0x0C */ s32  dpadLast;
    /* 0x10 */ s32  repeat;
    /* 0x14 */ s16  data[REG_GROUPS * REG_PER_GROUP]; // 0xAE0 entries
} GameInfo; // size = 0x15D4

extern GameInfo* gGameInfo; //0x0051B2F4

typedef enum {
    /*  0 */ TEXT_STATE_NONE,
    /*  1 */ TEXT_STATE_DONE_HAS_NEXT,
    /*  2 */ TEXT_STATE_CLOSING,
    /*  3 */ TEXT_STATE_DONE_FADING,
    /*  4 */ TEXT_STATE_CHOICE,
    /*  5 */ TEXT_STATE_EVENT,
    /*  6 */ TEXT_STATE_DONE,
    /*  7 */ TEXT_STATE_SONG_DEMO_DONE, 
    /*  8 */ TEXT_STATE_8, 
    /*  9 */ TEXT_STATE_9, 
    /* 10 */ TEXT_STATE_AWAITING_NEXT
} TextState;

// The game's view of the gyroscope (docs/motion.md). Angles are degrees since the last reset.
typedef struct MotionState {
    /* 0x00 */ void* reader; // the SDK's GyroscopeReader, GYRO_READER_*
    /* 0x04 */ f32 pitch;
    /* 0x08 */ f32 yaw;
    /* 0x0C */ f32 roll; // always 0
    /* 0x10 */ f32 direction[3]; // pitch, yaw, roll of the reader's direction matrix
    /* 0x1C */ f32 unwrapped[3];
    /* 0x28 */ f32 previous[3];
    /* 0x34 */ f32 reference[3];
    /* 0x40 */ f32 yawScale; // 2.1: view degrees per degree of the 3DS
    /* 0x44 */ f32 pitchScale; // 1.46
    /* 0x48 */ u8 reset; // asks Motion_Update for a new reference
    /* 0x49 */ u8 resetDone; // for the frame the reference was taken
    /* 0x4A */ u8 valid;
} MotionState; // size = 0x4C

// MotionState.reader. Speeds are in units of 360 degrees a second.
#define GYRO_READER_SPEED 0x7E0 // f32[3]
#define GYRO_READER_ANGLE 0x7EC // f32[3], turns
#define GYRO_READER_DIRECTION 0x7F8 // f32[9], rows are the 3DS's axes
#define GYRO_READER_ZERO_PLAY_ON 0x854 // u8
#define GYRO_READER_ZERO_DRIFT_ON 0x855 // u8
#define GYRO_READER_REVISE_ON 0x856 // u8: the accelerometer pulls the direction toward gravity
#define GYRO_READER_ZERO_PLAY 0x858 // f32
#define GYRO_READER_DRIFT_RADIUS 0x85C // f32: samples this near the newest are averaged with it; negative, none is
#define GYRO_READER_SCALE 0x888 // f64[3]: the HID sample times this, rounded, is the reader's raw unit
#define GYRO_READER_DRIFT_BIAS 0x8A0 // f32[3], raw units

#define gStaticContext (*(StaticContext*)0x08080010)
#define gObjectTable ((ObjectFile*)0x53CCF4)
#define gEntranceTable ((EntranceInfo*)0x543BB8)
#define gItemUsabilityTable ((u8*)0x506C58)
#define gDungeonSceneTable ((Scene*)0x4DC400)
#define gMQDungeonSceneTable ((Scene*)0x4DCBA8)
#define gSceneTable ((Scene*)0x545484)
#define gRandInt (*(u32*)0x50C0C4)
#define gRandFloat (*(f32*)0x50C0C8)
#define gDrawItemTable ((DrawItemTableEntry*)0x4D88C8)
#define gRestrictionFlags ((RestrictionFlags*)0x539DC4)
#define PLAYER ((Player*)play->actorCtx.actorList[ACTORCAT_PLAYER].first)

#endif //_Z3D_H_
