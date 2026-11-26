# Pause screens

The touch screen's pause screens (items, gear, map, and the save prompt Start opens in play) and
how they are placed, for moving them on the top screen under The One Screen 891b60e (USA
`code.bin`). Runs are z3d-mods harness runs (`scratch/runs/<name>`), File 1 saved, in Link's house.

## Their state

Each screen has a state word: `MENU_SCREEN_CLOSED` (0), 1 while it opens, `MENU_SCREEN_UP` (2),
3 while it closes. `gItemMenuState` (0x50672C), `gGearMenuState` (0x504484), `gMapMenuState`
(0x506CE8, at +0x38 of the map screen's object at 0x506CB0) and `gSaveMenuState` (0x50A530, at
+0x28 of the save screen's object at 0x50A508). The function at 0x425140 runs the save screen by
its state: 0x4392A8 while up, 0x43AE94 while closing; 0x43AC88 opens it. The One Screen reads the
same four words (`RETAIL_ITEMS`, `RETAIL_GEAR`, `RETAIL_MAP`, `RETAIL_SYSTEM`) to draw the touch
screen on the top screen while one is not 0.

## Their quads

Each screen's quads get their corners when it opens, as file select's do (docs/file_select.md):
quad i is its size at its place in the screen's position table, in the touch screen's pixels
from the top left. The tables are initialised data, so a patch of the executable moves a quad.
Found from the batches' corners in the heap with each screen up (the batch's count, then its
corners' pointer at +0xC) and the table that holds the same places.

| Screen | Positions | Quads | BACK lit | BACK |
|---|---|---|---|---|
| Items | `gItemMenuQuadPositions` (0x5067B8) | 37 | 3 to 5 | 26, 27 |
| Gear | `gGearMenuQuadPositions` (0x5044DC) | 41 | 25 to 27 | 30, 31 |
| Map, overworld | `gMapMenuQuadPositions` (0x5073A0) | 30 | 7 to 11 | 19, 20 |
| Map, the other layout | `gMapMenuOtherQuadPositions` (0x506D60) | 50 | 10 to 12 | 39, 40 |
| Save | `gSaveMenuQuadPositions` (0x50A54C, sizes follow at 0x50A7BC) | 78 | 15 to 17 | 38, 39 |

The item screen's sizes follow its positions at `gItemMenuQuadSizes` (0x5068E0). Its panel is
quads 0 and 1 (`ITEM_MENU_QUAD_PANEL`), 124x200 each at (4, 0) and (128, 0), the second's cell
mirrored; its cell has a lit bevel along the top only. Under it the tabs' bar is quads 6 to 8
(`ITEM_MENU_QUAD_TAB_BAR`): an 8 wide end at (64, 202), a one-texel cell stretched 176 wide at
(72, 202) and the end mirrored at (248, 202), with BACK lit from 0 to 58.

BACK lit is a 50x38 and an 8x38 quad at (0, 202) and (50, 202) over a 56-wide one at (0, 204);
the overworld map cuts that last one into strips 4, 16 and 16 tall (quads 9 to 11). BACK is 40
and 16 wide at (0, 204), the save screen's at (0, 206). The map screen's object holds both
layouts' batches, the overworld's at +0x10 and the other's at +0x14: moving the first's corners
moved the overworld map's frame, tabs and BACK; moving the second's moved nothing there. Inside
the Deku Tree the map's BACK moves with the other layout's table (pages2), so it is likely the
dungeon map's. `MapMenu_Init` (0x46AC10, called by `PauseMenus_Init` 0x45596C) builds them once;
opening the map does not build them again (z3d-mods watch on `gMapMenu` +0x10, mapwatch). Each has
the item screen's tabs (`MAP_MENU_QUAD_TABS`, `_TAB_FRAMES`, `_TAB_BAR`), and `MapMenu_Update`
(0x42D0C0) hit-tests BACK and the tabs with `Touch_InRect` as the item screen does
(`MAP_MENU_*_TOUCH_CALL_*`). The gear screen is the same: `GearMenu_Init` (0x4670E4) builds its
batch (`gGearMenu` +0x0C) once, its sizes are `gGearMenuQuadSizes` (0x50463C, three words after its
positions' end), its tabs `GEAR_MENU_QUAD_TABS`, and `GearMenu_Update` (0x424300) hit-tests them
(`GEAR_MENU_*_TOUCH_CALL_*`).

The item screen's quads 12 to 24 are the item buttons' column right of the grid (x 256 to 320):
12 the stone under it all, 13 to 18 the I and II squares at the top and bottom (17 and 18 the
buttons, 52 wide), 19 to 21 and 22 to 24 the two round buttons at y 70 and 122 (48 wide); 0 and 1
are the grid's frame, 6 to 11 and 28 to 36 the tabs.

The buttons' letters are not in that table. They are `gItemMenu.buttonLetters` (`ItemButtonLetters`,
the object at +0x30 of the item screen's object 0x5066F8), four quads of a 128x128 sheet in the
`QuadBatch` inline at its +0x48: X at (270, 67), Y at (260, 119), I at (270, 8) and II at (270, 192),
11 to 13 pixels square (`ITEM_BUTTON_LETTER_*`). Their rects are set once, from integers in the
code, so their places are in no data table; `ItemButtonLetters_Update` (0x2F94A8) runs the object
each frame (`ItemMenu_Update`, 0x433A90, calls it beside `HudItemButtons_UpdateAmmo` for the grid
and the buttons' items at +0x3C to +0x48). A write watch on a letter's corner caught that frame's rebuild. Adding -150 to
each letter's `QuadBatch.offsets` entry, or to its base corners, drew all four 150 to the left
and they stayed there (z3d-mods runs oi19, oi20).

The items on the buttons are the grid's own: `gItemMenu.items` (`HudItemButtons`, +0x3C) holds
every node, the grid's and the buttons' (`ITEM_MENU_NODE_I`, `_X`, `_Y`, `_II`), and quad n of its
batch is node n's icon. Each node's count is a `HudCount` at `counts[n]`; the count's draw rewrites
its digits' offsets inside `HudItemButtons_UpdateAmmo`, which `ItemMenu_Update` calls for the grid
at +0x1178 (`ITEM_MENU_UPDATE_ITEMS_CALL`). The quad table is read once, when the screen is built,
so a runtime write to it or to the screen batch's offsets does not move the column; a patch of the
table does (z3d-mods run oi28). The cursor (`cursorColumn` +0x78, `cursorRow` +0x7C) wraps right
past the grid's six columns into the buttons at `ItemMenu_UpdateCursor` +0x754 (`cmp r0, #6`) and
left onto them at +0x7A4 (`movmi r0, #5`).

The grid's place is four copies of the same literals, node (col, row) at x = 11 + 48·col,
y = 7 + 48·row, 42 px: `ItemMenu_NodePosition` (0x2EB304) places the items,
`ItemMenu_UpdateCursor` the cursor's node, `ItemMenu_UpdateTouch` (0x2EBA9C) maps a touch to a node,
and `ItemMenu_PlaceCursor` (0x446E78, from `ItemMenu_Update`) aims the cursor's corners 2 px outside
it, with its own places for the button nodes 5, 11, 17 and 23, through `MenuCursor_Aim`
(0x2F7AF4; the cursor, then x and y in s0 and s1). Their tops are the `ITEM_MENU_*_TOP` offsets;
moving all four moves the grid with its cursor and touches.

`ItemMenu_UpdateCursor` runs in two states. Selecting (2): X or Y (`Pad_Pressed`) assigns the
cursor's item to node 11 or 17, A picks it up (`fromNode`, the held icon over it through
`ItemMenu_PlaceHeld`, `targetCursor` on it, state 8), B closes the screen (`FUN_002fd84c(0, 1)`,
state 14); otherwise `Pad_Repeated`'s + pad moves the cursor, each move and the last with
`SFX_MENU_CURSOR`. Moving (8): A drops the item on the cursor's node (`toNode`; back on its own
node is a cancel), X or Y assigns it, B puts it back; a placed move goes to state 9 and one
`ItemMenu_CanPlace` refuses to 1 through `ItemMenu_ApplyLayout(1)`; `ItemMenu_SameBowSwap` sends
both A and X/Y to state 12. Each move ends with `ItemMenu_AimCorners(1, 1)`. `ItemMenu_UpdateTouch`
(phase 1 lifted, 2 dragged) tests every node's 42 px square with `Touch_InRect`, phase 0 while no
node is pressed (`pressedNode` -1) and 2 on the pressed one: down on a node presses it, lifted
picks its item up as A does, dragged sets `to*`.

BACK and the tabs are hit-tested apart from their quads: `ItemMenu_Update` calls `Touch_InRect`
(0x33F428; x, y, width, height in touch pixels, unsigned and inclusive, against `gTouch`, and the
stylus's phase: 0 to 3 answer only while that phase's flag in `Touch` is set) with each rect
as immediates, three times each (`ITEM_MENU_*_TOUCH_CALL_*`): BACK (4, 204, 52, 32), Items (182, 198,
72, 42), Gear (66, 204, 54, 34) and Map (124, 206, 54, 34). Moving a quad does not move its touch.

`ItemMenu_Init` (0x46B534, once, from the menu's constructor at 0x455C40) builds the screen's
batch of 37 quads (`gItemMenu.quads`, +0x24) from `gItemMenuQuadPositions`, their sizes,
`gItemMenuQuadUvs` (0x506B30) and `gItemMenuQuadUvSizes` (0x506A08), in `menu_item_parts00.ctxb`'s
pixels; the column's stone is quad 12's cell (192, 0), 64x240, and a round button is quads 19 to
21: its shadow's halves (168, 88), 24x48, the second mirrored, and its stone (124, 0), 44x44 at
(+2, +2). Its model (+0x1C) is bound to `MENU_TEXTURE_ITEM_PARTS` with `MenuModel_SetTexture`
(0x348A64). It ends with `ItemMenu_Reset` at +0x430 (`ITEM_MENU_INIT_LAST_CALL`).
`ItemMenu_Draw` (0x434C5C) draws the screen's model (`blx r1` at +0x20, `ITEM_MENU_DRAW_MODEL_CALL`), then the grid, the cursor, the letters and
the layouts at +0x40 (`bl HudItemButtons_Draw` at +0x88, `ITEM_MENU_DRAW_LAYOUT_40_CALL`), +0x44
and +0x48.

A `HudItemButtons` of the screen's own kind can be made and drawn by a patch: `Heap_New`
(0x313CE0) of 0x474 bytes, `HudItemButtons_Init(layout, count, 0, noCounts)`, then
`MenuModel_SetTexture` on its model (+4) and `QuadBatch_SetUvs` (0x2FC40C, cells in the texture's
pixels over the batch's `textureSize`) for any sheet; `HudItemButtons_SetRect2D` (0x2F8D40, ints: `HudItemButtons_SetRect`, 0x2E6CD4, with its z, a float in s0, at 0),
`HudItemButtons_SetAlpha` (0x2F8CE0, the alpha in s0), `HudItemButtons_UpdateAmmo` and
`HudItemButtons_Draw` each frame. With noCounts 1 its quads draw as plain white in their shape;
with 0 they draw the texture (z3d-mods runs oi40, oi41). A quad whose z is NaN, as a call that
leaves s0 unset can make it, is not drawn on the touch screen's layers but is on the top screen's
HUD (oi53). A node's `HudCount` sizes its digits for
a 42 pixel icon at the icon's x + 18; `QuadBatch_SetRects` (0x2FC534, its z in s0) on the count's batch after
`UpdateAmmo` places them, and their alpha starts at 0. z3d-mods' slot column is drawn this way on
the item screen and, from The One Screen's top screen copy (docs/ocarina.md), on the song list.

A texture outside `gMenuTextures` loads as `MenuTextures_Load` (0x481194) loads those, see
`MENU_TEXTURE_PATH_LENGTH` in z3D.hpp: z3d-mods loads `rom:/message/parts.ctxb` a second time
there and draws the slots' labels from it (oi42).

## Quad batches
A layout's quads are a `QuadBatch`: the count, the sheet's size, the base corners (four `Vec3f`
a quad, set with the quad's rect: 0x2E6CD4 takes a layout, a quad, x, y, width and height and a z in s0),
the drawn corners and one `Vec2f` offset a quad at +0x1C. Each frame `QuadBatch_ApplyOffsets` (0x2F9A1C) writes every drawn
corner as the base corner plus its quad's offset, and the layout's draw (0x2E6ED0) hands the drawn
corners to the GPU through the identity transform at 0x2E7050, shared by every such layout.

## The backdrop

The brown book behind the screens is a model, not a quad: `TouchScreen2D_BeginLayer0` (0x41EC2C)
draws it through the vtable (0x4EBD60) of the object at +0x10 of 0x5043D4, its fourth entry
(0x3F9680), at 0x41EE2C, and then each screen. Its `MenuBackdrop.transform` (+0xC) is the
identity, and the book spans the touch screen's x 0 to 320: a transform x scale of 0.5 drew it
over x 0 to 160 (probe bg-m0). The One Screen wraps that call (`tos_common_backdrop`, 0x6156CC),
which draws it with `blx r5` at 0x6156EC.

## On the top screen

The One Screen draws a pause screen on the top screen through `Presentation_Target`'s viewport of
framebuffer rows 40 to 360 (`glViewport` at 0x6164CC). z3d-mods widens it while a screen's state
is not 0: rows 0 to 400 there, the touch screen's projection over x -40 to 360 as for file select,
the backdrop drawn with x scale 1.25 and x offset -40, and each BACK above moved 40 to the left
into the corner. The screens' own panels stay 4:3 in the middle, the book around them (pages2,
`inputtest pause-wide`). Closing the save prompt and switching tabs show no 4:3 frame (close,
tabs: every frame for half a second).
