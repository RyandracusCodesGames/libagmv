#ifndef AGMV_PSX_PLAYBACK_H
#define AGMV_PSX_PLAYBACK_H

/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_psx_playback.h
*   Date: 10/4/2024
*   Version: 2.0
*   Updated: 10/25/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agmv_psx_defines.h>

/*-------------INITIALIZE RESOURCES AND ATTRIBUTES FOR VIDEO PLAYBACK-------------------*/
void ObliterateAGMV(AGMV* agmv);
void AGMV_AllocResources(AGMV* agmv);
AGMV* AGMV_Open(char* filename);
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
void AGMV_SetBufferFlip(AGMV* agmv, Bool boolean);
void AGMV_InitScreen(AGMV* agmv, u16 x, u16 y, u16 width, u16 height);
void AGMV_SetNextPrim(AGMV* agmv, char* nextprim);

static inline void AGMV_StoreFrameOffset(AGMV* agmv, u32 offset){
	if(agmv->frame_chunk->frame_type == AGMV_IFRAME){
		agmv->offset_table[agmv->iframe_count].offset = offset;
		agmv->offset_table[agmv->iframe_count].frame_count = agmv->frame_count + 1;
		agmv->iframe_count++;
	}
}

AGMV_INFO AGMV_GetVideoInfo(AGMV* agmv);
void AGMV_SetVolume(AGMV* agmv, u8 volume);
void AGMV_SetImageBufferCoords(AGMV* agmv, u16 x, u16 y);
void AGMV_SetIntraFrameCoords(AGMV* agmv, u16 x, u16 y);
void AGMV_SetPrevFrameCoords(AGMV* agmv, u16 x, u16 y);
void AGMV_WaitUntilButtonReleased(int button);
/*--------------PRIMARY AUDIO AND VIDEO PLAYBACK ROUTINE------------------*/
void AGMV_StreamMovie(AGMV* agmv);
/*-------------AGMV PLAYBACK UTILITY FUNCTIONS-------------------*/
void AGMV_ResetVideo(File* file, AGMV* agmv);

static inline Bool AGMV_IsVideoDone(AGMV* agmv){
	if(agmv->frame_count + 1 >= agmv->header.num_of_frames){
		return AGMV_TRUE;
	}
	else return AGMV_FALSE;
}

void AGMV_SkipForwards(File* file, AGMV* agmv, int n);
void AGMV_SkipForwardsAndDecodeAudio(File* file, AGMV* agmv, int n);
void AGMV_SkipBackwards(File* file, AGMV* agmv, int n);
void AGMV_SkipTo(File* file, AGMV* agmv, int n);
void AGMV_PlayAGMV(File* file, AGMV* agmv);
void AGMV_DisplayFrame(u16* vram, u16 width, u16 height, AGMV* agmv);

#endif