#pragma once

#include "z3Dvec.hpp"

#define BG_ACTOR_MAX 50

typedef struct {
    /* 0x00 */ char unk_00[0x2];
    /* 0x02 */ s16 ySurface;
} WaterBox;

typedef struct {
    /* 0x00 */ char unk_00[0x28];
    /* 0x28 */ WaterBox* waterBoxes;
} CollisionHeader;

typedef struct {
    /* 0x00 */ CollisionHeader* colHeader;
    /* 0x04 */ char             unk_04[0x4C];
} StaticCollisionContext; // size = 0x50

typedef struct {
    /* 0x0000 */ u8     bitFlag;
    /* 0x0004 */ ActorMesh actorMeshArr[BG_ACTOR_MAX];
    /* 0x151C */ u16    bgActorFlags[BG_ACTOR_MAX];
    /* 0x1580 */ char   unk_13F0[0x24];
} DynaCollisionContext; // size = 0x15A4

typedef struct {
    /* 0x0000 */ StaticCollisionContext stat;
    /* 0x0050 */ DynaCollisionContext   dyna;
} CollisionContext; // size = 0x15F4

// BgCheck_Allocate's budget for a scene's static collision (docs/notes/scene_collision.md).
#define BGCHECK_SUBDIV_OVERLAP 50
#define BGCHECK_SUBDIV_MIN 150
#define BGCHECK_LOOKUP_SIZE 6
#define BGCHECK_SSNODE_SIZE 4
#define BGCHECK_SSNODE_NULL 0xFFFF
#define BGCHECK_POLY_ID_MAX 0x7FFF
#define BGCHECK_DYNA_POLY_SIZE 20
#define BGCHECK_DYNA_VTX_SIZE 6
#define BGCHECK_CONTEXT_SIZE 0x15F4

// Scenes with a fixed camera: R_SCENE_CAM_TYPE, the misc settings command's second byte.
#define SCENE_CAM_TYPE_FIXED_SHOP_VIEWPOINT 0x10
#define SCENE_CAM_TYPE_FIXED_TOGGLE_VIEWPOINT 0x20
#define SCENE_CAM_TYPE_FIXED 0x30
#define SCENE_CAM_TYPE_FIXED_MARKET 0x40
#define BGCHECK_FIXED_CAM_MEMSIZE 0xEA60
#define BGCHECK_FIXED_CAM_STABLE_MEMSIZE 0x6E40
#define BGCHECK_FIXED_CAM_SUBDIV 2
#define BGCHECK_FIXED_CAM_DYNA_NODES 500
#define BGCHECK_FIXED_CAM_DYNA_POLYS 256
#define BGCHECK_FIXED_CAM_DYNA_VTX 256
#define SCENE_STABLE 54
// The overworld scenes of sBgCheckSpotScenes.
#define BGCHECK_SPOT_MEMSIZE 0x78000
#define BGCHECK_SPOT_SUBDIV_XZ 32
#define BGCHECK_SPOT_SUBDIV_Y 8
#define BGCHECK_SPOT_DYNA_NODES 1000
#define BGCHECK_SPOT_DYNA_POLYS 512
#define BGCHECK_SPOT_DYNA_VTX 512
// Every other scene; sBgCheckSceneMem and sBgCheckSceneSubdiv name the exceptions.
#define BGCHECK_MEMSIZE 0x60000
#define BGCHECK_SUBDIV_XZ 16
#define BGCHECK_SUBDIV_Y 4
#define BGCHECK_DYNA_NODES 1000
#define BGCHECK_DYNA_POLYS 600
#define BGCHECK_DYNA_VTX 600

#define BGCHECK_SPOT_SCENE_COUNT 19
#define BGCHECK_SCENE_MEM_COUNT 8
#define BGCHECK_SCENE_SUBDIV_COUNT 2

typedef struct {
    /* 0x00 */ s16 sceneId;
    /* 0x04 */ u32 memSize;
} BgCheckSceneMem; // size = 0x8

typedef struct {
    /* 0x00 */ s16 sceneId;
    /* 0x02 */ Vec3s subdivAmount;
    /* 0x08 */ s32 nodeListMax;
} BgCheckSceneSubdiv; // size = 0xC

extern s16 sBgCheckSpotScenes[BGCHECK_SPOT_SCENE_COUNT]; //0x00514430
extern BgCheckSceneMem sBgCheckSceneMem[BGCHECK_SCENE_MEM_COUNT]; //0x00514458
extern BgCheckSceneSubdiv sBgCheckSceneSubdiv[BGCHECK_SCENE_SUBDIV_COUNT]; //0x00514498

// Scene files by scene id. gSceneTable's first twelve entries have no paths; gDungeonSceneTable
// holds scenes 0 to 13 and gDungeonSceneTableMq their Master Quest files.
#define SCENE_TABLE_COUNT 111
#define SCENE_TABLE_DUNGEON_COUNT 14

typedef struct {
    /* 0x00 */ char infoPath[0x44];
    /* 0x44 */ char archivePath[0x44];
    /* 0x88 */ char unk_88[0x4];
} SceneTableEntry; // size = 0x8C

extern SceneTableEntry gSceneTable[SCENE_TABLE_COUNT]; //0x00545484
extern SceneTableEntry gDungeonSceneTable[SCENE_TABLE_DUNGEON_COUNT]; //0x004DC400
extern SceneTableEntry gDungeonSceneTableMq[SCENE_TABLE_DUNGEON_COUNT]; //0x004DCBA8

void ReadCollisionHeaderFromZsi(void* arg1, CollisionHeader** col);
