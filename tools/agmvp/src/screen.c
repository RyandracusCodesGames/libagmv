/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   Program: agmvp.exe
*   File: screen.c
*   Date: 3/29/2024
*   Version: 2.0
*   Updated: 8/2/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include "screen.h"
#include <stdlib.h>
#include <agidl_mmu_utils.h>

Bitmap* AGMV_CreateBitmap(u32 width, u32 height, AGIDL_CLR_FMT fmt){
	Bitmap* bitmap = (Bitmap*)malloc(sizeof(Bitmap));
	bitmap->width = width;
	bitmap->height = height;
	bitmap->vram = (u32*)AGIDL_AllocImgDataMMU(width,height,fmt);
	return bitmap;
}

Screen* AGMV_CreateScreen(int x, int y, u32 width, u32 height, AGIDL_CLR_FMT fmt){
	Screen* screen = (Screen*)malloc(sizeof(Screen));
	screen->x = x;
	screen->y = y;
	screen->width = width;
	screen->height = height;
	screen->fmt = fmt;
	screen->bitmap = AGMV_CreateBitmap(width,height,fmt);
	screen->IsFullscreen = FALSE;
	screen->IsHidingHUD = FALSE;
	screen->IsPlay = FALSE;
	screen->OpenFile = FALSE;
	screen->HasVideo = FALSE;
	return screen;
}

void AGMV_InitHUD(AGIDL_ATTR* attr, int tx, int ty, f32 sx, f32 sy, AGIDL_Bool scale, AGIDL_Bool chroma, u32 chromakey, AGIDL_Bool fh, AGIDL_Bool fv){
	attr->tx = tx;
	attr->ty = ty;
	attr->sx = sx;
	attr->sy = sy;
	attr->enableScale = scale;
	attr->enableChroma = chroma;
	attr->chromaKey = chromakey;
	attr->flipHorz = fh;
	attr->flipVert = fv;
}

void AGMV_FillScreen(Screen* screen, u32 color){
	u32* vram, size;
	
	vram = screen->bitmap->vram;
	size = screen->width*screen->height;
	
	int i;
	for(i = 0; i < size; i++){
		*(vram++) = color;
	}
}

void AGMV_SetScreenPixel(Screen* screen, int x, int y, u32 color){
	if(x >= 0 && y >= 0 && x < screen->width && y < screen->height){
		screen->bitmap->vram[x+y*screen->width] = color;
	}
}

void AGMV_FillRect(Screen* screen, int x, int y, u32 width, u32 height, u32 color){
	int i,j;
	for(j = y; j <= y + height; j++){
		for(i = x; i <= x + width; i++){
			AGMV_SetScreenPixel(screen,i,j,color);
		}
	}
}

void AGMV_FreeBitmap(Bitmap* bitmap){
	if(bitmap != NULL){
		free(bitmap->vram);
		free(bitmap);
		bitmap = NULL;
	}
}

void AGMV_FreeScreen(Screen* screen){
	if(screen != NULL){
		AGMV_FreeBitmap(screen->bitmap);
		free(screen);
		screen = NULL;
	}
}