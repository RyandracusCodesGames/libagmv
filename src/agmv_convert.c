/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_convert.h
*   Date: 8/11/2024
*   Version: 2.0
*   Updated: 10/13/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <stdlib.h>
#include <agmv_convert.h>
#include <agmv_utils.h>
#include <agmv_decode.h>
#include <agmv_encode.h>
#include <agidl_imgp_scale.h>

int AGMV_ConvertAGMVtoAVI(const char* filename){
	int i, block_align, size, pos;
	int err, num_of_frames, frame_size;
	u32* frame;
	u16 bits_per_sample;
	AGMV* agmv;

	FILE* out, *in;
	
	in  = fopen(filename,"rb");
	out = fopen("output.avi","wb");
	
	agmv = (AGMV*)malloc(sizeof(AGMV));
	
	if(in == NULL){
		fclose(in);
		fclose(out);
		free(agmv);
		return FILE_NOT_FOUND_ERR;
	}
	
	agmv->header.total_audio_duration = 0;
	
	err = AGMV_DecodeHeader(in,agmv);
	
	frame_size = agmv->header.width*agmv->header.height;
	
	agmv->frame_chunk = (AGMV_FRAME_CHUNK*)malloc(sizeof(AGMV_FRAME_CHUNK));
	agmv->audio_chunk = (AGMV_AUDIO_CHUNK*)malloc(sizeof(AGMV_AUDIO_CHUNK));
	agmv->bitstream = (AGMV_BITSTREAM*)malloc(sizeof(AGMV_BITSTREAM));
	agmv->bitstream->pos = 0;
	agmv->frame = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->iframe = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->prev_frame = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->audio_track = (AGMV_AUDIO_TRACK*)malloc(sizeof(AGMV_AUDIO_TRACK));
	
	agmv->frame->width = agmv->header.width;
	agmv->frame->height = agmv->header.height;
	agmv->frame->img_data = (u32*)malloc(sizeof(u32)*agmv->frame->width*agmv->frame->height);
	
	agmv->iframe->width = agmv->header.width;
	agmv->iframe->height = agmv->header.height;
	agmv->iframe->img_data = (u32*)malloc(sizeof(u32)*agmv->frame->width*agmv->frame->height);
	
	agmv->prev_frame->width = agmv->header.width;
	agmv->prev_frame->height = agmv->header.height;
	agmv->prev_frame->img_data = (u32*)malloc(sizeof(u32)*agmv->frame->width*agmv->frame->height);
	
	agmv->bitstream->len = agmv->header.width*agmv->header.height*2;
	agmv->bitstream->data = (u8*)malloc(sizeof(u8)*agmv->bitstream->len);
	
	num_of_frames = AGMV_GetNumberOfFrames(agmv);
	bits_per_sample = AGMV_GetBitsPerSample(agmv);
	
	if(err != NO_ERR){
		fclose(in);
		fclose(out);
		DestroyAGMV(agmv);
		return err;
	}

	AGMV_WriteFourCC(out,'R','I','F','F');
	AGMV_WriteLong(out,0);
	AGMV_WriteFourCC(out,'A','V','I',0x20);
	AGMV_WriteFourCC(out,'L','I','S','T');
	
	if(AGMV_GetTotalAudioDuration(agmv) > 0){
		AGMV_WriteLong(out,310);
	}
	else{
		AGMV_WriteLong(out,200);
	}
	
	AGMV_WriteFourCC(out,'h','d','r','l');
	AGMV_WriteFourCC(out,'a','v','i','h');
	
	AGMV_WriteLong(out,56);
	AGMV_WriteLong(out,AGMV_GetMicrosecondsPerFrame(AGMV_GetFramesPerSecond(agmv)));
	AGMV_WriteLong(out,0);
	AGMV_WriteLong(out,0);
	
	if(AGMV_GetTotalAudioDuration(agmv) > 0){
		AGMV_WriteLong(out,0x00000100);
	}
	else{
		AGMV_WriteLong(out,2320);
	}
	
	AGMV_WriteLong(out,AGMV_GetNumberOfFrames(agmv));
	AGMV_WriteLong(out,0);
	
	if(AGMV_GetTotalAudioDuration(agmv) > 0){
		AGMV_WriteLong(out,2);
	}
	else{
		AGMV_WriteLong(out,1);
	}
	
	AGMV_WriteLong(out,0);
	AGMV_WriteLong(out,AGMV_GetWidth(agmv));
	AGMV_WriteLong(out,AGMV_GetHeight(agmv));
	
	for(i = 0; i < 4; i++){
		AGMV_WriteLong(out,0);
	}
	
	AGMV_WriteFourCC(out,'L','I','S','T');
	AGMV_WriteLong(out,124);
	AGMV_WriteFourCC(out,'s','t','r','l');
	AGMV_WriteFourCC(out,'s','t','r','h');
	AGMV_WriteLong(out,64);
	AGMV_WriteFourCC(out,'v','i','d','s');
	AGMV_WriteFourCC(out,'D','I','B',0x20);
	
	for(i = 0; i < 3; i++){
		AGMV_WriteLong(out,0);
	}
	
	AGMV_WriteLong(out,1000);
	AGMV_WriteLong(out,AGMV_GetFramesPerSecond(agmv)*1000);
	AGMV_WriteLong(out,0);
	AGMV_WriteLong(out,AGMV_GetNumberOfFrames(agmv));
	AGMV_WriteLong(out,AGMV_GetWidth(agmv)*AGMV_GetHeight(agmv)*3);
	AGMV_WriteFourCC(out,0xff,0xff,0xff,0xff);
	
	for(i = 0; i < 3; i++){
		AGMV_WriteLong(out,0);
	}
	
	AGMV_WriteLong(out,AGMV_GetWidth(agmv));
	AGMV_WriteLong(out,AGMV_GetHeight(agmv));
	AGMV_WriteFourCC(out,'s','t','r','f');
	AGMV_WriteLong(out,40);
	AGMV_WriteLong(out,40);
	AGMV_WriteLong(out,AGMV_GetWidth(agmv));
	AGMV_WriteLong(out,AGMV_GetHeight(agmv));
	AGMV_WriteShort(out,1);
	AGMV_WriteShort(out,24);
	AGMV_WriteLong(out,0);
	AGMV_WriteLong(out,AGMV_GetWidth(agmv)*AGMV_GetHeight(agmv)*3);
	
	for(i = 0; i < 4; i++){
		AGMV_WriteLong(out,0);
	}
	
	if(AGMV_GetTotalAudioDuration(agmv) != 0){
		AGMV_WriteFourCC(out,'L','I','S','T');
		AGMV_WriteLong(out,102);
		AGMV_WriteFourCC(out,'s','t','r','l');
		AGMV_WriteFourCC(out,'s','t','r','h');
		AGMV_WriteLong(out,64);
		AGMV_WriteFourCC(out,'a','u','d','s');
		AGMV_WriteLong(out,1);
		
		for(i = 0; i < 3; i++){
			AGMV_WriteLong(out,0);
		}
		
		block_align = AGMV_GetBitsPerSample(agmv)*AGMV_GetNumberOfChannels(agmv)/8.0f;
		
		AGMV_WriteLong(out,block_align);
		AGMV_WriteLong(out,block_align*AGMV_GetSampleRate(agmv));
		AGMV_WriteLong(out,0);
		AGMV_WriteLong(out,AGMV_GetAudioSize(agmv)/block_align);
		AGMV_WriteLong(out,AGMV_GetAudioSize(agmv)/(f32)AGMV_GetNumberOfFrames(agmv));
		AGMV_WriteFourCC(out,0xff,0xff,0xff,0xff);
		AGMV_WriteLong(out,block_align);
		
		for(i = 0; i < 4; i++){
			AGMV_WriteLong(out,0);
		}
		
		AGMV_WriteFourCC(out,'s','t','r','f');
		AGMV_WriteLong(out,18);
		AGMV_WriteShort(out,1);
		AGMV_WriteShort(out,AGMV_GetNumberOfChannels(agmv));
		AGMV_WriteLong(out,AGMV_GetSampleRate(agmv));
		AGMV_WriteLong(out,75600);
		AGMV_WriteShort(out,block_align);
		AGMV_WriteShort(out,AGMV_GetBitsPerSample(agmv));
		AGMV_WriteShort(out,0);
	}
	
	AGMV_WriteFourCC(out,'L','I','S','T');
	AGMV_WriteLong(out,0);
	AGMV_WriteFourCC(out,'m','o','v','i');
	
	pos = ftell(out) - 8;
	
	if(AGMV_GetTotalAudioDuration(agmv) != 0){
		agmv->audio_chunk->size = agmv->header.audio_size / (f32)agmv->header.num_of_frames;
		
		if(bits_per_sample == 16){
			agmv->audio_track->pcm = (u16*)malloc(sizeof(u16)*agmv->audio_chunk->size+5);
		}
		else{
			agmv->audio_track->pcm8 = (u8*)malloc(sizeof(u8)*agmv->audio_chunk->size+5);
		}
	}
	
	for(i = 0; i < num_of_frames; i++){
		AGMV_FindNextFrameChunk(in);
		AGMV_DecodeFrameChunk(in,agmv);
		
		AGMV_WriteFourCC(out,'0','0','d','c');
		AGMV_WriteLong(out,frame_size*3);
		
		frame = agmv->frame->img_data;
		
		int n;
		for(n = 0; n < frame_size; n++){
			u32 color = frame[n];
			
			u8 r = AGMV_GetR(color);
			u8 g = AGMV_GetG(color);
			u8 b = AGMV_GetB(color);
			
			AGMV_WriteByte(out,b);
			AGMV_WriteByte(out,g);
			AGMV_WriteByte(out,r);
		}
		
		if(AGMV_GetTotalAudioDuration(agmv) > 0){
			AGMV_FindNextAudioChunk(in);
			AGMV_DecodeAudioSample(in,agmv);
			
			AGMV_WriteFourCC(out,'0','1','w','b');
			
			if(bits_per_sample == 8){
				AGMV_WriteLong(out,agmv->audio_chunk->size);
				AGMV_WriteNBytes(out,agmv->audio_track->pcm8,agmv->audio_chunk->size);
			}
			else{
				AGMV_WriteLong(out,agmv->audio_chunk->size*2);
				fwrite(agmv->audio_track->pcm,2,agmv->audio_chunk->size,out);
			}
		}
	}
	
	size = ftell(out);
	
	fseek(out,4,SEEK_SET);
	AGMV_WriteLong(out,size);
	fseek(out,pos,SEEK_SET);
	AGMV_WriteLong(out,size-pos);
	
	fclose(in);
	fclose(out);
		
	DestroyAGMV(agmv);
	
	return NO_ERR;
}

int AGMV_ConvertAVItoAGMV(const char* filename, AGMV_OPT opt, AGMV_QUALITY quality, AGMV_COMPRESSION compression){
	f32 rate;
	u32 frame_rate, num_of_frames, i, n, num_of_streams, size, sample_size, samp;
	u32 width, height, block_align, sample_rate, audio_size, start_point;
	u32 num_of_channels, bits_per_sample, max_clr, color, hcolor;
	u32 palette0[256], palette1[256], pal[512], AGMV_GBA_W, AGMV_GBA_H;
	u8 r, g, b;
	char fourcc[4];
	u32* colorgram, *histogram, *image;
	AGMV_QUALITY org_quality;
	
	org_quality = quality;
	
	AGMV* agmv;
	FILE* in, *out, *audio;
	
	in  = fopen(filename,"rb");
	out = fopen("output.agmv","wb");
	
	if(in == NULL){
		fclose(in);
		fclose(out);
		return FILE_NOT_FOUND_ERR;
	}
	
	if(quality == AGMV_LOW_QUALITY){
		AGMV_GBA_W = AGMV_LQ_GBA_W;
		AGMV_GBA_H = AGMV_LQ_GBA_H;
	}
	else{
		AGMV_GBA_W = AGMV_HQ_GBA_W;
		AGMV_GBA_H = AGMV_HQ_GBA_H;
	}
	
	switch(quality){
		case AGMV_HIGH_QUALITY:{
			max_clr = AGMV_MAX_CLR;
		}break;
		case AGMV_MID_QUALITY:{
			max_clr = 65535;
		}break;
		case AGMV_LOW_QUALITY:{
			max_clr = 32767;
		}break;
		default:{
			max_clr = 32767;
		}break;
	}
	
	if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III){
		max_clr = 32767;
		quality = AGMV_LOW_QUALITY;
	}
	
	bits_per_sample = 0;
	audio_size = 0;
	num_of_channels = 0;
	sample_rate = 0;
	start_point = 0;
	
	fseek(in,32,SEEK_SET);
	
	rate = AGMV_ReadLong(in) / 1000.0f;
	rate = 1.0f / rate;
	rate *= 1000;
	
	frame_rate = (u32)rate;
	
	fseek(in,48,SEEK_SET);
	
	num_of_frames = AGMV_ReadLong(in);
	
	fseek(in,4,SEEK_CUR);
	
	num_of_streams = AGMV_ReadLong(in);
	
	fseek(in,64,SEEK_SET);
	
	width  = AGMV_ReadLong(in);
	height = AGMV_ReadLong(in);
	
	size = width*height;
	
	image = (u32*)malloc(sizeof(u32)*width*height);
	
	if(num_of_streams == 2){
		AGMV_FindNextAudsChunk(in);

		fseek(in,20,SEEK_CUR);
			
		block_align = AGMV_ReadLong(in);
		sample_rate = AGMV_ReadLong(in);
			
		sample_rate /= block_align;
		
		fseek(in,4,SEEK_CUR);
		
		audio_size = AGMV_ReadLong(in);
		
		AGMV_FindNextStrfChunk(in);
		
		fseek(in,10,SEEK_CUR);
		
		num_of_channels = AGMV_ReadShort(in);
		sample_rate = AGMV_ReadLong(in);
		
		fseek(in,4,SEEK_CUR);
		
		block_align = AGMV_ReadShort(in);
		bits_per_sample = AGMV_ReadShort(in);
		
		audio_size *= block_align;
	}
	
	agmv = CreateAGMV(num_of_frames,width,height,frame_rate);
	AGMV_SetOPT(agmv,opt);
	AGMV_SetQuality(agmv,org_quality);
	AGMV_SetCompression(agmv,compression);
	AGMV_SetNumberOfChannels(agmv,num_of_channels);
	AGMV_SetBitsPerSample(agmv,bits_per_sample);
	AGMV_SetSampleRate(agmv,sample_rate);
	AGMV_SetAudioSize(agmv,audio_size);
	AGMV_SetFramesPerSecond(agmv,frame_rate);
	
	if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
		agmv->max_diff = AGMV_MAX_15BPP_DIFF;
	}
	else{
		agmv->max_diff = AGMV_MAX_DIFF;
	}
	
	switch(opt){
		case AGMV_OPT_GBA_I:
		case AGMV_OPT_GBA_II:
		case AGMV_OPT_GBA_III:
		case AGMV_OPT_GBA_IV:{
			AGMV_SetLeniency(agmv,0.0f);
			
			AGMV_SetWidth(agmv,AGMV_GBA_W);
			AGMV_SetHeight(agmv,AGMV_GBA_H);
			
			free(agmv->frame->img_data);
			free(agmv->iframe->img_data);
			free(agmv->prev_frame->img_data);
			
			agmv->frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_GBA_W*AGMV_GBA_H);
			agmv->iframe->img_data = (u32*)malloc(sizeof(u32)*AGMV_GBA_W*AGMV_GBA_H);
			agmv->prev_frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_GBA_W*AGMV_GBA_H);
		}break;
		case AGMV_OPT_PSX_I:
		case AGMV_OPT_PSX_II:
		case AGMV_OPT_PSX_III:
		case AGMV_OPT_PSX_IV:{
			AGMV_SetLeniency(agmv,0.0f);
			
			AGMV_SetWidth(agmv,AGMV_PSX_W);
			AGMV_SetHeight(agmv,AGMV_PSX_H);
			
			free(agmv->frame->img_data);
			free(agmv->iframe->img_data);
			free(agmv->prev_frame->img_data);
			
			agmv->frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_PSX_W*AGMV_PSX_H);
			agmv->iframe->img_data = (u32*)malloc(sizeof(u32)*AGMV_PSX_W*AGMV_PSX_H);
			agmv->prev_frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_PSX_W*AGMV_PSX_H);
		}break;
		case AGMV_OPT_NDS_I:
		case AGMV_OPT_NDS_II:
		case AGMV_OPT_NDS_III:
		case AGMV_OPT_NDS_IV:{
			AGMV_SetLeniency(agmv,0.0f);
			
			AGMV_SetWidth(agmv,AGMV_NDS_W);
			AGMV_SetHeight(agmv,AGMV_NDS_H);
			
			free(agmv->frame->img_data);
			free(agmv->iframe->img_data);
			free(agmv->prev_frame->img_data);
			
			agmv->frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_NDS_W*AGMV_NDS_H);
			agmv->iframe->img_data = (u32*)malloc(sizeof(u32)*AGMV_NDS_W*AGMV_NDS_H);
			agmv->prev_frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_NDS_W*AGMV_NDS_H);
		}break;
	}
	
	if(opt != AGMV_OPT_IV && opt != AGMV_OPT_GBA_IV && opt != AGMV_OPT_PSX_IV && opt != AGMV_OPT_NDS_IV){
			
		colorgram = (u32*)malloc(sizeof(u32)*max_clr+5);
		histogram = (u32*)malloc(sizeof(u32)*max_clr+5);
		
		for(i = 0; i < 512; i++){
			if(i < 256){
				palette0[i] = 0;
				palette1[i] = 0;
			}
			
			pal[i] = 0;
		}
		
		for(i = 0; i < max_clr; i++){
			histogram[i] = 1;
			colorgram[i] = i;
		}
		
		for(i = 0; i < num_of_frames; i++){
			AGMV_FindNextFourCCChunk(in,'0','0','d','c');
			fseek(in,8,SEEK_CUR);
			
			for(n = 0; n < size; n++){
				
				b = AGMV_ReadByte(in);
				g = AGMV_ReadByte(in);
				r = AGMV_ReadByte(in);
				
				color = r << 16 | g << 8 | b;
				hcolor = AGMV_QuantizeColor(color,quality);
				histogram[hcolor] = histogram[hcolor] + 1;
			}
			
			AGMV_ReadFourCC(in,fourcc);
			
			if(AGMV_IsCorrectFourCC(fourcc,'0','1','w','b')){
				sample_size = AGMV_ReadLong(in);
				fseek(in,sample_size,SEEK_CUR);
			}
			
			fseek(in,ftell(in)-4,SEEK_SET);
		}
		
		AGMV_BubbleSort(histogram,colorgram,max_clr);
		AGMV_DistributePaletteData(colorgram,pal,palette0,palette1,max_clr,opt,quality);
		
		free(colorgram);
		free(histogram);
	}
	
	if(num_of_streams == 2){
		audio = fopen("output.audio","wb");
		
		AGMV_SetTotalAudioDuration(agmv,audio_size/(sample_rate*num_of_channels*(bits_per_sample/8)));
	
		if(bits_per_sample == 16){
			audio_size /= 2;
		}
		
		AGMV_SetAudioSize(agmv,audio_size);
		
		sample_size = agmv->header.audio_size / (f32)num_of_frames;
		agmv->audio_chunk->size = sample_size;
		agmv->audio_chunk->atsample = (u8*)malloc(sizeof(u8)*agmv->audio_chunk->size);
		agmv->audio_track->start_point = 0;
		
		if(bits_per_sample == 16){
			agmv->audio_track->pcm = (u16*)malloc(sizeof(u16)*agmv->audio_chunk->size);
		}
		else{
			agmv->audio_track->pcm8 = (u8*)malloc(sizeof(u8)*agmv->audio_chunk->size);
		}
		
		fseek(in,0,SEEK_SET);
		
		while(AGMV_EOF(in) != TRUE && start_point < audio_size){
			AGMV_FindNextFourCCChunk(in,'0','1','w','b');
			fseek(in,4,SEEK_CUR);
			samp = AGMV_ReadLong(in);
			
			if(bits_per_sample == 8){
				if(audio_size - start_point <= 60000){
					for(i = 0; i < samp && start_point < audio_size; i++){
						AGMV_WriteByte(audio,AGMV_ReadByte(in));
						start_point++;
					}
				}
				else{
					for(i = 0; i < samp; i++){
						AGMV_WriteByte(audio,AGMV_ReadByte(in));
					}
					
					start_point += samp;
				}
			}
			else{		
				if(audio_size - start_point <= 60000){
					for(i = 0; i < samp/2 && start_point < audio_size; i++){
						AGMV_WriteShort(audio,AGMV_ReadShort(in));
						start_point++;
					}
				}
				else{
					for(i = 0; i < samp/2; i++){
						AGMV_WriteShort(audio,AGMV_ReadShort(in));
					}
					start_point += samp / 2;
				}
			}
			
			AGMV_ReadFourCC(in,fourcc);
			
			while(AGMV_IsCorrectFourCC(fourcc,'0','0','d','c') && AGMV_EOF(in) != TRUE){
				fseek(in,4,SEEK_CUR);
				fseek(in,width*height*3,SEEK_CUR);
				AGMV_ReadFourCC(in,fourcc);
			}
			
			fseek(in,ftell(in)-4,SEEK_SET);
		}
	}
	
	fseek(in,0,SEEK_SET);
	
	AGMV_SetICP0(agmv,palette0);
	AGMV_SetICP1(agmv,palette1);
	
	AGMV_Printf("Encoding AGMV Header...\n");
	AGMV_EncodeHeader(out,agmv);
	AGMV_Printf("Encoded AGMV Header...\n");
	
	if(num_of_streams == 2){
		fclose(audio);
		
		audio = fopen("output.audio","rb");
	}
	
	for(i = 0; i < num_of_frames; i++){
		AGMV_FindNextFourCCChunk(in,'0','0','d','c');
		fseek(in,8,SEEK_CUR);
		
		for(n = 0; n < size; n++){
			
			b = AGMV_ReadByte(in);
			g = AGMV_ReadByte(in);
			r = AGMV_ReadByte(in);
			
			image[n] = r << 16 | g << 8 | b;
		}
		
		if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
			for(n = 0; n < size; n++){
				b = AGMV_GetB(image[n]);
				g = AGMV_GetG(image[n]);
				r = AGMV_GetR(image[n]);
				
				b >>= 3;
				g >>= 3;
				r >>= 3;
				
				b <<= 3;
				g <<= 3;
				r <<= 3;
			
				image[n] = r << 16 | g << 8 | b;
			}
		}
		
		if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
			u32* copy, *scale;
			u32 oldw, oldh;
			oldw = width; oldh = height;
			
			copy = (u32*)malloc(sizeof(u32)*width*height);
			AGIDL_ClrMemcpy(copy,image,width*height);
			scale = AGIDL_ScaleImgDataNearest(copy,&oldw,&oldh,((f32)AGMV_GBA_W/width)+0.001f,((f32)AGMV_GBA_H/height)+0.001f,AGIDL_SCALE_NEAREST);
			
			if(dither){
				AGMV_DitherImageData(scale,AGMV_GBA_W,AGMV_GBA_H);
			}
			
			AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
			AGMV_EncodeFrame(out,agmv,scale);
			AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
			free(scale);
			
			if(num_of_streams == 2){
				AGMV_Printf("Encoding AGMV Audio Chunk - %ld...\n",i);					
				AGMV_FillAudioBuffer(audio,agmv,AGMV_AUDIO_WAV);
				AGMV_CompressAudioBuffer(agmv);
				AGMV_EncodeAudioChunk(out,agmv);
				AGMV_Printf("Encoded AGMV Audio Chunk - %ld...\n",i);	
			}
		}
		else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
			u32* copy, *scale;
			u32 oldw, oldh;
			oldw = width; oldh = height;
			
			copy = (u32*)malloc(sizeof(u32)*width*height);
			AGIDL_ClrMemcpy(copy,image,width*height);
			scale = AGIDL_ScaleImgDataNearest(copy,&oldw,&oldh,((f32)AGMV_PSX_W/width)+0.001f,((f32)AGMV_PSX_H/height)+0.001f,AGIDL_SCALE_NEAREST);
			
			int x,y;
			for(y = 0; y < AGMV_PSX_H / 2; y++){
				u32 offset = y * AGMV_PSX_W;
				u32 offset2 = (AGMV_PSX_H - 1 - y) * AGMV_PSX_W;
				for(x = 0; x < AGMV_PSX_W; x++){
					u16 temp = scale[x + offset];
					scale[x + offset] = scale[x + offset2];
					scale[x + offset2] = temp;
				}
			}
			
			if(dither){
				AGMV_DitherImageData(scale,AGMV_PSX_W,AGMV_PSX_H);
			}
			
			AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
			AGMV_EncodeFrame(out,agmv,scale);
			AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
			free(scale);
			
			if(num_of_streams == 2){
				AGMV_Printf("Encoding AGMV Audio Chunk - %ld...\n",i);					
				AGMV_FillAudioBuffer(audio,agmv,AGMV_AUDIO_WAV);
				AGMV_CompressAudioBuffer(agmv);
				AGMV_EncodeAudioChunk(out,agmv);
				AGMV_Printf("Encoded AGMV Audio Chunk - %ld...\n",i);	
			}
		}
		else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
			u32* copy, *scale;
			u32 oldw, oldh;
			oldw = width; oldh = height;
			
			copy = (u32*)malloc(sizeof(u32)*width*height);
			AGIDL_ClrMemcpy(copy,image,width*height);
			scale = AGIDL_ScaleImgDataNearest(copy,&oldw,&oldh,((f32)AGMV_NDS_W/width)+0.001f,((f32)AGMV_NDS_H/height)+0.001f,AGIDL_SCALE_NEAREST);
			
			if(dither){
				AGMV_DitherImageData(scale,AGMV_NDS_W,AGMV_NDS_H);
			}
			
			AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
			AGMV_EncodeFrame(out,agmv,scale);
			AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
			free(scale);
			
			if(num_of_streams == 2){
				AGMV_Printf("Encoding AGMV Audio Chunk - %ld...\n",i);					
				AGMV_FillAudioBuffer(audio,agmv,AGMV_AUDIO_WAV);
				AGMV_CompressAudioBuffer(agmv);
				AGMV_EncodeAudioChunk(out,agmv);
				AGMV_Printf("Encoded AGMV Audio Chunk - %ld...\n",i);	
			}
		}
		else{
			
			if(dither){
				AGMV_DitherImageData(image,width,height);
			}
		
			AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
			AGMV_EncodeFrame(out,agmv,image);
			AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
					
			if(num_of_streams == 2){
				AGMV_Printf("Encoding AGMV Audio Chunk - %ld...\n",i);					
				AGMV_FillAudioBuffer(audio,agmv,AGMV_AUDIO_WAV);
				AGMV_CompressAudioBuffer(agmv);
				AGMV_EncodeAudioChunk(out,agmv);
				AGMV_Printf("Encoded AGMV Audio Chunk - %ld...\n",i);	
			}
		}
		
		AGMV_ReadFourCC(in,fourcc);
		
		if(AGMV_IsCorrectFourCC(fourcc,'0','1','w','b')){
			sample_size = AGMV_ReadLong(in);
			fseek(in,sample_size,SEEK_CUR);
		}
		
		fseek(in,ftell(in)-4,SEEK_SET);
	}
	
	if(num_of_streams == 2){
		fclose(audio);
		remove("output.audio");
	}
	
	free(image);
	
	fclose(in);
	fclose(out);
	
	DestroyAGMV(agmv);
	
	return NO_ERR;
}

u16 AGMV_ToRGB16(u32 color){
	u8 r, g, b;
	
	r = AGMV_GetR(color);
	g = AGMV_GetG(color);
	b = AGMV_GetB(color);
	
	r >>= 3;
	g >>= 3;
	b >>= 3;

	return 1 << 15 | b << 10 | g << 5 | r;
}