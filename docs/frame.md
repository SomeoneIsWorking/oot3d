# The frame

How one 30 Hz tick runs, from the disassembly of the USA build (Ghidra on `code.bin`).

- `Gameplay_Update` (0x45238C) runs `Play_Update` (0x2E2E60), the tick's logic, then
  `Gameplay_Draw` (0x2E25F0), which records the tick's draws; `Graph_DrawFrame` after it turns
  them into the PICA command list. The draw also runs the
  active camera's update once more when `PlayState + 0x361` is set (`camera.md`).
- The frame end, 0x3FD27C, is `Graph_DrawFrame`'s last call, virtual (`blx r3` at +0xD40,
  0x4198C8, `GRAPH_DRAW_FRAME_END_CALL`): binds the next display buffers (`nngxActiveDisplay`
  0x311EE0 and `nngxBindDisplaybuffer` 0x311C70, top screen 0x400 and bottom), waits vsyncs
  (`nngxWaitVSync` 0x3101DC) until the vsync count reaches the tick's target, then 0x40C678 runs
  the bound command list (`nngxRunCmdlist` 0x311954) and binds the other of two
  (`nngxBindCmdlist` 0x3120D4): the command lists are double-buffered, the index at the pair's +0.
- `Graph_ThreadEntry` (0x416208) is the game thread's loop. Each pass runs the game state
  (`Gameplay_Update` among them, through the state's function pointer), `Motion_Update`
  (0x416368), then `Graph_DrawFrame` (0x418B88) at +0x190 (0x416398), and 0x31BEBC, 0x31BD30
  and 0x31BB84.
- A tick's frame shows late: its display transfer lands in the back display buffer at the frame
  end, and the swap that shows it comes after the next tick's draw section. The vsync wait is a
  LightEvent at 0x5B5118 (wait 0x30B304) that the vblank handler signals until the counters at
  nngx state +0x154/+0x158 change.

## Requests

A `GxRequest` is nngx's own record, not a GSP command: its first byte is the kind (the GSP
command id it becomes), byte 2 set stops the run after it, and six words follow. The frame end's
dispatcher, 0x3027DC (from `nngxRunCmdlist`), turns each into GSP commands as follows (`r1` to
`r6` the words):

- 0, DMA: source `r1`, destination `r2`, size `r3`.
- 1, command list: address `r1`, size `r2`; bit 0 of `r3` is the GSP command's flag, and unless
  bit 2 of `r3` is set a cache flush of the list (GSP command 5) goes first.
- 2, memory fill: per buffer a start (`r1`, `r4`), a value (`r2`, `r5`) and a size word (`r3`,
  `r6`): the byte count in bits 0 to 22 (the end is start plus count) and the fill width in bits
  23 and 24 (the GSP control is `(word >> 15 & 0x300) | 1`). A start of 0 is no buffer.
- 3, display transfer: input `r1`, output `r2`, the input and output sizes `r3` and `r4` as the
  GSP takes them (width low, height high), and `r5` in nngx's flag bits: bit 6 the flip, bits 0-2
  the input format, 3-5 the output format, 7-8 the scaling, 9 the 32-pixel blocks, 10 linear input,
  11 no swizzle. The GSP's crop bit (2) is set when the output is narrower or shorter than the
  input.
- 4, texture copy: input `r1`, output `r2`, size `r3`; `r4` and `r5` hold the input's and output's
  width and gap with their halves the other way round from the GSP's; the GSP flags are 8, with 4
  when either gap word's low half is nonzero.

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
  two others differed in the word at 0x5BE20C.
- 0x5BE20C is the free-list head of a pool of 0xCC-byte objects (0x15A13xxx in this run).
  FUN_00407A9C takes one (FUN_00309E5C on the pool at its `this` +8) and FUN_00407ACC gives one
  back (FUN_00309250). The give-back also runs from FUN_00308F0C (the virtual call at 0x308F68,
  through FUN_00407840, vtable entry 0x4EC974), which walks 16 slots and returns the finished
  ones. A replay runs no other thread, so a give-back between the section's start and end
  (FUN_00308F0C's thread is inferred, not observed) leaves the real run's head different from the
  replay's; the replay's own output does not change (watch on 0x5BE20C; in a
  6 s walk the page differed in 22 of 180 replays, every PICA list byte-identical).
- `Graph_DrawFrame` (0x418B88) writes the command list: the current list's buffer (0x14193880
  in this run, 0x180000 bytes, 0x8E30 used) is stored to by FUN_003FB5EC, FUN_00466E0C and
  nngxSplitDrawCmdlist (0x302A1C, from FUN_00300328) and nothing in `Gameplay_Draw`. It also runs
  the HUD (`Hud_Update` 0x41E944, `TopHud_Update` 0x41DECC). It reaches the device: per tick six
  `SendSyncRequest` to `gsp::Gpu` with header 0x000C0000 (TriggerCmdReqQueue, at 0x4A0810), each
  after an `ArbitrateAddress` at 0x30E280, and two more `ArbitrateAddress` at 0x30B394. The six
  match the list object's six requests: four command lists and two display transfers (top and
  bottom screen).
- The nngx command-list object (`GxCmdlist` in include/z3D.hpp, two of them at 0x14000020 and
  0x1400006C, the bound one at `gNngxState.boundCmdlist`) holds the tick's requests (`GxRequest`,
  "Requests" above: here 1 run a command list and 3 display transfer). Up to the frame end call
  they are all in it, four command lists and the two screens' display transfers; the frame end
  hands them to the GSP. From `Gameplay_Update` +0xA8
  to that call the section makes no SVC but ArbitrateAddress signals (0x30E280 on 0x5B5098,
  four a tick), and a run from the same start state leaves the same requests and PICA lists byte
  for byte.
- Other writers store while the section runs (the window is preempted): heap pages 0x15325000-
  0x15330000 and 0x15A08000-0x15A39000, DSP memory 0x1FF52000-0x1FF77000 and the shared page
  0x1FF82000. A replay neither needs nor touches them.
- Not yet traced: the touch screen's share of the list and where the bottom-screen HUD records
  are drawn.

## Actors in the draw

- `Gameplay_Draw` calls `Actor_DrawAll` (0x4618E4) with the play state and its `actorCtx`. It
  walks the 12 actor lists (`ACTORCAT_MAX`, `Actor.next`), projects each actor's `world.pos`,
  and calls `Actor_Draw` (0x2D5F68) for those with a draw function and flag 0x40 or 0x20 set.
- `Actor_Draw` builds `Actor.modelMtx` at draw time (0x3679D0): translation `world.pos`, with
  `shape.yOffset * scale.y` added to y (and the play state's offset at +0x414 when flag 0x1000
  is set), rotation `shape.rot`, then `scale`. It then calls the actor's draw function. So an
  actor's root follows `world.pos` and `shape.rot` as they stand when the draw runs.
- Skeletons pose in the update, not the draw: `SkelAnime_Update` (0x36B4EC) advances `curFrame`
  and FUN_002BB34C evaluates the animation into `jointTable` (FUN_002BD9EC) and blends the
  morph (FUN_002C3814). `SkelAnime_DrawOpa` (0x35E240) only queues the skeleton's model
  (`SkelAnime.model`) in a deferred list (FUN_004C7AB0, called at +0x70,
  `SKELANIME_DRAW_OPA_QUEUE_CALL`, with the `SkelAnime` in r1) with its matrix and limb
  callbacks; it is the only caller.
- The joint table holds `limbCount` `SkelJoint`s (0x34 bytes): each limb's 3x4 matrix relative
  to its parent, rotation rows from the animation's Euler angles and the translation last, as
  FUN_002BD9EC writes them. Link's `SkelAnime` sits at +0x254 in the player actor, 25 limbs, its
  joint table inline after it.
- The player's red damage flash is stepped in the draw: `Player_Draw` (0x4BF618), while
  `Player.invincibilityTimer` (+0x2488, counted down a tick by the update, 0x250B50) is above 0,
  adds 50 less the timer, clamped to 8 to 40, to `Player.damageFlashTurn` (+0x227B, a byte) and
  sets the model's red fog from it. Watched in Hyrule Field by the Stalchildren (z3d-mods `field`
  state): the timer 12, 11, 10 ... and the turn 0x20, 0x48, 0x70, 0x98, written at 0x4BF6CC.

## Fades

- A scene's fade-in is the transition's screen fill, not the scene's lights. `PlayState.transitionCtx`
  (+0x7D08) holds the transition's instance and its methods; for a fade the instance is a
  `TransitionFade` and `draw` (+0x1F0) is `TransitionFade_Draw` (0x45CD50), `start` (+0x1F4)
  `TransitionFade_Start` (0x460B1C). The update steps `timer` by 2 a tick and sets the fill's alpha
  from it (warp into Link's house: alpha 0xFF down by 0x19 or 0x1A a tick to 0 over ten ticks,
  `timer` 0 to 20, then `isDone` 1).
- `Gameplay_Draw` calls `transitionCtx.draw` while `transitionMode` (+0x7F12) is 3 or 0xB, or the
  transition's type is 0x38 or more, then `TransitionFade_Draw` on `transitionFadeFlash` (+0x7F14)
  every tick. `TransitionFade_Draw` reads only the fill's colour (alpha at +4, then blue, green,
  red) and draws nothing at alpha 0.
- A place name's card fades by `ActorContext.titleCtx` (play +0x224C): `TitleCard_Update`
  (0x47953C) steps `alpha` (+0x12) and `intensity` (+0x14) with `Math_StepToS` in the update, up
  while `delayA` (+0x10) counts down and then down to 0, and the draw only reads them (Kakariko's
  first entry from `kf`, warp 0xDB: alpha 0x50, 0x55, 0x5A, 0x5F and intensity 0x90, 0x99, 0xA2,
  0xAB on four ticks).

## Material animations

- Texture scroll and the other material animations (CMAB files) play through `AnimPlayer`s
  (0x98 bytes, include/z3Danimation.hpp). `AnimPlayer_Update` (0x373BEC) adds `speed` to `frame`,
  wraps it at the animation's frame count when `loop` is set (holds it there otherwise), and
  applies the frame to each of its targets, which write the materials' texture coordinates.
  It does nothing while `paused` or the word at 0x54C8CC is set. 67 sites call it, actors' among them.
- The scene's are stepped in the draw: `Gameplay_Draw` calls `Scene_Draw` (0x45BFC4), which runs
  `AnimPlayer_Update` over the scene's lists (`SCENE_DRAW_ANIM_UPDATE_CALL*`) and then the scene's
  draw handler from the table at 0x549138 by `PlayState.sceneConfig`. One list's frames are set
  from the time of day first. Kokiri Forest's waterfall is the first list's one player (600
  frames, speed 1, looping; its frame read live 207, 208, ... a tick, the texture coordinates
  written right after by 0x40992C).
- A model's own material animation is the `AnimPlayer` at the model's +0xC: `Model_Update`
  (0x3FED90), reached from the draw's queueing of a model (0x372170), poses the model and then
  tail-branches to `AnimPlayer_Update` on it (`MODEL_UPDATE_MATERIAL_CALL`, r0 the player), so
  it is stepped in the draw section. Zora's Domain's waterfall is such a model's: `BgSpot07Taki_Draw`
  (0x29DDBC, actor 0x120) sets its player's speed and queues the model, and a write watch on
  the player's frame (entrance 0x108 from `kf`, frame 59 a tick on, speed 1, looping) shows each
  step written there with the draw's return address (0x29DEC4) in lr.

## Effects

- Kokiri Forest's floating light motes are `ObjectKankyo`'s fairies (actor 0x97, params 0;
  `ObjectKankyo_Draw` 0x297508 sends params 0 to 0x159B2C). The actor holds 64
  `ObjectKankyoEffect`s at +0x1A4, `PlayState.kankyoEffectCount` of them drawn; each fairy is
  drawn at `pos + base`. The update moves `pos` (about 0.5 to 1.3 units a tick) and steps
  `timer`; `base` stayed put while Link stood still. The draw steps `alpha` itself (up or down by
  10, 20 or 100 a tick by `state` and `timer`) and pushes each fairy into a billboard batch
  (0x371F1C, then 0x332A14 into the batch's arrays at +0x1E4), which its draw (0x3FC2F8) empties.

## The target arrow

- The yellow arrow that spins over a targetable actor (over a Stalchild in Hyrule Field at night,
  Navi hovering by it) is the Z-target arrow, not an effect: `Target_Draw` (0x47B124, the N64
  game's `func_8002C124`) draws it from `ActorContext` +0x6C (`PlayState` +0x20F8) during the
  draw. It turns the arrow about Y by `PlayState.gameplayFrames` (+0x5BF4, one a tick) times 3000
  binary angles (0x177 << 3, masked to 16 bits) and sets the matrix on its two models (0x3721E0
  at +0xADC and +0xAF8), so the turn steps once a tick. The angle in r0 is moved to s0 at +0xA58
  (`TARGET_DRAW_ARROW_TURN_MOVE`), made a float at +0xA64 (`TARGET_DRAW_ARROW_TURN_CONVERT`) and
  scaled by the unit in s1 for `Matrix_RotateY` (0x3735E8), as in MM3D. Found by write watches on the `TargetContext`'s model pointers in
  z3d-mods (`field` state): the models' rotation words, and nothing else of the arrow, change
  each tick.
