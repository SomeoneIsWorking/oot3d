# Scene collision and room meshes (romfs scene files)

Recovered from the zelda3d project's notes (its repository is gone); checked there across spot00,
spot01, spot04, gerudoway, ydan and ddan on the USA decrypted ROM; the collision header and
polygon layout were read again from spot01 with z3d-mods' `tools/z3dmods/zsi.py`.

## Scene header `<scene>_info.zsi`
- A command list at file offset 0x10, 8 bytes per command, byte 0 the type; a command's second
  word, when it is an address, counts from 0x10.
- Command 0x03 (collision): its second word is the collision header.

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
  inverse, the cell table at +0x40, then the node list: u16 length at +0x44, u16 used at +0x46.
  Every triangle takes a node in every cell it touches. The length shrinks by one for every
  four triangles (39017 in Kakariko with 1222 triangles, 36097 with 12901), and the game writes
  past it: a scene needing 54760 nodes dropped Link through late triangles and stopped the
  game on a second visit (z3d-mods runs oot-bg1, oot-as10). Ganon's Castle uses 14563 of 23568.
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
