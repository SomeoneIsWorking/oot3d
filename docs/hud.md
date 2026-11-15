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
| Ammo count | `SaveContext.inventory.ammo[slot]` (0x5879FE + slot, s8): sticks 0, nuts 1, bombs 2, bow 3, slingshot 6, bombchus 8, beans 14; the bow's count also for the three arrows | live for sticks 5, nuts 7, bombs 9, slingshot 17 on both screens (ootre-state, kokiri), and for the bow 30 and 29, bombchus 17 and 50, beans 7 and 15 (car6) |
| Count at its most | `HudItemButtons_UpdateAmmo` (0x2F8160) flags a button's count when it equals `CUR_CAPACITY` of the item's upgrade (sticks, nuts, bombs, the bow and its three arrow combinations 0x38 to 0x3A, slingshot), 50 for `ITEM_BOMBCHU` (0x2F83E0) and 15 for `ITEM_BEAN` (0x2F83F0), and hands the flag to the count's draw (0x2FCB04) | read from the code; z3d-mods draws the same rule (car7: bow 30 in a 30 quiver and 15 beans green, 17 bombchus white) |
| Carrots | `PlayState.interfaceCtx.numHorseBoosts` (play +0x2E18, u8): the boosts left, 6 (`HORSE_BOOSTS`) when rested. `EnHorse_Update` copies the horse's own count (EnHorse +0xE9C, u8) there at 0x14AFBC while the horse is ridden (EnHorse +0xE70 is 1). `Hud_UpdateStatusLayout` (0x444544) lights carrot n of six (quads 86 to 91) while the count is at least n, none at 0 | live: 6 at rest, 5 and 4 after one and two boosts with A, 6 again 20 seconds later (car4, car5: a write watch on the byte) |
| Carrots shown | `gHud.statusLayout` (0x50AF98, u32) is `HUD_STATUS_MOUNTED` (2). `Hud_UpdateStatusLayout` sets it when `Player.stateFlags1` has 0x800000 (mounted) and no minigame is on, moves the magic meter and hearts (quads 59 to 82) 400 pixels right, off the touch screen, and brings the carrots to their place; it puts them back when the value leaves 2 | live: 2 on Epona, 0 after dismounting (car6) |
| Button enabled or dim | `SaveContext.buttonStatus[0..4]` (+0x156F, 0x588EC7): B, Y, X, I, II; `BTN_ENABLED` 0, `BTN_DISABLED` 0xFF. The game writes it every frame | live: all 0xFF in Link's house with the items dim (ootre-save1, status: a write of 0 is 0xFF again on the next read); all 0 in Kokiri Forest with the same items at full brightness on both screens (ootre-state, kokiri) |
| A button's label | `gTopHud.doAction` (0x4FC694, s32): a `DO_ACTION_*`, -1 with no label. `TopHud_Update` copies it from `PlayState.interfaceCtx.doAction` (play +0x2DCA, u16), where no label is `DO_ACTION_NONE` | live: 1 beside the bed with "Check" on the button, -1 (interface 10) in Kokiri Forest with no label (ootre-state, house and kokiri); 3, 12, 15 and 16 in play (hud-ootlive3); the number is the word's cell in `hud_all00.ctxb` ("Item icons" below) |
| Minimap on | `gTopHud.minimapOn` (0x4FC6B0, s32), the player's switch (`TopHud_ToggleMinimap` 0x2F009C flips it and plays a sound; its one caller is 0x42E654, not identified), and `gMinimap.layout` (0x4FDA84) not NULL: the scene has a map | live: 0 written, the map and its arrow are gone on the next frame; 1, back (ootre-mm-flag). `layout` is NULL in Link's house and set in Kokiri Forest (ootre-state) |

Not found: a field for the small-key count's visibility; The One Screen reads
`inventory.dungeonKeys[mapIndex]` (0x587A2C + `SaveContext.mapIndex`) in scenes 3 to 16.

## Mounted
Seen on Epona in Hyrule Field (z3d-mods car4 to car7: an adult file with `eventChkInf[1]` bit 8
set, entrance 0x028A, which starts Link on her over Lon Lon Ranch's fence; the landing cutscene
ends by itself):
- `Player.stateFlags1` (+0x1710) has 0x800000 and `Player.rideActor` (+0x12B8) is the horse.
- `SaveContext.hudVisibilityMode` is `HUD_VISIBILITY_A_B_MINIMAP` (12), not 50. The interface's
  alphas (InterfaceContext +0x280 on) read 0xFF for A and B, 0 for the four item buttons
  (`buttonAlphas`) and 0xFF for `topHudAlpha`.
- `equips.buttonItems[0]` (B) is `ITEM_BOW` with a bow owned, and `buttonStatus[0]` keeps the item
  that was there (0x3C, the Master Sword's, read back); both return on dismounting.
  `buttonStatus[1..4]` are 0xFF.
- `InterfaceContext.doAction` is `DO_ACTION_FASTER` (8) while moving and `DO_ACTION_DOWN` (13)
  standing.
- `Hud_UpdateStatusLayout` chooses the touch screen's status layout and the top screen's B button
  together: `TopHud_SetBButtonMode` (0x2E64FC) with 1 for a shooting gallery
  (play +0x5C74 at least 2), 2 for bowling (scene 75 with switch 0x38), 3 for play +0x2E40 not 0,
  4 for the u16 at 0x588EEC being 1, 5 when mounted, 0 otherwise. Modes 1 to 4 set
  `statusLayout` to `HUD_STATUS_MINIGAME`.
- The top screen then shows a ring with the B glyph and, with a bow, its icon and the arrows left.
  The ring with its glyph is quad 7 of `gTopHud.quads` (48x47 at 312, 192; ring1 to ring3: the
  batch's positions and per-quad offsets read on foot and mounted). `TopHud_UpdateButtons`
  (0x42B194, called by `TopHud_Update` at 0x41DF70) gives it the offset (0, 0) while
  `gTopHud.bButtonMode` is not 0 and FUN_002fcdd4 returns 0, and (420, 240), off the screen,
  otherwise: the `beq` at 0x42B1D4 as a plain branch keeps it off the screen in every mode.
  The icon and count are `gTopHud.bButton`, drawn by `HudItemButtons_Draw` (0x2F780C) from
  `TopHud_Draw` at 0x42BE28; that call as a `nop` leaves the ring alone (car7). With both, the
  ring, glyph, icon and count are gone and the minimap's markers stay (z3d-mods hud-mounted).
- The batch's other quads: 0 (38x38) and 1 (the 64x18 word), which The One Screen keeps off the
  screen; 2 and 3, the minimap's player and entrance markers; 4 a second word; 5 and 6 at the
  bottom middle, not identified. The batch is drawn by the object at `gTopHud` +0x20 (0x42BC30),
  so a `nop` there takes the markers too, and so does mode 0 for 5 at 0x444628
  (car9-42BC30, car10).

## The B button in a minigame
`TopHud.bButtonMode` (`B_BUTTON_*`) says what the top screen's B button holds; `buttonItems[0]`
stays as it was in the minigames (0xFF read in the shooting gallery on a file with no sword).
`TopHud_SetBButtonMode` builds `gTopHud.bButton` with `HudItemButtons_Init` (0x2F8EE4), whose
kind (mode + 1, at +0x428) picks the icon, and `TopHud_Update` (0x41E1C8) writes the count (+0x42C)
each frame:

| Mode | Set when (`Hud_UpdateStatusLayout`) | Icon | Count |
|---|---|---|---|
| `B_BUTTON_SHOOTING_GALLERY` | `PlayState.shootingGalleryStatus` at least 2 | `ITEM_SLINGSHOT` as a child, `ITEM_BOW` as an adult | `shootingGalleryStatus` - 1 |
| `B_BUTTON_BOWLING` | scene 75 with switch 0x38 | `ITEM_BOMBCHU` | `PlayState.bombchuBowlingStatus` |
| `B_BUTTON_FISHING` | `PlayState.fishingPoleOut` not 0 | `ITEM_FISHING_POLE` | none |
| `B_BUTTON_HORSEBACK_ARCHERY` | `SaveContext.minigameState` is 1 | `ITEM_BOW` | `InterfaceContext.hbaAmmo` |
| `B_BUTTON_MOUNTED` | `stateFlags1` mounted, a bow owned | `ITEM_BOW` | the save's arrows, green at the quiver's capacity |

The minigames' counts are never drawn green. Seen live in the Market's shooting gallery as a
child (entrance 0x016D, the owner paid): mode 1, `shootingGalleryStatus` 16 with 15 shots, 14
after two (z3d-mods hud-gallery). The other three minigames are read from the code only.

## The view icon
Quad 28 of the touch screen's layout. `Hud_UpdateGameplay` (0x42DF18) picks its cell of
`menu_top_parts00.ctxb` at 0x42F400, the first that holds:

| Cell | When |
|---|---|
| Navi (176, 0) | `InterfaceContext.naviCalling` not 0 |
| The camera, "BACK" (176, 48) | `Player.viewMode` is `PLAYER_VIEW_FIRST_PERSON` |
| The camera, "VIEW" (128, 48) | `R_SCENE_CAM_TYPE` is `SCENE_CAM_TYPE_FIXED_TOGGLE_VIEWPOINT` |
| The eye, "VIEW" (128, 0) | otherwise |

It sets `HUD_FLAG_VIEW_DIM` in `gHud.flags` at 0x42E180 while the icon takes no press, and
`Hud_UpdateDimming` (0x444A90) draws the quad at 0.3 then (and dims the item buttons by
`buttonStatus`). Seen (z3d-mods view1 to view3, hud-view): the camera in Link's house (cam type
0x20, View changes the camera); the eye in Kokiri Forest, `viewMode` 1 in the first-person look
and 0 after; `naviCalling` 1 once `SaveContext.naviTimer` passed 600 there; the flag set on Epona
and in the bowling alley, clear in Kokiri Forest, Hyrule Field and Link's house.

## Writing a button's item by hand
`equips.buttonItems` is what a button uses and what a HUD should read, but it is not where an
item is put: the item screen's layout owns it (`docs/item_buttons.md`). A byte written there
alone, seen in z3d-mods runs:
- stays while nothing applies the layout (hud-rebased2: Y and X written, read back a second
  later and drawn);
- is gone once the layout is applied. Written right after new items were written into
  `inventory.items`, the buttons were empty a second later (hud-rebased, the frame only; the
  bytes were not read back): `ItemMenu_ApplyLayout` places an item it has no record of and
  rewrites `equips` from the layout's button nodes;
- is gone after z3d-mods' chord trade, which writes `equips` from the layout's nodes
  (hud-ootlook3: X and Y empty after the chord's release).
To put an item on a button from outside, write its inventory slot into the age's layout node
(`itemMenuChild` or `itemMenuAdult`) and let the game apply it, or assign it in the item screen:
Select opens the map screen, a touch on its Items tab (226, 228) the item screen with the cursor
on the first item, and the 3DS's X or Y puts the item under the cursor on a button
(hud-ootlive3). Under The One Screen the 3DS's X assigns `buttonItems[1]`, the Y lane, and its Y
`buttonItems[2]`: the mod trades the two buttons in the menu as it does in play.

## When the play HUD shows
All of:
- `SaveContext.gameMode` (0x588E3C) is 0: a loaded file, not the title or file select.
- `SaveContext.hudVisibilityMode` (+0x157A, 0x588ED2, u16) is `HUD_VISIBILITY_ALL` (50), or
  `HUD_VISIBILITY_A_B_MINIMAP` (12) while mounted ("Mounted" above).
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

The A button's words are `hud_all00.ctxb` (256x256, slot 1): cells of 64x18 pixels from (0, 93),
nine to a column, and a `DO_ACTION_*` is its word's cell counted down each column. The cells: 0
Attack, Check, Enter, Return, Open, Jump, Decide, Dive, Faster; 9 Throw, Navi, Climb, Drop, Down,
Save, Talk, Next, Grab; 18 Quit, Put Away, Reel, Swap, Equip (`DO_ACTION_WORDS` 23); the digits 1
to 8 fill the fourth column, cells 27 to 34. Seen with the number read and the action in the
frame (z3d-mods hud-ootlive3): 3 with the Slingshot aimed (Return), 12 with a bomb held (Drop),
15 at the shop's counter (Talk), 16 in the shop's text (Next). Swap, Equip and the digits were
not seen.

## The touch screen HUD's art
`menu_top_parts00.ctxb` (512x256, slot 2 of `gMenuTextures`: the third name in the table at
0x4D4010) holds the touch screen HUD's small art. Cells in the sheet's pixels, measured on the
decoded sheet (the layout that names them was not read):

| Art | Cell (x, y, w, h) |
|---|---|
| Rupee, green only | 128, 96, 18, 18 |
| Small key | 153, 96, 16, 18 |
| Carrot, whole | 488, 0, 24, 24 |
| Carrot, spent (grey) | 488, 24, 24, 24 |
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
| The minimap raised while mounted | `Minimap_Project` (`source/hud/minimap.c`): `dy` centres the map on row 156 while `Retail_Mounted` | 0x6148C8: `0A000092` (`beq`, mounted) to `E320F000`. Frames: z3d-mods hud-minimap, riding and on-foot, the map in one place |
| Minimap and its markers | the game: `TopHud_Draw` and the marker update at 0x42A908, both reading `minimapOn`; The One Screen only moves it (`tos_minimap` at 0x41EB04) | 0x42BB64: `E5970068` to `E3A00000`, and 0x42A908: `E59A0068` to `E3A00000` (`mov r0, #0` for the load); the player's switch stays readable. Frame: ootre-off-minimap2 |

Everything off, with Kokiri Forest in play and the Deku Tree's cutscene text still drawn:
ootre-off-everything. A `nop` of 0x42BB90 alone leaves the red arrow (ootre-mm-code2); a `nop` of
0x41EB04 puts the map back at the game's own place, top left (ootre-off-minimap).

The 0x61xxxx addresses are The One Screen 891b60e's payload: they hold for that build only, so a
patch that uses them names the expected word.

How the switches were shown: the harness's `poke` does not drop translated code, so each word was
written as the run's first step, before the game had run it.
