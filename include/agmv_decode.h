#ifndef AGMV_DECODE_H
#define AGMV_DECODE_H

/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_decode.h
*   Date: 5/17/2024
*   Version: 2.0
*   Updated: 10/22/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agmv_defines.h>
#include <stdio.h>

void AGMV_DecompressLZSS(FILE* file, u8* bitstream_data, u32 usize);
void AGMV_BlockCopy(u32* dest, u32 *src, u32 doffset, u32 soffset, u8 block_w, u8 block_h, u32 width);
void AGMV_SBlockCopy(u32* dest, u32 *src, u32 doffset, u32 soffset, u8 block_w, u8 block_h, u32 width);
void AGMV_BlockFill(u32* dest, u32 color, u8 block_w, u8 block_h, u32 width);
int AGMV_DecodeHeader(FILE* file, AGMV* agmv);
int AGMV_DecodeFrameChunk(FILE* file, AGMV* agmv);
int AGMV_DebugFrameChunk(FILE* file, AGMV* agmv);
int AGMV_DecodeAudioChunk(FILE* file, AGMV* agmv);
int AGMV_DecodeAudioSample(FILE* file, AGMV* agmv);
int AGMV_DecodeVideo(const char* filename, u8 img_type);
int AGMV_DebugVideo(const char* filename, u32 start_frame, u32 end_frame);
int AGMV_DecodeAudio(const char* filename, AGMV_AUDIO_TYPE audio_type);
int AGMV_DecodeAGMV(const char* filename, u8 img_type, AGMV_AUDIO_TYPE audio_type);

#endif