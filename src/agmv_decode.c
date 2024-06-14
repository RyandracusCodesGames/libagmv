
/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_decode.c
*   Date: 5/17/2024
*   Version: 1.1
*   Updated: 6/13/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <agidl.h>
#include <stdlib.h>
#include <time.h>
#include <agmv_decode.h>
#include <agmv_utils.h>

u16 AGMV_SQR_TABLE[256] = {
	0,1,4,9,16,25,36,49,64,
	81,100,121,144,169,196,225,256,
	289,324,361,400,441,484,529,576,
	625,676,729,784,841,900,961,1024,
	1089,1156,1225,1296,1369,1444,1521,1600,
	1681,1764,1849,1936,2025,2116,2209,2304,
	2401,2500,2601,2704,2809,2916,3025,3136,
	3249,3364,3481,3600,3721,3844,3969,4096,
	4225,4356,4489,4624,4761,4900,5041,5184,
	5329,5476,5625,5776,5929,6084,6241,6400,
	6561,6724,6889,7056,7225,7396,7569,7744,
	7921,8100,8281,8464,8649,8836,9025,9216,
	9409,9604,9801,10000,10201,10404,10609,10816,
	11025,11236,11449,11664,11881,12100,12321,12544,
	12769,12996,13225,13456,13689,13924,14161,14400,
	14641,14884,15129,15376,15625,15876,16129,16384,
	16641,16900,17161,17424,17689,17956,18225,18496,
	18769,19044,19321,19600,19881,20164,20449,20736,
	21025,21316,21609,21904,22201,22500,22801,23104,
	23409,23716,24025,24336,24649,24964,25281,25600,
	25921,26244,26569,26896,27225,27556,27889,28224,
	28561,28900,29241,29584,29929,30276,30625,30976,
	31329,31684,32041,32400,32761,33124,33489,33856,
	34225,34596,34969,35344,35721,36100,36481,36864,
	37249,37636,38025,38416,38809,39204,39601,40000,
	40401,40804,41209,41616,42025,42436,42849,43264,
	43681,44100,44521,44944,45369,45796,46225,46656,
	47089,47524,47961,48400,48841,49284,49729,50176,
	50625,51076,51529,51984,52441,52900,53361,53824,
	54289,54756,55225,55696,56169,56644,57121,57600,
	58081,58564,59049,59536,60025,60516,61009,61504,
	62001,62500,63001,63504,64009,64516,65025
};

u16 AGMV_SHIFT_TABLE[256] = {
	0,256,512,768,1024,1280,1536,1792,2048,
	2304,2560,2816,3072,3328,3584,3840,4096,
	4352,4608,4864,5120,5376,5632,5888,6144,
	6400,6656,6912,7168,7424,7680,7936,8192,
	8448,8704,8960,9216,9472,9728,9984,10240,
	10496,10752,11008,11264,11520,11776,12032,12288,
	12544,12800,13056,13312,13568,13824,14080,14336,
	14592,14848,15104,15360,15616,15872,16128,16384,
	16640,16896,17152,17408,17664,17920,18176,18432,
	18688,18944,19200,19456,19712,19968,20224,20480,
	20736,20992,21248,21504,21760,22016,22272,22528,
	22784,23040,23296,23552,23808,24064,24320,24576,
	24832,25088,25344,25600,25856,26112,26368,26624,
	26880,27136,27392,27648,27904,28160,28416,28672,
	28928,29184,29440,29696,29952,30208,30464,30720,
	30976,31232,31488,31744,32000,32256,32512,32768,
	33024,33280,33536,33792,34048,34304,34560,34816,
	35072,35328,35584,35840,36096,36352,36608,36864,
	37120,37376,37632,37888,38144,38400,38656,38912,
	39168,39424,39680,39936,40192,40448,40704,40960,
	41216,41472,41728,41984,42240,42496,42752,43008,
	43264,43520,43776,44032,44288,44544,44800,45056,
	45312,45568,45824,46080,46336,46592,46848,47104,
	47360,47616,47872,48128,48384,48640,48896,49152,
	49408,49664,49920,50176,50432,50688,50944,51200,
	51456,51712,51968,52224,52480,52736,52992,53248,
	53504,53760,54016,54272,54528,54784,55040,55296,
	55552,55808,56064,56320,56576,56832,57088,57344,
	57600,57856,58112,58368,58624,58880,59136,59392,
	59648,59904,60160,60416,60672,60928,61184,61440,
	61696,61952,62208,62464,62720,62976,63232,63488,
	63744,64000,64256,64512,64768,65024,65280
};

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
	agmv->header.bits_per_sample = AGIDL_ReadShort(file);
	
	if(!AGMV_IsCorrectFourCC(agmv->header.fourcc,'A','G','M','V') || !(agmv->header.version == 1 || agmv->header.version == 2 || agmv->header.version == 3 || agmv->header.version == 4) || agmv->header.frames_per_second >= 200
	|| !(agmv->header.bits_per_sample == 16 || agmv->header.bits_per_sample == 8)){
		return INVALID_HEADER_FORMATTING_ERR;
	}
	
	if(agmv->header.version == 1 || agmv->header.version == 3){	
		for(i = 0; i < 256; i++){
			u8 r = AGMV_ReadByte(file);
			u8 g = AGMV_ReadByte(file);
			u8 b = AGMV_ReadByte(file);
			
			agmv->header.palette0[i] = AGIDL_RGB(r,g,b,agmv->header.fmt);
		}
		
		for(i = 0; i < 256; i++){
			u8 r = AGMV_ReadByte(file);
			u8 g = AGMV_ReadByte(file);
			u8 b = AGMV_ReadByte(file);
			
			agmv->header.palette1[i] = AGIDL_RGB(r,g,b,agmv->header.fmt);
		}
	}
	else{
		for(i = 0; i < 256; i++){
			u8 r = AGMV_ReadByte(file);
			u8 g = AGMV_ReadByte(file);
			u8 b = AGMV_ReadByte(file);
			
			agmv->header.palette0[i] = AGIDL_RGB(r,g,b,agmv->header.fmt);
		}
	}
	
	return NO_ERR;
}

int AGMV_DecodeFrameChunk(FILE* file, AGMV* agmv){
	u32 pos, i, bitpos = 0, bits = 0, num_of_bits, size = agmv->header.width * agmv->header.height, width, height, bpos = 0, usize, csize, indice;	
	u32* img_data = agmv->frame->img_data, *iframe_data = agmv->iframe->img_data, *palette, color, offset;
	u8 byte, len, org, index, fbit, bot, *bitstream_data = agmv->bitstream->data;
	Bool escape = FALSE, invalid_flag = FALSE;

	agmv->bitstream->pos = 0;
	
	AGMV_ReadFourCC(file,agmv->frame_chunk->fourcc);
	agmv->frame_chunk->frame_num = AGMV_ReadLong(file);
	agmv->frame_chunk->uncompressed_size = AGMV_ReadLong(file);
	agmv->frame_chunk->compressed_size = AGMV_ReadLong(file);
	
	width  = agmv->frame->width;
	height = agmv->frame->height;
	
	usize = agmv->frame_chunk->uncompressed_size;
	csize = agmv->frame_chunk->compressed_size;
	
	num_of_bits = agmv->frame_chunk->compressed_size * 8;
	bpos = agmv->bitstream->pos;
	
	if(!AGMV_IsCorrectFourCC(agmv->frame_chunk->fourcc,'A','G','F','C')){
		return INVALID_HEADER_FORMATTING_ERR;
	}
	
	if(agmv->header.version == 1 || agmv->header.version == 2){
		for(bits = 0; bits < num_of_bits && bpos < usize;){

			byte = AGMV_ReadBits(file,1); bits++;
			
			if(byte & 1){
				if(bpos >= usize) {
					break;
				}
				bitstream_data[bpos++] = AGMV_ReadBits(file,8);
				bits += 8;
			}
			else{
				offset = AGMV_ReadBits(file,16);
				len = AGMV_ReadBits(file,4);
				
				bits += 20;
				
				pos = bpos;

				int i;
				for (i = 0; i < len; i++) {
					if (pos - offset + i >= 0 && pos - offset + i < bpos) {
						bitstream_data[bpos++] = bitstream_data[pos - offset + i];
					}
				}
			}
		}
	}
	else{
		for(i = 0; i < csize; i += 4){
			
			offset = AGMV_ReadShort(file);
			len = AGMV_ReadByte(file);	
			byte = AGMV_ReadByte(file);
			
			pos = bpos;
			
			int i;
			for (i = 0; i < len; i++) {
				if (pos - offset + i >= 0 && pos - offset + i < bpos) {
					bitstream_data[bpos++] = bitstream_data[pos - offset + i];
				}
			}
			
			bitstream_data[bpos++] = byte;
		}
	}

	agmv->bitstream->pos = bpos;

	AGMV_FlushReadBits();

	if(agmv->header.version == 1 || agmv->header.version == 3){
		int x,y;
		for(y = 0; y < height && escape != TRUE; y += 4){
			for(x = 0; x < width && escape != TRUE; x += 4){
				
				if(bitpos > bpos){
					escape = TRUE;
					break;
				}

				byte = bitstream_data[bitpos++];
				
				while(byte != AGMV_FILL_FLAG && byte != AGMV_NORMAL_FLAG && byte != AGMV_COPY_FLAG){
					byte = bitstream_data[bitpos++];
					
					if(bitpos > bpos){
						escape = TRUE;
						break;
					}
				}

				if(byte != AGMV_FILL_FLAG && byte != AGMV_NORMAL_FLAG && byte != AGMV_COPY_FLAG){
					invalid_flag = TRUE;
				}
				
				if(byte == AGMV_FILL_FLAG){
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
					
					if(x == width-4 && y == height-4){
						color = img_data[(x-1)+(y+1)*width];
					}
					
					if(bitpos > bpos){
						escape = TRUE;
						break;
					}
					
					int i,j;
					for(j = 0; j < 4; j++){
						offset = (y+j)*width;
						for(i = 0; i < 4; i++){
							img_data[(x+i)+offset] = color;
						}
					}
				}
				else if(byte == AGMV_COPY_FLAG){
					int i,j;
					for(j = 0; j < 4; j++){
						offset = (y+j)*width;
						for(i = 0; i < 4; i++){
							indice = (x+i)+offset;
							img_data[indice] = iframe_data[indice];
						}
					}
				}
				else{
					int i,j;
					for(j = 0; j < 4; j++){
						offset = (y+j)*width;
						for(i = 0; i < 4; i++){
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
							
							if(bitpos > bpos || invalid_flag == TRUE){
								escape = TRUE;
								invalid_flag = FALSE;
								break;
							}
							
							img_data[(x+i)+offset] = color;
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
				
				if(bitpos > bpos){
					escape = TRUE;
					break;
				}
				
				byte = bitstream_data[bitpos++];
				
				while(byte != AGMV_FILL_FLAG && byte != AGMV_NORMAL_FLAG && byte != AGMV_COPY_FLAG){
					byte = bitstream_data[bitpos++];
					
					if(bitpos > bpos){
						escape = TRUE;
						break;
					}
				}
				
				if(byte != AGMV_FILL_FLAG && byte != AGMV_NORMAL_FLAG && byte != AGMV_COPY_FLAG){
					invalid_flag = TRUE;
				}
				
				if(byte == AGMV_FILL_FLAG){
					index = bitstream_data[bitpos++];
					color = palette0[index];

					if(x == width-4 && y == height-4){
						color = img_data[(x-1)+(y+1)*width];
					}
					
					if(bitpos > bpos){
						escape = TRUE;
						break;
					}
					
					int i,j;
					for(j = 0; j < 4; j++){
						offset = (y+j)*width;
						for(i = 0; i < 4; i++){
							img_data[(x+i)+offset] = color;
						}
					}
				}
				else if(byte == AGMV_COPY_FLAG){
					int i,j;
					for(j = 0; j < 4; j++){
						offset = (y+j)*width;
						for(i = 0; i < 4; i++){
							indice = (x+i)+offset;
							img_data[indice] = iframe_data[indice];
						}
					}
				}
				else{
					int i,j;
					for(j = 0; j < 4; j++){
						offset = (y+j)*width;
						for(i = 0; i < 4; i++){
							index = bitstream_data[bitpos++];
							if(bitpos > bpos || invalid_flag == TRUE){
								escape = TRUE;
								invalid_flag = FALSE;
								break;
							}
							color = palette0[index];
							img_data[(x+i)+offset] = color;
						}
					}
				}
			}
		}
	}
	
	if(agmv->frame_count % 4 == 0){
		for(i = 0; i < size; i++){
			iframe_data[i] = img_data[i];
		}
	}
	
	agmv->frame_count++;

	return NO_ERR;
}

int AGMV_DecodeAudioChunk(FILE* file, AGMV* agmv){
	int i;
	u32 size, start_point;
	u16 sample, resample, *pcm = agmv->audio_track->pcm, bits_per_sample = AGMV_GetBitsPerSample(agmv);
	u8* pcm8 = agmv->audio_track->pcm8;
	
	AGMV_ReadFourCC(file,agmv->audio_chunk->fourcc);
	agmv->audio_chunk->size = AGIDL_ReadLong(file);
	
	if(!AGMV_IsCorrectFourCC(agmv->audio_chunk->fourcc,'A','G','A','C')){
		return INVALID_HEADER_FORMATTING_ERR;
	}
	
	size = agmv->audio_chunk->size;
	start_point = agmv->audio_track->start_point;
	
	
	if(bits_per_sample == 16){
		for(i = 0; i < size; i++){
			sample = AGIDL_ReadByte(file);
			resample = 0;
			
			if(sample % 2 == 0){
				resample = AGMV_SQR_TABLE[sample];
			}
			else{
				resample = AGMV_SHIFT_TABLE[sample];
			}
			
			pcm[start_point++] = resample; 
		}
	}
	else{
		for(i = 0; i < size; i++){
			pcm8[start_point++] = AGIDL_ReadByte(file); 
		}
	}
	
	agmv->audio_track->start_point = start_point;
	
	return NO_ERR;
}

int AGMV_DecodeVideo(const char* filename, u8 img_type){
	
	int err, err1, i, num_of_frames;
	Bool has_audio = FALSE;
	
	AGMV* agmv = (AGMV*)malloc(sizeof(AGMV));
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
	
	err = AGMV_DecodeHeader(file,agmv);
	
	if(agmv->header.total_audio_duration != 0){
		has_audio = TRUE;
	}
	
	agmv->frame->width = agmv->header.width;
	agmv->frame->height = agmv->header.height;
	agmv->frame->img_data = (u32*)malloc(sizeof(u32)*agmv->frame->width*agmv->frame->height);
	
	agmv->iframe->width = agmv->header.width;
	agmv->iframe->height = agmv->header.height;
	agmv->iframe->img_data = (u32*)malloc(sizeof(u32)*agmv->frame->width*agmv->frame->height);
	
	agmv->bitstream->len = agmv->header.width*agmv->header.height*2;
	agmv->bitstream->data = (u8*)malloc(sizeof(u8)*agmv->bitstream->len);
	
	num_of_frames = AGMV_GetNumberOfFrames(agmv);
	
	if(err != NO_ERR){
		fclose(file);
		DestroyAGMV(agmv);
		return err;
	}
	else{
		for(i = 0; i < num_of_frames; i++){
			AGMV_FindNextFrameChunk(file);
			err1 = AGMV_DecodeFrameChunk(file,agmv);
			
			if(has_audio == TRUE){
				AGMV_FindNextAudioChunk(file);
				AGMV_SkipAudioChunk(file);
			}
				
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

int AGMV_DecodeAGMV(const char* filename, u8 img_type, AGMV_AUDIO_TYPE audio_type){
	FILE* file, *audio;
	int err, err1, err2, i, num_of_frames;
	u16 bits_per_sample;
	
	AGMV* agmv = (AGMV*)malloc(sizeof(AGMV));
	agmv->frame_chunk = (AGMV_FRAME_CHUNK*)malloc(sizeof(AGMV_FRAME_CHUNK));
	agmv->audio_chunk = (AGMV_AUDIO_CHUNK*)malloc(sizeof(AGMV_AUDIO_CHUNK));
	agmv->bitstream = (AGMV_BITSTREAM*)malloc(sizeof(AGMV_BITSTREAM));
	agmv->bitstream->pos = 0;
	agmv->frame = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->iframe = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->audio_track = (AGMV_AUDIO_TRACK*)malloc(sizeof(AGMV_AUDIO_TRACK));
	
	file = fopen(filename,"rb");
	
	if(file == NULL){
		fclose(file);
		DestroyAGMV(agmv);
		return FILE_NOT_FOUND_ERR;
	}
	
	err = AGMV_DecodeHeader(file,agmv);
	
	agmv->frame->width = agmv->header.width;
	agmv->frame->height = agmv->header.height;
	agmv->frame->img_data = (u32*)malloc(sizeof(u32)*agmv->frame->width*agmv->frame->height);
	
	agmv->iframe->width = agmv->header.width;
	agmv->iframe->height = agmv->header.height;
	agmv->iframe->img_data = (u32*)malloc(sizeof(u32)*agmv->frame->width*agmv->frame->height);
	
	agmv->bitstream->len = agmv->header.width*agmv->header.height*2;
	agmv->bitstream->data = (u8*)malloc(sizeof(u8)*agmv->bitstream->len);
	
	num_of_frames = AGMV_GetNumberOfFrames(agmv);
	bits_per_sample = AGMV_GetBitsPerSample(agmv);
	
	if(err != NO_ERR){
		fclose(file);
		DestroyAGMV(agmv);
		return err;
	}
	else{
		if(agmv->header.total_audio_duration != 0){
			agmv->audio_track->start_point = 0;
			
			if(bits_per_sample == 16){
				agmv->audio_track->pcm = (u16*)malloc(sizeof(u16)*agmv->header.audio_size);
			}
			else{
				agmv->audio_track->pcm8 = (u8*)malloc(sizeof(u8)*agmv->header.audio_size);
			}
	
			agmv->audio_chunk->size = agmv->header.audio_size / (f32)agmv->header.num_of_frames;
			
			for(i = 0; i < num_of_frames; i++){
				AGMV_FindNextFrameChunk(file);
				err1 = AGMV_DecodeFrameChunk(file,agmv);
				AGMV_FindNextAudioChunk(file);
				err2 = AGMV_DecodeAudioChunk(file,agmv);
				
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
		} ;

	bytes [0] = 0x40 ;

	if (num >= mask)
	{	bytes [1] = 0x1D ;
		return ;
		} ;

	for (count = 0 ; count < 32 ; count ++)
	{	if (num & mask)
			break ;
		mask >>= 1 ;
		} ;

	num = count < 31 ? num << (count + 1) : 0 ;
	bytes [1] = 29 - count ;
	bytes [2] = (num >> 24) & 0xFF ;
	bytes [3] = (num >> 16) & 0xFF ;
	bytes [4] = (num >> 8) & 0xFF ;
	bytes [5] = num & 0xFF ;

} 

int AGMV_DecodeAudio(const char* filename, AGMV_AUDIO_TYPE audio_type){
	FILE* file, *audio;
	int err, err1, i, num_of_frames;
	u16 bits_per_sample;
	
	AGMV* agmv = (AGMV*)malloc(sizeof(AGMV));
	agmv->frame_chunk = (AGMV_FRAME_CHUNK*)malloc(sizeof(AGMV_FRAME_CHUNK));
	agmv->audio_chunk = (AGMV_AUDIO_CHUNK*)malloc(sizeof(AGMV_AUDIO_CHUNK));
	agmv->bitstream = (AGMV_BITSTREAM*)malloc(sizeof(AGMV_BITSTREAM));
	agmv->frame = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->iframe = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->audio_track = (AGMV_AUDIO_TRACK*)malloc(sizeof(AGMV_AUDIO_TRACK));
	
	file = fopen(filename,"rb");
	
	if(file == NULL){
		fclose(file);
		DestroyAGMV(agmv);
		return FILE_NOT_FOUND_ERR;
	}
	
	err = AGMV_DecodeHeader(file,agmv);
	num_of_frames = AGMV_GetNumberOfFrames(agmv);
	bits_per_sample = AGMV_GetBitsPerSample(agmv);
	
	if(err != NO_ERR){
		fclose(file);
		DestroyAGMV(agmv);
		return err;
	}
	else{
		if(agmv->header.total_audio_duration != 0){
			agmv->audio_track->start_point = 0;
			
			if(bits_per_sample == 16){
				agmv->audio_track->pcm = (u16*)malloc(sizeof(u16)*agmv->header.audio_size);
			}
			else{
				agmv->audio_track->pcm8 = (u8*)malloc(sizeof(u8)*agmv->header.audio_size);
			}
	
			agmv->audio_chunk->size = agmv->header.audio_size / (f32)agmv->header.num_of_frames;
			
			for(i = 0; i < num_of_frames; i++){
				AGMV_FindNextFrameChunk(file);
				AGMV_SkipFrameChunk(file);
				AGMV_FindNextAudioChunk(file);
				err1 = AGMV_DecodeAudioChunk(file,agmv);

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
	}
	
	fclose(file);
	DestroyAGMV(agmv);
	
	return NO_ERR;
}
