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

#endif //_Z3DMESSAGE_H_
