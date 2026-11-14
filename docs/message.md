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
fonts, format `QBF1`, read from the USA files:
- Header, 16 bytes: `"QBF1"`, u16 glyph count (199 in ltn16), u16 cell count (208), u32 0x2A
  (`*`, taken to be the character drawn for one the font lacks), then four bytes 04 10 10 02:
  bits a pixel, the cell's width and height (16 by 16), and one not understood.
- Glyph table, 8 bytes a glyph, in code order from 0x20: u16 character code, u16 cell index, u8
  the glyph's left edge in its cell, u8 its width, u16 zero. The left edge centres the glyph:
  a space is 6 and 4, `!` 6 and 3, `%` 3 and 11.
- Cells, 128 bytes each (16 x 16 at 4 bits), after the table: 16 + 199 x 8 + 208 x 128 is the
  file's size. Drawn as plain row-major nibbles the letters are recognisable but striped, so
  the pixel order or the nibble's meaning (two layers?) is not found yet.
- sys8.qbf: 288 glyphs, 0xC0 cells, bytes 08 08 10 04; not decoded.
- In the code: the magic `QBF1` is a literal at 0x2DF6A8 and `rom:/message/sys8.qbf` a string at
  0x46B4C4; the functions around them are not read. The messages are `message/us/us.qm`
  (`QM`), not decoded here.
