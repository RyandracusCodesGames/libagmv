#ifndef AGIDL_CC_MIXER
#define AGIDL_CC_MIXER

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_cc_mixer.h
*   Date: 9/10/2023
*   Version: 0.4b
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_cc_core.h>
#include <agidl_types.h>

/* MULTIPLY SOURCE COLOR BY INVERSE BLEND FACTOR AND DEST COLOR BY BLEND FACTOR
   COL1*(1-blend_factor)+COL2*(blend_factor)*/
#define CC_BLEND_CLR_SRCINV 0x12
/* MULTIPLY SOURCE COLOR BY BLEND FACTOR AND DEST COLOR BY INVERSE BLEND FACTOR
   COL1*(blend_factor)+COL2*(1-blend_factor)*/
#define CC_BLEND_CLR_DESTINV 0x13
/* MULTIPLY SOURCE COLOR BY BLEND FACTOR AND ADD DEST COLOR
   COL1*(blend_factor)+COL2*/
#define CC_BLEND_CLR_SRC 0x14
/* MULTIPLY DEST COLOR BY BLEND FACTOR AND ADD SOURCE COLOR
   COL1+COL2*(blend_factor)*/
#define CC_BLEND_CLR_DEST 0x15
/* MULTIPLY SOURCE COLOR BY BLEND FACTOR AND ADD DEST COLOR
   COL1*(1-blend_factor)+COL2*/
#define CC_BLEND_CLR_ISRC 0x16
/* MULTIPLY DEST COLOR BY INVERSE BLEND FACTOR AND ADD SOURCE COLOR
   COL1+COL2*(1-blend_factor)*/
#define CC_BLEND_CLR_IDEST 0x17

typedef enum AGIDL_CC_COMBINE_MODE{
	CC_MUL_CLR = 0x1,
	CC_ADD_CLR = 0x2,
	CC_SUB_CLR = 0x3,
	CC_MUL_AND_SUB_CLR = 0x4,
	CC_MUL_AND_ADD_CLR = 0x5,
	CC_MUL_AND_SUB_CLR1 = 0x6,
	CC_MUL_AND_ADD_CLR1 = 0x7,
	CC_MUL_AND_SUB_CLR2 = 0x8,
	CC_MUL_AND_ADD_CLR2 = 0x9,
	CC_INTERP_CLR = 0x10,
	CC_MUL_AND_INTERP_CLR1 = 0x11,
	CC_ADD_AND_INTERP_CLR1 = 0x12,
	CC_SUB_AND_INTERP_CLR1 = 0x13,
	CC_MUL_AND_INTERP_CLR2 = 0x14,
	CC_ADD_AND_INTERP_CLR2 = 0x15,
	CC_SUB_AND_INTERP_CLR2 = 0x16,
}AGIDL_CC_COMBINE_MODE;

void AGIDL_ClampRGB(u8* r, u8 *g, u8 *b, AGIDL_CLR_FMT fmt);
COLOR AGIDL_AddColor(COLOR clr1, COLOR clr2, AGIDL_CLR_FMT fmt);
COLOR AGIDL_SubColor(COLOR clr1, COLOR clr2, AGIDL_CLR_FMT fmt);
COLOR AGIDL_MulColor(COLOR clr1, COLOR clr2, AGIDL_CLR_FMT fmt);
COLOR AGIDL_DotColor(COLOR clr1, float factor, AGIDL_CLR_FMT fmt);
COLOR AGIDL_DivColor(COLOR clr1, float factor, AGIDL_CLR_FMT fmt);
COLOR AGIDL_BlendColor(COLOR clr1, COLOR clr2, float blend_factor, int blend_mode, AGIDL_CLR_FMT fmt);
COLOR AGIDL_ColorCombine(COLOR clr1, COLOR clr2, AGIDL_CC_COMBINE_MODE cc, AGIDL_CLR_FMT fmt);
COLOR AGIDL_InterpColor(COLOR clr1, COLOR clr2, float interp, AGIDL_CLR_FMT fmt);
COLOR AGIDL_BilerpColor(COLOR clr1, COLOR clr2, COLOR clr3, COLOR clr4, float interp, float bilerp, AGIDL_CLR_FMT fmt);
COLOR AGIDL_GrayscaleColor(COLOR clr, AGIDL_CLR_FMT fmt);




#endif