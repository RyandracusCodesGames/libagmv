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
*   Version: 2.0
*   Updated: 10/19/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <stdio.h>
#include <agmv_defines.h>

#define AGMV_HQ_GBA_W   192
#define AGMV_HQ_GBA_H   128

#define AGMV_LQ_GBA_W   120
#define AGMV_LQ_GBA_H    80

#define AGMV_PSX_W   192
#define AGMV_PSX_H   128

#define AGMV_NDS_W   216
#define AGMV_NDS_H   144

extern Bool dither;
extern Bool desperation;

void AGMV_EnableDither();
void AGMV_DisableDither();

AGMV_BLOCK_COUNT AGMV_GetFillBlockCount(AGMV* agmv, u32 x, u32 y, u32 color);
AGMV_BLOCK_COUNT AGMV_GetCopyBlockCount(AGMV* agmv, u32 x, u32 y);
AGMV_BLOCK_COUNT AGMV_GetPCopyBlockCount(AGMV* agmv, u32 x, u32 y);
AGMV_BLOCK_COUNT AGMV_GetPMVBlockCount(AGMV* agmv, u32 x, u32 y);
AGMV_BLOCK_COUNT AGMV_GetMVBlockCount(AGMV* agmv, u32 x, u32 y);
u8 AGMV_GetPMVSBlockCount(AGMV* agmv, AGMV_MV mv, u32 x, u32 y, int block_w, int block_h);
u8 AGMV_GetMVSBlockCount(AGMV* agmv, AGMV_MV mv, u32 x, u32 y, int block_w, int block_h);
AGMV_CRATE AGMV_GetVQBlockCount(AGMV* agmv, u32 x, u32 y);
AGMV_BLOCK_COUNT AGMV_GetIntraCopyBlockCount(AGMV* agmv, u32 x, u32 y);
AGMV_BLOCK_COUNT AGMV_GetRightIntraCopyBlockCount(AGMV* agmv, u32 x, u32 y);
AGMV_SUMMATION AGMV_MAD(AGMV* agmv, int i, int j, int x, int y);
AGMV_SUMMATION AGMV_UniversalMAD(AGMV* agmv, int i, int j, int x, int y, u8 block_w, u8 block_h);
void AGMV_ThreeStepSearch(AGMV* agmv, int x, int y);
void AGMV_ThreeStepSearch2x2(AGMV* agmv, AGMV_SMV* smv1, AGMV_SMV* smv2, AGMV_SMV* smv3, AGMV_SMV* smv4, int x, int y);
void AGMV_ThreeStepSearch4x4(AGMV* agmv, AGMV_SMV* smv1, AGMV_SMV* smv2, int x, int y);
void AGMV_EncodeHeader(FILE* file, AGMV* agmv);
void AGMV_EncodeFrame(FILE* file, AGMV* agmv, u32* img_data);
void AGMV_AssembleIFrameBitstream(AGMV* agmv, AGMV_ENTRY* img_entry);
void AGMV_AssemblePFrameBitstream(AGMV* agmv, AGMV_ENTRY* img_entry);
u32 AGMV_LZSS(FILE* file, AGMV_BITSTREAM* in);
void AGMV_CompressAudioBuffer(AGMV* agmv);
void AGMV_FillAudioBuffer(FILE* file, AGMV* agmv, AGMV_AUDIO_TYPE audio_type);
void AGMV_EncodeAudioChunk(FILE* file, AGMV* agmv);
void AGMV_LoadHistogramImageData(const char* dir, const char* basename, char* ext, u32 start_frame, u32 end_frame, u32 size, u32* histogram, u32* colorgram, u8 img_type, AGMV_QUALITY quality);
void AGMV_DistributePaletteData(u32* colorgram, u32* pal, u32* palette0, u32* palette1, u32 max_clr, AGMV_OPT opt, AGMV_QUALITY quality);
void AGMV_EncodeVideo(const char* filename, const char* dir, const char* basename, u8 img_type, u32 start_frame, u32 end_frame, u32 width, u32 height, u32 frames_per_second, AGMV_OPT opt, AGMV_QUALITY quality, AGMV_COMPRESSION compression, AGMV_CUSTOMIZATION* customization);
void AGMV_EncodeAGMV(AGMV* agmv, const char* filename, const char* dir, const char* basename, u8 img_type, u32 start_frame, u32 end_frame, u32 width, u32 height, u32 frames_per_second, AGMV_OPT opt, AGMV_QUALITY quality, AGMV_COMPRESSION compression);
void AGMV_EncodeFullAGMV(AGMV* agmv, const char* filename, const char* dir, const char* basename, u8 img_type, u32 start_frame, u32 end_frame, u32 width, u32 height, u32 frames_per_second, AGMV_OPT opt, AGMV_QUALITY quality, AGMV_COMPRESSION compression);

#endif