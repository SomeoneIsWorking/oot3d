# HUD

What each piece of the game's HUD reads, for a HUD drawn from the game's state, and who draws the
HUD of z3d-mods' OoT3D set (The One Screen 891b60e over the USA `code.bin`). Addresses are for
that build; `gSaveContext` is 0x587958. Runs are z3d-mods harness runs (`scratch/runs/<name>`),
File 1 as a child; a frame was read for every "frame" below.

## What each element reads

| Element | Reads | Proven |
|---|---|---|
| Heart containers | `SaveContext.healthCapacity` (+0x42, 0x58799A, u16), 0x10 a heart | live: 0xA0 written, ten hearts on both screens (ootre-save1, hearts) |
| Filled hearts | `SaveContext.health` (+0x44, 0x58799C, s16), 0x10 a heart | live: 0x50 written, five of ten red (ootre-save1, hearts) |
| Magic meter shown | `SaveContext.magicAcquired` (+0x4E, 0x5879A6, u8) | live: 1 written, the meter appears on both screens; the game then sets `magicLevel` (+0x46, 0x58799E) to 1 and `magicMeterSize` to 0x30 (ootre-save1, magic-acquired) |
| Magic amount | `SaveContext.magic` (+0x47, 0x58799F, s8) | live: 0x18 written, the meter is half full (ootre-save1, magic-half) |
| Magic capacity | `SaveContext.magicMeterSize` (+0x1584, 0x588EDC, u16): 0x30 | live for 0x30 only; the doubled meter (`doubleMagic` +0x50, `magicLevel` 2, 0x60) is the N64's and was not run |
| Rupees | `SaveContext.rupees` (+0x48, 0x5879A0, s16) | live: 123 written, both counters read 123 (ootre-save1, rupees) |
| Wallet size | `(inventory.upgrades & gUpgradeMasks[UPG_WALLET]) >> gUpgradeShifts[UPG_WALLET]` (upgrades 0x587A10; mask 0x3000, shift 12) indexes `gUpgradeCapacities[UPG_WALLET]` (0x53CC04): 99, 200, 500, 500 | live: `rupeeAccumulator` 300 stops the counter at 99, and at 200 with the field at 1 (ootre-save1, wallet0 and wallet1); `Hud_UpdateRupeeColor` (0x2EFD88) compares `rupees` with this capacity; 500 read from the table only |
| Item on a button | `SaveContext.equips.buttonItems[0..4]` (0x5879D8): B, Y, X, I, II; `ITEM_NONE` empty | live: five items written, The One Screen draws them at once (ootre-save1, items); the touch screen follows after a scene load when the item menu's layout agrees (ootre-state, kokiri; `docs/item_buttons.md`) |
| Ammo count | `SaveContext.inventory.ammo[slot]` (0x5879FE + slot, s8): sticks 0, nuts 1, bombs 2, bow 3, slingshot 6, bombchus 8, beans 14; the bow's count also for the three arrows | live for sticks 5, nuts 7, bombs 9, slingshot 17 on both screens (ootre-state, kokiri); the others follow the N64's slots |
| Button enabled or dim | `SaveContext.buttonStatus[0..4]` (+0x156F, 0x588EC7): B, Y, X, I, II; `BTN_ENABLED` 0, `BTN_DISABLED` 0xFF. The game writes it every frame | live: all 0xFF in Link's house with the items dim (ootre-save1, status: a write of 0 is 0xFF again on the next read); all 0 in Kokiri Forest with the same items at full brightness on both screens (ootre-state, kokiri) |
| A button's label | `gTopHud.doAction` (0x4FC694, s32): a `DO_ACTION_*`, -1 with no label. `TopHud_Update` copies it from `PlayState.interfaceCtx.doAction` (play +0x2DCA, u16), where no label is `DO_ACTION_NONE` | live: 1 beside the bed with "Check" on the button, -1 (interface 10) in Kokiri Forest with no label (ootre-state, house and kokiri); the other values are the N64's numbers in the order of `hud_all00.ctxb`'s word cells, not run |
| Minimap on | `gTopHud.minimapOn` (0x4FC6B0, s32), the player's switch (`TopHud_ToggleMinimap` 0x2F009C flips it and plays a sound; its one caller is 0x42E654, not identified), and `gMinimap.layout` (0x4FDA84) not NULL: the scene has a map | live: 0 written, the map and its arrow are gone on the next frame; 1, back (ootre-mm-flag). `layout` is NULL in Link's house and set in Kokiri Forest (ootre-state) |

Not found: a field for the small-key count's visibility and the mounted carrots; The One Screen
reads `inventory.dungeonKeys[mapIndex]` (0x587A2C + `SaveContext.mapIndex`) in scenes 3 to 16.

## When the play HUD shows
All of:
- `SaveContext.gameMode` (0x588E3C) is 0: a loaded file, not the title or file select.
- `SaveContext.hudVisibilityMode` (+0x157A, 0x588ED2, u16) is `HUD_VISIBILITY_ALL` (50).
  `Interface_ChangeAlpha` (0x34BE04) is its only writer read so far. Live: 50 in play, 5 with a
  text box up, 2 in a cutscene (ootre-state: house, dialog, cutscene). The game's own top HUD and
  The One Screen's are gone in the dialog and cutscene frames.
- `gHud.state` (0x50AF68) is `HUD_STATE_GAMEPLAY` (2): no items, gear, map or system screen
  (`docs/item_buttons.md`; not run again here: the menu presses of ootre-state landed in a text box).

The same states by other fields, as a cross-check: `gMessageManager.boxMode` (0x5C27B0) is not 0
while a text box is up (0 in play, 6 and 7 in the bed's text); `PlayState.csCtx.state` (play
+0x22A0) is not 0 in a cutscene (2 in the Deku Tree's); `InterfaceContext.topHudAlpha` (play
+0x2E30) is 0 in a cutscene and 0xFF in play and text. These three sit behind `gPlayState` except
the first.

## Item icons
`rom:/menu/<language>/icon_item_menu00.ctxb` (USA English: `menu/01_US_ENGLISH/`), 512x512, slot
`MENU_TEXTURE_ITEM_ICONS` (10) of `gMenuTextures` (0x55B490, `MenuTexture_Get` 0x2E11D0; the names
are the table at 0x4D4010). An item's icon is the 42x42 cell at column `id % 12`, row `id / 12`:
`HudItemButtons_SetIcon(layout, button, item)` (0x2F8D74) computes exactly that, with no table,
and `ITEM_NONE` empties the button. The decoded sheet's first rows are the item enum in order
(stick, nut, bomb, bow, fire arrow, Din's Fire, slingshot, the two ocarinas, bombchu, hookshot,
longshot; then ice arrow on). Henriko's pack replaces the sheet whole (4096x4096, found by hash).

The A button's words are `hud_all00.ctxb` (256x256, slot 1).

## The touch screen HUD's art
`menu_top_parts00.ctxb` (512x256, slot 2 of `gMenuTextures`: the third name in the table at
0x4D4010) holds the touch screen HUD's small art. Cells in the sheet's pixels, measured on the
decoded sheet (the layout that names them was not read):

| Art | Cell (x, y, w, h) |
|---|---|
| Rupee, green only | 128, 96, 18, 18 |
| Small key | 153, 96, 16, 18 |
| Skull token, chest | right of the key, same row |
| Heart, plain: empty, a quarter, half, three quarters, full | 241, 81 + 16n, 10, 10 for n = 0 to 4 |
| Heart with a white rim, the same five the other way up | 241, 1 + 16n, 10, 10: empty at the top |

The sheet has one rupee: the wallet does not change the gem. The count's colour does:
`Hud_UpdateRupeeColor` (above) turns it green at the wallet's capacity, and `num_all00.ctxb`
(slot 9) holds the digits in white, green, red and yellow. Henriko's pack replaces both sheets
whole (4096x2048 and 2048x1024, found by hash).

## Who draws the top screen's HUD
The game's own top screen HUD is `gTopHud` (0x4FC648): `TopHud_Update` (0x41DECC) and
`TopHud_Draw` (0x42B9D0). `TopHud_Draw` returns early unless `gameMode` is 0 and `hidden` is 0,
and skips the label, the markers and the minimap while a text box, a cutscene or a menu is up. It
draws the minimap (`bleq 0x441F08` at 0x42BB90, when `minimapOn`), then `node78` when `drawNode78`
(0x42BCC4 and 0x42BE64, one per pass), then the quads holding the A label and the map's player
and entrance markers (0x42BCE0, 0x42BCF0). `Hud_Update` (0x41E944) is the touch screen's HUD and
also runs `Minimap_Update` (0x42C0A4).

The One Screen (MIT, evidence only; `source/hud/hud.c`, `source/patch/hooks.s`) hangs its HUD on
those:

| Piece | Drawn by | Switch (bytes are instruction words, expected then new) |
|---|---|---|
| Hearts | the game's `node78`, whose vertex streams The One Screen fills in its `Hud_Submit` (0x612C14, called from `Hud_Update` at 0x41EABC) | 0x612D10: `0A0000B4` to `EA0000B4` (skip the fill; the hearts stay off screen). Frame: ootre-off-hearts |
| Magic meter, rupee icon, key icon, carrots, the view icon | its `native` scene, drawn in its `Hud_Draw` (0x6145F4, after `node78` at both sites) | 0x61465C: `EB000C5D` to `E320F000` |
| Button rings, the D-pad cross | its `chrome` scene | 0x614664: `EB000C5B` to `E320F000` |
| Item icons | its `icons` scene | 0x61466C: `EB000C59` to `E320F000`. The three together: ootre-off-scenes (hearts, lettering and minimap remain) |
| LB/RB/X/Y/A/B letters, ITEMS and GEAR, rupee, key and ammo digits, the A label | its `labels` scene | 0x614674: `EB000C57` to `E320F000`. Frame: ootre-off-labels |
| The A label alone | a copy of the game's label quad in `labels`; the game's own quads are moved off screen by its hook at 0x41E250 | 0x613604: `1A00003A` to `EA00003A`. Frame: ootre-off-action ("Check" gone, the A stays) |
| All of the above | `visible` in `Hud_Submit` | 0x612C84: `C3A00001` to `E3A00000`. Frames: ootre-off-all (play, then the bed's text box drawn as before) |
| Minimap and its markers | the game: `TopHud_Draw` and the marker update at 0x42A908, both reading `minimapOn`; The One Screen only moves it (`tos_minimap` at 0x41EB04) | 0x42BB64: `E5970068` to `E3A00000`, and 0x42A908: `E59A0068` to `E3A00000` (`mov r0, #0` for the load); the player's switch stays readable. Frame: ootre-off-minimap2 |

Everything off, with Kokiri Forest in play and the Deku Tree's cutscene text still drawn:
ootre-off-everything. A `nop` of 0x42BB90 alone leaves the red arrow (ootre-mm-code2); a `nop` of
0x41EB04 puts the map back at the game's own place, top left (ootre-off-minimap).

The 0x61xxxx addresses are The One Screen 891b60e's payload: they hold for that build only, so a
patch that uses them names the expected word.

How the switches were shown: the harness's `poke` does not drop translated code, so each word was
written as the run's first step, before the game had run it.
