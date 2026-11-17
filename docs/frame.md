# The frame

How one 30 Hz tick runs, from the disassembly of the USA build (Ghidra on `code.bin`).

- `Gameplay_Update` (0x45238C) runs `Play_Update` (0x2E2E60), the tick's logic, then
  `Gameplay_Draw` (0x2E25F0), which builds the tick's PICA command list. The draw also runs the
  active camera's update once more when `PlayState + 0x361` is set (`camera.md`).
- The frame end, 0x3FD27C, after the draw: binds the next display buffers (`nngxActiveDisplay`
  0x311EE0 and `nngxBindDisplaybuffer` 0x311C70, top screen 0x400 and bottom), waits vsyncs
  (`nngxWaitVSync` 0x3101DC) until the vsync count reaches the tick's target, then 0x40C678 runs
  the bound command list (`nngxRunCmdlist` 0x311954) and binds the other of two
  (`nngxBindCmdlist` 0x3120D4): the command lists are double-buffered, the index at the pair's +0.
- Not yet traced: what `Gameplay_Draw` writes besides the list, which of its calls reach a device
  (nngx locks, GX syncs, SVCs), and where the HUD and the touch screen add to the list.
