/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_playback.c
*   Date: 5/19/2024
*   Version: 2.0
*   Updated: 9/24/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <stdlib.h>
#include <agmv_playback.h>
#include <agmv_utils.h>
#include <agmv_decode.h>

void ObliterateAGMV(AGMV* agmv){
	if(agmv != NULL){
		
		if(agmv->frame != NULL){
			
			if(agmv->frame->img_data != NULL){
				free(agmv->frame->img_data);
				agmv->frame->img_data = NULL;
			}
			
			free(agmv->frame);
			agmv->frame = NULL;
		}
		if(agmv->iframe != NULL){
			
			if(agmv->iframe->img_data != NULL){
				free(agmv->iframe->img_data);
				agmv->iframe->img_data = NULL;
			}
		
			free(agmv->iframe);
			agmv->iframe = NULL;
		}
		if(agmv->prev_frame != NULL){
			
			if(agmv->prev_frame->img_data != NULL){
				free(agmv->prev_frame->img_data);
				agmv->prev_frame->img_data = NULL;
			}
			
			free(agmv->prev_frame);
			agmv->prev_frame = NULL;
		}
		

		if(agmv->bitstream != NULL){
			if(agmv->bitstream->data != NULL){
				free(agmv->bitstream->data);
			}
			free(agmv->bitstream);
			agmv->bitstream = NULL;
		}
		
		if(agmv->frame_chunk != NULL){
			free(agmv->frame_chunk);
		}
		
		if(agmv->header.total_audio_duration != 0){
			agmv->header.total_audio_duration = 0;
			
			if(agmv->header.bits_per_sample == 16){
				if(agmv->audio_track->pcm != NULL){
					free(agmv->audio_track->pcm);
					agmv->audio_track->pcm = NULL;
				}
			}
			else{
				if(agmv->audio_track->pcm8 != NULL){
					free(agmv->audio_track->pcm8);
					agmv->audio_track->pcm8 = NULL;
				}
			}
			
			if(agmv->audio_chunk->atsample != NULL){
				free(agmv->audio_chunk->atsample);
				agmv->audio_chunk->atsample = NULL;
			}
		}
	
		agmv->frame_chunk = NULL;
		
		if(agmv->audio_chunk != NULL){
			free(agmv->audio_chunk);
		}
		if(agmv->audio_track != NULL){
			free(agmv->audio_track);
		}
		
		agmv->audio_chunk = NULL;
		agmv->audio_track = NULL;
		
		free(agmv);

		agmv = NULL;
	}
}

void AGMV_AllocResources(AGMV* agmv){
	agmv->frame_chunk = (AGMV_FRAME_CHUNK*)malloc(sizeof(AGMV_FRAME_CHUNK));
	agmv->audio_chunk = (AGMV_AUDIO_CHUNK*)malloc(sizeof(AGMV_AUDIO_CHUNK));
	
	agmv->frame  = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->iframe = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->prev_frame = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	
	agmv->frame->width  = agmv->header.width;
	agmv->iframe->width = agmv->header.width;
	agmv->prev_frame->width = agmv->header.width;
	
	agmv->frame->height  = agmv->header.height;
	agmv->iframe->height = agmv->header.height;
	agmv->prev_frame->height = agmv->header.height;

	agmv->frame->img_data  = (u32*)malloc(sizeof(u32)*agmv->frame->width*agmv->frame->height);
	agmv->iframe->img_data = (u32*)malloc(sizeof(u32)*agmv->frame->width*agmv->frame->height);
	agmv->prev_frame->img_data = (u32*)malloc(sizeof(u32)*agmv->frame->width*agmv->frame->height);
	
	agmv->bitstream = (AGMV_BITSTREAM*)malloc(sizeof(AGMV_BITSTREAM));
	agmv->bitstream->len = agmv->frame->width*agmv->frame->height*2;
	agmv->bitstream->data = (u8*)malloc(agmv->bitstream->len);
	agmv->bitstream->pos = 0;
	
	agmv->audio_track = (AGMV_AUDIO_TRACK*)malloc(sizeof(AGMV_AUDIO_TRACK));
	agmv->audio_track->pcm = NULL;
	agmv->audio_track->pcm8 = NULL;
	agmv->audio_chunk->atsample = NULL;
	
	agmv->frame_count = 0;
	agmv->audio_track->start_point = 0;
	agmv->pframe_count = 0;
	
	agmv->prev_fill_color = 0;
	agmv->vq2color1 = 0;
	agmv->vq2color2 = 0;
	
	if(AGMV_GetTotalAudioDuration(agmv) != 0){
		u32 sample_size = AGMV_GetAudioSize(agmv)/(f32)AGMV_GetNumberOfFrames(agmv);
		
		agmv->audio_chunk->size = sample_size;
		
		if(AGMV_GetBitsPerSample(agmv) == 16){
			agmv->audio_track->pcm = (u16*)malloc(sizeof(u16)*sample_size+5);
			agmv->audio_chunk->atsample = (u8*)malloc(sizeof(u8)*sample_size+5);
		}
		else{
			agmv->audio_track->pcm8 = (u8*)malloc(sizeof(u8)*sample_size+5);
		}
	}
}

AGMV* AGMV_Open(const char* filename){
	AGMV* agmv = (AGMV*)malloc(sizeof(AGMV));
	agmv->file  = fopen(filename,"rb");
	agmv->error = AGMV_DecodeHeader(agmv->file,agmv);
	
	if(agmv->file == NULL){
		agmv->error = FILE_NOT_FOUND_ERR;
		fclose(agmv->file);
		return agmv;
	}
	
	if(agmv->error != NO_ERR){
		return agmv;
	}
	
	AGMV_AllocResources(agmv);
	AGMV_SetVolume(agmv,1.0f);
	AGMV_SetAudioState(agmv,AGMV_AUDIO_PLAY);
	AGMV_SetVideoState(agmv,AGMV_VIDEO_PLAY);
	
	agmv->info = AGMV_GetVideoInfo(agmv);
	
	return agmv;
}

void AGMV_Close(AGMV* agmv){
	fclose(agmv->file);
	ObliterateAGMV(agmv);
}

void AGMV_SetAudioState(AGMV* agmv, AGMV_AUDIO_STATE audio){
	agmv->audio_state = audio;
}

void AGMV_SetVideoState(AGMV* agmv, AGMV_VIDEO_STATE video){
	agmv->video_state = video;
}

AGMV_AUDIO_STATE AGMV_GetAudioState(AGMV* agmv){
	return agmv->audio_state;
}

AGMV_VIDEO_STATE AGMV_GetVideoState(AGMV* agmv){
	return agmv->video_state;
}

void AGMV_PlayVideo(AGMV* agmv){
	if(agmv->video_state != AGMV_VIDEO_PLAY){
		agmv->video_state = AGMV_VIDEO_PLAY;
	}
	
	if(agmv->audio_state != AGMV_AUDIO_DISABLE){
		if(agmv->audio_state != AGMV_AUDIO_PLAY){
			agmv->audio_state = AGMV_AUDIO_PLAY;
		}
	}
}

void AGMV_PauseVideo(AGMV* agmv){
	if(agmv->video_state != AGMV_VIDEO_PAUSE){
		agmv->video_state = AGMV_VIDEO_PAUSE;
	}
	
	if(agmv->audio_state != AGMV_AUDIO_DISABLE){
		if(agmv->audio_state != AGMV_AUDIO_PAUSE){
			agmv->audio_state = AGMV_AUDIO_PAUSE;
		}
	}
}

void AGMV_DisableAllAudio(AGMV* agmv){
	if(agmv->audio_state != AGMV_AUDIO_DISABLE){
		agmv->audio_state = AGMV_AUDIO_DISABLE;
	}
}

void AGMV_MuteAudio(AGMV* agmv){
	if(agmv->audio_state != AGMV_AUDIO_PAUSE){
		agmv->audio_state = AGMV_AUDIO_PAUSE;
	}
}

void AGMV_StoreFrameOffset(AGMV* agmv){
	agmv->offset_table[agmv->frame_count] = ftell(agmv->file);
}

void AGMV_StreamMovie(AGMV* agmv){
	if(agmv->video_state == AGMV_VIDEO_PLAY){
		if(!AGMV_IsVideoDone(agmv)){
			AGMV_FindNextFrameChunk(agmv->file);
			AGMV_StoreFrameOffset(agmv);
			AGMV_DecodeFrameChunk(agmv->file,agmv);
			
			if(AGMV_GetTotalAudioDuration(agmv) != 0){
				AGMV_FindNextAudioChunk(agmv->file);
				
				if(agmv->audio_state == AGMV_AUDIO_PLAY){
					AGMV_DecodeAudioSample(agmv->file,agmv);
					// AGMV_StreamAudioSample(); <- TO IMPLEMENT: PLATFORM DEPENDENT API CALL
				}
				else{
					AGMV_SkipAudioChunk(agmv->file);
				}
			}
		}
	}
}

void AGMV_ResetVideo(FILE* file, AGMV* agmv){
	u8 version = AGMV_GetVersion(agmv);
	if(version == 1 || version == 3 || version == 5 || version == 7){
		fseek(file,1574,SEEK_SET);
	}
	else{
		fseek(file,0,SEEK_SET);
	}
	agmv->prev_fill_color = 0;
	agmv->vq2color1 = 0;
	agmv->vq2color2 = 0;
	agmv->frame_count = 0;
	agmv->pframe_count = 0;
}

Bool AGMV_IsVideoDone(AGMV* agmv){
	if(agmv->frame_count >= AGMV_GetNumberOfFrames(agmv)){
		return TRUE;
	}
	else return FALSE;
}

void AGMV_SkipForwards(FILE* file, AGMV* agmv, int n){
	u32 i;
	u16 ftype;
	u32* offset_table = agmv->offset_table, frame_count = agmv->frame_count;
	u32 num_of_frames = AGMV_GetNumberOfFrames(agmv);
	
	agmv->pframe_count = 0;
	
	if(frame_count + n >= num_of_frames){
		n = num_of_frames - frame_count;
	}
	
	if(AGMV_GetTotalAudioDuration(agmv) != 0){
		for(i = 0; i < n; i++){
			AGMV_FindNextFrameChunk(file);
			offset_table[frame_count++] = ftell(file);
			ftype = AGMV_SkipFrameChunk(file);
			AGMV_FindNextAudioChunk(file);
			AGMV_SkipAudioChunk(file);
		}
		
		while(ftype != AGMV_IFRAME && frame_count < num_of_frames){
			AGMV_FindNextFrameChunk(file);
			offset_table[frame_count++] = ftell(file);
			ftype = AGMV_SkipFrameChunk(file);
			AGMV_FindNextAudioChunk(file);
			AGMV_SkipAudioChunk(file);
		}
		
		agmv->frame_count = frame_count - 1;
		
		if(AGMV_EOF(file)){
			agmv->frame_count = 1;
		}
		
		fseek(file,offset_table[agmv->frame_count],SEEK_SET);
	}
	else{
		for(i = 0; i < n; i++){
			AGMV_FindNextFrameChunk(file);
			offset_table[frame_count++] = ftell(file);
			ftype = AGMV_SkipFrameChunk(file);
		}
		
		while(ftype != AGMV_IFRAME && frame_count < num_of_frames){
			AGMV_FindNextFrameChunk(file);
			offset_table[frame_count++] = ftell(file);
			ftype = AGMV_SkipFrameChunk(file);
		}
		
		agmv->frame_count = frame_count - 1;
		
		if(AGMV_EOF(file)){
			agmv->frame_count = 1;
		}
		
		fseek(file,offset_table[agmv->frame_count],SEEK_SET);
	}
}

void AGMV_SkipForwardsAndDecodeAudio(FILE* file, AGMV* agmv, int n){
	u32 i, ftype, *offset_table, frame_count, num_of_frames;
	
	offset_table = agmv->offset_table;
	frame_count  = agmv->frame_count;
	num_of_frames = AGMV_GetNumberOfFrames(agmv);
	
	if(frame_count + n >= num_of_frames){
		n = num_of_frames - frame_count;
	}
	
	if(AGMV_GetTotalAudioDuration(agmv) != 0){
		for(i = 0; i < n; i++){
			AGMV_FindNextFrameChunk(file);
			offset_table[frame_count++] = ftell(file);
			ftype = AGMV_SkipFrameChunk(file);
			AGMV_FindNextAudioChunk(file);
			AGMV_DecodeAudioChunk(file,agmv);
		}
		
		while(ftype != AGMV_IFRAME && frame_count < num_of_frames){
			AGMV_FindNextFrameChunk(file);
			offset_table[frame_count++] = ftell(file);
			ftype = AGMV_SkipFrameChunk(file);
			AGMV_FindNextAudioChunk(file);
			AGMV_SkipAudioChunk(file);
		}
		
		agmv->frame_count = frame_count - 1;
		
		fseek(file,offset_table[agmv->frame_count],SEEK_SET);
	}
	else{
		for(i = 0; i < n; i++){
			AGMV_FindNextFrameChunk(file);
			offset_table[frame_count++] = ftell(file);
			ftype = AGMV_SkipFrameChunk(file);
		}
		
		while(ftype != AGMV_IFRAME && frame_count < num_of_frames){
			AGMV_FindNextFrameChunk(file);
			offset_table[frame_count++] = ftell(file);
			ftype = AGMV_SkipFrameChunk(file);
		}
		
		agmv->frame_count = frame_count - 1;
		
		fseek(file,offset_table[agmv->frame_count],SEEK_SET);
	}
}

void AGMV_SkipBackwards(FILE* file, AGMV* agmv, int n){
	u16 ftype;
	u32 offset, num_of_frames, *offset_table;
	int frame_count;
	
	agmv->pframe_count = 0;
	
	num_of_frames = AGMV_GetNumberOfFrames(agmv);
	frame_count = agmv->frame_count;	
	frame_count -= n;
	if(frame_count < 0){
		frame_count = 0;
	}
	offset = frame_count;
	offset_table = agmv->offset_table;
	
	fseek(file,offset_table[frame_count],SEEK_SET);
	
	AGMV_FindNextFrameChunk(file);
	ftype = AGMV_SkipFrameChunk(file);
	
	if(AGMV_GetTotalAudioDuration(agmv) != 0){
		while(ftype != AGMV_IFRAME && offset < num_of_frames){
			AGMV_FindNextFrameChunk(file);
			ftype = AGMV_SkipFrameChunk(file);
			AGMV_FindNextAudioChunk(file);
			AGMV_SkipAudioChunk(file);
			offset++;
		}
	}
	else{
		while(ftype != AGMV_IFRAME && offset < num_of_frames){
			AGMV_FindNextFrameChunk(file);
			ftype = AGMV_SkipFrameChunk(file);
			offset++;
		}
	}
	
	fseek(file,offset_table[offset],SEEK_SET);
	agmv->frame_count = offset;
}

/* ONLY CALL SKIP TO FUNCTION AFTER ALL FRAMES HAVE BEEN READ */

void AGMV_SkipTo(FILE* file, AGMV* agmv, int n){
	u32* offset_table = agmv->offset_table;
	u32 ftype, offset = n, num_of_frames = AGMV_GetNumberOfFrames(agmv);
	
	agmv->pframe_count = 0;
	
	if(n >= 0 && n < num_of_frames){
		fseek(file,offset_table[n],SEEK_SET);
		
		AGMV_FindNextFrameChunk(file);
		ftype = AGMV_SkipFrameChunk(file);
		
		if(AGMV_GetTotalAudioDuration(agmv) != 0){
			while(ftype != AGMV_IFRAME && offset < num_of_frames){
				AGMV_FindNextFrameChunk(file);
				ftype = AGMV_SkipFrameChunk(file);
				AGMV_FindNextAudioChunk(file);
				AGMV_SkipAudioChunk(file);
				offset++;
			}
		}
		else{
			while(ftype != AGMV_IFRAME && offset < num_of_frames){
				AGMV_FindNextFrameChunk(file);
				ftype = AGMV_SkipFrameChunk(file);
				offset++;
			}
		}
		
		fseek(file,offset_table[offset],SEEK_SET);
		agmv->frame_count = offset;
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