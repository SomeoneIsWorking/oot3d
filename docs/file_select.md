# File select

How file select places what it draws, for moving it on the top screen under The One Screen
891b60e (USA `code.bin`). Runs are z3d-mods harness runs (`scratch/runs/<name>`), File 1 saved.

## Its quads

`gFileChoose` (0x504FA0) is the screen's object. `active` (+0x10) is not 0 while it is up;
The One Screen's `FileSelect_Active` reads it with `SaveContext.gameMode` 2.

When the screen opens, the function at 0x46B0F4 makes a batch of 184 quads (0x2FC694), keeps it
in `quads` (+0x30) and gives it its corners with FUN_002fc534(batch, positions, sizes, count,
first): quad i is the rectangle `quadSizes[i]` (a table at 0x5055B8, width and height) at
`quadPositions[i]` (+0x58, x and y), in the touch screen's pixels from the top left. Both tables
are initialised data, so a patch of the executable moves a quad. Nothing writes the corners again
while the screen is up (a write watch on BACK's for a second, fsprobe).

| Quads | What | Place |
|---|---|---|
| 173, 174 | BACK | 40x36 at (0, 204), 16x36 at (40, 204) |
| 118 to 120 | BACK lit | 50x38 and 8x38 at (0, 202) and (50, 202); 56x36 at (0, 204) |

Found from the corners (four x, y, z vertices a quad) in the heap at file select, a write watch on
them while the screen opened (0x46B128), and the call at 0x46B144.

## The touch screen's projection

Graph_DrawFrame draws the touch screen in four 2D layers, `TouchScreen2D_BeginLayer0` to `3`
(0x41EC2C, 0x41D184, 0x41C4DC, 0x41AF88). Each builds the same projection and hands it to
`Camera2D_SetMatrices` (0x3246EC; r1 the projection, r2 the view), at +0x1CC, +0x1C4, +0x1F0 and
+0x1B8. The projection's rows, for the screen turned on its side: clip x = 1 - y / 120 and clip y
= 1 - x / 160, so x 0 to 320 fills the viewport. At file select layers 0, 1 and 3 run every frame
(a write watch on the view `Camera2D_SetMatrices` copies to 0x5B5018, by return address);
`TouchScreen2D_Begin` (0x3243A8, the same projection, from FUN_0032b770) does not.

The One Screen draws the touch screen's file select on the top screen with that projection and a
viewport of framebuffer rows 40 to 360 (`Presentation_Target`, `bind_target(..., 40, stereo,
320)`, at 0x6163FC), so a quad left of x 0 or right of 320 is cut off. z3d-mods widens both while
file select is up: rows 0 to 400, and clip y = 0.8 - x / 200 (x -40 to 360) in all four layers.
Every quad stays where it was and BACK, moved 40 to the left, shows in the corner (fs-check). The
touch screen's own pass draws with the same projection, so on the touch screen itself file select
is narrowed to its middle 256 pixels; with The One Screen that screen is not shown.
