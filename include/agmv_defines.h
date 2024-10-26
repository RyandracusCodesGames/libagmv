#ifndef AGMV_DEFINES_H
#define AGMV_DEFINES_H

/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_defines.h
*   Date: 5/17/2024
*   Version: 2.0
*   Updated: 10/23/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <stdio.h>

/*---------AGMV FUNDAMENTAL DATA TYPES-------*/

typedef unsigned char   u8;
typedef unsigned short u16;
typedef unsigned long  u32;

typedef signed char   s8;
typedef signed short s16;
typedef signed long  s32;

typedef float f32;

typedef int Bool;

#define TRUE  1
#define FALSE 0

/*-------------AGMV ERROR CODES--------------*/

typedef enum Error{
	NO_ERR = 0x0,
	INVALID_HEADER_FORMATTING_ERR = 0x1,
	FILE_NOT_FOUND_ERR = 0x2,
	MEMORY_CORRUPTION_ERR = 0x3,
}Error;

/*-----------AGMV DATA STRUCTURES-----------*/

#define AGMV_MAX_CLR     	131071
#define AGMV_MAX_DIFF       15
#define AGMV_MAX_15BPP_DIFF 7
#define MAX_OFFSET_TABLE 	20000
#define AGMV_MIN_DISTR      0.50f

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

#define AGMV_FILL_COUNT     13
#define AGMV_COPY_COUNT     11
#define AGMV_MV_COUNT       10
#define AGMV_ICOPY_COUNT    16
#define AGMV_PCOPY_COUNT    12
#define AGMV_PMV_COUNT      10
#define AGMV_VQ2_COUNT       2
#define AGMV_VQ4_COUNT       4

#define AGMV_SEARCH_RANGE    7
#define AGMV_SSEARCH_RANGE  32 

#define AGMV_MAX_PRINT     200

/* AGMV OPTIMIZATION FLAGS */
typedef enum AGMV_OPT{
	AGMV_OPT_I       = 0x1,  /* 512   COLORS, BITSTREAM V1, HEAVY PDIFS */
	AGMV_OPT_II      = 0x2,  /* 256   COLORS, BITSTREAM V2, LIGHT PDIFS */
	AGMV_OPT_III     = 0x3,  /* 512   COLORS, BITSTREAM V1, LIGHT PDIFS */
	AGMV_OPT_IV      = 0x4,  /* 32768 COLORS, BITSTREAM V1, LIGHT PDIFS */
	AGMV_OPT_ANIM    = 0x5,  /* 256   COLORS, BITSTREAM V2, HEAVY PDIFS */
	AGMV_OPT_GBA_I   = 0x6,  /* 512   COLORS, BITSTREAM V1, HEAVY PDIFS, REDUCED RES */
	AGMV_OPT_GBA_II  = 0x7,  /* 256   COLORS, BITSTREAM V2, HEAVY PDIFS, REDUCED RES */
	AGMV_OPT_GBA_III = 0x8,  /* 512   COLORS, BITSTREAM V2, LIGHT PDIFS, REDUCED RES */
	AGMV_OPT_GBA_IV  = 0x9,  /* 32768 COLORS, BITSTREAM V1, HEAVY PDIFS, REDUCED RES*/
	AGMV_OPT_PSX_I   = 0x0A, /* 512 COLORS, BITSTREAM V1, HEAVY PDIFS, REDUCED RES */
	AGMV_OPT_PSX_II  = 0x0B, /* 256 COLORS, BITSTREAM V2, LIGHT PDIFS, REDUCED RES */
	AGMV_OPT_PSX_III = 0x0C, /* 512 COLORS, BITSTREAM V1, LIGHT PDIFS, REDUCED RES*/
	AGMV_OPT_PSX_IV  = 0x0D, /* 32768 COLORS, BITSTREAM V1, LIGHT PDIFS, REDUCED RES*/
	AGMV_OPT_NDS_I   = 0x0E, /* 512 COLORS, BITSTREAM V1, HEAVY PDIFS, REDUCED RES */
	AGMV_OPT_NDS_II  = 0x0F, /* 256 COLORS, BITSTREAM V2, LIGHT PDIFS, REDUCED RES */
	AGMV_OPT_NDS_III = 0x10, /* 512 COLORS, BITSTREAM V1, LIGHT PDIFS, REDUCED RES*/
	AGMV_OPT_NDS_IV  = 0x11, /* 32768 COLORS, BITSTREAM V1, HEAVY PDIFS, REDUCED RES*/
}AGMV_OPT;

typedef enum AGMV_QUALITY{
	AGMV_HIGH_QUALITY   = 0x1,
	AGMV_MID_QUALITY    = 0x2,
	AGMV_LOW_QUALITY    = 0x3,
	AGMV_CUSTOM_QUALITY = 0x4,
}AGMV_QUALITY;

typedef struct AGMV_FLAG_COUNT{
	int fill;
	int icopy;
	int copy;
	int pcopy;
	int mv;
	int pmv;
	int vq2;
	int vq4;
}AGMV_FLAG_COUNT;

typedef struct AGMV_CUSTOMIZATION{
	AGMV_FLAG_COUNT b4x4;
	AGMV_FLAG_COUNT b8x4;
	AGMV_FLAG_COUNT b2x4;
	AGMV_FLAG_COUNT b8x8;
	AGMV_FLAG_COUNT b4x8;
}AGMV_CUSTOMIZATION;

typedef enum AGMV_COMPRESSION{
	AGMV_LZSS_COMPRESSION      = 0x1,
	AGMV_NONE_COMPRESSION      = 0x2,
}AGMV_COMPRESSION;

typedef enum AGMV_BLOCK_SIZE{
	AGMV_BLOCK_4x4 = 0x0,
	AGMV_BLOCK_8x4 = 0x1,
	AGMV_BLOCK_2x4 = 0x2,
	AGMV_BLOCK_8x8 = 0x3,
}AGMV_BLOCK_SIZE;

typedef struct AGMV_BLOCK_COUNT{
	u8 b4x4;
	u8 b8x4;
	u8 b2x4;
	u8 b8x8;
	u8 temp;
	u8 temp1;
	u32 copyx1, copyy1;
	u32 copyx2, copyy2;
	u32 copyx3, copyy3;
	u32 copyx4, copyy4;
}AGMV_BLOCK_COUNT;

typedef struct AGMV_SBLOCK_COUNT{
	u8 sector1,  sector2,  sector3,  sector4;
	u8 sector5,  sector6,  sector7,  sector8;
}AGMV_SBLOCK_COUNT;

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
	u32 palette0[256];
	u32 palette1[256];
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
	u32 csize;
	u8* atsample;
	s8* satsample;
}AGMV_AUDIO_CHUNK;

typedef struct AGMV_FRAME{
	u32 width;
	u32 height;
	u32* img_data;
}AGMV_FRAME;

typedef struct AGMV_AUDIO_TRACK{
	u32 total_audio_duration;
	u32 start_point;
	u16* pcm;
	u8* pcm8;
}AGMV_AUDIO_TRACK;

typedef struct AGMV_ENTRY{
	u8 pal_num;
	u8 index;
	u32 occurence;
}AGMV_ENTRY;

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
	u32 len;
	u32 pos;
}AGMV_BITSTREAM;

typedef struct AGMV_MV{
	int x, y;
}AGMV_MV;

typedef struct AGMV_SMV{
	AGMV_MV mv, pmv;
}AGMV_SMV;

typedef struct AGMV_SECTOR{
	AGMV_SMV quad[8];
}AGMV_SECTOR;

typedef struct AGMV_SUMMATION{
	u32 sum1, sum2, sum3, sum4;
}AGMV_SUMMATION;

typedef struct AGMV_Offset{
	u32 pos;
	u32 frame_count;
}AGMV_Offset;

typedef struct AGMV_DOUBLE_BUFFER{
	u16 curr_table[200];
	u16 read_table[200];
	u16 type_table[200];
	u8  curr_pos;
	u8  read_pos;
}AGMV_DOUBLE_BUFFER;

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

typedef struct AGMV_COFFRE{
	u32 color;
	u8 freq;
}AGMV_COFFRE;

typedef struct AGMV_CHEST{
	AGMV_COFFRE coffre[65];
	u8 num;
}AGMV_CHEST;

typedef struct AGMV_CRATE{
	AGMV_CHEST c4x4;
	AGMV_CHEST c8x4;
	AGMV_CHEST c2x4;
	AGMV_CHEST c8x8;
	AGMV_CHEST c4x8;
}AGMV_CRATE;

typedef enum AGMV_IMG_TYPE{
	AGMV_IMG_BMP = 0x1,
	AGMV_IMG_TGA = 0x2,
	AGMV_IMG_TIM = 0x3,
	AGMV_IMG_PCX = 0x4,
	AGMV_IMG_LMP = 0x5,
	AGMV_IMG_PVR = 0x6,
	AGMV_IMG_GXT = 0x7,
	AGMV_IMG_BTI = 0x8,
	AGMV_IMG_3DF = 0x9,
	AGMV_IMG_PPM = 0x0A,
	AGMV_IMG_LBM = 0x0B,
}AGMV_IMG_TYPE;

typedef enum AGMV_AUDIO_TYPE{
	AGMV_AUDIO_WAV  = 0x1,
	AGMV_AUDIO_AIFF = 0x2,
	AGMV_AUDIO_AIFC = 0x3,
	AGMV_AUDIO_RAW  = 0x4,
}AGMV_AUDIO_TYPE;

typedef enum AGMV_FRAME_TYPE{
	AGMV_IFRAME = 0x1,
	AGMV_PFRAME = 0x2,
}AGMV_FRAME_TYPE;

typedef struct AGMV{
	AGMV_MAIN_HEADER header;
	AGMV_FRAME_CHUNK* frame_chunk;
	AGMV_AUDIO_CHUNK* audio_chunk;
	AGMV_BITSTREAM* bitstream;
	AGMV_MV mv, pmv, mv8x4, pmv8x4, mv2x4, pmv2x4, mv8x8, pmv8x8; 
	u32 prev_fill_color;
	u32 vq2color1, vq2color2;
	AGMV_FRAME* frame;
	AGMV_FRAME* iframe;
	AGMV_FRAME* prev_frame;
	AGMV_AUDIO_TRACK* audio_track;
	AGMV_ENTRY* iframe_entries;
	AGMV_ENTRY* image_entries;
	AGMV_OPT opt;
	AGMV_COMPRESSION compression;
	AGMV_QUALITY quality;
	u32 frame_count;
	f32 leniency;
	Bool lenient;
	u32 offset_table[MAX_OFFSET_TABLE];
	AGMV_AUDIO_STATE audio_state;
	AGMV_VIDEO_STATE video_state;
	Bool enable_audio;
	AGMV_AUDIO_TYPE audio_type;
	f32 volume;
	u32 pframe_count;
	u32 iframe_count;
	u32 max_diff;
	AGMV_INFO info;
	Error error;
	Bool sowt;
	f32 normal_distribution;
	AGMV_DOUBLE_BUFFER db;
	AGMV_CUSTOMIZATION custom;
	FILE* file;
	FILE* audio;
}AGMV;

#endif