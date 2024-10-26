#ifndef AGMV_PSX_DEFINES_H
#define AGMV_PSX_DEFINES_H

/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_psx_defines.h
*   Date: 10/1/2024
*   Version: 2.0
*   Updated: 10/26/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>
#include <libspu.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

typedef signed char  s8;
typedef signed short s16;
typedef signed long  s32;

typedef int Bool;

#define AGMV_TRUE  1
#define AGMV_FALSE 0

#define AGMV_NULL ((void*)0)

#define AGMV_CD_SEC_SIZE  2048
#define AGMV_CD_SEC_COUNT 1

#define ScratchMem (u16*)0x1F800000

typedef enum Error{
	NO_ERR = 0x0,
	INVALID_HEADER_FORMATTING_ERR = 0x1,
	FILE_NOT_FOUND_ERR = 0x2,
	MEMORY_CORRUPTION_ERR = 0x3,
}Error;

typedef struct File{
	CdlFILE* file;
	CdlLOC* origin;
	u32 sector;
	u32 file_pos;
	u16 buf_index;
	u8* buf;
	Error error;
}File;

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
	u16* palette0;
	u16* palette1;
}AGMV_MAIN_HEADER;

typedef struct AGMV_FRAME_CHUNK{
	char fourcc[4]; /* AGFC IN PLAIN ASCII */
	u32 frame_num;
	u16 frame_type;
	u32 usize;
	u32 csize;
}AGMV_FRAME_CHUNK;

typedef struct AGMV_AUDIO_CHUNK{
	char fourcc[4]; /* AGAC IN PLAIN ASCII */
	u32 size;
	u32 csize;
	u8* atsample;
	s8* satsample;
}AGMV_AUDIO_CHUNK;

typedef struct AGMV_FRAME{
	u32 width;
	u32 height;
	u16* img_data;
}AGMV_FRAME;

typedef struct AGMV_OFFSET{
	u32 offset;
	u32 frame_count;
}AGMV_OFFSET;

typedef struct AGMV_AUDIO_TRACK{
	u32 total_audio_duration;
	u_long start_addr, get_start_addr, pcm_addr;
	u16* pcm;
}AGMV_AUDIO_TRACK;

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

typedef struct AGMV_BITSTREAM{
	u8* data;
	u8* cbuffer;
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

typedef enum AGMV_FRAME_TYPE{
	AGMV_IFRAME = 0x1,
	AGMV_PFRAME = 0x2,
}AGMV_FRAME_TYPE;

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
	AGMV_AUDIO_STATE audio_state;
	AGMV_VIDEO_STATE video_state;
	Bool flip;
	u8 volume;
	u32 pframe_count;
	u32 iframe_count;
	u32 max_diff;
	AGMV_INFO info;
	Error error;
	File* file;
	AGMV_OFFSET* offset_table;
	RECT rect, irect, prect, screen;
	POLY_FT4* poly;
	SVECTOR VertPos[4];
	VECTOR  transVector, scale;
	SVECTOR rotVector;
	MATRIX Work;
	char* nextpri;
	SpuVoiceAttr  voiceAttributes;
	SpuCommonAttr commonAttributes; 
	SpuReverbAttr rev_attr;
}AGMV;

#endif