/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_encode.c
*   Date: 5/17/2024
*   Version: 1.0
*   Updated: 6/5/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agmv_encode.h>

#include <agidl.h>
#include <stdlib.h>
#include <tgmath.h>

#include <agmv_utils.h>

void AGMV_EncodeHeader(FILE* file, const AGMV* agmv){
	u32 i;
	u8  r, g, b;

	const AGMV_OPT opt = AGMV_GetOPT(agmv);
	const AGMV_COMPRESSION compression = AGMV_GetCompression(agmv);

	AGMV_WriteFourCC(file,'A','G','M','V');
	AGMV_WriteLong(file,AGMV_GetNumberOfFrames(agmv));
	AGMV_WriteLong(file,AGMV_GetWidth(agmv));
	AGMV_WriteLong(file,AGMV_GetHeight(agmv));
	AGMV_WriteByte(file,1);

	if(opt != AGMV_OPT_II && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_II){
		
		AGMV_WriteByte(file,AGMV_GetVersionFromOPT(opt,compression));	
		
		AGMV_WriteLong(file,AGMV_GetFramesPerSecond(agmv));
		AGMV_WriteLong(file,AGMV_GetTotalAudioDuration(agmv));
		AGMV_WriteLong(file,AGMV_GetSampleRate(agmv));
		AGMV_WriteLong(file,AGMV_GetAudioSize(agmv));
		AGMV_WriteShort(file,AGMV_GetNumberOfChannels(agmv));

		for(i = 0; i < 256; i++){
			const u32 color = agmv->header.palette0[i];

			r = AGMV_GetR(color);
			g = AGMV_GetG(color);
			b = AGMV_GetB(color);

			AGMV_WriteByte(file,r);
			AGMV_WriteByte(file,g);
			AGMV_WriteByte(file,b);
		}

		for(i = 0; i < 256; i++){
			const u32 color = agmv->header.palette1[i];

			r = AGMV_GetR(color);
			g = AGMV_GetG(color);
			b = AGMV_GetB(color);

			AGMV_WriteByte(file,r);
			AGMV_WriteByte(file,g);
			AGMV_WriteByte(file,b);
		}
	}
	else{
		AGMV_WriteByte(file,AGMV_GetVersionFromOPT(opt,compression));

		AGMV_WriteLong(file,AGMV_GetFramesPerSecond(agmv));
		AGMV_WriteLong(file,AGMV_GetTotalAudioDuration(agmv));
		AGMV_WriteLong(file,AGMV_GetSampleRate(agmv));
		AGMV_WriteLong(file,AGMV_GetAudioSize(agmv));
		AGMV_WriteShort(file,AGMV_GetNumberOfChannels(agmv));

		for(i = 0; i < 256; i++){
			const u32 color = agmv->header.palette0[i];

			r = AGMV_GetR(color);
			g = AGMV_GetG(color);
			b = AGMV_GetB(color);

			AGMV_WriteByte(file,r);
			AGMV_WriteByte(file,g);
			AGMV_WriteByte(file,b);
		}
	}
}

/*
==================
LZSS
==================
*/
#define	BACK_WINDOW		65535
#define	BACK_BITS		16
#define	FRONT_WINDOW	15
#define	FRONT_BITS		4

u32 AGMV_LZSS (FILE* file, const AGMV_BITSTREAM* in)
{
	int		j;
	int		outbits = 0;
	const int     pos = in->pos;
	const u8*     data = in->data;

	outbits = 0;
	for (int i = 0 ; i<pos ; )
	{
		const int val = data[i];

		int max = FRONT_WINDOW;
		if (i + max > pos)
			max = pos - i;

		int start = i - BACK_WINDOW;
		if (start < 0)
			start = 0;
		int bestlength = 0;
		int beststart = 0;
		for ( ; start < i ; start++)
		{
			if (data[start] != val)
				continue;
			// count match length
			for (j=0 ; j<max ; j++)
				if (data[start+j] != data[i+j])
					break;
			if (j > bestlength)
			{
				bestlength = j;
				beststart = start;
			}
		}
		
		beststart = BACK_WINDOW - (i-beststart);

		if (bestlength < 3)
		{	/* output a single char */
			bestlength = 1;

			AGMV_WriteBits(file,1,1);
			AGMV_WriteBits(file,val,8);
			
			outbits += 9;
		}
		else
		{
			AGMV_WriteBits(file,0,1);
			if(BACK_WINDOW-beststart < 65536){
				AGMV_WriteBits(file,BACK_WINDOW-beststart,16);
			}
			else{
				AGMV_WriteBits(file,65535,16);
			}
			AGMV_WriteBits(file,bestlength,4);
			
			outbits += 21;
		}

		while (bestlength--)
		{
			i++;
		}
	}
	
	return outbits / 8.0f;
}

u32 AGMV_LZ77(FILE* file, const AGMV_BITSTREAM* in)
{
	int		j;
	int		outbits = 0;
	const int     pos = in->pos;
	const u8*     data = in->data;

	outbits = 0;
	for (int i = 0 ; i<pos ; )
	{
		const int val = data[i];

		int max = 255;
		if (i + max > pos)
			max = in->pos - i;

		int start = i - BACK_WINDOW;
		if (start < 0)
			start = 0;
		int bestlength = 0;
		int beststart = 0;
		for ( ; start < i ; start++)
		{
			if (data[start] != val)
				continue;
			// count match length
			for (j=0 ; j<max ; j++)
				if (data[start+j] != data[i+j])
					break;
			if (j > bestlength)
			{
				bestlength = j;
				beststart = i - start;
			}
		}

		if (bestlength > 0)
		{	
			AGMV_WriteShort(file,beststart);
			AGMV_WriteByte(file,bestlength);
			AGMV_WriteByte(file,in->data[i+bestlength]);
			i += bestlength + 1;
		}
		else
		{
			
			AGMV_WriteShort(file,0);
			AGMV_WriteByte(file,0);
			AGMV_WriteByte(file,in->data[i]);
			i++;
		}
		
		outbits += 32;
	}
	
	return outbits / 8.0f;
}

u8 AGMV_ComparePFrameBlock(const AGMV* agmv, const u32 x, const u32 y, const AGMV_ENTRY* entry){
	u32 color1, color2;

	const u32 width = agmv->frame->width;
	u8 count = 0;

	const AGMV_ENTRY* iframe_entries = agmv->iframe_entries;
	
	for(u32 j = 0; j < 4; j++){
		for(u32 i = 0; i < 4; i++){
			const AGMV_ENTRY ent1 = entry[(x+i)+(y+j)*width];
			const AGMV_ENTRY ent2 = iframe_entries[(x+i)+(y+j)*width];
			if(ent1.pal_num == 0){
				color1 = agmv->header.palette0[ent1.index];
			}
			else{
				color1 = agmv->header.palette1[ent1.index];
			}

			if(ent2.pal_num == 0){
				color2 = agmv->header.palette0[ent2.index];
			}
			else{
				color2 = agmv->header.palette1[ent2.index];
			}

			const int r1 = AGMV_GetR(color1);
			const int g1 = AGMV_GetG(color1);
			const int b1 = AGMV_GetB(color1);

			const int r2 = AGMV_GetR(color2);
			const int g2 = AGMV_GetG(color2);
			const int b2 = AGMV_GetB(color2);

			int rdiff = r1 - r2;
			int gdiff = g1 - g2;
			int bdiff = b1 - b2;

			if(rdiff < 0){
				rdiff = AGIDL_Abs(rdiff);
			}

			if(gdiff < 0){
				gdiff = AGIDL_Abs(gdiff);
			}

			if(bdiff < 0){
				bdiff = AGIDL_Abs(bdiff);
			}

			if(rdiff <= 2 && gdiff <= 2 && bdiff <= 2){
				count++;
			}
		}
	}

	return count;
}

u8 AGMV_CompareIFrameBlock(const AGMV* agmv, const u32 x, const u32 y, const u32 color, const AGMV_ENTRY* img_entry){
	const u32 width = agmv->frame->width;
	u8 count = 0;

	for(u32 j = 0; j < 4; j++){
		for(u32 i = 0; i < 4; i++){
			const AGMV_ENTRY entry = img_entry[(x+i)+(y+j)*width];
			u32 bc;
			if(entry.pal_num == 0){
				bc = agmv->header.palette0[entry.index];
			}
			else{
				bc = agmv->header.palette1[entry.index];
			}

			const int r1 = AGMV_GetR(color);
			const int g1 = AGMV_GetG(color);
			const int b1 = AGMV_GetB(color);

			const int r2 = AGMV_GetR(bc);
			const int g2 = AGMV_GetG(bc);
			const int b2 = AGMV_GetB(bc);

			int rdiff = r1 - r2;
			int gdiff = g1 - g2;
			int bdiff = b1 - b2;

			if(rdiff < 0){
				rdiff = AGIDL_Abs(rdiff);
			}

			if(gdiff < 0){
				gdiff = AGIDL_Abs(gdiff);
			}

			if(bdiff < 0){
				bdiff = AGIDL_Abs(bdiff);
			}

			if(rdiff <= 2 && gdiff <= 2 && bdiff <= 2){
				count++;
			}
		}
	}

	return count;
}

void AGMV_AssembleIFrameBitstream(const AGMV* agmv, const AGMV_ENTRY* img_entry){
	u32 x, y, i, j;
	u8* data = agmv->bitstream->data;

	const u32 width = agmv->frame->width;
	const u32 height = agmv->frame->height;

	const AGMV_OPT opt = AGMV_GetOPT(agmv);

	if(opt != AGMV_OPT_II && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_II){
		for(y = 0; y < height; y += 4){
			for(x = 0; x < width; x += 4){
				u32 color;
				const AGMV_ENTRY entry = img_entry[x+y*width];

				if(entry.pal_num == 0){
					color = agmv->header.palette0[entry.index];
				}
				else{
					color = agmv->header.palette1[entry.index];
				}

				const u8 count = AGMV_CompareIFrameBlock(agmv, x, y, color, img_entry);

				if(count >= AGMV_FILL_COUNT){
					data[agmv->bitstream->pos++] = AGMV_FILL_FLAG;
					if(entry.index < 127){
						data[agmv->bitstream->pos++] = entry.pal_num << 7 | entry.index;
					}
					else{
						data[agmv->bitstream->pos++] = entry.pal_num << 7 | 127;
						data[agmv->bitstream->pos++] = entry.index;
					}
				}
				else{
					data[agmv->bitstream->pos++] = AGMV_NORMAL_FLAG;
					for(j = 0; j < 4; j++){
						for(i = 0; i < 4; i++){
							const AGMV_ENTRY norm = img_entry[(x+i)+(y+j)*width];
							if(norm.index < 127){
								data[agmv->bitstream->pos++] = norm.pal_num << 7 | norm.index;
							}
							else{
								data[agmv->bitstream->pos++] = norm.pal_num << 7 | 127;
								data[agmv->bitstream->pos++] = norm.index;
							}
						}
					}
				}
			}
		}
	}
	else{
		for(y = 0; y < height; y += 4){
			for(x = 0; x < width; x += 4){
				AGMV_ENTRY entry = img_entry[x+y*width];

				const u32 color = agmv->header.palette0[entry.index];
				const u8 count = AGMV_CompareIFrameBlock(agmv, x, y, color, img_entry);

				if(count >= AGMV_FILL_COUNT){
					data[agmv->bitstream->pos++] = AGMV_FILL_FLAG;
					data[agmv->bitstream->pos++] = entry.index;
				}
				else{
					data[agmv->bitstream->pos++] = AGMV_NORMAL_FLAG;

					for(j = 0; j < 4; j++){
						for(i = 0; i < 4; i++){
							entry = img_entry[(x+i)+(y+j)*width];
							data[agmv->bitstream->pos++] = entry.index;
						}
					}
				}
			}
		}
	}
}

void AGMV_AssemblePFrameBitstream(const AGMV* agmv, const AGMV_ENTRY* img_entry){
	u32 x, y, i, j;
	u8* data = agmv->bitstream->data;

	const u32 width = agmv->frame->width;
	const u32 height = agmv->frame->height;

	const AGMV_OPT opt = AGMV_GetOPT(agmv);

	if(opt != AGMV_OPT_II && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_II){
		for(y = 0; y < height; y += 4){
			for(x = 0; x < width; x += 4){
				u32 color;
				const AGMV_ENTRY entry = img_entry[x+y*width];

				if(entry.pal_num == 0){
					color = agmv->header.palette0[entry.index];
				}
				else{
					color = agmv->header.palette1[entry.index];
				}

				const u8 count1 = AGMV_CompareIFrameBlock(agmv, x, y, color, img_entry);
				const u8 count2 = AGMV_ComparePFrameBlock(agmv, x, y, img_entry);

				if(count2 >= AGMV_COPY_COUNT){
					data[agmv->bitstream->pos++] = AGMV_COPY_FLAG;
				}
				else if(count1 >= AGMV_FILL_COUNT){
					data[agmv->bitstream->pos++] = AGMV_FILL_FLAG;
					if(entry.index < 127){
						data[agmv->bitstream->pos++] = entry.pal_num << 7 | entry.index;
					}
					else{
						data[agmv->bitstream->pos++] = entry.pal_num << 7 | 127;
						data[agmv->bitstream->pos++] = entry.index;
					}
				}
				else{
					data[agmv->bitstream->pos++] = AGMV_NORMAL_FLAG;
					for(j = 0; j < 4; j++){
						for(i = 0; i < 4; i++){
							const AGMV_ENTRY norm = img_entry[(x+i)+(y+j)*width];
							if(norm.index < 127){
								data[agmv->bitstream->pos++] = norm.pal_num << 7 | norm.index;
							}
							else{
								data[agmv->bitstream->pos++] = norm.pal_num << 7 | 127;
								data[agmv->bitstream->pos++] = norm.index;
							}
						}
					}
				}
			}
		}
	}
	else{
		for(y = 0; y < height; y += 4){
			for(x = 0; x < width; x += 4){
				AGMV_ENTRY entry = img_entry[x+y*width];

				const u32 color = agmv->header.palette0[entry.index];
				const u8 count1 = AGMV_CompareIFrameBlock(agmv, x, y, color, img_entry);
				const u8 count2 = AGMV_ComparePFrameBlock(agmv, x, y, img_entry);

				if(count2 >= AGMV_COPY_COUNT){
					data[agmv->bitstream->pos++] = AGMV_COPY_FLAG;
				}
				else if(count1 >= AGMV_FILL_COUNT){
					data[agmv->bitstream->pos++] = AGMV_FILL_FLAG;
					data[agmv->bitstream->pos++] = entry.index;
				}
				else{
					data[agmv->bitstream->pos++] = AGMV_NORMAL_FLAG;

					for(j = 0; j < 4; j++){
						for(i = 0; i < 4; i++){
							entry = img_entry[(x+i)+(y+j)*width];
							data[agmv->bitstream->pos++] = entry.index;
						}
					}
				}
			}
		}
	}
}

void AGMV_EncodeFrame(FILE* file, AGMV* agmv, const u32* img_data){
	const AGMV_OPT opt = AGMV_GetOPT(agmv);

	int i, csize, pos;
	u32 palette0[256], palette1[256];

	for(i = 0; i < 256; i++){
		palette0[i] = agmv->header.palette0[i];
		palette1[i] = agmv->header.palette1[i];
	}

	const int size = AGMV_GetWidth(agmv) * AGMV_GetHeight(agmv);

	AGMV_ENTRY* iframe_entries = agmv->iframe_entries;
	AGMV_ENTRY* img_entry = malloc(sizeof(AGMV_ENTRY) * size);

	AGMV_SyncFrameAndImage(agmv,img_data);
	AGMV_WriteFourCC(file,'A','G','F','C');
	AGMV_WriteLong(file,agmv->frame_count+1);

	agmv->bitstream->pos = 0;

	if(opt != AGMV_OPT_II && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_II){

		for(i = 0; i < size; i++){
			img_entry[i] = AGMV_FindNearestEntry(palette0,palette1,img_data[i]);
		}

		if(agmv->frame_count % 4 == 0){
			AGMV_AssembleIFrameBitstream(agmv,img_entry);
		}
		else{
			AGMV_AssemblePFrameBitstream(agmv,img_entry);
		}

		AGMV_WriteLong(file,agmv->bitstream->pos);
		AGMV_WriteLong(file,0);

		pos = ftell(file);

		if(AGMV_GetCompression(agmv) == AGMV_LZSS_COMPRESSION){
			csize = AGMV_LZSS(file,agmv->bitstream);
		}
		else{
			csize = AGMV_LZ77(file,agmv->bitstream);
		}

		AGMV_FlushWriteBits();

		fseek(file,pos-4,SEEK_SET);

		AGMV_WriteLong(file,csize);

		fseek(file,csize,SEEK_CUR);

	}
	else{
		for(i = 0; i < size; i++){
			img_entry[i].index = AGMV_FindNearestColor(palette0,img_data[i]);
			img_entry[i].pal_num = 0;
		}

		if(agmv->frame_count % 4 == 0){
			AGMV_AssembleIFrameBitstream(agmv,img_entry);
		}
		else{
			AGMV_AssemblePFrameBitstream(agmv,img_entry);
		}

		AGMV_WriteLong(file,agmv->bitstream->pos);
		AGMV_WriteLong(file,0);

		pos = ftell(file);

		if(AGMV_GetCompression(agmv) == AGMV_LZSS_COMPRESSION){
			csize = AGMV_LZSS(file,agmv->bitstream);
		}
		else{
			csize = AGMV_LZ77(file,agmv->bitstream);
		}

		AGMV_FlushWriteBits();

		fseek(file,pos-4,SEEK_SET);

		AGMV_WriteLong(file,csize);

		fseek(file,csize,SEEK_CUR);
	}

	for(i = 0; i < 16; i++){
		AGMV_WriteByte(file,0);
	}

	if(agmv->frame_count % 4 == 0){
		for(i = 0; i < size; i++){
			iframe_entries[i] = img_entry[i];
		}
	}

	free(img_entry);
	agmv->frame_count++;
}

void roundUpEven(u8* num){
	u8 temp = *num;
	while(temp % 2 != 0){
		temp++;
	}
	*num = temp;
}

void roundUpOdd(u8* num){
	u8 temp = *num;
	while(temp % 2 == 0){
		temp++;
	}
	*num = temp;
}

f32 AGMV_Round(const f32 x) {
    if (x >= 0.0)
        return floor(x + 0.5);
    return ceil(x - 0.5);
}

void AGMV_CompressAudio(const AGMV* agmv){
	const int size = AGMV_GetAudioSize(agmv);
	u8 ssqrt1, ssqrt2, shift, *atsample = agmv->audio_chunk->atsample;
	const u16* pcm = agmv->audio_track->pcm;

	for(int i = 0; i < size; i++){
		int samp = pcm[i];

		ssqrt1 = sqrt(samp);
		ssqrt2 = AGMV_Round(sqrt(samp));
		shift = samp >> 8;

		roundUpEven(&ssqrt1);
	    roundUpEven(&ssqrt2);
	    roundUpOdd(&shift);

		const int resamp1 = ssqrt1 * ssqrt1;
		const int resamp2 = ssqrt2 * ssqrt2;
		const int resamp3 = shift << 8;

		const u32 dist1 = AGMV_Abs(resamp1 - samp);
		const u32 dist2 = AGMV_Abs(resamp2 - samp);
		const u32 dist3 = AGMV_Abs(resamp3 - samp);

	    u32 dist = AGMV_Min(dist1, dist2);
	    dist = AGMV_Min(dist1,dist3);

	    if(dist == dist1){
	    	atsample[i] = ssqrt1;
	    }
	    else if(dist == dist2){
	    	atsample[i] = ssqrt2;
		}
		else{
	    	atsample[i] = shift;
		}
	}
}

void AGMV_EncodeAudioChunk(FILE* file, const AGMV* agmv){
	const int size = agmv->audio_chunk->size;
	const u8* atsample = agmv->audio_chunk->atsample;

	if(AGMV_GetOPT(agmv) != AGMV_OPT_GBA_I && AGMV_GetOPT(agmv) != AGMV_OPT_GBA_II && AGMV_GetOPT(agmv) != AGMV_OPT_GBA_III){

		AGMV_WriteFourCC(file,'A','G','A','C');
		AGMV_WriteLong(file,agmv->audio_chunk->size);

		for(int i = 0; i < size; i++){
			AGMV_WriteByte(file,atsample[agmv->audio_track->start_point++]);
		}
	}
}

void AGMV_EncodeVideo(const char* filename, const char* dir, const char* basename, u8 img_type, u32 start_frame, u32 end_frame, u32 width, u32 height, u32 frames_per_second, AGMV_OPT opt, AGMV_QUALITY quality, AGMV_COMPRESSION compression){
	u32 i, palette0[256], palette1[256], n, count = 0, num_of_frames_encoded = 0, w, h, num_of_pix, max_clr;
	u32 pal[512];

	AGMV* agmv = CreateAGMV(end_frame-start_frame,width,height,frames_per_second);
	AGMV_SetOPT(agmv,opt);
	AGMV_SetCompression(agmv,compression);

	switch(quality){
		case AGMV_HIGH_QUALITY:{
			max_clr = AGMV_MAX_CLR;
		}break;
		case AGMV_MID_QUALITY:{
			max_clr = 131071;
		}break;
		case AGMV_LOW_QUALITY:{
			max_clr = 65535;
		}break;
		default:{
			max_clr = AGMV_MAX_CLR;
		}break;
	}

	u32* colorgram = malloc(sizeof(u32)*max_clr);
	u32* histogram = malloc(sizeof(u32)*max_clr);

	switch(opt){
		case AGMV_OPT_I:{
			AGMV_SetLeniency(agmv,0.2282);
		}break;
		case AGMV_OPT_II:{
			AGMV_SetLeniency(agmv,0.1282);
		}break;
		case AGMV_OPT_III:{
			AGMV_SetLeniency(agmv,0.2282);
		}break;
		case AGMV_OPT_ANIM:{
			AGMV_SetLeniency(agmv,0.2282);
		}break;
		case AGMV_OPT_GBA_I:{
			AGMV_SetLeniency(agmv,0.0);

			AGMV_SetWidth(agmv,AGMV_GBA_W);
			AGMV_SetHeight(agmv,AGMV_GBA_H);

			free(agmv->frame->img_data);
			agmv->frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_GBA_W*AGMV_GBA_H);
		}break;
		case AGMV_OPT_GBA_II:{
			AGMV_SetLeniency(agmv,0.0);

			AGMV_SetWidth(agmv,AGMV_GBA_W);
			AGMV_SetHeight(agmv,AGMV_GBA_H);

			free(agmv->frame->img_data);
			agmv->frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_GBA_W*AGMV_GBA_H);
		}break;
		case AGMV_OPT_GBA_III:{
			AGMV_SetLeniency(agmv,0.0f);

			AGMV_SetWidth(agmv,AGMV_GBA_W);
			AGMV_SetHeight(agmv,AGMV_GBA_H);

			free(agmv->frame->img_data);
			agmv->frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_GBA_W*AGMV_GBA_H);
		}break;
		case AGMV_OPT_NDS:{
			AGMV_SetLeniency(agmv,0.2282);

			AGMV_SetWidth(agmv,AGMV_NDS_W);
			AGMV_SetHeight(agmv,AGMV_NDS_H);

			free(agmv->frame->img_data);
			agmv->frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_NDS_W*AGMV_NDS_H);
		}break;
		default:{
			AGMV_SetLeniency(agmv,0.2282);
		}break;
	}

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

	char* ext = AGIDL_GetImgExtension(img_type);

	for(i = start_frame; i <= end_frame; i++){
		char tmpFilename[60];
		if(dir[0] != 'c' || dir[1] != 'u' || dir[2] != 'r'){
			sprintf(tmpFilename,"%s/%s%ld%s",dir,basename,i,ext);
		}
		else{
			sprintf(tmpFilename,"%s%ld%s",basename,i,ext);
		}

		switch(img_type){
			case AGIDL_IMG_BMP:{
				u32* pixels;

				AGIDL_BMP* bmp = AGIDL_LoadBMP(tmpFilename);
				AGIDL_ColorConvertBMP(bmp,AGIDL_RGB_888);

				pixels = bmp->pixels.pix32;


				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreeBMP(bmp);
			}break;
			case AGIDL_IMG_TGA:{
				u32* pixels;

				AGIDL_TGA* tga = AGIDL_LoadTGA(tmpFilename);
				AGIDL_ColorConvertTGA(tga,AGIDL_RGB_888);

				pixels = tga->pixels.pix32;


				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreeTGA(tga);
			}break;
			case AGIDL_IMG_TIM:{
				u32* pixels;

				AGIDL_TIM* tim = AGIDL_LoadTIM(tmpFilename);

				pixels = tim->pixels.pix32;


				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreeTIM(tim);
			}break;
			case AGIDL_IMG_PCX:{
				u32* pixels;

				AGIDL_PCX* pcx = AGIDL_LoadPCX(tmpFilename);
				AGIDL_ColorConvertPCX(pcx,AGIDL_RGB_888);

				pixels = pcx->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreePCX(pcx);
			}break;
			case AGIDL_IMG_LMP:{
				u32* pixels;

				AGIDL_LMP* lmp = AGIDL_LoadLMP(tmpFilename);
				AGIDL_ColorConvertLMP(lmp,AGIDL_RGB_888);

				pixels = lmp->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreeLMP(lmp);
			}break;
			case AGIDL_IMG_PVR:{
				u32* pixels;

				AGIDL_PVR* pvr = AGIDL_LoadPVR(tmpFilename);
				AGIDL_ColorConvertPVR(pvr,AGIDL_RGB_888);

				pixels = pvr->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreePVR(pvr);
			}break;
			case AGIDL_IMG_GXT:{
				u32* pixels;

				AGIDL_GXT* gxt = AGIDL_LoadGXT(tmpFilename);
				AGIDL_ColorConvertGXT(gxt,AGIDL_RGB_888);

				pixels = gxt->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreeGXT(gxt);
			}break;
			case AGIDL_IMG_BTI:{
				u32* pixels;

				AGIDL_BTI* bti = AGIDL_LoadBTI(tmpFilename);
				AGIDL_ColorConvertBTI(bti,AGIDL_RGB_888);

				pixels = bti->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreeBTI(bti);
			}break;
			case AGIDL_IMG_3DF:{
				u32* pixels;

				AGIDL_3DF* glide = AGIDL_Load3DF(tmpFilename);
				AGIDL_ColorConvert3DF(glide,AGIDL_RGB_888);

				pixels = glide->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_Free3DF(glide);
			}break;
			case AGIDL_IMG_PPM:{
				u32* pixels;

				AGIDL_PPM* ppm = AGIDL_LoadPPM(tmpFilename);
				AGIDL_ColorConvertPPM(ppm,AGIDL_RGB_888);

				pixels = ppm->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreePPM(ppm);
			}break;
			case AGIDL_IMG_LBM:{
				u32* pixels;

				AGIDL_LBM* lbm = AGIDL_LoadLBM(tmpFilename);
				AGIDL_ColorConvertLBM(lbm,AGIDL_RGB_888);

				pixels = lbm->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreeLBM(lbm);
			}break;
		}
	}

	AGMV_QuickSort(histogram,colorgram,0,max_clr);

	for(n = max_clr; n > 0; n--){
		Bool skip = FALSE;

		u32 clr = colorgram[n];

		int r = AGMV_GetQuantizedR(clr,quality);
		int g = AGMV_GetQuantizedG(clr,quality);
		int b = AGMV_GetQuantizedB(clr,quality);

		int j;
		for(j = 0; j < 512; j++){
			u32 palclr = pal[j];

			int palr = AGMV_GetQuantizedR(palclr,quality);
			int palg = AGMV_GetQuantizedG(palclr,quality);
			int palb = AGMV_GetQuantizedB(palclr,quality);

			int rdiff = r-palr;
			int gdiff = g-palg;
			int bdiff = b-palb;

			if(rdiff < 0){
				rdiff = AGIDL_Abs(rdiff);
			}

			if(gdiff < 0){
				gdiff = AGIDL_Abs(gdiff);
			}

			if(bdiff < 0){
				bdiff = AGIDL_Abs(bdiff);
			}

			if(quality == AGMV_HIGH_QUALITY){
				if(rdiff <= 2 && gdiff <= 2 && bdiff <= 3){
					skip = TRUE;
				}
			}
			else{
				if(rdiff <= 1 && gdiff <= 1 && bdiff <= 1){
					skip = TRUE;
				}
			}
		}

		if(skip == FALSE){
			pal[count] = clr;
			count++;
		}

		if(count >= 512){
			break;
		}
	}

	if(opt == AGMV_OPT_I || opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_III || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_NDS){
		for(n = 0; n < 512; n++){
			u32 clr = pal[n];
			u32 invclr = AGMV_ReverseQuantizeColor(clr,quality);

			if(n < 126){
				palette0[n] = invclr;
			}
			else if(n >= 126 && n <= 252){
				palette1[n-126] = invclr;
			}

			if(n > 252 && n <= 378){
				palette0[n-126] = invclr;
			}

			if(n > 378){
				palette1[n-252] = invclr;
			}
		}
	}

	if(opt == AGMV_OPT_II || opt == AGMV_OPT_GBA_II|| opt == AGMV_OPT_ANIM){
		for(n = 0; n < 256; n++){
			u32 clr = pal[n];
			u32 invclr = AGMV_ReverseQuantizeColor(clr,quality);

			palette0[n] = invclr;
		}
	}

	free(colorgram);
	free(histogram);

	FILE* file = fopen(filename,"wb");

	AGMV_SetICP0(agmv,palette0);
	AGMV_SetICP1(agmv,palette1);

	printf("Encoding AGMV Header...\n");
	AGMV_EncodeHeader(file,agmv);
	printf("Encoded AGMV Header...\n");

	for(i = start_frame; i <= end_frame;){
		char filename1[60],filename2[60],filename3[60],filename4[60];
		if(dir[0] != 'c' || dir[1] != 'u' || dir[2] != 'r'){
			sprintf(filename1,"%s/%s%ld%s",dir,basename,i,ext);
			sprintf(filename2,"%s/%s%ld%s",dir,basename,i+1,ext);
			sprintf(filename3,"%s/%s%ld%s",dir,basename,i+2,ext);
			sprintf(filename4,"%s/%s%ld%s",dir,basename,i+3,ext);
		}
		else{
			sprintf(filename1,"%s%ld%s",basename,i,ext);
			sprintf(filename2,"%s%ld%s",basename,i+1,ext);
			sprintf(filename3,"%s%ld%s",basename,i+2,ext);
			sprintf(filename4,"%s%ld%s",basename,i+3,ext);
		}

		switch(img_type){
			case AGIDL_IMG_BMP:{
				printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_BMP* frame1 = AGIDL_LoadBMP(filename1);
				AGIDL_BMP* frame2 = AGIDL_LoadBMP(filename2);
				AGIDL_BMP* frame3 = AGIDL_LoadBMP(filename3);
				AGIDL_BMP* frame4 = AGIDL_LoadBMP(filename4);
				printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);

				AGIDL_ColorConvertBMP(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertBMP(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertBMP(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertBMP(frame4,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					w = AGIDL_BMPGetWidth(frame1), h = AGIDL_BMPGetHeight(frame1);
					AGIDL_ScaleBMP(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleBMP(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleBMP(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleBMP(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					w = AGIDL_BMPGetWidth(frame1), h = AGIDL_BMPGetHeight(frame1);
					AGIDL_FastScaleBMP(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
				}

				w = AGIDL_BMPGetWidth(frame2), h = AGIDL_BMPGetHeight(frame2);

				num_of_pix = w*h;

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II){

					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					f32 ratio = AGMV_CompareFrameSimilarity(frame2->pixels.pix32,frame3->pixels.pix32,w,h);

					if(ratio >= AGMV_GetLeniency(agmv)){
						u32* interp = malloc(sizeof(u32)*num_of_pix);

						AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);

						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
						AGMV_EncodeFrame(file,agmv,interp);
						printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i+3);

						num_of_frames_encoded += 3; i += 4;

						free(interp);
					}
					else{
						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					f32 ratio = AGMV_CompareFrameSimilarity(frame1->pixels.pix32,frame2->pixels.pix32,w,h);

					if(ratio >= AGMV_GetLeniency(agmv)){
						u32* interp = malloc(sizeof(u32)*num_of_pix);

						AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);

						printf("Encoding Interpolated Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,interp);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);

						num_of_frames_encoded++; i += 2;

						free(interp);
					}
					else{
						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}

				    printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeBMP(frame1);
				AGIDL_FreeBMP(frame2);
				AGIDL_FreeBMP(frame3);
				AGIDL_FreeBMP(frame4);
			}break;
			case AGIDL_IMG_TGA:{
				printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_TGA* frame1 = AGIDL_LoadTGA(filename1);
				AGIDL_TGA* frame2 = AGIDL_LoadTGA(filename2);
				AGIDL_TGA* frame3 = AGIDL_LoadTGA(filename3);
				AGIDL_TGA* frame4 = AGIDL_LoadTGA(filename4);
				printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);

				AGIDL_ColorConvertTGA(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertTGA(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertTGA(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertTGA(frame4,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					w = AGIDL_TGAGetWidth(frame1), h = AGIDL_TGAGetHeight(frame1);
					AGIDL_ScaleTGA(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTGA(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTGA(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTGA(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					w = AGIDL_TGAGetWidth(frame1), h = AGIDL_TGAGetHeight(frame1);
					AGIDL_ScaleTGA(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTGA(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTGA(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTGA(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				w = AGIDL_TGAGetWidth(frame2), h = AGIDL_TGAGetHeight(frame2);

				num_of_pix = w*h;

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_NDS){

					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					f32 ratio = AGMV_CompareFrameSimilarity(frame2->pixels.pix32,frame3->pixels.pix32,w,h);

					if(ratio >= AGMV_GetLeniency(agmv)){
						u32* interp = malloc(sizeof(u32)*num_of_pix);

						AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);

						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
						AGMV_EncodeFrame(file,agmv,interp);
						printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i+3);

						num_of_frames_encoded += 3; i += 4;

						free(interp);
					}
					else{
						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					f32 ratio = AGMV_CompareFrameSimilarity(frame1->pixels.pix32,frame2->pixels.pix32,w,h);

					if(ratio >= AGMV_GetLeniency(agmv)){
						u32* interp = malloc(sizeof(u32)*num_of_pix);

						AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);

						printf("Encoding Interpolated Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,interp);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);

						num_of_frames_encoded++; i += 2;

						free(interp);
					}
					else{
						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeTGA(frame1);
				AGIDL_FreeTGA(frame2);
				AGIDL_FreeTGA(frame3);
				AGIDL_FreeTGA(frame4);
			}break;
			case AGIDL_IMG_TIM:{
				printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_TIM* frame1 = AGIDL_LoadTIM(filename1);
				AGIDL_TIM* frame2 = AGIDL_LoadTIM(filename2);
				AGIDL_TIM* frame3 = AGIDL_LoadTIM(filename3);
				AGIDL_TIM* frame4 = AGIDL_LoadTIM(filename4);
				printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);

				AGIDL_ColorConvertTIM(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertTIM(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertTIM(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertTIM(frame4,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					w = AGIDL_TIMGetWidth(frame1), h = AGIDL_TIMGetHeight(frame1);
					AGIDL_ScaleTIM(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTIM(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTIM(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTIM(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					w = AGIDL_TIMGetWidth(frame1), h = AGIDL_TIMGetHeight(frame1);
					AGIDL_ScaleTIM(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTIM(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTIM(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTIM(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				w = AGIDL_TIMGetWidth(frame2), h = AGIDL_TIMGetHeight(frame2);

				num_of_pix = w*h;

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_NDS){

					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					f32 ratio = AGMV_CompareFrameSimilarity(frame2->pixels.pix32,frame3->pixels.pix32,w,h);

					if(ratio >= AGMV_GetLeniency(agmv)){
						u32* interp = malloc(sizeof(u32)*num_of_pix);

						AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);

						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
						AGMV_EncodeFrame(file,agmv,interp);
						printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i+3);

						num_of_frames_encoded += 3; i += 4;

						free(interp);
					}
					else{
						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					f32 ratio = AGMV_CompareFrameSimilarity(frame1->pixels.pix32,frame2->pixels.pix32,w,h);

					if(ratio >= AGMV_GetLeniency(agmv)){
						u32* interp = malloc(sizeof(u32)*num_of_pix);

						AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);

						printf("Encoding Interpolated Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,interp);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);

						num_of_frames_encoded++; i += 2;

						free(interp);
					}
					else{
						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeTIM(frame1);
				AGIDL_FreeTIM(frame2);
				AGIDL_FreeTIM(frame3);
				AGIDL_FreeTIM(frame4);
			}break;
			case AGIDL_IMG_PCX:{
				printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_PCX* frame1 = AGIDL_LoadPCX(filename1);
				AGIDL_PCX* frame2 = AGIDL_LoadPCX(filename2);
				AGIDL_PCX* frame3 = AGIDL_LoadPCX(filename3);
				AGIDL_PCX* frame4 = AGIDL_LoadPCX(filename4);
				printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);

				AGIDL_ColorConvertPCX(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertPCX(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertPCX(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertPCX(frame4,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					w = AGIDL_PCXGetWidth(frame1), h = AGIDL_PCXGetHeight(frame1);
					AGIDL_ScalePCX(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePCX(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePCX(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePCX(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					w = AGIDL_PCXGetWidth(frame1), h = AGIDL_PCXGetHeight(frame1);
					AGIDL_ScalePCX(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePCX(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePCX(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePCX(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				w = AGIDL_PCXGetWidth(frame2), h = AGIDL_PCXGetHeight(frame2);

				num_of_pix = w*h;

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_NDS){

					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					f32 ratio = AGMV_CompareFrameSimilarity(frame2->pixels.pix32,frame3->pixels.pix32,w,h);

					if(ratio >= AGMV_GetLeniency(agmv)){
						u32* interp = malloc(sizeof(u32)*num_of_pix);

						AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);

						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
						AGMV_EncodeFrame(file,agmv,interp);
						printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i+3);

						num_of_frames_encoded += 3; i += 4;

						free(interp);
					}
					else{
						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					f32 ratio = AGMV_CompareFrameSimilarity(frame1->pixels.pix32,frame2->pixels.pix32,w,h);

					if(ratio >= AGMV_GetLeniency(agmv)){
						u32* interp = malloc(sizeof(u32)*num_of_pix);

						AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);

						printf("Encoding Interpolated Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,interp);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);

						num_of_frames_encoded++; i += 2;

						free(interp);
					}
					else{
						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreePCX(frame1);
				AGIDL_FreePCX(frame2);
				AGIDL_FreePCX(frame3);
				AGIDL_FreePCX(frame4);
			}break;
			case AGIDL_IMG_LMP:{
				printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_LMP* frame1 = AGIDL_LoadLMP(filename1);
				AGIDL_LMP* frame2 = AGIDL_LoadLMP(filename2);
				AGIDL_LMP* frame3 = AGIDL_LoadLMP(filename3);
				AGIDL_LMP* frame4 = AGIDL_LoadLMP(filename4);
				printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);

				AGIDL_ColorConvertLMP(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertLMP(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertLMP(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertLMP(frame4,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					w = AGIDL_LMPGetWidth(frame1), h = AGIDL_LMPGetHeight(frame1);
					AGIDL_ScaleLMP(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLMP(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLMP(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLMP(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					w = AGIDL_LMPGetWidth(frame1), h = AGIDL_LMPGetHeight(frame1);
					AGIDL_ScaleLMP(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLMP(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLMP(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLMP(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				w = AGIDL_LMPGetWidth(frame2), h = AGIDL_LMPGetHeight(frame2);

				num_of_pix = w*h;

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_NDS){

					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					f32 ratio = AGMV_CompareFrameSimilarity(frame2->pixels.pix32,frame3->pixels.pix32,w,h);

					if(ratio >= AGMV_GetLeniency(agmv)){
						u32* interp = malloc(sizeof(u32)*num_of_pix);

						AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);

						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
						AGMV_EncodeFrame(file,agmv,interp);
						printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i+3);

						num_of_frames_encoded += 3; i += 4;

						free(interp);
					}
					else{
						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					f32 ratio = AGMV_CompareFrameSimilarity(frame1->pixels.pix32,frame2->pixels.pix32,w,h);

					if(ratio >= AGMV_GetLeniency(agmv)){
						u32* interp = malloc(sizeof(u32)*num_of_pix);

						AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);

						printf("Encoding Interpolated Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,interp);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);

						num_of_frames_encoded++; i += 2;

						free(interp);
					}
					else{
						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeLMP(frame1);
				AGIDL_FreeLMP(frame2);
				AGIDL_FreeLMP(frame3);
				AGIDL_FreeLMP(frame4);
			}break;
			case AGIDL_IMG_PVR:{
				printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_PVR* frame1 = AGIDL_LoadPVR(filename1);
				AGIDL_PVR* frame2 = AGIDL_LoadPVR(filename2);
				AGIDL_PVR* frame3 = AGIDL_LoadPVR(filename3);
				AGIDL_PVR* frame4 = AGIDL_LoadPVR(filename4);
				printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);

				AGIDL_ColorConvertPVR(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertPVR(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertPVR(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertPVR(frame4,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					w = AGIDL_PVRGetWidth(frame1), h = AGIDL_PVRGetHeight(frame1);
					AGIDL_ScalePVR(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePVR(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePVR(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePVR(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					w = AGIDL_PVRGetWidth(frame1), h = AGIDL_PVRGetHeight(frame1);
					AGIDL_ScalePVR(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePVR(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePVR(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePVR(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				w = AGIDL_PVRGetWidth(frame2), h = AGIDL_PVRGetHeight(frame2);

				num_of_pix = w*h;

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_NDS){

					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					f32 ratio = AGMV_CompareFrameSimilarity(frame2->pixels.pix32,frame3->pixels.pix32,w,h);

					if(ratio >= AGMV_GetLeniency(agmv)){
						u32* interp = malloc(sizeof(u32)*num_of_pix);

						AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);

						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
						AGMV_EncodeFrame(file,agmv,interp);
						printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i+3);

						num_of_frames_encoded += 3; i += 4;

						free(interp);
					}
					else{
						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					f32 ratio = AGMV_CompareFrameSimilarity(frame1->pixels.pix32,frame2->pixels.pix32,w,h);

					if(ratio >= AGMV_GetLeniency(agmv)){
						u32* interp = malloc(sizeof(u32)*num_of_pix);

						AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);

						printf("Encoding Interpolated Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,interp);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);

						num_of_frames_encoded++; i += 2;

						free(interp);
					}
					else{
						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreePVR(frame1);
				AGIDL_FreePVR(frame2);
				AGIDL_FreePVR(frame3);
				AGIDL_FreePVR(frame4);
			}break;
			case AGIDL_IMG_GXT:{
				printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_GXT* frame1 = AGIDL_LoadGXT(filename1);
				AGIDL_GXT* frame2 = AGIDL_LoadGXT(filename2);
				AGIDL_GXT* frame3 = AGIDL_LoadGXT(filename3);
				AGIDL_GXT* frame4 = AGIDL_LoadGXT(filename4);
				printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);

				AGIDL_ColorConvertGXT(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertGXT(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertGXT(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertGXT(frame4,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					w = AGIDL_GXTGetWidth(frame1), h = AGIDL_GXTGetHeight(frame1);
					AGIDL_ScaleGXT(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleGXT(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleGXT(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleGXT(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					w = AGIDL_GXTGetWidth(frame1), h = AGIDL_GXTGetHeight(frame1);
					AGIDL_ScaleGXT(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleGXT(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleGXT(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleGXT(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				w = AGIDL_GXTGetWidth(frame2), h = AGIDL_GXTGetHeight(frame2);

				num_of_pix = w*h;

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_NDS){

					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					f32 ratio = AGMV_CompareFrameSimilarity(frame2->pixels.pix32,frame3->pixels.pix32,w,h);

					if(ratio >= AGMV_GetLeniency(agmv)){
						u32* interp = malloc(sizeof(u32)*num_of_pix);

						AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);

						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
						AGMV_EncodeFrame(file,agmv,interp);
						printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i+3);

						num_of_frames_encoded += 3; i += 4;

						free(interp);
					}
					else{
						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					f32 ratio = AGMV_CompareFrameSimilarity(frame1->pixels.pix32,frame2->pixels.pix32,w,h);

					if(ratio >= AGMV_GetLeniency(agmv)){
						u32* interp = malloc(sizeof(u32)*num_of_pix);

						AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);

						printf("Encoding Interpolated Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,interp);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);

						num_of_frames_encoded++; i += 2;

						free(interp);
					}
					else{
						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeGXT(frame1);
				AGIDL_FreeGXT(frame2);
				AGIDL_FreeGXT(frame3);
				AGIDL_FreeGXT(frame4);
			}break;
			case AGIDL_IMG_BTI:{
				printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_BTI* frame1 = AGIDL_LoadBTI(filename1);
				AGIDL_BTI* frame2 = AGIDL_LoadBTI(filename2);
				AGIDL_BTI* frame3 = AGIDL_LoadBTI(filename3);
				AGIDL_BTI* frame4 = AGIDL_LoadBTI(filename4);
				printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);

				AGIDL_ColorConvertBTI(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertBTI(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertBTI(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertBTI(frame4,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					w = AGIDL_BTIGetWidth(frame1), h = AGIDL_BTIGetHeight(frame1);
					AGIDL_ScaleBTI(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleBTI(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleBTI(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleBTI(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					w = AGIDL_BTIGetWidth(frame1), h = AGIDL_BTIGetHeight(frame1);
					AGIDL_ScaleBTI(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleBTI(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleBTI(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleBTI(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				w = AGIDL_BTIGetWidth(frame2), h = AGIDL_BTIGetHeight(frame2);

				num_of_pix = w*h;

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_NDS){

					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					f32 ratio = AGMV_CompareFrameSimilarity(frame2->pixels.pix32,frame3->pixels.pix32,w,h);

					if(ratio >= AGMV_GetLeniency(agmv)){
						u32* interp = malloc(sizeof(u32)*num_of_pix);

						AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);

						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
						AGMV_EncodeFrame(file,agmv,interp);
						printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i+3);

						num_of_frames_encoded += 3; i += 4;

						free(interp);
					}
					else{
						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					f32 ratio = AGMV_CompareFrameSimilarity(frame1->pixels.pix32,frame2->pixels.pix32,w,h);

					if(ratio >= AGMV_GetLeniency(agmv)){
						u32* interp = malloc(sizeof(u32)*num_of_pix);

						AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);

						printf("Encoding Interpolated Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,interp);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);

						num_of_frames_encoded++; i += 2;

						free(interp);
					}
					else{
						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeBTI(frame1);
				AGIDL_FreeBTI(frame2);
				AGIDL_FreeBTI(frame3);
				AGIDL_FreeBTI(frame4);
			}break;
			case AGIDL_IMG_3DF:{
				printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_3DF* frame1 = AGIDL_Load3DF(filename1);
				AGIDL_3DF* frame2 = AGIDL_Load3DF(filename2);
				AGIDL_3DF* frame3 = AGIDL_Load3DF(filename3);
				AGIDL_3DF* frame4 = AGIDL_Load3DF(filename4);
				printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);

				AGIDL_ColorConvert3DF(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvert3DF(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvert3DF(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvert3DF(frame4,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					w = AGIDL_3DFGetWidth(frame1), h = AGIDL_3DFGetHeight(frame1);
					AGIDL_Scale3DF(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_Scale3DF(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_Scale3DF(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_Scale3DF(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					w = AGIDL_3DFGetWidth(frame1), h = AGIDL_3DFGetHeight(frame1);
					AGIDL_Scale3DF(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_Scale3DF(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_Scale3DF(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_Scale3DF(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				w = AGIDL_3DFGetWidth(frame2), h = AGIDL_3DFGetHeight(frame2);

				num_of_pix = w*h;

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_NDS){

					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					f32 ratio = AGMV_CompareFrameSimilarity(frame2->pixels.pix32,frame3->pixels.pix32,w,h);

					if(ratio >= AGMV_GetLeniency(agmv)){
						u32* interp = malloc(sizeof(u32)*num_of_pix);

						AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);

						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
						AGMV_EncodeFrame(file,agmv,interp);
						printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i+3);

						num_of_frames_encoded += 3; i += 4;

						free(interp);
					}
					else{
						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					f32 ratio = AGMV_CompareFrameSimilarity(frame1->pixels.pix32,frame2->pixels.pix32,w,h);

					if(ratio >= AGMV_GetLeniency(agmv)){
						u32* interp = malloc(sizeof(u32)*num_of_pix);

						AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);

						printf("Encoding Interpolated Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,interp);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);

						num_of_frames_encoded++; i += 2;

						free(interp);
					}
					else{
						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_Free3DF(frame1);
				AGIDL_Free3DF(frame2);
				AGIDL_Free3DF(frame3);
				AGIDL_Free3DF(frame4);
			}break;
			case AGIDL_IMG_PPM:{
				printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_PPM* frame1 = AGIDL_LoadPPM(filename1);
				AGIDL_PPM* frame2 = AGIDL_LoadPPM(filename2);
				AGIDL_PPM* frame3 = AGIDL_LoadPPM(filename3);
				AGIDL_PPM* frame4 = AGIDL_LoadPPM(filename4);
				printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);

				AGIDL_ColorConvertPPM(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertPPM(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertPPM(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertPPM(frame4,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					w = AGIDL_PPMGetWidth(frame1), h = AGIDL_PPMGetHeight(frame1);
					AGIDL_ScalePPM(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePPM(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePPM(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePPM(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					w = AGIDL_PPMGetWidth(frame1), h = AGIDL_PPMGetHeight(frame1);
					AGIDL_ScalePPM(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePPM(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePPM(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePPM(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				w = AGIDL_PPMGetWidth(frame2), h = AGIDL_PPMGetHeight(frame2);

				num_of_pix = w*h;

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_NDS){

					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					f32 ratio = AGMV_CompareFrameSimilarity(frame2->pixels.pix32,frame3->pixels.pix32,w,h);

					if(ratio >= AGMV_GetLeniency(agmv)){
						u32* interp = malloc(sizeof(u32)*num_of_pix);

						AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);

						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
						AGMV_EncodeFrame(file,agmv,interp);
						printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i+3);

						num_of_frames_encoded += 3; i += 4;

						free(interp);
					}
					else{
						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					f32 ratio = AGMV_CompareFrameSimilarity(frame1->pixels.pix32,frame2->pixels.pix32,w,h);

					if(ratio >= AGMV_GetLeniency(agmv)){
						u32* interp = malloc(sizeof(u32)*num_of_pix);

						AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);

						printf("Encoding Interpolated Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,interp);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);

						num_of_frames_encoded++; i += 2;

						free(interp);
					}
					else{
						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreePPM(frame1);
				AGIDL_FreePPM(frame2);
				AGIDL_FreePPM(frame3);
				AGIDL_FreePPM(frame4);
			}break;
			case AGIDL_IMG_LBM:{
				printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_LBM* frame1 = AGIDL_LoadLBM(filename1);
				AGIDL_LBM* frame2 = AGIDL_LoadLBM(filename2);
				AGIDL_LBM* frame3 = AGIDL_LoadLBM(filename3);
				AGIDL_LBM* frame4 = AGIDL_LoadLBM(filename4);
				printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);

				AGIDL_ColorConvertLBM(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertLBM(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertLBM(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertLBM(frame4,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					w = AGIDL_LBMGetWidth(frame1), h = AGIDL_LBMGetHeight(frame1);
					AGIDL_ScaleLBM(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLBM(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLBM(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLBM(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					w = AGIDL_LBMGetWidth(frame1), h = AGIDL_LBMGetHeight(frame1);
					AGIDL_ScaleLBM(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLBM(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLBM(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLBM(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				w = AGIDL_LBMGetWidth(frame2), h = AGIDL_LBMGetHeight(frame2);

				num_of_pix = w*h;

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_NDS){

					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					f32 ratio = AGMV_CompareFrameSimilarity(frame2->pixels.pix32,frame3->pixels.pix32,w,h);

					if(ratio >= AGMV_GetLeniency(agmv)){
						u32* interp = malloc(sizeof(u32)*num_of_pix);

						AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);

						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
						AGMV_EncodeFrame(file,agmv,interp);
						printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i+3);

						num_of_frames_encoded += 3; i += 4;

						free(interp);
					}
					else{
						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					f32 ratio = AGMV_CompareFrameSimilarity(frame1->pixels.pix32,frame2->pixels.pix32,w,h);

					if(ratio >= AGMV_GetLeniency(agmv)){
						u32* interp = malloc(sizeof(u32)*num_of_pix);

						AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);

						printf("Encoding Interpolated Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,interp);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);

						num_of_frames_encoded++; i += 2;

						free(interp);
					}
					else{
						printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
						printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeLBM(frame1);
				AGIDL_FreeLBM(frame2);
				AGIDL_FreeLBM(frame3);
				AGIDL_FreeLBM(frame4);
			}break;
		}

		if(i + 4 >= end_frame){
			break;
		}
	}

	fseek(file,4,SEEK_SET);
	AGIDL_WriteLong(file,num_of_frames_encoded);

	fseek(file,18,SEEK_SET);
	f32 rate = (f32)num_of_frames_encoded/AGMV_GetNumberOfFrames(agmv);
	AGIDL_WriteLong(file,round(AGMV_GetFramesPerSecond(agmv)*rate));

	fclose(file);

	free(ext);
	DestroyAGMV(agmv);

	if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
		file = fopen(filename,"rb");
		fseek(file,0,SEEK_END);
		u32 file_size = ftell(file);
		fseek(file,0,SEEK_SET);
		u8* data = malloc(sizeof(u8)*file_size);
		fread(data,1,file_size,file);
		fclose(file);

		FILE* out = fopen("GBA_GEN_AGMV.h","w");

		fprintf(out,"#ifndef GBA_GEN_AGMV_H\n");
		fprintf(out,"#define GBA_GEN_AGMV_H\n\n");
		fprintf(out,"const unsigned char GBA_AGMV_FILE[%ld] = {\n",file_size);

		for(i = 0; i < file_size; i++){
			if(i != 0 && i % 4000 == 0){
				fprintf(out,"\n");
			}

			fprintf(out,"%d,",data[i]);
		}

		fprintf(out,"};\n\n");
		fprintf(out,"#endif");

		free(data);
		fclose(out);
	}
}

void AGMV_EncodeAGMV(AGMV* agmv, const char* filename, const char* dir, const char* basename, u8 img_type, u32 start_frame, u32 end_frame, u32 width, u32 height, u32 frames_per_second, AGMV_OPT opt, AGMV_QUALITY quality, AGMV_COMPRESSION compression){
	u32 i, palette0[256], palette1[256], n, count = 0, num_of_frames_encoded = 0, w, h, num_of_pix, max_clr;
	u32 sample_size, adjusted_num_of_frames = end_frame-start_frame;
	u32 pal[512];

	AGMV_SetOPT(agmv,opt);
	AGMV_SetCompression(agmv,compression);

	switch(quality){
		case AGMV_HIGH_QUALITY:{
			max_clr = AGMV_MAX_CLR;
		}break;
		case AGMV_MID_QUALITY:{
			max_clr = 131071;
		}break;
		case AGMV_LOW_QUALITY:{
			max_clr = 65535;
		}break;
		default:{
			max_clr = AGMV_MAX_CLR;
		}break;
	}

	u32* colorgram = malloc(sizeof(u32)*max_clr);
	u32* histogram = malloc(sizeof(u32)*max_clr);

	switch(opt){
		case AGMV_OPT_I:{
			AGMV_SetLeniency(agmv,0); adjusted_num_of_frames /= 2;
		}break;
		case AGMV_OPT_II:{
			AGMV_SetLeniency(agmv,0); adjusted_num_of_frames *= 0.75;
		}break;
		case AGMV_OPT_III:{
			AGMV_SetLeniency(agmv,0); adjusted_num_of_frames *= 0.75;
		}break;
		case AGMV_OPT_ANIM:{
			AGMV_SetLeniency(agmv,0); adjusted_num_of_frames /= 2;
		}break;
		case AGMV_OPT_GBA_I:{
			AGMV_SetLeniency(agmv,0);

			AGMV_SetWidth(agmv,AGMV_GBA_W);
			AGMV_SetHeight(agmv,AGMV_GBA_H);

			free(agmv->frame->img_data);
			agmv->frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_GBA_W*AGMV_GBA_H);

			adjusted_num_of_frames /= 2;
		}break;
		case AGMV_OPT_GBA_II:{
			AGMV_SetLeniency(agmv,0);

			AGMV_SetWidth(agmv,AGMV_GBA_W);
			AGMV_SetHeight(agmv,AGMV_GBA_H);

			free(agmv->frame->img_data);
			agmv->frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_GBA_W*AGMV_GBA_H);

			adjusted_num_of_frames /= 2;
		}break;
		case AGMV_OPT_GBA_III:{
			AGMV_SetLeniency(agmv,0);

			AGMV_SetWidth(agmv,AGMV_GBA_W);
			AGMV_SetHeight(agmv,AGMV_GBA_H);

			free(agmv->frame->img_data);
			agmv->frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_GBA_W*AGMV_GBA_H);

			adjusted_num_of_frames *= 0.75f;
		}break;
		case AGMV_OPT_NDS:{
			AGMV_SetLeniency(agmv,0);

			AGMV_SetWidth(agmv,AGMV_NDS_W);
			AGMV_SetHeight(agmv,AGMV_NDS_H);

			free(agmv->frame->img_data);
			agmv->frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_NDS_W*AGMV_NDS_H);

			adjusted_num_of_frames *= 0.75;
		}break;
	}

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

	char* ext = AGIDL_GetImgExtension(img_type);

	for(i = start_frame; i <= end_frame; i++){
		char tmpFilename[60];
		if(dir[0] != 'c' || dir[1] != 'u' || dir[2] != 'r'){
			sprintf(tmpFilename,"%s/%s%ld%s",dir,basename,i,ext);
		}
		else{
			sprintf(tmpFilename,"%s%ld%s",basename,i,ext);
		}

		switch(img_type){
			case AGIDL_IMG_BMP:{
				u32* pixels;

				AGIDL_BMP* bmp = AGIDL_LoadBMP(tmpFilename);
				AGIDL_ColorConvertBMP(bmp,AGIDL_RGB_888);

				pixels = bmp->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreeBMP(bmp);
			}break;
			case AGIDL_IMG_TGA:{
				u32* pixels;

				AGIDL_TGA* tga = AGIDL_LoadTGA(tmpFilename);
				AGIDL_ColorConvertTGA(tga,AGIDL_RGB_888);

				pixels = tga->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreeTGA(tga);
			}break;
			case AGIDL_IMG_TIM:{
				u32* pixels;

				AGIDL_TIM* tim = AGIDL_LoadTIM(tmpFilename);

				pixels = tim->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreeTIM(tim);
			}break;
			case AGIDL_IMG_PCX:{
				u32* pixels;

				AGIDL_PCX* pcx = AGIDL_LoadPCX(tmpFilename);
				AGIDL_ColorConvertPCX(pcx,AGIDL_RGB_888);

				pixels = pcx->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreePCX(pcx);
			}break;
			case AGIDL_IMG_LMP:{
				u32* pixels;

				AGIDL_LMP* lmp = AGIDL_LoadLMP(tmpFilename);
				AGIDL_ColorConvertLMP(lmp,AGIDL_RGB_888);

				pixels = lmp->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreeLMP(lmp);
			}break;
			case AGIDL_IMG_PVR:{
				u32* pixels;

				AGIDL_PVR* pvr = AGIDL_LoadPVR(tmpFilename);
				AGIDL_ColorConvertPVR(pvr,AGIDL_RGB_888);

				pixels = pvr->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreePVR(pvr);
			}break;
			case AGIDL_IMG_GXT:{
				u32* pixels;

				AGIDL_GXT* gxt = AGIDL_LoadGXT(tmpFilename);
				AGIDL_ColorConvertGXT(gxt,AGIDL_RGB_888);

				pixels = gxt->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreeGXT(gxt);
			}break;
			case AGIDL_IMG_BTI:{
				u32* pixels;

				AGIDL_BTI* bti = AGIDL_LoadBTI(tmpFilename);
				AGIDL_ColorConvertBTI(bti,AGIDL_RGB_888);

				pixels = bti->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreeBTI(bti);
			}break;
			case AGIDL_IMG_3DF:{
				u32* pixels;

				AGIDL_3DF* glide = AGIDL_Load3DF(tmpFilename);
				AGIDL_ColorConvert3DF(glide,AGIDL_RGB_888);

				pixels = glide->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_Free3DF(glide);
			}break;
			case AGIDL_IMG_PPM:{
				u32* pixels;

				AGIDL_PPM* ppm = AGIDL_LoadPPM(tmpFilename);
				AGIDL_ColorConvertPPM(ppm,AGIDL_RGB_888);

				pixels = ppm->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreePPM(ppm);
			}break;
			case AGIDL_IMG_LBM:{
				u32* pixels;

				AGIDL_LBM* lbm = AGIDL_LoadLBM(tmpFilename);
				AGIDL_ColorConvertLBM(lbm,AGIDL_RGB_888);

				pixels = lbm->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreeLBM(lbm);
			}break;
		}
	}

	AGMV_QuickSort(histogram,colorgram,0,max_clr);

	for(n = max_clr; n > 0; n--){
		Bool skip = FALSE;

		u32 clr = colorgram[n];

		int r = AGMV_GetQuantizedR(clr,quality);
		int g = AGMV_GetQuantizedG(clr,quality);
		int b = AGMV_GetQuantizedB(clr,quality);

		int j;
		for(j = 0; j < 512; j++){
			u32 palclr = pal[j];

			int palr = AGMV_GetQuantizedR(palclr,quality);
			int palg = AGMV_GetQuantizedG(palclr,quality);
			int palb = AGMV_GetQuantizedB(palclr,quality);

			int rdiff = r-palr;
			int gdiff = g-palg;
			int bdiff = b-palb;

			if(rdiff < 0){
				rdiff = AGIDL_Abs(rdiff);
			}

			if(gdiff < 0){
				gdiff = AGIDL_Abs(gdiff);
			}

			if(bdiff < 0){
				bdiff = AGIDL_Abs(bdiff);
			}

			if(quality == AGMV_HIGH_QUALITY){
				if(rdiff <= 2 && gdiff <= 2 && bdiff <= 3){
					skip = TRUE;
				}
			}
			else{
				if(rdiff <= 1 && gdiff <= 1 && bdiff <= 1){
					skip = TRUE;
				}
			}
		}

		if(skip == FALSE){
			pal[count] = clr;
			count++;
		}

		if(count >= 512){
			break;
		}
	}

	if(opt == AGMV_OPT_I || opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_III || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_NDS){
		for(n = 0; n < 512; n++){
			u32 clr = pal[n];
			u32 invclr = AGMV_ReverseQuantizeColor(clr,quality);

			if(n < 126){
				palette0[n] = invclr;
			}
			else if(n >= 126 && n <= 252){
				palette1[n-126] = invclr;
			}

			if(n > 252 && n <= 378){
				palette0[n-126] = invclr;
			}

			if(n > 378){
				palette1[n-252] = invclr;
			}
		}
	}

	if(opt == AGMV_OPT_II || opt == AGMV_OPT_GBA_II|| opt == AGMV_OPT_ANIM){
		for(n = 0; n < 256; n++){
			u32 clr = pal[n];
			u32 invclr = AGMV_ReverseQuantizeColor(clr,quality);

			palette0[n] = invclr;
		}
	}

	free(colorgram);
	free(histogram);

	sample_size = agmv->header.audio_size / (f32)adjusted_num_of_frames;

	if(opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II){
		agmv->audio_chunk->size = sample_size;
		agmv->audio_chunk->atsample = (u8*)malloc(sizeof(u8)*agmv->header.audio_size);
		agmv->audio_track->start_point = 0;

		AGMV_CompressAudio(agmv);
	}

	FILE* file = fopen(filename,"wb");

	AGMV_SetICP0(agmv,palette0);
	AGMV_SetICP1(agmv,palette1);

	printf("Encoding AGMV Header...\n");
	AGMV_EncodeHeader(file,agmv);
	printf("Encoded AGMV Header...\n");

	for(i = start_frame; i <= end_frame;){
		char filename1[60],filename2[60],filename3[60],filename4[60];
		if(dir[0] != 'c' || dir[1] != 'u' || dir[2] != 'r'){
			sprintf(filename1,"%s/%s%ld%s",dir,basename,i,ext);
			sprintf(filename2,"%s/%s%ld%s",dir,basename,i+1,ext);
			sprintf(filename3,"%s/%s%ld%s",dir,basename,i+2,ext);
			sprintf(filename4,"%s/%s%ld%s",dir,basename,i+3,ext);
		}
		else{
			sprintf(filename1,"%s%ld%s",basename,i,ext);
			sprintf(filename2,"%s%ld%s",basename,i+1,ext);
			sprintf(filename3,"%s%ld%s",basename,i+2,ext);
			sprintf(filename4,"%s%ld%s",basename,i+3,ext);
		}

		switch(img_type){
			case AGIDL_IMG_BMP:{
				printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_BMP* frame1 = AGIDL_LoadBMP(filename1);
				AGIDL_BMP* frame2 = AGIDL_LoadBMP(filename2);
				AGIDL_BMP* frame3 = AGIDL_LoadBMP(filename3);
				AGIDL_BMP* frame4 = AGIDL_LoadBMP(filename4);
				printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);

				AGIDL_ColorConvertBMP(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertBMP(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertBMP(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertBMP(frame4,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					w = AGIDL_BMPGetWidth(frame1), h = AGIDL_BMPGetHeight(frame1);
					AGIDL_ScaleBMP(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleBMP(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleBMP(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleBMP(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					w = AGIDL_BMPGetWidth(frame1), h = AGIDL_BMPGetHeight(frame1);
					AGIDL_FastScaleBMP(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
				}

				w = AGIDL_BMPGetWidth(frame2), h = AGIDL_BMPGetHeight(frame2);

				num_of_pix = w*h;

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II){

					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					u32* interp = malloc(sizeof(u32)*num_of_pix);

					AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);

					printf("Encoding AGIDL Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i);
					printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
					AGMV_EncodeFrame(file,agmv,interp);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i+3);

					num_of_frames_encoded += 3; i += 4;

					free(interp);

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					u32* interp = malloc(sizeof(u32)*num_of_pix);

					AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);

					printf("Encoding Interpolated Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,interp);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i);

					num_of_frames_encoded++; i += 2;

					free(interp);

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeBMP(frame1);
				AGIDL_FreeBMP(frame2);
				AGIDL_FreeBMP(frame3);
				AGIDL_FreeBMP(frame4);
			}break;
			case AGIDL_IMG_TGA:{
				printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_TGA* frame1 = AGIDL_LoadTGA(filename1);
				AGIDL_TGA* frame2 = AGIDL_LoadTGA(filename2);
				AGIDL_TGA* frame3 = AGIDL_LoadTGA(filename3);
				AGIDL_TGA* frame4 = AGIDL_LoadTGA(filename4);
				printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);

				AGIDL_ColorConvertTGA(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertTGA(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertTGA(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertTGA(frame4,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					w = AGIDL_TGAGetWidth(frame1), h = AGIDL_TGAGetHeight(frame1);
					AGIDL_ScaleTGA(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTGA(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTGA(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTGA(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					w = AGIDL_TGAGetWidth(frame1), h = AGIDL_TGAGetHeight(frame1);
					AGIDL_ScaleTGA(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTGA(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTGA(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTGA(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				w = AGIDL_TGAGetWidth(frame2), h = AGIDL_TGAGetHeight(frame2);

				num_of_pix = w*h;

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_NDS){

					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					u32* interp = malloc(sizeof(u32)*num_of_pix);

					AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);

					printf("Encoding AGIDL Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i);
					printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
					AGMV_EncodeFrame(file,agmv,interp);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i+3);

					num_of_frames_encoded += 3; i += 4;

					free(interp);

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					u32* interp = malloc(sizeof(u32)*num_of_pix);

					AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);

					printf("Encoding Interpolated Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,interp);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i);

					num_of_frames_encoded++; i += 2;

					free(interp);

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeTGA(frame1);
				AGIDL_FreeTGA(frame2);
				AGIDL_FreeTGA(frame3);
				AGIDL_FreeTGA(frame4);
			}break;
			case AGIDL_IMG_TIM:{
				printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_TIM* frame1 = AGIDL_LoadTIM(filename1);
				AGIDL_TIM* frame2 = AGIDL_LoadTIM(filename2);
				AGIDL_TIM* frame3 = AGIDL_LoadTIM(filename3);
				AGIDL_TIM* frame4 = AGIDL_LoadTIM(filename4);
				printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);

				AGIDL_ColorConvertTIM(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertTIM(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertTIM(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertTIM(frame4,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					w = AGIDL_TIMGetWidth(frame1), h = AGIDL_TIMGetHeight(frame1);
					AGIDL_ScaleTIM(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTIM(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTIM(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTIM(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					w = AGIDL_TIMGetWidth(frame1), h = AGIDL_TIMGetHeight(frame1);
					AGIDL_ScaleTIM(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTIM(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTIM(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleTIM(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				w = AGIDL_TIMGetWidth(frame2), h = AGIDL_TIMGetHeight(frame2);

				num_of_pix = w*h;

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_NDS){

					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					u32* interp = malloc(sizeof(u32)*num_of_pix);

					AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);

					printf("Encoding AGIDL Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i);
					printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
					AGMV_EncodeFrame(file,agmv,interp);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i+3);

					num_of_frames_encoded += 3; i += 4;

					free(interp);

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					u32* interp = malloc(sizeof(u32)*num_of_pix);

					AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);

					printf("Encoding Interpolated Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,interp);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i);

					num_of_frames_encoded++; i += 2;

					free(interp);

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeTIM(frame1);
				AGIDL_FreeTIM(frame2);
				AGIDL_FreeTIM(frame3);
				AGIDL_FreeTIM(frame4);
			}break;
			case AGIDL_IMG_PCX:{
				printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_PCX* frame1 = AGIDL_LoadPCX(filename1);
				AGIDL_PCX* frame2 = AGIDL_LoadPCX(filename2);
				AGIDL_PCX* frame3 = AGIDL_LoadPCX(filename3);
				AGIDL_PCX* frame4 = AGIDL_LoadPCX(filename4);
				printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);

				AGIDL_ColorConvertPCX(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertPCX(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertPCX(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertPCX(frame4,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					w = AGIDL_PCXGetWidth(frame1), h = AGIDL_PCXGetHeight(frame1);
					AGIDL_ScalePCX(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePCX(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePCX(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePCX(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					w = AGIDL_PCXGetWidth(frame1), h = AGIDL_PCXGetHeight(frame1);
					AGIDL_ScalePCX(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePCX(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePCX(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePCX(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}


				w = AGIDL_PCXGetWidth(frame2), h = AGIDL_PCXGetHeight(frame2);

				num_of_pix = w*h;

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_NDS){

					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					u32* interp = malloc(sizeof(u32)*num_of_pix);

					AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);

					printf("Encoding AGIDL Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i);
					printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
					AGMV_EncodeFrame(file,agmv,interp);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i+3);

					num_of_frames_encoded += 3; i += 4;

					free(interp);

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					u32* interp = malloc(sizeof(u32)*num_of_pix);

					AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);

					printf("Encoding Interpolated Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,interp);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i);

					num_of_frames_encoded++; i += 2;

					free(interp);

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreePCX(frame1);
				AGIDL_FreePCX(frame2);
				AGIDL_FreePCX(frame3);
				AGIDL_FreePCX(frame4);
			}break;
			case AGIDL_IMG_LMP:{
				printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_LMP* frame1 = AGIDL_LoadLMP(filename1);
				AGIDL_LMP* frame2 = AGIDL_LoadLMP(filename2);
				AGIDL_LMP* frame3 = AGIDL_LoadLMP(filename3);
				AGIDL_LMP* frame4 = AGIDL_LoadLMP(filename4);
				printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);

				AGIDL_ColorConvertLMP(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertLMP(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertLMP(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertLMP(frame4,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					w = AGIDL_LMPGetWidth(frame1), h = AGIDL_LMPGetHeight(frame1);
					AGIDL_ScaleLMP(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLMP(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLMP(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLMP(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					w = AGIDL_LMPGetWidth(frame1), h = AGIDL_LMPGetHeight(frame1);
					AGIDL_ScaleLMP(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLMP(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLMP(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLMP(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				w = AGIDL_LMPGetWidth(frame2), h = AGIDL_LMPGetHeight(frame2);

				num_of_pix = w*h;

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_NDS){

					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					u32* interp = malloc(sizeof(u32)*num_of_pix);

					AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);

					printf("Encoding AGIDL Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i);
					printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
					AGMV_EncodeFrame(file,agmv,interp);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i+3);

					num_of_frames_encoded += 3; i += 4;

					free(interp);

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					u32* interp = malloc(sizeof(u32)*num_of_pix);

					AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);

					printf("Encoding Interpolated Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,interp);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i);

					num_of_frames_encoded++; i += 2;

					free(interp);

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeLMP(frame1);
				AGIDL_FreeLMP(frame2);
				AGIDL_FreeLMP(frame3);
				AGIDL_FreeLMP(frame4);
			}break;
			case AGIDL_IMG_PVR:{
				printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_PVR* frame1 = AGIDL_LoadPVR(filename1);
				AGIDL_PVR* frame2 = AGIDL_LoadPVR(filename2);
				AGIDL_PVR* frame3 = AGIDL_LoadPVR(filename3);
				AGIDL_PVR* frame4 = AGIDL_LoadPVR(filename4);
				printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);

				AGIDL_ColorConvertPVR(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertPVR(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertPVR(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertPVR(frame4,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					w = AGIDL_PVRGetWidth(frame1), h = AGIDL_PVRGetHeight(frame1);
					AGIDL_ScalePVR(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePVR(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePVR(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePVR(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					w = AGIDL_PVRGetWidth(frame1), h = AGIDL_PVRGetHeight(frame1);
					AGIDL_ScalePVR(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePVR(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePVR(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePVR(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				w = AGIDL_PVRGetWidth(frame2), h = AGIDL_PVRGetHeight(frame2);

				num_of_pix = w*h;

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_NDS){

					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					u32* interp = malloc(sizeof(u32)*num_of_pix);

					AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);

					printf("Encoding AGIDL Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i);
					printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
					AGMV_EncodeFrame(file,agmv,interp);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i+3);

					num_of_frames_encoded += 3; i += 4;

					free(interp);

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					u32* interp = malloc(sizeof(u32)*num_of_pix);

					AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);

					printf("Encoding Interpolated Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,interp);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i);

					num_of_frames_encoded++; i += 2;

					free(interp);

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreePVR(frame1);
				AGIDL_FreePVR(frame2);
				AGIDL_FreePVR(frame3);
				AGIDL_FreePVR(frame4);
			}break;
			case AGIDL_IMG_GXT:{
				printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_GXT* frame1 = AGIDL_LoadGXT(filename1);
				AGIDL_GXT* frame2 = AGIDL_LoadGXT(filename2);
				AGIDL_GXT* frame3 = AGIDL_LoadGXT(filename3);
				AGIDL_GXT* frame4 = AGIDL_LoadGXT(filename4);
				printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);

				AGIDL_ColorConvertGXT(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertGXT(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertGXT(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertGXT(frame4,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					w = AGIDL_GXTGetWidth(frame1), h = AGIDL_GXTGetHeight(frame1);
					AGIDL_ScaleGXT(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleGXT(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleGXT(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleGXT(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					w = AGIDL_GXTGetWidth(frame1), h = AGIDL_GXTGetHeight(frame1);
					AGIDL_ScaleGXT(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleGXT(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleGXT(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleGXT(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				w = AGIDL_GXTGetWidth(frame2), h = AGIDL_GXTGetHeight(frame2);

				num_of_pix = w*h;

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_NDS){

					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					u32* interp = malloc(sizeof(u32)*num_of_pix);

					AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);

					printf("Encoding AGIDL Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i);
					printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
					AGMV_EncodeFrame(file,agmv,interp);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i+3);

					num_of_frames_encoded += 3; i += 4;

					free(interp);

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					u32* interp = malloc(sizeof(u32)*num_of_pix);

					AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);

					printf("Encoding Interpolated Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,interp);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i);

					num_of_frames_encoded++; i += 2;

					free(interp);

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeGXT(frame1);
				AGIDL_FreeGXT(frame2);
				AGIDL_FreeGXT(frame3);
				AGIDL_FreeGXT(frame4);
			}break;
			case AGIDL_IMG_BTI:{
				printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_BTI* frame1 = AGIDL_LoadBTI(filename1);
				AGIDL_BTI* frame2 = AGIDL_LoadBTI(filename2);
				AGIDL_BTI* frame3 = AGIDL_LoadBTI(filename3);
				AGIDL_BTI* frame4 = AGIDL_LoadBTI(filename4);
				printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);

				AGIDL_ColorConvertBTI(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertBTI(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertBTI(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertBTI(frame4,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					w = AGIDL_BTIGetWidth(frame1), h = AGIDL_BTIGetHeight(frame1);
					AGIDL_ScaleBTI(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleBTI(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleBTI(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleBTI(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					w = AGIDL_BTIGetWidth(frame1), h = AGIDL_BTIGetHeight(frame1);
					AGIDL_ScaleBTI(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleBTI(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleBTI(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleBTI(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				w = AGIDL_BTIGetWidth(frame2), h = AGIDL_BTIGetHeight(frame2);

				num_of_pix = w*h;

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_NDS){

					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					u32* interp = malloc(sizeof(u32)*num_of_pix);

					AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);

					printf("Encoding AGIDL Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i);
					printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
					AGMV_EncodeFrame(file,agmv,interp);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i+3);

					num_of_frames_encoded += 3; i += 4;

					free(interp);

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					u32* interp = malloc(sizeof(u32)*num_of_pix);

					AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);

					printf("Encoding Interpolated Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,interp);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i);

					num_of_frames_encoded++; i += 2;

					free(interp);

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeBTI(frame1);
				AGIDL_FreeBTI(frame2);
				AGIDL_FreeBTI(frame3);
				AGIDL_FreeBTI(frame4);
			}break;
			case AGIDL_IMG_3DF:{
				printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_3DF* frame1 = AGIDL_Load3DF(filename1);
				AGIDL_3DF* frame2 = AGIDL_Load3DF(filename2);
				AGIDL_3DF* frame3 = AGIDL_Load3DF(filename3);
				AGIDL_3DF* frame4 = AGIDL_Load3DF(filename4);
				printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);

				AGIDL_ColorConvert3DF(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvert3DF(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvert3DF(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvert3DF(frame4,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					w = AGIDL_3DFGetWidth(frame1), h = AGIDL_3DFGetHeight(frame1);
					AGIDL_Scale3DF(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_Scale3DF(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_Scale3DF(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_Scale3DF(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					w = AGIDL_3DFGetWidth(frame1), h = AGIDL_3DFGetHeight(frame1);
					AGIDL_Scale3DF(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_Scale3DF(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_Scale3DF(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_Scale3DF(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				w = AGIDL_3DFGetWidth(frame2), h = AGIDL_3DFGetHeight(frame2);

				num_of_pix = w*h;

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_NDS){

					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					u32* interp = malloc(sizeof(u32)*num_of_pix);

					AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);

					printf("Encoding AGIDL Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i);
					printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
					AGMV_EncodeFrame(file,agmv,interp);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i+3);

					num_of_frames_encoded += 3; i += 4;

					free(interp);

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					u32* interp = malloc(sizeof(u32)*num_of_pix);

					AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);

					printf("Encoding Interpolated Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,interp);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i);

					num_of_frames_encoded++; i += 2;

					free(interp);

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_Free3DF(frame1);
				AGIDL_Free3DF(frame2);
				AGIDL_Free3DF(frame3);
				AGIDL_Free3DF(frame4);
			}break;
			case AGIDL_IMG_PPM:{
				printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_PPM* frame1 = AGIDL_LoadPPM(filename1);
				AGIDL_PPM* frame2 = AGIDL_LoadPPM(filename2);
				AGIDL_PPM* frame3 = AGIDL_LoadPPM(filename3);
				AGIDL_PPM* frame4 = AGIDL_LoadPPM(filename4);
				printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);

				AGIDL_ColorConvertPPM(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertPPM(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertPPM(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertPPM(frame4,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					w = AGIDL_PPMGetWidth(frame1), h = AGIDL_PPMGetHeight(frame1);
					AGIDL_ScalePPM(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePPM(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePPM(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePPM(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					w = AGIDL_PPMGetWidth(frame1), h = AGIDL_PPMGetHeight(frame1);
					AGIDL_ScalePPM(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePPM(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePPM(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScalePPM(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				w = AGIDL_PPMGetWidth(frame2), h = AGIDL_PPMGetHeight(frame2);

				num_of_pix = w*h;

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_NDS){

					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					u32* interp = malloc(sizeof(u32)*num_of_pix);

					AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);

					printf("Encoding AGIDL Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i);
					printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
					AGMV_EncodeFrame(file,agmv,interp);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i+3);

					num_of_frames_encoded += 3; i += 4;

					free(interp);

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					u32* interp = malloc(sizeof(u32)*num_of_pix);

					AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);

					printf("Encoding Interpolated Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,interp);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i);

					num_of_frames_encoded++; i += 2;

					free(interp);

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreePPM(frame1);
				AGIDL_FreePPM(frame2);
				AGIDL_FreePPM(frame3);
				AGIDL_FreePPM(frame4);
			}break;
			case AGIDL_IMG_LBM:{
				printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_LBM* frame1 = AGIDL_LoadLBM(filename1);
				AGIDL_LBM* frame2 = AGIDL_LoadLBM(filename2);
				AGIDL_LBM* frame3 = AGIDL_LoadLBM(filename3);
				AGIDL_LBM* frame4 = AGIDL_LoadLBM(filename4);
				printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);

				AGIDL_ColorConvertLBM(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertLBM(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertLBM(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertLBM(frame4,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					w = AGIDL_LBMGetWidth(frame1), h = AGIDL_LBMGetHeight(frame1);
					AGIDL_ScaleLBM(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLBM(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLBM(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLBM(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					w = AGIDL_LBMGetWidth(frame1), h = AGIDL_LBMGetHeight(frame1);
					AGIDL_ScaleLBM(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLBM(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLBM(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
					AGIDL_ScaleLBM(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				w = AGIDL_LBMGetWidth(frame2), h = AGIDL_LBMGetHeight(frame2);

				num_of_pix = w*h;

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_NDS){

					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					u32* interp = malloc(sizeof(u32)*num_of_pix);

					AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);

					printf("Encoding AGIDL Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i);
					printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
					AGMV_EncodeFrame(file,agmv,interp);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i+3);

					num_of_frames_encoded += 3; i += 4;

					free(interp);

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					u32* interp = malloc(sizeof(u32)*num_of_pix);

					AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);

					printf("Encoding Interpolated Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,interp);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGIDL Image Frame - %ld...\n",i);

					num_of_frames_encoded++; i += 2;

					free(interp);

					printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeLBM(frame1);
				AGIDL_FreeLBM(frame2);
				AGIDL_FreeLBM(frame3);
				AGIDL_FreeLBM(frame4);
			}break;
		}

		if(i + 4 >= end_frame){
			break;
		}
	}

	fseek(file,4,SEEK_SET);
	AGIDL_WriteLong(file,num_of_frames_encoded);

	fseek(file,18,SEEK_SET);
	f32 rate = (f32)adjusted_num_of_frames/(AGMV_GetNumberOfFrames(agmv)+1);
	AGIDL_WriteLong(file,round(AGMV_GetFramesPerSecond(agmv)*rate));

	fclose(file);

	free(ext);
	DestroyAGMV(agmv);

	if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
		file = fopen(filename,"rb");
		fseek(file,0,SEEK_END);
		u32 file_size = ftell(file);
		fseek(file,0,SEEK_SET);
		u8* data = malloc(sizeof(u8)*file_size);
		fread(data,1,file_size,file);
		fclose(file);

		FILE* out = fopen("GBA_GEN_AGMV.h","w");

		fprintf(out,"#ifndef GBA_GEN_AGMV_H\n");
		fprintf(out,"#define GBA_GEN_AGMV_H\n\n");
		fprintf(out,"const unsigned char GBA_AGMV_FILE[%ld] = {\n",file_size);

		for(i = 0; i < file_size; i++){
			if(i != 0 && i % 4000 == 0){
				fprintf(out,"\n");
			}

			fprintf(out,"%d,",data[i]);
		}

		fprintf(out,"};\n\n");
		fprintf(out,"#endif");

		free(data);
		fclose(out);
	}
}

void AGMV_EncodeFullAGMV(AGMV* agmv, const char* filename, const char* dir, const char* basename, u8 img_type, u32 start_frame, u32 end_frame, u32 width, u32 height, u32 frames_per_second, AGMV_OPT opt, AGMV_QUALITY quality, AGMV_COMPRESSION compression){
	u32 i, palette0[256], palette1[256], n, count = 0, max_clr;
	u32 pal[512];

	AGMV_SetOPT(agmv,opt);
	AGMV_SetCompression(agmv,compression);

	switch(quality){
		case AGMV_HIGH_QUALITY:{
			max_clr = 524287;
		}break;
		case AGMV_MID_QUALITY:{
			max_clr = 131071;
		}break;
		case AGMV_LOW_QUALITY:{
			max_clr = 65535;
		}break;
		default:{
			max_clr = 524287;
		}break;
	}

	u32* colorgram = malloc(sizeof(u32)*max_clr);
	u32* histogram = malloc(sizeof(u32)*max_clr);

	switch(opt){
		case AGMV_OPT_GBA_I:{
			AGMV_SetWidth(agmv,AGMV_GBA_W);
			AGMV_SetHeight(agmv,AGMV_GBA_H);

			free(agmv->frame->img_data);
			agmv->frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_GBA_W*AGMV_GBA_H);

		}break;
		case AGMV_OPT_GBA_II:{
			AGMV_SetWidth(agmv,AGMV_GBA_W);
			AGMV_SetHeight(agmv,AGMV_GBA_H);

			free(agmv->frame->img_data);
			agmv->frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_GBA_W*AGMV_GBA_H);
		}break;
		case AGMV_OPT_GBA_III:{
			AGMV_SetWidth(agmv,AGMV_GBA_W);
			AGMV_SetHeight(agmv,AGMV_GBA_H);

			free(agmv->frame->img_data);
			agmv->frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_GBA_W*AGMV_GBA_H);
		}break;
		case AGMV_OPT_NDS:{
			AGMV_SetWidth(agmv,AGMV_NDS_W);
			AGMV_SetHeight(agmv,AGMV_NDS_H);

			free(agmv->frame->img_data);
			agmv->frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_NDS_W*AGMV_NDS_H);
		}break;
	}

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

	char* ext = AGIDL_GetImgExtension(img_type);

	for(i = start_frame; i <= end_frame; i++){
		char tmpFilename[60];
		if(dir[0] != 'c' || dir[1] != 'u' || dir[2] != 'r'){
			sprintf(tmpFilename,"%s/%s%ld%s",dir,basename,i,ext);
		}
		else{
			sprintf(tmpFilename,"%s%ld%s",basename,i,ext);
		}

		switch(img_type){
			case AGIDL_IMG_BMP:{
				u32* pixels;

				AGIDL_BMP* bmp = AGIDL_LoadBMP(tmpFilename);
				AGIDL_ColorConvertBMP(bmp,AGIDL_RGB_888);

				pixels = bmp->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreeBMP(bmp);
			}break;
			case AGIDL_IMG_TGA:{
				u32* pixels;

				AGIDL_TGA* tga = AGIDL_LoadTGA(tmpFilename);
				AGIDL_ColorConvertTGA(tga,AGIDL_RGB_888);

				pixels = tga->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreeTGA(tga);
			}break;
			case AGIDL_IMG_TIM:{
				u32* pixels;

				AGIDL_TIM* tim = AGIDL_LoadTIM(tmpFilename);

				pixels = tim->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreeTIM(tim);
			}break;
			case AGIDL_IMG_PCX:{
				u32* pixels;

				AGIDL_PCX* pcx = AGIDL_LoadPCX(tmpFilename);
				AGIDL_ColorConvertPCX(pcx,AGIDL_RGB_888);

				pixels = pcx->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreePCX(pcx);
			}break;
			case AGIDL_IMG_LMP:{
				u32* pixels;

				AGIDL_LMP* lmp = AGIDL_LoadLMP(tmpFilename);
				AGIDL_ColorConvertLMP(lmp,AGIDL_RGB_888);

				pixels = lmp->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreeLMP(lmp);
			}break;
			case AGIDL_IMG_PVR:{
				u32* pixels;

				AGIDL_PVR* pvr = AGIDL_LoadPVR(tmpFilename);
				AGIDL_ColorConvertPVR(pvr,AGIDL_RGB_888);

				pixels = pvr->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreePVR(pvr);
			}break;
			case AGIDL_IMG_GXT:{
				u32* pixels;

				AGIDL_GXT* gxt = AGIDL_LoadGXT(tmpFilename);
				AGIDL_ColorConvertGXT(gxt,AGIDL_RGB_888);

				pixels = gxt->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreeGXT(gxt);
			}break;
			case AGIDL_IMG_BTI:{
				u32* pixels;

				AGIDL_BTI* bti = AGIDL_LoadBTI(tmpFilename);
				AGIDL_ColorConvertBTI(bti,AGIDL_RGB_888);

				pixels = bti->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreeBTI(bti);
			}break;
			case AGIDL_IMG_3DF:{
				u32* pixels;

				AGIDL_3DF* glide = AGIDL_Load3DF(tmpFilename);
				AGIDL_ColorConvert3DF(glide,AGIDL_RGB_888);

				pixels = glide->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_Free3DF(glide);
			}break;
			case AGIDL_IMG_PPM:{
				u32* pixels;

				AGIDL_PPM* ppm = AGIDL_LoadPPM(tmpFilename);
				AGIDL_ColorConvertPPM(ppm,AGIDL_RGB_888);

				pixels = ppm->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreePPM(ppm);
			}break;
			case AGIDL_IMG_LBM:{
				u32* pixels;

				AGIDL_LBM* lbm = AGIDL_LoadLBM(tmpFilename);
				AGIDL_ColorConvertLBM(lbm,AGIDL_RGB_888);

				pixels = lbm->pixels.pix32;

				for(n = 0; n < width*height; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}

				AGIDL_FreeLBM(lbm);
			}break;
		}
	}

	AGMV_QuickSort(histogram,colorgram,0,max_clr);

	for(n = max_clr; n > 0; n--){
		Bool skip = FALSE;

		u32 clr = colorgram[n];

		int r = AGMV_GetQuantizedR(clr,quality);
		int g = AGMV_GetQuantizedG(clr,quality);
		int b = AGMV_GetQuantizedB(clr,quality);

		int j;
		for(j = 0; j < 512; j++){
			u32 palclr = pal[j];

			int palr = AGMV_GetQuantizedR(palclr,quality);
			int palg = AGMV_GetQuantizedG(palclr,quality);
			int palb = AGMV_GetQuantizedB(palclr,quality);

			int rdiff = r-palr;
			int gdiff = g-palg;
			int bdiff = b-palb;

			if(rdiff < 0){
				rdiff = AGIDL_Abs(rdiff);
			}

			if(gdiff < 0){
				gdiff = AGIDL_Abs(gdiff);
			}

			if(bdiff < 0){
				bdiff = AGIDL_Abs(bdiff);
			}

			if(quality == AGMV_HIGH_QUALITY){
				if(rdiff <= 2 && gdiff <= 2 && bdiff <= 3){
					skip = TRUE;
				}
			}
			else{
				if(rdiff <= 1 && gdiff <= 1 && bdiff <= 1){
					skip = TRUE;
				}
			}
		}

		if(skip == FALSE){
			pal[count] = clr;
			count++;
		}

		if(count >= 512){
			break;
		}
	}

	if(opt == AGMV_OPT_I || opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_III || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_NDS){
		for(n = 0; n < 512; n++){
			u32 clr = pal[n];
			u32 invclr = AGMV_ReverseQuantizeColor(clr,quality);

			if(n < 126){
				palette0[n] = invclr;
			}
			else if(n >= 126 && n <= 252){
				palette1[n-126] = invclr;
			}

			if(n > 252 && n <= 378){
				palette0[n-126] = invclr;
			}

			if(n > 378){
				palette1[n-252] = invclr;
			}
		}
	}

	if(opt == AGMV_OPT_II || opt == AGMV_OPT_GBA_II|| opt == AGMV_OPT_ANIM){
		for(n = 0; n < 256; n++){
			u32 clr = pal[n];
			u32 invclr = AGMV_ReverseQuantizeColor(clr,quality);

			palette0[n] = invclr;
		}
	}

	free(colorgram);
	free(histogram);

	if(AGMV_GetTotalAudioDuration(agmv) != 0){
		const u32 sample_size = agmv->header.audio_size / (f32)(end_frame-start_frame);

		if(opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II){
			agmv->audio_chunk->size = sample_size;
			agmv->audio_chunk->atsample = (u8*)malloc(sizeof(u8)*agmv->header.audio_size);
			agmv->audio_track->start_point = 0;

			AGMV_CompressAudio(agmv);
		}
		else{
			agmv->audio_chunk->size = sample_size;
			agmv->audio_track->start_point = 0;
		}
	}

	FILE* file = fopen(filename,"wb");

	AGMV_SetICP0(agmv,palette0);
	AGMV_SetICP1(agmv,palette1);

	printf("Encoding AGMV Header...\n");
	AGMV_EncodeHeader(file,agmv);
	printf("Encoded AGMV Header...\n");

	for(i = start_frame; i <= end_frame; i++){
		char filename1[60];
		if(dir[0] != 'c' || dir[1] != 'u' || dir[2] != 'r'){
			sprintf(filename1,"%s/%s%ld%s",dir,basename,i,ext);
		}
		else{
			sprintf(filename1,"%s%ld%s",basename,i,ext);
		}

		switch(img_type){
			case AGIDL_IMG_BMP:{
				printf("Loading AGIDL Image Frame - %ld\n",i);
				AGIDL_BMP* frame1 = AGIDL_LoadBMP(filename1);
				printf("Loaded AGIDL Image Frame - %ld\n",i);

				AGIDL_ColorConvertBMP(frame1,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					u32 w = AGIDL_BMPGetWidth(frame1), h = AGIDL_BMPGetHeight(frame1);
					AGIDL_ScaleBMP(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					u32 w = AGIDL_BMPGetWidth(frame1), h = AGIDL_BMPGetHeight(frame1);
					AGIDL_ScaleBMP(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				printf("Encoding AGIDL Image Frame - %ld...\n",i);
				AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
				printf("Encoded AGIDL Image Frame - %ld...\n",i);

				if(AGMV_GetTotalAudioDuration(agmv) != 0){
					printf("Encoding AGMV Audio Chunk - %ld...\n",i);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGMV Audio Chunk - %ld...\n",i);
				}

				AGIDL_FreeBMP(frame1);
			}break;
			case AGIDL_IMG_TGA:{
				printf("Loading AGIDL Image Frame - %ld\n",i);
				AGIDL_TGA* frame1 = AGIDL_LoadTGA(filename1);
				printf("Loaded AGIDL Image Frame - %ld\n",i);

				AGIDL_ColorConvertTGA(frame1,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					u32 w = AGIDL_TGAGetWidth(frame1), h = AGIDL_TGAGetHeight(frame1);
					AGIDL_ScaleTGA(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					u32 w = AGIDL_TGAGetWidth(frame1), h = AGIDL_TGAGetHeight(frame1);
					AGIDL_ScaleTGA(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				printf("Encoding AGIDL Image Frame - %ld...\n",i);
				AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
				printf("Encoded AGIDL Image Frame - %ld...\n",i);

				if(AGMV_GetTotalAudioDuration(agmv) != 0){
					printf("Encoding AGMV Audio Chunk - %ld...\n",i);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGMV Audio Chunk - %ld...\n",i);
				}

				AGIDL_FreeTGA(frame1);
			}break;
			case AGIDL_IMG_TIM:{
				printf("Loading AGIDL Image Frame - %ld\n",i);
				AGIDL_TIM* frame1 = AGIDL_LoadTIM(filename1);
				printf("Loaded AGIDL Image Frame - %ld\n",i);

				AGIDL_ColorConvertTIM(frame1,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					u32 w = AGIDL_TIMGetWidth(frame1), h = AGIDL_TIMGetHeight(frame1);
					AGIDL_ScaleTIM(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					u32 w = AGIDL_TIMGetWidth(frame1), h = AGIDL_TIMGetHeight(frame1);
					AGIDL_ScaleTIM(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				printf("Encoding AGIDL Image Frame - %ld...\n",i);
				AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
				printf("Encoded AGIDL Image Frame - %ld...\n",i);

				if(AGMV_GetTotalAudioDuration(agmv) != 0){
					printf("Encoding AGMV Audio Chunk - %ld...\n",i);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGMV Audio Chunk - %ld...\n",i);
				}

				AGIDL_FreeTIM(frame1);
			}break;
			case AGIDL_IMG_PCX:{
				printf("Loading AGIDL Image Frame - %ld\n",i);
				AGIDL_PCX* frame1 = AGIDL_LoadPCX(filename1);
				printf("Loaded AGIDL Image Frame - %ld\n",i);

				AGIDL_ColorConvertPCX(frame1,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					u32 w = AGIDL_PCXGetWidth(frame1), h = AGIDL_PCXGetHeight(frame1);
					AGIDL_ScalePCX(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					u32 w = AGIDL_PCXGetWidth(frame1), h = AGIDL_PCXGetHeight(frame1);
					AGIDL_ScalePCX(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				printf("Encoding AGIDL Image Frame - %ld...\n",i);
				AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
				printf("Encoded AGIDL Image Frame - %ld...\n",i);

				if(AGMV_GetTotalAudioDuration(agmv) != 0){
					printf("Encoding AGMV Audio Chunk - %ld...\n",i);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGMV Audio Chunk - %ld...\n",i);
				}

				AGIDL_FreePCX(frame1);
			}break;
			case AGIDL_IMG_LMP:{
				printf("Loading AGIDL Image Frame - %ld\n",i);
				AGIDL_LMP* frame1 = AGIDL_LoadLMP(filename1);
				printf("Loaded AGIDL Image Frame - %ld\n",i);

				AGIDL_ColorConvertLMP(frame1,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					u32 w = AGIDL_LMPGetWidth(frame1), h = AGIDL_LMPGetHeight(frame1);
					AGIDL_ScaleLMP(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					u32 w = AGIDL_LMPGetWidth(frame1), h = AGIDL_LMPGetHeight(frame1);
					AGIDL_ScaleLMP(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				printf("Encoding AGIDL Image Frame - %ld...\n",i);
				AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
				printf("Encoded AGIDL Image Frame - %ld...\n",i);

				if(AGMV_GetTotalAudioDuration(agmv) != 0){
					printf("Encoding AGMV Audio Chunk - %ld...\n",i);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGMV Audio Chunk - %ld...\n",i);
				}

				AGIDL_FreeLMP(frame1);
			}break;
			case AGIDL_IMG_PVR:{
				printf("Loading AGIDL Image Frame - %ld\n",i);
				AGIDL_PVR* frame1 = AGIDL_LoadPVR(filename1);
				printf("Loaded AGIDL Image Frame - %ld\n",i);

				AGIDL_ColorConvertPVR(frame1,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					u32 w = AGIDL_PVRGetWidth(frame1), h = AGIDL_PVRGetHeight(frame1);
					AGIDL_ScalePVR(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					u32 w = AGIDL_PVRGetWidth(frame1), h = AGIDL_PVRGetHeight(frame1);
					AGIDL_ScalePVR(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				printf("Encoding AGIDL Image Frame - %ld...\n",i);
				AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
				printf("Encoded AGIDL Image Frame - %ld...\n",i);

				if(AGMV_GetTotalAudioDuration(agmv) != 0){
					printf("Encoding AGMV Audio Chunk - %ld...\n",i);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGMV Audio Chunk - %ld...\n",i);
				}

				AGIDL_FreePVR(frame1);
			}break;
			case AGIDL_IMG_GXT:{
				printf("Loading AGIDL Image Frame - %ld\n",i);
				AGIDL_GXT* frame1 = AGIDL_LoadGXT(filename1);
				printf("Loaded AGIDL Image Frame - %ld\n",i);

				AGIDL_ColorConvertGXT(frame1,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					u32 w = AGIDL_GXTGetWidth(frame1), h = AGIDL_GXTGetHeight(frame1);
					AGIDL_ScaleGXT(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					u32 w = AGIDL_GXTGetWidth(frame1), h = AGIDL_GXTGetHeight(frame1);
					AGIDL_ScaleGXT(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				printf("Encoding AGIDL Image Frame - %ld...\n",i);
				AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
				printf("Encoded AGIDL Image Frame - %ld...\n",i);

				if(AGMV_GetTotalAudioDuration(agmv) != 0){
					printf("Encoding AGMV Audio Chunk - %ld...\n",i);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGMV Audio Chunk - %ld...\n",i);
				}

				AGIDL_FreeGXT(frame1);
			}break;
			case AGIDL_IMG_BTI:{
				printf("Loading AGIDL Image Frame - %ld\n",i);
				AGIDL_BTI* frame1 = AGIDL_LoadBTI(filename1);
				printf("Loaded AGIDL Image Frame - %ld\n",i);

				AGIDL_ColorConvertBTI(frame1,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					u32 w = AGIDL_BTIGetWidth(frame1), h = AGIDL_BTIGetHeight(frame1);
					AGIDL_ScaleBTI(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					u32 w = AGIDL_BTIGetWidth(frame1), h = AGIDL_BTIGetHeight(frame1);
					AGIDL_ScaleBTI(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				printf("Encoding AGIDL Image Frame - %ld...\n",i);
				AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
				if(AGMV_GetTotalAudioDuration(agmv) != 0){
					printf("Encoding AGMV Audio Chunk - %ld...\n",i);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGMV Audio Chunk - %ld...\n",i);
				}
				printf("Encoded AGIDL Image Frame - %ld...\n",i);

				AGIDL_FreeBTI(frame1);
			}break;
			case AGIDL_IMG_3DF:{
				printf("Loading AGIDL Image Frame - %ld\n",i);
				AGIDL_3DF* frame1 = AGIDL_Load3DF(filename1);
				printf("Loaded AGIDL Image Frame - %ld\n",i);

				AGIDL_ColorConvert3DF(frame1,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					u32 w = AGIDL_3DFGetWidth(frame1), h = AGIDL_3DFGetHeight(frame1);
					AGIDL_Scale3DF(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					u32 w = AGIDL_3DFGetWidth(frame1), h = AGIDL_3DFGetHeight(frame1);
					AGIDL_Scale3DF(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				printf("Encoding AGIDL Image Frame - %ld...\n",i);
				AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
				printf("Encoded AGIDL Image Frame - %ld...\n",i);

				if(AGMV_GetTotalAudioDuration(agmv) != 0){
					printf("Encoding AGMV Audio Chunk - %ld...\n",i);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGMV Audio Chunk - %ld...\n",i);
				}

				AGIDL_Free3DF(frame1);
			}break;
			case AGIDL_IMG_PPM:{
				printf("Loading AGIDL Image Frame - %ld\n",i);
				AGIDL_PPM* frame1 = AGIDL_LoadPPM(filename1);
				printf("Loaded AGIDL Image Frame - %ld\n",i);

				AGIDL_ColorConvertPPM(frame1,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					u32 w = AGIDL_PPMGetWidth(frame1), h = AGIDL_PPMGetHeight(frame1);
					AGIDL_ScalePPM(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					u32 w = AGIDL_PPMGetWidth(frame1), h = AGIDL_PPMGetHeight(frame1);
					AGIDL_ScalePPM(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				printf("Encoding AGIDL Image Frame - %ld...\n",i);
				AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
				printf("Encoded AGIDL Image Frame - %ld...\n",i);

				if(AGMV_GetTotalAudioDuration(agmv) != 0){
					printf("Encoding AGMV Audio Chunk - %ld...\n",i);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGMV Audio Chunk - %ld...\n",i);
				}

				AGIDL_FreePPM(frame1);
			}break;
			case AGIDL_IMG_LBM:{
				printf("Loading AGIDL Image Frame - %ld\n",i);
				AGIDL_LBM* frame1 = AGIDL_LoadLBM(filename1);
				printf("Loaded AGIDL Image Frame - %ld\n",i);

				AGIDL_ColorConvertLBM(frame1,AGIDL_RGB_888);

				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
					u32 w = AGIDL_LBMGetWidth(frame1), h = AGIDL_LBMGetHeight(frame1);
					AGIDL_ScaleLBM(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				if(opt == AGMV_OPT_NDS){
					u32 w = AGIDL_LBMGetWidth(frame1), h = AGIDL_LBMGetHeight(frame1);
					AGIDL_ScaleLBM(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_BILERP);
				}

				printf("Encoding AGIDL Image Frame - %ld...\n",i);
				AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
				printf("Encoded AGIDL Image Frame - %ld...\n",i);

				if(AGMV_GetTotalAudioDuration(agmv) != 0){
					printf("Encoding AGMV Audio Chunk - %ld...\n",i);
					AGMV_EncodeAudioChunk(file,agmv);
					printf("Encoded AGMV Audio Chunk - %ld...\n",i);
				}

				AGIDL_FreeLBM(frame1);
			}break;
		}
	}

	fclose(file);

	free(ext);
	DestroyAGMV(agmv);

	if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III){
		file = fopen(filename,"rb");
		fseek(file,0,SEEK_END);
		u32 file_size = ftell(file);
		fseek(file,0,SEEK_SET);
		u8* data = malloc(sizeof(u8)*file_size);
		fread(data,1,file_size,file);
		fclose(file);
		
		FILE* out = fopen("GBA_GEN_AGMV.h","w");
		
		fprintf(out,"#ifndef GBA_GEN_AGMV_H\n");
		fprintf(out,"#define GBA_GEN_AGMV_H\n\n");
		fprintf(out,"const unsigned char GBA_AGMV_FILE[%ld] = {\n",file_size);

		for(i = 0; i < file_size; i++){
			if(i != 0 && i % 4000 == 0){
				fprintf(out,"\n");
			}
			
			fprintf(out,"%d,",data[i]);
		}
		
		fprintf(out,"};\n\n");
		fprintf(out,"#endif");
		
		free(data);
		fclose(out);		
	}
}
