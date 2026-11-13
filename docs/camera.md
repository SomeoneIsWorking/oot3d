# Camera

Findings from z3d-mods (its right-stick free camera), checked on the USA build in Azahar.

## The per-frame update
- `Play_Update` (0x2E2E60) updates the cameras at its end: the inactive ones in a loop
  (`bl Camera_Update` at +0x151C, 0x2E437C), then the active one, `cameraPtrs[activeCamera]`
  (+0x1540, 0x2E43A0). `Gameplay_Draw` (0x2E25F0) runs the active camera's update once more
  (+0x808, 0x2E2DF8) when the byte at `PlayState + 0x361` is set, and clears it.
  `Gameplay_Update` (0x45238C) calls `Play_Update` and tail-calls `Gameplay_Draw` (+0xA8).
- `Camera_Update(Vec3s* out, Camera* camera)` (0x2D84C4) picks the camera function as
  `sCameraFunctions[sCameraSettings[setting & 0xFF].cameraModes[mode & 0xFF].funcIdx]` (the `blx`
  at +0x420): `sCameraSettings` (0x516FF0, 78 entries of `CameraSetting`) and the function table
  (0x517260) keep the N64 layout and function order, so index 2 is `Camera_Normal1` (0x239FD8),
  4 `Camera_Normal3` and 22 `Camera_Jump1`. After the function it applies the quake offsets to a
  copy of eye and at, calls `View_LookAt(&play->view, eye, at, up)` (0x2D77DC) and stores the
  eye-to-at pitch and yaw in `camDir` (the write at 0x2D93EC); `out` receives `inputDir`.
- `camDir` holds binary angles with `at - eye = dist * (cos p sin y, sin p, cos p cos y)`: a
  negative pitch looks down on Link from above. Seen live: -0x68E at rest behind Link in Kokiri
  Forest; a camera placed by that formula at pitch -0x3000 looks down on him, at +0x1400 up from
  the ground.
- `dist` is the radius the camera function measured from `at` to `eyeNext`, the eye before
  collision, clamped into the setting's range (136.7 in Kokiri Forest at rest, 92.8 after a
  jump). A caller that writes `eye` must write the uncollided position to `eyeNext`, or `dist`
  follows every wall the eye was pushed off.
- `Camera_BGCheckInfo(camera, from, colChk)` (0x3553FC) reads only `colChk->pos`. It line-tests
  from `from` to `pos`; on a hit, or when `pos` is less than 5 units above the floor under it,
  `pos` becomes the hit point plus the polygon's normal, `poly` and `bgId` name the polygon and
  the result is `bgId + 1`. Otherwise `pos` moves one unit back toward `from` and the result is 0.

## Which settings follow Link
Mode 0 (`CAM_MODE_NORMAL`) of a setting runs `Camera_Normal1` for settings 0x01-0x04, 0x07-0x11
(the boss rooms), 0x2C, 0x2D, 0x34-0x37, 0x3A, 0x3B, 0x3D-0x3F, 0x41-0x43 and 0x47-0x4C. Setting
0x05 runs function 24 (`Jump3`), 0x06 (the horse) `Camera_Normal3`, 0x1E (the crawlspace)
function 20; 0x12-0x4D otherwise run fixed, pivoting, door, start or scripted functions. In a
normal setting, mode 1 (Z-targeting without a target, `CAM_MODE_Z_PARALLEL`) runs function 7,
modes 2-4 (targets and talking) 12, 14 and 27, first person and the aimed items (6-11) 19 and 66,
climbing 23, jumping and falling (13, 16) `Camera_Jump1`, hanging 42, the spin charge 30.

Settings read live (`Camera.setting`, File 1 as a child):

| Scene | Entrance | Setting |
|---|---|---|
| Kokiri Forest, Lon Lon Ranch, Graveyard, Temple of Time | 0xEE, 0x157, 0xE4, 0x53 | 0x01 |
| Hyrule Field, Hyrule Castle grounds | 0xCD, 0x138 | 0x02 |
| Inside the Deku Tree, Dodongo's Cavern | 0x00, 0x04 | 0x03 |
| Market entrance, Temple of Time exterior | 0x33, 0x171 | 0x19 |
| Market, Link's house, Know-It-All Brothers' house | 0xB1, 0xBB, 0xC9 | 0x1A |
| Scene 70 | 0x7A | 0x20 |
| Back alley | 0xAD | 0x46 |

Setting 0x46, the back alley's, runs function 79, a fixed view.

Modes read live in Kokiri Forest: 0 walking, 1 with L held, 6 in first person (The One Screen's
D-pad Left), 16 falling, 3 talking.

## The C-stick
The game never opens `ir:rst`. z3d-mods' patch reads the C-stick from that service's shared
memory itself.

## Checking it
z3d-mods' free camera replaces both `bl Camera_Update` calls for the active camera and, while the
selected function is `Camera_Normal1` or `Camera_Jump1`, places the eye on its own orbit around
the game's `at` after the game's update. In Kokiri Forest the C-stick turns the view around Link
and tilts it; L, first person and talking hand the view back on the same frame; the Market
and a running cutscene do not react; in Dodongo's Cavern's entrance corridor the eye stays inside
the walls.
