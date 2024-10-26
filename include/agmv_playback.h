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
*   Version: 2.0
*   Updated: 8/6/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <stdio.h>
#include <agmv_defines.h>

/*-------------INITIALIZE RESOURCES AND ATTRIBUTES FOR VIDEO PLAYBACK-------------------*/
void ObliterateAGMV(AGMV* agmv);
void AGMV_AllocResources(AGMV* agmv);
AGMV* AGMV_Open(const char* filename);
void AGMV_Close(AGMV* agmv);
/*-------------SET STATE VARIABLES OF AUDIO AND VIDEO-------------------*/
void AGMV_SetAudioState(AGMV* agmv, AGMV_AUDIO_STATE audio);
void AGMV_SetVideoState(AGMV* agmv, AGMV_VIDEO_STATE video);
AGMV_AUDIO_STATE AGMV_GetAudioState(AGMV* agmv);
AGMV_VIDEO_STATE AGMV_GetVideoState(AGMV* agmv);
void AGMV_PlayVideo(AGMV* agmv);
void AGMV_PauseVideo(AGMV* agmv);
void AGMV_DisableAllAudio(AGMV* agmv);
void AGMV_MuteAudio(AGMV* agmv);
void AGMV_StoreFrameOffset(AGMV* agmv);
/*--------------PRIMARY AUDIO AND VIDEO PLAYBACK ROUTINE------------------*/
void AGMV_StreamMovie(AGMV* agmv);
/*-------------AGMV PLAYBACK UTILITY FUNCTIONS-------------------*/
void AGMV_ResetVideo(FILE* file, AGMV* agmv);
Bool AGMV_IsVideoDone(AGMV* agmv);
void AGMV_SkipForwards(FILE* file, AGMV* agmv, int n);
void AGMV_SkipForwardsAndDecodeAudio(FILE* file, AGMV* agmv, int n);
void AGMV_SkipBackwards(FILE* file, AGMV* agmv, int n);
void AGMV_SkipTo(FILE* file, AGMV* agmv, int n);
void AGMV_PlayAGMV(FILE* file, AGMV* agmv);
void PlotPixel(u32* vram, int x, int y, int w, int h, u32 color);
void AGMV_DisplayFrame(u32* vram, u16 width, u16 height, AGMV* agmv);

#endif