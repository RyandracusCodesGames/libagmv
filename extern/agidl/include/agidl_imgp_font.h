#ifndef AGIDL_IMGP_FONT_H
#define AGIDL_IMGP_FONT_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_font.h
*   Date: 12/25/2023
*   Version: 0.2b
*   Updated: 1/19/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_cc_types.h>
#include <agidl_types.h>

#define FONT_MODE_IMAGE = 0x1
#define FONT_MODE_FILE = 0x2

typedef struct AGIDL_GLYPH{
	char c;
	u32 width;
	u32 height;
	void* img_data;
}AGIDL_GLYPH;

typedef struct AGIDL_GlyphNode{
	AGIDL_GLYPH glyph;
	struct AGIDL_GlyphNode* next;
}AGIDL_GlyphNode;

typedef struct AGIDL_GlyphList{
	AGIDL_GlyphNode* head;
	int len;
}AGIDL_GlyphList;

typedef struct AGIDL_FONT{
	AGIDL_GlyphList *list;
	u32 width;
	u32 height;
	AGIDL_CLR_FMT fmt;
	u16 num_of_letters;
	int mode;
}AGIDL_FONT;

AGIDL_GlyphList* AGIDL_CreateGlyphList();
AGIDL_GlyphNode* AGIDL_CreateGlyphNode(char c, void* data, u32 width, u32 height);
u32 AGIDL_GetGlyphListLength(const AGIDL_GlyphList* list);
void AGIDL_AddNode(AGIDL_GlyphList* list, char c, void* data, u32 width, u32 height);
int AGIDL_DeleteNode(AGIDL_GlyphList* list, char c);
int AGIDL_FindNode(const AGIDL_GlyphList* list, char c);
void AGIDL_Pop(AGIDL_GlyphList* list);
void AGIDL_PrintGlyphList(const AGIDL_GlyphList* list);
void AGIDL_DestroyGlyphList(AGIDL_GlyphList* list);
AGIDL_FONT* AGIDL_CreateFont(u32 width, u32 height, AGIDL_CLR_FMT fmt);
void AGIDL_DestroyFont(AGIDL_FONT* font);
int AGIDL_IsLetterPresent(AGIDL_FONT* font);
void AGIDL_SetFontColor(AGIDL_FONT* font, COLOR clr);
void AGIDL_AddLetter(AGIDL_FONT* font, char c, const void* data);
void AGIDL_DeleteLetter(AGIDL_FONT* font, char c);
AGIDL_GLYPH AGIDL_FindLetter(const AGIDL_FONT* font, char c);
void AGIDL_PrintFont(const AGIDL_FONT* font);
void AGIDL_ExportFont(AGIDL_FONT* font);
void AGIDL_SetExportMode(AGIDL_FONT* font, int mode);
int AGIDL_GetFontLen(const AGIDL_FONT* font);
void AGIDL_BlitFont(const AGIDL_FONT* font, char c, void* data, u32 x, u32 y, u32 width, u32 height, AGIDL_CLR_FMT fmt);
void AGIDL_FontPrint(const char* string, void* data, u32 width, u32 height, AGIDL_CLR_FMT fmt, const AGIDL_FONT* font);
void AGIDL_LoadFont(AGIDL_FONT* font);
AGIDL_FONT* AGIDL_GenerateDefaultFont();

#endif
