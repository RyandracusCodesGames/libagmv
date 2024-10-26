/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_gba.c
*   Date: 6/29/2024
*   Version: 2.0
*   Updated: 10/24/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include "agmv_gba.h"

/* the global interrupt enable register */
volatile unsigned short* interrupt_enable = (unsigned short*) 0x4000208;

/* this register stores the individual interrupts we want */
volatile unsigned short* interrupt_selection = (unsigned short*) 0x4000200;

/* this registers stores which interrupts if any occured */
volatile unsigned short* interrupt_state = (unsigned short*) 0x4000202;

/* the address of the function to call when an interrupt occurs */
volatile unsigned int* interrupt_callback = (unsigned int*) 0x3007FFC;

/* this register needs a bit set to tell the hardware to send the vblank interrupt */
volatile unsigned short* display_interrupts = (unsigned short*) 0x4000004;

/* the interrupts are identified by number, we only care about this one */
#define INTERRUPT_VBLANK 0x1

/* allows turning on and off sound for the GBA altogether */
volatile unsigned short* master_sound = (volatile unsigned short*) 0x4000084;
#define SOUND_MASTER_ENABLE 0x80

/* has various bits for controlling the direct sound channels */
volatile unsigned short* sound_control = (volatile unsigned short*) 0x4000082;

/* bit patterns for the sound control register */
#define SOUND_A_RIGHT_CHANNEL 0x100
#define SOUND_A_LEFT_CHANNEL 0x200
#define SOUND_A_FIFO_RESET 0x800
#define SOUND_B_RIGHT_CHANNEL 0x1000
#define SOUND_B_LEFT_CHANNEL 0x2000
#define SOUND_B_FIFO_RESET 0x8000

/* the location of where sound samples are placed for each channel */
volatile unsigned char* fifo_buffer_a  = (volatile unsigned char*) 0x40000A0;
volatile unsigned char* fifo_buffer_b  = (volatile unsigned char*) 0x40000A4;

/* global variables to keep track of how much longer the sounds are to play */
unsigned int channel_a_vblanks_remaining = 0;
unsigned int channel_a_total_vblanks = 0;
unsigned int channel_b_vblanks_remaining = 0;

void Open(File* file, const u8* data, u32 len){
	file->pos = 0;
	file->len = len;
	file->data = data;
}

void read(File* file, u8* buf, u32 count){
	if(file->pos + count <= file->len){
		int i;
		for(i = 0; i < count; i++){
			buf[i] = AGMV_ReadByte(file);
		}
	}
}

u16 AGMV_SkipFrameChunk(File* file){
	u32 fourcc, frame_num, frame_type, usize, csize;

	AGMV_FindNextFrameChunk(file);

	fourcc     = AGMV_ReadLong(file);
	frame_num  = AGMV_ReadLong(file);
	frame_type = AGMV_ReadShort(file);
	usize      = AGMV_ReadLong(file);
	csize      = AGMV_ReadLong(file);

	seek(file,csize,SEEK_CUR);
	
	return frame_type;
}

void AGMV_SkipAudioChunk(File* file){
	u32 fourcc, size, csize;

	AGMV_FindNextAudioChunk(file);

	fourcc = AGMV_ReadLong(file);
	size   = AGMV_ReadLong(file);
	csize  = AGMV_ReadLong(file);

	seek(file,csize,SEEK_CUR);
}

void AGMV_DisableAllAudio(AGMV* agmv){
	agmv->disable_all_audio = TRUE;
}

void AGMV_EnableAllAudio(AGMV* agmv){
	agmv->disable_all_audio = FALSE;
}

int AGMV_DecodeHeader(File* file, AGMV* agmv){
	
	int i, version;
	
	AGMV_ReadFourCC(file,agmv->header.fourcc);
	agmv->header.num_of_frames = AGMV_ReadLong(file);
	agmv->header.width = AGMV_ReadLong(file);
	agmv->header.height = AGMV_ReadLong(file);
	
	agmv->header.fmt = AGMV_ReadByte(file);
	agmv->header.version = AGMV_ReadByte(file);
	agmv->header.frames_per_second = AGMV_ReadLong(file);
	
	agmv->header.total_audio_duration = AGMV_ReadLong(file);
	agmv->header.sample_rate = AGMV_ReadLong(file);
	agmv->header.audio_size = AGMV_ReadLong(file);
	agmv->header.num_of_channels = AGMV_ReadShort(file);
	agmv->header.bits_per_sample = AGMV_ReadShort(file);
	
	version = agmv->header.version;
	
	if(!AGMV_IsCorrectFourCC(agmv->header.fourcc,'A','G','M','V') || !(version == 5 || version == 6 || version == 7 || version == 8 || 
	version == 10 || version == 101) || agmv->header.frames_per_second >= 200 || !(agmv->header.bits_per_sample == 8
	|| agmv->header.bits_per_sample == 0 || agmv->header.bits_per_sample == 16)){
		return INVALID_HEADER_FORMATTING_ERR;
	}
	
	if(version == 10 || version == 101){
		return NO_ERR;
	}
	
	if(version == 1 || version == 3 || version == 5 || version == 7){
		for(i = 0; i < 256; i++){
			u8 r = AGMV_ReadByte(file);
			u8 g = AGMV_ReadByte(file);
			u8 b = AGMV_ReadByte(file);
			
			r >>= 3;
			g >>= 3;
			b >>= 3;
			
			agmv->header.palette0[i] = b << 10 | g << 5 | r;
		}
		
		for(i = 0; i < 256; i++){
			u8 r = AGMV_ReadByte(file);
			u8 g = AGMV_ReadByte(file);
			u8 b = AGMV_ReadByte(file);
			
			r >>= 3;
			g >>= 3;
			b >>= 3;
			
			agmv->header.palette1[i] = b << 10 | g << 5 | r;
		}
	}
	else{
		for(i = 0; i < 256; i++){
			u8 r = AGMV_ReadByte(file);
			u8 g = AGMV_ReadByte(file);
			u8 b = AGMV_ReadByte(file);
			
			r >>= 3;
			g >>= 3;
			b >>= 3;
			
			agmv->header.palette0[i] = b << 10 | g << 5 | r;
		}
	}
	
	return NO_ERR;
}

void AGMV_AllocResources(AGMV* agmv){
	agmv->error = NO_ERR;
	
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

	agmv->frame->img_data  = (u16*)malloc(sizeof(u16)*agmv->frame->width*agmv->frame->height);
	agmv->iframe->img_data = (u16*)malloc(sizeof(u16)*agmv->frame->width*agmv->frame->height);
	agmv->prev_frame->img_data = (u16*)malloc(sizeof(u16)*agmv->frame->width*agmv->frame->height);
	
	agmv->bitstream = (AGMV_BITSTREAM*)malloc(sizeof(AGMV_BITSTREAM));
	agmv->bitstream->data = (u8*)malloc((agmv->frame->width*agmv->frame->height*2));
	agmv->bitstream->pos = 0;
	
	agmv->offset_table = (AGMV_OFFSET*)malloc(sizeof(AGMV_OFFSET)*(agmv->header.num_of_frames/3));
	
	agmv->audio_chunk->sample = NULL;
	
	if(agmv->frame->img_data == NULL || agmv->iframe->img_data == NULL || agmv->prev_frame->img_data == NULL || agmv->bitstream->data == NULL){
		agmv->error = MEMORY_CORRUPTION_ERR;
	}
	
	agmv->prev_fill_color = 0;
	agmv->vq2color1 = 0;
	agmv->vq2color2 = 0;
	agmv->frame_count = 0;
	agmv->pframe_count = 0;
	agmv->frame_count = 0;
	agmv->iframe_count = 0;
	
	if(agmv->header.total_audio_duration != 0){
		AGMV_SetVolume(agmv,1);

		agmv->audio_chunk->enable_audio = TRUE;
		u32 sample_size = (agmv->header.audio_size/(float)agmv->header.num_of_frames);
		agmv->audio_chunk->size = sample_size;
		agmv->audio_chunk->sample = (s8*)malloc(sizeof(s8)*sample_size*2);
		
		AGMV_EnableAllAudio(agmv);
	}
}

AGMV* AGMV_Open(const u8* data, u32 len){
	AGMV* agmv = (AGMV*)malloc(sizeof(AGMV));
	agmv->file = (File*)malloc(sizeof(File));
	
	Open(agmv->file,data,len);
	
	agmv->error = AGMV_DecodeHeader(agmv->file,agmv);
	
	if(agmv->error != NO_ERR){
		free(agmv->file);
		free(agmv);
		return NULL;
	}
	
	AGMV_AllocResources(agmv);
	AGMV_SetVolume(agmv,1);
	AGMV_SetAudioState(agmv,AGMV_AUDIO_PLAY);
	AGMV_SetVideoState(agmv,AGMV_VIDEO_PLAY);
	
	agmv->info = AGMV_GetVideoInfo(agmv);
	
	return agmv;
}

void AGMV_Close(AGMV* agmv){
	free(agmv->file);
	DestroyAGMV(agmv);
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

void AGMV_InitSound(AGMV* agmv){
	*interrupt_enable = 0;
    *interrupt_selection |= INTERRUPT_VBLANK;
    *display_interrupts |= 0x08;
    *interrupt_enable = 1;

    /* clear the sound control initially */
    *sound_control = 0;
	
	AGMV_SetVolume(agmv,1);
}

void AGMV_EnableMasterSound(){
	*(volatile unsigned short*) 0x4000084 = 0x80;
}

void AGMV_DisableMasterSound(){
	*(volatile unsigned short*) 0x4000084 = 0;
}

void AGMV_SetVolume(AGMV* agmv, int volume){
	u16* agmv_master_sound = AGMV_MASTER_SOUND;
	
	if(volume > 1){
		volume = 1;
	}
	
	if(volume < 0){
		volume = 0;
	}
	
	agmv->volume = volume;
	
	if(volume == 1){
		*agmv_master_sound |= 4;
	}
	else{
		*agmv_master_sound = 0;
	}
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

void AGMV_DisableAudio(AGMV* agmv){
	if(agmv->audio_state != AGMV_AUDIO_DISABLE){
		agmv->audio_state = AGMV_AUDIO_DISABLE;
	}
}

void AGMV_MuteAudio(AGMV* agmv){
	if(agmv->audio_state != AGMV_AUDIO_PAUSE){
		agmv->audio_state = AGMV_AUDIO_PAUSE;
	}
}

void DestroyAGMV(AGMV* agmv){
	if(agmv != NULL){
		
		if(agmv->offset_table != NULL){
			free(agmv->offset_table);
			agmv->offset_table = NULL;
		}
		
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

			if(agmv->audio_chunk->sample != NULL){
				free(agmv->audio_chunk->sample);
				agmv->audio_chunk->sample = NULL;
			}
		}
	
		agmv->bitstream = NULL;
		agmv->frame_chunk = NULL;

		if(agmv->audio_chunk != NULL){
			free(agmv->audio_chunk);
			agmv->audio_chunk = NULL;
		}
		
		if(agmv->audio_track != NULL){
			free(agmv->audio_track);
			agmv->audio_track = NULL;
		}
		
		agmv->audio_chunk = NULL;
		
		if(agmv->file != NULL){
			free(agmv->file);
			agmv->file = NULL;
		}
		
		free(agmv);

		agmv = NULL;
	}
}

void AGMV_IWRAM AGMV_DecompressLZSS(File* file, u8* bitstream_data, u32 usize){
	u32 bpos = 0;
	u8 byte = 0, flag_num = 0;
	u16 word, offset;
	u8 len, *src, *dest;
	int window_start;

	while(bpos < usize){
		if(flag_num == 0){
			byte = AGMV_ReadByte(file);
			flag_num = 8;
		}

		if(byte & 0x80){
			bitstream_data[bpos++] = AGMV_ReadByte(file);
		}
		else{
			word = AGMV_ReadShort(file);
			
			offset = (word >> 4) & 0xFFF;
			len = (word & 0xF);

			window_start = bpos - offset;
			
			dest = &bitstream_data[bpos];
			src = &bitstream_data[window_start];
			
			AGMV_Memcpy(dest,src,len);

			bpos += len;
		}

		byte <<= 1;
		flag_num--;
	}
}

int AGMV_IWRAM AGMV_DecodeFrameChunk(File* file, AGMV* agmv){
	u32 i, bits, size = agmv->header.width * agmv->header.height, usize, width, height, yoffset;
	u32 block_size, block_w, block_h, pframe_count;
	u16 offset, *palette0, *palette1, color;
	u16* img_data, *iframe_data, *pframe_data, *img_ptr, frame_type, color_array[8];
	u16 vq2color1, vq2color2, prev_color;
	s8 n;
	u8 byte, index, fbit, *bitstream_data, version;
	Bool v1, v4;
	int x, y;
	
	v1 = FALSE;
	v4 = FALSE;
	agmv->bitstream->pos = 0;
	
	seek(file,8,SEEK_CUR);
	
	frame_type = AGMV_ReadShort(file);
	agmv->frame_chunk->frame_type = frame_type;
	agmv->frame_chunk->uncompressed_size = AGMV_ReadLong(file);
	agmv->frame_chunk->compressed_size = AGMV_ReadLong(file);
	
	usize = agmv->frame_chunk->uncompressed_size;
	
	width  = agmv->frame->width;
	height = agmv->frame->height;
	version = agmv->header.version;
		
	bitstream_data = agmv->bitstream->data;
	
	img_data = agmv->frame->img_data;
	iframe_data = agmv->iframe->img_data;
	pframe_data = agmv->prev_frame->img_data;
		
	palette0 = agmv->header.palette0;
	palette1 = agmv->header.palette1;
	
	vq2color1 = agmv->vq2color1;
	vq2color2 = agmv->vq2color2;
	prev_color = agmv->prev_fill_color;
	
	if(version == 7 || version == 8 || version == 101){
		if((u32)(file->data + file->pos) & 1){
			bitstream_data++;
		}			
		AGMV_ReadNBytes(file,bitstream_data,usize);
	}
	else{
		AGMV_DecompressLZSS(file,bitstream_data,usize);
	}
	
	if(version == 5 || version == 7){
		v1 = TRUE;
	}
	
	if(version == 10 || version == 101){
		v4 = TRUE;
	}
	
	if(agmv->frame_chunk->frame_type == AGMV_IFRAME){
		agmv->pframe_count = 0;
	}
	else{
		agmv->pframe_count++;
	}
	
	pframe_count = agmv->pframe_count;
	
	for(y = 0; y < height; y += 4){
		yoffset = y * width;
		for(x = 0; x < width; x += 4){
			
			bits       = *(bitstream_data++);
			block_size = (bits >> 6) & 3;
			byte       = (bits & 0x3f);
			block_w    = AGMV_GetBlockWidth(block_size);
			block_h    = AGMV_GetBlockHeight(block_size);

			offset = x + yoffset;
			
			img_ptr = &img_data[offset];
			
			switch(byte){
				case AGMV_NORMAL_FLAG:{
					if(v1){
						for(i = 0; i < 5; i++){
							index = *(bitstream_data++);
							fbit = (index >> 7);
							bits = (index & 0x7f);
							
							if(bits >= 127) bits = *(bitstream_data++);
							color_array[i] = fbit ? palette1[bits] : palette0[bits];

							if(i <= 1){
								img_ptr[i] = color_array[i];
							}
						}
					}
					else if(v4){
						for(i = 0; i < 5; i++){
							bits = *(bitstream_data++);
							byte = *(bitstream_data++);
							
							color_array[i] = bits << 8 | byte;
							
							if(i <= 1){
								img_ptr[i] = color_array[i];
							}
						}
					}
					else{
						for(i = 0; i < 5; i++){
							color_array[i] = palette0[*(bitstream_data++)];
							
							if(i <= 1){
								img_ptr[i] = color_array[i];
							}
						}
					}
					
					img_ptr += width;

					img_ptr[0] = AGMV_InterpColor(color_array[0],color_array[2]);
					img_ptr[1] = AGMV_InterpColor(color_array[1],color_array[3]);
					
					img_ptr += width;
					
					img_ptr[0] = color_array[2];
					img_ptr[1] = color_array[3];
					
					img_ptr += width;
					
					img_ptr[0] = img_ptr[1] = color_array[4];
				
					x -= 2;
				}break;
				case AGMV_PCOPY_FLAG:{
					
					AGMV_BlockCopy(img_data,pframe_data,offset,offset,block_w,block_h,width);
					
					x += (block_size == 1 || block_size == 3) ? 4 : (block_size == 2) ? -2 : 0;
				}break;
				case AGMV_COPY_FLAG:{

					if(pframe_count > 1)
						AGMV_BlockCopy(img_data,iframe_data,offset,offset,block_w,block_h,width);

					x += (block_size == 1 || block_size == 3) ? 4 : (block_size == 2) ? -2 : 0;
				}break;
				case AGMV_PMV_FLAG:
				case AGMV_MV_FLAG:{
					u32 offset2;
					s8 mx, my;
					int mvx, mvy;
					
					bits = *(bitstream_data++);
					
					mx = ((bits >> 4) & 0x0f) - 8;
					my = (bits & 0x0f) - 8;
					
					mvx = mx + x;
					mvy = my + y;
					
					if(mvx + block_w >= width || mvy + block_h >= height){
						break;
					}

					offset2 = mvx + mvy * width;
					
					if(byte == AGMV_MV_FLAG){
						AGMV_BlockCopy(img_data,iframe_data,offset,offset2,block_w,block_h,width);
					}
					else{
						AGMV_BlockCopy(img_data,pframe_data,offset,offset2,block_w,block_h,width);
					}
					
					x += (block_size == 1 || block_size == 3) ? 4 : (block_size == 2) ? -2 : 0;
				}break;
				case AGMV_FILL_FLAG:{
					if(v4){
						bits = *(bitstream_data++);
						byte = *(bitstream_data++);
						
						color = bits << 8 | byte;
					}
					else if(v1){
						index = *(bitstream_data++); 
						fbit = (index >> 7);
						bits = (index & 0x7f);
				
						if(bits >= 127) bits = *(bitstream_data++);
						color = fbit ? palette1[bits] : palette0[bits];
					}
					else{
						color = palette0[*(bitstream_data++)];
					}
					
					AGMV_Memset16(color_array,color,block_w);

					AGMV_BlockFill(img_ptr,color_array,block_w,block_h,width);
					
					prev_color = color;
					
					x += (block_size == 1 || block_size == 3) ? 4 : (block_size == 2) ? -2 : 0;
				}break;
				case AGMV_PFILL_FLAG:{

					AGMV_Memset16(color_array,prev_color,block_w);

					AGMV_BlockFill(img_ptr,color_array,block_w,block_h,width);
					
					x += (block_size == 1 || block_size == 3) ? 4 : (block_size == 2) ? -2 : 0;
				}break;
				case AGMV_SKIP_8:{
					x += 4;
				}break;
				case AGMV_ICOPY_FLAG:
				case AGMV_ICOPYR_FLAG:{
					u32 ix, iy, ioffset, sx, sy;
					
					bits = *(bitstream_data++);
					
					sx = (bits >> 4) & 15;
					sy = (bits & 15);
					
					if(byte == AGMV_ICOPY_FLAG){
						ix = x - sx; iy = y - sy;
					}
					else{
						ix = x + sx; iy = y - sy;
					}
					
					ioffset = ix + iy * width;
					
					AGMV_BlockCopy(img_data,img_data,offset,ioffset,block_w,block_h,width);
					
					x += (block_size == 1 || block_size == 3) ? 4 : (block_size == 2) ? -2 : 0;
				}break;
				case AGMV_SMV_FLAG:{
					u32 offset_arr[4], xoffset_arr[4], yoffset_arr[4], offset2;
					s8 mx, my;
					int mvx, mvy;
					
					if(block_size == AGMV_BLOCK_8x4){
						
						offset = x + yoffset;
						
						offset_arr[0] = offset;
						offset_arr[1] = offset + 4;
						
						xoffset_arr[0] = x; xoffset_arr[1] = x + 4;
						
						for(i = 0; i < 2; i++){
							
							byte = *(bitstream_data++);
							
							mx = (s8)(byte);
							
							byte = *(bitstream_data++);
							
							my = (s8)(byte);
							
							mvx = mx + xoffset_arr[i];
							mvy = my + y;
							
							if(mvx + 4 >= width || mvy + 4 >= height){
								break;
							}
							
							offset  = offset_arr[i];
							offset2 = mvx + mvy * width;
							
							if(pframe_count > 1){
								AGMV_BlockCopy(img_data,pframe_data,offset,offset2,4,4,width);
							}
							else{
								AGMV_BlockCopy(img_data,iframe_data,offset,offset2,4,4,width);
							}
						}
						
						x += 4;
					}
					else if(block_size == AGMV_BLOCK_4x4){
						
						offset_arr[0] = offset;
						offset_arr[1] = offset + 2;
						offset_arr[2] = offset + width + width;
						offset_arr[3] = offset_arr[2] + 2;
						
						xoffset_arr[0] = x = xoffset_arr[2] = x;
						xoffset_arr[1] = xoffset_arr[3] = x + 2;
						
						yoffset_arr[0] = yoffset_arr[1] = y;
						yoffset_arr[2] = yoffset_arr[3] = y + 2;
						
						for(i = 0; i < 4; i++){
							byte = *(bitstream_data++);
							
							mx = ((byte >> 4) & 0x0f) - 8;
							my = (byte & 0x0f) - 8;
							
							mvx = mx + xoffset_arr[i];
							mvy = my + yoffset_arr[i];
							
							if(mvx + 2 >= width || mvy + 2 >= height){
								break;
							}
							
							offset  = offset_arr[i];
							offset2 = mvx + mvy * width;
							
							if(pframe_count > 1){
								AGMV_BlockCopy(img_data,pframe_data,offset,offset2,2,2,width);
							}
							else{
								AGMV_BlockCopy(img_data,iframe_data,offset,offset2,2,2,width);
							}
						}
					}
					else{
						
						offset_arr[0] =  offset;
						offset_arr[1] =  offset + width + width;
						
						yoffset_arr[0] = y; yoffset_arr[1] = y + 2;
						
						for(i = 0; i < 2; i++){
							byte = *(bitstream_data++);
							
							mx = ((byte >> 4) & 0x0f) - 8;
							my = (byte & 0x0f) - 8;
							
							mvx = mx + x;
							mvy = my + yoffset_arr[i];

							if(mvx + 2 >= width || mvy + 2 >= height){
								break;
							}

							offset  = offset_arr[i];
							offset2 = mvx + mvy * width;
							
							if(pframe_count > 1){
								AGMV_BlockCopy(img_data,pframe_data,offset,offset2,2,2,width);
							}
							else{
								AGMV_BlockCopy(img_data,iframe_data,offset,offset2,2,2,width);
							}
						}
						
						x -= 2;
					}
				}break;
				case AGMV_VQ2_FLAG:
				case AGMV_PVQ2_FLAG:{
				
					if(byte == AGMV_VQ2_FLAG){
						if(v4){
							for(i = 0; i < 2; i++){
								bits = *(bitstream_data++);
								byte = *(bitstream_data++);
								
								color_array[i] =  bits << 8 | byte;
							}
						}
						else if(v1){
							for(i = 0; i < 2; i++){
								index = *(bitstream_data++);
								fbit = (index >> 7);
								bits = (index & 0x7f);
						
								if(bits >= 127) bits = *(bitstream_data++);
								color_array[i] = fbit ? palette1[bits] : palette0[bits];
							}
						}
						else{
							color_array[0] = palette0[*(bitstream_data++)];
							color_array[1] = palette0[*(bitstream_data++)];
						}
							
						vq2color1 = color_array[0];
						vq2color2 = color_array[1];
					}
					else{
						color_array[0] = vq2color1;
						color_array[1] = vq2color2;
					}
					
					if(block_size == AGMV_BLOCK_4x4){
						byte = *(bitstream_data++);
						
						*(img_ptr++) = color_array[byte >> 7];
						*(img_ptr++) = color_array[(byte >> 6) & 1];
						*(img_ptr++) = color_array[(byte >> 5) & 1];
						*(img_ptr++) = color_array[(byte >> 4) & 1];

						img_ptr += width - 4;

						*(img_ptr++) = color_array[(byte >> 3) & 1];
						*(img_ptr++) = color_array[(byte >> 2) & 1];
						*(img_ptr++) = color_array[(byte >> 1) & 1];
						*(img_ptr++) = color_array[byte & 1];

						img_ptr += width - 4;
						
						byte = *(bitstream_data++);
						
						*(img_ptr++) = color_array[byte >> 7];
						*(img_ptr++) = color_array[(byte >> 6) & 1];
						*(img_ptr++) = color_array[(byte >> 5) & 1];
						*(img_ptr++) = color_array[(byte >> 4) & 1];

						img_ptr += width - 4;

						*(img_ptr++) = color_array[(byte >> 3) & 1];
						*(img_ptr++) = color_array[(byte >> 2) & 1];
						*(img_ptr++) = color_array[(byte >> 1) & 1];
						*(img_ptr++) = color_array[byte & 1];
					}
					else if(block_size == AGMV_BLOCK_8x4 || block_size == AGMV_BLOCK_8x8){
						for(i = 0; i < block_h; i++){
							byte = *(bitstream_data++);
							
							for(n = 7; n >= 0; n--)
								*(img_ptr++) = color_array[(byte >> n) & 1];

							img_ptr += width - 8;
						}
						
						x += 4;
					}
					else{
						byte = *(bitstream_data++);
						
						*(img_ptr++) = color_array[byte >> 7];
						*(img_ptr++) = color_array[(byte >> 6) & 1];
						
						img_ptr += width - 2;
						
						*(img_ptr++) = color_array[(byte >> 5) & 1];
						*(img_ptr++) = color_array[(byte >> 4) & 1];

						img_ptr += width - 2;

						*(img_ptr++) = color_array[(byte >> 3) & 1];
						*(img_ptr++) = color_array[(byte >> 2) & 1];
						
						img_ptr += width - 2;
						
						*(img_ptr++) = color_array[(byte >> 1) & 1];
						*(img_ptr++) = color_array[byte & 1];

						x -= 2;
					}
				}break;
				case AGMV_VQ4_FLAG:{
					if(v4){
						for(i = 0; i < 4; i++){
							bits = *(bitstream_data++);
							byte = *(bitstream_data++);
							
							color_array[i] =  bits << 8 | byte;
						}
					}
					else if(v1){
						for(i = 0; i < 4; i++){
							index = *(bitstream_data++);
							fbit = (index >> 7);
							bits = (index & 0x7f);
							
							if(bits >= 127) bits = *(bitstream_data++);
							color_array[i] = fbit ? palette1[bits] : palette0[bits];
						}
					}
					else{
						for(i = 0; i < 4; i++){
							color_array[i] = palette0[*(bitstream_data++)];
						}
					}
					
					if(block_size == AGMV_BLOCK_4x4){
						for(i = 0; i < 4; i++){
							byte = *(bitstream_data++);
							*(img_ptr++) = color_array[(byte >> 6) & 3];
							*(img_ptr++) = color_array[(byte >> 4) & 3];
							*(img_ptr++) = color_array[(byte >> 2) & 3];
							*(img_ptr++) = color_array[byte & 3];
							img_ptr += width - 4;
						}
					}
					else{
						for(i = 0; i < block_h; i++){
							byte = *(bitstream_data++);
							
							*(img_ptr++) = color_array[(byte >> 6) & 3];
							*(img_ptr++) = color_array[(byte >> 4) & 3];
							*(img_ptr++) = color_array[(byte >> 2) & 3];
							*(img_ptr++) = color_array[byte & 3];
							
							byte = *(bitstream_data++);
							
							*(img_ptr++) = color_array[(byte >> 6) & 3];
							*(img_ptr++) = color_array[(byte >> 4) & 3];
							*(img_ptr++) = color_array[(byte >> 2) & 3];
							*(img_ptr++) = color_array[byte & 3];
							
							img_ptr += width - 8;
						}
						
						x += 4;
					}
				}break;
			}
		}
	}
	
	if(frame_type == AGMV_IFRAME){
		AGMV_DMACopy16(img_data,iframe_data,size);
	}
	else{
		AGMV_DMACopy16(img_data,pframe_data,size);
	}
	
	agmv->vq2color1 = vq2color1;
	agmv->vq2color2 = vq2color2;
	agmv->prev_fill_color = prev_color;
	agmv->frame_count++;
	
	return NO_ERR;
}

int AGMV_IWRAM AGMV_DecodeAudioChunk(File* file, AGMV* agmv){
	u32 i, size, csize;
	s8* pcm, diff;
	u8 *bitstream_data = agmv->bitstream->data, previous_sample = 0, current_sample;
	
	pcm = agmv->audio_chunk->sample;
	
	AGMV_ReadFourCC(file,agmv->audio_chunk->fourcc);
	size  = AGMV_ReadLong(file);
	csize = AGMV_ReadLong(file);
	agmv->audio_chunk->size = size;

	if(!AGMV_IsCorrectFourCC(agmv->audio_chunk->fourcc,'A','G','A','C')){
		return INVALID_HEADER_FORMATTING_ERR;
	}
	
	AGMV_DecompressLZSS(file,bitstream_data,size);

	for(i = 0; i < size; i++){
		diff = (s8)bitstream_data[i];
		current_sample = previous_sample + diff;
		pcm[i] = (current_sample<<1) - 128;
		previous_sample = current_sample;
	}
	
	return NO_ERR;
}

void SetVideoMode(u8 mode){
	*(u16*)0x4000000 = 0x400 | mode; 
}

void EnableTimer2(){	
	*(u16*)0x400010A = 0x82;
	*(u16*)0x400010E = 0x84;
}

void AGMV_ResetVideo(File* file, AGMV* agmv){
	u8 version = agmv->header.version;
	if(version == 5 || version == 7){
		seek(file,1574,SEEK_SET);
	}
	else{
		seek(file,0,SEEK_SET);
	}
	agmv->prev_fill_color = 0;
	agmv->vq2color1 = 0;
	agmv->vq2color2 = 0;
	agmv->frame_count = 0;
	agmv->pframe_count = 0;
	agmv->frame_count = 0;
	agmv->iframe_count = 0;
}

void AGMV_SkipForwards(File* file, AGMV* agmv, int n){
	u32 i;
	u32 offset, seek_to, iframe_count, frame_count = agmv->frame_count;
	u32 num_of_frames = agmv->header.num_of_frames;
	
	if(frame_count + n >= num_of_frames){
		n = num_of_frames - frame_count;
	}
	
	if(agmv->header.total_audio_duration != 0){
		for(i = 0; i < n; i++){
			agmv->frame_chunk->frame_type = AGMV_PeekFrameChunk(file);
			
			offset    = tell(agmv->file);
			
			if(agmv->frame_chunk->frame_type == AGMV_IFRAME){
				frame_count = agmv->frame_count;
				iframe_count = agmv->iframe_count;
				seek_to = tell(agmv->file);
			}
			
			AGMV_StoreFrameOffset(agmv,offset);
			AGMV_SkipFrameChunk(file);
			
			agmv->frame_count++;

			AGMV_FindNextAudioChunk(file);
			AGMV_SkipAudioChunk(file);
		}
		
		while(agmv->frame_chunk->frame_type != AGMV_IFRAME && agmv->frame_count < num_of_frames){
			agmv->frame_chunk->frame_type = AGMV_PeekFrameChunk(file);
			
			offset    = tell(agmv->file);
			
			AGMV_StoreFrameOffset(agmv,offset);
			
			if(agmv->frame_chunk->frame_type == AGMV_IFRAME){
				seek_to = tell(agmv->file);
				frame_count = agmv->frame_count;
				iframe_count = agmv->iframe_count;
				AGMV_SkipFrameChunk(file);
			}
			else{
				AGMV_SkipFrameChunk(file);
			}
			
			agmv->frame_count++;
			
			AGMV_FindNextAudioChunk(file);
			AGMV_SkipAudioChunk(file);
		}
		
		seek(file,seek_to,SEEK_SET);
		agmv->frame_count = frame_count;
		agmv->iframe_count = iframe_count;

		if(AGMV_EOF(file)){
			agmv->frame_count = 0;
		}
	}
	else{
		for(i = 0; i < n; i++){
			agmv->frame_chunk->frame_type = AGMV_PeekFrameChunk(file);
			
			offset    = tell(agmv->file);
			
			if(agmv->frame_chunk->frame_type == AGMV_IFRAME){
				seek_to = tell(agmv->file);
				frame_count = agmv->frame_count;
				iframe_count = agmv->iframe_count;
			}
			
			AGMV_StoreFrameOffset(agmv,offset);
			AGMV_SkipFrameChunk(file);
			agmv->frame_count++;
		}
		
		while(agmv->frame_chunk->frame_type != AGMV_IFRAME && agmv->frame_count < num_of_frames){
			agmv->frame_chunk->frame_type = AGMV_PeekFrameChunk(file);
			
			offset    = tell(agmv->file);
			
			AGMV_StoreFrameOffset(agmv,offset);
			
			if(agmv->frame_chunk->frame_type == AGMV_IFRAME){
				seek_to = tell(agmv->file);
				frame_count = agmv->frame_count;
				iframe_count = agmv->iframe_count;
				AGMV_SkipFrameChunk(file);
			}
			else{
				AGMV_SkipFrameChunk(file);
			}
			
			agmv->frame_count++;
		}
		
		seek(file,seek_to,SEEK_SET);
		agmv->frame_count = frame_count;
		agmv->iframe_count = iframe_count;
		
		if(AGMV_EOF(file)){
			agmv->frame_count = 0;
		}
	}
}

void AGMV_SkipBackwards(File* file, AGMV* agmv, int n){
	int offset;
	int iframe_count;
	
	iframe_count = agmv->iframe_count;	
	iframe_count -= n;
	
	if(iframe_count < 0){
		iframe_count = 0;
	}
	
	agmv->iframe_count = iframe_count;

	offset = agmv->offset_table[agmv->iframe_count].offset;
	agmv->frame_count = agmv->offset_table[agmv->iframe_count].frame;
	
	seek(file,offset,SEEK_SET);
}

/* ONLY CALL SKIP TO FUNCTION AFTER ALL FRAMES HAVE BEEN READ */

void AGMV_SkipTo(File* file, AGMV* agmv, int n){
	int offset;
	int iframe_count;
	
	iframe_count = agmv->iframe_count;	
	
	if(n < 0){
		n = 0;
	}
	
	if(n >= iframe_count){
		n = iframe_count;
	}
	
	iframe_count = n;
	
	agmv->iframe_count = iframe_count;
	
	offset = agmv->offset_table[iframe_count].offset;
	agmv->frame_count = agmv->offset_table[iframe_count].frame;
	
	seek(file,offset,SEEK_SET);
}


volatile unsigned short* buttons = (volatile unsigned short*) 0x04000130;

/* define the timer control registers */
volatile unsigned short* timer0_data = (volatile unsigned short*) 0x4000100;
volatile unsigned short* timer0_control = (volatile unsigned short*) 0x4000102;

/* make defines for the bit positions of the control register */
#define TIMER_FREQ_1 0x0
#define TIMER_FREQ_64 0x2
#define TIMER_FREQ_256 0x3
#define TIMER_FREQ_1024 0x4
#define TIMER_ENABLE 0x80
#define TIMER_IRQ_ENABLE	0x40

/* the GBA clock speed is fixed at this rate */
#define CLOCK 16777216 
#define CYCLES_PER_BLANK 280806

/* this causes the DMA destination to be the same each time rather than increment */
#define DMA_DEST_FIXED 0x400000

/* this causes the DMA repeat interval to be synced with timer 0 */
#define DMA_SYNC_TO_TIMER 0x30000000

/* pointers to the DMA source/dest locations and control registers */
volatile unsigned int* dma1_source = (volatile unsigned int*) 0x40000BC;
volatile unsigned int* dma1_destination = (volatile unsigned int*) 0x40000C0;
volatile unsigned int* dma1_control = (volatile unsigned int*) 0x40000C4;

volatile unsigned int* dma2_source = (volatile unsigned int*) 0x40000C8;
volatile unsigned int* dma2_destination = (volatile unsigned int*) 0x40000CC;
volatile unsigned int* dma2_control = (volatile unsigned int*) 0x40000D0;

volatile unsigned int* dma3_source = (volatile unsigned int*) 0x40000D4;
volatile unsigned int* dma3_destination = (volatile unsigned int*) 0x40000D8;
volatile unsigned int* dma3_control = (volatile unsigned int*) 0x40000DC;

/* copy data using DMA channel 3 (normal memory transfers) */
void memcpy16_dma(unsigned short* dest, unsigned short* source, int amount) {
    *dma3_source = (unsigned int) source;
    *dma3_destination = (unsigned int) dest;
    *dma3_control = AGMV_DMA_ENABLE | AGMV_DMA_16 | amount;
}

/* this function checks whether a particular button has been pressed */
unsigned char button_pressed(unsigned short button) {
    /* and the button register with the button constant we want */
    unsigned short pressed = *buttons & button;

    /* if this value is zero, then it's not pressed */
    if (pressed == 0) {
        return 1;
    } else {
        return 0;
    }
}

void AGMV_IWRAM play_sound(const signed char* sound, int total_samples, int sample_rate, char channel){
    /* start by disabling the timer and dma controller (to reset a previous sound) */
    *timer0_control = 0;
    if (channel == 'A') {
        *dma1_control = 0;
    } else if (channel == 'B') {
        *dma2_control = 0; 
    }

    /* output to both sides and reset the FIFO */
    if (channel == 'A') {
        *sound_control |= SOUND_A_RIGHT_CHANNEL | SOUND_A_LEFT_CHANNEL | SOUND_A_FIFO_RESET;
    } else if (channel == 'B') {
        *sound_control |= SOUND_B_RIGHT_CHANNEL | SOUND_B_LEFT_CHANNEL | SOUND_B_FIFO_RESET;
    }

    /* enable all sound */
    *master_sound = SOUND_MASTER_ENABLE;

    /* set the dma channel to transfer from the sound array to the sound buffer */
    if (channel == 'A') {
        *dma1_source = (unsigned int) sound;
        *dma1_destination = (unsigned int) fifo_buffer_a;
        *dma1_control = DMA_DEST_FIXED | AGMV_DMA_REPEAT | AGMV_DMA_32 | DMA_SYNC_TO_TIMER | AGMV_DMA_ENABLE;
    } else if (channel == 'B') {
        *dma2_source = (unsigned int) sound;
        *dma2_destination = (unsigned int) fifo_buffer_b;
        *dma2_control = DMA_DEST_FIXED | AGMV_DMA_REPEAT | AGMV_DMA_32 | DMA_SYNC_TO_TIMER | AGMV_DMA_ENABLE;
    }

    /* set the timer so that it increments once each time a sample is due
     * we divide the clock (ticks/second) by the sample rate (samples/second)
     * to get the number of ticks/samples */
    unsigned short ticks_per_sample = CLOCK / sample_rate;

    /* the timers all count up to 65536 and overflow at that point, so we count up to that
     * now the timer will trigger each time we need a sample, and cause DMA to give it one! */
    *timer0_data = 65536 - ticks_per_sample;

    /* determine length of playback in vblanks
     * this is the total number of samples, times the number of clock ticks per sample,
     * divided by the number of machine cycles per vblank (a constant) */
    if (channel == 'A') {
        channel_a_vblanks_remaining = total_samples * ticks_per_sample * (1.0 / CYCLES_PER_BLANK);
        channel_a_total_vblanks = channel_a_vblanks_remaining;
    } else if (channel == 'B') {
        channel_b_vblanks_remaining = total_samples * ticks_per_sample * (1.0 / CYCLES_PER_BLANK);
    }

    /* enable the timer */
    *timer0_control = TIMER_ENABLE | TIMER_FREQ_1;
}