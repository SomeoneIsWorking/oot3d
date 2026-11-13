# Item buttons: equips, the item menu's assignment and the HUD

USA `code.bin`. Read from the disassembly and live memory (z3d-mods harness, File 1 as a child).

## Equips
`gSaveContext.equips` (`ItemEquips`, save +0x80): `buttonItems[5]` are B, Y, X, I, II;
`buttonSlots[4]` are the inventory slots behind Y, X, I, II (`SLOT_NONE` when empty). Using a
button reads the item from here every time. `childEquips` and `adultEquips` hold the other age's
set and trade places with `equips` when Link's age changes.

Writing `equips` alone changes what a button uses but not what the HUD draws.

## Assignment in the item menu
`FUN_0033c25c` handles a press on an item: it stores the item and slot at 0x33C5F8 and clears the
button that held it before at 0x33C6A4, then calls `Hud_SetItemButton`. Its panel nodes map to
equips as 11 → `buttonItems[2]`, 17 → `[1]`, 5 → `[3]`, 23 → `[4]`.

## HUD
`Hud_SetItemButton(button, item)` (0x2EFEA0) redraws one button's icon on `gHud.itemButtons`
(`ITEM_NONE` empties it). `button`: 0 the Ocarina button, 1 I, 2 II, 3 the X lane
(`buttonItems[2]`), 4 the Y lane (`buttonItems[1]`). The item menu calls it while its own screen
is up.

`gHud.state` (0x50AF68) reads `HUD_STATE_GAMEPLAY` (2) in play and 0 while the items, gear or
system screen is up; those screens' own state words (0x50672C items, 0x504484 gear, 0x50A530
system; 0x506CE8 for the map, not seen up) read 2 while theirs is up. The One Screen (MIT) names
the same words in its source.

## Pad
`GameState.buttonsHeld` (+0x14) is the HID pad word. Bit 12 (`PAD_DEBUG`) reaches it when Azahar's
Debug button is held; bit 13 (Azahar's GPIO14) does not. ZL and ZR are not in this word: the game
never reads `ir:rst`.

## Unused save bytes
`SaveContext.unused_6C` (0x12 bytes at +0x6C) is zero in the files read. z3d-mods' chord items keep
their second item page there.
