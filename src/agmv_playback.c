/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_playback.c
*   Date: 5/19/2024
*   Version: 1.0
*   Updated: 6/2/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agmv_playback.h>

#include <agmv_decode.h>
#include <agmv_utils.h>

void AGMV_ResetVideo(FILE* file, AGMV* agmv){
	if(AGMV_GetVersion(agmv) == 1){
		fseek(file,1572,SEEK_SET);
	}
	else{
		fseek(file,804,SEEK_SET);
	}
	agmv->frame_count = 0;
}

Bool AGMV_IsVideoDone(const AGMV* agmv){
	if(agmv->frame_count >= AGMV_GetNumberOfFrames(agmv)){
		return TRUE;
	}
	return FALSE;
}

void AGMV_SkipForwards(FILE* file, AGMV* agmv, int n){
	if(AGMV_GetTotalAudioDuration(agmv) != 0){
		n = AGMV_NextIFrame(n,agmv->frame_count);
		for(int i = 0; i < n; i++){
			AGMV_FindNextFrameChunk(file);
			agmv->offset_table[agmv->frame_count++] = ftell(file);
			AGMV_SkipFrameChunk(file);
			AGMV_FindNextAudioChunk(file);
			AGMV_SkipAudioChunk(file);
		}
	}
	else{
		n = AGMV_NextIFrame(n,agmv->frame_count);
		for(int i = 0; i < n; i++){
			AGMV_FindNextFrameChunk(file);
			agmv->offset_table[agmv->frame_count++] = ftell(file);
			AGMV_SkipFrameChunk(file);
		}
	}
}

void AGMV_SkipForwardsAndDecodeAudio(FILE* file, AGMV* agmv, int n){
	if(AGMV_GetTotalAudioDuration(agmv) != 0){
		n = AGMV_NextIFrame(n,agmv->frame_count);
		for(int i = 0; i < n; i++){
			AGMV_FindNextFrameChunk(file);
			agmv->offset_table[agmv->frame_count++] = ftell(file);
			AGMV_SkipFrameChunk(file);
			AGMV_FindNextAudioChunk(file);
			AGMV_DecodeAudioChunk(file,agmv);
		}
	}
	else{
		n = AGMV_NextIFrame(n,agmv->frame_count);
		for(int i = 0; i < n; i++){
			AGMV_FindNextFrameChunk(file);
			agmv->offset_table[agmv->frame_count++] = ftell(file);
			AGMV_SkipFrameChunk(file);
		}
	}
}

void AGMV_SkipBackwards(FILE* file, AGMV* agmv, int n){
	n = AGMV_PrevIFrame(n,agmv->frame_count);
	int frame_count = agmv->frame_count;
	frame_count -= n;
	if(frame_count < 0){
		agmv->frame_count = 0;
	}
	agmv->frame_count = frame_count;
	fseek(file,agmv->offset_table[agmv->frame_count],SEEK_SET);
}

/* ONLY CALL SKIP TO FUNCTION AFTER ALL FRAMES HAVE BEEN READ */

void AGMV_SkipTo(FILE* file, AGMV* agmv, int n){
	n = AGMV_SkipToNearestIFrame(n);
	if(n >= 0 && n < AGMV_GetNumberOfFrames(agmv)){
		fseek(file,agmv->offset_table[n],SEEK_SET);
		agmv->frame_count = n;
	}
}

void AGMV_PlayAGMV(FILE* file, AGMV* agmv){
	if(AGMV_GetTotalAudioDuration(agmv) != 0){
		AGMV_FindNextFrameChunk(file);
		agmv->offset_table[agmv->frame_count] = ftell(file);
		AGMV_DecodeFrameChunk(file,agmv);
		AGMV_FindNextAudioChunk(file);
		AGMV_SkipAudioChunk(file);
	}
	else{
		AGMV_FindNextFrameChunk(file);
		agmv->offset_table[agmv->frame_count] = ftell(file);
		AGMV_DecodeFrameChunk(file,agmv);
	}
}

void PlotPixel(u32* vram, const int x, const int y, const int w, const int h, const u32 color){
	if(x >= 0 && y >= 0 && x < w && y < h){
		vram[x+y*w] = color;
	}
}

void AGMV_DisplayFrame(u32* vram, const u16 width, const u16 height, const AGMV* agmv){
	const u32 fwidth = agmv->frame->width;
	const u32 fheight = agmv->frame->height;
	const u32* img_data = agmv->frame->img_data;

	for(int y = 0; y < fheight; y++){
		const u32 offset = y * fwidth;
		for(int x = 0; x < fwidth; x++){
			PlotPixel(vram,x,y,width,height,img_data[x+offset]);
		}
	}
}
