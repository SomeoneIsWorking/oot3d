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

## Text inside a cutscene
- A text command (0x13) holds its end frame while `Message_GetState()` (0x3769D8) is not 0, 2, 7
  or 8 (the check at 0x2C7AD4).
- `Message_CloseTextbox(play)` (0x3725E0) closes the box. The message update finishes closing on a
  later frame: manager byte +0xF38 becomes 0x0E and the counter at +0xFA4 drops below 2. Calling it
  again every frame resets that counter and the cutscene stalls.
- `TEXT_STATE_CHOICE` (4) waits for the player's answer.

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
