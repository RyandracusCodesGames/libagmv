/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_utils.c
*   Date: 5/17/2024
*   Version: 1.0
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <stdlib.h>
#include <agmv_utils.h>
#include <agmv_decode.h>

/*-------FILE READING UTILITY FUNCTIONS------*/

Bool AGMV_EOF(FILE* file){
	u32 pos = ftell(file), file_size;
	fseek(file,0,SEEK_END);
	file_size = ftell(file);
	fseek(file,pos,SEEK_SET);
	
	if(pos >= file_size){
		return TRUE;
	}
	else return FALSE;
}

u32 matchlength = 0,
    matchpos = 0,
    bitbuf = 0,
    bitsin = 0,
    masks[17] = {0,1,3,7,15,31,63,127,255,511,1023,2047,4095,8191,16383,32767,65535};

u32 AGMV_ReadBits(FILE* file, u32 num_of_bits){
	
	u32 i;

	i = bitbuf >> (8 - bitsin);

	while (num_of_bits > bitsin)
	{
			bitbuf = AGMV_ReadByte(file);
			i |= (bitbuf << bitsin);
			bitsin += 8;
	}

	bitsin -= num_of_bits;

	return (i & masks[num_of_bits]);
}


void AGMV_FlushReadBits() {
    bitbuf = 0;
    bitsin = 0;
}

u8 AGMV_ReadByte(FILE* file){
	u8 byte = 0;
	fread(&byte,1,1,file);
	return byte;
}

u16 AGMV_ReadShort(FILE* file){
	u16 word = 0;
	fread(&word,1,2,file);
	return word;
}

u32 AGMV_ReadLong(FILE* file){
	u32 dword = 0;
	fread(&dword,1,4,file);
	return dword;
}

void AGMV_ReadFourCC(FILE* file, char fourcc[4]){
	fourcc[0] = AGMV_ReadByte(file);
	fourcc[1] = AGMV_ReadByte(file);
	fourcc[2] = AGMV_ReadByte(file);
	fourcc[3] = AGMV_ReadByte(file);
}

void AGMV_WriteBits(FILE* file, u32 num, u16 num_of_bits){
	bitbuf |= (num << bitsin);

	bitsin += num_of_bits;

	if (bitsin > 16) 
	{
			AGMV_WriteByte(file,bitbuf & 0xFF);
			bitbuf = num >> (8 - (bitsin - num_of_bits));
			bitsin -= 8;
	}

	while (bitsin >= 8)
	{
			AGMV_WriteByte(file,bitbuf & 0xFF);
			bitbuf >>= 8;
			bitsin -= 8;
	}
}

void AGMV_FlushWriteBits(){
	AGMV_FlushReadBits();
}

void AGMV_WriteByte(FILE* file, u8 byte){
	fwrite(&byte,1,1,file);
}

void AGMV_WriteShort(FILE* file, u16 word){
	fwrite(&word,2,1,file);
}

void AGMV_WriteLong(FILE* file, u32 dword){
	fwrite(&dword,4,1,file);
}

void AGMV_WriteFourCC(FILE* file, char f, char o, char u, char r){
	AGMV_WriteByte(file,f);
	AGMV_WriteByte(file,o);
	AGMV_WriteByte(file,u);
	AGMV_WriteByte(file,r);
}

Bool AGMV_IsCorrectFourCC(char fourcc[4], char f, char o, char u, char r){
	if(f != fourcc[0] || o != fourcc[1] || u != fourcc[2] || r != fourcc[3]){
		return FALSE;
	}
	else return TRUE;
}

void AGMV_FindNextFrameChunk(FILE* file){
	u32 pos;
	Bool isFrame;
	char fourcc[4];

	AGMV_ReadFourCC(file,fourcc);
	isFrame = AGMV_IsCorrectFourCC(fourcc,'A','G','F','C');

	if(isFrame){
		pos = ftell(file);
		fseek(file,pos-4,SEEK_SET);
		return;
	}

	while(!isFrame && !AGMV_EOF(file)){
		AGMV_ReadFourCC(file,fourcc);
		isFrame = AGMV_IsCorrectFourCC(fourcc,'A','G','F','C');
		
		if(!isFrame){
			pos = ftell(file);
			fseek(file,pos-3,SEEK_SET);
		}
	}

	pos = ftell(file);
	fseek(file,pos-4,SEEK_SET);
}

void AGMV_FindNextAudioChunk(FILE* file){
	u32 pos;
	Bool isAudio;
	char fourcc[4];

	AGMV_ReadFourCC(file,fourcc);
	isAudio = AGMV_IsCorrectFourCC(fourcc,'A','G','A','C');

	if(isAudio){
		pos = ftell(file);
		fseek(file,pos-4,SEEK_SET);
		return;
	}

	while(!isAudio && !AGMV_EOF(file)){
		AGMV_ReadFourCC(file,fourcc);
		isAudio = AGMV_IsCorrectFourCC(fourcc,'A','G','A','C');
		
		if(!isAudio){
			pos = ftell(file);
			fseek(file,pos-3,SEEK_SET);
		}
	}

	pos = ftell(file);
	fseek(file,pos-4,SEEK_SET);
}

void AGMV_SkipFrameChunk(FILE* file){
	u32 fourcc, frame_num, usize, csize;

	AGMV_FindNextFrameChunk(file);

	fourcc    = AGMV_ReadLong(file);
	frame_num = AGMV_ReadLong(file);
	usize     = AGMV_ReadLong(file);
	csize     = AGMV_ReadLong(file);

	fseek(file,csize,SEEK_CUR);
}

void AGMV_SkipAudioChunk(FILE* file){
	u32 fourcc, size;

	AGMV_FindNextAudioChunk(file);

	fourcc = AGMV_ReadLong(file);
	size   = AGMV_ReadLong(file);

	fseek(file,size,SEEK_CUR);
}

void AGMV_ParseAGMV(FILE* file, AGMV* agmv){
	u32 n = AGMV_GetNumberOfFrames(agmv);
	
	if(AGMV_GetTotalAudioDuration(agmv) != 0){
		int i;
		for(i = 0; i < n; i++){
			AGMV_FindNextFrameChunk(file);
			agmv->offset_table[agmv->frame_count++] = ftell(file);
			AGMV_SkipFrameChunk(file);
			AGMV_FindNextAudioChunk(file);
			AGMV_DecodeAudioChunk(file,agmv);
		}
	}
	else{
		int i;
		for(i = 0; i < n; i++){
			AGMV_FindNextFrameChunk(file);
			agmv->offset_table[agmv->frame_count++] = ftell(file);
			AGMV_SkipFrameChunk(file);
		}
	}
}

/*------PRIMARY FUNCTIONS TO INITIALIZE AGMV ATTRIBUTES----------*/

void AGMV_SetWidth(AGMV* agmv, u32 width){
	agmv->header.width = width;
	agmv->frame->width = width;
	agmv->iframe->width = width;
}

void AGMV_SetHeight(AGMV* agmv, u32 height){
	agmv->header.height = height;
	agmv->frame->height = height;
	agmv->iframe->height = height;
}

void AGMV_SetICP0(AGMV* agmv, u32 palette0[256]){
	int i;
	for(i = 0; i < 256; i++){
		agmv->header.palette0[i] = palette0[i];
	}
}

void AGMV_SetICP1(AGMV* agmv, u32 palette1[256]){
	int i;
	for(i = 0; i < 256; i++){
		agmv->header.palette1[i] = palette1[i];
	}
}

void AGMV_SetFramesPerSecond(AGMV* agmv, u32 frames_per_second){
	agmv->header.frames_per_second = frames_per_second;
}

void AGMV_SetNumberOfFrames(AGMV* agmv, u32 num_of_frames){
	agmv->header.num_of_frames = num_of_frames;
}

void AGMV_SetTotalAudioDuration(AGMV* agmv, u32 total_audio_duration){
	agmv->header.total_audio_duration = total_audio_duration;
}

void AGMV_SetSampleRate(AGMV* agmv, u32 sample_rate){
	agmv->header.sample_rate = sample_rate;
}

void AGMV_SetNumberOfChannels(AGMV* agmv, u8 num_of_channels){
	agmv->header.num_of_channels = num_of_channels;
}

void AGMV_SetAudioSize(AGMV* agmv, u32 size){
	agmv->header.audio_size = size;
}

void AGMV_SetLeniency(AGMV* agmv, f32 leniency){
	agmv->leniency = leniency;
}

void AGMV_SetOPT(AGMV* agmv, AGMV_OPT opt){
	agmv->opt = opt;
}

void AGMV_SetVersion(AGMV* agmv, u8 version){
	agmv->header.version = version;
}

void AGMV_SetCompression(AGMV* agmv, AGMV_COMPRESSION compression){
	agmv->compression = compression;
}

void AGMV_SetAudioState(AGMV* agmv, Bool audio){
	agmv->enable_audio = audio;
}

void AGMV_SetVolume(AGMV* agmv, f32 volume){
	if(volume < 0.0f){
		volume = 0.0f;
	}
	if(volume > 1.0f){
		volume = 1.0f;
	}
	
	agmv->volume = volume;
}

AGMV* CreateAGMV(u32 num_of_frames, u32 width, u32 height, u32 frames_per_second){
	AGMV* agmv = (AGMV*)malloc(sizeof(AGMV));

	agmv->frame_chunk = (AGMV_FRAME_CHUNK*)malloc(sizeof(AGMV_FRAME_CHUNK));
	agmv->audio_chunk = (AGMV_AUDIO_CHUNK*)malloc(sizeof(AGMV_AUDIO_CHUNK));
	agmv->bitstream = (AGMV_BITSTREAM*)malloc(sizeof(AGMV_BITSTREAM));
	agmv->bitstream->len = width*height*2;
	agmv->bitstream->pos = 0;
	agmv->bitstream->data = (u8*)malloc(sizeof(u8)*agmv->bitstream->len);
	agmv->frame = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->frame->img_data = (u32*)malloc(sizeof(u32)*width*height);
	agmv->iframe = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->iframe->img_data = (u32*)malloc(sizeof(u32)*width*height);
	agmv->audio_track = (AGMV_AUDIO_TRACK*)malloc(sizeof(AGMV_AUDIO_TRACK));
	agmv->iframe_entries = (AGMV_ENTRY*)malloc(sizeof(AGMV_ENTRY)*width*height);

	agmv->frame_count = 0;
	agmv->audio_track->start_point = 0;
	
	AGMV_SetWidth(agmv,width);
	AGMV_SetHeight(agmv,height);
	AGMV_SetNumberOfFrames(agmv,num_of_frames);
	AGMV_SetFramesPerSecond(agmv,frames_per_second);
	AGMV_SetTotalAudioDuration(agmv,0);
	AGMV_SetSampleRate(agmv,0);
	AGMV_SetAudioSize(agmv,0);
	AGMV_SetNumberOfChannels(agmv,0);
	AGMV_SetLeniency(agmv,0.1282f);
	AGMV_SetOPT(agmv,AGMV_OPT_I);
	AGMV_SetCompression(agmv,AGMV_LZSS_COMPRESSION);
	AGMV_SetVolume(agmv,1.0f);

	return agmv;
}

void DestroyAGMV(AGMV* agmv){
	if(agmv != NULL){
		if(agmv->iframe_entries != NULL){
			free(agmv->iframe_entries);
			agmv->iframe_entries = NULL;
		}
		
		if(agmv->frame->img_data != NULL){
			free(agmv->frame->img_data);
			agmv->frame->img_data = NULL;
		}
		
		if(agmv->iframe->img_data != NULL){
			free(agmv->iframe->img_data);
			agmv->iframe->img_data = NULL;
		}

		free(agmv->frame);
		free(agmv->iframe);
		
		if(agmv->bitstream->data != NULL){
			free(agmv->bitstream->data);
		}

		free(agmv->bitstream);
		free(agmv->frame_chunk);
		free(agmv->audio_chunk);
		
		if(agmv->audio_track->pcm != NULL){
			free(agmv->audio_track->pcm);
		}
		
		if(agmv->audio_chunk->atsample != NULL){
			free(agmv->audio_chunk->atsample);
		}

		free(agmv->audio_track);

		agmv = NULL;
	}
}

/*------PRIMARY FUNCTIONS TO RETRIEVE AGMV ATTRIBUTES----------*/

u32 AGMV_GetWidth(AGMV* agmv){
	return agmv->header.width;
}

u32 AGMV_GetHeight(AGMV* agmv){
	return agmv->header.height;
}

u32 AGMV_GetFramesPerSecond(AGMV* agmv){
	return agmv->header.frames_per_second;
}

u32 AGMV_GetNumberOfFrames(AGMV* agmv){
	return agmv->header.num_of_frames;
}

u32 AGMV_GetTotalAudioDuration(AGMV* agmv){
	return agmv->header.total_audio_duration;
}

u32 AGMV_GetSampleRate(AGMV* agmv){
	return agmv->header.sample_rate;
}

u16 AGMV_GetNumberOfChannels(AGMV* agmv){
	return agmv->header.num_of_channels;
}

u32 AGMV_GetAudioSize(AGMV* agmv){
	return agmv->header.audio_size;
}

f32 AGMV_GetLeniency(AGMV* agmv){
	return agmv->leniency;
}

u8 AGMV_GetVersion(AGMV* agmv){
	return agmv->header.version;
}

AGMV_OPT AGMV_GetOPT(AGMV* agmv){
	return agmv->opt;
}

AGMV_COMPRESSION AGMV_GetCompression(AGMV* agmv){
	return agmv->compression;
}

Bool AGMV_GetAudioState(AGMV* agmv){
	return agmv->enable_audio;
}

f32 AGMV_GetVolume(AGMV* agmv){
	return agmv->volume;
}

/*-----------------VARIOUS UTILITY FUNCTIONS-----------------*/

u8 AGMV_GetVersionFromOPT(AGMV_OPT opt, AGMV_COMPRESSION compression){
	if(compression == AGMV_LZSS_COMPRESSION){
		switch(opt){
			case AGMV_OPT_I:{
				return 1;
			}
			case AGMV_OPT_II:{
				return 2;
			}
			case AGMV_OPT_III:{
				return 1;
			}
			case AGMV_OPT_ANIM:{
				return 2;
			}
			case AGMV_OPT_GBA_I:{
				return 1;
			}
			case AGMV_OPT_GBA_II:{
				return 2;
			}
			case AGMV_OPT_GBA_III:{
				return 1;
			}
			case AGMV_OPT_NDS:{
				return 1;
			}
		}
	}
	else{
		switch(opt){
			case AGMV_OPT_I:{
				return 3;
			}
			case AGMV_OPT_II:{
				return 4;
			}
			case AGMV_OPT_III:{
				return 3;
			}
			case AGMV_OPT_ANIM:{
				return 4;
			}
			case AGMV_OPT_GBA_I:{
				return 3;
			}
			case AGMV_OPT_GBA_II:{
				return 4;
			}
			case AGMV_OPT_GBA_III:{
				return 3;
			}
			case AGMV_OPT_NDS:{
				return 3;
			}
		}
	}
	return 1;
}

f32 AGMV_ClampVolume(f32 volume){
	if(volume < 0.0f){
		volume = 0.0f;
	}
	if(volume > 1.0f){
		volume = 1.0f;
	}
	return volume;
}

u16 AGMV_SwapShort(u16 word){
	u8 msb = (word & 0xff00) >> 8;
	u8 lsb = (word & 0xff);
	
	return lsb << 8 | msb;
}

u32 AGMV_SwapLong(u32 dword){
	u8 msb = (dword >> 24) & 0xff;
	u8 msb2 = (dword >> 16) & 0xff;
	u8 lsb2 = (dword >> 8)  & 0xff;
	u8 lsb = (dword & 0xff);
	
	return lsb << 24 | lsb2 << 16 | msb2 << 8 | msb;
}

void AGMV_CopyImageData(u32* dest, u32* src, u32 size){
	int i;
	for(i = 0; i < size; i++){
		dest[i] = src[i];
	}
}

void AGMV_SyncFrameAndImage(AGMV* agmv, u32* img_data){
	AGMV_CopyImageData(agmv->frame->img_data,img_data,AGMV_GetWidth(agmv)*AGMV_GetHeight(agmv));
}

void AGMV_SyncAudioTrack(AGMV* agmv, u16* pcm){
	int i;
	for(i = 0; i < AGMV_GetAudioSize(agmv); i++){
		agmv->audio_track->pcm[i] = pcm[i];
	}
}

int AGMV_Abs(int a){
	if(a < 0){
		return -a;
	}
	else return a;
}

int AGMV_Min(int a, int b){
	if(a < b){
		return a;
	}
	else return b;
}

u8 AGMV_GetR(u32 color){
	return (color & 0xff0000) >> 16;
}

u8 AGMV_GetG(u32 color){
	return (color & 0xff00) >> 8;
}

u8 AGMV_GetB(u32 color){
	return (color & 0xff);
}

u8 AGMV_GetQuantizedR(u32 color, AGMV_QUALITY quality){
	switch(quality){
		case AGMV_HIGH_QUALITY:{
			return(color & 0x7e000) >> 13;
		}
		case AGMV_MID_QUALITY:{
			return (color & 0x1f000) >> 12;
		}
		case AGMV_LOW_QUALITY:{
			return (color & 0xf800) >> 11;
		}
		default:{
			return(color & 0x7e000) >> 13;
		}
	}
}

u8 AGMV_GetQuantizedG(u32 color, AGMV_QUALITY quality){
	switch(quality){
		case AGMV_HIGH_QUALITY:{
			return (color & 0x1f80) >> 7;
		}
		case AGMV_MID_QUALITY:{
			return (color & 0xfc0) >> 6;
		}
		case AGMV_LOW_QUALITY:{
			return (color & 0x7e0) >> 5;
		}
		default:{
			return(color & 0x1f80) >> 7;
		}
	}
}

u8 AGMV_GetQuantizedB(u32 color, AGMV_QUALITY quality){
	switch(quality){
		case AGMV_HIGH_QUALITY:{
			return (color & 0x7f);
		}
		case AGMV_MID_QUALITY:{
			return (color & 0x3f);
		}
		case AGMV_LOW_QUALITY:{
			return (color & 0x001F);
		}
		default:{
			return (color & 0x7f);
		}
	}
}

u32 AGMV_QuantizeColor(u32 color, AGMV_QUALITY quality){
	switch(quality){
		case AGMV_HIGH_QUALITY:{
			u8 r = AGMV_GetR(color);
			u8 g = AGMV_GetG(color);
			u8 b = AGMV_GetB(color);
			
			r >>= 2;
			g >>= 2;
			b >>= 1;
			
			return r << 13 | g << 7 | b;
		}
		case AGMV_MID_QUALITY:{
			u8 r = AGMV_GetR(color);
			u8 g = AGMV_GetG(color);
			u8 b = AGMV_GetB(color);
			
			r >>= 3;
			g >>= 2;
			b >>= 2;
			
			return r << 12 | g << 6 | b;
		}
		case AGMV_LOW_QUALITY:{
			u8 r = AGMV_GetR(color);
			u8 g = AGMV_GetG(color);
			u8 b = AGMV_GetB(color);
			
			r >>= 3;
			g >>= 2;
			b >>= 3;
			
			return r << 11 | g << 5 | b;
		}
		default:{
			u8 r = AGMV_GetR(color);
			u8 g = AGMV_GetG(color);
			u8 b = AGMV_GetB(color);
			
			r >>= 2;
			g >>= 2;
			b >>= 1;
			
			return r << 13 | g << 7 | b;
		}
	}
}

u32 AGMV_ReverseQuantizeColor(u32 color, AGMV_QUALITY quality){
	int r = AGMV_GetQuantizedR(color,quality);
	int g = AGMV_GetQuantizedG(color,quality);
	int b = AGMV_GetQuantizedB(color,quality);
	
	switch(quality){
		case AGMV_HIGH_QUALITY:{
			
			r <<= 2;
			g <<= 2;
			b <<= 1;
			
			return r << 16 | g << 8 | b;
		}
		case AGMV_MID_QUALITY:{
			
			r <<= 3;
			g <<= 2;
			b <<= 2;
			
			return r << 16 | g << 8 | b;
		}
		case AGMV_LOW_QUALITY:{
			
			r <<= 3;
			g <<= 2;
			b <<= 3;
			
			return r << 16 | g << 8 | b;
		}
		default:{
			
			r <<= 2;
			g <<= 2;
			b <<= 1;
			
			return r << 16 | g << 8 | b;
		}
	}
}

u8 AGMV_FindNearestColor(u32 palette[256], u32 color){
	int i, rdiff, gdiff, bdiff, r, g, b;
	u32 min, dist;
	u8 pr, pg, pb, index;

	r = AGMV_GetR(color);
	g = AGMV_GetG(color);
	b = AGMV_GetB(color);
	
	min = 255*255 + 255*255 + 255*255 + 1;
	index = 0;

	for(i = 0; i < 256; i++){
		u32 palclr = palette[i];
		
		pr = AGMV_GetR(palclr);
		pg = AGMV_GetG(palclr);
	    pb = AGMV_GetB(palclr);
		
		rdiff = r-pr;
		gdiff = g-pg;
		bdiff = b-pb;
		
		dist = rdiff*rdiff + gdiff*gdiff + bdiff*bdiff;
		
		if(dist < min){
			min = dist;
			index = i;
		}
	}
	return index;
}

u8 AGMV_FindSmallestColor(u32 palette[256], u32 color){
	int i, rdiff, gdiff, bdiff, r, g, b;
	u32 min, dist;
	u8 pr, pg, pb, index;

	r = AGMV_GetR(color);
	g = AGMV_GetG(color);
	b = AGMV_GetB(color);
	
	min = 255*255 + 255*255 + 255*255 + 1;
	index = 0;

	for(i = 0; i < 200; i++){
		u32 palclr = palette[i];
		
		pr = AGMV_GetR(palclr);
		pg = AGMV_GetG(palclr);
	    pb = AGMV_GetB(palclr);
		
		rdiff = r-pr;
		gdiff = g-pg;
		bdiff = b-pb;
		
		dist = rdiff*rdiff + gdiff*gdiff + bdiff*bdiff;
		
		if(dist < min){
			min = dist;
			index = i;
		}
	}
	return index;
}

AGMV_ENTRY AGMV_FindNearestEntry(u32 palette0[256], u32 palette1[256], u32 color){
	AGMV_ENTRY entry;
	u32 color1, color2, dist1, dist2;
	int index1, index2, r, g, b, r1, g1, b1, r2, g2, b2;
	int rdiff1, gdiff1, bdiff1, rdiff2, gdiff2, bdiff2;

	index1 = AGMV_FindNearestColor(palette0,color);
	index2 = AGMV_FindNearestColor(palette1,color);

	color1 = palette0[index1]; color2 = palette1[index2];

	r = AGMV_GetR(color);
	g = AGMV_GetG(color);
	b = AGMV_GetB(color);

	r1 = AGMV_GetR(color1);
	g1 = AGMV_GetG(color1);
	b1 = AGMV_GetB(color1);

	r2 = AGMV_GetR(color2);
	g2 = AGMV_GetG(color2);
	b2 = AGMV_GetB(color2);

	rdiff1 = r - r1;
	gdiff1 = g - g1;
	bdiff1 = b - b1;

	rdiff2 = r - r2;
	gdiff2 = g - g2;
	bdiff2 = b - b2;

	dist1 = rdiff1*rdiff1 + gdiff1*gdiff1 + bdiff1*bdiff1;
	dist2 = rdiff2*rdiff2 + gdiff2*gdiff2 + bdiff2*bdiff2;

	if(dist1 <= dist2){
		entry.index = index1;
		entry.pal_num = 0;
	}
	else{
		entry.index = index2;
		entry.pal_num = 1;
	}

	return entry;
}

AGMV_ENTRY AGMV_FindSmallestEntry(u32 palette0[256], u32 palette1[256], u32 color){
	AGMV_ENTRY entry;
	u8 index1, index2;

	index1 = AGMV_FindSmallestColor(palette0,color);
	index2 = AGMV_FindSmallestColor(palette1,color);

	if(index1 <= index2){
		entry.index = index1;
		entry.pal_num = 0;
	}
	else{
		entry.index = index2;
		entry.pal_num = 1;
	}

	return entry;
}

u32 AGMV_CalculateTotalAudioDuration(u32 size, u32 sample_rate, u16 num_of_channels, u16 bits_per_sample){
	return (u32)(size/(f32)sample_rate*num_of_channels*(bits_per_sample/8));
}

u32 AGMV_GrayscaleColor(u32 clr){
	u8 r = AGMV_GetR(clr);
	u8 g = AGMV_GetG(clr);
	u8 b = AGMV_GetB(clr);
	
	u8 rgb = (u8)((r+g+b)/3.0f);

	return (rgb << 16 | rgb << 8 | rgb);
}

f32 AGMV_CompareFrameSimilarity(u32* frame1, u32* frame2, u32 width, u32 height){
	u32 total_num_of_pixels = width*height, count = 0;
	
	int i;
	for(i = 0; i < width*height; i++){
		u32 color1 = frame1[i];
		u32 color2 = frame2[i];
		
		int gray1 = AGMV_GrayscaleColor(color1);
		int gray2 = AGMV_GrayscaleColor(color2);
		
		if(gray1-gray2 == 0){
			count++;
		}
	}
	
	return count/(f32)total_num_of_pixels;
}

void AGMV_InterpFrame(u32* interp, u32* frame1, u32* frame2, u32 width, u32 height){
	int i;
	for(i = 0; i < width*height; i++){
		u32 color1 = frame1[i];
		u32 color2 = frame2[i];
		
		int r1 = AGMV_GetR(color1);
		int g1 = AGMV_GetG(color1);
		int b1 = AGMV_GetB(color1);
		
		int r2 = AGMV_GetR(color2);
		int g2 = AGMV_GetG(color2);
		int b2 = AGMV_GetB(color2);
		
		int r = r1 + ((r2-r1) >> 1);
		int g = g1 + ((g2-g1) >> 1);
		int b = b1 + ((b2-b1) >> 1);
		
		interp[i] = r << 16 | g << 8 | b;
	}
}

int AGMV_NextIFrame(int n, int frame_count){
	int nexti = n;
	while((nexti + frame_count) % 4 != 0){
		nexti++;
	}
	return nexti;
}

int AGMV_PrevIFrame(int n, int frame_count){
	int nexti = n;
	while((nexti - frame_count) % 4 != 0){
		nexti--;
	}
	return nexti;
}

int AGMV_SkipToNearestIFrame(int n){
	int nexti = n;
	while(nexti % 4 != 0){
		nexti++;
	}
	return nexti;
}

void QQSwap(u32* a, u32* b){
	u32 temp = *a;
	*a = *b;
	*b = temp;
}

int ppartition(u32* data, u32* gram, int low, int high)
{
    // choose the pivot
    int pivot = data[high];

    // Index of smaller element and Indicate
    // the right position of pivot found so far
    int i = (low - 1);
	int j;
    for (j = low; j <= high; j++) {
        // If current element is smaller than the pivot
        if (data[j] < pivot) {
            // Increment index of smaller element
            i++;
            QQSwap(&data[i], &data[j]);
			QQSwap(&gram[i], &gram[j]);
        }
    }
    QQSwap(&data[i + 1], &data[high]);
	QQSwap(&gram[i + 1], &gram[high]);
    return (i + 1);
}

void AGMV_QuickSort(u32* data, u32* gram, int low, int high)
{
    // when low is less than high
    if (low < high) {
        // pi is the partition return index of pivot

        int pi = ppartition(data, gram, low, high);

        // Recursion Call
        // smaller element than pivot goes left and
        // higher element goes right
        AGMV_QuickSort(data, gram, low, pi - 1);
        AGMV_QuickSort(data, gram, pi + 1, high);
    }
}

char* AGMV_Error2Str(Error error){
	switch(error){
		case NO_ERR:{
			return "NO ERROR";
		}
		case FILE_NOT_FOUND_ERR:{
			return "FILE NOT FOUND ERROR";
		}
		case INVALID_HEADER_FORMATTING_ERR:{
			return "INVALID HEADER FORMATTING ERROR";
		}
		case MEMORY_CORRUPTION_ERR:{
			return "MEMORY CORRUPTION ERROR";
		}
	}
	return "INVALID ERROR CODE";
}

u32 AGMV_GetNumberOfBytesRead(u32 bits){
	return (u32)(bits / 8.0f);
}

void AGMV_WavToAudioTrack(const char* filename, AGMV* agmv){
	u32 riff, chunk_size, wave, fmt, sub_chunk_size, audio_fmt, num_of_channels, sample_rate, byte_rate, block_align, bits_per_sample, data_chunk, sub_chunk_size2, i;
	u16* pcm, *pcm8;
	FILE* wav;
	
	wav = fopen(filename,"rb");
			
	riff = AGMV_ReadLong(wav);
	chunk_size = AGMV_ReadLong(wav);
	wave = AGMV_ReadLong(wav);
	fmt = AGMV_ReadLong(wav);
	sub_chunk_size = AGMV_ReadLong(wav);
	audio_fmt = AGMV_ReadShort(wav);
	num_of_channels = AGMV_ReadShort(wav);
	sample_rate = AGMV_ReadLong(wav);
	byte_rate = AGMV_ReadLong(wav);
	block_align = AGMV_ReadShort(wav);
	bits_per_sample = AGMV_ReadShort(wav);
	data_chunk = AGMV_ReadLong(wav);
	sub_chunk_size2 = AGMV_ReadLong(wav);
	
	if(bits_per_sample == 16){
		pcm = (u16*)malloc(sizeof(u16)*(chunk_size/2));
		fread(pcm,2,chunk_size/2,wav);
	}
	else{
		pcm8 = (u8*)malloc(sizeof(u8)*(chunk_size));
		fread(pcm8,1,chunk_size,wav);
	}
	
	fclose(wav);
	
	AGMV_SetTotalAudioDuration(agmv,chunk_size/(sample_rate*num_of_channels*(bits_per_sample/8)));
	AGMV_SetSampleRate(agmv,sample_rate);
	AGMV_SetNumberOfChannels(agmv,num_of_channels);
	
	if(bits_per_sample == 16){
		AGMV_SetAudioSize(agmv,chunk_size/2);
		agmv->audio_track->pcm = (u16*)malloc(sizeof(u16)*agmv->header.audio_size);
		AGMV_SyncAudioTrack(agmv,pcm);
		free(pcm);
	}
	else{
		AGMV_SetAudioSize(agmv,chunk_size);
		agmv->audio_chunk->atsample = (u8*)malloc(sizeof(u8)*agmv->header.audio_size);
		
		for(i = 0; i < agmv->header.audio_size; i++){
			agmv->audio_chunk->atsample[i] = pcm8[i];
		}
		
		free(pcm8);
	}
	
}

u32 AGMV_80BitFloat(FILE* file){
	u8 buf[10];
	
    buf[0] = AGMV_ReadByte(file);
	buf[1] = AGMV_ReadByte(file);
	buf[2] = AGMV_ReadByte(file);
	buf[3] = AGMV_ReadByte(file);
	buf[4] = AGMV_ReadByte(file);
	buf[5] = AGMV_ReadByte(file);
	buf[6] = AGMV_ReadByte(file);
	buf[7] = AGMV_ReadByte(file);
	buf[8] = AGMV_ReadByte(file);
	buf[9] = AGMV_ReadByte(file);
	
	if(buf[0] == 0x40 && buf[1] == 0x0E && buf[2] == 0xAC && buf[3] == 0x44){
		return 44100;
	}
	else if(buf[0] == 0x40 && buf[1] == 0x0D && buf[2] == 0x93 && buf[3] == 0xA8){
		return 18900;
	}
	else if(buf[0] == 0x40 && buf[1] == 0x0E && buf[2] == 0x93 && buf[3] == 0xA8){
		return 37800;
	}
	
	u32 freq = 0;

    int sign = (buf[0] >> 7) & 1;
	int exp = ((buf[0] & 127) << 8) + buf[1] - 16383;
	int j = (buf[2]>>7)&1;
	unsigned long frac = ((buf[2]&127) << 24) +
			      (buf[3] << 16) +
			      (buf[4] << 8) +
			      (buf[5]);
	unsigned long frac2 = (unsigned)(buf[6] << 24) +
			      (buf[7] << 16) +
			      (buf[8] << 8) +
			      (buf[9]);
	int i;

	freq = frac2;
	for (i=0; i<32; i++) freq /= 2;
	freq += frac;
	for (i=0; i<31; i++) freq /= 2;
	if (j) freq += 1.;
	while (exp) {
	  freq *= 2;
	  exp--;
	}

	if (sign) freq = -freq;
	
	return freq;
}

void AGMV_AIFCToAudioTrack(const char* filename, AGMV* agmv){
	char fourcc[4];
	u8* pcm8; u16* pcm;
	u16 num_of_channels, sample_size;
	u32 size, num_of_samples, sample_rate, i;
	FILE* file;
	
	file = fopen(filename,"rb");
	
	if(file != NULL){
		fseek(file,50,SEEK_SET);
		AGMV_ReadFourCC(file,fourcc);
		fseek(file,32,SEEK_SET);
		
		if(AGMV_IsCorrectFourCC(fourcc,'N','O','N','E')){
			num_of_channels = AGMV_SwapShort(AGMV_ReadShort(file));
			num_of_samples  = AGMV_SwapLong(AGMV_ReadLong(file));
			sample_size     = AGMV_SwapShort(AGMV_ReadShort(file));
			sample_rate     = AGMV_80BitFloat(file);
			
			fseek(file,10,SEEK_CUR);
			size = AGMV_SwapLong(AGMV_ReadLong(file));
			fseek(file,8,SEEK_CUR);

			AGMV_SetTotalAudioDuration(agmv,(size)/(sample_rate*num_of_channels*(sample_size/8)));
			AGMV_SetSampleRate(agmv,sample_rate);
			AGMV_SetNumberOfChannels(agmv,num_of_channels);
			
			if(sample_size == 16){
				pcm = (u16*)malloc(sizeof(u16)*(size/2));
				for(i = 0; i < size/2; i++){
					pcm[i] = AGMV_SwapShort(AGMV_ReadShort(file));
				}
				AGMV_SetAudioSize(agmv,size/2);
				agmv->audio_track->pcm = (u16*)malloc(sizeof(u16)*agmv->header.audio_size);
				AGMV_SyncAudioTrack(agmv,pcm);
				free(pcm);
			}
			else{
				pcm8 = (u8*)malloc(sizeof(u8)*size);
				fread(pcm8,1,size,file);
				AGMV_SetAudioSize(agmv,size);
				agmv->audio_chunk->atsample = (u8*)malloc(sizeof(u8)*agmv->header.audio_size);
				
				for(i = 0; i < agmv->header.audio_size; i++){
					agmv->audio_chunk->atsample[i] = pcm8[i];
				}
				
				free(pcm8);
			}
		}
		else if(AGMV_IsCorrectFourCC(fourcc,'s','o','w','t')){
			num_of_channels = AGMV_SwapShort(AGMV_ReadShort(file));
			num_of_samples  = AGMV_SwapLong(AGMV_ReadLong(file));
			sample_size     = AGMV_SwapShort(AGMV_ReadShort(file));
			sample_rate     = AGMV_80BitFloat(file);
			
			fseek(file,10,SEEK_CUR);
			size = AGMV_SwapLong(AGMV_ReadLong(file));
			fseek(file,8,SEEK_CUR);

			AGMV_SetTotalAudioDuration(agmv,(size)/(sample_rate*num_of_channels*(sample_size/8)));
			AGMV_SetSampleRate(agmv,sample_rate);
			AGMV_SetNumberOfChannels(agmv,num_of_channels);
			
			if(sample_size == 16){
				pcm = (u16*)malloc(sizeof(u16)*(size/2));
				for(i = 0; i < size/2; i++){
					pcm[i] = AGMV_ReadShort(file);
				}
				AGMV_SetAudioSize(agmv,size/2);
				agmv->audio_track->pcm = (u16*)malloc(sizeof(u16)*agmv->header.audio_size);
				AGMV_SyncAudioTrack(agmv,pcm);
				free(pcm);
			}
			else{
				pcm8 = (u8*)malloc(sizeof(u8)*size);
				fread(pcm8,1,size,file);
				AGMV_SetAudioSize(agmv,size);
				agmv->audio_chunk->atsample = (u8*)malloc(sizeof(u8)*agmv->header.audio_size);
				
				for(i = 0; i < agmv->header.audio_size; i++){
					agmv->audio_chunk->atsample[i] = pcm8[i];
				}
				
				free(pcm8);
			}
		}
		else{
			
		}
	}
	
	fclose(file);
}

void AGMV_AIFFToAudioTrack(const char* filename, AGMV* agmv){
	u8* pcm8; u16* pcm;
	u16 num_of_channels, sample_size;
	u32 size, num_of_samples, sample_rate, i;
	FILE* file;
	
	file = fopen(filename,"rb");
	
	if(file != NULL){
		fseek(file,20,SEEK_SET);
		
		num_of_channels = AGMV_SwapShort(AGMV_ReadShort(file));
		num_of_samples = AGMV_SwapLong(AGMV_ReadLong(file));
		sample_size = AGMV_SwapShort(AGMV_ReadShort(file));
		sample_rate = AGMV_80BitFloat(file);
		
		fseek(file,4,SEEK_CUR);
		size = AGMV_SwapLong(AGMV_ReadLong(file));
		fseek(file,8,SEEK_CUR);

		AGMV_SetTotalAudioDuration(agmv,(size/2)/(sample_rate*num_of_channels*(sample_size/8)));
		AGMV_SetSampleRate(agmv,sample_rate);
		AGMV_SetNumberOfChannels(agmv,num_of_channels);
		
		if(sample_size == 16){
			pcm = (u16*)malloc(sizeof(u16)*(size/2));
			for(i = 0; i < size/2; i++){
				pcm[i] = AGMV_SwapShort(AGMV_ReadShort(file));
			}
			AGMV_SetAudioSize(agmv,size/2);
			agmv->audio_track->pcm = (u16*)malloc(sizeof(u16)*agmv->header.audio_size);
			AGMV_SyncAudioTrack(agmv,pcm);
			free(pcm);
		}
		else{
			pcm8 = (u8*)malloc(sizeof(u8)*size);
			fread(pcm8,1,size,file);
			AGMV_SetAudioSize(agmv,size);
			agmv->audio_chunk->atsample = (u8*)malloc(sizeof(u8)*agmv->header.audio_size);
			
			for(i = 0; i < agmv->header.audio_size; i++){
				agmv->audio_chunk->atsample[i] = pcm8[i];
			}
			
			free(pcm8);
		}
	}
}

void AGMV_Raw8PCMToAudioTrack(const char* filename, AGMV* agmv){
	int i, file_size;
	s8* data;
	FILE* file;
	
	file = fopen(filename,"rb");
	fseek(file,0,SEEK_END);
	file_size = ftell(file);
	fseek(file,0,SEEK_SET);
	
	data = (s8*)malloc(sizeof(s8)*file_size);
	fread(data,1,file_size,file);
	
	AGMV_SetTotalAudioDuration(agmv,file_size/(16000));
	AGMV_SetSampleRate(agmv,16000);
	AGMV_SetNumberOfChannels(agmv,1);
	AGMV_SetAudioSize(agmv,file_size);
	
	agmv->audio_chunk->satsample = (s8*)malloc(sizeof(s8)*agmv->header.audio_size);
	
	for(i = 0; i < agmv->header.audio_size; i++){
		agmv->audio_chunk->satsample[i] = data[i];
	}
		
	free(data);
	fclose(file);
}

void AGMV_Export8PCMWav(const char* filename){
	u32 riff, chunk_size, wave, fmt, sub_chunk_size, audio_fmt, num_of_channels, sample_rate, byte_rate, block_align, bits_per_sample, data_chunk, sub_chunk_size2;
	u8* pcm; int i;
	FILE* wav, *out;
	
	wav = fopen(filename,"rb");
			
	riff = AGMV_ReadLong(wav);
	chunk_size = AGMV_ReadLong(wav);
	wave = AGMV_ReadLong(wav);
	fmt = AGMV_ReadLong(wav);
	sub_chunk_size = AGMV_ReadLong(wav);
	audio_fmt = AGMV_ReadShort(wav);
	num_of_channels = AGMV_ReadShort(wav);
	sample_rate = AGMV_ReadLong(wav);
	byte_rate = AGMV_ReadLong(wav);
	block_align = AGMV_ReadShort(wav);
	bits_per_sample = AGMV_ReadShort(wav);
	data_chunk = AGMV_ReadLong(wav);
	sub_chunk_size2 = AGMV_ReadLong(wav);
	
	pcm = (u8*)malloc(sizeof(u8)*(sub_chunk_size2));
	fread(pcm,1,sub_chunk_size2,wav);
	
	fclose(wav);
	
	out = fopen("sound.h","w");
	
	fprintf(out,"#ifndef SOUND_H\n");
	fprintf(out,"#define SOUND_H\n\n");
	fprintf(out,"#define AGMV_SAMPLE_RATE %ld\n",sample_rate);
	fprintf(out,"#define AGMV_AUDIO_SIZE %ld\n\n",sub_chunk_size2);
	fprintf(out,"unsigned char agmv_sound[AGMV_AUDIO_SIZE] = {\n");
	
	for(i = 0; i < sub_chunk_size2; i++){
		if(i % 100 == 0){
			fprintf(out,"\n");
		}
		fprintf(out,"%d,",pcm[i]);
	}
	
	fprintf(out,"\n};\n");
	fprintf(out,"#endif");
	
	fclose(out);
	free(pcm);
}

void AGMV_ExportRaw8PCM(const char* filename, u32 total_num_frames){
	int i, file_size;
	s8* data;
	FILE* file, *out;
	
	file = fopen(filename,"rb");
	fseek(file,0,SEEK_END);
	file_size = ftell(file);
	fseek(file,0,SEEK_SET);
	
	data = (s8*)malloc(sizeof(s8)*file_size);
	fread(data,1,file_size,file);
	
	fclose(file);
	
	out = fopen("sound.h","w");
	
	fprintf(out,"#ifndef SOUND_H\n");
	fprintf(out,"#define SOUND_H\n\n");
	fprintf(out,"#define SIZE %ld\n",file_size);
	fprintf(out,"#define SAMPLE_SIZE %ld\n\n",(u32)(file_size/(f32)(total_num_frames)));
	fprintf(out,"const signed char agmv_sound[SIZE] = {\n");
	
	for(i = 0; i < file_size; i++){
		if(i % 500 == 0){
			fprintf(out,"\n");
		}
		fprintf(out,"%d,",data[i]);
	}
	
	fprintf(out,"};\n");
	fprintf(out,"#endif");
	
	fclose(out);
	
	free(data);
	
}

AGMV_INFO AGMV_GetVideoInfo(AGMV* agmv){
	AGMV_INFO info;
	
	info.width = AGMV_GetWidth(agmv);
	info.height = AGMV_GetHeight(agmv);
	info.number_of_frames = AGMV_GetNumberOfFrames(agmv);
	info.version = AGMV_GetVersion(agmv);
	info.total_audio_duration = AGMV_GetTotalAudioDuration(agmv);
	info.sample_rate = AGMV_GetSampleRate(agmv);
	info.audio_size = AGMV_GetAudioSize(agmv);
	info.number_of_channels = AGMV_GetNumberOfChannels(agmv);
	
	return info;
}

int AGMV_ResetFrameRate(const char* filename, u32 frames_per_second){
	u32 file_size;
	FILE* file, *out;
	u8* data;
	
	file = fopen(filename,"rb");
	
	if(file == NULL){
		return FILE_NOT_FOUND_ERR;
	}
	
	fseek(file,0,SEEK_END);
	file_size = ftell(file);
	fseek(file,0,SEEK_SET);
	
	data = (u8*)malloc(sizeof(u8)*file_size);
	
	if(!fread(data,1,file_size,file)){
		free(data);
		fclose(file);
		return MEMORY_CORRUPTION_ERR;
	}
	
	fclose(file);
	
	data[21] = (frames_per_second >> 24) & 0xff;
	data[20] = (frames_per_second >> 16) & 0xff;
	data[19] = (frames_per_second >>  8) & 0xff;
	data[18] = (frames_per_second      ) & 0xff;
	
	out = fopen(filename,"wb");
	fwrite(data,1,file_size,out);
	
	free(data);
	fclose(out);

	return NO_ERR;
}

void to_80bitfloat(u32 num, u8 bytes[10]);

void AGMV_ExportAudioType(FILE* audio, AGMV* agmv, AGMV_AUDIO_TYPE audio_type){
	int i;
	u8 bytes[10];
	switch(audio_type){
		case AGMV_AUDIO_WAV:{
			AGMV_WriteFourCC(audio,'R','I','F','F');
			AGMV_WriteLong(audio,agmv->header.audio_size*2);
			AGMV_WriteFourCC(audio,'W','A','V','E');
			AGMV_WriteFourCC(audio,'f','m','t',0x20);
			AGMV_WriteLong(audio,16);
			AGMV_WriteShort(audio,1);
			AGMV_WriteShort(audio,agmv->header.num_of_channels);
			AGMV_WriteLong(audio,agmv->header.sample_rate);
			AGMV_WriteLong(audio,75600);
			AGMV_WriteShort(audio,4);
			AGMV_WriteShort(audio,16);
			AGMV_WriteFourCC(audio,'d','a','t','a');
			AGMV_WriteLong(audio,agmv->header.audio_size*2);
			fwrite(agmv->audio_track->pcm,2,agmv->header.audio_size,audio);
		}break;
		case AGMV_AUDIO_AIFF:{
			AGMV_WriteFourCC(audio,'F','O','R','M');
			AGMV_WriteLong(audio,AGMV_SwapLong((agmv->header.audio_size*2)+32));
			AGMV_WriteFourCC(audio,'A','I','F','C');
			AGMV_WriteFourCC(audio,'F','V','E','R');
			AGMV_WriteLong(audio,AGMV_SwapLong(4));
			AGMV_WriteFourCC(audio,0xA2,0x80,0x51,0x40);
			AGMV_WriteFourCC(audio,'C','O','M','M');
			AGMV_WriteLong(audio,AGMV_SwapLong(26));
			AGMV_WriteShort(audio,AGMV_SwapShort(AGMV_GetNumberOfChannels(agmv)));
			AGMV_WriteLong(audio,AGMV_SwapLong((AGMV_GetAudioSize(agmv))/AGMV_GetNumberOfChannels(agmv)));
			AGMV_WriteShort(audio,AGMV_SwapShort(16));
			
			to_80bitfloat(AGMV_GetSampleRate(agmv),bytes);
			
			for(i = 0; i < 10; i++){
				if(i < 4){
					AGMV_WriteByte(audio,bytes[i]);
				}
				else{
					AGMV_WriteByte(audio,0);
				}
			}
			
			AGMV_WriteFourCC(audio,'N','O','N','E');
			AGMV_WriteFourCC(audio,'N','O','N','E');
			AGMV_WriteFourCC(audio,'S','S','N','D');
			AGMV_WriteLong(audio,AGMV_SwapLong(AGMV_GetAudioSize(agmv)*2));
			AGMV_WriteLong(audio,0);
			AGMV_WriteLong(audio,0);
			
			for(i = 0; i < AGMV_GetAudioSize(agmv); i++){
				AGMV_WriteShort(audio,AGMV_SwapShort(agmv->audio_track->pcm[i]));
			}
		}break;
		case AGMV_AUDIO_AIFC:{
			AGMV_WriteFourCC(audio,'F','O','R','M');
			AGMV_WriteLong(audio,AGMV_SwapLong((agmv->header.audio_size*2)+32));
			AGMV_WriteFourCC(audio,'A','I','F','F');
			AGMV_WriteFourCC(audio,'C','O','M','M');
			AGMV_WriteLong(audio,AGMV_SwapLong(18));
			AGMV_WriteShort(audio,AGMV_SwapShort(AGMV_GetNumberOfChannels(agmv)));
			AGMV_WriteLong(audio,AGMV_SwapLong((AGMV_GetAudioSize(agmv))/AGMV_GetNumberOfChannels(agmv)));
			AGMV_WriteShort(audio,AGMV_SwapShort(16));
			
			to_80bitfloat(AGMV_GetSampleRate(agmv),bytes);
			
			for(i = 0; i < 10; i++){
				if(i < 4){
					AGMV_WriteByte(audio,bytes[i]);
				}
				else{
					AGMV_WriteByte(audio,0);
				}
			}

			AGMV_WriteFourCC(audio,'S','S','N','D');
			AGMV_WriteLong(audio,AGMV_SwapLong(AGMV_GetAudioSize(agmv)*2));
			AGMV_WriteLong(audio,0);
			AGMV_WriteLong(audio,0);
			
			for(i = 0; i < AGMV_GetAudioSize(agmv); i++){
				AGMV_WriteShort(audio,AGMV_SwapShort(agmv->audio_track->pcm[i]));
			}
		}break;
		default:{
			AGMV_WriteFourCC(audio,'R','I','F','F');
			AGMV_WriteLong(audio,agmv->header.audio_size*2);
			AGMV_WriteFourCC(audio,'W','A','V','E');
			AGMV_WriteFourCC(audio,'f','m','t',0x20);
			AGMV_WriteLong(audio,16);
			AGMV_WriteShort(audio,1);
			AGMV_WriteShort(audio,agmv->header.num_of_channels);
			AGMV_WriteLong(audio,agmv->header.sample_rate);
			AGMV_WriteLong(audio,75600);
			AGMV_WriteShort(audio,4);
			AGMV_WriteShort(audio,16);
			AGMV_WriteFourCC(audio,'d','a','t','a');
			AGMV_WriteLong(audio,agmv->header.audio_size*2);
			fwrite(agmv->audio_track->pcm,2,agmv->header.audio_size,audio);
		}break;
	}
}

void AGMV_ExportAGMVToHeader(const char* filename){
	FILE* in, *out;
	u32 file_size, i;
	u8* data;
	
	in = fopen(filename,"rb");
	
	fseek(in,0,SEEK_END);
	file_size = ftell(in);
	fseek(in,0,SEEK_SET);
	
	data = (u8*)malloc(sizeof(u8)*file_size);
	fread(data,1,file_size,in);
	
	fclose(in);
	
	out = fopen("agmv.h","w");
	
	fprintf(out,"#ifndef AGMV_H\n");
	fprintf(out,"#define AGMV_H\n\n");
	fprintf(out,"#define FILE_SIZE %ld\n\n",file_size);
	fprintf(out,"const unsigned char agmv_file[FILE_SIZE] = {\n");
	
	for(i = 0; i < file_size; i++){
		if(i % 500 == 0 && i != 0){
			fprintf(out,"\n");
		}
		fprintf(out,"%d,",data[i]);
	}
	
	fprintf(out,"};\n");
	fprintf(out,"#endif");
	
	fclose(out);
	
	free(data);
	
	
}