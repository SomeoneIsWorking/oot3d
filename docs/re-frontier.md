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
- evidence: live read of play+0x364 in Kokiri Forest: play (+0xD4) is the play state, |at-eye| matches dist (+0x124), playerPosRot (+0xDC) follows Link, camDir/inputDir (+0x182/+0x17C) turn as he walks, status 7, setting 1; Camera_Update (0x2D84C4) loads +0xD4 and checks +0x188
- where: include/z3Dcamera.hpp
- gap: mode, camDataIdx and prevSetting only read at rest; animState, behaviorFlags, stateFlags and the distortion fields not yet checked; Camera_BGCheckInfo, Quake_Update and Camera_GetCamDataId unchecked
- notes: the OoT3D Randomizer's layout (no licence) was the starting guess; only fields seen live are named
