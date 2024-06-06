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
*   Version: 1.0
*   Updated: 5/24/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include "agmv_defines.h"
#include <stdio.h>

int AGMV_DecodeHeader(FILE* file, AGMV* agmv);
int AGMV_DecodeFrameChunk(FILE* file, AGMV* agmv);
int AGMV_DecodeGBAFrameChunk(FILE* file, AGMV* agmv);
int AGMV_DecodeAudioChunk(FILE* file, AGMV* agmv);
int AGMV_DecodeGBAAudioChunk(FILE* file, AGMV* agmv);
int AGMV_DecodeVideo(const char* filename, u8 img_type);
int AGMV_DecodeAudio(const char* filename, AGMV_AUDIO_TYPE audio_type);
int AGMV_DecodeAGMV(const char* filename, u8 img_type, AGMV_AUDIO_TYPE audio_type);

#endif