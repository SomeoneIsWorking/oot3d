# Prior art

Build: OoT3D USA, `code.bin` md5 `20c7ec6288ba0af4feb6db646c6cc6d5`.

| Source | Licence | Build match | What we take |
|---|---|---|---|
| zeldaret/oot3d (this repository's upstream) | CC0-1.0 | builds this `code.bin` (README checksum) | everything |
| SomeoneIsWorking/oot3d-decomp (private) | ours | anchors `"cmab"` 0x4ED6EA and `rom:/kankyo/BlueSky.zar` 0x54984C match | `docs/notes/` and `docs/source_file_attribution.md` |
| zeldaret/oot (N64) | CC0-1.0 | engine ancestor | names and struct shapes, checked against this build |
| gamestabled/OoT3D_Randomizer | none for its own code (`LICENSE.md` carries only the N64 OoT-Randomizer's MIT notice) | USA layout (`oot.ld`) | evidence only: names for the message functions, the free camera's hook at `Camera_Update` |
| toadalle/The-One-Screen 891b60e | MIT for its own code; inherited headers and game-derived assets excluded | USA Rev 1 (`source/retail/abi.h`); its hooks' expected words match this `code.bin` | evidence only: where its HUD hooks the game (`docs/hud.md`), each address checked in the disassembly or live |
| OTPR26/OOT3DHud (Ocarina ReFramed) | GPL-3.0-or-later | USA | evidence only |
| HarbourMasters/Shipwright (N64 PC port) | none stated | N64 build, not this one | evidence only: where its frame interpolation is told not to blend the camera (`FrameInterpolation_DontInterpolateCamera` in `z_camera.c`, `z_view.c`), leads for this build's camera cuts; its per-matrix recording is not the method we use |

Frame interpolation: coccofresco/TriAevum (GPL-3.0, a static recompilation of this game's code) shows 60 and 90 fps over the 30 Hz logic by decomposing the affine matrices it finds in the PICA vertex shader's float uniforms and blending those (`runtime/three_ds_recomp/include/fast/renderer3ds/pica_rigid_interpolation.h`): evidence only, and not the method we use, since a uniform search misses every draw it cannot recognise. OTPR26/Ocarina-ReTuned (no licence) is a fork of it. The 60 fps modes reported for the 3DS games raise the logic rate; they are not interpolation. Method references outside this game: psxport `docs/plans/replay-interpolation.md` (ours) and ZeldaWWHDRecomp `runtime/src/interp/` (MPL-2.0, design only).
