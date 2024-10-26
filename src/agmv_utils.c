/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_utils.c
*   Date: 5/17/2024
*   Version: 2.0
*   Updated: 10/25/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <stdlib.h>
#include <agmv_utils.h>
#include <agmv_decode.h>
#include <stdarg.h>

const   int BAYER_PATTERN_16X16[16][16] =   
{   //  16x16 Bayer Dithering Matrix.  Color levels: 256
    {     0, 191,  48, 239,  12, 203,  60, 251,   3, 194,  51, 242,  15, 206,  63, 254  }, 
    {   127,  64, 175, 112, 139,  76, 187, 124, 130,  67, 178, 115, 142,  79, 190, 127  },
    {    32, 223,  16, 207,  44, 235,  28, 219,  35, 226,  19, 210,  47, 238,  31, 222  },
    {   159,  96, 143,  80, 171, 108, 155,  92, 162,  99, 146,  83, 174, 111, 158,  95  },
    {     8, 199,  56, 247,   4, 195,  52, 243,  11, 202,  59, 250,   7, 198,  55, 246  },
    {   135,  72, 183, 120, 131,  68, 179, 116, 138,  75, 186, 123, 134,  71, 182, 119  },
    {    40, 231,  24, 215,  36, 227,  20, 211,  43, 234,  27, 218,  39, 230,  23, 214  },
    {   167, 104, 151,  88, 163, 100, 147,  84, 170, 107, 154,  91, 166, 103, 150,  87  },
    {     2, 193,  50, 241,  14, 205,  62, 253,   1, 192,  49, 240,  13, 204,  61, 252  },
    {   129,  66, 177, 114, 141,  78, 189, 126, 128,  65, 176, 113, 140,  77, 188, 125  },
    {    34, 225,  18, 209,  46, 237,  30, 221,  33, 224,  17, 208,  45, 236,  29, 220  },
    {   161,  98, 145,  82, 173, 110, 157,  94, 160,  97, 144,  81, 172, 109, 156,  93  },
    {    10, 201,  58, 249,   6, 197,  54, 245,   9, 200,  57, 248,   5, 196,  53, 244  },
    {   137,  74, 185, 122, 133,  70, 181, 118, 136,  73, 184, 121, 132,  69, 180, 117  },
    {    42, 233,  26, 217,  38, 229,  22, 213,  41, 232,  25, 216,  37, 228,  21, 212  },
    {   169, 106, 153,  90, 165, 102, 149,  86, 168, 105, 152,  89, 164, 101, 148,  85  }
};

Bool print = TRUE;

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

void AGMV_ReadNBytes(FILE* file, u8* buf, u32 n){
	fread(buf,1,n,file);
}

void AGMV_ReadNShorts(FILE* file, u16* buf, u32 n){
	fread(buf,2,n,file);
}

void AGMV_ReadSwappedNShorts(FILE* file, u16* buf, u32 n){
	int i;
	for(i = 0; i < n; i++){
		buf[i] = AGMV_SwapShort(AGMV_ReadShort(file));
	}
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

void AGMV_WriteNBytes(FILE* file, u8* buf, u32 n){
	fwrite(buf,1,n,file);
}

Bool AGMV_IsCorrectFourCC(char fourcc[4], char f, char o, char u, char r){
	if(f != fourcc[0] || o != fourcc[1] || u != fourcc[2] || r != fourcc[3]){
		return FALSE;
	}
	else return TRUE;
}

Bool AGMV_IsValidFlag(u8 byte){
	if(byte == AGMV_NORMAL_FLAG || byte == AGMV_COPY_FLAG || byte == AGMV_FILL_FLAG || byte == AGMV_COPY_FLAG || byte == AGMV_MV_FLAG || 
	byte == AGMV_PCOPY_FLAG || byte == AGMV_PMV_FLAG || byte == AGMV_VQ2_FLAG || byte == AGMV_VQ4_FLAG){
		return TRUE;
	}
	else return FALSE;
}

Bool AGMV_IsNextFrameAnIFrame(FILE* file){
	u32 pos, frame_type;
	
	pos = ftell(file);

	AGMV_FindNextFrameChunk(file);
	frame_type = AGMV_SkipFrameChunk(file);
	
	fseek(file,pos,SEEK_SET);
	
	if(frame_type == AGMV_IFRAME){
		return TRUE;
	}
	else return FALSE;
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

void AGMV_FindNextAudsChunk(FILE* file){
	u32 pos;
	Bool isAudio;
	char fourcc[4];

	AGMV_ReadFourCC(file,fourcc);
	isAudio = AGMV_IsCorrectFourCC(fourcc,'a','u','d','s');

	if(isAudio){
		pos = ftell(file);
		fseek(file,pos-4,SEEK_SET);
		return;
	}

	while(!isAudio && !AGMV_EOF(file)){
		AGMV_ReadFourCC(file,fourcc);
		isAudio = AGMV_IsCorrectFourCC(fourcc,'a','u','d','s');
		
		if(!isAudio){
			pos = ftell(file);
			fseek(file,pos-3,SEEK_SET);
		}
	}

	pos = ftell(file);
	fseek(file,pos-4,SEEK_SET);
}

void AGMV_FindNextStrfChunk(FILE* file){
	u32 pos;
	Bool isAudio;
	char fourcc[4];

	AGMV_ReadFourCC(file,fourcc);
	isAudio = AGMV_IsCorrectFourCC(fourcc,'s','t','r','f');

	if(isAudio){
		pos = ftell(file);
		fseek(file,pos-4,SEEK_SET);
		return;
	}

	while(!isAudio && !AGMV_EOF(file)){
		AGMV_ReadFourCC(file,fourcc);
		isAudio = AGMV_IsCorrectFourCC(fourcc,'s','t','r','f');
		
		if(!isAudio){
			pos = ftell(file);
			fseek(file,pos-3,SEEK_SET);
		}
	}

	pos = ftell(file);
	fseek(file,pos-4,SEEK_SET);
}

void AGMV_FindNextFourCCChunk(FILE* file, char f, char o, char u, char r){
	u32 pos;
	Bool isFourcc;
	char fourcc[4];

	AGMV_ReadFourCC(file,fourcc);
	isFourcc = AGMV_IsCorrectFourCC(fourcc,f,o,u,r);

	if(isFourcc){
		pos = ftell(file);
		fseek(file,pos-4,SEEK_SET);
		return;
	}

	while(!isFourcc && !AGMV_EOF(file)){
		AGMV_ReadFourCC(file,fourcc);
		isFourcc = AGMV_IsCorrectFourCC(fourcc,f,o,u,r);
		
		if(!isFourcc){
			pos = ftell(file);
			fseek(file,pos-3,SEEK_SET);
		}
	}

	pos = ftell(file);
	fseek(file,pos-4,SEEK_SET);
}

u16 AGMV_SkipFrameChunk(FILE* file){
	u32 fourcc, frame_num, frame_type, usize, csize;

	AGMV_FindNextFrameChunk(file);

	fourcc     = AGMV_ReadLong(file);
	frame_num  = AGMV_ReadLong(file);
	frame_type = AGMV_ReadShort(file);
	usize      = AGMV_ReadLong(file);
	csize      = AGMV_ReadLong(file);

	fseek(file,csize,SEEK_CUR);
	
	return frame_type;
}

void AGMV_SkipAudioChunk(FILE* file){
	u32 fourcc, size, csize;

	AGMV_FindNextAudioChunk(file);

	fourcc = AGMV_ReadLong(file);
	size   = AGMV_ReadLong(file);
	csize  = AGMV_ReadLong(file);

	fseek(file,csize,SEEK_CUR);
}

void AGMV_ParseAGMV(FILE* file, AGMV* agmv){
	u32* offset_table, n;
	
	offset_table = agmv->offset_table;
	n            = AGMV_GetNumberOfFrames(agmv);
	
	if(AGMV_GetTotalAudioDuration(agmv) != 0){
		int i;
		for(i = 0; i < n; i++){
			AGMV_FindNextFrameChunk(file);
			offset_table[i] = ftell(file);
			AGMV_SkipFrameChunk(file);
			AGMV_FindNextAudioChunk(file);
			AGMV_SkipAudioChunk(file);
		}
	}
	else{
		int i;
		for(i = 0; i < n; i++){
			AGMV_FindNextFrameChunk(file);
			offset_table[i] = ftell(file);
			AGMV_SkipFrameChunk(file);
		}
	}
}

/*------PRIMARY FUNCTIONS TO INITIALIZE AGMV ATTRIBUTES----------*/

void AGMV_SetWidth(AGMV* agmv, u32 width){
	agmv->header.width = width;
	agmv->frame->width = width;
	agmv->iframe->width = width;
	agmv->prev_frame->width = width;
}

void AGMV_SetHeight(AGMV* agmv, u32 height){
	agmv->header.height = height;
	agmv->frame->height = height;
	agmv->iframe->height = height;
	agmv->prev_frame->height = height;
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

void AGMV_SetQuality(AGMV* agmv, AGMV_QUALITY quality){
	agmv->quality = quality;
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

void AGMV_SetBitsPerSample(AGMV* agmv, u16 bits_per_sample){
	agmv->header.bits_per_sample = bits_per_sample;
}

void AGMV_SetError(AGMV* agmv, Error error){
	agmv->error = error;
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
	agmv->prev_frame = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->prev_frame->img_data = (u32*)malloc(sizeof(u32)*width*height);
	agmv->audio_track = (AGMV_AUDIO_TRACK*)malloc(sizeof(AGMV_AUDIO_TRACK));
	agmv->iframe_entries = (AGMV_ENTRY*)malloc(sizeof(AGMV_ENTRY)*width*height);
	agmv->image_entries = (AGMV_ENTRY*)malloc(sizeof(AGMV_ENTRY)*width*height);
	agmv->prev_fill_color = 0;
	agmv->vq2color1 = 0; agmv->vq2color2 = 0;
	agmv->audio_track->pcm = NULL;
	agmv->audio_track->pcm8 = NULL;
	agmv->audio_chunk->atsample = NULL;

	agmv->frame_count = 0;
	agmv->audio_track->start_point = 0;
	agmv->pframe_count = 0;
	agmv->iframe_count = 0;
	
	agmv->db.curr_pos = 0;
	agmv->db.read_pos = 0;
	
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
	AGMV_SetQuality(agmv,AGMV_LOW_QUALITY);
	AGMV_SetVolume(agmv,1.0f);
	AGMV_SetBitsPerSample(agmv,16);

	return agmv;
}

void DestroyAGMV(AGMV* agmv){
	if(agmv != NULL){
		if(agmv->iframe_entries != NULL){
			free(agmv->iframe_entries);
			agmv->iframe_entries = NULL;
		}
		
		if(agmv->image_entries != NULL){
			free(agmv->image_entries);
			agmv->image_entries = NULL;
		}
		
		if(agmv->frame != NULL){
			
			if(agmv->frame->img_data != NULL){
				free(agmv->frame->img_data);
				agmv->frame->img_data = NULL;
			}
			
			free(agmv->frame);
			agmv->frame = NULL;
		}
				
		if(agmv->iframe != NULL){
			
			if(agmv->iframe->img_data != NULL){
				free(agmv->iframe->img_data);
				agmv->iframe->img_data = NULL;
			}

			free(agmv->iframe);
			agmv->iframe = NULL;
		}
		if(agmv->prev_frame != NULL){
			
			if(agmv->prev_frame->img_data != NULL){
				free(agmv->prev_frame->img_data);
				agmv->prev_frame->img_data = NULL;
			}
			
			free(agmv->prev_frame);
			agmv->prev_frame = NULL;
		}

		if(agmv->bitstream != NULL){
			if(agmv->bitstream->data != NULL){
				free(agmv->bitstream->data);
			}
			free(agmv->bitstream);
			agmv->bitstream = NULL;
		}
		
		if(agmv->frame_chunk != NULL){
			free(agmv->frame_chunk);
		}
		
		if(agmv->header.total_audio_duration != 0){
			agmv->header.total_audio_duration = 0;
			
			if(agmv->header.bits_per_sample == 16){
				if(agmv->audio_track->pcm != NULL){
					free(agmv->audio_track->pcm);
					agmv->audio_track->pcm = NULL;
				}
			}
			else{
				if(agmv->audio_track->pcm8 != NULL){
					free(agmv->audio_track->pcm8);
					agmv->audio_track->pcm8 = NULL;
				}
			}
			
			if(agmv->audio_chunk->atsample != NULL){
				free(agmv->audio_chunk->atsample);
				agmv->audio_chunk->atsample = NULL;
			}
		}
	
		agmv->bitstream = NULL;
		agmv->frame_chunk = NULL;

		free(agmv->audio_chunk);
		free(agmv->audio_track);
		
		agmv->audio_chunk = NULL;
		agmv->audio_track = NULL;
		
		free(agmv);

		agmv = NULL;

	}
}

void AGMV_EnablePrintf(){
	print = TRUE;
}

void AGMV_DisablePrintf(){
	print = FALSE;
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

u32 AGMV_GetNumberOfIFrames(AGMV* agmv){
	return agmv->iframe_count;
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

AGMV_QUALITY AGMV_GetQuality(AGMV* agmv){
	return agmv->quality;
}

f32 AGMV_GetVolume(AGMV* agmv){
	return agmv->volume;
}

u16 AGMV_GetBitsPerSample(AGMV* agmv){
	return agmv->header.bits_per_sample;
}

Error AGMV_GetError(AGMV* agmv){
	return agmv->error;
}

/*-----------------VARIOUS UTILITY FUNCTIONS-----------------*/
void AGMV_Memcpy32(u32* dest, u32* src, u32 n){
	while(n--){
		*(dest++) = *(src++);
	}
}

void AGMV_Memset32(u32* dest, u32 num, u32 n){
	while(n--){
		*(dest++) = num;	
	}
}

void AGMV_Memcpy16(u16* dest, u16* src, u32 n){
	while(n--){
		*(dest++) = *(src++);
	}
}

void AGMV_Memcpy8(u8* dest, u8* src, u32 n){
	while(n--){
		*(dest++) = *(src++);
	}
}

void AGMV_Memset8(u8* dest, u8 num, u32 n){
	while(n--){
		*(dest++) = num;	
	}
}

u8 AGMV_GetMSB(u16 color){
	return (color >> 8) & 0xff;
}

u8 AGMV_GetLSB(u16 color){
	return (color & 0xff);
}

void AGMV_Printf(char* fmt, ...){
	if(print){
		va_list	argptr;
		char msg[AGMV_MAX_PRINT];
		
		va_start(argptr,fmt);
		vsprintf(msg,fmt,argptr);
		va_end(argptr);

		printf("%s",msg);
	}
}

u8 AGMV_SetBitstreamFlag(AGMV_BLOCK_SIZE block_size, u8 flag){
	return block_size << 6 | flag;
}

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
			case AGMV_OPT_IV:{
				return 0;
			}
			case AGMV_OPT_ANIM:{
				return 2;
			}
			case AGMV_OPT_GBA_I:{
				return 5;
			}
			case AGMV_OPT_GBA_II:{
				return 6;
			}
			case AGMV_OPT_GBA_III:{
				return 5;
			}
			case AGMV_OPT_GBA_IV:{
				return 10;
			}
			case AGMV_OPT_PSX_I:{
				return 35;
			}
			case AGMV_OPT_PSX_II:{
				return 36;
			}
			case AGMV_OPT_PSX_III:{
				return 35;
			}
			case AGMV_OPT_PSX_IV:{
				return 20;
			}
			case AGMV_OPT_NDS_I:{
				return 25;
			}
			case AGMV_OPT_NDS_II:{
				return 26;
			}
			case AGMV_OPT_NDS_III:{
				return 25;
			}
			case AGMV_OPT_NDS_IV:{
				return 30;
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
			case AGMV_OPT_IV:{
				return 100;
			}
			case AGMV_OPT_ANIM:{
				return 4;
			}
			case AGMV_OPT_GBA_I:{
				return 7;
			}
			case AGMV_OPT_GBA_II:{
				return 8;
			}
			case AGMV_OPT_GBA_III:{
				return 7;
			}
			case AGMV_OPT_GBA_IV:{
				return 101;
			}
			case AGMV_OPT_PSX_I:{
				return 37;
			}
			case AGMV_OPT_PSX_II:{
				return 38;
			}
			case AGMV_OPT_PSX_III:{
				return 37;
			}
			case AGMV_OPT_PSX_IV:{
				return 102;
			}
			case AGMV_OPT_NDS_I:{
				return 27;
			}
			case AGMV_OPT_NDS_II:{
				return 28;
			}
			case AGMV_OPT_NDS_III:{
				return 27;
			}
			case AGMV_OPT_NDS_IV:{
				return 103;
			}
		}
	}
	
	return 1;
}

u8 AGMV_GetCount(AGMV* agmv, AGMV_BLOCK_SIZE block_size, u8 flag, AGMV_QUALITY quality, AGMV_OPT opt){
	u8 count;
	
	if(quality == AGMV_CUSTOM_QUALITY){
		if(block_size == AGMV_BLOCK_4x4){
			switch(flag){
				case AGMV_FILL_FLAG:{
					return agmv->custom.b4x4.fill;
				}
				case AGMV_ICOPY_FLAG:{
					return agmv->custom.b4x4.icopy;
				}
				case AGMV_COPY_FLAG:{
					return agmv->custom.b4x4.copy;
				}
				case AGMV_PCOPY_FLAG:{
					return agmv->custom.b4x4.pcopy;
				}
				case AGMV_PMV_FLAG:{
					return agmv->custom.b4x4.pmv;
				}
				case AGMV_MV_FLAG:{
					return agmv->custom.b4x4.mv;
				}
				case AGMV_VQ2_FLAG:{
					return agmv->custom.b4x4.vq2;
				}
				case AGMV_VQ4_FLAG:{
					return agmv->custom.b4x4.vq4;
				}
			}
		}
		else if(block_size == AGMV_BLOCK_8x4){
			switch(flag){
				case AGMV_FILL_FLAG:{
					return agmv->custom.b8x4.fill;
				}
				case AGMV_ICOPY_FLAG:{
					return agmv->custom.b8x4.icopy;
				}
				case AGMV_COPY_FLAG:{
					return agmv->custom.b8x4.copy;
				}
				case AGMV_PCOPY_FLAG:{
					return agmv->custom.b8x4.pcopy;
				}
				case AGMV_PMV_FLAG:{
					return agmv->custom.b8x4.pmv;
				}
				case AGMV_MV_FLAG:{
					return agmv->custom.b8x4.mv;
				}
				case AGMV_VQ2_FLAG:{
					return agmv->custom.b8x4.vq2;
				}
				case AGMV_VQ4_FLAG:{
					return agmv->custom.b8x4.vq4;
				}
			}
		}
		else if(block_size == AGMV_BLOCK_8x8){
			switch(flag){
				case AGMV_FILL_FLAG:{
					return agmv->custom.b8x8.fill;
				}
				case AGMV_ICOPY_FLAG:{
					return agmv->custom.b8x8.icopy;
				}
				case AGMV_COPY_FLAG:{
					return agmv->custom.b8x8.copy;
				}
				case AGMV_PCOPY_FLAG:{
					return agmv->custom.b8x8.pcopy;
				}
				case AGMV_PMV_FLAG:{
					return agmv->custom.b8x8.pmv;
				}
				case AGMV_MV_FLAG:{
					return agmv->custom.b8x8.mv;
				}
				case AGMV_VQ2_FLAG:{
					return agmv->custom.b8x8.vq2;
				}
				case AGMV_VQ4_FLAG:{
					return agmv->custom.b8x8.vq4;
				}
			}
		}
		else{
			switch(flag){
				case AGMV_FILL_FLAG:{
					return agmv->custom.b2x4.fill;
				}
				case AGMV_ICOPY_FLAG:{
					return agmv->custom.b2x4.icopy;
				}
				case AGMV_COPY_FLAG:{
					return agmv->custom.b2x4.copy;
				}
				case AGMV_PCOPY_FLAG:{
					return agmv->custom.b2x4.pcopy;
				}
				case AGMV_PMV_FLAG:{
					return agmv->custom.b2x4.pmv;
				}
				case AGMV_MV_FLAG:{
					return agmv->custom.b2x4.mv;
				}
				case AGMV_VQ2_FLAG:{
					return agmv->custom.b2x4.vq2;
				}
				case AGMV_VQ4_FLAG:{
					return agmv->custom.b2x4.vq4;
				}
			}
		}
	}

	if(quality == AGMV_LOW_QUALITY){
		if(block_size == AGMV_BLOCK_4x4){
			switch(flag){
				case AGMV_FILL_FLAG:{
					count = AGMV_FILL_COUNT;
				}break;
				case AGMV_ICOPY_FLAG:{
					count = AGMV_ICOPY_COUNT;
				}break;
				case AGMV_COPY_FLAG:{
					count = AGMV_COPY_COUNT;
				}break;
				case AGMV_PCOPY_FLAG:{
					count = AGMV_PCOPY_COUNT;
				}break;
				case AGMV_PMV_FLAG:{
					count = AGMV_PMV_COUNT;
				}break;
				case AGMV_MV_FLAG:{
					count = AGMV_MV_COUNT;
				}break;
				case AGMV_VQ2_FLAG:{
					count = 2;
				}break;
				case AGMV_VQ4_FLAG:{
					count = 4;
				}break;
			}
		}
		else if(block_size == AGMV_BLOCK_8x4){
			switch(flag){
				case AGMV_FILL_FLAG:{
					count = 26;
				}break;
				case AGMV_ICOPY_FLAG:{
					count = 26;
				}break;
				case AGMV_PCOPY_FLAG:{
					count = 25;
				}break;
				case AGMV_COPY_FLAG:{
					count = 26;
				}break;
				case AGMV_PMV_FLAG:{
					count = 28;
				}break;
				case AGMV_MV_FLAG:{
					count = 28;
				}break;
				case AGMV_VQ2_FLAG:{
					count = 3;
				}break;
				case AGMV_VQ4_FLAG:{
					count = 6;
				}break;
			}
		}
		else if(block_size == AGMV_BLOCK_8x8){
			switch(flag){
				case AGMV_FILL_FLAG:{
					count = 56;
				}break;
				case AGMV_ICOPY_FLAG:{
					count = 54;
				}break;
				case AGMV_COPY_FLAG:{
					count = 54;
				}break;
				case AGMV_PCOPY_FLAG:{
					count = 52;
				}break;
				case AGMV_PMV_FLAG:{
					count = 52;
				}break;
				case AGMV_MV_FLAG:{
					count = 52;
				}break;
				case AGMV_VQ2_FLAG:{
					count = 2;
				}break;
				case AGMV_VQ4_FLAG:{
					count = 4;
				}break;
			}
		}
		else{
			switch(flag){
				case AGMV_FILL_FLAG:{
					count = 6;
				}break;
				case AGMV_ICOPY_FLAG:{
					count = 6;
				}break;
				case AGMV_COPY_FLAG:{
					count = 5;
				}break;
				case AGMV_PCOPY_FLAG:{
					count = 5;
				}break;
				case AGMV_PMV_FLAG:{
					count = 5;
				}break;
				case AGMV_MV_FLAG:{
					count = 5;
				}break;
				case AGMV_VQ2_FLAG:{
					count = 2;
				}break;
				case AGMV_VQ4_FLAG:{
					count = 4;
				}break;
			}
		}
	}
	else if(quality == AGMV_MID_QUALITY){
		if(block_size == AGMV_BLOCK_4x4){
			switch(flag){
				case AGMV_FILL_FLAG:{
					count = 14;
				}break;
				case AGMV_ICOPY_FLAG:{
					count = 14;
				}break;
				case AGMV_COPY_FLAG:{
					count = 14;
				}break;
				case AGMV_PCOPY_FLAG:{
					count = 14;
				}break;
				case AGMV_PMV_FLAG:{
					count = 14;
				}break;
				case AGMV_MV_FLAG:{
					count = 14;
				}break;
				case AGMV_VQ2_FLAG:{
					count = 2;
				}break;
				case AGMV_VQ4_FLAG:{
					count = 4;
				}break;
			}
		}
		else if(block_size == AGMV_BLOCK_8x4){
			switch(flag){
				case AGMV_FILL_FLAG:{
					count = 30;
				}break;
				case AGMV_ICOPY_FLAG:{
					count = 30;
				}break;
				case AGMV_PCOPY_FLAG:{
					count = 28;
				}break;
				case AGMV_COPY_FLAG:{
					count = 28;
				}break;
				case AGMV_PMV_FLAG:{
					count = 28;
				}break;
				case AGMV_MV_FLAG:{
					count = 28;
				}break;
				case AGMV_VQ2_FLAG:{
					count = 3;
				}break;
				case AGMV_VQ4_FLAG:{
					count = 6;
				}break;
			}
		}
		else if(block_size == AGMV_BLOCK_8x8){
			switch(flag){
				case AGMV_FILL_FLAG:{
					count = 60;
				}break;
				case AGMV_ICOPY_FLAG:{
					count = 58;
				}break;
				case AGMV_COPY_FLAG:{
					count = 58;
				}break;
				case AGMV_PCOPY_FLAG:{
					count = 59;
				}break;
				case AGMV_PMV_FLAG:{
					count = 58;
				}break;
				case AGMV_MV_FLAG:{
					count = 58;
				}break;
				case AGMV_VQ2_FLAG:{
					count = 3;
				}break;
				case AGMV_VQ4_FLAG:{
					count = 6;
				}break;
			}
		}
		else{
			switch(flag){
				case AGMV_FILL_FLAG:{
					count = 7;
				}break;
				case AGMV_ICOPY_FLAG:{
					count = 6;
				}break;
				case AGMV_COPY_FLAG:{
					count = 7;
				}break;
				case AGMV_PCOPY_FLAG:{
					count = 7;
				}break;
				case AGMV_PMV_FLAG:{
					count = 6;
				}break;
				case AGMV_MV_FLAG:{
					count = 6;
				}break;
				case AGMV_VQ2_FLAG:{
					count = 2;
				}break;
				case AGMV_VQ4_FLAG:{
					count = 4;
				}break;
			}
		}
	}
	else{
		if(block_size == AGMV_BLOCK_4x4){
			switch(flag){
				case AGMV_FILL_FLAG:{
					count = 16;
				}break;
				case AGMV_ICOPY_FLAG:{
					count = 16;
				}break;
				case AGMV_COPY_FLAG:{
					count = 16;
				}break;
				case AGMV_PCOPY_FLAG:{
					count = 16;
				}break;
				case AGMV_PMV_FLAG:{
					count = 15;
				}break;
				case AGMV_MV_FLAG:{
					count = 15;
				}break;
				case AGMV_VQ2_FLAG:{
					count = 2;
				}break;
				case AGMV_VQ4_FLAG:{
					count = 4;
				}break;
			}
		}
		else if(block_size == AGMV_BLOCK_8x4){
			switch(flag){
				case AGMV_FILL_FLAG:{
					count = 32;
				}break;
				case AGMV_ICOPY_FLAG:{
					count = 32;
				}break;
				case AGMV_PCOPY_FLAG:{
					count = 32;
				}break;
				case AGMV_COPY_FLAG:{
					count = 32;
				}break;
				case AGMV_PMV_FLAG:{
					count = 31;
				}break;
				case AGMV_MV_FLAG:{
					count = 31;
				}break;
				case AGMV_VQ2_FLAG:{
					count = 2;
				}break;
				case AGMV_VQ4_FLAG:{
					count = 4;
				}break;
			}
		}
		else if(block_size == AGMV_BLOCK_8x8){
			switch(flag){
				case AGMV_FILL_FLAG:{
					count = 63;
				}break;
				case AGMV_ICOPY_FLAG:{
					count = 63;
				}break;
				case AGMV_COPY_FLAG:{
					count = 63;
				}break;
				case AGMV_PCOPY_FLAG:{
					count = 63;
				}break;
				case AGMV_PMV_FLAG:{
					count = 62;
				}break;
				case AGMV_MV_FLAG:{
					count = 62;
				}break;
				case AGMV_VQ2_FLAG:{
					count = 2;
				}break;
				case AGMV_VQ4_FLAG:{
					count = 4;
				}break;
			}
		}
		else{
			switch(flag){
				case AGMV_FILL_FLAG:{
					count = 8;
				}break;
				case AGMV_ICOPY_FLAG:{
					count = 8;
				}break;
				case AGMV_COPY_FLAG:{
					count = 8;
				}break;
				case AGMV_PCOPY_FLAG:{
					count = 8;
				}break;
				case AGMV_PMV_FLAG:{
					count = 7;
				}break;
				case AGMV_MV_FLAG:{
					count = 7;
				}break;
				case AGMV_VQ2_FLAG:{
					count = 2;
				}break;
				case AGMV_VQ4_FLAG:{
					count = 4;
				}break;
			}
		}
	}
	
	return count;
}

u8 AGMV_GetBlockWidth(AGMV_BLOCK_SIZE block){
	if(block == AGMV_BLOCK_4x4){
		return 4;
	}
	else if(block == AGMV_BLOCK_2x4){
		return 2;
	}
	else return 8;
}

u8 AGMV_GetBlockHeight(AGMV_BLOCK_SIZE block){
	if(block == AGMV_BLOCK_8x8){
		return 8;
	}
	else return 4;
}

int AGMV_Clamp(int min, int value, int max){
	if(value < min){
		return min;
	}
	else if(value > max){
		return max;
	}
	else return value;
}

void AGMV_DitherImageData(u32* img_data, u32 width, u32 height){
	int x, y, row, col, t, corr, blue, green, red, i1, i2, i3;
	u32 color, yoffset;
	for(y = 0; y < height; y++)
	{
		row	= y & 15;	//	y % 16
		
		yoffset = y * width;
		
		for(x = 0; x < width; x++)
		{
			col	= x & 15;	//	x % 16

			t		= BAYER_PATTERN_16X16[col][row];
			corr	= t / 15;
			
			color = img_data[x+yoffset];
			
			blue	= AGMV_GetB(color);
			green	= AGMV_GetG(color);
			red		= AGMV_GetR(color);

			i1	= (blue  + corr) / 17;		AGMV_Clamp(0,i1,15);
			i2	= (green + corr) / 17;		AGMV_Clamp(0,i2,15);
			i3	= (red   + corr) / 17;		AGMV_Clamp(0,i3,15);
			
			img_data[x+yoffset] = (i3*17) << 16 | (i2*17) << 8 | (i1*17);
		}
	}
}

u32 AGMV_GetMicrosecondsPerFrame(u32 frame_rate){
	f32 div = 1.0f / frame_rate;
	
	return (u32)(div * 1000000);
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
	AGMV_Memcpy32(dest,src,size);
}

void AGMV_SyncFrameAndImage(AGMV* agmv, u32* img_data){
	AGMV_CopyImageData(agmv->frame->img_data,img_data,AGMV_GetWidth(agmv)*AGMV_GetHeight(agmv));
}

void AGMV_SyncAudioTrack(AGMV* agmv, const void* pcm){
	u32 audio_size = AGMV_GetAudioSize(agmv);
	u16* apcm, bps = AGMV_GetBitsPerSample(agmv);
	u8* apcm8;
	
	apcm  = agmv->audio_track->pcm;
	apcm8 = agmv->audio_track->pcm8;
	
	if(bps == 16){
		u16* pcm16 = (u16*)pcm;
		
		AGMV_Memcpy16(apcm,pcm16,audio_size);
	}
	else{
		u8* pcm8 = (u8*)pcm;
		
		AGMV_Memcpy8(apcm8,pcm8,audio_size);
	}
}

void AGMV_SignedToUnsignedPCM(u8* pcm, u32 size){
	int i;
	for(i = 0; i < size; i++){
		pcm[i] = pcm[i] + 128;
	}
}

void AGMV_UnsignedToSignedPCM(u8* pcm, u32 size){
	int i;
	for(i = 0; i < size; i++){
		pcm[i] = pcm[i] - 128;
	}
}

int AGMV_Min(int a, int b){
	if(a < b){
		return a;
	}
	else return b;
}

u8 AGMV_GetQuantizedR(u32 color, AGMV_QUALITY quality){
	switch(quality){
		case AGMV_HIGH_QUALITY:{
			return (color & 0x1f000) >> 12;
		}
		case AGMV_MID_QUALITY:{
			return (color & 0xf800) >> 11;
		}
		case AGMV_LOW_QUALITY:{
			return (color & 0x7c00) >> 10;
		}
		default:{
			return (color & 0x7c00) >> 10;
		}
	}
}

u8 AGMV_GetQuantizedG(u32 color, AGMV_QUALITY quality){
	switch(quality){
		case AGMV_HIGH_QUALITY:{
			return (color & 0xfc0) >> 6;
		}
		case AGMV_MID_QUALITY:{
			return (color & 0x7e0) >> 5;
		}
		case AGMV_LOW_QUALITY:{
			return (color & 0x3e0) >> 5;
		}
		default:{
			return (color & 0x3e0) >> 5;
		}
	}
}

u8 AGMV_GetQuantizedB(u32 color, AGMV_QUALITY quality){
	switch(quality){
		case AGMV_HIGH_QUALITY:{
			return (color & 0x3f);
		}
		case AGMV_MID_QUALITY:{
			return (color & 0x001F);
		}
		case AGMV_LOW_QUALITY:{
			return (color & 0x001F);
		}
		default:{
			return (color & 0x001F);
		}
	}
}

u32 AGMV_QuantizeColor(u32 color, AGMV_QUALITY quality){
	switch(quality){
		case AGMV_HIGH_QUALITY:{
			u8 r = AGMV_GetR(color);
			u8 g = AGMV_GetG(color);
			u8 b = AGMV_GetB(color);
			
			r >>= 3;
			g >>= 2;
			b >>= 2;
			
			return r << 12 | g << 6 | b;
		}
		case AGMV_MID_QUALITY:{
			u8 r = AGMV_GetR(color);
			u8 g = AGMV_GetG(color);
			u8 b = AGMV_GetB(color);
			
			r >>= 3;
			g >>= 2;
			b >>= 3;
			
			return r << 11 | g << 5 | b;
		}
		case AGMV_LOW_QUALITY:{
			u8 r = AGMV_GetR(color);
			u8 g = AGMV_GetG(color);
			u8 b = AGMV_GetB(color);
			
			r >>= 3;
			g >>= 3;
			b >>= 3;
			
			return r << 10 | g << 5 | b;
		}
		default:{
			u8 r = AGMV_GetR(color);
			u8 g = AGMV_GetG(color);
			u8 b = AGMV_GetB(color);
			
			r >>= 3;
			g >>= 3;
			b >>= 3;
			
			return r << 10 | g << 5 | b;
		}
	}
}

u32 AGMV_ReverseQuantizeColor(u32 color, AGMV_QUALITY quality){
	int r = AGMV_GetQuantizedR(color,quality);
	int g = AGMV_GetQuantizedG(color,quality);
	int b = AGMV_GetQuantizedB(color,quality);
	
	switch(quality){
		case AGMV_HIGH_QUALITY:{
			
			r <<= 3;
			g <<= 2;
			b <<= 2;
			
			return r << 16 | g << 8 | b;
		}
		case AGMV_MID_QUALITY:{
			
			r <<= 3;
			g <<= 2;
			b <<= 3;
			
			return r << 16 | g << 8 | b;
		}
		case AGMV_LOW_QUALITY:{
			
			r <<= 3;
			g <<= 3;
			b <<= 3;
			
			return r << 16 | g << 8 | b;
		}
		default:{
			
			r <<= 3;
			g <<= 3;
			b <<= 3;
			
			return r << 16 | g << 8 | b;
		}
	}
}

u32 AGMV_InterpColor(u32 color1, u32 color2){
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
	
	return r << 16 | g << 8 | b;
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

	for(i = 0; i < 228; i++){
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

u8 AGMV_FindColorFromRange(u32 palette[256], u32 start, u32 end, u32 color){
	int i, rdiff, gdiff, bdiff, r, g, b;
	u32 min, dist;
	u8 pr, pg, pb, index;

	r = AGMV_GetR(color);
	g = AGMV_GetG(color);
	b = AGMV_GetB(color);
	
	min = 255*255 + 255*255 + 255*255 + 1;
	index = 0;

	for(i = start; i < end; i++){
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

AGMV_ENTRY AGMV_FindEntryFromRange(u32 palette0[256], u32 palette1[256], u32 start, u32 end, u32 color){
	AGMV_ENTRY entry;
	u8 index1, index2;

	index1 = AGMV_FindColorFromRange(palette0,start,end,color);
	index2 = AGMV_FindColorFromRange(palette1,start,end,color);

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

u32 AGMV_GetGrayscaleIntensity(u32 clr){
	u8 r = AGMV_GetR(clr);
	u8 g = AGMV_GetG(clr);
	u8 b = AGMV_GetB(clr);
	
	u8 rgb = (u8)((r+g+b)/3.0f);

	return rgb;
}

f32 AGMV_CompareFrameSimilarity(u32* frame1, u32* frame2, u32 width, u32 height){
	u32 total_num_of_pixels = width*height, count = 0;
	u32 color1, color2;
	int gray1, gray2;
	
	int i;
	for(i = 0; i < total_num_of_pixels; i++){
		color1 = *(frame1++);
		color2 = *(frame2++);
		
		gray1 = AGMV_GetGrayscaleIntensity(color1);
		gray2 = AGMV_GetGrayscaleIntensity(color2);
		
		if(gray1-gray2 <= 3){
			count++;
		}
	}
	
	return count/(f32)total_num_of_pixels;
}

void AGMV_InterpFrame(u32* interp, u32* frame1, u32* frame2, u32 width, u32 height){
	u32 color1, color2;
	int r, g, b, r1, g1, b1, r2, g2, b2;
	int i, size = width*height;
	for(i = 0; i < size; i++){
		color1 = *(frame1++);
		color2 = *(frame2++);
		
		r1 = AGMV_GetR(color1);
		g1 = AGMV_GetG(color1);
		b1 = AGMV_GetB(color1);
		
		r2 = AGMV_GetR(color2);
		g2 = AGMV_GetG(color2);
		b2 = AGMV_GetB(color2);
		
		r = (r1 + r2) >> 1;
		g = (g1 + g2) >> 1;
		b = (b1 + b2) >> 1;
		
		*(interp++) = r << 16 | g << 8 | b;
	}
}

void AGMV_ConstructFrameFromEntries(AGMV* agmv){
	AGMV_ENTRY* img_entry = agmv->iframe_entries, entry;
	AGMV_OPT opt = AGMV_GetOPT(agmv);
	u32 i, width, height, *iframe_data = agmv->iframe->img_data, color, size;
	u32* palette0, *palette1;

	width = agmv->frame->width;
	height = agmv->frame->height;
	
	palette0 = agmv->header.palette0;
	palette1 = agmv->header.palette1;
	
	size = width*height;
	
	if(opt != AGMV_OPT_II && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_II){
		for(i = 0; i < size; i++){
			entry = *(img_entry++);

			if(entry.pal_num == 0){
				color = palette0[entry.index];
			}
			else{
				color = palette1[entry.index];
			}
			
			*(iframe_data++) = color;
		}
	}
	else{
		for(i = 0; i < size; i++){
			entry = *(img_entry++);
			*(iframe_data++) = palette0[entry.index];
		}
	}
}

void AGMV_BubbleSort(u32* data, u32* gram, u32 num_of_colors){
	int i,j;
	for(j = 0; j < num_of_colors - 1; j++){
		for(i = 0; i < num_of_colors - 1; i++){
			if (data[i] > data[i + 1]) {
				u32 temp = data[i];
				data[i] = data[i + 1];
				data[i + 1] = temp;

				u32 tempc = gram[i];
				gram[i] = gram[i + 1];
				gram[i + 1] = tempc;
			}
		}
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
	u32 riff, chunk_size, wave, fmt, sub_chunk_size, audio_fmt, num_of_channels, sample_rate, byte_rate, block_align, bits_per_sample, data_chunk, sub_chunk_size2;
	
	agmv->audio_type = AGMV_AUDIO_WAV;
	agmv->audio = fopen(filename,"rb");
			
	riff = AGMV_ReadLong(agmv->audio);
	chunk_size = AGMV_ReadLong(agmv->audio);
	wave = AGMV_ReadLong(agmv->audio);
	fmt = AGMV_ReadLong(agmv->audio);
	sub_chunk_size = AGMV_ReadLong(agmv->audio);
	audio_fmt = AGMV_ReadShort(agmv->audio);
	num_of_channels = AGMV_ReadShort(agmv->audio);
	sample_rate = AGMV_ReadLong(agmv->audio);
	byte_rate = AGMV_ReadLong(agmv->audio);
	block_align = AGMV_ReadShort(agmv->audio);
	bits_per_sample = AGMV_ReadShort(agmv->audio);
	data_chunk = AGMV_ReadLong(agmv->audio);
	sub_chunk_size2 = AGMV_ReadLong(agmv->audio);
	
	if(bits_per_sample == 16){
		AGMV_SetBitsPerSample(agmv,16);
	}
	else{
		AGMV_SetBitsPerSample(agmv,8);
	}
	
	AGMV_SetTotalAudioDuration(agmv,chunk_size/(sample_rate*num_of_channels*(bits_per_sample/8)));
	AGMV_SetSampleRate(agmv,sample_rate);
	AGMV_SetNumberOfChannels(agmv,num_of_channels);
	
	if(bits_per_sample == 16){
		AGMV_SetAudioSize(agmv,chunk_size/2);
	}
	else{
		AGMV_SetAudioSize(agmv,chunk_size);
	}
}

void AGMV_RawSignedPCMToAudioTrack(const char* filename, AGMV* agmv, u8 num_of_channels, u32 sample_rate){
	u32 file_size;
	
	agmv->audio_type = 12;
	agmv->audio = fopen(filename,"rb");
	
	fseek(agmv->audio,0,SEEK_END);
	file_size = ftell(agmv->audio);
	fseek(agmv->audio,0,SEEK_SET);
	
	AGMV_SetAudioSize(agmv,file_size);
	AGMV_SetSampleRate(agmv,sample_rate);
	AGMV_SetNumberOfChannels(agmv,num_of_channels);
	AGMV_SetBitsPerSample(agmv,8);
	AGMV_SetTotalAudioDuration(agmv,file_size/(sample_rate*num_of_channels));

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
	else if(buf[0] == 0x40 && buf[1] == 0x0C && buf[2] == 0xFA && buf[3] == 0x00){
		return 16000;
	}
	
	int freq = 0;

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
	u16 num_of_channels, sample_size;
	u32 size, num_of_samples, sample_rate;
	
	agmv->audio_type = AGMV_AUDIO_AIFC;
	agmv->audio = fopen(filename,"rb");
	
	if(agmv->audio != NULL){
		fseek(agmv->audio,50,SEEK_SET);
		AGMV_ReadFourCC(agmv->audio,fourcc);
		fseek(agmv->audio,32,SEEK_SET);
		
		if(AGMV_IsCorrectFourCC(fourcc,'N','O','N','E')){
			agmv->sowt = FALSE;
			num_of_channels = AGMV_SwapShort(AGMV_ReadShort(agmv->audio));
			num_of_samples  = AGMV_SwapLong(AGMV_ReadLong(agmv->audio));
			sample_size     = AGMV_SwapShort(AGMV_ReadShort(agmv->audio));
			sample_rate     = AGMV_80BitFloat(agmv->audio);
			
			fseek(agmv->audio,10,SEEK_CUR);
			size = AGMV_SwapLong(AGMV_ReadLong(agmv->audio));
			fseek(agmv->audio,8,SEEK_CUR);

			AGMV_SetTotalAudioDuration(agmv,(size)/(sample_rate*num_of_channels*(sample_size/8)));
			AGMV_SetSampleRate(agmv,sample_rate);
			AGMV_SetNumberOfChannels(agmv,num_of_channels);
			
			if(sample_size == 16){
				AGMV_SetBitsPerSample(agmv,16);
				AGMV_SetAudioSize(agmv,size/2);
			}
			else{
				AGMV_SetBitsPerSample(agmv,8);
				AGMV_SetAudioSize(agmv,size);
			}
		}
		else if(AGMV_IsCorrectFourCC(fourcc,'s','o','w','t')){
			agmv->sowt = TRUE;
			num_of_channels = AGMV_SwapShort(AGMV_ReadShort(agmv->audio));
			num_of_samples  = AGMV_SwapLong(AGMV_ReadLong(agmv->audio));
			sample_size     = AGMV_SwapShort(AGMV_ReadShort(agmv->audio));
			sample_rate     = AGMV_80BitFloat(agmv->audio);
			
			fseek(agmv->audio,10,SEEK_CUR);
			size = AGMV_SwapLong(AGMV_ReadLong(agmv->audio));
			fseek(agmv->audio,8,SEEK_CUR);

			AGMV_SetTotalAudioDuration(agmv,(size)/(sample_rate*num_of_channels*(sample_size/8)));
			AGMV_SetSampleRate(agmv,sample_rate);
			AGMV_SetNumberOfChannels(agmv,num_of_channels);
			
			if(sample_size == 16){
				AGMV_SetBitsPerSample(agmv,16);
				AGMV_SetAudioSize(agmv,size/2);
			}
			else{
				AGMV_SetBitsPerSample(agmv,8);
				AGMV_SetAudioSize(agmv,size);
			}
		}
		else{
			
		}
	}
}

void AGMV_AIFFToAudioTrack(const char* filename, AGMV* agmv){
	u16 num_of_channels, sample_size;
	u32 size, num_of_samples, sample_rate;
	
	agmv->audio_type = AGMV_AUDIO_AIFF;
	agmv->audio = fopen(filename,"rb");
	
	if(agmv->audio != NULL){
		fseek(agmv->audio,20,SEEK_SET);
		
		num_of_channels = AGMV_SwapShort(AGMV_ReadShort(agmv->audio));
		num_of_samples = AGMV_SwapLong(AGMV_ReadLong(agmv->audio));
		sample_size = AGMV_SwapShort(AGMV_ReadShort(agmv->audio));
		sample_rate = AGMV_80BitFloat(agmv->audio);

		fseek(agmv->audio,4,SEEK_CUR);
		size = AGMV_SwapLong(AGMV_ReadLong(agmv->audio));
		fseek(agmv->audio,8,SEEK_CUR);

		AGMV_SetTotalAudioDuration(agmv,(size/2)/(sample_rate*num_of_channels*(sample_size/8)));
		AGMV_SetSampleRate(agmv,sample_rate);
		AGMV_SetNumberOfChannels(agmv,num_of_channels);
		
		if(sample_size == 16){
			AGMV_SetBitsPerSample(agmv,16);
			AGMV_SetAudioSize(agmv,size/2);
		}
		else{
			AGMV_SetBitsPerSample(agmv,8);
			AGMV_SetAudioSize(agmv,size);
		}
	}
}

void AGMV_Raw8PCMToAudioTrack(const char* filename, AGMV* agmv){
	u32 file_size;
	
	agmv->audio_type = 12;
	agmv->audio = fopen(filename,"rb");
	fseek(agmv->audio,0,SEEK_END);
	file_size = ftell(agmv->audio);
	fseek(agmv->audio,0,SEEK_SET);
	
	AGMV_SetTotalAudioDuration(agmv,file_size/(16000));
	AGMV_SetSampleRate(agmv,16000);
	AGMV_SetNumberOfChannels(agmv,1);
	AGMV_SetAudioSize(agmv,file_size);
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
	info.bits_per_sample = AGMV_GetBitsPerSample(agmv);
	
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
			
			if(agmv->header.bits_per_sample == 16){
				AGMV_WriteLong(audio,agmv->header.audio_size*2);
			}
			else{
				AGMV_WriteLong(audio,agmv->header.audio_size);
			}
			
			AGMV_WriteFourCC(audio,'W','A','V','E');
			AGMV_WriteFourCC(audio,'f','m','t',0x20);
			AGMV_WriteLong(audio,16);
			AGMV_WriteShort(audio,1);
			AGMV_WriteShort(audio,agmv->header.num_of_channels);
			AGMV_WriteLong(audio,agmv->header.sample_rate);
			AGMV_WriteLong(audio,75600);
			AGMV_WriteShort(audio,(agmv->header.num_of_channels * agmv->header.bits_per_sample) / 8);
			AGMV_WriteShort(audio,agmv->header.bits_per_sample);
			AGMV_WriteFourCC(audio,'d','a','t','a');
		}break;
		case AGMV_AUDIO_AIFF:{
			AGMV_WriteFourCC(audio,'F','O','R','M');
			
			if(AGMV_GetBitsPerSample(agmv) == 16){
				AGMV_WriteLong(audio,AGMV_SwapLong((agmv->header.audio_size*2)+32));
			}
			else{
				AGMV_WriteLong(audio,AGMV_SwapLong((agmv->header.audio_size)+32));
			}
			
			AGMV_WriteFourCC(audio,'A','I','F','C');
			AGMV_WriteFourCC(audio,'F','V','E','R');
			AGMV_WriteLong(audio,AGMV_SwapLong(4));
			AGMV_WriteFourCC(audio,0xA2,0x80,0x51,0x40);
			AGMV_WriteFourCC(audio,'C','O','M','M');
			AGMV_WriteLong(audio,AGMV_SwapLong(26));
			AGMV_WriteShort(audio,AGMV_SwapShort(AGMV_GetNumberOfChannels(agmv)));
			AGMV_WriteLong(audio,AGMV_SwapLong((AGMV_GetAudioSize(agmv))/AGMV_GetNumberOfChannels(agmv)));
			AGMV_WriteShort(audio,AGMV_SwapShort(AGMV_GetBitsPerSample(agmv)));
			
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
			
			if(AGMV_GetBitsPerSample(agmv) == 16){
				AGMV_WriteLong(audio,AGMV_SwapLong(AGMV_GetAudioSize(agmv)*2));
			}
			else{
				AGMV_WriteLong(audio,AGMV_SwapLong(AGMV_GetAudioSize(agmv)));
			}
			
			AGMV_WriteLong(audio,0);
			AGMV_WriteLong(audio,0);
		}break;
		case AGMV_AUDIO_AIFC:{
			AGMV_WriteFourCC(audio,'F','O','R','M');
			
			if(AGMV_GetBitsPerSample(agmv) == 16){
				AGMV_WriteLong(audio,AGMV_SwapLong((agmv->header.audio_size*2)+32));
			}
			else{
				AGMV_WriteLong(audio,AGMV_SwapLong((agmv->header.audio_size)+32));
			}
			
			AGMV_WriteFourCC(audio,'A','I','F','F');
			AGMV_WriteFourCC(audio,'C','O','M','M');
			AGMV_WriteLong(audio,AGMV_SwapLong(18));
			AGMV_WriteShort(audio,AGMV_SwapShort(AGMV_GetNumberOfChannels(agmv)));
			AGMV_WriteLong(audio,AGMV_SwapLong((AGMV_GetAudioSize(agmv))/AGMV_GetNumberOfChannels(agmv)));
			AGMV_WriteShort(audio,AGMV_SwapShort(AGMV_GetBitsPerSample(agmv)));
			
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
			
			if(AGMV_GetBitsPerSample(agmv) == 16){
				AGMV_WriteLong(audio,AGMV_SwapLong(AGMV_GetAudioSize(agmv)*2));
			}
			else{
				AGMV_WriteLong(audio,AGMV_SwapLong(AGMV_GetAudioSize(agmv)));
			}
			
			AGMV_WriteLong(audio,0);
			AGMV_WriteLong(audio,0);
		}break;
		default:{
			AGMV_WriteFourCC(audio,'R','I','F','F');
			
			if(agmv->header.bits_per_sample == 16){
				AGMV_WriteLong(audio,agmv->header.audio_size*2);
			}
			else{
				AGMV_WriteLong(audio,agmv->header.audio_size);
			}
			
			AGMV_WriteFourCC(audio,'W','A','V','E');
			AGMV_WriteFourCC(audio,'f','m','t',0x20);
			AGMV_WriteLong(audio,16);
			AGMV_WriteShort(audio,1);
			AGMV_WriteShort(audio,agmv->header.num_of_channels);
			AGMV_WriteLong(audio,agmv->header.sample_rate);
			AGMV_WriteLong(audio,75600);
			AGMV_WriteShort(audio,(agmv->header.num_of_channels * agmv->header.bits_per_sample) / 8);
			AGMV_WriteShort(audio,agmv->header.bits_per_sample);
			AGMV_WriteFourCC(audio,'d','a','t','a');
		}break;
	}
}

void AGMV_ExportAudio(FILE* audio, AGMV* agmv, AGMV_AUDIO_TYPE audio_type){
	int i;
	u8 bytes[10];
	switch(audio_type){
		case AGMV_AUDIO_WAV:{
			if(agmv->header.bits_per_sample == 16){
				fwrite(agmv->audio_track->pcm,2,agmv->audio_chunk->size,audio);
			}
			else{
				AGMV_WriteLong(audio,agmv->header.audio_size);
				fwrite(agmv->audio_track->pcm8,1,agmv->audio_chunk->size,audio);
			}
		}break;
		case AGMV_AUDIO_AIFF:{		
			if(AGMV_GetBitsPerSample(agmv) == 16){
				for(i = 0; i < agmv->audio_chunk->size; i++){
					AGMV_WriteShort(audio,AGMV_SwapShort(agmv->audio_track->pcm[i]));
				}
			}
			else{
				AGMV_UnsignedToSignedPCM(agmv->audio_track->pcm8,agmv->audio_chunk->size);
				fwrite(agmv->audio_track->pcm8,1,agmv->audio_chunk->size,audio);
				AGMV_SignedToUnsignedPCM(agmv->audio_track->pcm8,agmv->audio_chunk->size);
			}
		}break;
		case AGMV_AUDIO_AIFC:{		
			if(AGMV_GetBitsPerSample(agmv) == 16){
				for(i = 0; i < agmv->audio_chunk->size; i++){
					AGMV_WriteShort(audio,AGMV_SwapShort(agmv->audio_track->pcm[i]));
				}
			}
			else{
				AGMV_UnsignedToSignedPCM(agmv->audio_track->pcm8,agmv->audio_chunk->size);
				fwrite(agmv->audio_track->pcm8,1,agmv->audio_chunk->size,audio);
				AGMV_SignedToUnsignedPCM(agmv->audio_track->pcm8,agmv->audio_chunk->size);
			}
		}break;
		default:{
			if(agmv->header.bits_per_sample == 16){
				fwrite(agmv->audio_track->pcm,2,agmv->audio_chunk->size,audio);
			}
			else{
				AGMV_WriteLong(audio,agmv->header.audio_size);
				fwrite(agmv->audio_track->pcm8,1,agmv->audio_chunk->size,audio);
			}
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