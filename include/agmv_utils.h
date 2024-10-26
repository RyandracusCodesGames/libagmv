#ifndef AGMV_UTILS_H
#define AGMV_UTILS_H

/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_utils.h
*   Date: 5/17/2024
*   Version: 2.0
*   Updated: 10/25/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agmv_defines.h>

/*-------FILE READING UTILITY FUNCTIONS------*/

Bool AGMV_EOF(FILE* file);

u8 AGMV_ReadByte(FILE* file);
u16 AGMV_ReadShort(FILE* file);
u32 AGMV_ReadLong(FILE* file);
void AGMV_ReadFourCC(FILE* file, char fourcc[4]);
void AGMV_ReadNBytes(FILE* file, u8* buf, u32 n);
void AGMV_ReadNShorts(FILE* file, u16* buf, u32 n);
void AGMV_ReadSwappedNShorts(FILE* file, u16* buf, u32 n);

void AGMV_WriteByte(FILE* file, u8 byte);
void AGMV_WriteShort(FILE* file, u16 word);
void AGMV_WriteLong(FILE* file, u32 dword);
void AGMV_WriteFourCC(FILE* file, char f, char o, char u, char r);
void AGMV_WriteNBytes(FILE* file, u8* buf, u32 n);

void AGMV_FindNextFrameChunk(FILE* file);
void AGMV_FindNextAudioChunk(FILE* file);
void AGMV_FindNextAudsChunk(FILE* file);
void AGMV_FindNextStrfChunk(FILE* file);
void AGMV_FindNextFourCCChunk(FILE* file, char f, char o, char u, char r);

u16 AGMV_SkipFrameChunk(FILE* file);
void AGMV_SkipAudioChunk(FILE* file);

void AGMV_ParseAGMV(FILE* file, AGMV* agmv);

Bool AGMV_IsCorrectFourCC(char fourcc[4], char f, char o, char u, char r);
Bool AGMV_IsValidFlag(u8 byte);
Bool AGMV_IsNextFrameAnIFrame(FILE* file);

/*------PRIMARY FUNCTIONS TO INITIALIZE AGMV ATTRIBUTES----------*/

void AGMV_SetWidth(AGMV* agmv, u32 width);
void AGMV_SetHeight(AGMV* agmv, u32 height);
void AGMV_SetICP0(AGMV* agmv, u32 palette0[256]);
void AGMV_SetICP1(AGMV* agmv, u32 palette1[256]);
void AGMV_SetFramesPerSecond(AGMV* agmv, u32 frames_per_second);
void AGMV_SetNumberOfFrames(AGMV* agmv, u32 num_of_frames);
void AGMV_SetTotalAudioDuration(AGMV* agmv, u32 total_audio_duration);
void AGMV_SetSampleRate(AGMV* agmv, u32 sample_rate);
void AGMV_SetNumberOfChannels(AGMV* agmv, u8 num_of_channels);
void AGMV_SetAudioSize(AGMV* agmv, u32 size);
void AGMV_SetLeniency(AGMV* agmv, f32 leniency);
void AGMV_SetOPT(AGMV* agmv, AGMV_OPT opt);
void AGMV_SetVersion(AGMV* agmv, u8 version);
void AGMV_SetCompression(AGMV* agmv, AGMV_COMPRESSION compression);
void AGMV_SetQuality(AGMV* agmv, AGMV_QUALITY quality);
void AGMV_SetVolume(AGMV* agmv, f32 volume);
void AGMV_SetBitsPerSample(AGMV* agmv, u16 bits_per_sample);
void AGMV_SetError(AGMV* agmv, Error error);
AGMV* CreateAGMV(u32 num_of_frames, u32 width, u32 height, u32 frames_per_second);
void DestroyAGMV(AGMV* agmv);
void AGMV_EnablePrintf();
void AGMV_DisablePrintf();
/*------PRIMARY FUNCTIONS TO RETRIEVE AGMV ATTRIBUTES----------*/

u32 AGMV_GetWidth(AGMV* agmv);
u32 AGMV_GetHeight(AGMV* agmv);
u32 AGMV_GetFramesPerSecond(AGMV* agmv);
u32 AGMV_GetNumberOfFrames(AGMV* agmv);
u32 AGMV_GetNumberOfIFrames(AGMV* agmv);
u32 AGMV_GetTotalAudioDuration(AGMV* agmv);
u32 AGMV_GetSampleRate(AGMV* agmv);
u16 AGMV_GetNumberOfChannels(AGMV* agmv);
u32 AGMV_GetAudioSize(AGMV* agmv);
f32 AGMV_GetLeniency(AGMV* agmv);
u8 AGMV_GetVersion(AGMV* agmv);
AGMV_OPT AGMV_GetOPT(AGMV* agmv);
AGMV_COMPRESSION AGMV_GetCompression(AGMV* agmv);
AGMV_QUALITY AGMV_GetQuality(AGMV* agmv);
f32 AGMV_GetVolume(AGMV* agmv);
u16 AGMV_GetBitsPerSample(AGMV* agmv);
Error AGMV_GetError(AGMV* agmv);

/*-----------------VARIOUS UTILITY FUNCTIONS-----------------*/
void AGMV_Memcpy32(u32* dest, u32* src, u32 n);
void AGMV_Memset32(u32* dest, u32 num, u32 n);
void AGMV_Memcpy16(u16* dest, u16* src, u32 n);
void AGMV_Memset16(u16* dest, u16 num, u32 n);
void AGMV_Memcpy8(u8* dest, u8* src, u32 n);
void AGMV_Memset8(u8* dest, u8 num, u32 n);
u8 AGMV_GetMSB(u16 color);
u8 AGMV_GetLSB(u16 color);
void AGMV_Printf(char* fmt, ...);
u8 AGMV_SetBitstreamFlag(AGMV_BLOCK_SIZE block_size, u8 flag);
u8 AGMV_GetCount(AGMV* agmv, AGMV_BLOCK_SIZE block_size, u8 flag, AGMV_QUALITY quality, AGMV_OPT opt);
u8 AGMV_GetVersionFromOPT(AGMV_OPT opt, AGMV_COMPRESSION compression);
u8 AGMV_GetBlockWidth(AGMV_BLOCK_SIZE block);
u8 AGMV_GetBlockHeight(AGMV_BLOCK_SIZE block);
void AGMV_DitherImageData(u32* img_data, u32 width, u32 height);
u32 AGMV_GetMicrosecondsPerFrame(u32 frame_rate);
int AGMV_Clamp(int min, int value, int max);
f32 AGMV_ClampVolume(f32 volume);
u16 AGMV_SwapShort(u16 word);
u32 AGMV_SwapLong(u32 dword);
void AGMV_CopyImageData(u32* dest, u32* src, u32 size);
void AGMV_SyncFrameAndImage(AGMV* agmv, u32* img_data);
void AGMV_SyncAudioTrack(AGMV* agmv, const void* pcm);
void AGMV_SignedToUnsignedPCM(u8* pcm, u32 size);
void AGMV_UnsignedToSignedPCM(u8* pcm, u32 size);

static inline int AGMV_Abs(int a){
	if(a < 0){
		return -a;
	}
	else return a;
}

int AGMV_Min(int a, int b);

static inline u8 AGMV_GetR(u32 color){
	return (color & 0xff0000) >> 16;
}

static inline u8 AGMV_GetG(u32 color){
	return (color & 0xff00) >> 8;
}

static inline u8 AGMV_GetB(u32 color){
	return (color & 0xff);
}

u8 AGMV_GetQuantizedR(u32 color, AGMV_QUALITY quality);
u8 AGMV_GetQuantizedG(u32 color, AGMV_QUALITY quality);
u8 AGMV_GetQuantizedB(u32 color, AGMV_QUALITY quality);
u32 AGMV_QuantizeColor(u32 color, AGMV_QUALITY quality);
u32 AGMV_ReverseQuantizeColor(u32 color, AGMV_QUALITY quality);
u32 AGMV_InterpColor(u32 color1, u32 color2);
u8 AGMV_FindNearestColor(u32 palette[256], u32 color);
u8 AGMV_FindSmallestColor(u32 palette[256], u32 color);
u8 AGMV_FindColorFromRange(u32 palette[256], u32 start, u32 end, u32 color);
AGMV_ENTRY AGMV_FindNearestEntry(u32 palette0[256], u32 palette1[256], u32 color);
AGMV_ENTRY AGMV_FindSmallestEntry(u32 palette0[256], u32 palette1[256], u32 color);
AGMV_ENTRY AGMV_FindEntryFromRange(u32 palette0[256], u32 palette1[256], u32 start, u32 end, u32 color);
u32 AGMV_CalculateTotalAudioDuration(u32 size, u32 sample_rate, u16 num_of_channels, u16 bits_per_sample);
f32 AGMV_CompareFrameSimilarity(u32* frame1, u32* frame2 , u32 width, u32 height);
void AGMV_InterpFrame(u32* interp, u32* frame1, u32* frame2, u32 width, u32 height);
void AGMV_ConstructFrameFromEntries(AGMV* agmv);
void AGMV_BubbleSort(u32* data, u32* gram, u32 num_of_colors);
char* AGMV_Error2Str(Error error);
u32 AGMV_GetNumberOfBytesRead(u32 bits);
void AGMV_WavToAudioTrack(const char* filename, AGMV* agmv);
void AGMV_RawSignedPCMToAudioTrack(const char* filename, AGMV* agmv, u8 num_of_channels, u32 sample_rate);
u32 AGMV_80BitFloat(FILE* file);
void AGMV_AIFCToAudioTrack(const char* filename, AGMV* agmv);
void AGMV_AIFFToAudioTrack(const char* filename, AGMV* agmv);
void AGMV_Raw8PCMToAudioTrack(const char* filename, AGMV* agmv);
AGMV_INFO AGMV_GetVideoInfo(AGMV* agmv);
int AGMV_ResetFrameRate(const char* filename, u32 frames_per_second);
void AGMV_ExportAudioType(FILE* audio, AGMV* agmv, AGMV_AUDIO_TYPE audio_type);
void AGMV_ExportAudio(FILE* audio, AGMV* agmv, AGMV_AUDIO_TYPE audio_type);
void AGMV_ExportAGMVToHeader(const char* filename);

 #endif