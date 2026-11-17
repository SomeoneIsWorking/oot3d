# The frame

How one 30 Hz tick runs, from the disassembly of the USA build (Ghidra on `code.bin`).

- `Gameplay_Update` (0x45238C) runs `Play_Update` (0x2E2E60), the tick's logic, then
  `Gameplay_Draw` (0x2E25F0), which records the tick's draws; the list build after it turns them
  into the PICA command list. The draw also runs the
  active camera's update once more when `PlayState + 0x361` is set (`camera.md`).
- The frame end, 0x3FD27C, after the draw: binds the next display buffers (`nngxActiveDisplay`
  0x311EE0 and `nngxBindDisplaybuffer` 0x311C70, top screen 0x400 and bottom), waits vsyncs
  (`nngxWaitVSync` 0x3101DC) until the vsync count reaches the tick's target, then 0x40C678 runs
  the bound command list (`nngxRunCmdlist` 0x311954) and binds the other of two
  (`nngxBindCmdlist` 0x3120D4): the command lists are double-buffered, the index at the pair's +0.
- `Graph_ThreadEntry` (0x416208) is the game thread's loop. Each pass runs the game state
  (`Gameplay_Update` among them, through the state's function pointer), `Motion_Update`
  (0x416368), then the list build, `bl 0x418B88` at +0x190 (0x416398), and 0x31BEBC, 0x31BD30
  and 0x31BB84.

## What the draw section touches

Measured in z3d-mods with the core's store journal around the call (USA build, Kokiri Forest,
`interp census`; Azahar `core/section_replay`), per tick:

- `Gameplay_Draw` (from `Gameplay_Update` +0xA8, `GAMEPLAY_UPDATE_DRAW_CALL`) makes no SVC and
  touches no GSP or hardware register. It runs about 99k instructions and stores to about 65
  pages: .data/.bss (0x500000-0x5FFFFF, ~10), the heap's 0x81xxxxx pool (~33) and 0x80/0x82/0x87,
  0x98 pages, its stack, and the linear heap's 0x140000xx command-list objects. It does not write
  the PICA list: it fills the game's own draw records, which the list build turns into PICA
  commands.
- Run twice from the same start state, it stores the same bytes: with the first run's stores
  rolled back, a second run left every page it wrote equal to the first in 178 of 180 ticks. The
  two others differed in the word at 0x5BE20C, a current-object pointer that FUN_00407A9C sets and
  FUN_00407ACC clears (values 0x15A13AA8 and 0x15A13B74 alternate within a tick).
- The list build (`bl 0x418B88`) writes the command list: the current list's buffer (0x14193880
  in this run, 0x180000 bytes, 0x8E30 used) is stored to by FUN_003FB5EC, FUN_00466E0C and
  nngxSplitDrawCmdlist (0x302A1C, from FUN_00300328) and nothing in `Gameplay_Draw`. It also runs
  the HUD (`Hud_Update` 0x41E944, `TopHud_Update` 0x41DECC). It reaches the device: per tick six
  `SendSyncRequest` to `gsp::Gpu` with header 0x000C0000 (TriggerCmdReqQueue, at 0x4A0810), each
  after an `ArbitrateAddress` at 0x30E280, and two more `ArbitrateAddress` at 0x30B394. The six
  match the list object's six requests: four command lists and two display transfers (top and
  bottom screen).
- The nngx command-list object (0x3C bytes, two of them at 0x14000020 and 0x1400006C, the bound
  one at nngx state 0x5A6F30 +0x9C): +0x4 buffer, +0x8 size, +0xC used bytes, +0x18 request
  array, +0x1C request capacity, +0x20 request count. A request is 0x1C bytes: type (1 run a
  command list, 3 display transfer) then its arguments (list address and size; source, target
  and dimensions).
- Other writers store while the section runs (the window is preempted): heap pages 0x15325000-
  0x15330000 and 0x15A08000-0x15A39000, DSP memory 0x1FF52000-0x1FF77000 and the shared page
  0x1FF82000. A replay neither needs nor touches them.
- Not yet traced: the touch screen's share of the list and where the bottom-screen HUD records
  are drawn.
