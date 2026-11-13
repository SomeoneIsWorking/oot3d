#ifndef _Z3DCUTSCENE_H_
#define _Z3DCUTSCENE_H_

#include "z3Dvec.hpp"

typedef struct {
    /* 0x00 */ u16 action; // "dousa"
    /* 0x02 */ u16 startFrame;
    /* 0x04 */ u16 endFrame;
    /* 0x06 */ Vec3s rot;
    /* 0x0C */ Vec3i startPos;
    /* 0x18 */ Vec3i endPos;
    /* 0x24 */ Vec3i normal;
} CsCmdActorAction; // size = 0x30

// A script (" BDQ") is this header, then cmdCount commands: an s32 id and its payload.
typedef struct {
    /* 0x00 */ u32 magic;
    /* 0x04 */ u32 version;
    /* 0x08 */ s32 cmdCount;
    /* 0x0C */ s32 endFrame;
} CutsceneScriptHeader; // size = 0x10

// Command ids are N64 z_demo's plus Grezzo's 0x96 and 0x97 (camera spline).
#define CS_CMD_END -1
#define CS_CMD_CAM_EYE_SPLINE 0x01
#define CS_CMD_CAM_AT_SPLINE 0x02
#define CS_CMD_CAM_EYE_SPLINE_REL_TO_PLAYER 0x05
#define CS_CMD_CAM_AT_SPLINE_REL_TO_PLAYER 0x06
#define CS_CMD_CAM_EYE 0x07
#define CS_CMD_CAM_AT 0x08
#define CS_CMD_RUMBLE_CONTROLLER 0x09
#define CS_CMD_TEXT 0x13
#define CS_CMD_TRANSITION 0x2D
#define CS_CMD_TIME 0x8C
#define CS_CMD_UNK_96 0x96
#define CS_CMD_CAM_SPLINE 0x97
#define CS_CMD_DESTINATION 0x3E8

// Command sizes, id included (Cutscene_ProcessCommands' switch, docs/notes/cutscene_format.md).
// A camera point list is a 12-byte head, then 16-byte points through the one whose first byte is 0xFF.
#define CS_CAM_SPLINE_HEAD_SIZE 0x0C
#define CS_CAM_POINT_SIZE 0x10
#define CS_CMD_CAM_SIZE 0x1C
#define CS_CMD_FIXED_SIZE 0x10
// Lists: id, u32 count, then count entries; 0x09, 0x13 and 0x8C have 12-byte entries, others actor cues.
#define CS_LIST_HEAD_SIZE 0x08
#define CS_LIST_ENTRY_SIZE 0x0C
// 0x96: u16 count at +0x0A, then 32-byte entries after a 12-byte head. 0x97: u32 size at +0x04.
#define CS_CMD_UNK_96_HEAD_SIZE 0x0C
#define CS_CMD_UNK_96_COUNT 0x0A
#define CS_CMD_UNK_96_ENTRY_SIZE 0x20
#define CS_CMD_CAM_SPLINE_HEAD_SIZE 0x08

typedef struct {
    /* 0x00 */ u16 textId; // 0xFFFF for none
    /* 0x02 */ u16 startFrame;
    /* 0x04 */ u16 endFrame;
    /* 0x06 */ u16 type;
    /* 0x08 */ u16 altTextId1; // after the first answer of a choice
    /* 0x0A */ u16 altTextId2;
} CsCmdText; // size = 0xC

#define CS_TEXT_NORMAL 0
#define CS_TEXT_CHOICE 1
#define CS_TEXT_OCARINA_ACTION 2

typedef enum {
    CS_STATE_IDLE,
    CS_STATE_SKIPPABLE_INIT,
    CS_STATE_SKIPPABLE_EXEC,
    CS_STATE_UNSKIPPABLE_INIT,
    CS_STATE_UNSKIPPABLE_EXEC
} CutsceneState;

typedef enum {
    /* 0x00 */ OCARINA_ACTION_UNK_0,
    /* 0x01 */ OCARINA_ACTION_FREE_PLAY_OCARINA,
    /* 0x02 */ OCARINA_ACTION_MINUET,
    /* 0x03 */ OCARINA_ACTION_BOLERO,
    /* 0x04 */ OCARINA_ACTION_SERENADE,
    /* 0x05 */ OCARINA_ACTION_REQUIEM,
    /* 0x06 */ OCARINA_ACTION_NOCTURNE,
    /* 0x07 */ OCARINA_ACTION_PRELUDE,
    /* 0x08 */ OCARINA_ACTION_SARIAS_SONG,
    /* 0x09 */ OCARINA_ACTION_EPONAS_SONG,
    /* 0x0A */ OCARINA_ACTION_ZELDAS_LULLABY,
    /* 0x0B */ OCARINA_ACTION_SUNS_SONG,
    /* 0x0C */ OCARINA_ACTION_SONG_OF_TIME,
    /* 0x0D */ OCARINA_ACTION_SONG_OF_STORMS,
    /* 0x0E */ OCARINA_ACTION_OCARINA_ACTION_UNK_E,
    /* 0x0F */ OCARINA_ACTION_MINUET_PLAYBACK,
    /* 0x10 */ OCARINA_ACTION_BOLERO_PLAYBACK,
    /* 0x11 */ OCARINA_ACTION_SERENADE_PLAYBACK,
    /* 0x12 */ OCARINA_ACTION_REQUIEM_PLAYBACK,
    /* 0013 */ OCARINA_ACTION_NOCTURNE_PLAYBACK,
    /* 0x14 */ OCARINA_ACTION_PRELUDE_PLAYBACK,
    /* 0x15 */ OCARINA_ACTION_SARIAS_SONG_PLAYBACK,
    /* 0x16 */ OCARINA_ACTION_EPONAS_SONG_PLAYBACK,
    /* 0x17 */ OCARINA_ACTION_ZELDAS_LULLABY_PLAYBACK,
    /* 0x18 */ OCARINA_ACTION_SUNS_SONG_PLAYBACK,
    /* 0x19 */ OCARINA_ACTION_SONG_OF_TIME_PLAYBACK,
    /* 0x1A */ OCARINA_ACTION_SONG_OF_STORMS_PLAYBACK
} OcarinaSongActionIDs;

#endif //_Z3DCUTSCENE_H_
