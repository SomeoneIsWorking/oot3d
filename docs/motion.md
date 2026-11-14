# Motion: the gyroscope and the first-person look

USA build. The code is the same as Majora's Mask 3D's, function for function; its decomp's
`docs/motion.md` has the reader's arithmetic in full.

## The path

1. `ReadLocal` (0x2FE0F0, the SDK's `GyroscopeReader`) takes up to 32 new HID samples (about 100
   a second). Per sample and axis `CalculateGyroscopeAxisStatus` (0x2FA5EC) turns the raw value
   into a speed, 0x436354 turns the reader's direction matrix by it, and once per call
   `ReviseDirection_Acceleration` (0x4367DC) pulls the matrix toward gravity.
2. `Motion_Update` (0x301A30), once a game frame from `Graph_ThreadEntry` (0x416368), reads the
   latest status (`GyroscopeReader_Read`, 0x41B6C0) and takes only the direction matrix: yaw from
   its first row projected on the horizontal plane, pitch from what is left once that yaw is
   undone. It stores them in degrees in `gMotionState` (0x55B240), unwrapped, minus a reference
   taken at the last reset. Roll is always 0.
3. `Player_UpdateLook` (0x2C036C, four callers: first person and the aimed items) first moves
   `Actor.focus.rot` by the circle pad, then, while the gyro look is active, overwrites it:
   pitch = `lookBase[0]` + `gMotionState.pitch` * `pitchScale`, yaw = `lookBase[1]` +
   `gMotionState.yaw` * `yawScale`, clamped to the look's limits (`Motion_GetLook`, 0x4C0588,
   scales). In Master Quest (`gSaveContext.masterQuestFlag`) the yaw is mirrored.

The view follows the 3DS's orientation since the last reset, not its turn rate: one degree of yaw
is 2.1 degrees of view, one degree of pitch 1.46 (`yawScale`, `pitchScale`, set once at 0x416970).
A turn on the HID x axis is positive pitch and looks down; a turn on y looks left.

## When the gyro look is active

All of:
- `gSaveContext.motionOff` (+0xF) is 0. Written 1 live, the same input leaves the view still.
- `Player_UpdateLook`'s last argument is 0.
- `Player.lookFlags` (+0x29B8) bit 0. It is set, with a reset, on the first frame where bit 1 is
  set or `stateFlags1` has 0x1000000. In the plain first-person view (D-pad Left under The One
  Screen, `stateFlags1` 0x100000) the low byte reads 3.
- `gMotionState.valid`.

`Motion_Reset` (0x2C0804) zeroes the angles and asks `Motion_Update` for a new reference; its
callers copy `focus.rot` to `Player.lookBase`. It runs when the look becomes active and on every
frame where the circle pad is off centre (`Player.pad`: direction bits 0xF0000000 of its first
word, or stick x and y at +0x28 and +0x2C with a squared length of 5.2 or more). The pad's own
step is (1 - cos(stick * 200 binary angle units)) * 725 (pitch) or 715 (yaw) units a frame, with
the stick's sign; `gSaveContext` + 0x13D8 at 0 flips the pitch stick.

## The reader's settings

`Motion_Init` (0x417188) leaves the reader (`MotionState.reader`, 0x080001AC in the run) with
zero play on (+0x854, threshold +0x858 = 0.005, 1.8 degrees a second), zero drift on (+0x855,
mode 1: radius 0.01, 100 samples, power 0.02) and the accelerometer revise on (+0x856, power 0.03,
range 0.4). The three flag bytes read 01 01 01 live. So, as in MM3D: a rate under 1.8 degrees a
second is dropped; a rate that holds within 3.6 degrees a second for a second is learnt as bias
and removed; and with an accelerometer that never changes, pitch decays back to level.

## Seen in the running game

Kokiri Forest, first person, the app's harness (the emulator scales the fed rate by the host's
frame time, so the degrees are read back from the reader and `gMotionState`):

- `gyro-oot1`: a turn on y moves `gMotionState.yaw` 14.4 degrees and `focus.rot.y` 30.2: the view
  swings left. A turn on x moves pitch 6.17 degrees and `focus.rot.x` 9.0, the view tilts down,
  and two seconds later the pitch is 1.09 and the view is level again.
- `gyro-oot2`: with +0x854, +0x855 and +0x856 written 0, 11.156 degrees of pitch reach the
  reader, `gMotionState.pitch` reads 11.155, `focus.rot.x` moves 16.28 (1.46 a degree) and it
  stays. A yaw fed as (0, cos p, -sin p) for that pitch p turns 34.2 degrees for a rate vector
  34.27 long and leaves the pitch within 0.3 degrees.
- `gyro-oot3`: `motionOff` written 1, the input that had turned the view leaves it still.

## Not known

- The aimed items (slingshot, bow, hookshot, boomerang) were not run.
- Zero play and zero drift were read in the code and switched live only in MM3D.
- What sets bit 1 of `lookFlags`.
