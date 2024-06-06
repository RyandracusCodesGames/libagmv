#ifndef SCREEN_H
#define SCREEN_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   Program: agmvp.exe
*   File: screen.h
*   Date: 3/29/2024
*   Version: 0.4b
*   Updated: 6/3/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_types.h>
#include <agidl_cc_types.h>
#include <agidl_gd_display.h>

#define A_PRESSED          65
#define D_PRESSED          68
#define ESC_PRESSED        27
#define SPACE_PRESSED      32
#define ENTER_PRESSED      13
#define H_PRESSED        0x48
#define ARROW_UP_PRESSED   38
#define ARROW_DOWN_PRESSED 40
#define M_PRESSED 0x4D

typedef struct Bitmap{
	u32 width;
	u32 height;
	u32* vram;
}Bitmap;

typedef struct Screen{
	int x;
	int y;
	u32 width;
	u32 height;
	AGIDL_CLR_FMT fmt;
	AGIDL_Bool IsFullscreen;
	AGIDL_Bool IsHidingHUD;
	AGIDL_Bool IsPlay;
	AGIDL_Bool OpenFile;
	AGIDL_Bool HasVideo;
	AGIDL_Bool HasAudio;
	Bitmap* bitmap;
}Screen;

Bitmap* AGMV_CreateBitmap(u32 width, u32 height, AGIDL_CLR_FMT fmt);
Screen* AGMV_CreateScreen(int x, int y, u32 width, u32 height, AGIDL_CLR_FMT fmt);
void AGMV_FillScreen(Screen* screen, u32 color);
void AGMV_SetScreenPixel(Screen* screen, int x, int y, u32 color);
void AGMV_FillRect(Screen* screen, int x, int y, u32 width, u32 height, u32 color);
void AGMV_FreeBitmap(Bitmap* bitmap);
void AGMV_FreeScreen(Screen* screen);
void AGMV_InitHUD(AGIDL_ATTR* attr, int tx, int ty, f32 sx, f32 sy, AGIDL_Bool scale, AGIDL_Bool chroma, u32 chromakey, AGIDL_Bool fh, AGIDL_Bool fv);

#endif