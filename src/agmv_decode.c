/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_decode.c
*   Date: 5/17/2024
*   Version: 1.0
*   Updated: 6/7/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agmv_decode.h>

#include <agidl.h>
#include <stdlib.h>

#include <agmv_utils.h>

int AGMV_DecodeHeader(FILE* file, AGMV* agmv){
	
	int i;
	
	AGMV_ReadFourCC(file,agmv->header.fourcc);
	agmv->header.num_of_frames = AGMV_ReadLong(file);
	agmv->header.width = AGMV_ReadLong(file);
	agmv->header.height = AGMV_ReadLong(file);
	
	agmv->header.fmt = AGIDL_ReadByte(file);
	agmv->header.version = AGIDL_ReadByte(file);
	agmv->header.frames_per_second = AGIDL_ReadLong(file);
	
	agmv->header.total_audio_duration = AGIDL_ReadLong(file);
	agmv->header.sample_rate = AGIDL_ReadLong(file);
	agmv->header.audio_size = AGIDL_ReadLong(file);
	agmv->header.num_of_channels = AGIDL_ReadShort(file);
	
	if(!AGMV_IsCorrectFourCC(agmv->header.fourcc,'A','G','M','V') || !(agmv->header.version == 1 || agmv->header.version == 2 || agmv->header.version == 3 || agmv->header.version == 4) || agmv->header.frames_per_second >= 200){
		return INVALID_HEADER_FORMATTING_ERR;
	}
	
	if(agmv->header.version == 1 || agmv->header.version == 3){	
		for(i = 0; i < 256; i++){
			const u8 r = AGMV_ReadByte(file);
			const u8 g = AGMV_ReadByte(file);
			const u8 b = AGMV_ReadByte(file);
			
			agmv->header.palette0[i] = AGIDL_RGB(r,g,b,agmv->header.fmt);
		}
		
		for(i = 0; i < 256; i++){
			const u8 r = AGMV_ReadByte(file);
			const u8 g = AGMV_ReadByte(file);
			const u8 b = AGMV_ReadByte(file);
			
			agmv->header.palette1[i] = AGIDL_RGB(r,g,b,agmv->header.fmt);
		}
	}
	else{
		for(i = 0; i < 256; i++){
			const u8 r = AGMV_ReadByte(file);
			const u8 g = AGMV_ReadByte(file);
			const u8 b = AGMV_ReadByte(file);
			
			agmv->header.palette0[i] = AGIDL_RGB(r,g,b,agmv->header.fmt);
		}
	}
	
	return NO_ERR;
}

int AGMV_DecodeFrameChunk(FILE* file, AGMV* agmv){
	u32 i, bitpos = 0, num_of_bits, width, height, bpos, csize, indice;
	u32* img_data = agmv->frame->img_data, *iframe_data = agmv->iframe->img_data, color, offset;
	u8 byte, len, index, *bitstream_data = agmv->bitstream->data;
	Bool escape = FALSE;

	agmv->bitstream->pos = 0;
	
	AGMV_ReadFourCC(file,agmv->frame_chunk->fourcc);
	agmv->frame_chunk->frame_num = AGMV_ReadLong(file);
	agmv->frame_chunk->uncompressed_size = AGMV_ReadLong(file);
	agmv->frame_chunk->compressed_size = AGMV_ReadLong(file);
	
	width  = agmv->frame->width;
	height = agmv->frame->height;

	csize = agmv->frame_chunk->compressed_size;
	
	num_of_bits = agmv->frame_chunk->compressed_size * 8;
	bpos = agmv->bitstream->pos;
	
	if(!AGMV_IsCorrectFourCC(agmv->frame_chunk->fourcc,'A','G','F','C')){
		return INVALID_HEADER_FORMATTING_ERR;
	}
	
	if(agmv->header.version == 1 || agmv->header.version == 2) {
		u32 bits = 0;
		u8 org;
		for(bits = 0; bits <= num_of_bits;){

			byte = AGMV_ReadBits(file,1); bits++;
			
			if(byte & 1){
				bitstream_data[bpos++] = AGMV_ReadBits(file,8);
				bits += 8;
			}
			else{
				offset = AGMV_ReadBits(file,16);
				len = AGMV_ReadBits(file,4);
				
				org = len;
				
				bits += 20;
				
				u8 *dest = &bitstream_data[bpos];
				u8 *src = dest - offset;

				do {
					*dest++ = *src++;
				}while(len--);
				
				bpos += org;
			}
		}
	}
	else {
		u32 pos;
		for(i = 0; i < csize; i += 4){
			
			offset = AGMV_ReadShort(file);
			len = AGMV_ReadByte(file);	
			byte = AGMV_ReadByte(file);
			
			pos = bpos;

			int j;
			for(j = 0; j < len; j++){
				bitstream_data[bpos++] = bitstream_data[pos - offset + j];
			}
			
			bitstream_data[bpos++] = byte;
		}
	}
	
	agmv->bitstream->pos = bpos;

	AGMV_FlushReadBits();
	
	if(agmv->header.version == 1 || agmv->header.version == 3){
		u32*palette;
		u8 fbit;
		u8 bot;
		int x,y;
		for(y = 0; y < height && escape != TRUE; y += 4){
			for(x = 0; x < width && escape != TRUE; x += 4){
				
				if(bitpos > agmv->bitstream->pos){
					escape = TRUE;
					break;
				}

				byte = bitstream_data[bitpos++];
				
				if(byte == AGMV_FILL_FLAG){
					index = bitstream_data[bitpos++]; 
					fbit = (index >> 7) & 1;
					bot = (index & 0x7f);
					
					if(bitpos > agmv->bitstream->pos){
						escape = TRUE;
						break;
					}
					
					palette = fbit ? agmv->header.palette1 : agmv->header.palette0;

					if(bot < 127){
						color = palette[bot];
					}
					else{
						index = bitstream_data[bitpos++];
						color = palette[index];
					}
					
					int k,j;
					for(j = 0; j < 4; j++){
						offset = (y+j)*width;
						for(k = 0; k < 4; k++){
							img_data[x+k+offset] = color;
						}
					}
				}
				else if(byte == AGMV_COPY_FLAG){
					int k,j;
					for(j = 0; j < 4; j++){
						offset = (y+j)*width;
						for(k = 0; k < 4; k++){
							indice = x+k+offset;
							img_data[indice] = iframe_data[indice];
						}
					}
				}
				else{
					int k,j;
					for(j = 0; j < 4; j++){
						offset = (y+j)*width;
						for(k = 0; k < 4; k++){
							if(bitpos > agmv->bitstream->pos){
								escape = TRUE;
								break;
							}
							index = bitstream_data[bitpos++];
							fbit = (index >> 7) & 1;
							bot = (index & 0x7f);
							palette = fbit ? agmv->header.palette1 : agmv->header.palette0;

							if(bot < 127){
								color = palette[bot];
							}
							else{
								index = bitstream_data[bitpos++];
								color = palette[index];
							}
							img_data[x+k+offset] = color;
						}
					}
				}
			}
		}
	}
	else{
		u32* palette0 = agmv->header.palette0;
		
		int x,y;
		for(y = 0; y < height && escape != TRUE; y += 4){
			for(x = 0; x < width && escape != TRUE; x += 4){
				byte = bitstream_data[bitpos++];
				
				if(bitpos > agmv->bitstream->pos){
					escape = TRUE;
					break;
				}
				
				if(byte == AGMV_FILL_FLAG){
					index = bitstream_data[bitpos++];
					color = palette0[index];
					
					if(bitpos > agmv->bitstream->pos){
						escape = TRUE;
						break;
					}
				}
				else if(byte == AGMV_COPY_FLAG){
					int k,j;
					for(j = 0; j < 4; j++){
						offset = (y+j)*width;
						for(k = 0; k < 4; k++){
							indice = x+k+offset;
							img_data[indice] = iframe_data[indice];
						}
					}
				}
				else{
					int k,j;
					for(j = 0; j < 4; j++){
						offset = (y+j)*width;
						for(k = 0; k < 4; k++){
							index = bitstream_data[bitpos++];
							if(bitpos > agmv->bitstream->pos){
								escape = TRUE;
								break;
							}
							color = palette0[index];
							if(escape == FALSE){
								img_data[x+k+offset] = color;
							}
						}
					}
				}
			}
		}
	}
	
	if(agmv->frame_count % 4 == 0){
		for(i = 0; i < width*height; i++){
			iframe_data[i] = img_data[i];
		}
	}
	
	agmv->frame_count++;

	return NO_ERR;
}

int AGMV_DecodeAudioChunk(FILE* file, const AGMV* agmv){
	u16*pcm = agmv->audio_track->pcm;
	
	AGMV_ReadFourCC(file,agmv->audio_chunk->fourcc);
	agmv->audio_chunk->size = AGIDL_ReadLong(file);
	
	if(!AGMV_IsCorrectFourCC(agmv->audio_chunk->fourcc,'A','G','A','C')){
		return INVALID_HEADER_FORMATTING_ERR;
	}

	const u32 size = agmv->audio_chunk->size;
	u32 start_point = agmv->audio_track->start_point;

	for(int i = 0; i < size; i++){
		const u16 sample = AGIDL_ReadByte(file);
		u16 resample = 0;
		
		if(sample % 2 == 0){
			resample = sample * sample;
		}
		else{
			resample = sample << 8;
		}
		
		pcm[start_point++] = resample; 
	}
	
	agmv->audio_track->start_point = start_point;
	
	return NO_ERR;
}

int AGMV_DecodeGBAAudioChunk(FILE* file, const AGMV* agmv){
	s8* satsample = agmv->audio_chunk->satsample;
	
	AGMV_ReadFourCC(file,agmv->audio_chunk->fourcc);
	agmv->audio_chunk->size = AGIDL_ReadLong(file);
	
	if(!AGMV_IsCorrectFourCC(agmv->audio_chunk->fourcc,'A','G','A','C')){
		return INVALID_HEADER_FORMATTING_ERR;
	}

	const u32 size = agmv->audio_chunk->size;

	for(int i = 0; i < size; i++){
		const s8 sample = AGIDL_ReadByte(file);
		satsample[i] = sample; 
	}
	
	return NO_ERR;
}

int AGMV_DecodeVideo(const char* filename, const u8 img_type){
	AGMV* agmv = malloc(sizeof(AGMV));
	agmv->frame_chunk = (AGMV_FRAME_CHUNK*)malloc(sizeof(AGMV_FRAME_CHUNK));
	agmv->audio_chunk = (AGMV_AUDIO_CHUNK*)malloc(sizeof(AGMV_AUDIO_CHUNK));
	agmv->bitstream = (AGMV_BITSTREAM*)malloc(sizeof(AGMV_BITSTREAM));
	agmv->bitstream->pos = 0;
	agmv->frame = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->iframe = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->audio_track = (AGMV_AUDIO_TRACK*)malloc(sizeof(AGMV_AUDIO_TRACK));
	
	FILE* file = fopen(filename,"rb");
	
	if(file == NULL){
		fclose(file);
		DestroyAGMV(agmv);
		return FILE_NOT_FOUND_ERR;
	}

	const int err = AGMV_DecodeHeader(file, agmv);
	
	agmv->frame->width = agmv->header.width;
	agmv->frame->height = agmv->header.height;
	agmv->frame->img_data = (u32*)malloc(sizeof(u32)*agmv->frame->width*agmv->frame->height);
	
	agmv->iframe->width = agmv->header.width;
	agmv->iframe->height = agmv->header.height;
	agmv->iframe->img_data = (u32*)malloc(sizeof(u32)*agmv->frame->width*agmv->frame->height);
	
	agmv->bitstream->len = agmv->header.width*agmv->header.height*2;
	agmv->bitstream->data = (u8*)malloc(sizeof(u8)*agmv->bitstream->len);

	const int num_of_frames = AGMV_GetNumberOfFrames(agmv);
	
	if(err != NO_ERR){
		fclose(file);
		DestroyAGMV(agmv);
		return err;
	}
	for(int i = 0; i < num_of_frames; i++){
		AGMV_FindNextFrameChunk(file);
		const int err1 = AGMV_DecodeFrameChunk(file, agmv);
				
		if(err1 != NO_ERR){
			fclose(file);
			DestroyAGMV(agmv);
			return err1;
		}
			
		AGIDL_QuickExport(agmv->frame->img_data,agmv->frame->width,agmv->frame->height,AGIDL_RGB_888,img_type);
	}

	fclose(file);
	DestroyAGMV(agmv);
	
	return NO_ERR;
}

int AGMV_DecodeAGMV(const char* filename, const u8 img_type, const AGMV_AUDIO_TYPE audio_type){
	int err1, i;
	
	AGMV* agmv = malloc(sizeof(AGMV));
	agmv->frame_chunk = (AGMV_FRAME_CHUNK*)malloc(sizeof(AGMV_FRAME_CHUNK));
	agmv->audio_chunk = (AGMV_AUDIO_CHUNK*)malloc(sizeof(AGMV_AUDIO_CHUNK));
	agmv->bitstream = (AGMV_BITSTREAM*)malloc(sizeof(AGMV_BITSTREAM));
	agmv->bitstream->pos = 0;
	agmv->frame = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->iframe = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->audio_track = (AGMV_AUDIO_TRACK*)malloc(sizeof(AGMV_AUDIO_TRACK));
	
	FILE* file = fopen(filename, "rb");
	
	if(file == NULL){
		fclose(file);
		DestroyAGMV(agmv);
		return FILE_NOT_FOUND_ERR;
	}

	const int err = AGMV_DecodeHeader(file, agmv);
	
	agmv->frame->width = agmv->header.width;
	agmv->frame->height = agmv->header.height;
	agmv->frame->img_data = (u32*)malloc(sizeof(u32)*agmv->frame->width*agmv->frame->height);
	
	agmv->iframe->width = agmv->header.width;
	agmv->iframe->height = agmv->header.height;
	agmv->iframe->img_data = (u32*)malloc(sizeof(u32)*agmv->frame->width*agmv->frame->height);
	
	agmv->bitstream->len = agmv->header.width*agmv->header.height*2;
	agmv->bitstream->data = (u8*)malloc(sizeof(u8)*agmv->bitstream->len);

	const int num_of_frames = AGMV_GetNumberOfFrames(agmv);
	
	if(err != NO_ERR){
		fclose(file);
		DestroyAGMV(agmv);
		return err;
	}
	if(agmv->header.total_audio_duration != 0){
		FILE*audio;
		agmv->audio_track->start_point = 0;
		agmv->audio_track->pcm = (u16*)malloc(sizeof(u16)*agmv->header.audio_size);

		agmv->audio_chunk->size = agmv->header.audio_size / (f32)agmv->header.num_of_frames;

		for(i = 0; i < num_of_frames; i++){
			AGMV_FindNextFrameChunk(file);
			err1 = AGMV_DecodeFrameChunk(file,agmv);
			AGMV_FindNextAudioChunk(file);
			const int err2 = AGMV_DecodeAudioChunk(file, agmv);

			if(err1 != NO_ERR){
				fclose(file);
				DestroyAGMV(agmv);
				return err1;
			}

			if(err2 != NO_ERR){
				fclose(file);
				DestroyAGMV(agmv);
				return err1;
			}

			AGIDL_QuickExport(agmv->frame->img_data,agmv->frame->width,agmv->frame->height,AGIDL_RGB_888,img_type);
		}

		switch(audio_type){
			case AGMV_AUDIO_WAV:{
				audio = fopen("quick_export.wav","wb");
				AGMV_ExportAudioType(audio,agmv,AGMV_AUDIO_WAV);
				fclose(audio);
			}break;
			case AGMV_AUDIO_AIFC:{
				audio = fopen("quick_export.aiff","wb");
				AGMV_ExportAudioType(audio,agmv,AGMV_AUDIO_AIFC);
				fclose(audio);
			}break;
			case AGMV_AUDIO_AIFF:{
				audio = fopen("quick_export.aiff","wb");
				AGMV_ExportAudioType(audio,agmv,AGMV_AUDIO_AIFF);
				fclose(audio);
			}break;
			default:{
				audio = fopen("quick_export.wav","wb");
				AGMV_ExportAudioType(audio,agmv,AGMV_AUDIO_WAV);
				fclose(audio);
			}break;
		}
	}
	else{
		for(i = 0; i < num_of_frames; i++){
			AGMV_FindNextFrameChunk(file);
			err1 = AGMV_DecodeFrameChunk(file,agmv);

			if(err1 != NO_ERR){
				fclose(file);
				DestroyAGMV(agmv);
				return err1;
			}

			AGIDL_QuickExport(agmv->frame->img_data,agmv->frame->width,agmv->frame->height,AGIDL_RGB_888,img_type);
		}
	}

	fclose(file);
	DestroyAGMV(agmv);
	
	return NO_ERR;
}

void to_80bitfloat(u32 num, u8 bytes[10])
{	u32 mask = 0x40000000 ;
	int	count ;

	if (num <= 1)
	{	bytes [0] = 0x3F ;
		bytes [1] = 0xFF ;
		bytes [2] = 0x80 ;
		return ;
		}

	bytes [0] = 0x40 ;

	if (num >= mask)
	{	bytes [1] = 0x1D ;
		return ;
		}

	for (count = 0 ; count < 32 ; count ++)
	{	if (num & mask)
			break ;
		mask >>= 1 ;
		}

	num = count < 31 ? num << (count + 1) : 0 ;
	bytes [1] = 29 - count ;
	bytes [2] = (num >> 24) & 0xFF ;
	bytes [3] = (num >> 16) & 0xFF ;
	bytes [4] = (num >> 8) & 0xFF ;
	bytes [5] = num & 0xFF ;

} 

int AGMV_DecodeAudio(const char* filename, const AGMV_AUDIO_TYPE audio_type){
	AGMV* agmv = malloc(sizeof(AGMV));
	agmv->frame_chunk = (AGMV_FRAME_CHUNK*)malloc(sizeof(AGMV_FRAME_CHUNK));
	agmv->audio_chunk = (AGMV_AUDIO_CHUNK*)malloc(sizeof(AGMV_AUDIO_CHUNK));
	agmv->bitstream = (AGMV_BITSTREAM*)malloc(sizeof(AGMV_BITSTREAM));
	agmv->frame = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->iframe = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->audio_track = (AGMV_AUDIO_TRACK*)malloc(sizeof(AGMV_AUDIO_TRACK));
	
	FILE* file = fopen(filename, "rb");
	
	if(file == NULL){
		fclose(file);
		DestroyAGMV(agmv);
		return FILE_NOT_FOUND_ERR;
	}

	const int err = AGMV_DecodeHeader(file, agmv);
	const int num_of_frames = AGMV_GetNumberOfFrames(agmv);
	
	if(err != NO_ERR){
		fclose(file);
		DestroyAGMV(agmv);
		return err;
	}
	if(agmv->header.total_audio_duration != 0){
		FILE*audio;
		agmv->audio_track->start_point = 0;
		agmv->audio_track->pcm = (u16*)malloc(sizeof(u16)*agmv->header.audio_size);

		agmv->audio_chunk->size = agmv->header.audio_size / (f32)agmv->header.num_of_frames;

		for(int i = 0; i < num_of_frames; i++){
			AGMV_FindNextFrameChunk(file);
			AGMV_SkipFrameChunk(file);
			AGMV_FindNextAudioChunk(file);
			const int err1 = AGMV_DecodeAudioChunk(file, agmv);

			if(err1 != NO_ERR){
				fclose(file);
				DestroyAGMV(agmv);
				return err1;
			}
		}

		switch(audio_type){
			case AGMV_AUDIO_WAV:{
				audio = fopen("quick_export.wav","wb");
				AGMV_ExportAudioType(audio,agmv,AGMV_AUDIO_WAV);
				fclose(audio);
			}break;
			case AGMV_AUDIO_AIFC:{
				audio = fopen("quick_export.aiff","wb");
				AGMV_ExportAudioType(audio,agmv,AGMV_AUDIO_AIFC);
				fclose(audio);
			}break;
			case AGMV_AUDIO_AIFF:{
				audio = fopen("quick_export.aiff","wb");
				AGMV_ExportAudioType(audio,agmv,AGMV_AUDIO_AIFF);
				fclose(audio);
			}break;
			default:{
				audio = fopen("quick_export.wav","wb");
				AGMV_ExportAudioType(audio,agmv,AGMV_AUDIO_WAV);
				fclose(audio);
			}break;
		}
	}

	fclose(file);
	DestroyAGMV(agmv);
	
	return NO_ERR;
}
