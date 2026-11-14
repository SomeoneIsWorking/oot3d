# Arenas

USA `code.bin`. Read from the disassembly and walked in live memory (z3d-mods `arena:` step,
runs oot-k3, oot-k4).

- The system arena is at 0x565500, the Zelda arena at 0x5A2E3C. An arena's first word is its
  first block.
- Block header, 16 bytes, the data after it: `+0x0 s16` magic 0x7373, `+0x2 u16` free flag,
  `+0x4 u32` size without the header, `+0x8` next, `+0xC` previous. The N64's `ArenaNode`
  without its debug fields.
- `LockAndAlloc` (0x2C36EC) takes the first free block that fits, from the start;
  `Arena_MallocR` (0x2EAB8C) the last, from the end. `SystemArena_Malloc_0035010c` and
  `ZeldaArena_Malloc` are the first on their arena, `SystemArena_Malloc` (0x2F1424) and
  `ZeldaArena_MallocR` (0x2F1434) the second.
- A failed allocation returns 0 and the file loader (0x42C674) does not check it: it reads the
  file to address 0. A 0x432E70 byte room with no hole that large stopped the game this way
  (z3d-mods run oot-k1: 1 MB of unmapped writes from 0, then the code itself overwritten).
- File 1 as a child: the system arena holds about 0x18CBB40 bytes; Link's house leaves
  0x966BE0 free, Kokiri Forest 0x680640, Kakariko with z3d-mods' stepped room 0x415270.
- A room is one system arena block of three times its file's size: Kokiri Forest's room 0
  (0xA0C64 bytes in the ROM) takes 0x1E2550, z3d-mods' stepped Kakariko room (0x1664D0 bytes)
  0x432E70; the block starts with the file (`ZSI\x01`).
