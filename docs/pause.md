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

BACK lit is a 50x38 and an 8x38 quad at (0, 202) and (50, 202) over a 56-wide one at (0, 204);
the overworld map cuts that last one into strips 4, 16 and 16 tall (quads 9 to 11). BACK is 40
and 16 wide at (0, 204), the save screen's at (0, 206). The map screen's object holds both
layouts' batches, the overworld's at +0x10 and the other's at +0x14: moving the first's corners
moved the overworld map's frame, tabs and BACK; moving the second's moved nothing there. Inside
the Deku Tree the map's BACK moves with the other layout's table (pages2), so it is likely the
dungeon map's.

The item screen's quads 12 to 24 are the item buttons' column right of the grid (x 256 to 320):
12 the stone under it all, 13 to 18 the I and II squares at the top and bottom (17 and 18 the
buttons, 52 wide), 19 to 21 and 22 to 24 the two round buttons at y 70 and 122 (48 wide); 0 and 1
are the grid's frame, 6 to 11 and 28 to 36 the tabs.

The buttons' letters are not in that table. They are `gItemMenu.buttonLetters` (`ItemButtonLetters`,
the object at +0x30 of the item screen's object 0x5066F8), four quads of a 128x128 sheet in the
`QuadBatch` inline at its +0x48: X at (270, 67), Y at (260, 119), I at (270, 8) and II at (270, 192),
11 to 13 pixels square (`ITEM_BUTTON_LETTER_*`). Their rects are set once, from integers in the
code, so their places are in no data table; 0x2F94A8 runs the object each frame (the item screen's
update, 0x433A90, calls it beside `HudItemButtons_UpdateAmmo` for the grid and the buttons' items
at +0x3C to +0x48). A write watch on a letter's corner caught that frame's rebuild. Adding -150 to
each letter's `QuadBatch.offsets` entry, or to its base corners, drew all four 150 to the left
and they stayed there (z3d-mods runs oi19, oi20).

## Quad batches
A layout's quads are a `QuadBatch`: the count, the sheet's size, the base corners (four `Vec3f`
a quad, set with the quad's rect: 0x2E6CD4 takes a layout, a quad and x, y, width and height),
the drawn corners and one `Vec2f` offset a quad at +0x1C. Each frame 0x2F9A1C writes every drawn
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
