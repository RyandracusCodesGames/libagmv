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
*   Version: 1.1
*   Updated: 6/13/2024
*   Author: Ryandracus Chapman
*
********************************************/

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

#define AGMV_MAX_CLR     524287
#define MAX_OFFSET_TABLE 40000

#define AGMV_FILL_FLAG    0x4E
#define AGMV_NORMAL_FLAG  0x2f
#define AGMV_COPY_FLAG    0x5E
#define AGMV_FILL_COUNT     14
#define AGMV_COPY_COUNT     13

/* AGMV OPTIMIZATION FLAGS */
typedef enum AGMV_OPT{
	AGMV_OPT_I       = 0x1,  /* 512 COLORS, BITSTREAM V1, HEAVY PDIFS */
	AGMV_OPT_II      = 0x2,  /* 256 COLORS, BITSTREAM V2, LIGHT PDIFS */
	AGMV_OPT_III     = 0x3,  /* 512 COLORS, BITSTREAM V1, LIGHT PDIFS */
	AGMV_OPT_ANIM    = 0x4,  /* 256 COLORS, BITSTREAM V2, HEAVY PDIFS */
	AGMV_OPT_GBA_I   = 0x5,  /* 512 COLORS, BITSTREAM V1, HEAVY PDIFS, REDUCED RES */
	AGMV_OPT_GBA_II  = 0x6,  /* 256 COLORS, BITSTREAM V2, HEAVY PDIFS, REDUCED RES */
	AGMV_OPT_GBA_III = 0x7,  /* 512 COLORS, BITSTREAM V2, LIGHT PDIFS, REDUCED RES */
	AGMV_OPT_NDS     = 0x8,  /* 512 COLORS, BITSTREAM V1, HEAVY PDIFS, REDUCED RES*/
}AGMV_OPT;

typedef enum AGMV_QUALITY{
	AGMV_HIGH_QUALITY = 0x1,
	AGMV_MID_QUALITY  = 0x2,
	AGMV_LOW_QUALITY  = 0x3,
}AGMV_QUALITY;

typedef enum AGMV_COMPRESSION{
	AGMV_LZSS_COMPRESSION = 0x1,
	AGMV_LZ77_COMPRESSION = 0x2,
}AGMV_COMPRESSION;

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
	u32 uncompressed_size;
	u32 compressed_size;
}AGMV_FRAME_CHUNK;

typedef struct AGMV_AUDIO_CHUNK{
	char fourcc[4]; /* AGAC IN PLAIN ASCII */
	u32 size;
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

typedef struct AGMV{
	AGMV_MAIN_HEADER header;
	AGMV_FRAME_CHUNK* frame_chunk;
	AGMV_AUDIO_CHUNK* audio_chunk;
	AGMV_BITSTREAM* bitstream;
	AGMV_FRAME* frame;
	AGMV_FRAME* iframe;
	AGMV_AUDIO_TRACK* audio_track;
	AGMV_ENTRY* iframe_entries;
	AGMV_OPT opt;
	AGMV_COMPRESSION compression;
	u32 frame_count;
	f32 leniency;
	u32 offset_table[MAX_OFFSET_TABLE];
	Bool enable_audio;
	f32 volume;
}AGMV;

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

#endif