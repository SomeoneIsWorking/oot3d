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
