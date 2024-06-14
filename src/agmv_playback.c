/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_playback.c
*   Date: 5/19/2024
*   Version: 1.1
*   Updated: 6/12/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <agmv_playback.h>
#include <agmv_utils.h>
#include <agmv_decode.h>

void AGMV_ResetVideo(FILE* file, AGMV* agmv){
	if(AGMV_GetVersion(agmv) == 1){
		fseek(file,1574,SEEK_SET);
	}
	else{
		fseek(file,806,SEEK_SET);
	}
	agmv->frame_count = 0;
}

Bool AGMV_IsVideoDone(AGMV* agmv){
	if(agmv->frame_count >= AGMV_GetNumberOfFrames(agmv)){
		return TRUE;
	}
	else return FALSE;
}

void AGMV_SkipForwards(FILE* file, AGMV* agmv, int n){
	if(AGMV_GetTotalAudioDuration(agmv) != 0){
		n = AGMV_NextIFrame(n,agmv->frame_count);
		int i;
		for(i = 0; i < n; i++){
			AGMV_FindNextFrameChunk(file);
			agmv->offset_table[agmv->frame_count++] = ftell(file);
			AGMV_SkipFrameChunk(file);
			AGMV_FindNextAudioChunk(file);
			AGMV_SkipAudioChunk(file);
		}
	}
	else{
		n = AGMV_NextIFrame(n,agmv->frame_count);
		int i;
		for(i = 0; i < n; i++){
			AGMV_FindNextFrameChunk(file);
			agmv->offset_table[agmv->frame_count++] = ftell(file);
			AGMV_SkipFrameChunk(file);
		}
	}
}

void AGMV_SkipForwardsAndDecodeAudio(FILE* file, AGMV* agmv, int n){
	if(AGMV_GetTotalAudioDuration(agmv) != 0){
		n = AGMV_NextIFrame(n,agmv->frame_count);
		int i;
		for(i = 0; i < n; i++){
			AGMV_FindNextFrameChunk(file);
			agmv->offset_table[agmv->frame_count++] = ftell(file);
			AGMV_SkipFrameChunk(file);
			AGMV_FindNextAudioChunk(file);
			AGMV_DecodeAudioChunk(file,agmv);
		}
	}
	else{
		n = AGMV_NextIFrame(n,agmv->frame_count);
		int i;
		for(i = 0; i < n; i++){
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

void PlotPixel(u32* vram, int x, int y, int w, int h, u32 color){
	if(x >= 0 && y >= 0 && x < w && y < h){
		vram[x+y*w] = color;
	}
}

void AGMV_DisplayFrame(u32* vram, u16 width, u16 height, AGMV* agmv){
	u32 fwidth = agmv->frame->width, fheight = agmv->frame->height, offset;
	u32* img_data = agmv->frame->img_data;
	
	int x,y;
	for(y = 0; y < fheight; y++){
		offset = y * fwidth;
		for(x = 0; x < fwidth; x++){
			PlotPixel(vram,x,y,width,height,img_data[x+offset]);
		}
	}
}