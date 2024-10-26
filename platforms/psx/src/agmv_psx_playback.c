/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_psx_playback.c
*   Date: 10/4/2024
*   Version: 2.0
*   Updated: 10/26/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <agmv_psx_playback.h>
#include <agmv_psx_decode.h>
#include <agmv_psx_file_utils.h>
#include <agmv_psx_mem_utils.h>

void ObliterateAGMV(AGMV* agmv){
	if(agmv != AGMV_NULL){
		
		if(agmv->header.palette0 != AGMV_NULL){
			AGMV_FreeMemory(agmv->header.palette0);
			agmv->header.palette0 = AGMV_NULL;
		}
		
		if(agmv->header.palette1 != AGMV_NULL){
			AGMV_FreeMemory(agmv->header.palette1);
			agmv->header.palette1 = AGMV_NULL;
		}
		
		if(agmv->frame != AGMV_NULL){
			
			if(agmv->frame->img_data != AGMV_NULL){
				AGMV_FreeMemory(agmv->frame->img_data);
				agmv->frame->img_data = AGMV_NULL;
			}
			
			AGMV_FreeMemory(agmv->frame);
			agmv->frame = AGMV_NULL;
		}
		if(agmv->iframe != AGMV_NULL){
			
			if(agmv->iframe->img_data != AGMV_NULL){
				AGMV_FreeMemory(agmv->iframe->img_data);
				agmv->iframe->img_data = AGMV_NULL;
			}
		
			AGMV_FreeMemory(agmv->iframe);
			agmv->iframe = AGMV_NULL;
		}
		if(agmv->prev_frame != AGMV_NULL){
			
			if(agmv->prev_frame->img_data != AGMV_NULL){
				AGMV_FreeMemory(agmv->prev_frame->img_data);
				agmv->prev_frame->img_data = AGMV_NULL;
			}
			
			AGMV_FreeMemory(agmv->prev_frame);
			agmv->prev_frame = AGMV_NULL;
		}
		

		if(agmv->bitstream != AGMV_NULL){
			if(agmv->bitstream->data != AGMV_NULL){
				AGMV_FreeMemory(agmv->bitstream->data);
				agmv->bitstream->data = AGMV_NULL;
			}
			
			if(agmv->bitstream->cbuffer != AGMV_NULL){
				AGMV_FreeMemory(agmv->bitstream->cbuffer);
				agmv->bitstream->cbuffer = AGMV_NULL;
			}
			
			AGMV_FreeMemory(agmv->bitstream);
			agmv->bitstream = AGMV_NULL;
		}
		
		if(agmv->frame_chunk != AGMV_NULL){
			AGMV_FreeMemory(agmv->frame_chunk);
		}
		
		if(agmv->offset_table != AGMV_NULL){
			AGMV_FreeMemory(agmv->offset_table);
			agmv->offset_table = AGMV_NULL;
		}
		
		if(agmv->header.total_audio_duration != 0){
			agmv->header.total_audio_duration = 0;
			
			if(agmv->header.bits_per_sample == 16){
				if(agmv->audio_track->pcm != AGMV_NULL){
					AGMV_FreeMemory(agmv->audio_track->pcm);
					agmv->audio_track->pcm = AGMV_NULL;
				}
			}
		}
	
		agmv->frame_chunk = AGMV_NULL;
		
		if(agmv->audio_chunk != AGMV_NULL){
			AGMV_FreeMemory(agmv->audio_chunk);
		}
		if(agmv->audio_track != AGMV_NULL){
			AGMV_FreeMemory(agmv->audio_track);
		}
		
		agmv->audio_chunk = AGMV_NULL;
		agmv->audio_track = AGMV_NULL;
		
		AGMV_FreeMemory(agmv);

		agmv = AGMV_NULL;
	}
}

void AGMV_AllocResources(AGMV* agmv){

	agmv->frame_chunk = (AGMV_FRAME_CHUNK*)malloc3(sizeof(AGMV_FRAME_CHUNK));
	agmv->audio_chunk = (AGMV_AUDIO_CHUNK*)malloc3(sizeof(AGMV_AUDIO_CHUNK));
	
	agmv->frame  = (AGMV_FRAME*)malloc3(sizeof(AGMV_FRAME));
	agmv->iframe = (AGMV_FRAME*)malloc3(sizeof(AGMV_FRAME));
	agmv->prev_frame = (AGMV_FRAME*)malloc3(sizeof(AGMV_FRAME));
	
	agmv->frame->width  = agmv->header.width;
	agmv->iframe->width = agmv->header.width;
	agmv->prev_frame->width = agmv->header.width;
	
	agmv->frame->height  = agmv->header.height;
	agmv->iframe->height = agmv->header.height;
	agmv->prev_frame->height = agmv->header.height;

	agmv->frame->img_data  = (u16*)malloc3(sizeof(u16)*agmv->frame->width*agmv->frame->height);
	agmv->iframe->img_data = (u16*)malloc3(sizeof(u16)*agmv->frame->width*agmv->frame->height);
	agmv->prev_frame->img_data = (u16*)malloc3(sizeof(u16)*agmv->frame->width*agmv->frame->height);
	
	agmv->bitstream = (AGMV_BITSTREAM*)malloc3(sizeof(AGMV_BITSTREAM));
	agmv->bitstream->len = agmv->frame->width*agmv->frame->height*2;
	agmv->bitstream->data = (u8*)malloc3(agmv->bitstream->len);
	agmv->bitstream->cbuffer = AGMV_NULL;
	
	if(agmv->header.version == 35 || agmv->header.version == 36 || agmv->header.version == 20){
		agmv->bitstream->cbuffer = (u8*)malloc3(agmv->bitstream->len);
	}
	
	agmv->bitstream->pos = 0;
	
	agmv->audio_track = (AGMV_AUDIO_TRACK*)malloc3(sizeof(AGMV_AUDIO_TRACK));
	agmv->audio_track->pcm = AGMV_NULL;
	
	agmv->frame_count = 0;
	agmv->pframe_count = 0;
	agmv->iframe_count = 0;
	
	agmv->prev_fill_color = 0;
	agmv->vq2color1 = 0;
	agmv->vq2color2 = 0;
	
	agmv->offset_table = (AGMV_OFFSET*)malloc3(sizeof(AGMV_OFFSET)*(agmv->header.num_of_frames/4));
	
	if(agmv->header.total_audio_duration != 0){
		u32 sample_size = agmv->header.audio_size / agmv->header.num_of_frames;
		
		agmv->audio_chunk->size = sample_size;
		
		if(agmv->header.bits_per_sample == 16){
			agmv->audio_track->pcm = (u16*)malloc3(sizeof(u16)*sample_size+5);  
			
			agmv->commonAttributes.mask = (SPU_COMMON_MVOLL | SPU_COMMON_MVOLR); 
			agmv->commonAttributes.mvol.left  = 0x3fff;                         
			agmv->commonAttributes.mvol.right = 0x3fff;                      
			SpuSetCommonAttr(&agmv->commonAttributes);    
			SpuSetIRQ(SPU_OFF);

		    SpuSetReverb(SPU_OFF);
			SpuClearReverbWorkArea(SPU_REV_MODE_CLEAR_WA);
			SpuSetReverbModeDepth(0,0);
			agmv->audio_track->pcm_addr = 0x80000 - (sample_size << 1) + 10;
			SpuSetReverbModeDepth(0x3FFF,0x3FFF);
			SpuSetESA(agmv->audio_track->pcm_addr);
			SpuSetTransferStartAddr(agmv->audio_track->pcm_addr);
		}
	}
}

AGMV_INFO AGMV_GetVideoInfo(AGMV* agmv){
	AGMV_INFO info;
	
	info.width = agmv->header.width;
	info.height = agmv->header.height;
	info.number_of_frames = agmv->header.num_of_frames;
	info.version = agmv->header.version;
	info.total_audio_duration = agmv->header.total_audio_duration;
	info.sample_rate = agmv->header.sample_rate;
	info.audio_size = agmv->header.audio_size;
	info.number_of_channels = agmv->header.num_of_channels;
	info.bits_per_sample = agmv->header.bits_per_sample;
	
	return info;
}

void AGMV_SetVolume(AGMV* agmv, u8 volume){
	agmv->volume = volume;
}

AGMV* AGMV_Open(char* filename){
	AGMV* agmv = (AGMV*)malloc3(sizeof(AGMV));
	agmv->file  = AGMV_OpenFile(filename);
	agmv->header.palette0 = (u16*)malloc3(sizeof(u16)*256);
	agmv->header.palette1 = (u16*)malloc3(sizeof(u16)*256);
	agmv->error = AGMV_DecodeHeader(agmv->file,agmv);
	
	if(agmv->file == AGMV_NULL){
		agmv->error = FILE_NOT_FOUND_ERR;
		AGMV_CloseFile(agmv->file);
		AGMV_FreeMemory(agmv->header.palette0);
		AGMV_FreeMemory(agmv->header.palette1);
		AGMV_FreeMemory(agmv);
		return agmv;
	}
	
	if(agmv->error != NO_ERR){
		AGMV_CloseFile(agmv->file);
		AGMV_FreeMemory(agmv->header.palette0);
		AGMV_FreeMemory(agmv->header.palette1);
		AGMV_FreeMemory(agmv);
		return agmv;
	}
	
	agmv->rect.x = 256 + agmv->header.width + 1;
	agmv->rect.y = 0;
	agmv->rect.w = agmv->header.width;
	agmv->rect.h = agmv->header.height;
	
	agmv->irect.x = 256;
	agmv->irect.y = 0;
	agmv->irect.w = agmv->header.width;
	agmv->irect.h = agmv->header.height;
	
	agmv->prect.x = 256;
	agmv->prect.y = agmv->header.height + 1;
	agmv->prect.w = agmv->header.width;
	agmv->prect.h = agmv->header.height;
	
	agmv->VertPos[0].vx = -32;
	agmv->VertPos[0].vy = -32;
	agmv->VertPos[0].vz =  1;
	
	agmv->VertPos[1].vx = -32;
	agmv->VertPos[1].vy = 32;
	agmv->VertPos[1].vz =  1;
	
	agmv->VertPos[2].vx = 32;
	agmv->VertPos[2].vy = -32;
	agmv->VertPos[2].vz =  1;
	
	agmv->VertPos[3].vx = 32;
	agmv->VertPos[3].vy = 32;
	agmv->VertPos[3].vz =  1;    

	agmv->transVector.vx = 64;
	agmv->transVector.vy = 60;
	agmv->transVector.vz = 128;
	agmv->transVector.pad = 0;
	
	agmv->rotVector.vx = 0;
	agmv->rotVector.vy = 0;
	agmv->rotVector.vz = 0;
	agmv->rotVector.pad = 0;
	
	agmv->scale.vx = 9000;
	agmv->scale.vy = 8192;
	agmv->scale.vz = 4096;
	agmv->scale.pad = 0;
	
	AGMV_AllocResources(agmv);
	AGMV_SetVolume(agmv,127);
	AGMV_SetAudioState(agmv,AGMV_AUDIO_PLAY);
	AGMV_SetVideoState(agmv,AGMV_VIDEO_PLAY);
	AGMV_SetBufferFlip(agmv,AGMV_FALSE);
	
	agmv->info = AGMV_GetVideoInfo(agmv);
	
	return agmv;
}

void AGMV_Close(AGMV* agmv){
	AGMV_CloseFile(agmv->file);
	ObliterateAGMV(agmv);
}

void AGMV_SetAudioState(AGMV* agmv, AGMV_AUDIO_STATE audio){
	agmv->audio_state = audio;
}

void AGMV_SetVideoState(AGMV* agmv, AGMV_VIDEO_STATE video){
	agmv->video_state = video;
}

void AGMV_SetBufferFlip(AGMV* agmv, Bool boolean){
	agmv->flip = boolean;
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

void AGMV_SetImageBufferCoords(AGMV* agmv, u16 x, u16 y){
	agmv->rect.x = x;
	agmv->rect.y = y;
}

void AGMV_SetIntraFrameCoords(AGMV* agmv, u16 x, u16 y){
	agmv->irect.x = x;
	agmv->irect.y = y;
}

void AGMV_SetPrevFrameCoords(AGMV* agmv, u16 x, u16 y){
	agmv->prect.x = x;
	agmv->prect.y = y;
}

void AGMV_InitScreen(AGMV* agmv, u16 x, u16 y, u16 w, u16 h){
	agmv->screen.x = x;
	agmv->screen.y = y;
	agmv->screen.w = w;
	agmv->screen.h = h;
}

void AGMV_SetNextPrim(AGMV* agmv, char* nextprim){
	agmv->nextpri = nextprim;
}

void AGMV_WaitUntilButtonReleased(int button){
	int pad;
	
	do{
		pad = PadRead(1);
	}while(pad & button);
}

void AGMV_StreamMovie(AGMV* agmv){
	u32 offset;
	long p, flag;
	
	if(agmv->video_state == AGMV_VIDEO_PLAY){
		if(!AGMV_IsVideoDone(agmv)){	
			AGMV_FindNextFrameChunk(agmv->file);
			
			offset    = AGMV_Ftell(agmv->file);
			
			AGMV_StoreFrameOffset(agmv,offset);
			AGMV_DecodeFrameChunk(agmv->file,agmv);
			
			if(agmv->header.total_audio_duration != 0){
				AGMV_FindNextAudioChunk(agmv->file);
				
				if(agmv->audio_state == AGMV_AUDIO_PLAY){
					AGMV_DecodeAudioChunk(agmv->file,agmv);
					SpuSetTransferMode(SpuTransByDMA);
					SpuWrite((u_char*)agmv->audio_track->pcm, agmv->audio_chunk->size<<1);
					SpuIsTransferCompleted(SPU_TRANSFER_WAIT);
				}
				else{
					AGMV_SkipAudioChunk(agmv->file);
				}
			}
		}
	}
	
	RotMatrix(&agmv->rotVector, &agmv->Work);  
	TransMatrix(&agmv->Work, &agmv->transVector);      
	ScaleMatrix(&agmv->Work, &agmv->scale);
	SetRotMatrix(&agmv->Work);             
	SetTransMatrix(&agmv->Work);  
	
	agmv->poly = (POLY_FT4 *)agmv->nextpri;
	setPolyFT4(agmv->poly);    
	agmv->poly->tpage = getTPage(2,0,agmv->rect.x,agmv->rect.y); 
	setRGB0(agmv->poly, 128, 128, 128);   
	SetSemiTrans(agmv->poly, 0);
	RotTransPers4(
				&agmv->VertPos[0],      &agmv->VertPos[1],      &agmv->VertPos[2],      &agmv->VertPos[3],
				(long*)&agmv->poly->x0, (long*)&agmv->poly->x1, (long*)&agmv->poly->x2, (long*)&agmv->poly->x3,
				&p,
				&flag
				);                            
	setUV4(agmv->poly, 0, 0, 0, agmv->frame->height, agmv->frame->width, 0, agmv->frame->width, agmv->frame->height);
}

void AGMV_ResetVideo(File* file, AGMV* agmv){
	u8 version = agmv->header.version;
	if(version == 35 || version == 37){
		AGMV_SeekSet(file);
		AGMV_SeekForwards(file,1574);
	}
	else{
		AGMV_SeekSet(file);
	}
	agmv->prev_fill_color = 0;
	agmv->vq2color1 = 0;
	agmv->vq2color2 = 0;
	agmv->frame_count = 0;
	agmv->pframe_count = 0;
	agmv->iframe_count = 0;
}

void AGMV_SkipForwards(File* file, AGMV* agmv, int n){
	u32 i;
	u32 num_of_frames = agmv->header.num_of_frames;
	u32 offset;
	u16 buf_index;
	
	agmv->pframe_count = 0;
	
	if(agmv->frame_count + n >= num_of_frames){
		n = num_of_frames - agmv->frame_count;
	}
	
	if(agmv->header.total_audio_duration != 0){
		for(i = 0; i < n; i++){
			agmv->frame_chunk->frame_type = AGMV_PeekFrameChunk(file);
			
			offset    = AGMV_Ftell(agmv->file);
			
			AGMV_StoreFrameOffset(agmv,offset);
			AGMV_SkipFrameChunk(file);
			
			agmv->frame_count++;

			AGMV_FindNextAudioChunk(file);
			AGMV_SkipAudioChunk(file);
		}
		
		while(agmv->frame_chunk->frame_type != AGMV_IFRAME && agmv->frame_count < num_of_frames){
			agmv->frame_chunk->frame_type = AGMV_PeekFrameChunk(file);
			
			offset    = AGMV_Ftell(agmv->file);
			
			AGMV_StoreFrameOffset(agmv,offset);
			
			if(agmv->frame_chunk->frame_type == AGMV_IFRAME){
				AGMV_DecodeFrameChunk(file,agmv);
			}
			else{
				AGMV_SkipFrameChunk(file);
			}
			
			agmv->frame_count++;
			
			AGMV_FindNextAudioChunk(file);
			AGMV_SkipAudioChunk(file);
		}

		if(AGMV_EOF(file)){
			agmv->frame_count = 0;
		}
	}
	else{
		for(i = 0; i < n; i++){
			agmv->frame_chunk->frame_type = AGMV_PeekFrameChunk(file);
			
			offset    = AGMV_Ftell(agmv->file);
			
			AGMV_StoreFrameOffset(agmv,offset);
			AGMV_SkipFrameChunk(file);
			agmv->frame_count++;
		}
		
		while(agmv->frame_chunk->frame_type != AGMV_IFRAME && agmv->frame_count < num_of_frames){
			agmv->frame_chunk->frame_type = AGMV_PeekFrameChunk(file);
			
			offset    = AGMV_Ftell(agmv->file);
			
			AGMV_StoreFrameOffset(agmv,offset);
			
			if(agmv->frame_chunk->frame_type == AGMV_IFRAME){
				AGMV_DecodeFrameChunk(file,agmv);
			}
			else{
				AGMV_SkipFrameChunk(file);
			}
			
			agmv->frame_count++;
		}
		
		if(AGMV_EOF(file)){
			agmv->frame_count = 0;
		}
	}
}

void AGMV_SkipBackwards(File* file, AGMV* agmv, int n){
	s32 offset, current_offset;
	s32 iframe_count;
	
	iframe_count = agmv->iframe_count;	
	iframe_count -= n;
	
	if(iframe_count < 0){
		iframe_count = 0;
	}
	
	agmv->iframe_count = iframe_count;
	
	if(iframe_count == 0){
		AGMV_ResetVideo(file,agmv);
		return;
	}
	
	current_offset = AGMV_Ftell(file);
	offset = agmv->offset_table[agmv->iframe_count].offset;
	agmv->frame_count = agmv->offset_table[agmv->iframe_count].frame_count;
	
	AGMV_SeekBackwards(file,current_offset-offset);
}