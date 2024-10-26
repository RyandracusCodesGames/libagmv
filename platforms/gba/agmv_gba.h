#ifndef AGMV_GBA_H
#define AGMV_GBA_H

/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_gba.h
*   Date: 5/20/2024
*   Version: 2.0
*   Updated: 10/24/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <stdlib.h>
#include <string.h>

typedef unsigned char   u8;
typedef unsigned short u16;
typedef unsigned long  u32;

typedef signed char s8;

typedef float f32;

typedef int Bool;

#define TRUE  1
#define FALSE 0

typedef struct File{
	u32 len;
	u32 pos;
	const u8* data;
}File;

#define SEEK_SET 0
#define SEEK_CUR 1

#define AGMV_IWRAM __attribute__((section (".iwram")))
#define AGMV_EWRAM __attribute__((section (".ewram")))

#define AGMV_DMA_ENABLE		0x80000000
#define AGMV_DMA_REPEAT		0x2000000
#define AGMV_DMA_16			0x00000000
#define AGMV_DMA_32			0x04000000

#define AGMV_REG_DMA3SAD	*(volatile u32*)(0x04000000 + 0x0d4)
#define AGMV_REG_DMA3DAD	*(volatile u32*)(0x04000000 + 0x0d8)
#define AGMV_REG_DMA3CNT	*(volatile u32*)(0x04000000 + 0x0dc)

#define AGMV_DMA_SRC_FIXED	0x01000000

#define AGMV_MASTER_SOUND  (volatile u16*)0x4000082;

#define AGMV_DMA_Copy(channel, source, dest, mode) {\
	AGMV_REG_DMA##channel##SAD = (u32)(source);\
	AGMV_REG_DMA##channel##DAD = (u32)(dest);\
	AGMV_REG_DMA##channel##CNT = AGMV_DMA_ENABLE | (mode); \
}

static inline void AGMV_IWRAM AGMV_DMACopy16(u16* source, u16* dest, u32 size){
	AGMV_REG_DMA3SAD = (u32)(source);
	AGMV_REG_DMA3DAD = (u32)(dest);
	AGMV_REG_DMA3CNT = AGMV_DMA_ENABLE | AGMV_DMA_16 | size;
}

/*--------------GBA FILE I/O ROUTINES----------------*/
static inline Bool AGMV_IWRAM AGMV_EOF(File* file){
	if(file->pos + 1 >= file->len){
		return TRUE;
	}
	else return FALSE;
}

static inline u8 AGMV_IWRAM AGMV_ReadByte(File* file){
	return file->data[file->pos++];
}

static inline void AGMV_IWRAM AGMV_Memcpy(u8* dest, u8* src, u32 n){
	while(n >= 4){
		*(dest++) = *(src++);
		*(dest++) = *(src++);
		*(dest++) = *(src++);
		*(dest++) = *(src++);
		n -= 4;
	}
	while(n--){
		*(dest++) = *(src++);
	}
}

static inline void AGMV_IWRAM AGMV_Memcpy16(u16* dest, u16* src, u32 n){
	while(n >= 4){
		*(dest++) = *(src++);
		*(dest++) = *(src++);
		*(dest++) = *(src++);
		*(dest++) = *(src++);
		n -= 4;
	}
	while(n--){
		*(dest++) = *(src++);
	}
}

static inline void AGMV_IWRAM AGMV_Memset(u8* dest, u32 num, u32 n){
	while(n >= 4){
		*(dest++) = num;
		*(dest++) = num;
		*(dest++) = num;
		*(dest++) = num;
		n -= 4;
	}
	while(n--){
		*(dest++) = num;
	}
}

static inline void AGMV_IWRAM AGMV_Memset16(u16* dest, u32 num, u32 n){
	while(n >= 4){
		*(dest++) = num;
		*(dest++) = num;
		*(dest++) = num;
		*(dest++) = num;
		n -= 4;
	}
	while(n--){
		*(dest++) = num;
	}
}

static inline u8 AGMV_IWRAM AGMV_GetB(u16 color){
	return (color >> 10) & 0x1f;
}

static inline u8 AGMV_IWRAM AGMV_GetG(u16 color){
	return (color >> 5) & 0x1f;
}

static inline u8 AGMV_IWRAM AGMV_GetR(u16 color){
	return (color & 0x1f);
}

static inline u16 AGMV_IWRAM AGMV_InterpColor(u16 color1, u16 color2){
	u8 r1, g1, b1, r2, g2, b2, r, g, b;
	
	r1 = AGMV_GetR(color1);
	g1 = AGMV_GetG(color1);
	b1 = AGMV_GetB(color1);
	
	r2 = AGMV_GetR(color2);
	g2 = AGMV_GetG(color2);
	b2 = AGMV_GetB(color2);
	
	r = (r1 + r2) >> 1;
	g = (g1 + g2) >> 1;
	b = (b1 + b2) >> 1;
	
	return b << 10 | g << 5 | r;
}

static inline void AGMV_IWRAM AGMV_BlockCopy(u16* dest, u16 *src, u32 doffset, u32 soffset, u8 block_w, u8 block_h, u16 width){
	u16 *dest_ptr, *src_ptr, i;
	
	dest_ptr = dest + doffset;
	src_ptr  = src  + soffset;
	
	for(i = 0; i < block_h; i += 4){
		AGMV_DMACopy16(src_ptr,dest_ptr,block_w);
		dest_ptr += width;   src_ptr += width;
		AGMV_DMACopy16(src_ptr,dest_ptr,block_w);
		dest_ptr += width;   src_ptr += width;
		AGMV_DMACopy16(src_ptr,dest_ptr,block_w);
		dest_ptr += width;   src_ptr += width;
		AGMV_DMACopy16(src_ptr,dest_ptr,block_w);
		dest_ptr += width;   src_ptr += width;
	}
}

static inline void AGMV_IWRAM AGMV_SBlockCopy(u16* dest, u16 *src, u32 doffset, u32 soffset, u8 block_w, u8 block_h, u16 width){
	u16 *dest_ptr, *src_ptr, i;
	
	dest_ptr = dest + doffset;
	src_ptr  = src  + soffset;
	
	for(i = 0; i < block_h; i += 2){
		AGMV_DMACopy16(src_ptr,dest_ptr,block_w);
		dest_ptr += width;   src_ptr += width;
		AGMV_DMACopy16(src_ptr,dest_ptr,block_w);
		dest_ptr += width;   src_ptr += width;
	}
}

static inline void AGMV_IWRAM AGMV_BlockFill(u16 *dest, u16* color_arr, u8 block_w, u8 block_h, u16 width){
	while(block_h--){
		AGMV_DMACopy16(color_arr,dest,block_w);
		dest += width;
	}
}

static inline void AGMV_IWRAM AGMV_ReadNBytes(File* file, u8* buf, u32 n){
	u32 size;
	u8 *data;
	
	size = n;
	data = file->data + file->pos;

	if((u32)data & 1){
		*(buf++) = *(data++);
		n--;
		
		AGMV_DMACopy16((u16*)data,(u16*)buf,n>>1);
		
		if(n & 1){
			buf[n-1] = data[n-1];
		}
	}
	else{
		AGMV_DMACopy16((u16*)data,(u16*)buf,n>>1);
		
		if(n & 1){
			buf[n-1] = data[n-1];
		}
	}
	
	file->pos += size;
}

static inline u16 AGMV_IWRAM AGMV_ReadShort(File* file){
	u8 lsb = AGMV_ReadByte(file);
	u8 msb = AGMV_ReadByte(file);
	
	return msb << 8 | lsb;
}

static inline u32 AGMV_IWRAM AGMV_ReadLong(File* file){
	u8 lsb = AGMV_ReadByte(file);
	u8 lsb2 = AGMV_ReadByte(file);
	u8 msb1 = AGMV_ReadByte(file);
	u8 msb2 = AGMV_ReadByte(file);
	
	return msb2 << 24 | msb1 << 16 | lsb2 << 8 | lsb;
}
void Open(File* file, const u8* data, u32 len);

static inline u32 AGMV_IWRAM tell(File* file){
	return file->pos;
}

static inline void AGMV_IWRAM seek(File* file, u32 offset, u8 mode){
	if(mode == SEEK_SET){
		file->pos = offset;
	}
	if(mode == SEEK_CUR){
		if(file->pos + offset < file->len){
			file->pos += offset;
		}
	}
}

void read(File* file, u8* buf, u32 count);

static inline void AGMV_IWRAM AGMV_ReadFourCC(File* file, char fourcc[4]){
	fourcc[0] = AGMV_ReadByte(file);
	fourcc[1] = AGMV_ReadByte(file);
	fourcc[2] = AGMV_ReadByte(file);
	fourcc[3] = AGMV_ReadByte(file);
}

/*---------GBA AUDIO ROUTINES----------------------*/

void memcpy16_dma(unsigned short* dest, unsigned short* source, int amount);

/* this function checks whether a particular button has been pressed */
unsigned char button_pressed(unsigned short button);

/* play a sound with a number of samples, and sample rate on one channel 'A' or 'B' */
void AGMV_IWRAM play_sound(const signed char* sound, int total_samples, int sample_rate, char channel);

/*-----------AGMV FILE UTILITY FUNCTIONS----------------*/

static inline Bool AGMV_IWRAM AGMV_IsCorrectFourCC(char fourcc[4], char f, char o, char u, char r){
	if(f != fourcc[0] || o != fourcc[1] || u != fourcc[2] || r != fourcc[3]){
		return FALSE;
	}
	else return TRUE;
}

static inline void AGMV_IWRAM AGMV_FindNextFrameChunk(File* file){
	u32 pos;
	Bool isFrame;
	char fourcc[4];

	AGMV_ReadFourCC(file,fourcc);
	isFrame = AGMV_IsCorrectFourCC(fourcc,'A','G','F','C');

	if(isFrame){
		pos = tell(file);
		seek(file,pos-4,SEEK_SET);
		return;
	}

	while(!isFrame && !AGMV_EOF(file)){
		AGMV_ReadFourCC(file,fourcc);
		isFrame = AGMV_IsCorrectFourCC(fourcc,'A','G','F','C');
		
		if(!isFrame){
			pos = tell(file);
			seek(file,pos-3,SEEK_SET);
		}
	}

	pos = tell(file);
	seek(file,pos-4,SEEK_SET);
}

static inline u16 AGMV_IWRAM AGMV_PeekFrameChunk(File* file){
	u32 fourcc, frame_num, frame_type, usize, csize;
	int pos;
	
	AGMV_FindNextFrameChunk(file);
	
	fourcc     = AGMV_ReadLong(file);
	frame_num  = AGMV_ReadLong(file);
	frame_type = AGMV_ReadShort(file);
	
	pos = tell(file);
	
	seek(file,pos-10,SEEK_SET);
	
	return frame_type;
}

static inline void AGMV_IWRAM AGMV_FindNextAudioChunk(File* file){
	u32 pos;
	Bool isAudio;
	char fourcc[4];

	AGMV_ReadFourCC(file,fourcc);
	isAudio = AGMV_IsCorrectFourCC(fourcc,'A','G','A','C');

	if(isAudio){
		pos = tell(file);
		seek(file,pos-4,SEEK_SET);
		return;
	}

	while(!isAudio && !AGMV_EOF(file)){
		AGMV_ReadFourCC(file,fourcc);
		isAudio = AGMV_IsCorrectFourCC(fourcc,'A','G','A','C');
		
		if(!isAudio){
			pos = tell(file);
			seek(file,pos-3,SEEK_SET);
		}
	}

	pos = tell(file);
	seek(file,pos-4,SEEK_SET);
}

u16 AGMV_SkipFrameChunk(File* file);
void AGMV_SkipAudioChunk(File* file);

/*-------------AGMV ERROR CODES--------------*/

typedef enum Error{
	NO_ERR = 0x0,
	INVALID_HEADER_FORMATTING_ERR = 0x1,
	FILE_NOT_FOUND_ERR = 0x2,
	MEMORY_CORRUPTION_ERR = 0x3,
}Error;

/*-----------AGMV DATA STRUCTURES-----------*/

#define AGMV_MAX_CLR      65535
#define MAX_OFFSET_TABLE  9000

#define AGMV_FILL_FLAG    0x1
#define AGMV_NORMAL_FLAG  0x2
#define AGMV_COPY_FLAG    0x3
#define AGMV_MV_FLAG      0x4
#define AGMV_ICOPY_FLAG   0x5
#define AGMV_PCOPY_FLAG   0x6
#define AGMV_PMV_FLAG     0x7
#define AGMV_VQ2_FLAG     0x8
#define AGMV_VQ4_FLAG     0x9
#define AGMV_SKIP_8       0x0A
#define AGMV_SMV_FLAG     0x0B
#define AGMV_ICOPYR_FLAG  0x0C
#define AGMV_PFILL_FLAG   0x0D
#define AGMV_PVQ2_FLAG    0x0E

typedef enum AGMV_BLOCK_SIZE{
	AGMV_BLOCK_4x4 = 0x0,
	AGMV_BLOCK_8x4 = 0x1,
	AGMV_BLOCK_2x4 = 0x2,
	AGMV_BLOCK_8x8 = 0x3,
}AGMV_BLOCK_SIZE;

static inline AGMV_IWRAM u8 AGMV_GetBlockWidth(u8 block_size){
	if(block_size == AGMV_BLOCK_4x4){
		return 4;
	}
	else if(block_size == AGMV_BLOCK_2x4){
		return 2;
	}
	else return 8;
}

static inline AGMV_IWRAM u8 AGMV_GetBlockHeight(u8 block_size){
	if(block_size == AGMV_BLOCK_8x8){
		return 8;
	}
	else return 4;
}

typedef struct AGMV_MAIN_HEADER{
	char fourcc[4]; /* AGMV IN PLAIN ASCII */
	u32 num_of_frames;
	u32 width;
	u32 height;
	u8  fmt;
	u8  version;
	u32 frames_per_second;
	u32 total_audio_duration;
	u32 sample_rate;
	u32 audio_size;
	u16 num_of_channels;
	u16 bits_per_sample;
	u16 palette0[256];
	u16 palette1[256];
}AGMV_MAIN_HEADER;

typedef struct AGMV_FRAME_CHUNK{
	char fourcc[4]; /* AGFC IN PLAIN ASCII */
	u32 frame_num;
	u16 frame_type;
	u32 uncompressed_size;
	u32 compressed_size;
}AGMV_FRAME_CHUNK;

typedef struct AGMV_AUDIO_CHUNK{
	char fourcc[4]; /* AGAC IN PLAIN ASCII */
	u32 size;
	u32 total_size;
	u32 csize;
	s8* sample;
	u32 point;
	Bool enable_audio;
}AGMV_AUDIO_CHUNK;

typedef struct AGMV_FRAME{
	u32 width;
	u32 height;
	u32 dwidth;
	u32 dheight;
	u16* img_data;
}AGMV_FRAME;

typedef struct AGMV_AUDIO_TRACK{
	u32 total_audio_duration;
	u32 start_point;
	u8* pcm;
}AGMV_AUDIO_TRACK;

typedef struct AGMV_BITSTREAM{
	u8* data;
	u32 len;
	u32 pos;
}AGMV_BITSTREAM;

typedef enum AGMV_AUDIO_STATE{
	AGMV_AUDIO_PLAY    = 0x1,
	AGMV_AUDIO_PAUSE   = 0x2,
	AGMV_AUDIO_STOP    = 0x3,
	AGMV_AUDIO_DISABLE = 0x4,
}AGMV_AUDIO_STATE;

typedef enum AGMV_VIDEO_STATE{
	AGMV_VIDEO_PLAY    = 0x1,
	AGMV_VIDEO_PAUSE   = 0x2,
}AGMV_VIDEO_STATE;

typedef struct AGMV_INFO{
	u32 width;
	u32 height;
	u32 number_of_frames;
	u8 version;
	u32 total_audio_duration;
	u32 sample_rate;
	u32 audio_size;
	u16 number_of_channels;
	u16 bits_per_sample;
}AGMV_INFO;

typedef struct AGMV_OFFSET{
	u32 offset;
	u32 frame;
}AGMV_OFFSET;

typedef struct AGMV{
	AGMV_MAIN_HEADER header;
	AGMV_FRAME_CHUNK* frame_chunk;
	AGMV_AUDIO_CHUNK* audio_chunk;
	AGMV_BITSTREAM* bitstream;
	u16 prev_fill_color;
	u16 vq2color1, vq2color2;
	AGMV_FRAME* frame;
	AGMV_FRAME* iframe;
	AGMV_FRAME* prev_frame;
	AGMV_AUDIO_TRACK* audio_track;
	u32 frame_count;
	u32 pframe_count;
	u32 iframe_count;
	u32 frame_index;
	AGMV_OFFSET* offset_table;
	AGMV_AUDIO_STATE audio_state;
	AGMV_VIDEO_STATE video_state;
	AGMV_INFO info;
	Bool disable_all_audio;
	int volume;
	Error error;
	File* file;
}AGMV;

typedef enum AGMV_FRAME_TYPE{
	AGMV_IFRAME = 0x1,
	AGMV_PFRAME = 0x2,
}AGMV_FRAME_TYPE;

/*-----------------AGMV DECODING------------------*/

void AGMV_IWRAM AGMV_DecompressLZSS(File* file, u8* bitstream_data, u32 usize);
void AGMV_DisableAllAudio(AGMV* agmv);
void AGMV_EnableAllAudio(AGMV* agmv);
int AGMV_DecodeHeader(File* file, AGMV* agmv);
void DestroyAGMV(AGMV* agmv);
int AGMV_IWRAM AGMV_DecodeFrameChunk(File* file, AGMV* agmv);
int AGMV_IWRAM AGMV_DecodeAudioChunk(File* file, AGMV* agmv);

#define AGMV_MODE_3   0x3
#define AGMV_MODE_5   0x5

/* the bit positions indicate each button - the first bit is for A, second for
 * B, and so on, each constant below can be ANDED into the register to get the
 * status of any one button */
#define BUTTON_A (1 << 0)
#define BUTTON_B (1 << 1)
#define BUTTON_SELECT (1 << 2)
#define BUTTON_START (1 << 3)
#define BUTTON_RIGHT (1 << 4)
#define BUTTON_LEFT (1 << 5)
#define BUTTON_UP (1 << 6)
#define BUTTON_DOWN (1 << 7)
#define BUTTON_R (1 << 8)
#define BUTTON_L (1 << 9)

/*-------------AGMV PLAYBACK LIBRARY------------------*/
void AGMV_AllocResources(AGMV* agmv);
AGMV* AGMV_Open(const u8* data, u32 len);
void AGMV_Close(AGMV* agmv);
/*-------------SET STATE VARIABLES OF AUDIO AND VIDEO-------------------*/
void AGMV_InitSound(AGMV* agmv);
void AGMV_SetAudioState(AGMV* agmv, AGMV_AUDIO_STATE audio);
void AGMV_SetVideoState(AGMV* agmv, AGMV_VIDEO_STATE video);
AGMV_AUDIO_STATE AGMV_GetAudioState(AGMV* agmv);
AGMV_VIDEO_STATE AGMV_GetVideoState(AGMV* agmv);
void AGMV_PlayVideo(AGMV* agmv);
void AGMV_PauseVideo(AGMV* agmv);
void AGMV_DisableAudio(AGMV* agmv);
void AGMV_MuteAudio(AGMV* agmv);
void AGMV_SetVolume(AGMV* agmv, int volume);
static inline void AGMV_IWRAM AGMV_StoreFrameOffset(AGMV* agmv, u32 offset){
	if(agmv->frame_chunk->frame_type == AGMV_IFRAME){
		agmv->offset_table[agmv->iframe_count].offset = offset;
		agmv->offset_table[agmv->iframe_count].frame = agmv->frame_count + 1;
		agmv->iframe_count++;
	}
}
AGMV_INFO AGMV_GetVideoInfo(AGMV* agmv);
/*--------------PRIMARY AUDIO AND VIDEO PLAYBACK ROUTINES------------------*/

static inline Bool AGMV_IWRAM AGMV_IsVideoDone(AGMV* agmv){
	if(agmv->frame_count >= agmv->header.num_of_frames){
		return TRUE;
	}
	else return FALSE;
}

static inline void AGMV_IWRAM AGMV_StreamMovie(AGMV* agmv){
	u32 offset;
	if(agmv->video_state == AGMV_VIDEO_PLAY){
		if(!AGMV_IsVideoDone(agmv)){
			AGMV_FindNextFrameChunk(agmv->file);
			
			offset    = tell(agmv->file);
			
			AGMV_DecodeFrameChunk(agmv->file,agmv);
			AGMV_StoreFrameOffset(agmv,offset);
			
			if(agmv->header.total_audio_duration != 0){
				AGMV_FindNextAudioChunk(agmv->file);
				
				if(agmv->audio_state == AGMV_AUDIO_PLAY){
					AGMV_DecodeAudioChunk(agmv->file,agmv); 
					play_sound(agmv->audio_chunk->sample, agmv->audio_chunk->size, agmv->header.sample_rate, 'A');	
				}
				else{
					AGMV_SkipAudioChunk(agmv->file);
				}
			}
		}
	}
}
void SetVideoMode(u8 mode);
void EnableTimer2();
void AGMV_EnableMasterSound();
void AGMV_DisableMasterSound();
void AGMV_ResetVideo(File* file, AGMV* agmv);
void AGMV_SkipForwards(File* file, AGMV* agmv, int n);
void AGMV_SkipBackwards(File* file, AGMV* agmv, int n);
/* ONLY CALL SKIP TO FUNCTION AFTER ALL FRAMES HAVE BEEN READ */
void AGMV_SkipTo(File* file, AGMV* agmv, int n);

static inline void AGMV_IWRAM AGMV_DisplayFrame(u16* vram, u16 width, u16 height, AGMV* agmv){
	u16 frame_width = agmv->frame->width;
	u16 frame_height = agmv->frame->height;
	u32 yoffset, y2w;
	
	u16* img_data = agmv->frame->img_data;
	u16* vrm = vram;
	
	y2w = 0;
	yoffset = (frame_height - 1) * width;
	
	int y;
	for(y = 0; y < frame_height; y += 2){
		AGMV_DMACopy16(img_data+y2w,vrm+yoffset,frame_width);
		
		y2w += frame_width;
		yoffset -= width;
		
		AGMV_DMACopy16(img_data+y2w,vrm+yoffset,frame_width);
		
		y2w += frame_width;
		yoffset -= width;
	}
}

#endif