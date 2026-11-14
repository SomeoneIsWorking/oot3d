# Item buttons: equips, the item menu's assignment and the HUD

USA `code.bin`. Read from the disassembly and live memory (z3d-mods harness, File 1 as a child).

## Equips
`gSaveContext.equips` (`ItemEquips`, save +0x80): `buttonItems[5]` are B, Y, X, I, II;
`buttonSlots[4]` are the inventory slots behind Y, X, I, II (`SLOT_NONE` when empty). Using a
button reads the item from here every time. `childEquips` and `adultEquips` hold the other age's
set and trade places with `equips` when Link's age changes.

Writing `equips` alone changes what a button uses but not what the HUD draws.

## Ammo and its capacity
Using ammo clamps the count to the bag's capacity (live, z3d-mods run `oot-am2`, the Slingshot
written into `equips`, Kokiri Forest): with `inventory.upgrades` 0 (no Bullet Bag) a count of 30
reads 0 after one shot; with the Bullet Bag's field at 1 (`upgrades` 0x4000, holds 30) it reads
29. A Slingshot put into the inventory without its bag therefore empties on the first shot.

## The item screen's layout owns the buttons
`SaveContext.itemMenuChild` and `itemMenuAdult` (24 bytes each) are the item screen's nodes for
each age: 20 grid cells and the four item buttons (`ITEM_MENU_NODE_I` 5, `_X` 11, `_Y` 17, `_II`
23), each an inventory slot or `SLOT_NONE`. Assigning an item moves its slot from a grid node to a
button node.

`ItemMenu_ApplyLayout` (0x33C25C) runs when the item screen opens and after an assignment:
- For each inventory slot whose item differs from `itemSlotDataRecords[slot]`: a record of 0xFF
  (an item just obtained) puts the slot in the first empty grid node; any other record is only
  updated.
- For each node of the current age's layout (`linkAge == 0` reads `itemMenuAdult`) it draws the
  slot's item, and for a button node writes `equips` (the item is `inventory.items[slot]`; stores
  from 0x33C5F8, clears from 0x33C6A4) and calls `Hud_SetItemButton`: node 11 → `buttonItems[2]` and
  `buttonSlots[1]`, 17 → `[1]`/`[0]`, 5 → `[3]`/`[2]`, 23 → `[4]`/`[3]`.

So `equips` written behind the layout's back is overwritten the next time the item screen opens
(seen live: run oot-ch5, a write watch on `equips` while the screen opens).

## The item screen's cursor
`gItemMenuCursorNode` (0x506760, s32; the item screen's state word at 0x50672C + 0x34) is the
layout node the cursor is on, six to a row: the row's last is its button node. Live (z3d-mods
hud-oot-cur1, hud-oot-cur2): 0 when the screen opens on the first cell, 1 and 2 after moves
right, 8 after a move down from 2. The two words after it change with it (0 then 1, 2 then 6)
and the two after 0x50676C are the cursor's column and row. The item under the cursor is
`inventory.items[layout[node]]`. Not seen: the value on a button node, and after the screen
closes.

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

## Using any item through the X and Y buttons
An item written into `equips.buttonItems[2]` with its inventory slot in `buttonSlots[1]`, and
`PAD_X` set in `GameState.buttonsHeld` and `buttonsPressed` before `Play_Update` in the same
frame, is used as if it sat on X; `buttonItems[1]`, `buttonSlots[0]` and `PAD_Y` do the same for
Y. Live (z3d-mods runs ctl-slots3, ctl-slots4): Bombs and the Slingshot used this way with the
layout's own button nodes empty, the ammo counted down; the ocarina (run ctl-song3) comes out the
same way. The item stays in hand while its `buttonItems` byte stays.

With the item screen up, the circle pad moves its cursor and Start closes it (ctl-slots4).

## The item in hand
`Player.heldItemButton` (+0x1A8) is the index in `buttonItems` of the item in hand,
`heldItemId` (+0x1AA) its item (0xFF with none), `itemAction` (+0x1A9) and `heldItemAction`
(+0x1AC) what it is doing, 0 (`PLAYER_IA_NONE`) with empty hands (z3d-mods run ctl-held1: watched
through taking out and putting away Bombs, the Slingshot and the ocarina).

When the item in hand leaves its last `buttonItems` byte the game puts it away, and an item
button pressed before `itemAction` is back at 0 is dropped: a press in the same frame, the next
frame, or once `heldItemId` alone reads 0xFF does nothing (ctl-song6 to ctl-song8); pressed once
`heldItemId` is 0xFF and `itemAction` is 0 the new item comes out (ctl-song11).

## The pause screen's Save
Start in play opens "Save your progress?" with the cursor on Save; A saves, and "Quit" on the
prompt after it returns to the title screen (z3d-mods runs ctl-save2, ctl-save3).

## Unused save bytes
`SaveContext.unused_6C` (0x12 bytes at +0x6C) is zero in the files read. z3d-mods keeps its item
slots there; a file saved with them loads and plays without the patch (run ctl-save3).
