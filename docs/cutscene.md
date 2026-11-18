# Cutscenes and the textbox

Findings from z3d-mods (its Start cutscene skip), checked on the USA build in Azahar.

## Running a script
- `CutsceneHandler_RunScript` (0x321F50) runs `play->csCtx` each frame. It calls
  `Cutscene_ProcessCommands(play, csCtx, script)` (0x2C5BA0) twice: a `bl` at +0xF0 inside the
  catch-up loop and a tail `b` at +0x120.
- `CutsceneContext`: `state` +0x08 is 2 while a script runs; `endFrame` +0x18 is copied from the
  script header's end frame on every call; `frames` +0x20 is the current frame.
- Script header: magic `" BDQ"`, version 3, command count at +0x08, end frame at +0x0C
  (`notes/cutscene_format.md`).
- `Cutscene_Command_Terminator` (0x491364) takes the destination. Its own title-demo skip requires
  `*(u32*)(0x5043D4 + 0x18) == 0`, and that word is 1 during every gameplay cutscene.

## The camera's shots
- The N64 point-list commands (1, 2, 5, 6) keep their logic: each takes its list when its start
  frame is past the list's applied frame (`camEyeSplinePointsAppliedFrame` +0x28 for eye lists,
  `camAtSplinePointsAppliedFrame` +0x2A for at lists), sets `camEyeReady`/`camAtReady`
  (+0x35/+0x34) and `camEyePoints`/`camAtPoints` (+0x3C/+0x38), and once both are ready stores the
  start frame as applied, switches the cutscene camera (`subCamId` +0x24) to setting 0x25 and
  calls 0x2C4130 with (camera, at points, eye points, the player, 0): `Camera_SetCSParams`. The
  at-list commands also store the shot in `camShot` (+0x84).
- Grezzo's 0x97 (`notes/cutscene_format.md`) parses its "ccb" payload into `camSpline` (+0x88)
  and finds the segment whose frames cover `frames`. When that is not `camShot`, it stores it there
  (-1 for none) and restarts the spline's frame counter at the cutscene's frame; otherwise the
  counter steps by one. It evaluates the segment into +0x94 (0x33CB90) and hands the pose to the
  cutscene camera (0x33CB1C).
- So a shot starts exactly when `camShot` or an applied frame changes. Read live in the Kakariko
  intro (warp 0xDB from Kokiri Forest): `subCamId` 1, the active camera 1, `camShot` 0 from the
  start, 1 from about frame 190, the camera cutting from the village to the graveyard gate.

## Text inside a cutscene
- A text command (0x13) holds its end frame while `Message_GetState()` (0x3769D8) is not 0, 2, 7
  or 8 (the check at 0x2C7AD4).
- `Message_CloseTextbox(play)` (0x3725E0) closes the box. The message update finishes closing on a
  later frame: manager byte +0xF38 becomes 0x0E and the counter at +0xFA4 drops below 2. Calling it
  again every frame resets that counter and the cutscene stalls.
- `TEXT_STATE_CHOICE` (4) waits for the player's answer. A box reaches it only after its text has
  printed (manager mode 7 in the state getter at 0x2C2700; 3 while printing), so a choice cannot be
  told from the message on the frame it opens. The script's text entry says so up front:
  `CsCmdText.type` is `CS_TEXT_CHOICE` (1), with `altTextId1`/`altTextId2` shown after the answer
  (`include/z3Dcutscene.hpp`). The text command is inlined in `Cutscene_ProcessCommands` (0x2C79A4).

## Play state
- `GameState.buttonsPressed` (+0x18) holds the buttons pressed this frame; bit 3 is Start.
- `PlayState.sceneLoadFlag` (+0x5C2D) is 20 when a scene transition is requested, -20 during the
  incoming fade (through the opening narration's black screen to about frame 160), 0 otherwise.
- `gGameInfo` (0x51B2F4) points at the `GameInfo` whose `R_UPDATE_RATE` (`SREG(30)`, +0x110) is
  the frame-rate divisor.
- `SaveContext.fileNum` 0xFEDC is the debug file; `gameMode` 0 is normal play.

## Checking it
Kakariko intro from Link's house (warp 0xDB) and the opening narration (nextCutsceneIndex 0xFFF1
into Link's house): ending the script on Start through `Cutscene_ProcessCommands` lands where
watching it lands, and the save context differs only in `naviTimer` (+0x4C) and the counter at
+0x15B4.
Deku Tree after Gohma (nextCutsceneIndex 0xFFF1 into Kokiri Forest, warp 0xEE): Start at frame 60
runs to the choice at 499 ("wouldst thou listen", text 0x1092), which waits; after either answer
the skip lands in the next script (scene 71), whose save context at its frame 100 differs from
watching only in +0x15B4.
