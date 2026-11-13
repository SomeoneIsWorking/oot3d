# Scene collision and room meshes (romfs scene files)

Recovered from the zelda3d project's notes (its repository is gone); checked there across spot00,
spot01, spot04, gerudoway, ydan and ddan on the USA decrypted ROM. Our tool for it is not rebuilt
yet.

## Scene header `<scene>_info.zsi`
- A command list at file offset 0x10, 8 bytes per command, byte 0 the type. Command addresses are
  plain file offsets (the Rooms command, 0x04, points at the room-name strings).
- Command 0x03 (collision): its second word is the file offset of the collision header.

## Collision header
| Offset | Type | Field |
|---|---|---|
| 0x1C | u16 | vertex count |
| 0x1E | u16 | polygon count |
| 0x28 | u32 | vertex list |
| 0x2C | u32 | polygon list |
| 0x30 | u32 | surface type list |
| 0x34 | u32 | camera data |
| 0x38 | u32 | water boxes |

The header's own pointers are not plain offsets: vertices start at `vertexList + 0x10` and the
polygon array at `polygonList - 2`. Both shifts held in every scene checked; they differ and are
both right. Surface types, camera data and water boxes are not decoded yet (likely `+0x10` too).

- Vertex: `Vec3s` (s16 x, y, z), stride 6, world space in N64 units, the same frame as the room
  mesh (collision floors sit within about 0.1 unit of the drawn floor in spot01).
- Polygon, stride 20: `+0 vA`, `+2 vB`, `+4 vC` (u16, index in the low 13 bits, flags in the top
  3), `+6 u16` surface type index and flags, `+8/+A/+C s16` normal (x, y, z over 32767), `+E f32`
  plane distance (`n·p == -dist`). The stored normal is the face normal.
- Floor and wall are told apart by the normal (floors have `ny > 0.5`), not by the surface type.

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

## Dead ends
- Reading vertices at the plain `vertexList` offset gives garbage; the polygon's last u16 is the
  next polygon's first field unless the array is framed at `polygonList - 2`.
- noclip names command 0x03 but never parses it. Its room mesh-header chain is right once read
  from 0x10, as its `parseRooms` does; at plain file offsets it does not resolve.
