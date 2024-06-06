#ifndef AGMV_ENCODE_H
#define AGMV_ENCODE_H

/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_encode.h
*   Date: 5/17/2024
*   Version: 1.0
*   Updated: 6/2/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <stdio.h>
#include "agmv_defines.h"

#define AGMV_GBA_W   120
#define AGMV_GBA_H    80
#define AGMV_NDS_W   128
#define AGMV_NDS_H    96

u8 AGMV_ComparePFrameBlock(AGMV* agmv, u32 x, u32 y, AGMV_ENTRY* entry);
u8 AGMV_CompareIFrameBlock(AGMV* agmv, u32 x, u32 y, u32 color, AGMV_ENTRY* img_entry);
void AGMV_EncodeHeader(FILE* file, AGMV* agmv);
void AGMV_EncodeFrame(FILE* file, AGMV* agmv, u32* img_data);
void AGMV_AssembleIFrameBitstream(AGMV* agmv, AGMV_ENTRY* img_entry);
void AGMV_AssemblePFrameBitstream(AGMV* agmv, AGMV_ENTRY* img_entry);
u32 AGMV_LZSS(FILE* file, AGMV_BITSTREAM* in);
void AGMV_CompressAudio(AGMV* agmv);
void AGMV_EncodeAudioChunk(FILE* file, AGMV* agmv);
void AGMV_EncodeVideo(const char* filename, const char* dir, const char* basename, u8 img_type, u32 start_frame, u32 end_frame, u32 width, u32 height, u32 frames_per_second, AGMV_OPT opt, AGMV_QUALITY quality, AGMV_COMPRESSION compression);
void AGMV_EncodeAGMV(AGMV* agmv, const char* filename, const char* dir, const char* basename, u8 img_type, u32 start_frame, u32 end_frame, u32 width, u32 height, u32 frames_per_second, AGMV_OPT opt, AGMV_QUALITY quality, AGMV_COMPRESSION compression);
void AGMV_EncodeFullAGMV(AGMV* agmv, const char* filename, const char* dir, const char* basename, u8 img_type, u32 start_frame, u32 end_frame, u32 width, u32 height, u32 frames_per_second, AGMV_OPT opt, AGMV_QUALITY quality, AGMV_COMPRESSION compression);

#endif