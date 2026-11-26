# The ocarina: its screen, its notes and its songs

USA `code.bin`. "Live" is the z3d-mods harness on a child file in Kokiri Forest with the Fairy
Ocarina and the six non-warp songs written into the save; the rest is read from the disassembly
and marked so.

## The screen
The ocarina screen is the object at 0x5093E4. `gOcarinaScreenState` (+0x14, 0x5093F8):

| Value | Meaning | Seen |
|---|---|---|
| 0 `OCARINA_SCREEN_CLOSED` | not up | live |
| 4 `OCARINA_SCREEN_SONG_LIST` | the song list page | live |
| 12 `OCARINA_SCREEN_PLAY` | free play, waiting for notes | live |
| 1, 3, 11, 16 | opening and page changes | read (jump table 0x425B18 in the update 0x425AB4) |
| 2, 15 | closing | read |

Under The One Screen the ocarina comes out on D-pad Right, ZR goes from the play page to the song
list, and A on the list puts the ocarina away (live, runs ctl-oca1, ctl-song2).

`OcarinaScreen_Draw` (0x426748) runs from `TouchScreen2D_BeginLayer0` (0x41EE74), so its models
reach the touch screen only. The One Screen's top screen copy is its own: its `Hud_Draw` (0x6145F4,
891b60e's payload) calls `Ocarina_Draw` (0x610D88) at 0x61462C (`EBFFF1D5`) while the frontend's
owner is the ocarina, which copies the ocarina's quads into its scene in the top screen's 400x240
pixels. A layout drawn after that call, from `TopHud_Draw`, lands on the top screen over the song
list (z3d-mods run oi40); one drawn inside `OcarinaScreen_Draw` does not show.

`Ocarina_Draw` places the song list at scale 0.85 centred on the top screen: x is
(width - 320 * 0.85) / 2, the width loaded by `vldr s6` at 0x610EB0 (`ED9F3A27`) from the literal
400.0 at 0x610F54, which the play page's branch (0x610E8C) shares. A different width there centres
the list in that many pixels from the left edge (z3d-mods run centre1).

## The song list
Twelve entries, four to a row. The cursor's column and row are the words at 0x5093FC and
0x509400, and `gOcarinaSongListShown` (0x509404) is the entry on show, column + row * 4 (live:
1 after one move right, 5 after a move down, 9 after another). `gOcarinaSongListItems` (0x50A3B0,
u32[12]) holds each entry's `ITEM_SONG_*` (live):

| Row | 0 | 1 | 2 | 3 |
|---|---|---|---|---|
| 0 | Lullaby | Sun | Minuet | Requiem |
| 1 | Epona | Time | Bolero | Nocturne |
| 2 | Saria | Storms | Serenade | Prelude |

A song is learned when its bit is set in `inventory.questItems`: `QUEST_SONG_MINUET` (6) to
`QUEST_SONG_STORMS` (17), the same order as `ITEM_SONG_MINUET` to `ITEM_SONG_STORMS`. Live: bits
12 to 17 written, the six non-warp entries show their tablets and the warp songs a bare note.

Not seen: `gOcarinaSongListShown` on an entry that is not learned (read: -1).

## Notes
`PadMgr_RequestPadData` (0x41A874) maps the pad to the audio's ocarina input on every pad update.
At +0x98 (0x41A90C) r0 is the pad word the mapping reads; The One Screen's hook two instructions
before it has already swapped A and B there. The mapping passes notes only while the screen
takes them (read: 0x43C1EC returns 2 and the object's +0x44 is set):

| Note, low to high | Pad button | Number in `gOcarinaSongButtons` |
|---|---|---|
| D4 | L (`PAD_L`) | 0 |
| F4 | R (`PAD_R`) | 1 |
| A4 | Y (`PAD_Y`) | 2 |
| B4 | X (`PAD_X`) | 3 |
| D5 | A (`PAD_A`) | 4 |

`gAudioOcarinaInputEnabled` (0x54AC5C) reads 1 on the play page (live).

## Songs
`gOcarinaSongButtons` (0x54C222, 14 rows of 9 bytes): a count, then the song's notes by the
numbers above (live dump). Rows 0 to 5 are the warp songs in quest order; then 6 Saria, 7 Epona,
8 Lullaby, 9 Sun, 10 Time, 11 Storms; 12 and 13 are filled at run time (read).

The game recognises a song from the pitches alone (read: the matcher at 0x47BD10 forces each
note's length). Live (z3d-mods run ctl-song3): the Song of Storms' six notes ORed into the pad
word at 0x41A90C, each held for four pad updates with four empty ones between, on the play page:
the staff fills, "You played the Song of Storms" shows and it rains.

## Not done
- The warp songs and the scarecrow's song were not played this way.
- The notes' timing limits are read, not measured: one update after a pitch change is dropped.
