#ifndef _Z3DMESSAGE_H_
#define _Z3DMESSAGE_H_

// Message text is UTF-8 with control codes: 0x7F, then the code.
#define TEXT_CTRL_PREFIX 0x7F
#define TEXT_CTRL_WAIT_FOR_INPUT 0x01
#define TEXT_CTRL_INSTANT_TEXT_ON 0x04
#define TEXT_CTRL_INSTANT_TEXT_OFF 0x05

// Message_LayoutText hands each element to a text object through this table (vtable 0x4ECCD8).
typedef struct {
    /* 0x00 */ void* unk_00[5];
    /* 0x14 */ void* waitForInput; // box break
    /* 0x18 */ void* unk_18[2];
    /* 0x20 */ void* instantOn;    // MessageText_InstantOn
    /* 0x24 */ void* instantOff;   // MessageText_InstantOff
} MessageTextVtbl;

// A text element's button or icon: TEXT_CTRL_ICON, then the icon's number (docs/message.md).
#define TEXT_CTRL_NEW_LINE 0x1C
#define TEXT_CTRL_COLOR 0x1D
#define TEXT_CTRL_ICON 0x24

// message/<language>/ltn16.qbf and message/sys8.qbf (docs/message.md, "The font files").
#define QBF_MAGIC 0x31464251 // "QBF1"

typedef struct {
    /* 0x00 */ u32 magic;
    /* 0x04 */ u16 glyphCount;
    /* 0x06 */ u16 cellCount;
    /* 0x08 */ u16 defaultChar; // drawn for a character the font lacks
    /* 0x0A */ u16 unk_0A;
    /* 0x0C */ u8 bitsPerPixel;
    /* 0x0D */ u8 cellWidth;
    /* 0x0E */ u8 cellHeight;
    /* 0x0F */ u8 format;       // 0 to 3: 4-bit alpha cells; 4: 8-bit
} QbfHeader; // size = 0x10

// Sorted by code; the cells follow the table.
typedef struct {
    /* 0x00 */ u16 code;
    /* 0x02 */ u16 cell;
    /* 0x04 */ u8 left;    // where the advance box starts in the cell
    /* 0x05 */ u8 advance;
    /* 0x06 */ u8 unk_06;
    /* 0x07 */ u8 unk_07;
} QbfGlyph; // size = 0x8

typedef struct {
    /* 0x00 */ u32 unk_00;
    /* 0x04 */ QbfHeader* data;
    /* 0x08 */ u32 size;
} QbfFont; // size = 0xC

// How a TextRenderer keeps glyphs in its texture (TextRenderer.glyphTextureMode).
#define GLYPH_TEXTURE_STATIC 1 // the font's characters 0 to 255, sixteen a row, filled once
#define GLYPH_TEXTURE_CACHE 2  // each character copied in as text first uses it (what the game uses)
#define GLYPH_TEXTURE_COLUMNS 16
#define GLYPH_TEXTURE_CHARS 256

// Lays text out as quads over one glyph texture (docs/message.md, "Drawing glyphs").
typedef struct {
    /* 0x000 */ char unk_000[0x10];
    /* 0x010 */ QbfFont* font;
    /* 0x014 */ QbfFont* rubyFont;
    /* 0x018 */ u8 rubyOn;
    /* 0x019 */ u8 glyphTextureMode;
    /* 0x01A */ char unk_01A[0xDE];
    /* 0x0F8 */ void* cachedGlyphs; // 0x10 each: font, character, x, y in the texture
    /* 0x0FC */ u32 cachedGlyphCount;
    /* 0x100 */ char unk_100[0xA8];
    /* 0x1A8 */ u32 quadCount;
    /* 0x1AC */ char unk_1AC[0x230];
    /* 0x3DC */ void* glyphTexture;
    /* 0x3E0 */ s32 glyphTextureWidth;
    /* 0x3E4 */ s32 glyphTextureHeight;
} TextRenderer;

#endif //_Z3DMESSAGE_H_
