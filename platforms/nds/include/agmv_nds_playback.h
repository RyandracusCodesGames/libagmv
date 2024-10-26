#ifndef AGMV_NDS_PLAYBACK_H
#define AGMV_NDS_PLAYBACK_H

/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_nds_playback.h
*   Date: 8/14/2024
*   Version: 2.0
*   Updated: 8/14/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <agmv_nds_defines.h>

/*-------------INITIALIZE RESOURCES AND ATTRIBUTES FOR VIDEO PLAYBACK-------------------*/
void ObliterateAGMV(AGMV* agmv);
void AGMV_AllocResources(AGMV* agmv);
AGMV* AGMV_Open(char* filename);
void AGMV_Close(AGMV* agmv);
AGMV_INFO AGMV_GetVideoInfo(AGMV* agmv);
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
void AGMV_SetVolume(AGMV* agmv, u32 volume);
/*--------------PRIMARY AUDIO AND VIDEO PLAYBACK ROUTINE------------------*/
void ITCM_CODE AGMV_StreamMovie(AGMV* agmv);
/*-------------AGMV PLAYBACK UTILITY FUNCTIONS-------------------*/
void AGMV_ResetVideo(FILE* file, AGMV* agmv);
Bool AGMV_IsVideoDone(AGMV* agmv);
void AGMV_SkipForwards(FILE* file, AGMV* agmv, int n);
void AGMV_SkipForwardsAndDecodeAudio(FILE* file, AGMV* agmv, int n);
void AGMV_SkipBackwards(FILE* file, AGMV* agmv, int n);
void AGMV_SkipTo(FILE* file, AGMV* agmv, int n);
void ITCM_CODE AGMV_DisplayFrame(u16* vram, u16 width, u16 height, AGMV* agmv);
#endif