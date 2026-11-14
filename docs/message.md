# Message text layout

Findings from z3d-mods (its Quick Text), checked on the USA build in Azahar.

## Laying out a message
- `Message_LayoutText` (0x2E03E8) walks a message's text one element at a time. Each step calls
  `Text_ParseNextCharacter_maybe(state, next)` (0x4C08C0, the `bl` at +0xA8) with the parse state in
  r6, the next character in r8, the text object in r5 and the element index in r9 (0 at the start
  of the message).
- A control code (`0x7F`, code; `include/z3Dmessage.hpp`) leaves its number at state+4 and
  dispatches through the jump table at +0x1A4 (48 entries, 0x2E058C). Each handler calls one
  method of the text object, then joins the common tail at +0xAEC (0x2E0ED4).
- Text object methods (`MessageTextVtbl`, vtable 0x4ECCD8): +0x14 box break (code 0x01), +0x20
  instant text on (`MessageText_InstantOn`, 0x40B608, code 0x04), +0x24 instant text off
  (`MessageText_InstantOff`, 0x40B2C0, code 0x05), +0x8C unskippable (code 0x19).
- Instant text lasts until code 0x05 or the next box: after a box break the next box prints
  letter by letter again.

- Delay codes (0x08) still pause inside a box under instant text; the opening narration uses them
  to keep its lines in time with the cutscene.

## Checking it
z3d-mods' Quick Text calls `instantOn` on element 0 and after each box break, and points the
table's entry for code 0x05 (+0x1B8) at the common tail. In the Deku Tree's scene after Gohma
every box, the choice included, shows its whole text on the first frame it is visible; in the
opening narration each part between delay codes appears whole.

## The font files
`romfs:/message/<language>/ltn16.qbf` (US: 28232 bytes) and `message/sys8.qbf` are the text
fonts, format `QBF1` (`QbfHeader`, `QbfGlyph` in `include/z3Dmessage.hpp`), read from the USA
files:
- Header, 16 bytes: `"QBF1"`, u16 glyph count (199 in ltn16), u16 cell count (208), u16 the
  character drawn for one the font lacks (0x2A, `*`; `QbfFont_GetDefaultChar`, 0x2D2664), u16
  zero, then four bytes 04 10 10 02: bits a pixel, the cell's width and height (16 by 16), and
  the format (0 to 3: 4-bit alpha cells; 4: 8-bit).
- Glyph table, 8 bytes a glyph, sorted by character code (`QbfFont_FindGlyph`, 0x2D2674, is a
  binary search): u16 code, u16 cell index, u8 where the advance box starts in the cell, u8
  the advance, u16 zero. A glyph is drawn with its cell's left edge at the pen minus the first
  and the pen moves on by the second: a space is 6 and 4, `!` 6 and 3, `%` 3 and 11. ltn16
  has Latin-1 from 0x20, then U+0152, U+0153, U+0178, U+2014, U+201C, U+201E, U+20AC and
  U+25C6; 0xA0 and U+25C6 have no advance.
- Cells, 128 bytes each, after the table: 16 + 199 x 8 + 208 x 128 is the file's size. A cell
  is 4-bit coverage, row by row from the top, the high nibble of each byte first.
- sys8.qbf: 288 glyphs, 0xC0 cells, bytes 08 08 10 04 (8 bits, 8 by 16, format 4); its cells
  are not drawn here.
- `QbfFont_Init(font, data, size)` (0x2DF620) checks the magic (the literal at 0x2DF6A8) and
  that the table and cells fit, and keeps the file at `QbfFont.data`. The message manager's
  init (0x2F41F8) gives it ltn16 (manager +0x2C) and, for a language that has one, a second
  font for the small text over a word (+0x38).

## Drawing glyphs
A `TextRenderer` turns text into quads over one glyph texture. `TextObject_Init` (0x2E76B8)
makes one for each piece of text on screen, and the message manager has its own
(`TextRenderer_Init`, 0x2DAC68, with room for 448 glyphs).
- `TextRenderer_AddGlyph(renderer, character)` (0x2BAF9C) is called for every character of a
  run (from 0x4C0D38, which walks the laid-out elements). It looks the glyph up, falls back to
  the default character, and adds a quad of the cell's size at the pen
  (`TextRenderer_AddGlyphQuad`, 0x2B7234), with texture coordinates in texels.
- `TextRenderer.glyphTextureMode` says where the cell is in the texture
  (`TextRenderer_AllocGlyphTexture(renderer, mode, width, height)`, 0x2DA7E8):
  - 2 (`GLYPH_TEXTURE_CACHE`), what every caller asks for: a 256 by 256 4-bit alpha texture
    that starts empty. `TextRenderer_AddCachedGlyph` (0x2B7498) copies a character's cell into
    the next free place the first time the text uses it and remembers it in
    `cachedGlyphs`. The texture's content follows the text on screen, so every message makes a
    different texture.
  - 1 (`GLYPH_TEXTURE_STATIC`): the texture is sixteen cells wide and sixteen high (each
    rounded up to a power of two) and `TextRenderer_FillStaticGlyphTexture` (0x46A738) fills it
    once with characters 0 to 255, each in the cell of its code; a character the font lacks
    gets the default character's glyph. `TextRenderer_AddGlyph` then draws a character from
    the cell of its code, and one past 255 as the default character. No caller in the game
    uses it, and it works: with the mode forced to 1 the dialogs, the file select and the
    menus print as before from one texture that never changes.
- Seen in Azahar's texture dump (z3d-mods runs `txt-dump1`, `txt-atlas1`): the game's own mode
  uploads a new 256 by 256 A4 texture for each new text (six by the first dialog); mode 1
  uploads one, the same in every screen.
- z3d-mods forces mode 1 at `TextRenderer_Init` + 0x13C and gives the characters past 255 the
  cells of the control codes, 0 to 0x1F, in the table's order, at the lookup in
  `TextRenderer_FillStaticGlyphTexture` + 0x80 and the cell choice in `TextRenderer_AddGlyph`
  + 0x150 (its `patch/oot3d/font_atlas.cpp`). The texture is then a function of the font file
  alone, which a texture pack can replace.

## The message file
`romfs:/message/us/us.qm` (`QM`), read from the USA file and written back byte for byte by
z3d-mods' `tools/z3dmods/qm.py`:
- Header, 16 bytes: `"QM\0\0"`, u32 4, u32 message count (2510), u32 zero.
- One entry of 0x60 bytes a message: u32 number, five u32s (the box's kind and place among
  them, not named), then nine pairs of u32 file offset and length, one a language. The USA
  file fills 0 (English), 4 (French) and 6 (Spanish). The texts follow the table, each padded
  to four bytes.
- Text is UTF-8. A control code is 0x7F, the code, then its arguments. The bytes to step over
  come from the table at 0x4D6708 (`Text_ParseNextCharacter_maybe`; the UTF-16 parser at
  0x4C0AB0 has its own at 0x4D6588): one byte for 0x02, 0x06, 0x08, 0x0A, 0x0F, 0x10, 0x18,
  0x1D, 0x23, 0x24 and 0x26; two for 0x03; three for 0x11 and 0x25; four for 0x1A; five for
  0x0D and 0x0E; six for 0x1B; none for the rest up to 0x2F.
- Codes read from the text: 0x00 ends a message, 0x01 a box, 0x1C a line; 0x1D sets the
  colour (its low three bits); 0x0F shows an item's icon beside the box; 0x1A and 0x1B come
  before a choice of two and of three; 0x24 draws a prompt icon.

Prompt icons (0x24), by the words around them in the English text and, for those marked,
seen drawn (z3d-mods runs `txt-oot-r4`, `txt-oot-w1`). Each is a 16 by 16 cell of
`romfs:/message/parts.ctxb` (256 by 128); a button the game can show unavailable has its dim
copy in the cell to the right.

| Icon | Is | Cell (x, y) |
|---|---|---|
| 0x00 | Circle Pad (seen) | 128, 0 |
| 0x01 to 0x05 | the +Control Pad, then its Up, Right, Down and Left (seen); the text uses 0x01 and 0x04 (the map's display), no message in any language 0x02, 0x03 or 0x05 | 128 to 192, 16 |
| 0x06 to 0x0B | A (seen), B (seen), X (seen), Y (seen), L (seen), R (seen) | 128 to 208, 32 |
| 0x0C | START (seen; no English message uses it) | 224, 32 |
| 0x0F, 0x11 | Circle Pad up and down | |
| 0x14 | Circle Pad left and right | |
| 0x16, 0x17 | the touch screen's Ocarina and Navi icons (seen) | 128, 48 and 160, 48 |
| 0x18 to 0x1B | the touch screen's View (seen), Gear, Items and Map (seen) icons | 128, 160, 192 and 224, 64 |
| 0x1C, 0x1D | the I and II item buttons (seen) | 192, 48 and 224, 48 |
| 0x1E, 0x1F | the X and Y item buttons (seen) | 128, 80 and 160, 80 |
| 0x0E | a 3DS with its touch screen lit (seen; no message uses it) | 208, 0 |
| 0x10, 0x12, 0x13 | the Circle Pad's right, left and up cells (seen; no message uses them) | |
| 0x21 | the target arrow (seen) | 240, 0 |
| 0x22 | the map's marker | |

0x0D, 0x15, 0x20 and 0x23 to 0x31 draw nothing (z3d-mods runs `gap-ic1`, `gap-ic2`: the codes
put into a message).

A message can be shown on demand by writing its number to a sign's `Actor.textId` (+0x116)
before reading it: Kokiri Forest's sign at (49, -80, 966), by Link's house, read from the
south (`txt-oot-k23`).
