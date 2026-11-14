# RE Frontier — the ordered RE dependency chain toward a faithful port

Tracked by `tools/re_frontier.py` (consult it FIRST; update it in the SAME commit
that changes a step). This is the fine-grained companion to `docs/codemap.md`:
the codemap says *what subsystem exists*, this says *which ordered RE step is
real reverse-engineering vs a hack that jumped ahead*.

**Hard rule (no hacks / no fallbacks):** a `⛔ hack` status is DEBT, never an
acceptable resting state. It marks a shortcut standing in for absent RE and MUST
be removed as its real mechanism lands. `re_frontier.py hacks` is the debt list;
`re_frontier.py next` tells you the next RE-ready step.

**`re-verified` MEANS FAITHFUL to the real target — not "the mechanism runs."** A
step is `re-verified` only when its OUTPUT matches the real game/binary (look /
sound / behavior) on real data. An internal trace ("bytecode reached the call
site", "N rows attached") is a mechanism check, NOT faithfulness — if it runs but
the result doesn't match the real target, it is `re-partial` with the
faithfulness gap named. The user observes the running system; that observation
overrides any internal trace.

**Fail fast & loud:** a failure must surface loudly, never silently fall back —
unless the fallback IS intended behavior of the real target being reproduced.

Statuses: ✅ re-verified · 🟡 re-partial (honest gap) · 🔬 in-progress ·
⛔ hack (debt, must remove) · ⬜ todo · ➖ skip-by-design · ✍ authored · ⏸ blocked (computed).

<!-- Entries are `## <area>` sections holding `### <id> — <title>` headings, each
     followed by `- <field>: <value>` lines. Prose ANYWHERE in this file — before,
     between or inside entries — is yours: tools/re_frontier.py edits only the
     field lines it is told to change and copies everything else through byte for
     byte, refusing the write if anything else would be lost. -->

## cutscene

### cutscene.script-format — Cutscene script container and commands
- status: re-partial
- deps: 
- evidence: docs/notes/cutscene_format.md: header and command shapes, interpreter end-frame store at 0x2C5C20 seen by watchpoint
- where: docs/cutscene.md
- gap: command payloads named only for the commands the title demo and z3d-mods use; command sizes and the text entry are in include/z3Dcutscene.hpp
- notes: 

### cutscene.interpreter — CutsceneHandler_RunScript and Cutscene_ProcessCommands
- status: re-verified
- deps: cutscene.script-format
- evidence: z3d-mods Start skip: stepping Cutscene_ProcessCommands to the end frame lands where watching lands (Kakariko intro, opening narration), save context equal but naviTimer and +0x15B4
- where: docs/cutscene.md
- gap: 
- notes: 

### cutscene.textbox — Text commands, Message_GetState and Message_CloseTextbox
- status: re-partial
- deps: cutscene.interpreter
- evidence: opening narration skip closes every box and lands in scene 81 like watching; the Deku Tree's choice (spot04 0xFFF1, text 0x1092) waits for the answer and both answers land in scene 71 like watching
- where: docs/cutscene.md
- gap: manager bytes +0xF38 and +0xFA4 are unnamed
- notes: 


## message

### message.layout — Message_LayoutText, its control codes and the text object methods
- status: re-verified
- deps: 
- evidence: z3d-mods Quick Text built on it prints every box whole (Deku Tree after Gohma, opening narration); disassembly: the parse call at 0x2E0490, the 48-entry control-code table at 0x2E058C, handlers for 0x01, 0x04, 0x05 and 0x19 calling text object methods +0x14, +0x20, +0x24, +0x8C (vtable 0x4ECCD8)
- where: docs/message.md
- gap: 
- notes: 

## camera

### camera.fields — Camera fields the free camera reads and writes
- status: re-partial
- deps: 
- evidence: live read of play+0x364 in Kokiri Forest: play (+0xD4) is the play state, |at-eye| matches dist (+0x124), playerPosRot (+0xDC) follows Link, camDir/inputDir (+0x182/+0x17C) turn as he walks, status 7, setting 1; mode (+0x18C) reads 0 walking, 1 with L held, 6 in first person, 16 falling; Camera_Update (0x2D84C4) loads +0xD4 and checks +0x188
- where: include/z3Dcamera.hpp
- gap: camDataIdx and prevSetting only read at rest; animState, behaviorFlags, stateFlags and the distortion fields not yet checked; Quake_Update and Camera_GetCamDataId unchecked
- notes: the OoT3D Randomizer's layout (no licence) was the starting guess; only fields seen live are named

### camera.update — Camera_Update's dispatch, call sites, View_LookAt and Camera_BGCheckInfo
- status: re-verified
- deps: camera.fields
- evidence: z3d-mods' free camera built on it orbits Link in Kokiri Forest, stays inside Dodongo's Cavern's corridor walls and hands back on L, first person, text, the Market's fixed view and cutscenes (runs oot-fc6, oot-fc8); disassembly: the three bl Camera_Update sites (0x2E2DF8, 0x2E437C, 0x2E43A0), the settings and function tables loaded at 0x2D8890/0x2D88D4, the camDir store at 0x2D93EC, Camera_BGCheckInfo's reads and writes of colChk
- where: docs/camera.md
- gap: 
- notes: 

### camera.settings — sCameraSettings and the function each setting and mode runs
- status: re-partial
- deps: camera.update
- evidence: table dumped from the binary (0x516FF0, 78 entries; function table 0x517260); settings read live in 14 scenes (docs/camera.md)
- where: docs/camera.md, include/z3Dcamera.hpp
- gap: settings are numbered, not named: only 0x01-0x06, 0x19, 0x1A and 0x1E are matched to their N64 names by function and scene; the horse (0x06) and the crawlspace (0x1E) are read from the table, not seen live; each mode's values are not decoded
- notes: 

## items

### items.buttons — equips, the item screen's layout, ItemMenu_ApplyLayout, Hud_SetItemButton and gHud.state
- status: re-partial
- deps: 
- evidence: live: an item assigned in the item menu writes equips at 0x33C5F8/0x33C6A4 (write watch) and the button then uses it; Hud_SetItemButton's five-way switch at 0x2EFEDC; gHud.state and the three screens' state words peeked in play and under each screen; pad bit 12 seen in GameState.buttonsHeld (runs oot-ch2, oot-ch3); ItemMenu_ApplyLayout read from the disassembly (0x33C25C-0x33C72C) and seen rewriting equips from the layout when the screen opens (oot-ch5)
- where: docs/item_buttons.md, include/z3D.hpp, include/z3Dsave.hpp
- gap: ItemMenu_ApplyLayout is not decompiled and its callers are unread; gHud's other fields and the meaning of its state beyond 0 and 2 are unknown; the map screen's word was not seen up; unused_6C is called unused from the files read and one play session, not from every writer of the save
- notes: 

## hud

### hud.state — what the HUD's elements read and when it shows
- status: re-partial
- deps: items.buttons
- evidence: live writes seen on both screens' HUD for health, capacity, magic, rupees, wallet, button items, ammo and the minimap switch; buttonStatus, hudVisibilityMode and doAction read in play, a text box and a cutscene (z3d-mods runs ootre-save1, ootre-state, ootre-mm-flag); Interface_ChangeAlpha, TopHud_Update, TopHud_Draw and HudItemButtons_SetIcon read from the disassembly
- where: docs/hud.md, include/z3D.hpp, include/z3Dsave.hpp
- gap: the doubled magic meter, the 500 wallet and every doAction but CHECK and NONE are not run; InterfaceContext is named at four fields only; hudVisibilityMode's other values and writers are unread; gHud.state was not re-checked under each menu
- notes: 

### hud.owners — who draws the top screen's HUD under The One Screen, and the switches
- status: re-verified
- deps: hud.state
- evidence: each switch written before the code first ran and the piece gone in the frame, text boxes still drawn (runs ootre-off-hearts, ootre-off-scenes, ootre-off-labels, ootre-off-action, ootre-off-all, ootre-off-minimap2, ootre-off-everything)
- where: docs/hud.md
- gap: 
- notes: the payload addresses hold for The One Screen 891b60e only

## save

### save.write — Sram_WriteSave and the file's layout
- status: re-partial
- deps: 
- evidence: a write watch on the checksum during the pause screen's Save lands in Sram_WriteSave (0x2FDD00, 0x2FDD24; run oot-as1); disassembly 0x2FDAC8-0x2FDE08 and its three call sites; save00.bin from a save is 0x14DC bytes equal to gSaveContext, unused_6C included (oot-sv1)
- where: docs/save.md
- gap: the callers at 0x41CACC and 0x43E120 are not identified; the queue and the second file slot are unread
- notes: 

### save.request — the queued write is the caller's to free
- status: re-partial
- deps: save.write
- evidence: disassembly of 0x2E613C, Sram_IsWriteDone (0x2FDAA4), Sram_FinishWrite (0x2FDA7C) and the pause screen's poll at 0x43A3E0; a save without Sram_FinishWrite leaves a 0x120 block holding the request and its path in the system arena (z3d-mods runs oot-k4, oot-k5); with it the arena is the same on every visit of a scene over 17 saves (oot-k6)
- where: docs/save.md, docs/notes/arena.md
- gap: the worker's queue (0x306E40, 0x31B9C0) is unread; the file loader's cache (0x4476B8, 0x447748) is unread
- notes: 

## collision

### collision.budget — what BgCheck_Allocate gives a scene
- status: re-partial
- deps: none
- evidence: disassembly of BgCheck_Allocate (0x1311D0), BgCheck_InitializeStaticLookup (0x14DA40) and StaticLookup_AddPolyToSSList (0x3562C0); the truncated list length the formula gives is the one read live (39017 in Kakariko, 23568 in Ganon's Castle, 36989 with 9333 triangles); 42355 nodes used past that length with Link standing on the last triangles over three visits (z3d-mods run oot-n1); every ROM scene's triangle and cell pairs fit its list (z3d-mods `bgcheck.py`)
- where: docs/notes/scene_collision.md, include/z3Dbgcheck.hpp
- gap: the triangle and cell test is not decompiled: z3d-mods counts the pairs that touch, 0.2% under the game's count in three scenes; the fixed-camera branch was not seen run; the moving collision's lists are unread
- notes: 
