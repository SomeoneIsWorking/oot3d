# Scene collision and room meshes (romfs scene files)

Recovered from the zelda3d project's notes (its repository is gone); checked there across spot00,
spot01, spot04, gerudoway, ydan and ddan on the USA decrypted ROM; the collision header and
polygon layout were read again from spot01 with z3d-mods' `tools/z3dmods/zsi.py`.

## Scene header `<scene>_info.zsi`
- A command list at file offset 0x10, 8 bytes per command, byte 0 the type; a command's second
  word, when it is an address, counts from 0x10.
- Command 0x03 (collision): its second word is the collision header.

- The scene table `gSceneTable` (0x545484) has 111 entries of 0x8C bytes, the scene file's
  path at +0x00 and its archive's at +0x44; the index is the scene id (82 `spot01`). Its first
  twelve entries have no paths: `gDungeonSceneTable` (0x4DC400) holds scenes 0 to 13 and
  `gDungeonSceneTableMq` (0x4DCBA8) their Master Quest files (`ydan_dd`, ...).

## Collision header
At `0x10 + word`: like every offset in the file, the command's word and the header's own pointers
count from 0x10, the end of the `ZSI\x01` name header.

| Offset | Type | Field |
|---|---|---|
| 0x00 | Vec3s | minimum bounds |
| 0x06 | Vec3s | maximum bounds |
| 0x0C | u16 | vertex count |
| 0x0E | u16 | polygon count |
| 0x10 | u16 | surface type count |
| 0x12 | u16 | camera data count |
| 0x14 | u16 | water box count |
| 0x18 | u32 | vertex list |
| 0x1C | u32 | polygon list |
| 0x20 | u32 | surface type list (8 bytes each) |
| 0x24 | u32 | camera data |
| 0x28 | u32 | water boxes |

Kakariko (spot01): 786 vertices at 0x4AC, 1222 polygons at 0x1718, 28 surface types at 0x7690,
camera data at 0x7770, one water box at 0x787C, the header at 0x788C; each list ends where the
next begins.

- Vertex: `Vec3s` (s16 x, y, z), stride 6, world space in N64 units, the same frame as the room
  mesh (collision floors sit within about 0.1 unit of the drawn floor in spot01).
- Polygon, stride 20: `+0 u16` surface type index, `+2 vA`, `+4 vB`, `+6 vC` (u16, index in the
  low 13 bits, flags in the top 3 of vA and vB), `+8 u16` one value for the whole scene (0 in
  spot01 and ganontika, 0xBC80 in ydan, 0xC52F in spot07; meaning unknown, z3d-mods copies it),
  `+A/+C/+E s16` normal (x, y, z
  over 32767), `+0x10 f32` plane distance (`n·p == -dist`). The normal is the face normal and
  points the way `(vB - vA) x (vC - vA)` does. With this frame all 1222 of Kakariko's polygons
  satisfy the plane equation at vA; small polygons store a normal a few degrees off the one
  their integer vertices give.
- Floor and wall are told apart by the normal (floors have `ny > 0.5`), not by the surface type.
- The play state's static collision context is at `PlayState +0xA98`, laid out as on the N64:
  header pointer, bounds (6 f32), subdivisions (3 s32; 32, 8, 32 in Kakariko), cell size and its
  inverse, the cell table at +0x40, then the node list: u16 length at +0x44, u16 used at +0x46,
  the nodes at +0x48.

## The room a scene's collision gets (BgCheck_Allocate, 0x1311D0)
Scene command 0x03 (`Scene_CommandCollisionHeader`, 0x273070) calls it. It is the N64 routine
with other numbers; the names are in `include/z3Dbgcheck.hpp`.

| Scene | Memory | Cells x, y, z | Moving collision: nodes, triangles, vertices |
|---|---|---|---|
| fixed camera (`R_SCENE_CAM_TYPE` 0x10, 0x20, 0x30, 0x40) | 0xEA60, the stable (54) 0x6E40 | 2, 2, 2 | 500, 256, 256 |
| `sBgCheckSpotScenes` (0x514430): 81 to 99, the overworld | 0x78000 | 32, 8, 32 | 1000, 512, 512 |
| every other | 0x60000, or `sBgCheckSceneMem`'s (0x514458) | 16, 4, 16, or `sBgCheckSceneSubdiv`'s (0x514498) | 1000, 600, 600 |

- `sBgCheckSceneMem`: scene 81 94000, 26 61840, 79 57744, 23 88464, 68 57744, 6 186768,
  4 209296, 25 67984 (81 is a spot scene and never gets there). `sBgCheckSceneSubdiv`: scene 7
  23, 7, 14 and scene 3 38, 1, 38, both with a list length of -1 (none of its own).
- `R_SCENE_CAM_TYPE` (`gGameInfo->data` +0x4B2) is the second byte of scene command 0x19
  (`Scene_CommandMiscSettings`, 0x256CA8).
- The node list gets what the rest leaves:
  `(memory - cells * 6 - triangles - nodes * 4 - triangles' * 20 - vertices' * 6 - 0x15F4) / 4`,
  the primed counts the moving collision's. That many nodes are taken from the play state's
  arena, but the length at +0x44 is stored as a u16: Kakariko's 104553 reads 39017, Ganon's
  Castle's 89104 reads 23568.
- A cell is `(extent / cells) + 1` long, at least 150. `BgCheck_InitializeStaticLookup`
  (0x14DA40) walks each triangle's range of cells, widened by one where a corner is within 50
  of the cell's edge, tests the triangle against the cell grown by 50 on every side, and
  `StaticLookup_AddPolyToSSList` (0x3562C0) takes one node per pair. It does not look at the
  length: the used count at +0x46 is a u16 and a node's link is a u16 with 0xFFFF for none, so
  a scene has at most 65535 nodes; a node names its triangle in an s16, so at most 32768
  triangles.
- Seen in game (z3d-mods run oot-n1): a Kakariko with 9333 triangles reads a length of 36989
  and 42355 nodes used, past that length; Link lands on its last triangles, the steps by the
  well, on three visits. Counting the pairs whose triangle touches the grown cell gives
  42288; the game's own test lets a few more through (14563 for 14536 in Ganon's Castle,
  28584 for 28515 in z3d-mods' stepped Kakariko). Every scene of the ROM fits by this count:
  the fullest is `ganon_final`, 4858 of 7431.
- Corner flag 0x4000 on vA lets actors through a triangle (shown in MM3D, decomp/mm3d
  `docs/formats/scene_files.md`; in OoT3D Link walks Kakariko's flagged risers, run
  oot-st17-spot01, and stopped at an unflagged one, oot-st16-spot01).
- The lists can live anywhere: z3d-mods writes longer vertex and polygon lists after the end of
  the file and points the header at them.

## Room mesh `<scene>_<R>_info.zsi`
- Each room file holds exactly one embedded CMB (find its `cmb ` magic; its offset varies by room)
  behind the 0x0A Mesh command. All 390 room files follow this.
- Opaque and translucent parts are separate materials of that one CMB, not separate meshes.
- Its vertices are already in world space at N64 scale: draw with an identity model matrix.
- Texture formats seen across every room: ETC1, ETC1A4, A8, L8, LA8, L4, RGBA5551, RGB565.
- Room header offsets count from file offset 0x10 (the end of the `ZSI\x01` name header): the
  command list, the 0x18 alternate-setup table, actor and object lists, and the 0x0A mesh header.
  The mesh header (type 2, one entry) points at an entry whose `+0x08`/`+0x0C` are the opaque and
  translucent CMB offsets (0 when absent). The CMB sits before the mesh header and the lists, so
  growing it moves them and every offset at or past its end.
- A primitive's index count (`prm +0x14`, u16) is sign-extended into the draw's vertex count: a
  count of 38907 drew 0xFFFF97FB vertices and Azahar faulted reading the index buffer. A primitive
  holds at most 32767 indices.
- A sepd draws each of its primitive sets (`sepd +0x08`, u16 count) in turn: after the eight
  attributes at `+0x104` come a bone-dimension word and one u16 `prms` offset per set (from the
  sepd), padded to 4, then the `prms` chunks. Actor models use up to five; a room shape split into
  two sets of 20094 indices draws in Kakariko.

## Dead ends
- The zelda3d notes framed the header at the command's plain offset (0x10 too early) and so
  read the lists as "vertexList + 0x10" and "polygonList - 2" with the type after the vertices.
  Framed from 0x10 nothing needs a shift.
- noclip names command 0x03 but never parses it. Its room mesh-header chain is right once read
  from 0x10, as its `parseRooms` does; at plain file offsets it does not resolve.
