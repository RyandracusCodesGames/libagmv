#ifndef AGMV_PLAYBACK_H
#define AGMV_PLAYBACK_H

/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_playback.h
*   Date: 5/19/2024
*   Version: 1.0
*   Updated: 6/2/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agmv_defines.h>

#include <stdio.h>

/*-------------AGMV PLAYBACK UTILITY FUNCTIONS-------------------*/

void AGMV_ResetVideo(FILE* file, AGMV* agmv);
Bool AGMV_IsVideoDone(const AGMV* agmv);
void AGMV_SkipForwards(FILE* file, AGMV* agmv, int n);
void AGMV_SkipForwardsAndDecodeAudio(FILE* file, AGMV* agmv, int n);
void AGMV_SkipBackwards(FILE* file, AGMV* agmv, int n);
void AGMV_SkipTo(FILE* file, AGMV* agmv, int n);
void AGMV_PlayAGMV(FILE* file, AGMV* agmv);
void PlotPixel(u32* vram, int x, int y, int w, int h, u32 color);
void AGMV_DisplayFrame(u32* vram, u16 width, u16 height, const AGMV* agmv);

#endif
