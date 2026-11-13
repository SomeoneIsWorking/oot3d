# Saving

USA `code.bin`. Read from the disassembly and live memory (z3d-mods harness).

## Sram_WriteSave (0x2FDAC8)
`Sram_WriteSave(GameState* state, s32 fileSelect)` is the one routine that writes the file. Its
three callers: the pause screen's Save (0x43A344, `fileSelect` 0), 0x41CACC (`fileSelect` 0) and
a tail call at 0x43E120 (`fileSelect` 1).

With `fileSelect` 0 and the state a play state (the byte at `GameState +0x100` is 3) it:
- sets `gSaveContext.sceneIndex` to `play->sceneNum`,
- copies the scene's chest, switch, clear and collect flags into `gSaveContext.sceneFlags`
  (0x2E62DC, the N64's `Play_SaveSceneFlags`),
- adds one to `gSaveContext.saveCount`.

Then, for either argument, it stamps the date (save +0x13BC..+0x13CC), zeroes the checksum at
+0x14D8, sums the first 0x14DC bytes with 0x2FAF90 and stores the result there, copies those bytes
into the file's buffer and queues the write.

The write itself runs on another thread (0x42C964: open, set size, write, `CommitSaveData`, then
sleep so the whole takes at least a second). 0x2FDAA4 returns 1 once nothing is queued.

`save00.bin` is those 0x14DC bytes as they are: `gSaveContext` from its start, checksum last. A
file saved with bytes in `unused_6C` keeps them (read back from the file: z3d-mods run oot-sv1).

Called from play with `fileSelect` 0, outside the pause screen, it writes a file that loads after
a restart (z3d-mods' autosave: runs oot-as2, oot-as3).
