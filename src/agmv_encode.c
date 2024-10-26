/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_encode.c
*   Date: 5/17/2024
*   Version: 2.0
*   Updated: 10/26/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <math.h>
#include <agidl.h>
#include <stdlib.h>
#include <string.h>
#include <agmv_encode.h>
#include <agmv_utils.h>
#include <agmv_convert.h>

Bool dither = FALSE;

void AGMV_EnableDither(){
	dither = TRUE;
}

void AGMV_DisableDither(){
	dither = FALSE;
}

void AGMV_EncodeHeader(FILE* file, AGMV* agmv){
	u32 i, color;
	u8  r, g, b;

	AGMV_OPT opt;
	AGMV_COMPRESSION compression;
	
	opt = AGMV_GetOPT(agmv);
	compression = AGMV_GetCompression(agmv);

	AGMV_WriteFourCC(file,'A','G','M','V');
	AGMV_WriteLong(file,AGMV_GetNumberOfFrames(agmv));
	AGMV_WriteLong(file,AGMV_GetWidth(agmv));
	AGMV_WriteLong(file,AGMV_GetHeight(agmv));
	AGMV_WriteByte(file,1);
	
	if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
		AGMV_WriteByte(file,AGMV_GetVersionFromOPT(opt,compression));	
		
		AGMV_WriteLong(file,AGMV_GetFramesPerSecond(agmv));
		AGMV_WriteLong(file,AGMV_GetTotalAudioDuration(agmv));
		AGMV_WriteLong(file,AGMV_GetSampleRate(agmv));
		AGMV_WriteLong(file,AGMV_GetAudioSize(agmv));
		AGMV_WriteShort(file,AGMV_GetNumberOfChannels(agmv));
		AGMV_WriteShort(file,AGMV_GetBitsPerSample(agmv));
	}
	else if(opt != AGMV_OPT_II && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_II){
		
		AGMV_WriteByte(file,AGMV_GetVersionFromOPT(opt,compression));	
		
		AGMV_WriteLong(file,AGMV_GetFramesPerSecond(agmv));
		AGMV_WriteLong(file,AGMV_GetTotalAudioDuration(agmv));
		AGMV_WriteLong(file,AGMV_GetSampleRate(agmv));
		AGMV_WriteLong(file,AGMV_GetAudioSize(agmv));
		AGMV_WriteShort(file,AGMV_GetNumberOfChannels(agmv));
		AGMV_WriteShort(file,AGMV_GetBitsPerSample(agmv));

		for(i = 0; i < 256; i++){
			color = agmv->header.palette0[i];

			r = AGMV_GetR(color);
			g = AGMV_GetG(color);
			b = AGMV_GetB(color);

			AGMV_WriteByte(file,r);
			AGMV_WriteByte(file,g);
			AGMV_WriteByte(file,b);
		}

		for(i = 0; i < 256; i++){
			color = agmv->header.palette1[i];

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
		AGMV_WriteShort(file,AGMV_GetBitsPerSample(agmv));

		for(i = 0; i < 256; i++){
			color = agmv->header.palette0[i];

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
#define	BACK_WINDOW		4095
#define	BACK_BITS		12
#define	FRONT_WINDOW	15
#define	FRONT_BITS		4

u32 AGMV_LZSS (FILE* file, AGMV_BITSTREAM* in)
{
	int		i;
	int		val;
	int		j, start, max;
	int		bestlength, beststart, offset;
	int		outbits = 0;
	int     pos = in->pos;
	u8*     data = in->data;
	u8      flag, flag_num;
	u32     flag_pos, file_pos;
	
	flag = 0;
	flag_num = 0;
	flag_pos = 0;

	for (i=0 ; i<pos ; )
	{
		val = data[i];

		max = FRONT_WINDOW;
		if (i + max > pos)
			max = pos - i;

		start = i - BACK_WINDOW;
		if (start < 0)
			start = 0;
		bestlength = 0;
		beststart = 0;
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
		
		if(flag_num == 8){
			flag_num = 0;
			file_pos = ftell(file);
			fseek(file,flag_pos,SEEK_SET);
			AGMV_WriteByte(file,flag);
			fseek(file,file_pos,SEEK_SET);
			flag = 0;
		}
		
		if(flag_num == 0){
			flag_pos = ftell(file);
			AGMV_WriteByte(file,0);
			outbits += 8;
		}
		
		if (bestlength < 3)
		{	/* output a single char */
			bestlength = 1;

			AGMV_WriteByte(file,val);
			
			flag |= 1 << (7 - flag_num);
			flag_num++;
			outbits += 8;
		}
		else
		{
			offset = AGMV_Clamp(0,BACK_WINDOW-beststart,4095);
			AGMV_WriteShort(file,offset<<4|bestlength);
			
			flag |= 0 << (7 - flag_num);
			flag_num++;
			outbits += 16;
		}

		i += bestlength;
	}
	
	if(flag_num != 0){
		file_pos = ftell(file);
		fseek(file,flag_pos,SEEK_SET);
		AGMV_WriteByte(file,flag);
		fseek(file,file_pos,SEEK_SET);
	}
	
	return outbits / 8.0f;
}

AGMV_BLOCK_COUNT AGMV_GetCopyBlockCount(AGMV* agmv, u32 x, u32 y){
	u32 i, j, *iframe_ptr, *frame_ptr, *iframe_data = agmv->iframe->img_data, *frame_data = agmv->frame->img_data;
	u32 width, height, count1, count2, count3, count4, pcolor, icolor, max_diff = agmv->max_diff;
	int r1, g1, b1, r2, g2, b2, rdiff, gdiff, bdiff;
	AGMV_BLOCK_COUNT block_count;
	
	count1 = 0; count2 = 0; count3 = 0; count4 = 0;
	
	width  = agmv->frame->width;
	height = agmv->frame->height;
	
	iframe_ptr = &iframe_data[x+y*width];
	frame_ptr  = &frame_data[x+y*width];
	
	for(j = 0; j < 8; j++){
		if(y + j < height){
			for(i = 0; i < 8; i++){
				if(x + i < width){
					pcolor = iframe_ptr[i];
					icolor = frame_ptr[i];
					
					r1 = AGMV_GetR(pcolor);
					g1 = AGMV_GetG(pcolor);
					b1 = AGMV_GetB(pcolor);
					
					r2 = AGMV_GetR(icolor);
					g2 = AGMV_GetG(icolor);
					b2 = AGMV_GetB(icolor);
					
					rdiff = r1-r2;
					gdiff = g1-g2;
					bdiff = b1-b2;
					
					if(rdiff < 0){
						rdiff = -rdiff;
					}
					
					if(gdiff < 0){
						gdiff = -gdiff;
					}
					
					if(bdiff < 0){
						bdiff = -bdiff;
					}
					
					if(rdiff <= max_diff && gdiff <= max_diff && bdiff <= max_diff){
						if(j < 4){
							if(i < 4){
								count1++;
							}
						
							count2++;
							count4++;
						
							if(i < 2){
								count3++;
							}
						}
						else{
							count4++;
						}
					}
				}
			}
			
			iframe_ptr += width;
			frame_ptr  += width;
		}
	}
	
	if(x + 8 > width){
		count2 = 0;
		count4 = 0;
	}
	
	if(x + 4 > width){
		count1 = 0;
	}
	
	if(y + 8 > height){
		count4 = 0;
	}
		
	block_count.b4x4 = count1;
	block_count.b8x4 = count2;
	block_count.b2x4 = count3;
	block_count.b8x8 = count4;
	
	return block_count;
}

AGMV_BLOCK_COUNT AGMV_GetPCopyBlockCount(AGMV* agmv, u32 x, u32 y){
	u32 i, j, *pframe_ptr, *frame_ptr, *pframe_data = agmv->prev_frame->img_data, *frame_data = agmv->frame->img_data;
	u32 width, height, count1, count2, count3, count4, pcolor, icolor, max_diff = agmv->max_diff;
	int r1, g1, b1, r2, g2, b2, rdiff, gdiff, bdiff;
	AGMV_BLOCK_COUNT block_count;
		
	count1 = 0; count2 = 0; count3 = 0; count4 = 0;
	
	width  = agmv->frame->width;
	height = agmv->frame->height;
	
	pframe_ptr = &pframe_data[x+y*width];
	frame_ptr  = &frame_data[x+y*width];
	
	for(j = 0; j < 8; j++){
		if(y + j < height){
			for(i = 0; i < 8; i++){
				if(x + i < width){
					pcolor = pframe_ptr[i];
					icolor = frame_ptr[i];
					
					r1 = AGMV_GetR(pcolor);
					g1 = AGMV_GetG(pcolor);
					b1 = AGMV_GetB(pcolor);
					
					r2 = AGMV_GetR(icolor);
					g2 = AGMV_GetG(icolor);
					b2 = AGMV_GetB(icolor);
					
					rdiff = r1-r2;
					gdiff = g1-g2;
					bdiff = b1-b2;
					
					if(rdiff < 0){
						rdiff = -rdiff;
					}
					
					if(gdiff < 0){
						gdiff = -gdiff;
					}
					
					if(bdiff < 0){
						bdiff = -bdiff;
					}
					
					if(rdiff <= max_diff && gdiff <= max_diff && bdiff <= max_diff){
						if(j < 4){
							if(i < 4){
								count1++;
							}
						
							count2++;
							count4++;
						
							if(i < 2){
								count3++;
							}
						}
						else{
							count4++;
						}
					}
				}
			}
			
			pframe_ptr += width;
			frame_ptr += width;
		}
	}
	
	if(x + 8 > width){
		count2 = 0;
		count4 = 0;
	}
	
	if(x + 4 > width){
		count1 = 0;
	}
	
	if(y + 8 > height){
		count4 = 0;
	}
		
	block_count.b4x4 = count1;
	block_count.b8x4 = count2;
	block_count.b2x4 = count3;
	block_count.b8x8 = count4;
	
	return block_count;
}

AGMV_BLOCK_COUNT AGMV_GetFillBlockCount(AGMV* agmv, u32 x, u32 y, u32 color){
	u32 i, j, bc, width, height, *frame_ptr, max_diff = agmv->max_diff;
	int r1, g1, b1, r2, g2, b2, rdiff, gdiff, bdiff;
	u8 count1, count2, count3, count4;
	AGMV_BLOCK_COUNT block_count;
	
	count1 = 0; count2 = 0; count3 = 0; count4 = 0;
	
	width  = agmv->frame->width;
	height = agmv->frame->height;

	frame_ptr = &agmv->frame->img_data[x+y*width];
	
	r1 = AGMV_GetR(color);
	g1 = AGMV_GetG(color);
	b1 = AGMV_GetB(color);

	for(j = 0; j < 8; j++){
		if(y + j < height){
			for(i = 0; i < 8; i++){
				if(x + i < width){
					bc = frame_ptr[i];

					r2 = AGMV_GetR(bc);
					g2 = AGMV_GetG(bc);
					b2 = AGMV_GetB(bc);
					
					rdiff = r1-r2;
					gdiff = g1-g2;
					bdiff = b1-b2;
					
					if(rdiff < 0){
						rdiff = -rdiff;
					}
					
					if(gdiff < 0){
						gdiff = -gdiff;
					}
					
					if(bdiff < 0){
						bdiff = -bdiff;
					}
					
					if(rdiff <= max_diff && gdiff <= max_diff && bdiff <= max_diff){
						if(j < 4){
							if(i < 4){
								count1++;
							}
						
							count2++;
							count4++;
						
							if(i < 2){
								count3++;
							}
						}
						else{
							count4++;
						}
					}
				}
			}
			
			frame_ptr += width;
		}
	}
	
	if(x + 8 > width){
		count2 = 0;
		count4 = 0;
	}
	
	if(x + 4 > width){
		count1 = 0;
	}
	
	if(y + 8 > height){
		count4 = 0;
	}
	
	block_count.b4x4 = count1;
	block_count.b8x4 = count2;
	block_count.b2x4 = count3;
	block_count.b8x8 = count4;
	
	return block_count;
}

AGMV_BLOCK_COUNT AGMV_GetMVBlockCount(AGMV* agmv, u32 x, u32 y){
	u32 i, j, n, *pframe_ptr, *iframe_ptr, *pframe_data = agmv->frame->img_data, *iframe_data = agmv->iframe->img_data;
	u32 width, height, offset, count, pcolor, icolor, ix, iy, max_diff = agmv->max_diff;
	u32 block_w, block_h;
	int r1, g1, b1, r2, g2, b2, rdiff, gdiff, bdiff;
	AGMV_BLOCK_COUNT block_count;
	AGMV_OPT opt;
	Bool v4;
	
	width  = agmv->frame->width;
	height = agmv->frame->height;
	offset = x + y * width;
	opt    = agmv->opt;
	
	if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_NDS_IV || opt == AGMV_OPT_PSX_IV){
		v4 = TRUE;
	}
	else{
		v4 = FALSE;
	}
	
	for(n = 0; n < 4; n++){
		count = 0;
		
		block_w = AGMV_GetBlockWidth(n);
		block_h = AGMV_GetBlockHeight(n);
		
		pframe_ptr = &pframe_data[offset];
		
		if(n == 0){
			ix = ((int)x + agmv->mv.x); iy = ((int)y + agmv->mv.y);
		}
		else if(n == 1){
			ix = ((int)x + agmv->mv8x4.x); iy = ((int)y + agmv->mv8x4.y);
		}
		else if(n == 2){
			ix = ((int)x + agmv->mv2x4.x); iy = ((int)y + agmv->mv2x4.y);
		}
		else{
			ix = ((int)x + agmv->mv8x8.x); iy = ((int)y + agmv->mv8x8.y);
		}
		
		if(ix >= 0 && iy >= 0 && ix < width && iy < height){
			iframe_ptr = &iframe_data[ix+iy*width];
		}
		else{
			iframe_ptr = &iframe_data[0];
		}
		
		if(x + block_w < width && y + block_h < height && ix + block_w < width && iy + block_h < height && iy >= 0 && ix >= 0){
		
			for(j = 0; j < block_h; j++){
				for(i = 0; i < block_w; i++){
					pcolor = pframe_ptr[i];
					icolor = iframe_ptr[i];
					
					r1 = AGMV_GetR(pcolor);
					g1 = AGMV_GetG(pcolor);
					b1 = AGMV_GetB(pcolor);
					
					r2 = AGMV_GetR(icolor);
					g2 = AGMV_GetG(icolor);
					b2 = AGMV_GetB(icolor);
					
					rdiff = r1-r2;
					gdiff = g1-g2;
					bdiff = b1-b2;
					
					if(rdiff < 0){
						rdiff = -rdiff;
					}
					
					if(gdiff < 0){
						gdiff = -gdiff;
					}
					
					if(bdiff < 0){
						bdiff = -bdiff;
					}
					
					if(v4){
						if(rdiff <= max_diff && gdiff <= max_diff && bdiff <= max_diff){
							count++;
						}
					}else{
						if(rdiff <= max_diff+1 && gdiff <= max_diff+1 && bdiff <= max_diff){
							count++;
						}
					}
				}
				
				pframe_ptr += width;
				iframe_ptr += width;
			}
		}
		
		if(n == 0){
			block_count.b4x4 = count;
		}
		else if(n == 1){
			block_count.b8x4 = count;
		}
		else if(n == 2){ 
			block_count.b2x4 = count;
		}
		else{
			block_count.b8x8 = count;
		}
	}
	
	return block_count;
}

AGMV_SUMMATION AGMV_UniversalMAD(AGMV* agmv, int i, int j, int x, int y, u8 block_w, u8 block_h){
	AGMV_SUMMATION summation;
	u32 k, l, sum1 = 0, sum2 = 0, *frame_data = agmv->frame->img_data, *iframe_data = agmv->iframe->img_data, *pframe_data = agmv->prev_frame->img_data;
	u32 width = agmv->frame->width, offset1, offset2, offset3;
	int iframe, pframe, frame;
	u32 mul1[8] = {(y+j)*width,(y+j+1)*width,(y+j+2)*width,(y+j+3)*width,(y+j+4)*width,(y+j+5)*width,(y+j+6)*width,(y+j+7)*width};
	u32 mul2[8] = {y*width,(y+1)*width,(y+2)*width,(y+3)*width,(y+4)*width,(y+5)*width,(y+6)*width,(y+7)*width};
	
	for(k = 0; k < block_w; k++){
		offset1 = (x+k);
		offset2 = (x+i+k);
		for(l = 0; l < block_h; l++){
			offset3 = offset2+mul1[l];
			frame  = frame_data[offset1+mul2[l]];
			iframe = iframe_data[offset3];
			pframe = pframe_data[offset3];
			sum1 += AGMV_Abs(frame-iframe);
			sum2 += AGMV_Abs(frame-pframe);
		}
	}
	
	sum1 /= block_w * block_h;
	sum2 /= block_w * block_h;
	
	summation.sum1 = sum1;
	summation.sum2 = sum2;
	
	return summation;
}

void AGMV_ThreeStepSearch2x2(AGMV* agmv, AGMV_SMV* smv1, AGMV_SMV* smv2, AGMV_SMV* smv3, AGMV_SMV* smv4, int x, int y){
	AGMV_SUMMATION summation;
	int stepSize, searchX, searchY, searchX2, searchY2;
	int searchX3, searchY3, searchX4, searchY4;
	int x2 = x + 2, y2 = y + 2;
	int max;
	int i, j, min1, min2, width = agmv->frame->width, height = agmv->frame->height;
	int mx1, my1, mx2, my2, mad1, mad2;
	int nmin1, nmin2, nmx1, nmy1, nmx2, nmy2, nmad1, nmad2;
	int emin1, emin2, emx1, emy1, emx2, emy2, emad1, emad2;
	int dmin1, dmin2, dmx1, dmy1, dmx2, dmy2, dmad1, dmad2;
	
	max = 255*255 + 255*255 + 255*255 + 1;
	
	min1  = max; min2  = max;
	nmin1 = max; nmin2 = max;
	emin1 = max; emin2 = max;
	dmin1 = max; dmin2 = max;
	
	mx1  = 0; my1  = 0; mx2  = 0; my2  = 0;
	nmx1 = 0; nmy1 = 0; nmx2 = 0; nmy2 = 0;
	emx1 = 0; emy1 = 0; emx2 = 0; emy2 = 0;
	dmx1 = 0; dmy1 = 0; dmx2 = 0; dmy2 = 0;
	
	stepSize = (AGMV_SEARCH_RANGE + 1) / 2;

	while(stepSize >= 1){
		for(j = -stepSize; j <= stepSize; j++){
			for(i = -stepSize; i <= stepSize; i++){
				
				searchX  = x + mx1 + i;
                searchY  = y + my1 + j;
				
				searchX2 = x2 + nmx1 + i;
				searchY2 = y  + nmy1 + j;
				
				searchX3 = x  + emx1 + i;
				searchY3 = y2 + emy1 + j;
				
				searchX4 = x2 + dmx1 + i;
				searchY4 = y2 + dmy1 + j;

				if(searchX >= 0 && searchY >= 0 && searchX + 2 < width && searchY + 2 < height && x + 2 < width && y + 2 < height){
					summation = AGMV_UniversalMAD(agmv,searchX-x,searchY-y,x,y,2,2);	
					
					mad1 = summation.sum1;
					mad2 = summation.sum2;
					
					if(mad1 < min1){
						min1 = mad1;
						mx1 = searchX - x;
						my1 = searchY - y;
						
						mx1 = AGMV_Clamp(-AGMV_SEARCH_RANGE,mx1,AGMV_SEARCH_RANGE);
						my1 = AGMV_Clamp(-AGMV_SEARCH_RANGE,my1,AGMV_SEARCH_RANGE);
					}
					
					if(mad2 < min2){
						min2 = mad2;
						mx2 = searchX - x;
						my2 = searchY - y;
						
						mx2 = AGMV_Clamp(-AGMV_SEARCH_RANGE,mx2,AGMV_SEARCH_RANGE);
						my2 = AGMV_Clamp(-AGMV_SEARCH_RANGE,my2,AGMV_SEARCH_RANGE);
					}
				}
				
				if(searchX2 >= 0 && searchY2 >= 0 && searchX2 + 2 < width && searchY2 + 2 < height && x2 + 2 < width && y + 2 < height){
					summation = AGMV_UniversalMAD(agmv,searchX2-x2,searchY2-y,x2,y,2,2);	
					
					nmad1 = summation.sum1;
					nmad2 = summation.sum2;
					
					if(nmad1 < nmin1){
						nmin1 = nmad1;
						nmx1 = searchX2 - x2;
						nmy1 = searchY2 - y;
						
						nmx1 = AGMV_Clamp(-AGMV_SEARCH_RANGE,nmx1,AGMV_SEARCH_RANGE);
						nmy1 = AGMV_Clamp(-AGMV_SEARCH_RANGE,nmy1,AGMV_SEARCH_RANGE);
					}
					
					if(nmad2 < nmin2){
						nmin2 = nmad2;
						nmx2 = searchX2 - x2;
						nmy2 = searchY2 - y;
						
						nmx2 = AGMV_Clamp(-AGMV_SEARCH_RANGE,nmx2,AGMV_SEARCH_RANGE);
						nmy2 = AGMV_Clamp(-AGMV_SEARCH_RANGE,nmy2,AGMV_SEARCH_RANGE);
					}
				}
				
				if(searchX3 >= 0 && searchY3 >= 0 && searchX3 + 2 < width && searchY3 + 2 < height && x + 2 < width && y2 + 2 < height){
					summation = AGMV_UniversalMAD(agmv,searchX3-x,searchY3-y2,x,y2,2,2);	
					
					emad1 = summation.sum1;
					emad2 = summation.sum2;
					
					if(emad1 < emin1){
						emin1 = emad1;
						emx1 = searchX3 - x;
						emy1 = searchY3 - y2;
						
						emx1 = AGMV_Clamp(-AGMV_SEARCH_RANGE,emx1,AGMV_SEARCH_RANGE);
						emy1 = AGMV_Clamp(-AGMV_SEARCH_RANGE,emy1,AGMV_SEARCH_RANGE);
					}
					
					if(emad2 < emin2){
						emin2 = emad2;
						emx2 = searchX4 - x;
						emy2 = searchY4 - y2;
						
						emx2 = AGMV_Clamp(-AGMV_SEARCH_RANGE,emx2,AGMV_SEARCH_RANGE);
						emy2 = AGMV_Clamp(-AGMV_SEARCH_RANGE,emy2,AGMV_SEARCH_RANGE);
					}
				}
				
				if(searchX4 >= 0 && searchY4 >= 0 && searchX4 + 2 < width && searchY4 + 2 < height && x2 + 2 < width && y2 + 2 < height){
					summation = AGMV_UniversalMAD(agmv,searchX4-x2,searchY4-y2,x2,y2,2,2);	
					
					dmad1 = summation.sum1;
					dmad2 = summation.sum2;
					
					if(dmad1 < dmin1){
						dmin1 = dmad1;
						dmx1 = searchX4 - x2;
						dmy1 = searchY4 - y2;
						
						dmx1 = AGMV_Clamp(-AGMV_SEARCH_RANGE,dmx1,AGMV_SEARCH_RANGE);
						dmy1 = AGMV_Clamp(-AGMV_SEARCH_RANGE,dmy1,AGMV_SEARCH_RANGE);
					}
					
					if(dmad2 < dmin2){
						dmin2 = dmad2;
						dmx2 = searchX4 - x2;
						dmy2 = searchY4 - y2;
						
						dmx2 = AGMV_Clamp(-AGMV_SEARCH_RANGE,dmx2,AGMV_SEARCH_RANGE);
						dmy2 = AGMV_Clamp(-AGMV_SEARCH_RANGE,dmy2,AGMV_SEARCH_RANGE);
					}
				}
			}
		}
		
		stepSize /= 2;
	}

	smv1->mv.x = mx1;
	smv1->mv.y = my1;
	
	smv1->pmv.x = mx2;
	smv1->pmv.y = my2;
	
	smv2->mv.x = nmx1;
	smv2->mv.y = nmy1;
	
	smv2->pmv.x = nmx2;
	smv2->pmv.y = nmy2;
	
	smv3->mv.x = emx1;
	smv3->mv.y = emy1;
	
	smv3->pmv.x = emx2;
	smv3->pmv.y = emy2;
	
	smv4->mv.x = dmx1;
	smv4->mv.y = dmy1;
	
	smv4->pmv.x = dmx2;
	smv4->pmv.y = dmy2;
}

void AGMV_ThreeStepSearch4x4(AGMV* agmv, AGMV_SMV* smv1, AGMV_SMV* smv2, int x, int y){
	AGMV_SUMMATION summation;
	int stepSize, searchX, searchY, searchX2, searchY2;
	int x2 = x + 4;
	int max;
	int i, j, min1, min2, width = agmv->frame->width, height = agmv->frame->height;
	int mx1, my1, mx2, my2, mad1, mad2;
	int nmin1, nmin2, nmx1, nmy1, nmx2, nmy2, nmad1, nmad2;
	
	max = 255*255 + 255*255 + 255*255 + 1;
	
	min1  = max; min2  = max;
	nmin1 = max; nmin2 = max;
	
	mx1  = 0; my1  = 0; mx2  = 0; my2  = 0;
	nmx1 = 0; nmy1 = 0; nmx2 = 0; nmy2 = 0;
	
	stepSize = AGMV_SSEARCH_RANGE / 2;

	while(stepSize >= 1){
		for(j = -stepSize; j <= stepSize; j++){
			for(i = -stepSize; i <= stepSize; i++){
				
				searchX  = x + mx1 + i;
                searchY  = y + my1 + j;
				
				searchX2 = x2 + nmx1 + i;
				searchY2 = y  + nmy1 + j;

				if(searchX >= 0 && searchY >= 0 && searchX + 4 < width && searchY + 4 < height && x + 4 < width && y + 4 < height){
					summation = AGMV_UniversalMAD(agmv,searchX-x,searchY-y,x,y,4,4);	
					
					mad1 = summation.sum1;
					mad2 = summation.sum2;
					
					if(mad1 < min1){
						min1 = mad1;
						mx1 = searchX - x;
						my1 = searchY - y;
					}
					
					if(mad2 < min2){
						min2 = mad2;
						mx2 = searchX - x;
						my2 = searchY - y;
					}
				}
				
				if(searchX2 >= 0 && searchY2 >= 0 && searchX2 + 4 < width && searchY2 + 4 < height && x2 + 4 < width && y + 4 < height){
					summation = AGMV_UniversalMAD(agmv,searchX2-x2,searchY2-y,x2,y,4,4);	
					
					nmad1 = summation.sum1;
					nmad2 = summation.sum2;
					
					if(nmad1 < nmin1){
						nmin1 = nmad1;
						nmx1 = searchX2 - x2;
						nmy1 = searchY2 - y;
					}
					
					if(nmad2 < nmin2){
						nmin2 = nmad2;
						nmx2 = searchX2 - x2;
						nmy2 = searchY2 - y;
					}
				}
			}
		}
		
		stepSize /= 2;
	}

	smv1->mv.x = mx1;
	smv1->mv.y = my1;
	
	smv1->pmv.x = mx2;
	smv1->pmv.y = my2;
	
	smv2->mv.x = nmx1;
	smv2->mv.y = nmy1;
	
	smv2->pmv.x = nmx2;
	smv2->pmv.y = nmy2;
}

void AGMV_ThreeStepSearch(AGMV* agmv, int x, int y){
	AGMV_SUMMATION summation;
	int max;
	int stepSize, searchX, searchY, searchX2, searchY2;
	int searchX3, searchY3, searchX4, searchY4;
	int i, j, min1, min2, width = agmv->frame->width, height = agmv->frame->height;
	int mmin1, mmin2, nmin1, nmin2, emin1, emin2;
	int mx1, my1, mx2, my2, mad1, mad2;
	int mmx1, mmy1, mmx2, mmy2, mmad1, mmad2;
	int nmx1, nmy1, nmx2, nmy2, nmad1, nmad2;
	int emx1, emy1, emx2, emy2, emad1, emad2;
	
	stepSize = (AGMV_SEARCH_RANGE + 1) / 2;
	
	max = 255*255 + 255*255 + 255*255 + 1;
	
	min1  = max; min2  = max; mmin1 = max; mmin2 = max;
	nmin1 = max; nmin2 = max; emin1 = max; emin2 = max;
	
	mx1  = 0; my1  = 0; mx2  = 0; my2  = 0;
	mmx1 = 0; mmy1 = 0; mmx2 = 0; mmy2 = 0;
	nmx1 = 0; nmy1 = 0; nmx2 = 0; nmy2 = 0;
	emx1 = 0; emy1 = 0; emx2 = 0; emy2 = 0;

	while(stepSize >= 1){
		for(j = -stepSize; j <= stepSize; j++){
			for(i = -stepSize; i <= stepSize; i++){
				
				searchX  = x + emx1 + i;
                searchY  = y + emy1 + j;
				
				searchX2  = x + nmx1 + i;
                searchY2  = y + nmy1 + j;
				
				searchX3  = x + mx1 + i;
                searchY3  = y + my1 + j;
				
				searchX4  = x + mmx1 + i;
                searchY4  = y + mmy1 + j;
				
				if(searchX >= 0 && searchY >= 0 && searchX + 8 < width && searchY + 8 < height && x + 8 < width && y + 8 < height){
					summation = AGMV_UniversalMAD(agmv,searchX-x,searchY-y,x,y,8,8);	
					
					emad1 = summation.sum1;
					emad2 = summation.sum2;
					
					if(emad1 < emin1){
						emin1 = emad1;
						emx1 = searchX - x;
						emy1 = searchY - y;
						
						emx1 = AGMV_Clamp(-AGMV_SEARCH_RANGE,emx1,AGMV_SEARCH_RANGE);
						emy1 = AGMV_Clamp(-AGMV_SEARCH_RANGE,emy1,AGMV_SEARCH_RANGE);
					}
					
					if(emad2 < emin2){
						emin2 = emad2;
						emx2 = searchX - x;
						emy2 = searchY - y;
						
						emx2 = AGMV_Clamp(-AGMV_SEARCH_RANGE,emx2,AGMV_SEARCH_RANGE);
						emy2 = AGMV_Clamp(-AGMV_SEARCH_RANGE,emy2,AGMV_SEARCH_RANGE);
					}
				}
				
				if(searchX2 >= 0 && searchY2 >= 0 && searchX2 + 2 < width && searchY2 + 4 < height && x + 2 < width && y + 4 < height){
					summation = AGMV_UniversalMAD(agmv,searchX2-x,searchY2-y,x,y,2,4);	
					
					nmad1 = summation.sum1;
					nmad2 = summation.sum2;
					
					if(nmad1 < nmin1){
						nmin1 = nmad1;
						nmx1 = searchX2 - x;
						nmy1 = searchY2 - y;
						
						nmx1 = AGMV_Clamp(-AGMV_SEARCH_RANGE,nmx1,AGMV_SEARCH_RANGE);
						nmy1 = AGMV_Clamp(-AGMV_SEARCH_RANGE,nmy1,AGMV_SEARCH_RANGE);
					}
					
					if(nmad2 < nmin2){
						nmin2 = nmad2;
						nmx2 = searchX2 - x;
						nmy2 = searchY2 - y;
						
						nmx2 = AGMV_Clamp(-AGMV_SEARCH_RANGE,nmx2,AGMV_SEARCH_RANGE);
						nmy2 = AGMV_Clamp(-AGMV_SEARCH_RANGE,nmy2,AGMV_SEARCH_RANGE);
					}
				}
				
				if(searchX3 >= 0 && searchY3 >= 0 && searchX3 + 4 < width && searchY3 + 4 < height && x + 4 < width && y + 4 < height){
					summation = AGMV_UniversalMAD(agmv,searchX3-x,searchY3-y,x,y,4,4);	
					
					mad1 = summation.sum1;
					mad2 = summation.sum2;
					
					if(mad1 < min1){
						min1 = mad1;
						mx1 = searchX3 - x;
						my1 = searchY3 - y;
						
						mx1 = AGMV_Clamp(-AGMV_SEARCH_RANGE,mx1,AGMV_SEARCH_RANGE);
						my1 = AGMV_Clamp(-AGMV_SEARCH_RANGE,my1,AGMV_SEARCH_RANGE);
					}
					
					if(mad2 < min2){
						min2 = mad2;
						mx2 = searchX3 - x;
						my2 = searchY3 - y;
						
						mx2 = AGMV_Clamp(-AGMV_SEARCH_RANGE,mx2,AGMV_SEARCH_RANGE);
						my2 = AGMV_Clamp(-AGMV_SEARCH_RANGE,my2,AGMV_SEARCH_RANGE);
					}
				}
				
				if(searchX4 >= 0 && searchY4 >= 0 && searchX4 + 8 < width && searchY4 + 4 < height && x + 8 < width && y + 4 < height){
					summation = AGMV_UniversalMAD(agmv,searchX4-x,searchY4-y,x,y,8,4);	
					
					mmad1 = summation.sum1;
					mmad2 = summation.sum2;
					
					if(mmad1 < mmin1){
						mmin1 = mmad1;
						mmx1 = searchX4 - x;
						mmy1 = searchY4 - y;
						
						mmx1 = AGMV_Clamp(-AGMV_SEARCH_RANGE,mmx1,AGMV_SEARCH_RANGE);
						mmy1 = AGMV_Clamp(-AGMV_SEARCH_RANGE,mmy1,AGMV_SEARCH_RANGE);
					}
					
					if(mmad2 < mmin2){
						mmin2 = mmad2;
						mmx2 = searchX4 - x;
						mmy2 = searchY4 - y;
						
						mmx2 = AGMV_Clamp(-AGMV_SEARCH_RANGE,mmx2,AGMV_SEARCH_RANGE);
						mmy2 = AGMV_Clamp(-AGMV_SEARCH_RANGE,mmy2,AGMV_SEARCH_RANGE);
					}
				}
			}
		}
		
		stepSize /= 2;
	}
	
	agmv->mv.x = mx1;
	agmv->mv.y = my1;
	
	agmv->pmv.x = mx2;
	agmv->pmv.y = my2;
	
	agmv->mv8x4.x = mmx1;
	agmv->mv8x4.y = mmy1;
	
	agmv->pmv8x4.x = mmx2;
	agmv->pmv8x4.y = mmy2;
	
	agmv->mv2x4.x = nmx1;
	agmv->mv2x4.y = nmy1;
	
	agmv->pmv2x4.x = nmx2;
	agmv->pmv2x4.y = nmy2;
	
	agmv->mv8x8.x = emx1;
	agmv->mv8x8.y = emy1;
	
	agmv->pmv8x8.x = emx2;
	agmv->pmv8x8.y = emy2;
}

AGMV_BLOCK_COUNT AGMV_GetPMVBlockCount(AGMV* agmv, u32 x, u32 y){
	u32 i, j, n, *frame_ptr, *pframe_ptr, *pframe_data = agmv->prev_frame->img_data;
	u32 width, height, offset, count, pcolor, icolor, ix, iy, max_diff = agmv->max_diff;
	u32 block_w, block_h;
	int r1, g1, b1, r2, g2, b2, rdiff, gdiff, bdiff;
	AGMV_BLOCK_COUNT block_count;
	AGMV_OPT opt;
	Bool v4;
	
	width = agmv->frame->width;
	height = agmv->frame->height;
	offset = x + y * width;
	opt    = agmv->opt;
	
	if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_NDS_IV || opt == AGMV_OPT_PSX_IV){
		v4 = TRUE;
	}
	else{
		v4 = FALSE;
	}
	
	for(n = 0; n < 4; n++){
		count = 0;
		
		block_w = AGMV_GetBlockWidth(n);
		block_h = AGMV_GetBlockHeight(n);
		
		if(n == 0){
			ix = ((int)x + agmv->pmv.x); iy = ((int)y + agmv->pmv.y);
		}
		else if(n == 1){
			ix = ((int)x + agmv->pmv8x4.x); iy = ((int)y + agmv->pmv8x4.y);
		}
		else if(n == 2){
			ix = ((int)x + agmv->pmv2x4.x); iy = ((int)y + agmv->pmv2x4.y);
		}
		else{
			ix = ((int)x + agmv->pmv8x8.x); iy = ((int)y + agmv->pmv8x8.y);
		}
		
		frame_ptr = &agmv->frame->img_data[offset];
		
		if(ix < width && iy < height && iy >= 0 && ix >= 0){
			pframe_ptr = &pframe_data[ix+iy*width];
		}
		else{
			pframe_ptr = &pframe_data[0];
		}
		
		if(x + block_w < width && y + block_h < height && ix + block_w < width && iy + block_h < height && iy >= 0 && ix >= 0){
		
			for(j = 0; j < block_h; j++){
				for(i = 0; i < block_w; i++){
					pcolor = frame_ptr[i];
					icolor = pframe_ptr[i];
					
					r1 = AGMV_GetR(pcolor);
					g1 = AGMV_GetG(pcolor);
					b1 = AGMV_GetB(pcolor);
					
					r2 = AGMV_GetR(icolor);
					g2 = AGMV_GetG(icolor);
					b2 = AGMV_GetB(icolor);
					
					rdiff = r1-r2;
					gdiff = g1-g2;
					bdiff = b1-b2;
					
					if(rdiff < 0){
						rdiff = -rdiff;
					}
					
					if(gdiff < 0){
						gdiff = -gdiff;
					}
					
					if(bdiff < 0){
						bdiff = -bdiff;
					}
					
					if(v4){
						if(rdiff <= max_diff && gdiff <= max_diff && bdiff <= max_diff){
							count++;
						}
					}else{
						if(rdiff <= max_diff+1 && gdiff <= max_diff+1 && bdiff <= max_diff){
							count++;
						}
					}
				}
				
				frame_ptr  += width;
				pframe_ptr += width;
			}
		}
		
		if(n == 0){
			block_count.b4x4 = count;
		}
		else if(n == 1){
			block_count.b8x4 = count;
		}
		else if(n == 2){
			block_count.b2x4 = count;
		}
		else{
			block_count.b8x8 = count;
		}
	}
	
	return block_count;
}

u8 AGMV_GetPMVSBlockCount(AGMV* agmv, AGMV_MV mv, u32 x, u32 y, int block_w, int block_h){
	u32 i, j, *frame_ptr, *pframe_ptr, *frame_data = agmv->frame->img_data, *pframe_data = agmv->prev_frame->img_data;
	u32 count, pcolor, icolor, max_diff = agmv->max_diff;
	int r1, g1, b1, r2, g2, b2, rdiff, gdiff, bdiff;
	int width, height, ix, iy;
	AGMV_OPT opt;
	Bool v4;

	width  = agmv->frame->width;
	height = agmv->frame->height;
	opt    = agmv->opt;

	count = 0;
	
	if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_NDS_IV || opt == AGMV_OPT_PSX_IV){
		v4 = TRUE;
	}
	else{
		v4 = FALSE;
	}

	ix = ((int)x + mv.x); iy = ((int)y + mv.y);
	
	frame_ptr  = &frame_data[x+y*width];

	if(ix < width && iy < height && iy >= 0 && ix >= 0){
		pframe_ptr = &pframe_data[ix+iy*width];
	}
	else{
		pframe_ptr = &pframe_data[0];
	}
	
	if(x + block_w < width && y + block_h < height && ix + block_w < width && iy + block_h < height && iy >= 0 && ix >= 0){

		for(j = 0; j < block_h; j++){
			for(i = 0; i < block_w; i++){
				pcolor = frame_ptr[i];
				icolor = pframe_ptr[i];
				
				r1 = AGMV_GetR(pcolor);
				g1 = AGMV_GetG(pcolor);
				b1 = AGMV_GetB(pcolor);
				
				r2 = AGMV_GetR(icolor);
				g2 = AGMV_GetG(icolor);
				b2 = AGMV_GetB(icolor);
				
				rdiff = r1-r2;
				gdiff = g1-g2;
				bdiff = b1-b2;
				
				if(rdiff < 0){
					rdiff = -rdiff;
				}
				
				if(gdiff < 0){
					gdiff = -gdiff;
				}
				
				if(bdiff < 0){
					bdiff = -bdiff;
				}
				
				if(v4){
					if(rdiff <= max_diff && gdiff <= max_diff && bdiff <= max_diff){
						count++;
					}
				}
				else{
					if(rdiff <= max_diff+1 && gdiff <= max_diff+1 && bdiff <= max_diff){
						count++;
					}
				}
			}
			
			frame_ptr  += width;
			pframe_ptr += width;
		}
	}
	
	return count;
}

u8 AGMV_GetMVSBlockCount(AGMV* agmv, AGMV_MV mv, u32 x, u32 y, int block_w, int block_h){
	u32 i, j, *frame_ptr, *iframe_ptr;
	u32 width, height, count, pcolor, icolor, ix, iy, max_diff = agmv->max_diff;
	int r1, g1, b1, r2, g2, b2, rdiff, gdiff, bdiff;
	AGMV_OPT opt;
	Bool v4;
	
	width  = agmv->frame->width;
	height = agmv->frame->height;
	opt    = agmv->opt;

	if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_NDS_IV || opt == AGMV_OPT_PSX_IV){
		v4 = TRUE;
	}
	else{
		v4 = FALSE;
	}

	count = 0;

	ix = ((int)x + mv.x); iy = ((int)y + mv.y);
	
	frame_ptr  = &agmv->frame->img_data[x+y*width];
	
	if(ix < width && iy < height && iy >= 0 && ix >= 0){
		iframe_ptr = &agmv->iframe->img_data[ix+iy*width];
	}
	else{
		iframe_ptr = &agmv->iframe->img_data[0];
	}
	
	if(x + block_w < width && y + block_h < height && ix + block_w < width && iy + block_h < height && iy >= 0 && ix >= 0){
	
		for(j = 0; j < block_h; j++){
			for(i = 0; i < block_w; i++){
				pcolor = frame_ptr[i];
				icolor = iframe_ptr[i];
				
				r1 = AGMV_GetR(pcolor);
				g1 = AGMV_GetG(pcolor);
				b1 = AGMV_GetB(pcolor);
				
				r2 = AGMV_GetR(icolor);
				g2 = AGMV_GetG(icolor);
				b2 = AGMV_GetB(icolor);
				
				rdiff = r1-r2;
				gdiff = g1-g2;
				bdiff = b1-b2;
				
				if(rdiff < 0){
					rdiff = -rdiff;
				}
				
				if(gdiff < 0){
					gdiff = -gdiff;
				}
				
				if(bdiff < 0){
					bdiff = -bdiff;
				}
				
				if(v4){
					if(rdiff <= max_diff && gdiff <= max_diff && bdiff <= max_diff){
						count++;
					}
				}
				else{
					if(rdiff <= max_diff+1 && gdiff <= max_diff+1 && bdiff <= max_diff){
						count++;
					}
				}
			}
			
			frame_ptr  += width;
			iframe_ptr += width;
		}
	}
	
	return count;
}

AGMV_CRATE AGMV_GetVQBlockCount(AGMV* agmv, u32 x, u32 y){
	AGMV_CRATE crate;
	u32 i, j, n, k, offset, color, coffre, block_w, block_h, num;
	int r1, g1, b1, r2, g2, b2, rdiff, gdiff, bdiff;
	u32 *frame_ptr, *frame_data = agmv->frame->img_data, width, height, max_diff = agmv->max_diff;
	Bool add_color;
	
	width  = agmv->frame->width;
	height = agmv->frame->height;
	offset = x + y * width;
	
	crate.c4x4.num = 0;
	crate.c8x4.num = 0;
	crate.c2x4.num = 0;
	crate.c8x8.num = 0;
	
	for(i = 0; i < 64; i++){
		crate.c4x4.coffre[i].color = 0x1f;
		crate.c4x4.coffre[i].freq  = 0;
		
		crate.c8x4.coffre[i].color = 0x1f;
		crate.c8x4.coffre[i].freq  = 0;
		
		crate.c8x8.coffre[i].color = 0x1f;
		crate.c8x8.coffre[i].freq  = 0;
		
		crate.c2x4.coffre[i].color = 0x1f;
		crate.c2x4.coffre[i].freq  = 0;
	}
	
	for(k = 0; k < 4; k++){
		
		block_w = AGMV_GetBlockWidth(k);
		block_h = AGMV_GetBlockHeight(k);
		
		frame_ptr = &frame_data[offset];
	
		if(x + block_w < width && y + block_h < height){
			for(j = 0; j < block_h; j++){
				for(i = 0; i < block_w; i++){
					add_color = TRUE;
					color = frame_ptr[i];
					
					if(k == 0){
						num = crate.c4x4.num;
					}
					else if(k == 1){
						num = crate.c8x4.num;
					}
					else if(k == 2){
						num = crate.c2x4.num;
					}
					else{
						num = crate.c8x8.num;
					}
					
					for(n = 0; n < num; n++){
						
						if(k == 0){
							coffre = crate.c4x4.coffre[n].color;
						}
						else if(k == 1){
							coffre = crate.c8x4.coffre[n].color;
						}
						else if(k == 2){
							coffre = crate.c2x4.coffre[n].color;
						}
						else{
							coffre = crate.c8x8.coffre[n].color;
						}
						
						r1 = AGMV_GetR(color);
						g1 = AGMV_GetG(color);
						b1 = AGMV_GetB(color);
						
						r2 = AGMV_GetR(coffre);
						g2 = AGMV_GetG(coffre);
						b2 = AGMV_GetB(coffre);
						
						rdiff = r1-r2;
						gdiff = g1-g2;
						bdiff = b1-b2;
						
						if(rdiff < 0){
							rdiff = -rdiff;
						}
						
						if(gdiff < 0){
							gdiff = -gdiff;
						}
						
						if(bdiff < 0){
							bdiff = -bdiff;
						}
						
						if(rdiff <= max_diff && gdiff <= max_diff && bdiff <= max_diff){
							add_color = FALSE;
			
							if(k == 0){
								coffre = crate.c4x4.coffre[n].freq++;
							}
							else if(k == 1){
								coffre = crate.c8x4.coffre[n].freq++;
							}
							else if(k == 2){
								coffre = crate.c2x4.coffre[n].freq++;
							}
							else{
								coffre = crate.c8x8.coffre[n].freq++;
							}
						}
					}
					
					if(add_color){
						if(k == 0){
							crate.c4x4.coffre[crate.c4x4.num].color = color;
							crate.c4x4.coffre[crate.c4x4.num++].freq++;
						}
						else if(k == 1){
							crate.c8x4.coffre[crate.c8x4.num].color = color;
							crate.c8x4.coffre[crate.c8x4.num++].freq++;
						}
						else if(k == 2){
							crate.c2x4.coffre[crate.c2x4.num].color = color;
							crate.c2x4.coffre[crate.c2x4.num++].freq++;
						}
						else{
							crate.c8x8.coffre[crate.c8x8.num].color = color;
							crate.c8x8.coffre[crate.c8x8.num++].freq++;
						}
					}
				}
				
				frame_ptr += width;
			}
			
			if(k == 0){
				for(j = 0; j < crate.c4x4.num - 1; j++){
					for(i = 0; i < crate.c4x4.num - 1; i++){
						if(crate.c4x4.coffre[i].freq > crate.c4x4.coffre[i+1].freq){
							u32 tempf = crate.c4x4.coffre[i].freq;
							crate.c4x4.coffre[i].freq = crate.c4x4.coffre[i+1].freq;
							crate.c4x4.coffre[i+1].freq = tempf;
							
							u32 tempc = crate.c4x4.coffre[i].color;
							crate.c4x4.coffre[i].color = crate.c4x4.coffre[i+1].color;
							crate.c4x4.coffre[i+1].color = tempc;
						}
					}
				}
			}
			else if(k == 1){
				for(j = 0; j < crate.c8x4.num - 1; j++){
					for(i = 0; i < crate.c8x4.num - 1; i++){
						if(crate.c8x4.coffre[i].freq > crate.c8x4.coffre[i+1].freq){
							u32 tempf = crate.c8x4.coffre[i].freq;
							crate.c8x4.coffre[i].freq = crate.c8x4.coffre[i+1].freq;
							crate.c8x4.coffre[i+1].freq = tempf;
							
							u32 tempc = crate.c8x4.coffre[i].color;
							crate.c8x4.coffre[i].color = crate.c8x4.coffre[i+1].color;
							crate.c8x4.coffre[i+1].color = tempc;
						}
					}
				}
			}
			else if(k == 2){
				for(j = 0; j < crate.c2x4.num - 1; j++){
					for(i = 0; i < crate.c2x4.num - 1; i++){
						if(crate.c2x4.coffre[i].freq > crate.c2x4.coffre[i+1].freq){
							u32 tempf = crate.c2x4.coffre[i].freq;
							crate.c2x4.coffre[i].freq = crate.c2x4.coffre[i+1].freq;
							crate.c2x4.coffre[i+1].freq = tempf;
							
							u32 tempc = crate.c2x4.coffre[i].color;
							crate.c2x4.coffre[i].color = crate.c2x4.coffre[i+1].color;
							crate.c2x4.coffre[i+1].color = tempc;
						}
					}
				}
			}
			else{
				for(j = 0; j < crate.c8x8.num - 1; j++){
					for(i = 0; i < crate.c8x8.num - 1; i++){
						if(crate.c8x8.coffre[i].freq > crate.c8x8.coffre[i+1].freq){
							u32 tempf = crate.c8x8.coffre[i].freq;
							crate.c8x8.coffre[i].freq = crate.c8x8.coffre[i+1].freq;
							crate.c8x8.coffre[i+1].freq = tempf;
							
							u32 tempc = crate.c8x8.coffre[i].color;
							crate.c8x8.coffre[i].color = crate.c8x8.coffre[i+1].color;
							crate.c8x8.coffre[i+1].color = tempc;
						}
					}
				}
			}
		}
	}
	
	return crate;
}

AGMV_BLOCK_COUNT AGMV_GetIntraCopyBlockCount(AGMV* agmv, u32 x, u32 y){
	AGMV_BLOCK_COUNT block_count;
	u32 i, j, n, offset, startx, starty, max_count, count, block_w, block_h;
	u32 width, height, colori, color, *img_ptr, *iptr, *img_data, max_diff = agmv->max_diff;
	int r1, g1, b1, r2, g2, b2, rdiff, gdiff, bdiff, start_at_y, start_at_x;
	
	block_count.b4x4 = 0;
	block_count.b2x4 = 0;
	block_count.b8x4 = 0;
	block_count.b8x8 = 0;
	
	width  = agmv->frame->width;
	height = agmv->frame->height;
	img_data = agmv->frame->img_data;
	
	offset = x + y * width;
	
	start_at_y = y - 15;
	start_at_x = x - 15;
	
	if(start_at_y < 0){
		start_at_y = 0;
	}
	
	if(start_at_x < 0){
		start_at_x = 0;
	}
	
	for(n = 0; n < 4; n++){
		block_w = AGMV_GetBlockWidth(n);
		block_h = AGMV_GetBlockHeight(n);
		
		max_count = 0;
	
		if(x + block_w < width && y + block_h < height){
			for(starty = start_at_y; starty + block_h <= y; starty++){
				for(startx = start_at_x; startx + block_w < x; startx++){
					
					count = 0;
					
					iptr = &img_data[startx+starty*width];
					img_ptr = &img_data[offset];
					
					for(j = 0; j < block_h; j++){
						for(i = 0; i < block_w; i++){
							colori = img_ptr[i];
							color  = iptr[i];
							
							r1 = AGMV_GetR(colori);
							g1 = AGMV_GetG(colori);
							b1 = AGMV_GetB(colori);
							
							r2 = AGMV_GetR(color);
							g2 = AGMV_GetG(color);
							b2 = AGMV_GetB(color);
							
							rdiff = r1-r2;
							gdiff = g1-g2;
							bdiff = b1-b2;
							
							if(rdiff < 0){
								rdiff = -rdiff;
							}
							
							if(gdiff < 0){
								gdiff = -gdiff;
							}
							
							if(bdiff < 0){
								bdiff = -bdiff;
							}
							
							if(rdiff <= max_diff && gdiff <= max_diff && bdiff <= max_diff){
								count++;
							}
						}
						
						img_ptr += width;
						iptr    += width;
					}
					
					if(count > max_count){
						max_count = count;
						
						if(n == 0){
							block_count.copyx1 = startx;
							block_count.copyy1 = starty;
						}
						else if(n == 1){
							block_count.copyx2 = startx;
							block_count.copyy2 = starty;
						}
						else if(n == 2){
							block_count.copyx3 = startx;
							block_count.copyy3 = starty;
						}
						else{
							block_count.copyx4 = startx;
							block_count.copyy4 = starty;
						}
					}
				}
			}
		}
		
		if(n == 0){
			block_count.b4x4 = max_count;
		}
		else if(n == 1){
			block_count.b8x4 = max_count;
		}
		else if(n == 2){
			block_count.b2x4 = max_count;
		}
		else{
			block_count.b8x8 = max_count;
		}
	}
	
	return block_count;
}

AGMV_BLOCK_COUNT AGMV_GetRightIntraCopyBlockCount(AGMV* agmv, u32 x, u32 y){
	AGMV_BLOCK_COUNT block_count;
	u32 i, j, n, offset, startx, starty, max_count, count, block_w, block_h;
	u32 width, height, colori, color, *img_ptr, *iptr, *img_data, max_diff = agmv->max_diff;
	int r1, g1, b1, r2, g2, b2, rdiff, gdiff, bdiff, start_at_y, end_at_x;
	
	block_count.b4x4 = 0; block_count.copyx1 = 0; block_count.copyy1 = 0;
	block_count.b2x4 = 0; block_count.copyx2 = 0; block_count.copyy2 = 0;
	block_count.b8x4 = 0; block_count.copyx3 = 0; block_count.copyy3 = 0;
	block_count.b8x8 = 0; block_count.copyx4 = 0; block_count.copyy4 = 0;
	
	width  = agmv->frame->width;
	height = agmv->frame->height;
	img_data = agmv->frame->img_data;
	
	offset = x + y * width;

	start_at_y = y - 15;
	end_at_x = x + 15;
	
	if(start_at_y < 0){
		start_at_y = 0;
	}
	
	if(x + 15 > width){
		return block_count;
	}
	
	for(n = 0; n < 4; n++){
		block_w = AGMV_GetBlockWidth(n);
		block_h = AGMV_GetBlockHeight(n);
		
		max_count = 0;
		
		if(x + block_w < width && y + block_h < height){
			for(starty = start_at_y; starty + block_h < y; starty++){
				for(startx = x; startx + block_w <= end_at_x; startx++){
					
					count = 0;
					
					iptr = &img_data[startx+starty*width];
					img_ptr = &img_data[offset];
					
					for(j = 0; j < block_h; j++){
						for(i = 0; i < block_w; i++){
							colori = img_ptr[i];
							color  = iptr[i];
							
							r1 = AGMV_GetR(colori);
							g1 = AGMV_GetG(colori);
							b1 = AGMV_GetB(colori);
							
							r2 = AGMV_GetR(color);
							g2 = AGMV_GetG(color);
							b2 = AGMV_GetB(color);
							
							rdiff = r1-r2;
							gdiff = g1-g2;
							bdiff = b1-b2;
							
							if(rdiff < 0){
								rdiff = -rdiff;
							}
							
							if(gdiff < 0){
								gdiff = -gdiff;
							}
							
							if(bdiff < 0){
								bdiff = -bdiff;
							}
							
							if(rdiff <= max_diff && gdiff <= max_diff && bdiff <= max_diff){
								count++;
							}
						}
						
						img_ptr += width;
						iptr    += width;
					}
					
					if(count > max_count){
						max_count = count;
						
						if(n == 0){
							block_count.copyx1 = startx;
							block_count.copyy1 = starty;
						}
						else if(n == 1){
							block_count.copyx2 = startx;
							block_count.copyy2 = starty;
						}
						else if(n == 2){
							block_count.copyx3 = startx;
							block_count.copyy3 = starty;
						}
						else{
							block_count.copyx4 = startx;
							block_count.copyy4 = starty;
						}
					}
				}
			}
		}
		
		if(n == 0){
			block_count.b4x4 = max_count;
		}
		else if(n == 1){
			block_count.b8x4 = max_count;
		}
		else if(n == 2){
			block_count.b2x4 = max_count;
		}
		else{
			block_count.b8x8 = max_count;
		}
	}
	
	return block_count;
}

void AGMV_AssembleIFrameBitstream(AGMV* agmv, AGMV_ENTRY* img_entry){
	AGMV_OPT opt;
	AGMV_ENTRY entry;
	AGMV_CRATE vq_count;
	AGMV_BLOCK_COUNT fill_count, copy_count, icopyr_count;
	AGMV_QUALITY quality;
	u32 width, height, x, y, i, j, n, offset1, bitpos, color;
	u32* palette0, *palette1, *frame = agmv->frame->img_data;
	u16 color16, max_diff;
	u8* data = agmv->bitstream->data, fill4x4, fill8x4, fill2x4, fill8x8;
	int r1, g1, b1, r2, g2, b2, diffr, diffg, diffb;
	Bool skip;
	
	opt = AGMV_GetOPT(agmv);
	quality = AGMV_GetQuality(agmv);
	
	fill4x4 = AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_FILL_FLAG,quality,opt);
	fill8x4 = AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_FILL_FLAG,quality,opt);
	fill2x4 = AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_FILL_FLAG,quality,opt);
	fill8x8 = AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_FILL_FLAG,quality,opt);
	
	max_diff = agmv->max_diff;
	width = agmv->frame->width;
	height = agmv->frame->height;
	
	palette0 = agmv->header.palette0;
	palette1 = agmv->header.palette1;
	
	bitpos = agmv->bitstream->pos;
	
	if(opt != AGMV_OPT_II && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_II){
		for(y = 0; y < height; y += 4){
			offset1 = (y*width);
			for(x = 0; x < width; x += 4){
				
				skip = FALSE;

				for(n = 0; n < agmv->db.read_pos; n++){
					if(x == agmv->db.read_table[n]){
						data[bitpos++] = AGMV_SKIP_8;
						x += 4;
						skip = TRUE;
						break;
					}
				}
				
				if(skip == FALSE){
					
					if(opt != AGMV_OPT_IV && opt != AGMV_OPT_GBA_IV && opt != AGMV_OPT_PSX_IV && opt != AGMV_OPT_NDS_IV){
						entry = img_entry[x+offset1];
						
						if(entry.pal_num == 0){
							color = palette0[entry.index];
						}
						else{
							color = palette1[entry.index];
						}
					}
					else{
						color = frame[x+offset1];
					}
		
					fill_count = AGMV_GetFillBlockCount(agmv,x,y,color);

					vq_count.c4x4.num = 0; vq_count.c8x4.num = 0; vq_count.c2x4.num = 0; vq_count.c8x8.num = 0;
					
					copy_count.b4x4 = 0; copy_count.copyx1 = 0; copy_count.copyy1 = 0;
					copy_count.b2x4 = 0; copy_count.copyx2 = 0; copy_count.copyy2 = 0;
					copy_count.b8x4 = 0; copy_count.copyx3 = 0; copy_count.copyy3 = 0;
					copy_count.b8x8 = 0; copy_count.copyx4 = 0; copy_count.copyy4 = 0;
					
					icopyr_count.b4x4 = 0; icopyr_count.copyx1 = 0; icopyr_count.copyy1 = 0;
					icopyr_count.b2x4 = 0; icopyr_count.copyx2 = 0; icopyr_count.copyy2 = 0;
					icopyr_count.b8x4 = 0; icopyr_count.copyx3 = 0; icopyr_count.copyy3 = 0;
					icopyr_count.b8x8 = 0; icopyr_count.copyx4 = 0; icopyr_count.copyy4 = 0;
					
					if(fill_count.b2x4 < fill2x4){
						copy_count = AGMV_GetIntraCopyBlockCount(agmv,x,y);
						icopyr_count = AGMV_GetRightIntraCopyBlockCount(agmv,x,y);
						vq_count = AGMV_GetVQBlockCount(agmv,x,y);
					}
					
					r1 = AGMV_GetR(color);
					g1 = AGMV_GetG(color);
					b1 = AGMV_GetB(color);
					
					r2 = AGMV_GetR(agmv->prev_fill_color);
					g2 = AGMV_GetG(agmv->prev_fill_color);
					b2 = AGMV_GetB(agmv->prev_fill_color);
					
					diffr = AGMV_Abs(r1-r2);
					diffg = AGMV_Abs(g1-g2);
					diffb = AGMV_Abs(b1-b2);
					
					if((diffr <= max_diff && diffg <= max_diff && diffb <= max_diff) && (fill_count.b4x4 >= fill4x4 || fill_count.b8x4 >= fill8x4 || fill_count.b2x4 >= fill2x4 || fill_count.b8x8 >= fill8x8)){
						if(fill_count.b8x8 >= fill8x8){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_PFILL_FLAG);
							x += 4;
						}
						else if(fill_count.b8x4 >= fill8x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_PFILL_FLAG);
							x += 4;
						}
						else if(fill_count.b4x4 >= fill4x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_PFILL_FLAG);
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_PFILL_FLAG);
							x -= 2;
						}
					}
					else if(fill_count.b4x4 >= fill4x4 || fill_count.b8x4 >= fill8x4 || fill_count.b2x4 >= fill2x4 || fill_count.b8x8 >= fill8x8){
						
						if(fill_count.b8x8 >= fill8x8){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_FILL_FLAG);
							x += 4;
						}
						else if(fill_count.b8x4 >= fill8x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_FILL_FLAG);
							x += 4;
						}
						else if(fill_count.b4x4 >= fill4x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_FILL_FLAG);
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_FILL_FLAG);
							x -= 2;
						}
						
						if(opt != AGMV_OPT_IV && opt != AGMV_OPT_GBA_IV && opt != AGMV_OPT_PSX_IV && opt != AGMV_OPT_NDS_IV){
							if(entry.index < 127){
								data[bitpos++] = entry.pal_num << 7 | entry.index;
							}
							else{
								data[bitpos++] = entry.pal_num << 7 | 127;
								data[bitpos++] = entry.index;
							}
						}
						else{
							color16 = AGMV_ToRGB16(color);
							
							data[bitpos++] = AGMV_GetMSB(color16);
							data[bitpos++] = AGMV_GetLSB(color16);
						}
	
						agmv->prev_fill_color = color;
					}
					else if(copy_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_ICOPY_FLAG,quality,opt) || copy_count.b2x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_ICOPY_FLAG,quality,opt)){
						if(copy_count.b8x8 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_ICOPY_FLAG,quality,opt)){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_ICOPY_FLAG);
							data[bitpos++] = (x - copy_count.copyx4) << 4 | (y - copy_count.copyy4);

							x += 4;
						}
						else if(copy_count.b8x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_ICOPY_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_ICOPY_FLAG);
							data[bitpos++] = (x - copy_count.copyx2) << 4 | (y - copy_count.copyy2);

							x += 4;
						}
						else if(copy_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_ICOPY_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_ICOPY_FLAG);
							data[bitpos++] = (x - copy_count.copyx1) << 4 | (y - copy_count.copyy1);
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_ICOPY_FLAG);
							data[bitpos++] = (x - copy_count.copyx3) << 4 | (y - copy_count.copyy3);
							
							x -= 2;
						}
					}
					else if(icopyr_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_ICOPY_FLAG,quality,opt) || icopyr_count.b2x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_ICOPY_FLAG,quality,opt)){
						if(icopyr_count.b8x8 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_ICOPY_FLAG,quality,opt)){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_ICOPYR_FLAG);
							data[bitpos++] = (icopyr_count.copyx4 - x) << 4 | (y - icopyr_count.copyy4);

							x += 4;
						}
						else if(icopyr_count.b8x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_ICOPY_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_ICOPYR_FLAG);
							data[bitpos++] = (icopyr_count.copyx2 - x) << 4 | (y - icopyr_count.copyy2);

							x += 4;
						}
						else if(icopyr_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_ICOPY_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_ICOPYR_FLAG);
							data[bitpos++] = (icopyr_count.copyx1 - x) << 4 | (y - icopyr_count.copyy1);
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_ICOPYR_FLAG);
							data[bitpos++] = (icopyr_count.copyx3 - x) << 4 | (y - icopyr_count.copyy3);
							
							x -= 2;
						}
					}
					else if(vq_count.c2x4.num > 0 && vq_count.c2x4.num <= AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_VQ2_FLAG,quality,opt)){
						u32* frame_ptr, color1, color2, dist1, dist2;
						u16 color1_16, color2_16;
						u32 block_w, block_h;
						AGMV_ENTRY index1, index2;
						int r, g, b, r1, g1, b1, r2, g2, b2, rdiff1, gdiff1, bdiff1, rdiff2, gdiff2, bdiff2;
						u8 count = 0, byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8;
						Bool col1, col2;
						u8 indices[64];
						
						col1 = FALSE;
						col2 = FALSE;

						if(vq_count.c8x8.num > 0 && vq_count.c8x8.num <= AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_VQ2_FLAG,quality,opt)){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							
							color1 = vq_count.c8x8.coffre[vq_count.c8x8.num-1].color;
							color2 = vq_count.c8x8.coffre[vq_count.c8x8.num-2].color;
							
							r1 = AGMV_GetR(color1);
							g1 = AGMV_GetG(color1);
							b1 = AGMV_GetB(color1);
							
							r2 = AGMV_GetR(agmv->vq2color1);
							g2 = AGMV_GetG(agmv->vq2color1);
							b2 = AGMV_GetB(agmv->vq2color1);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col1 = TRUE;
							}
							
							r1 = AGMV_GetR(color2);
							g1 = AGMV_GetG(color2);
							b1 = AGMV_GetB(color2);
							
							r2 = AGMV_GetR(agmv->vq2color2);
							g2 = AGMV_GetG(agmv->vq2color2);
							b2 = AGMV_GetB(agmv->vq2color2);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col2 = TRUE;
							}
							
							if(col1 == TRUE && col2 == TRUE){
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_PVQ2_FLAG);
							}
							else{
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_VQ2_FLAG);
							}
							
							block_w = 8; block_h = 8;
						}
						else if(vq_count.c8x4.num > 0 && vq_count.c8x4.num <= AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_VQ2_FLAG,quality,opt)){	
							color1 = vq_count.c8x4.coffre[vq_count.c8x4.num-1].color;
							color2 = vq_count.c8x4.coffre[vq_count.c8x4.num-2].color;
							
							r1 = AGMV_GetR(color1);
							g1 = AGMV_GetG(color1);
							b1 = AGMV_GetB(color1);
							
							r2 = AGMV_GetR(agmv->vq2color1);
							g2 = AGMV_GetG(agmv->vq2color1);
							b2 = AGMV_GetB(agmv->vq2color1);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col1 = TRUE;
							}
							
							r1 = AGMV_GetR(color2);
							g1 = AGMV_GetG(color2);
							b1 = AGMV_GetB(color2);
							
							r2 = AGMV_GetR(agmv->vq2color2);
							g2 = AGMV_GetG(agmv->vq2color2);
							b2 = AGMV_GetB(agmv->vq2color2);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col2 = TRUE;
							}
							
							if(col1 == TRUE && col2 == TRUE){
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_PVQ2_FLAG);
							}
							else{
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_VQ2_FLAG);
							}
							
							block_w = 8; block_h = 4;
						}
						else if(vq_count.c4x4.num > 0 && vq_count.c4x4.num <= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_VQ2_FLAG,quality,opt)){
							color1 = vq_count.c4x4.coffre[vq_count.c4x4.num-1].color;
							color2 = vq_count.c4x4.coffre[vq_count.c4x4.num-2].color;
							
							r1 = AGMV_GetR(color1);
							g1 = AGMV_GetG(color1);
							b1 = AGMV_GetB(color1);
							
							r2 = AGMV_GetR(agmv->vq2color1);
							g2 = AGMV_GetG(agmv->vq2color1);
							b2 = AGMV_GetB(agmv->vq2color1);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col1 = TRUE;
							}
							
							r1 = AGMV_GetR(color2);
							g1 = AGMV_GetG(color2);
							b1 = AGMV_GetB(color2);
							
							r2 = AGMV_GetR(agmv->vq2color2);
							g2 = AGMV_GetG(agmv->vq2color2);
							b2 = AGMV_GetB(agmv->vq2color2);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col2 = TRUE;
							}
							
							if(col1 == TRUE && col2 == TRUE){
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_PVQ2_FLAG);
							}
							else{
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_VQ2_FLAG);
							}
							
							block_w = 4; block_h = 4;
						}
						else{
							color1 = vq_count.c2x4.coffre[vq_count.c2x4.num-1].color;
							color2 = vq_count.c2x4.coffre[vq_count.c2x4.num-2].color;
						
							r1 = AGMV_GetR(color1);
							g1 = AGMV_GetG(color1);
							b1 = AGMV_GetB(color1);
							
							r2 = AGMV_GetR(agmv->vq2color1);
							g2 = AGMV_GetG(agmv->vq2color1);
							b2 = AGMV_GetB(agmv->vq2color1);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col1 = TRUE;
							}
							
							r1 = AGMV_GetR(color2);
							g1 = AGMV_GetG(color2);
							b1 = AGMV_GetB(color2);
							
							r2 = AGMV_GetR(agmv->vq2color2);
							g2 = AGMV_GetG(agmv->vq2color2);
							b2 = AGMV_GetB(agmv->vq2color2);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col2 = TRUE;
							}
							
							if(col1 == TRUE && col2 == TRUE){
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_PVQ2_FLAG);
							}
							else{
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_VQ2_FLAG);
							}
							
							block_w = 2; block_h = 4;
						}
						
						agmv->vq2color1 = color1;
						agmv->vq2color2 = color2;
						
						frame_ptr = &frame[x+y*width];
						
						index1 = AGMV_FindNearestEntry(palette0,palette1,color1);
						index2 = AGMV_FindNearestEntry(palette0,palette1,color2);
						
						if(col1 == FALSE || col2 == FALSE){
							if(opt != AGMV_OPT_IV && opt != AGMV_OPT_GBA_IV && opt != AGMV_OPT_PSX_IV && opt != AGMV_OPT_NDS_IV){						
								if(index1.index < 127){
									data[bitpos++] = index1.pal_num << 7 | index1.index;
								}
								else{
									data[bitpos++] = index1.pal_num << 7 | 127;
									data[bitpos++] = index1.index;
								}
								
								if(index2.index < 127){
									data[bitpos++] = index2.pal_num << 7 | index2.index;
								}
								else{
									data[bitpos++] = index2.pal_num << 7 | 127;
									data[bitpos++] = index2.index;
								}
							}
							else{
								color1_16 = AGMV_ToRGB16(color1);
								color2_16 = AGMV_ToRGB16(color2);
								
								data[bitpos++] = AGMV_GetMSB(color1_16);
								data[bitpos++] = AGMV_GetLSB(color1_16);
								
								data[bitpos++] = AGMV_GetMSB(color2_16);
								data[bitpos++] = AGMV_GetLSB(color2_16);
							}
						}
						
						r1 = AGMV_GetR(color1);
						g1 = AGMV_GetG(color1);
						b1 = AGMV_GetB(color1);
						
						r2 = AGMV_GetR(color2);
						g2 = AGMV_GetG(color2);
						b2 = AGMV_GetB(color2);
						
						for(j = 0; j < block_h; j++){
							for(i = 0; i < block_w; i++){
								color = frame_ptr[i];
								
								r = AGMV_GetR(color);
								g = AGMV_GetG(color);
								b = AGMV_GetB(color);

								rdiff1 = r1-r;
								gdiff1 = g1-g;
								bdiff1 = b1-b;
								
								rdiff2 = r2-r;
								gdiff2 = g2-g;
								bdiff2 = b2-b;
								
								dist1 = rdiff1*rdiff1 + gdiff1*gdiff1 + bdiff1*bdiff1;
								dist2 = rdiff2*rdiff2 + gdiff2*gdiff2 + bdiff2*bdiff2;
								
								if(dist1 < dist2){
									indices[count++] = 0;
								}
								else{
									indices[count++] = 1;
								}
							}
							
							frame_ptr += width;
						}
						
						if(block_w == 8 && block_h == 8){
							byte1 = indices[0] << 7 | indices[1] << 6 | indices[2] << 5 | indices[3] << 4 | indices[4] << 3 | indices[5] << 2 | indices[6] << 1 | indices[7];
							byte2 = indices[8] << 7 | indices[9] << 6 | indices[10] << 5 | indices[11] << 4 | indices[12] << 3 | indices[13] << 2 | indices[14] << 1 | indices[15];
							byte3 = indices[16] << 7 | indices[17] << 6 | indices[18] << 5 | indices[19] << 4 | indices[20] << 3 | indices[21] << 2 | indices[22] << 1 | indices[23];
							byte4 = indices[24] << 7 | indices[25] << 6 | indices[26] << 5 | indices[27] << 4 | indices[28] << 3 | indices[29] << 2 | indices[30] << 1 | indices[31];
							byte5 = indices[32] << 7 | indices[33] << 6 | indices[34] << 5 | indices[35] << 4 | indices[36] << 3 | indices[37] << 2 | indices[38] << 1 | indices[39];
							byte6 = indices[40] << 7 | indices[41] << 6 | indices[42] << 5 | indices[43] << 4 | indices[44] << 3 | indices[45] << 2 | indices[46] << 1 | indices[47];
							byte7 = indices[48] << 7 | indices[49] << 6 | indices[50] << 5 | indices[51] << 4 | indices[52] << 3 | indices[53] << 2 | indices[54] << 1 | indices[55];
							byte8 = indices[56] << 7 | indices[57] << 6 | indices[58] << 5 | indices[59] << 4 | indices[60] << 3 | indices[61] << 2 | indices[62] << 1 | indices[63];

							
							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
							data[bitpos++] = byte3;
							data[bitpos++] = byte4;
							data[bitpos++] = byte5;
							data[bitpos++] = byte6;
							data[bitpos++] = byte7;
							data[bitpos++] = byte8;
							
							x += 4;
						}
						else if(block_w == 8){
							byte1 = indices[0] << 7 | indices[1] << 6 | indices[2] << 5 | indices[3] << 4 | indices[4] << 3 | indices[5] << 2 | indices[6] << 1 | indices[7];
							byte2 = indices[8] << 7 | indices[9] << 6 | indices[10] << 5 | indices[11] << 4 | indices[12] << 3 | indices[13] << 2 | indices[14] << 1 | indices[15];
							byte3 = indices[16] << 7 | indices[17] << 6 | indices[18] << 5 | indices[19] << 4 | indices[20] << 3 | indices[21] << 2 | indices[22] << 1 | indices[23];
							byte4 = indices[24] << 7 | indices[25] << 6 | indices[26] << 5 | indices[27] << 4 | indices[28] << 3 | indices[29] << 2 | indices[30] << 1 | indices[31];
						
							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
							data[bitpos++] = byte3;
							data[bitpos++] = byte4;
							
							x += 4;
						}
						else if(block_w == 4){
							byte1 = indices[0] << 7 | indices[1] << 6 | indices[2] << 5 | indices[3] << 4 | indices[4] << 3 | indices[5] << 2 | indices[6] << 1 | indices[7];
							byte2 = indices[8] << 7 | indices[9] << 6 | indices[10] << 5 | indices[11] << 4 | indices[12] << 3 | indices[13] << 2 | indices[14] << 1 | indices[15];
						
							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
						}
						else{
							byte1 = indices[0] << 7 | indices[1] << 6 | indices[2] << 5 | indices[3] << 4 | indices[4] << 3 | indices[5] << 2 | indices[6] << 1 | indices[7];					
							data[bitpos++] = byte1;
							
							x -= 2;
						}
					}
					else if(vq_count.c4x4.num > 0 && vq_count.c4x4.num <= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_VQ4_FLAG,quality,opt)){
						u32* frame_ptr, color1, color2, color3, color4, dist1, dist2, dist3, dist4, min, block_w, block_h;
						u16 color1_16, color2_16, color3_16, color4_16;
						AGMV_ENTRY index1, index2, index3, index4;
						int r, g, b, r1, g1, b1, r2, g2, b2, r3, g3, b3, r4, g4, b4, rdiff1, gdiff1, bdiff1, rdiff2, gdiff2, bdiff2;
						int rdiff3, gdiff3, bdiff3, rdiff4, gdiff4, bdiff4;
						u8 count = 0, byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, byte9, byte10, byte11, byte12, byte13, byte14, byte15, byte16;
						u8 indices[64];
						
						if(vq_count.c8x8.num > 0 && vq_count.c8x8.num <= AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_VQ4_FLAG,quality,opt)){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_VQ4_FLAG);
						
							color1 = vq_count.c8x8.coffre[vq_count.c8x8.num-1].color;
							color2 = vq_count.c8x8.coffre[vq_count.c8x8.num-2].color;
							color3 = vq_count.c8x8.coffre[vq_count.c8x8.num-3].color;
							color4 = vq_count.c8x8.coffre[vq_count.c8x8.num-4].color;

							block_w = 8; block_h = 8;
						}
						else if(vq_count.c8x4.num > 0 && vq_count.c8x4.num <= AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_VQ4_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_VQ4_FLAG);
						
							color1 = vq_count.c8x4.coffre[vq_count.c8x4.num-1].color;
							color2 = vq_count.c8x4.coffre[vq_count.c8x4.num-2].color;
							color3 = vq_count.c8x4.coffre[vq_count.c8x4.num-3].color;
							color4 = vq_count.c8x4.coffre[vq_count.c8x4.num-4].color;

							block_w = 8; block_h = 4;
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_VQ4_FLAG);
							
							color1 = vq_count.c4x4.coffre[vq_count.c4x4.num-1].color;
							color2 = vq_count.c4x4.coffre[vq_count.c4x4.num-2].color;
							color3 = vq_count.c4x4.coffre[vq_count.c4x4.num-3].color;
							color4 = vq_count.c4x4.coffre[vq_count.c4x4.num-4].color;
							
							block_w = 4; block_h = 4;
						}

						index1 = AGMV_FindNearestEntry(palette0,palette1,color1);
						index2 = AGMV_FindNearestEntry(palette0,palette1,color2);
						index3 = AGMV_FindNearestEntry(palette0,palette1,color3);
						index4 = AGMV_FindNearestEntry(palette0,palette1,color4);
						
						if(opt != AGMV_OPT_IV && opt != AGMV_OPT_GBA_IV && opt != AGMV_OPT_PSX_IV && opt != AGMV_OPT_NDS_IV){
							if(index1.index < 127){
								data[bitpos++] = index1.pal_num << 7 | index1.index;
							}
							else{
								data[bitpos++] = index1.pal_num << 7 | 127;
								data[bitpos++] = index1.index;
							}
							
							if(index2.index < 127){
								data[bitpos++] = index2.pal_num << 7 | index2.index;
							}
							else{
								data[bitpos++] = index2.pal_num << 7 | 127;
								data[bitpos++] = index2.index;
							}
							
							if(index3.index < 127){
								data[bitpos++] = index3.pal_num << 7 | index3.index;
							}
							else{
								data[bitpos++] = index3.pal_num << 7 | 127;
								data[bitpos++] = index3.index;
							}
							
							if(index4.index < 127){
								data[bitpos++] = index4.pal_num << 7 | index4.index;
							}
							else{
								data[bitpos++] = index4.pal_num << 7 | 127;
								data[bitpos++] = index4.index;
							}
						}
						else{
							color1_16 = AGMV_ToRGB16(color1);
							color2_16 = AGMV_ToRGB16(color2);
							color3_16 = AGMV_ToRGB16(color3);
							color4_16 = AGMV_ToRGB16(color4);
							
							data[bitpos++] = AGMV_GetMSB(color1_16);
							data[bitpos++] = AGMV_GetLSB(color1_16);
							
							data[bitpos++] = AGMV_GetMSB(color2_16);
							data[bitpos++] = AGMV_GetLSB(color2_16);
							
							data[bitpos++] = AGMV_GetMSB(color3_16);
							data[bitpos++] = AGMV_GetLSB(color3_16);
							
							data[bitpos++] = AGMV_GetMSB(color4_16);
							data[bitpos++] = AGMV_GetLSB(color4_16);
						}
						
						frame_ptr = &frame[x+y*width];
							
						r1 = AGMV_GetR(color1);
						g1 = AGMV_GetG(color1);
						b1 = AGMV_GetB(color1);
						
						r2 = AGMV_GetR(color2);
						g2 = AGMV_GetG(color2);
						b2 = AGMV_GetB(color2);
						
						r3 = AGMV_GetR(color3);
						g3 = AGMV_GetG(color3);
						b3 = AGMV_GetB(color3);
						
						r4 = AGMV_GetR(color4);
						g4 = AGMV_GetG(color4);
						b4 = AGMV_GetB(color4);
						
						for(j = 0; j < block_h; j++){
							for(i = 0; i < block_w; i++){
								color = frame_ptr[i];
								
								r = AGMV_GetR(color);
								g = AGMV_GetG(color);
								b = AGMV_GetB(color);
								
								rdiff1 = r1-r;
								gdiff1 = g1-g;
								bdiff1 = b1-b;
								
								rdiff2 = r2-r;
								gdiff2 = g2-g;
								bdiff2 = b2-b;
								
								rdiff3 = r3-r;
								gdiff3 = g3-g;
								bdiff3 = b3-b;
								
								rdiff4 = r4-r;
								gdiff4 = g4-g;
								bdiff4 = b4-b;
								
								dist1 = rdiff1*rdiff1 + gdiff1*gdiff1 + bdiff1*bdiff1;
								dist2 = rdiff2*rdiff2 + gdiff2*gdiff2 + bdiff2*bdiff2;
								dist3 = rdiff3*rdiff3 + gdiff3*gdiff3 + bdiff3*bdiff3;
								dist4 = rdiff4*rdiff4 + gdiff4*gdiff4 + bdiff4*bdiff4;
								
								min = AGMV_Min(dist1,dist2);
								min = AGMV_Min(dist3,min);
								min = AGMV_Min(dist4,min);
								
								if(min == dist1){
									indices[count++] = 0;
								}
								else if(min == dist2){
									indices[count++] = 1;
								}
								else if(min == dist3){
									indices[count++] = 2;
								}
								else{
									indices[count++] = 3;
								}
							}
							
							frame_ptr += width;
						}
						
						if(block_w == 4){
							byte1 = indices[0] << 6 | indices[1] << 4 | indices[2] << 2 | indices[3];
							byte2 = indices[4] << 6 | indices[5] << 4 | indices[6] << 2 | indices[7];
							byte3 = indices[8] << 6 | indices[9] << 4 | indices[10] << 2 | indices[11];
							byte4 = indices[12] << 6 | indices[13] << 4 | indices[14] << 2 | indices[15];
							
							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
							data[bitpos++] = byte3;
							data[bitpos++] = byte4;
						}
						else if(block_w == 8 && block_h == 4){
							byte1 = indices[0] << 6 | indices[1] << 4 | indices[2] << 2 | indices[3];
							byte2 = indices[4] << 6 | indices[5] << 4 | indices[6] << 2 | indices[7];
							byte3 = indices[8] << 6 | indices[9] << 4 | indices[10] << 2 | indices[11];
							byte4 = indices[12] << 6 | indices[13] << 4 | indices[14] << 2 | indices[15];
							byte5 = indices[16] << 6 | indices[17] << 4 | indices[18] << 2 | indices[19];
							byte6 = indices[20] << 6 | indices[21] << 4 | indices[22] << 2 | indices[23];
							byte7 = indices[24] << 6 | indices[25] << 4 | indices[26] << 2 | indices[27];
							byte8 = indices[28] << 6 | indices[29] << 4 | indices[30] << 2 | indices[31];
							
							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
							data[bitpos++] = byte3;
							data[bitpos++] = byte4;
							data[bitpos++] = byte5;
							data[bitpos++] = byte6;
							data[bitpos++] = byte7;
							data[bitpos++] = byte8;
							
							x += 4;
						}
						else{
							byte1 = indices[0] << 6 | indices[1] << 4 | indices[2] << 2 | indices[3];
							byte2 = indices[4] << 6 | indices[5] << 4 | indices[6] << 2 | indices[7];
							byte3 = indices[8] << 6 | indices[9] << 4 | indices[10] << 2 | indices[11];
							byte4 = indices[12] << 6 | indices[13] << 4 | indices[14] << 2 | indices[15];
							byte5 = indices[16] << 6 | indices[17] << 4 | indices[18] << 2 | indices[19];
							byte6 = indices[20] << 6 | indices[21] << 4 | indices[22] << 2 | indices[23];
							byte7 = indices[24] << 6 | indices[25] << 4 | indices[26] << 2 | indices[27];
							byte8 = indices[28] << 6 | indices[29] << 4 | indices[30] << 2 | indices[31];
							byte9 = indices[32] << 6 | indices[33] << 4 | indices[34] << 2 | indices[35];
							byte10 = indices[36] << 6 | indices[37] << 4 | indices[38] << 2 | indices[39];
							byte11 = indices[40] << 6 | indices[41] << 4 | indices[42] << 2 | indices[43];
							byte12 = indices[44] << 6 | indices[45] << 4 | indices[46] << 2 | indices[47];
							byte13 = indices[48] << 6 | indices[49] << 4 | indices[50] << 2 | indices[51];
							byte14 = indices[52] << 6 | indices[53] << 4 | indices[54] << 2 | indices[55];
							byte15 = indices[56] << 6 | indices[57] << 4 | indices[58] << 2 | indices[59];
							byte16 = indices[60] << 6 | indices[61] << 4 | indices[62] << 2 | indices[63];
							
							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
							data[bitpos++] = byte3;
							data[bitpos++] = byte4;
							data[bitpos++] = byte5;
							data[bitpos++] = byte6;
							data[bitpos++] = byte7;
							data[bitpos++] = byte8;
							data[bitpos++] = byte9;
							data[bitpos++] = byte10;
							data[bitpos++] = byte11;
							data[bitpos++] = byte12;
							data[bitpos++] = byte13;
							data[bitpos++] = byte14;
							data[bitpos++] = byte15;
							data[bitpos++] = byte16;
							
							x += 4;
						}
					}
					else{
						data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_NORMAL_FLAG);
					
						AGMV_ENTRY norm_arr[5], *img_entryp;
						u32 color_arr[5], *frame_ptr;
						
						img_entryp = &img_entry[x+y*width];
						frame_ptr  = &frame[x+y*width];
						
						if(opt != AGMV_OPT_IV && opt != AGMV_OPT_GBA_IV && opt != AGMV_OPT_PSX_IV && opt != AGMV_OPT_NDS_IV){
							norm_arr[0] = img_entryp[0];
							norm_arr[1] = img_entryp[1];
							
							img_entryp += width;
							img_entryp += width;
							
							norm_arr[2] = img_entryp[0];
							norm_arr[3] = img_entryp[1];
							
							img_entryp += width;
							
							norm_arr[4] = img_entryp[0];
							
							for(i = 0; i < 5; i++){
								if(norm_arr[i].index < 127){
									data[bitpos++] = norm_arr[i].pal_num << 7 | norm_arr[i].index;
								}
								else{
									data[bitpos++] = norm_arr[i].pal_num << 7 | 127;
									data[bitpos++] = norm_arr[i].index;
								}
							}
						}
						else{
							color_arr[0] = frame_ptr[0];
							color_arr[1] = frame_ptr[1];
							
							frame_ptr += width;
							frame_ptr += width;
							
							color_arr[2] = frame_ptr[0];
							color_arr[3] = frame_ptr[1];
							
							frame_ptr += width;
							
							color_arr[4] = frame_ptr[0];
							
							for(i = 0; i < 5; i++){
								color16 = AGMV_ToRGB16(color_arr[i]);
								
								data[bitpos++] = AGMV_GetMSB(color16);
								data[bitpos++] = AGMV_GetLSB(color16);
							}
						}
						
						x -= 2;
					}
				}
			}
			
			for(n = 0; n < agmv->db.curr_pos; n++){
				agmv->db.read_table[n] = agmv->db.curr_table[n];
			}
			
			agmv->db.read_pos = agmv->db.curr_pos;
			agmv->db.curr_pos = 0;
		}
	}
	else{
		for(y = 0; y < height; y += 4){
			offset1 = y*width;
			for(x = 0; x < width; x += 4){
				
				skip = FALSE;

				for(n = 0; n < agmv->db.read_pos; n++){
					if(x == agmv->db.read_table[n]){
						data[bitpos++] = AGMV_SKIP_8;
						x += 4;
						skip = TRUE;
						break;
					}
				}
				
				if(skip == FALSE){
					entry = img_entry[x+offset1];
		
					color = palette0[entry.index];

					fill_count = AGMV_GetFillBlockCount(agmv,x,y,color);
					
					copy_count.b4x4 = 0; copy_count.copyx1 = 0; copy_count.copyy1 = 0;
					copy_count.b2x4 = 0; copy_count.copyx2 = 0; copy_count.copyy2 = 0;
					copy_count.b8x4 = 0; copy_count.copyx3 = 0; copy_count.copyy3 = 0;
					copy_count.b8x8 = 0; copy_count.copyx4 = 0; copy_count.copyy4 = 0;
					
					icopyr_count.b4x4 = 0; icopyr_count.copyx1 = 0; icopyr_count.copyy1 = 0;
					icopyr_count.b2x4 = 0; icopyr_count.copyx2 = 0; icopyr_count.copyy2 = 0;
					icopyr_count.b8x4 = 0; icopyr_count.copyx3 = 0; icopyr_count.copyy3 = 0;
					icopyr_count.b8x8 = 0; icopyr_count.copyx4 = 0; icopyr_count.copyy4 = 0;
					
					vq_count.c4x4.num = 0; vq_count.c8x4.num = 0; vq_count.c2x4.num = 0; vq_count.c8x8.num = 0;
					
					if(fill_count.b4x4 <= fill4x4 || fill_count.b8x4 <= fill8x4 || fill_count.b2x4 <= fill2x4 || fill_count.b8x8 <= fill8x8){ 
						vq_count = AGMV_GetVQBlockCount(agmv,x,y);
						copy_count = AGMV_GetIntraCopyBlockCount(agmv,x,y);
						icopyr_count = AGMV_GetRightIntraCopyBlockCount(agmv,x,y);
					}
					
					r1 = AGMV_GetR(color);
					g1 = AGMV_GetG(color);
					b1 = AGMV_GetB(color);
					
					r2 = AGMV_GetR(agmv->prev_fill_color);
					g2 = AGMV_GetG(agmv->prev_fill_color);
					b2 = AGMV_GetB(agmv->prev_fill_color);
					
					diffr = AGMV_Abs(r1-r2);
					diffg = AGMV_Abs(g1-g2);
					diffb = AGMV_Abs(b1-b2);
					
					if((diffr <= max_diff && diffg <= max_diff && diffb <= max_diff) && (fill_count.b4x4 >= fill4x4 || fill_count.b8x4 >= fill8x4 || fill_count.b2x4 >= fill2x4 || fill_count.b8x8 >= fill8x8)){
						if(fill_count.b8x8 >= fill8x8){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_PFILL_FLAG);
							x += 4;
						}
						else if(fill_count.b8x4 >= fill8x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_PFILL_FLAG);
							x += 4;
						}
						else if(fill_count.b4x4 >= fill4x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_PFILL_FLAG);
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_PFILL_FLAG);
							x -= 2;
						}
					}
					else if(fill_count.b4x4 >= fill4x4 || fill_count.b8x4 >= fill8x4 || fill_count.b2x4 >= fill2x4 || fill_count.b8x8 >= fill8x8){
						
						if(fill_count.b8x8 >= fill8x8){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_FILL_FLAG);
							x += 4;
						}
						else if(fill_count.b8x4 >= fill8x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_FILL_FLAG);
							x += 4;
						}
						else if(fill_count.b4x4 >= fill4x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_FILL_FLAG);
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_FILL_FLAG);
							x -= 2;
						}

						data[bitpos++] = entry.index;
						agmv->prev_fill_color = color;
					}
					else if(copy_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_ICOPY_FLAG,quality,opt) || copy_count.b2x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_ICOPY_FLAG,quality,opt)){
						if(copy_count.b8x8 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_ICOPY_FLAG,quality,opt)){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_ICOPY_FLAG);
							data[bitpos++] = (x - copy_count.copyx4) << 4 | (y - copy_count.copyy4);

							x += 4;
						}
						else if(copy_count.b8x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_ICOPY_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_ICOPY_FLAG);
							data[bitpos++] = (x - copy_count.copyx2) << 4 | (y - copy_count.copyy2);
							
							x += 4;
						}
						else if(copy_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_ICOPY_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_ICOPY_FLAG);
							data[bitpos++] = (x - copy_count.copyx1) << 4 | (y - copy_count.copyy1);
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_ICOPY_FLAG);
							data[bitpos++] = (x - copy_count.copyx3) << 4 | (y - copy_count.copyy3);
							
							x -= 2;
						}
					}
					else if(icopyr_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_ICOPY_FLAG,quality,opt) || icopyr_count.b2x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_ICOPY_FLAG,quality,opt)){
						if(icopyr_count.b8x8 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_ICOPY_FLAG,quality,opt)){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_ICOPYR_FLAG);
							data[bitpos++] = (icopyr_count.copyx4 - x) << 4 | (y - icopyr_count.copyy4);

							x += 4;
						}
						else if(icopyr_count.b8x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_ICOPY_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_ICOPYR_FLAG);
							data[bitpos++] = (icopyr_count.copyx2 - x) << 4 | (y - icopyr_count.copyy2);

							x += 4;
						}
						else if(icopyr_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_ICOPY_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_ICOPYR_FLAG);
							data[bitpos++] = (icopyr_count.copyx1 - x) << 4 | (y - icopyr_count.copyy1);
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_ICOPYR_FLAG);
							data[bitpos++] = (icopyr_count.copyx3 - x) << 4 | (y - icopyr_count.copyy3);
							
							x -= 2;
						}
					}
					else if(vq_count.c2x4.num > 0 && vq_count.c2x4.num <= AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_VQ2_FLAG,quality,opt)){
						u32* frame_ptr, color1, color2, dist1, dist2;
						u32 block_w, block_h;
						u8 index1, index2;
						int r, g, b, r1, g1, b1, r2, g2, b2, rdiff1, gdiff1, bdiff1, rdiff2, gdiff2, bdiff2;
						u8 count = 0, byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8;
						Bool col1, col2;
						u8 indices[64];
						
						col1 = FALSE;
						col2 = FALSE;

						if(vq_count.c8x8.num > 0 && vq_count.c8x8.num <= AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_VQ2_FLAG,quality,opt)){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							
							color1 = vq_count.c8x8.coffre[vq_count.c8x8.num-1].color;
							color2 = vq_count.c8x8.coffre[vq_count.c8x8.num-2].color;
							
							r1 = AGMV_GetR(color1);
							g1 = AGMV_GetG(color1);
							b1 = AGMV_GetB(color1);
							
							r2 = AGMV_GetR(agmv->vq2color1);
							g2 = AGMV_GetG(agmv->vq2color1);
							b2 = AGMV_GetB(agmv->vq2color1);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col1 = TRUE;
							}
							
							r1 = AGMV_GetR(color2);
							g1 = AGMV_GetG(color2);
							b1 = AGMV_GetB(color2);
							
							r2 = AGMV_GetR(agmv->vq2color2);
							g2 = AGMV_GetG(agmv->vq2color2);
							b2 = AGMV_GetB(agmv->vq2color2);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col2 = TRUE;
							}
							
							if(col1 == TRUE && col2 == TRUE){
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_PVQ2_FLAG);
							}
							else{
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_VQ2_FLAG);
							}
							
							block_w = 8; block_h = 8;
						}
						else if(vq_count.c8x4.num > 0 && vq_count.c8x4.num <= AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_VQ2_FLAG,quality,opt)){	
							color1 = vq_count.c8x4.coffre[vq_count.c8x4.num-1].color;
							color2 = vq_count.c8x4.coffre[vq_count.c8x4.num-2].color;
							
							r1 = AGMV_GetR(color1);
							g1 = AGMV_GetG(color1);
							b1 = AGMV_GetB(color1);
							
							r2 = AGMV_GetR(agmv->vq2color1);
							g2 = AGMV_GetG(agmv->vq2color1);
							b2 = AGMV_GetB(agmv->vq2color1);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col1 = TRUE;
							}
							
							r1 = AGMV_GetR(color2);
							g1 = AGMV_GetG(color2);
							b1 = AGMV_GetB(color2);
							
							r2 = AGMV_GetR(agmv->vq2color2);
							g2 = AGMV_GetG(agmv->vq2color2);
							b2 = AGMV_GetB(agmv->vq2color2);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col2 = TRUE;
							}
							
							if(col1 == TRUE && col2 == TRUE){
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_PVQ2_FLAG);
							}
							else{
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_VQ2_FLAG);
							}
							
							block_w = 8; block_h = 4;
						}
						else if(vq_count.c4x4.num > 0 && vq_count.c4x4.num <= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_VQ2_FLAG,quality,opt)){
							color1 = vq_count.c4x4.coffre[vq_count.c4x4.num-1].color;
							color2 = vq_count.c4x4.coffre[vq_count.c4x4.num-2].color;
							
							r1 = AGMV_GetR(color1);
							g1 = AGMV_GetG(color1);
							b1 = AGMV_GetB(color1);
							
							r2 = AGMV_GetR(agmv->vq2color1);
							g2 = AGMV_GetG(agmv->vq2color1);
							b2 = AGMV_GetB(agmv->vq2color1);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col1 = TRUE;
							}
							
							r1 = AGMV_GetR(color2);
							g1 = AGMV_GetG(color2);
							b1 = AGMV_GetB(color2);
							
							r2 = AGMV_GetR(agmv->vq2color2);
							g2 = AGMV_GetG(agmv->vq2color2);
							b2 = AGMV_GetB(agmv->vq2color2);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col2 = TRUE;
							}
							
							if(col1 == TRUE && col2 == TRUE){
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_PVQ2_FLAG);
							}
							else{
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_VQ2_FLAG);
							}
							
							block_w = 4; block_h = 4;
						}
						else{
							color1 = vq_count.c2x4.coffre[vq_count.c2x4.num-1].color;
							color2 = vq_count.c2x4.coffre[vq_count.c2x4.num-2].color;
						
							r1 = AGMV_GetR(color1);
							g1 = AGMV_GetG(color1);
							b1 = AGMV_GetB(color1);
							
							r2 = AGMV_GetR(agmv->vq2color1);
							g2 = AGMV_GetG(agmv->vq2color1);
							b2 = AGMV_GetB(agmv->vq2color1);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col1 = TRUE;
							}
							
							r1 = AGMV_GetR(color2);
							g1 = AGMV_GetG(color2);
							b1 = AGMV_GetB(color2);
							
							r2 = AGMV_GetR(agmv->vq2color2);
							g2 = AGMV_GetG(agmv->vq2color2);
							b2 = AGMV_GetB(agmv->vq2color2);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col2 = TRUE;
							}
							
							if(col1 == TRUE && col2 == TRUE){
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_PVQ2_FLAG);
							}
							else{
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_VQ2_FLAG);
							}
							
							block_w = 2; block_h = 4;
						}
						
						agmv->vq2color1 = color1;
						agmv->vq2color2 = color2;
						
						index1 = AGMV_FindNearestColor(palette0,color1);
						index2 = AGMV_FindNearestColor(palette0,color2);
						
						if(col1 == FALSE || col2 == FALSE){
							data[bitpos++] = index1;
							data[bitpos++] = index2;
						}
						
						frame_ptr = &frame[x+y*width];
				
						r1 = AGMV_GetR(color1);
						g1 = AGMV_GetG(color1);
						b1 = AGMV_GetB(color1);
						
						r2 = AGMV_GetR(color2);
						g2 = AGMV_GetG(color2);
						b2 = AGMV_GetB(color2);
		
						for(j = 0; j < block_h; j++){
							for(i = 0; i < block_w; i++){
								color = frame_ptr[i];
								
								r = AGMV_GetR(color);
								g = AGMV_GetG(color);
								b = AGMV_GetB(color);
								
								rdiff1 = r1-r;
								gdiff1 = g1-g;
								bdiff1 = b1-b;
								
								rdiff2 = r2-r;
								gdiff2 = g2-g;
								bdiff2 = b2-b;
								
								dist1 = rdiff1*rdiff1 + gdiff1*gdiff1 + bdiff1*bdiff1;
								dist2 = rdiff2*rdiff2 + gdiff2*gdiff2 + bdiff2*bdiff2;
								
								if(dist1 < dist2){
									indices[count++] = 0;
								}
								else{
									indices[count++] = 1;
								}
							}
							
							frame_ptr += width;
						}
						
						if(block_w == 8 && block_h == 8){
							byte1 = indices[0] << 7 | indices[1] << 6 | indices[2] << 5 | indices[3] << 4 | indices[4] << 3 | indices[5] << 2 | indices[6] << 1 | indices[7];
							byte2 = indices[8] << 7 | indices[9] << 6 | indices[10] << 5 | indices[11] << 4 | indices[12] << 3 | indices[13] << 2 | indices[14] << 1 | indices[15];
							byte3 = indices[16] << 7 | indices[17] << 6 | indices[18] << 5 | indices[19] << 4 | indices[20] << 3 | indices[21] << 2 | indices[22] << 1 | indices[23];
							byte4 = indices[24] << 7 | indices[25] << 6 | indices[26] << 5 | indices[27] << 4 | indices[28] << 3 | indices[29] << 2 | indices[30] << 1 | indices[31];
							byte5 = indices[32] << 7 | indices[33] << 6 | indices[34] << 5 | indices[35] << 4 | indices[36] << 3 | indices[37] << 2 | indices[38] << 1 | indices[39];
							byte6 = indices[40] << 7 | indices[41] << 6 | indices[42] << 5 | indices[43] << 4 | indices[44] << 3 | indices[45] << 2 | indices[46] << 1 | indices[47];
							byte7 = indices[48] << 7 | indices[49] << 6 | indices[50] << 5 | indices[51] << 4 | indices[52] << 3 | indices[53] << 2 | indices[54] << 1 | indices[55];
							byte8 = indices[56] << 7 | indices[57] << 6 | indices[58] << 5 | indices[59] << 4 | indices[60] << 3 | indices[61] << 2 | indices[62] << 1 | indices[63];

							
							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
							data[bitpos++] = byte3;
							data[bitpos++] = byte4;
							data[bitpos++] = byte5;
							data[bitpos++] = byte6;
							data[bitpos++] = byte7;
							data[bitpos++] = byte8;
							
							x += 4;
						}
						else if(block_w == 8){
							byte1 = indices[0] << 7 | indices[1] << 6 | indices[2] << 5 | indices[3] << 4 | indices[4] << 3 | indices[5] << 2 | indices[6] << 1 | indices[7];
							byte2 = indices[8] << 7 | indices[9] << 6 | indices[10] << 5 | indices[11] << 4 | indices[12] << 3 | indices[13] << 2 | indices[14] << 1 | indices[15];
							byte3 = indices[16] << 7 | indices[17] << 6 | indices[18] << 5 | indices[19] << 4 | indices[20] << 3 | indices[21] << 2 | indices[22] << 1 | indices[23];
							byte4 = indices[24] << 7 | indices[25] << 6 | indices[26] << 5 | indices[27] << 4 | indices[28] << 3 | indices[29] << 2 | indices[30] << 1 | indices[31];
						
							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
							data[bitpos++] = byte3;
							data[bitpos++] = byte4;
							
							x += 4;
						}
						else if(block_w == 4){
							byte1 = indices[0] << 7 | indices[1] << 6 | indices[2] << 5 | indices[3] << 4 | indices[4] << 3 | indices[5] << 2 | indices[6] << 1 | indices[7];
							byte2 = indices[8] << 7 | indices[9] << 6 | indices[10] << 5 | indices[11] << 4 | indices[12] << 3 | indices[13] << 2 | indices[14] << 1 | indices[15];
						
							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
						}
						else{
							byte1 = indices[0] << 7 | indices[1] << 6 | indices[2] << 5 | indices[3] << 4 | indices[4] << 3 | indices[5] << 2 | indices[6] << 1 | indices[7];					
							data[bitpos++] = byte1;
	
							data[bitpos++] = byte1;
							
							x -= 2;
						}
					}
					else if(vq_count.c4x4.num > 0 && vq_count.c4x4.num <= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_VQ4_FLAG,quality,opt)){
						u32* frame_ptr, color1, color2, color3, color4, dist1, dist2, dist3, dist4, min, block_w, block_h;
						u8 index1, index2, index3, index4;
						int r, g, b, r1, g1, b1, r2, g2, b2, r3, g3, b3, r4, g4, b4, rdiff1, gdiff1, bdiff1, rdiff2, gdiff2, bdiff2;
						int rdiff3, gdiff3, bdiff3, rdiff4, gdiff4, bdiff4;
						u8 count = 0, byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, byte9, byte10, byte11, byte12, byte13, byte14, byte15, byte16;
						u8 indices[64];
						
						if(vq_count.c8x8.num > 0 && vq_count.c8x8.num <= AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_VQ4_FLAG,quality,opt)){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_VQ4_FLAG);
						
							color1 = vq_count.c8x8.coffre[vq_count.c8x8.num-1].color;
							color2 = vq_count.c8x8.coffre[vq_count.c8x8.num-2].color;
							color3 = vq_count.c8x8.coffre[vq_count.c8x8.num-3].color;
							color4 = vq_count.c8x8.coffre[vq_count.c8x8.num-4].color;

							block_w = 8; block_h = 8;
						}
						else if(vq_count.c8x4.num > 0 && vq_count.c8x4.num <= AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_VQ4_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_VQ4_FLAG);
						
							color1 = vq_count.c8x4.coffre[vq_count.c8x4.num-1].color;
							color2 = vq_count.c8x4.coffre[vq_count.c8x4.num-2].color;
							color3 = vq_count.c8x4.coffre[vq_count.c8x4.num-3].color;
							color4 = vq_count.c8x4.coffre[vq_count.c8x4.num-4].color;

							block_w = 8; block_h = 4;
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_VQ4_FLAG);
							
							color1 = vq_count.c4x4.coffre[vq_count.c4x4.num-1].color;
							color2 = vq_count.c4x4.coffre[vq_count.c4x4.num-2].color;
							color3 = vq_count.c4x4.coffre[vq_count.c4x4.num-3].color;
							color4 = vq_count.c4x4.coffre[vq_count.c4x4.num-4].color;
							
							block_w = 4; block_h = 4;
						}
						
						index1 = AGMV_FindNearestColor(palette0,color1);
						index2 = AGMV_FindNearestColor(palette0,color2);
						index3 = AGMV_FindNearestColor(palette0,color3);
						index4 = AGMV_FindNearestColor(palette0,color4);
					
						data[bitpos++] = index1;
						data[bitpos++] = index2;
						data[bitpos++] = index3;
						data[bitpos++] = index4;
						
						frame_ptr = &frame[x+y*width];
						
						r1 = AGMV_GetR(color1);
						g1 = AGMV_GetG(color1);
						b1 = AGMV_GetB(color1);
						
						r2 = AGMV_GetR(color2);
						g2 = AGMV_GetG(color2);
						b2 = AGMV_GetB(color2);
						
						r3 = AGMV_GetR(color3);
						g3 = AGMV_GetG(color3);
						b3 = AGMV_GetB(color3);
						
						r4 = AGMV_GetR(color4);
						g4 = AGMV_GetG(color4);
						b4 = AGMV_GetB(color4);
						
						for(j = 0; j < block_h; j++){
							for(i = 0; i < block_w; i++){
								color = frame_ptr[i];
								
								r = AGMV_GetR(color);
								g = AGMV_GetG(color);
								b = AGMV_GetB(color);
								
								rdiff1 = r1-r;
								gdiff1 = g1-g;
								bdiff1 = b1-b;
								
								rdiff2 = r2-r;
								gdiff2 = g2-g;
								bdiff2 = b2-b;
								
								rdiff3 = r3-r;
								gdiff3 = g3-g;
								bdiff3 = b3-b;
								
								rdiff4 = r4-r;
								gdiff4 = g4-g;
								bdiff4 = b4-b;
								
								dist1 = rdiff1*rdiff1 + gdiff1*gdiff1 + bdiff1*bdiff1;
								dist2 = rdiff2*rdiff2 + gdiff2*gdiff2 + bdiff2*bdiff2;
								dist3 = rdiff3*rdiff3 + gdiff3*gdiff3 + bdiff3*bdiff3;
								dist4 = rdiff4*rdiff4 + gdiff4*gdiff4 + bdiff4*bdiff4;
								
								min = AGMV_Min(dist1,dist2);
								min = AGMV_Min(dist3,min);
								min = AGMV_Min(dist4,min);
								
								if(min == dist1){
									indices[count++] = 0;
								}
								else if(min == dist2){
									indices[count++] = 1;
								}
								else if(min == dist3){
									indices[count++] = 2;
								}
								else{
									indices[count++] = 3;
								}
							}
							
							frame_ptr += width;
						}
						
						if(block_w == 4){
							byte1 = indices[0] << 6 | indices[1] << 4 | indices[2] << 2 | indices[3];
							byte2 = indices[4] << 6 | indices[5] << 4 | indices[6] << 2 | indices[7];
							byte3 = indices[8] << 6 | indices[9] << 4 | indices[10] << 2 | indices[11];
							byte4 = indices[12] << 6 | indices[13] << 4 | indices[14] << 2 | indices[15];
							
							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
							data[bitpos++] = byte3;
							data[bitpos++] = byte4;
						}
						else if(block_w == 8 && block_h == 4){
							byte1 = indices[0] << 6 | indices[1] << 4 | indices[2] << 2 | indices[3];
							byte2 = indices[4] << 6 | indices[5] << 4 | indices[6] << 2 | indices[7];
							byte3 = indices[8] << 6 | indices[9] << 4 | indices[10] << 2 | indices[11];
							byte4 = indices[12] << 6 | indices[13] << 4 | indices[14] << 2 | indices[15];
							byte5 = indices[16] << 6 | indices[17] << 4 | indices[18] << 2 | indices[19];
							byte6 = indices[20] << 6 | indices[21] << 4 | indices[22] << 2 | indices[23];
							byte7 = indices[24] << 6 | indices[25] << 4 | indices[26] << 2 | indices[27];
							byte8 = indices[28] << 6 | indices[29] << 4 | indices[30] << 2 | indices[31];
							
							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
							data[bitpos++] = byte3;
							data[bitpos++] = byte4;
							data[bitpos++] = byte5;
							data[bitpos++] = byte6;
							data[bitpos++] = byte7;
							data[bitpos++] = byte8;
							
							x += 4;
						}
						else{
							byte1 = indices[0] << 6 | indices[1] << 4 | indices[2] << 2 | indices[3];
							byte2 = indices[4] << 6 | indices[5] << 4 | indices[6] << 2 | indices[7];
							byte3 = indices[8] << 6 | indices[9] << 4 | indices[10] << 2 | indices[11];
							byte4 = indices[12] << 6 | indices[13] << 4 | indices[14] << 2 | indices[15];
							byte5 = indices[16] << 6 | indices[17] << 4 | indices[18] << 2 | indices[19];
							byte6 = indices[20] << 6 | indices[21] << 4 | indices[22] << 2 | indices[23];
							byte7 = indices[24] << 6 | indices[25] << 4 | indices[26] << 2 | indices[27];
							byte8 = indices[28] << 6 | indices[29] << 4 | indices[30] << 2 | indices[31];
							byte9 = indices[32] << 6 | indices[33] << 4 | indices[34] << 2 | indices[35];
							byte10 = indices[36] << 6 | indices[37] << 4 | indices[38] << 2 | indices[39];
							byte11 = indices[40] << 6 | indices[41] << 4 | indices[42] << 2 | indices[43];
							byte12 = indices[44] << 6 | indices[45] << 4 | indices[46] << 2 | indices[47];
							byte13 = indices[48] << 6 | indices[49] << 4 | indices[50] << 2 | indices[51];
							byte14 = indices[52] << 6 | indices[53] << 4 | indices[54] << 2 | indices[55];
							byte15 = indices[56] << 6 | indices[57] << 4 | indices[58] << 2 | indices[59];
							byte16 = indices[60] << 6 | indices[61] << 4 | indices[62] << 2 | indices[63];
							
							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
							data[bitpos++] = byte3;
							data[bitpos++] = byte4;
							data[bitpos++] = byte5;
							data[bitpos++] = byte6;
							data[bitpos++] = byte7;
							data[bitpos++] = byte8;
							data[bitpos++] = byte9;
							data[bitpos++] = byte10;
							data[bitpos++] = byte11;
							data[bitpos++] = byte12;
							data[bitpos++] = byte13;
							data[bitpos++] = byte14;
							data[bitpos++] = byte15;
							data[bitpos++] = byte16;
							
							x += 4;
						}
					}
					else{
						data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_NORMAL_FLAG);
					
						AGMV_ENTRY norm_arr[5], *img_entryp;
						
						img_entryp = &img_entry[x+y*width];
	
						norm_arr[0] = img_entryp[0];
						norm_arr[1] = img_entryp[1];
						
						img_entryp += width;
						img_entryp += width;
						
						norm_arr[2] = img_entryp[0];
						norm_arr[3] = img_entryp[1];
						
						img_entryp += width;
						
						norm_arr[4] = img_entryp[0];

						for(i = 0; i < 5; i++){
							data[bitpos++] = norm_arr[i].index;
						}
						
						x -= 2;
					}
				}
			}
			
			for(n = 0; n < agmv->db.curr_pos; n++){
				agmv->db.read_table[n] = agmv->db.curr_table[n];
			}
			
			agmv->db.read_pos = agmv->db.curr_pos;
			agmv->db.curr_pos = 0;
		}
	}

	agmv->db.read_pos = 0;
	agmv->db.curr_pos = 0;
	
	agmv->bitstream->pos = bitpos;
}

void AGMV_AssemblePFrameBitstream(AGMV* agmv, AGMV_ENTRY* img_entry){
	AGMV_OPT opt;
	AGMV_ENTRY entry;
	AGMV_CRATE vq_count;
	AGMV_SECTOR sector, sector4x4;
	AGMV_BLOCK_COUNT fill_count, copy_count, pcopy_count, icopy_count, icopyr_count, mv_count, pmv_count;
	AGMV_SBLOCK_COUNT sblock_count, sblock_count4x4;
	AGMV_QUALITY quality;
	u32 width, height, i, j, n, pframe_count, offset1, bitpos, color, size;
	u32 flagc, fillc, pfillc, copyc, pcopyc, icopyc, mvc, pmvc, smvc;
	u32 vq2c, vq4c, nc;
	u32* palette0, *palette1, *prev_frame, *frame;
	u16 color16, max_diff;
	u8* data = agmv->bitstream->data, fill4x4, fill8x4, fill2x4, fill8x8, vq28x8, vq28x4, vq24x4, vq22x4, vq44x4, vq48x8, vq48x4, u1, u2;
	u8 pmv8x8, pmv8x4, pmv4x4, pmv2x4, pcopy8x8, pcopy8x4, pcopy4x4, pcopy2x4;
	int x, y, r1, g1, b1, r2, g2, b2, diffr, diffg, diffb;
	Bool skip;
	
	flagc = 0;
	fillc = 0; pfillc = 0; copyc = 0; pcopyc = 0; icopyc = 0; mvc = 0; pmvc = 0;
	smvc = 0; vq2c = 0; vq4c = 0; nc = 0;
	
	opt = AGMV_GetOPT(agmv);
	quality = AGMV_GetQuality(agmv);
	pframe_count = agmv->pframe_count;

	fill4x4 = AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_FILL_FLAG,quality,opt);
	fill8x4 = AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_FILL_FLAG,quality,opt);
	fill2x4 = AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_FILL_FLAG,quality,opt);
	fill8x8 = AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_FILL_FLAG,quality,opt);
	
	vq24x4 = AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_VQ2_FLAG,quality,opt);
	vq28x4 = AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_VQ2_FLAG,quality,opt);
	vq22x4 = AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_VQ2_FLAG,quality,opt);
	vq28x8 = AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_VQ2_FLAG,quality,opt);
	
	vq44x4 = AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_VQ4_FLAG,quality,opt);
	vq48x4 = AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_VQ4_FLAG,quality,opt);
	vq48x8 = AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_VQ4_FLAG,quality,opt);
	
	pmv4x4 = AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_PMV_FLAG,quality,opt);
	pmv8x4 = AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_PMV_FLAG,quality,opt);
	pmv2x4 = AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_PMV_FLAG,quality,opt);
	pmv8x8 = AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_PMV_FLAG,quality,opt);
	
	pcopy4x4 = AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_PCOPY_FLAG,quality,opt);
	pcopy8x4 = AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_PCOPY_FLAG,quality,opt);
	pcopy2x4 = AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_PCOPY_FLAG,quality,opt);
	pcopy8x8 = AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_PCOPY_FLAG,quality,opt);
	
	if(agmv->lenient){
		vq44x4 = 8;
		vq48x4 = 8;
		vq48x8 = 8;
		vq28x8 = 4;
		vq28x4 = 4;
		vq24x4 = 4;
		vq22x4 = 4;
		pmv8x8 -= 6;
		pmv8x4 -= 4;
		pmv4x4 -= 3;
		pmv2x4 -= 2;
		pcopy8x8 -= 5;
		pcopy8x4 -= 4;
		pcopy4x4 -= 3;
		pcopy2x4 -= 2;
		fill8x8 -= 3;
		fill8x4 -= 3;
		fill4x4 -= 3;
		fill2x4 -= 2;
	}
	
	max_diff = agmv->max_diff;
	width = agmv->frame->width;
	height = agmv->frame->height;
	
	size = width*height;
	
	palette0 = agmv->header.palette0;
	palette1 = agmv->header.palette1;
	
	prev_frame = agmv->prev_frame->img_data;
	frame = agmv->frame->img_data;
	
	bitpos = agmv->bitstream->pos;
	
	agmv->db.read_pos = 0;
	agmv->db.curr_pos = 0;
	
	if(opt != AGMV_OPT_II && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_II){
		for(y = 0; y < height; y += 4){
			offset1 = y*width;
			for(x = 0; x < width; x += 4){
				skip = FALSE;

				for(n = 0; n < agmv->db.read_pos; n++){
					if(x == agmv->db.read_table[n]){
						data[bitpos++] = AGMV_SKIP_8;
						x += 4;
						skip = TRUE;
						break;
					}
				}
				
				if(skip == FALSE){
					if(opt != AGMV_OPT_IV && opt != AGMV_OPT_GBA_IV && opt != AGMV_OPT_PSX_IV && opt != AGMV_OPT_NDS_IV){
						entry = img_entry[x+offset1];
			
						if(entry.pal_num == 0){
							color = palette0[entry.index];
						}
						else{
							color = palette1[entry.index];
						}
					}
					else{
						color = frame[x+offset1];
					}
					
					fill_count  = AGMV_GetFillBlockCount(agmv,x,y,color);
					copy_count  = AGMV_GetCopyBlockCount(agmv,x,y);
					pcopy_count = AGMV_GetPCopyBlockCount(agmv,x,y);

					pmv_count.b4x4 = 0; mv_count.b4x4 = 0;
					pmv_count.b8x4 = 0; mv_count.b8x4 = 0;
					pmv_count.b2x4 = 0; mv_count.b2x4 = 0;
					pmv_count.b8x8 = 0; mv_count.b8x8 = 0;
					
					icopy_count.b4x4 = 0; icopy_count.copyx1 = 0; icopy_count.copyy1 = 0;
					icopy_count.b2x4 = 0; icopy_count.copyx2 = 0; icopy_count.copyy2 = 0;
					icopy_count.b8x4 = 0; icopy_count.copyx3 = 0; icopy_count.copyy3 = 0;
					icopy_count.b8x8 = 0; icopy_count.copyx4 = 0; icopy_count.copyy4 = 0;
					
					icopyr_count.b4x4 = 0; icopyr_count.copyx1 = 0; icopyr_count.copyy1 = 0;
					icopyr_count.b2x4 = 0; icopyr_count.copyx2 = 0; icopyr_count.copyy2 = 0;
					icopyr_count.b8x4 = 0; icopyr_count.copyx3 = 0; icopyr_count.copyy3 = 0;
					icopyr_count.b8x8 = 0; icopyr_count.copyx4 = 0; icopyr_count.copyy4 = 0;
					
					vq_count.c4x4.num = 0; vq_count.c8x4.num = 0; vq_count.c2x4.num = 0; vq_count.c8x8.num = 0;
					
					sblock_count.sector1 = 0;    sblock_count.sector2 = 0; 	  sblock_count.sector3 = 0;    sblock_count.sector4 = 0; 
					sblock_count4x4.sector1 = 0; sblock_count4x4.sector2 = 0; sblock_count4x4.sector3 = 0; sblock_count4x4.sector4 = 0; 
					
					r1 = AGMV_GetR(color);
					g1 = AGMV_GetG(color);
					b1 = AGMV_GetB(color);
					
					r2 = AGMV_GetR(agmv->prev_fill_color);
					g2 = AGMV_GetG(agmv->prev_fill_color);
					b2 = AGMV_GetB(agmv->prev_fill_color);
					
					diffr = AGMV_Abs(r1-r2);
					diffg = AGMV_Abs(g1-g2);
					diffb = AGMV_Abs(b1-b2);
					
					if(copy_count.b2x4 < AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_COPY_FLAG,quality,opt) && pcopy_count.b2x4 < AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_PCOPY_FLAG,quality,opt)){
						
						AGMV_ThreeStepSearch(agmv,x,y);
						
						pmv_count   = AGMV_GetPMVBlockCount(agmv,x,y);
						mv_count    = AGMV_GetMVBlockCount(agmv,x,y);
			
						if(fill_count.b2x4 < fill2x4 && fill_count.b4x4 < fill4x4 && icopy_count.b2x4 < AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_ICOPY_FLAG,quality,opt) && 
						mv_count.b2x4 < AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_MV_FLAG,quality,opt) && pmv_count.b2x4 < AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_PMV_FLAG,quality,opt)){
							
							icopy_count = AGMV_GetIntraCopyBlockCount(agmv,x,y);
							icopyr_count = AGMV_GetRightIntraCopyBlockCount(agmv,x,y);
							vq_count = AGMV_GetVQBlockCount(agmv,x,y);
							
							if(icopy_count.b2x4 < AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_ICOPY_FLAG,quality,opt) && icopyr_count.b2x4 < AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_ICOPY_FLAG,quality,opt)){
								AGMV_ThreeStepSearch2x2(agmv,&sector.quad[0],&sector.quad[1],&sector.quad[2],&sector.quad[3],x,y);

								if(pframe_count >= 1){
									sblock_count.sector1 = AGMV_GetPMVSBlockCount(agmv,sector.quad[0].pmv,x,y,2,2);
									sblock_count.sector2 = AGMV_GetPMVSBlockCount(agmv,sector.quad[1].pmv,x+2,y,2,2);
									sblock_count.sector3 = AGMV_GetPMVSBlockCount(agmv,sector.quad[2].pmv,x,y+2,2,2);
									sblock_count.sector4 = AGMV_GetPMVSBlockCount(agmv,sector.quad[3].pmv,x+2,y+2,2,2);
								}
								else{
									sblock_count.sector1 = AGMV_GetMVSBlockCount(agmv,sector.quad[0].mv,x,y,2,2);
									sblock_count.sector2 = AGMV_GetMVSBlockCount(agmv,sector.quad[1].mv,x+2,y,2,2);
									sblock_count.sector3 = AGMV_GetMVSBlockCount(agmv,sector.quad[2].mv,x,y+2,2,2);
									sblock_count.sector4 = AGMV_GetMVSBlockCount(agmv,sector.quad[3].mv,x+2,y+2,2,2);
								}
								
								if(sblock_count.sector1 < 4 && sblock_count.sector2 < 3 && sblock_count.sector3 < 4 && sblock_count.sector4 < 3){
								
									AGMV_ThreeStepSearch4x4(agmv,&sector4x4.quad[0],&sector4x4.quad[1],x,y);
									
									if(pframe_count >= 1){
										sblock_count4x4.sector1 = AGMV_GetPMVSBlockCount(agmv,sector4x4.quad[0].pmv,x,y,4,4);
										sblock_count4x4.sector2 = AGMV_GetPMVSBlockCount(agmv,sector4x4.quad[1].pmv,x+4,y,4,4);
									}
									else{
										sblock_count4x4.sector1 = AGMV_GetMVSBlockCount(agmv,sector4x4.quad[0].mv,x,y,4,4);
										sblock_count4x4.sector2 = AGMV_GetMVSBlockCount(agmv,sector4x4.quad[1].mv,x+4,y,4,4);
									}
								}	
							}							
						}
					}
					
					if((pcopy_count.b4x4 >= pcopy4x4 || pcopy_count.b8x4 >= pcopy8x4 || pcopy_count.b2x4 >= pcopy2x4 || pcopy_count.b8x8 >= pcopy8x8) && pframe_count >= 1){
						
						if(pcopy_count.b8x8 >= pcopy8x8){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_PCOPY_FLAG);
							x += 4;
						}
						else if(pcopy_count.b8x4 >= pcopy8x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_PCOPY_FLAG);
							x += 4;
						}
						else if(pcopy_count.b4x4 >= pcopy4x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_PCOPY_FLAG);
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_PCOPY_FLAG);
							x -= 2;
						}
						
						flagc++; pcopyc++;
					}
					else if(copy_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_COPY_FLAG,quality,opt) || copy_count.b8x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_COPY_FLAG,quality,opt)
					|| copy_count.b2x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_COPY_FLAG,quality,opt) || copy_count.b8x8 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_COPY_FLAG,quality,opt)){
						
						if(copy_count.b8x8 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_COPY_FLAG,quality,opt)){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_COPY_FLAG);
							x += 4;
						}						
						else if(copy_count.b8x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_COPY_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_COPY_FLAG);
							x += 4;
						}
						else if(copy_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_COPY_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_COPY_FLAG);
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_COPY_FLAG);
							x -= 2;
						}
						
						flagc++; copyc++;
					}
					else if((diffr <= max_diff && diffg <= max_diff && diffb <= max_diff) && (fill_count.b4x4 >= fill4x4 || fill_count.b8x4 >= fill8x4 || fill_count.b2x4 >= fill2x4 || fill_count.b8x8 >= fill8x8)){
						if(fill_count.b8x8 >= fill8x8){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_PFILL_FLAG);
							x += 4;
						}
						else if(fill_count.b8x4 >= fill8x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_PFILL_FLAG);
							x += 4;
						}
						else if(fill_count.b4x4 >= fill4x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_PFILL_FLAG);
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_PFILL_FLAG);
							x -= 2;
						}
						
						flagc++; pfillc++;
					}
					else if((pmv_count.b4x4 >= pmv4x4 || pmv_count.b8x4 >= pmv8x4 || pmv_count.b2x4 >= pmv2x4 || pmv_count.b8x8 >= pmv8x8) && pframe_count >= 1){
						
						if(pmv_count.b8x8 >= pmv8x8){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_PMV_FLAG);
							
							u1 = agmv->pmv8x8.x + 8;
							u2 = agmv->pmv8x8.y + 8;
							
							data[bitpos++] =  (u1 << 4) | u2;
		
							x += 4;
						}
						else if(pmv_count.b8x4 >= pmv8x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_PMV_FLAG);
							
							u1 = agmv->pmv8x4.x + 8;
							u2 = agmv->pmv8x4.y + 8;
							
							data[bitpos++] =  (u1 << 4) | u2;
							
							x += 4;
						}
						else if(pmv_count.b4x4 >= pmv4x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_PMV_FLAG);
							
							u1 = agmv->pmv.x + 8;
							u2 = agmv->pmv.y + 8;
							
							data[bitpos++] =  (u1 << 4) | u2;
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_PMV_FLAG);
							
							u1 = agmv->pmv2x4.x + 8;
							u2 = agmv->pmv2x4.y + 8;
							
							data[bitpos++] =  (u1 << 4) | u2;
							
							x -= 2;
						}
						
						flagc++; pmvc++;
					}
					else if(mv_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_MV_FLAG,quality,opt) || mv_count.b8x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_MV_FLAG,quality,opt)
					|| mv_count.b2x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_MV_FLAG,quality,opt) || mv_count.b8x8 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_MV_FLAG,quality,opt)){
						
						if(mv_count.b8x8 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_MV_FLAG,quality,opt)){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_MV_FLAG);
							
							u1 = agmv->mv8x8.x + 8;
							u2 = agmv->mv8x8.y + 8;
							
							data[bitpos++] =  (u1 << 4) | u2;
							
							x += 4;
						}
						else if(mv_count.b8x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_MV_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_MV_FLAG);
							
							u1 = agmv->mv8x4.x + 8;
							u2 = agmv->mv8x4.y + 8;
							
							data[bitpos++] =  (u1 << 4) | u2;
							
							x += 4;
						}
						else if(mv_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_MV_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_MV_FLAG);
							
							u1 = agmv->mv.x + 8;
							u2 = agmv->mv.y + 8;
							
							data[bitpos++] =  (u1 << 4) | u2;
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_MV_FLAG);
							
							u1 = agmv->mv2x4.x + 8;
							u2 = agmv->mv2x4.y + 8;
							
							data[bitpos++] =  (u1 << 4) | u2;
							
							x -= 2;
						}
						
						flagc++; mvc++;
					}
					else if(fill_count.b4x4 >= fill4x4 || fill_count.b8x4 >= fill8x4 || fill_count.b2x4 >= fill2x4 || fill_count.b8x8 >= fill8x8){
						if(fill_count.b8x8 >= fill8x8){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_FILL_FLAG);
							x += 4;
						}
						else if(fill_count.b8x4 >= fill8x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_FILL_FLAG);
							x += 4;
						}
						else if(fill_count.b4x4 >= fill4x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_FILL_FLAG);
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_FILL_FLAG);
							x -= 2;
						}
						
						if(opt != AGMV_OPT_IV && opt != AGMV_OPT_GBA_IV && opt != AGMV_OPT_PSX_IV && opt != AGMV_OPT_NDS_IV){
							if(entry.index < 127){
								data[bitpos++] = entry.pal_num << 7 | entry.index;
							}
							else{
								data[bitpos++] = entry.pal_num << 7 | 127;
								data[bitpos++] = entry.index;
							}
						}
						else{
							color16 = AGMV_ToRGB16(color);
							
							data[bitpos++] = AGMV_GetMSB(color16);
							data[bitpos++] = AGMV_GetLSB(color16);
						}
						
						flagc++; fillc++;
						
						agmv->prev_fill_color = color;
					}						
			
					else if(icopy_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_ICOPY_FLAG,quality,opt) || icopy_count.b2x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_ICOPY_FLAG,quality,opt)){
						if(icopy_count.b8x8 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_ICOPY_FLAG,quality,opt)){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_ICOPY_FLAG);
							data[bitpos++] = (x - icopy_count.copyx4) << 4 | (y - icopy_count.copyy4);
							
							x += 4;
						}
						else if(icopy_count.b8x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_ICOPY_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_ICOPY_FLAG);
							data[bitpos++] = (x - icopy_count.copyx2) << 4 | (y - icopy_count.copyy2);
							
							x += 4;
						}
						else if(icopy_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_ICOPY_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_ICOPY_FLAG);
							data[bitpos++] = (x - icopy_count.copyx1) << 4 | (y - icopy_count.copyy1);
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_ICOPY_FLAG);
							data[bitpos++] = (x - icopy_count.copyx3) << 4 | (y - icopy_count.copyy3);
							
							x -= 2;
						}
						
						flagc++; icopyc++;
					}		
					else if(icopyr_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_ICOPY_FLAG,quality,opt) || icopyr_count.b2x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_ICOPY_FLAG,quality,opt)){
						if(icopyr_count.b8x8 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_ICOPY_FLAG,quality,opt)){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_ICOPYR_FLAG);
							data[bitpos++] = (icopyr_count.copyx4 - x) << 4 | (y - icopyr_count.copyy4);

							x += 4;
						}
						else if(icopyr_count.b8x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_ICOPY_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_ICOPYR_FLAG);
							data[bitpos++] = (icopyr_count.copyx2 - x) << 4 | (y - icopyr_count.copyy2);

							x += 4;
						}
						else if(icopyr_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_ICOPY_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_ICOPYR_FLAG);
							data[bitpos++] = (icopyr_count.copyx1 - x) << 4 | (y - icopyr_count.copyy1);
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_ICOPYR_FLAG);
							data[bitpos++] = (icopyr_count.copyx3 - x) << 4 | (y - icopyr_count.copyy3);
							
							x -= 2;
						}
						
						flagc++; icopyc++;
					}
					else if((sblock_count.sector1 >= 4 && sblock_count.sector3 >= 3) || (sblock_count4x4.sector1 >= 14 && sblock_count4x4.sector2 >= 14)){

						if(sblock_count4x4.sector1 >= 14 && sblock_count4x4.sector2 >= 14){
							
							if(pframe_count >= 1){
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_SMV_FLAG);
								
								for(i = 0; i < 2; i++){
									data[bitpos++] = sector4x4.quad[i].pmv.x;
									data[bitpos++] = sector4x4.quad[i].pmv.y;
								}
							}
							else{
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_SMV_FLAG);
								
								for(i = 0; i < 2; i++){
									data[bitpos++] = sector4x4.quad[i].mv.x;
									data[bitpos++] = sector4x4.quad[i].mv.y;
								}
							}
							
							x += 4;
						}
						else if(sblock_count.sector1 >= 4 && sblock_count.sector2 >= 3 && sblock_count.sector3 >= 4 && sblock_count.sector4 >= 3){
							
							if(pframe_count >= 1){
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_SMV_FLAG);
								
								for(i = 0; i < 4; i++){
									u1 = sector.quad[i].pmv.x + 8;
									u2 = sector.quad[i].pmv.y + 8;
								
									data[bitpos++] =  (u1 << 4) | u2;
								}
							}
							else{
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_SMV_FLAG);
								
								for(i = 0; i < 4; i++){
									u1 = sector.quad[i].mv.x + 8;
									u2 = sector.quad[i].mv.y + 8;
								
									data[bitpos++] =  (u1 << 4) | u2;
								}
							}
						}
						else{
							if(pframe_count >= 1){
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_SMV_FLAG);

								u1 = sector.quad[0].pmv.x + 8;
								u2 = sector.quad[0].pmv.y + 8;
							
								data[bitpos++] =  (u1 << 4) | u2;
								
								u1 = sector.quad[2].pmv.x + 8;
								u2 = sector.quad[2].pmv.y + 8;
							
								data[bitpos++] =  (u1 << 4) | u2;
							}
							else{
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_SMV_FLAG);
								
								u1 = sector.quad[0].mv.x + 8;
								u2 = sector.quad[0].mv.y + 8;
							
								data[bitpos++] =  (u1 << 4) | u2;
								
								u1 = sector.quad[2].mv.x + 8;
								u2 = sector.quad[2].mv.y + 8;
							
								data[bitpos++] =  (u1 << 4) | u2;
							}
							
							x -= 2;
						}
						
						flagc++; smvc++;
					}
					else if(vq_count.c2x4.num > 0 && vq_count.c2x4.num <= vq22x4){
						u32* frame_ptr, color1, color2, dist1, dist2;
						u16 color1_16, color2_16;
						u32 block_w, block_h;
						AGMV_ENTRY index1, index2;
						int r, g, b, r1, g1, b1, r2, g2, b2, rdiff1, gdiff1, bdiff1, rdiff2, gdiff2, bdiff2;
						u8 count = 0, byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8;
						Bool col1, col2;
						u8 indices[64];
						
						col1 = FALSE;
						col2 = FALSE;

						if(vq_count.c8x8.num > 0 && vq_count.c8x8.num <= vq28x8){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							
							color1 = vq_count.c8x8.coffre[vq_count.c8x8.num-1].color;
							color2 = vq_count.c8x8.coffre[vq_count.c8x8.num-2].color;
							
							r1 = AGMV_GetR(color1);
							g1 = AGMV_GetG(color1);
							b1 = AGMV_GetB(color1);
							
							r2 = AGMV_GetR(agmv->vq2color1);
							g2 = AGMV_GetG(agmv->vq2color1);
							b2 = AGMV_GetB(agmv->vq2color1);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col1 = TRUE;
							}
							
							r1 = AGMV_GetR(color2);
							g1 = AGMV_GetG(color2);
							b1 = AGMV_GetB(color2);
							
							r2 = AGMV_GetR(agmv->vq2color2);
							g2 = AGMV_GetG(agmv->vq2color2);
							b2 = AGMV_GetB(agmv->vq2color2);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col2 = TRUE;
							}
							
							if(col1 == TRUE && col2 == TRUE){
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_PVQ2_FLAG);
							}
							else{
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_VQ2_FLAG);
							}
							
							block_w = 8; block_h = 8;
						}
						else if(vq_count.c8x4.num > 0 && vq_count.c8x4.num <= vq28x4){	
							color1 = vq_count.c8x4.coffre[vq_count.c8x4.num-1].color;
							color2 = vq_count.c8x4.coffre[vq_count.c8x4.num-2].color;
							
							r1 = AGMV_GetR(color1);
							g1 = AGMV_GetG(color1);
							b1 = AGMV_GetB(color1);
							
							r2 = AGMV_GetR(agmv->vq2color1);
							g2 = AGMV_GetG(agmv->vq2color1);
							b2 = AGMV_GetB(agmv->vq2color1);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col1 = TRUE;
							}
							
							r1 = AGMV_GetR(color2);
							g1 = AGMV_GetG(color2);
							b1 = AGMV_GetB(color2);
							
							r2 = AGMV_GetR(agmv->vq2color2);
							g2 = AGMV_GetG(agmv->vq2color2);
							b2 = AGMV_GetB(agmv->vq2color2);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col2 = TRUE;
							}
							
							if(col1 == TRUE && col2 == TRUE){
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_PVQ2_FLAG);
							}
							else{
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_VQ2_FLAG);
							}
							
							block_w = 8; block_h = 4;
						}
						else if(vq_count.c4x4.num > 0 && vq_count.c4x4.num <= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_VQ2_FLAG,quality,opt)){
							color1 = vq_count.c4x4.coffre[vq_count.c4x4.num-1].color;
							color2 = vq_count.c4x4.coffre[vq_count.c4x4.num-2].color;
							
							r1 = AGMV_GetR(color1);
							g1 = AGMV_GetG(color1);
							b1 = AGMV_GetB(color1);
							
							r2 = AGMV_GetR(agmv->vq2color1);
							g2 = AGMV_GetG(agmv->vq2color1);
							b2 = AGMV_GetB(agmv->vq2color1);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col1 = TRUE;
							}
							
							r1 = AGMV_GetR(color2);
							g1 = AGMV_GetG(color2);
							b1 = AGMV_GetB(color2);
							
							r2 = AGMV_GetR(agmv->vq2color2);
							g2 = AGMV_GetG(agmv->vq2color2);
							b2 = AGMV_GetB(agmv->vq2color2);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col2 = TRUE;
							}
							
							if(col1 == TRUE && col2 == TRUE){
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_PVQ2_FLAG);
							}
							else{
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_VQ2_FLAG);
							}
							
							block_w = 4; block_h = 4;
						}
						else{
							color1 = vq_count.c2x4.coffre[vq_count.c2x4.num-1].color;
							color2 = vq_count.c2x4.coffre[vq_count.c2x4.num-2].color;
						
							r1 = AGMV_GetR(color1);
							g1 = AGMV_GetG(color1);
							b1 = AGMV_GetB(color1);
							
							r2 = AGMV_GetR(agmv->vq2color1);
							g2 = AGMV_GetG(agmv->vq2color1);
							b2 = AGMV_GetB(agmv->vq2color1);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col1 = TRUE;
							}
							
							r1 = AGMV_GetR(color2);
							g1 = AGMV_GetG(color2);
							b1 = AGMV_GetB(color2);
							
							r2 = AGMV_GetR(agmv->vq2color2);
							g2 = AGMV_GetG(agmv->vq2color2);
							b2 = AGMV_GetB(agmv->vq2color2);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col2 = TRUE;
							}
							
							if(col1 == TRUE && col2 == TRUE){
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_PVQ2_FLAG);
							}
							else{
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_VQ2_FLAG);
							}
							
							block_w = 2; block_h = 4;
						}
						
						agmv->vq2color1 = color1;
						agmv->vq2color2 = color2;
						
						frame_ptr = &frame[x+y*width];
						
						index1 = AGMV_FindNearestEntry(palette0,palette1,color1);
						index2 = AGMV_FindNearestEntry(palette0,palette1,color2);
						
						if(col1 == FALSE || col2 == FALSE){
							if(opt != AGMV_OPT_IV && opt != AGMV_OPT_GBA_IV && opt != AGMV_OPT_PSX_IV && opt != AGMV_OPT_NDS_IV){						
								if(index1.index < 127){
									data[bitpos++] = index1.pal_num << 7 | index1.index;
								}
								else{
									data[bitpos++] = index1.pal_num << 7 | 127;
									data[bitpos++] = index1.index;
								}
								
								if(index2.index < 127){
									data[bitpos++] = index2.pal_num << 7 | index2.index;
								}
								else{
									data[bitpos++] = index2.pal_num << 7 | 127;
									data[bitpos++] = index2.index;
								}
							}
							else{
								color1_16 = AGMV_ToRGB16(color1);
								color2_16 = AGMV_ToRGB16(color2);
								
								data[bitpos++] = AGMV_GetMSB(color1_16);
								data[bitpos++] = AGMV_GetLSB(color1_16);
								
								data[bitpos++] = AGMV_GetMSB(color2_16);
								data[bitpos++] = AGMV_GetLSB(color2_16);
							}
						}
						
						r1 = AGMV_GetR(color1);
						g1 = AGMV_GetG(color1);
						b1 = AGMV_GetB(color1);
						
						r2 = AGMV_GetR(color2);
						g2 = AGMV_GetG(color2);
						b2 = AGMV_GetB(color2);
						
						for(j = 0; j < block_h; j++){
							for(i = 0; i < block_w; i++){
								color = frame_ptr[i];
								
								r = AGMV_GetR(color);
								g = AGMV_GetG(color);
								b = AGMV_GetB(color);

								rdiff1 = r1-r;
								gdiff1 = g1-g;
								bdiff1 = b1-b;
								
								rdiff2 = r2-r;
								gdiff2 = g2-g;
								bdiff2 = b2-b;
								
								dist1 = rdiff1*rdiff1 + gdiff1*gdiff1 + bdiff1*bdiff1;
								dist2 = rdiff2*rdiff2 + gdiff2*gdiff2 + bdiff2*bdiff2;
								
								if(dist1 < dist2){
									indices[count++] = 0;
								}
								else{
									indices[count++] = 1;
								}
							}
							
							frame_ptr += width;
						}
						
						if(block_w == 8 && block_h == 8){
							byte1 = indices[0] << 7 | indices[1] << 6 | indices[2] << 5 | indices[3] << 4 | indices[4] << 3 | indices[5] << 2 | indices[6] << 1 | indices[7];
							byte2 = indices[8] << 7 | indices[9] << 6 | indices[10] << 5 | indices[11] << 4 | indices[12] << 3 | indices[13] << 2 | indices[14] << 1 | indices[15];
							byte3 = indices[16] << 7 | indices[17] << 6 | indices[18] << 5 | indices[19] << 4 | indices[20] << 3 | indices[21] << 2 | indices[22] << 1 | indices[23];
							byte4 = indices[24] << 7 | indices[25] << 6 | indices[26] << 5 | indices[27] << 4 | indices[28] << 3 | indices[29] << 2 | indices[30] << 1 | indices[31];
							byte5 = indices[32] << 7 | indices[33] << 6 | indices[34] << 5 | indices[35] << 4 | indices[36] << 3 | indices[37] << 2 | indices[38] << 1 | indices[39];
							byte6 = indices[40] << 7 | indices[41] << 6 | indices[42] << 5 | indices[43] << 4 | indices[44] << 3 | indices[45] << 2 | indices[46] << 1 | indices[47];
							byte7 = indices[48] << 7 | indices[49] << 6 | indices[50] << 5 | indices[51] << 4 | indices[52] << 3 | indices[53] << 2 | indices[54] << 1 | indices[55];
							byte8 = indices[56] << 7 | indices[57] << 6 | indices[58] << 5 | indices[59] << 4 | indices[60] << 3 | indices[61] << 2 | indices[62] << 1 | indices[63];

							
							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
							data[bitpos++] = byte3;
							data[bitpos++] = byte4;
							data[bitpos++] = byte5;
							data[bitpos++] = byte6;
							data[bitpos++] = byte7;
							data[bitpos++] = byte8;
							
							x += 4;
						}
						else if(block_w == 8){
							byte1 = indices[0] << 7 | indices[1] << 6 | indices[2] << 5 | indices[3] << 4 | indices[4] << 3 | indices[5] << 2 | indices[6] << 1 | indices[7];
							byte2 = indices[8] << 7 | indices[9] << 6 | indices[10] << 5 | indices[11] << 4 | indices[12] << 3 | indices[13] << 2 | indices[14] << 1 | indices[15];
							byte3 = indices[16] << 7 | indices[17] << 6 | indices[18] << 5 | indices[19] << 4 | indices[20] << 3 | indices[21] << 2 | indices[22] << 1 | indices[23];
							byte4 = indices[24] << 7 | indices[25] << 6 | indices[26] << 5 | indices[27] << 4 | indices[28] << 3 | indices[29] << 2 | indices[30] << 1 | indices[31];
						
							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
							data[bitpos++] = byte3;
							data[bitpos++] = byte4;
							
							x += 4;
						}
						else if(block_w == 4){
							byte1 = indices[0] << 7 | indices[1] << 6 | indices[2] << 5 | indices[3] << 4 | indices[4] << 3 | indices[5] << 2 | indices[6] << 1 | indices[7];
							byte2 = indices[8] << 7 | indices[9] << 6 | indices[10] << 5 | indices[11] << 4 | indices[12] << 3 | indices[13] << 2 | indices[14] << 1 | indices[15];
						
							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
						}
						else{
							byte1 = indices[0] << 7 | indices[1] << 6 | indices[2] << 5 | indices[3] << 4 | indices[4] << 3 | indices[5] << 2 | indices[6] << 1 | indices[7];					
							data[bitpos++] = byte1;

							x -= 2;
						}
						
						flagc++; vq2c++;
					}
					else if(vq_count.c4x4.num > 0 && vq_count.c4x4.num <= vq44x4){
						u32* frame_ptr, color1, color2, color3, color4, dist1, dist2, dist3, dist4, min, block_w, block_h;
						u16 color1_16, color2_16, color3_16, color4_16;
						AGMV_ENTRY index1, index2, index3, index4;
						int r, g, b, r1, g1, b1, r2, g2, b2, r3, g3, b3, r4, g4, b4, rdiff1, gdiff1, bdiff1, rdiff2, gdiff2, bdiff2;
						int rdiff3, gdiff3, bdiff3, rdiff4, gdiff4, bdiff4;
						u8 count = 0, byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, byte9, byte10, byte11, byte12, byte13, byte14, byte15, byte16;
						u8 indices[64];
						
						flagc++; vq4c++;
						
						if(vq_count.c8x8.num > 0 && vq_count.c8x8.num <= vq48x8){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_VQ4_FLAG);
						
							color1 = vq_count.c8x8.coffre[vq_count.c8x8.num-1].color;
							color2 = vq_count.c8x8.coffre[vq_count.c8x8.num-2].color;
							color3 = vq_count.c8x8.coffre[vq_count.c8x8.num-3].color;
							color4 = vq_count.c8x8.coffre[vq_count.c8x8.num-4].color;

							block_w = 8; block_h = 8;
						}
						else if(vq_count.c8x4.num > 0 && vq_count.c8x4.num <= vq48x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_VQ4_FLAG);
						
							color1 = vq_count.c8x4.coffre[vq_count.c8x4.num-1].color;
							color2 = vq_count.c8x4.coffre[vq_count.c8x4.num-2].color;
							color3 = vq_count.c8x4.coffre[vq_count.c8x4.num-3].color;
							color4 = vq_count.c8x4.coffre[vq_count.c8x4.num-4].color;

							block_w = 8; block_h = 4;
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_VQ4_FLAG);
							
							color1 = vq_count.c4x4.coffre[vq_count.c4x4.num-1].color;
							color2 = vq_count.c4x4.coffre[vq_count.c4x4.num-2].color;
							color3 = vq_count.c4x4.coffre[vq_count.c4x4.num-3].color;
							color4 = vq_count.c4x4.coffre[vq_count.c4x4.num-4].color;
							
							block_w = 4; block_h = 4;
						}

						index1 = AGMV_FindNearestEntry(palette0,palette1,color1);
						index2 = AGMV_FindNearestEntry(palette0,palette1,color2);
						index3 = AGMV_FindNearestEntry(palette0,palette1,color3);
						index4 = AGMV_FindNearestEntry(palette0,palette1,color4);
						
						if(opt != AGMV_OPT_IV && opt != AGMV_OPT_GBA_IV && opt != AGMV_OPT_PSX_IV && opt != AGMV_OPT_NDS_IV){
							if(index1.index < 127){
								data[bitpos++] = index1.pal_num << 7 | index1.index;
							}
							else{
								data[bitpos++] = index1.pal_num << 7 | 127;
								data[bitpos++] = index1.index;
							}
							
							if(index2.index < 127){
								data[bitpos++] = index2.pal_num << 7 | index2.index;
							}
							else{
								data[bitpos++] = index2.pal_num << 7 | 127;
								data[bitpos++] = index2.index;
							}
							
							if(index3.index < 127){
								data[bitpos++] = index3.pal_num << 7 | index3.index;
							}
							else{
								data[bitpos++] = index3.pal_num << 7 | 127;
								data[bitpos++] = index3.index;
							}
							
							if(index4.index < 127){
								data[bitpos++] = index4.pal_num << 7 | index4.index;
							}
							else{
								data[bitpos++] = index4.pal_num << 7 | 127;
								data[bitpos++] = index4.index;
							}
						}
						else{
							color1_16 = AGMV_ToRGB16(color1);
							color2_16 = AGMV_ToRGB16(color2);
							color3_16 = AGMV_ToRGB16(color3);
							color4_16 = AGMV_ToRGB16(color4);
							
							data[bitpos++] = AGMV_GetMSB(color1_16);
							data[bitpos++] = AGMV_GetLSB(color1_16);
							
							data[bitpos++] = AGMV_GetMSB(color2_16);
							data[bitpos++] = AGMV_GetLSB(color2_16);
							
							data[bitpos++] = AGMV_GetMSB(color3_16);
							data[bitpos++] = AGMV_GetLSB(color3_16);
							
							data[bitpos++] = AGMV_GetMSB(color4_16);
							data[bitpos++] = AGMV_GetLSB(color4_16);
						}
						
						frame_ptr = &frame[x+y*width];
							
						r1 = AGMV_GetR(color1);
						g1 = AGMV_GetG(color1);
						b1 = AGMV_GetB(color1);
						
						r2 = AGMV_GetR(color2);
						g2 = AGMV_GetG(color2);
						b2 = AGMV_GetB(color2);
						
						r3 = AGMV_GetR(color3);
						g3 = AGMV_GetG(color3);
						b3 = AGMV_GetB(color3);
						
						r4 = AGMV_GetR(color4);
						g4 = AGMV_GetG(color4);
						b4 = AGMV_GetB(color4);
						
						for(j = 0; j < block_h; j++){
							for(i = 0; i < block_w; i++){
								color = frame_ptr[i];
								
								r = AGMV_GetR(color);
								g = AGMV_GetG(color);
								b = AGMV_GetB(color);
								
								rdiff1 = r1-r;
								gdiff1 = g1-g;
								bdiff1 = b1-b;
								
								rdiff2 = r2-r;
								gdiff2 = g2-g;
								bdiff2 = b2-b;
								
								rdiff3 = r3-r;
								gdiff3 = g3-g;
								bdiff3 = b3-b;
								
								rdiff4 = r4-r;
								gdiff4 = g4-g;
								bdiff4 = b4-b;
								
								dist1 = rdiff1*rdiff1 + gdiff1*gdiff1 + bdiff1*bdiff1;
								dist2 = rdiff2*rdiff2 + gdiff2*gdiff2 + bdiff2*bdiff2;
								dist3 = rdiff3*rdiff3 + gdiff3*gdiff3 + bdiff3*bdiff3;
								dist4 = rdiff4*rdiff4 + gdiff4*gdiff4 + bdiff4*bdiff4;
								
								min = AGMV_Min(dist1,dist2);
								min = AGMV_Min(dist3,min);
								min = AGMV_Min(dist4,min);
								
								if(min == dist1){
									indices[count++] = 0;
								}
								else if(min == dist2){
									indices[count++] = 1;
								}
								else if(min == dist3){
									indices[count++] = 2;
								}
								else{
									indices[count++] = 3;
								}
							}
							
							frame_ptr += width;
						}
						
						if(block_w == 4){
							byte1 = indices[0] << 6 | indices[1] << 4 | indices[2] << 2 | indices[3];
							byte2 = indices[4] << 6 | indices[5] << 4 | indices[6] << 2 | indices[7];
							byte3 = indices[8] << 6 | indices[9] << 4 | indices[10] << 2 | indices[11];
							byte4 = indices[12] << 6 | indices[13] << 4 | indices[14] << 2 | indices[15];

							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
							data[bitpos++] = byte3;
							data[bitpos++] = byte4;
						}
						else if(block_w == 8 && block_h == 4){
							byte1 = indices[0] << 6 | indices[1] << 4 | indices[2] << 2 | indices[3];
							byte2 = indices[4] << 6 | indices[5] << 4 | indices[6] << 2 | indices[7];
							byte3 = indices[8] << 6 | indices[9] << 4 | indices[10] << 2 | indices[11];
							byte4 = indices[12] << 6 | indices[13] << 4 | indices[14] << 2 | indices[15];
							byte5 = indices[16] << 6 | indices[17] << 4 | indices[18] << 2 | indices[19];
							byte6 = indices[20] << 6 | indices[21] << 4 | indices[22] << 2 | indices[23];
							byte7 = indices[24] << 6 | indices[25] << 4 | indices[26] << 2 | indices[27];
							byte8 = indices[28] << 6 | indices[29] << 4 | indices[30] << 2 | indices[31];
							
							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
							data[bitpos++] = byte3;
							data[bitpos++] = byte4;
							data[bitpos++] = byte5;
							data[bitpos++] = byte6;
							data[bitpos++] = byte7;
							data[bitpos++] = byte8;
							
							x += 4;
						}
						else{
							byte1 = indices[0] << 6 | indices[1] << 4 | indices[2] << 2 | indices[3];
							byte2 = indices[4] << 6 | indices[5] << 4 | indices[6] << 2 | indices[7];
							byte3 = indices[8] << 6 | indices[9] << 4 | indices[10] << 2 | indices[11];
							byte4 = indices[12] << 6 | indices[13] << 4 | indices[14] << 2 | indices[15];
							byte5 = indices[16] << 6 | indices[17] << 4 | indices[18] << 2 | indices[19];
							byte6 = indices[20] << 6 | indices[21] << 4 | indices[22] << 2 | indices[23];
							byte7 = indices[24] << 6 | indices[25] << 4 | indices[26] << 2 | indices[27];
							byte8 = indices[28] << 6 | indices[29] << 4 | indices[30] << 2 | indices[31];
							byte9 = indices[32] << 6 | indices[33] << 4 | indices[34] << 2 | indices[35];
							byte10 = indices[36] << 6 | indices[37] << 4 | indices[38] << 2 | indices[39];
							byte11 = indices[40] << 6 | indices[41] << 4 | indices[42] << 2 | indices[43];
							byte12 = indices[44] << 6 | indices[45] << 4 | indices[46] << 2 | indices[47];
							byte13 = indices[48] << 6 | indices[49] << 4 | indices[50] << 2 | indices[51];
							byte14 = indices[52] << 6 | indices[53] << 4 | indices[54] << 2 | indices[55];
							byte15 = indices[56] << 6 | indices[57] << 4 | indices[58] << 2 | indices[59];
							byte16 = indices[60] << 6 | indices[61] << 4 | indices[62] << 2 | indices[63];
							
							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
							data[bitpos++] = byte3;
							data[bitpos++] = byte4;
							data[bitpos++] = byte5;
							data[bitpos++] = byte6;
							data[bitpos++] = byte7;
							data[bitpos++] = byte8;
							data[bitpos++] = byte9;
							data[bitpos++] = byte10;
							data[bitpos++] = byte11;
							data[bitpos++] = byte12;
							data[bitpos++] = byte13;
							data[bitpos++] = byte14;
							data[bitpos++] = byte15;
							data[bitpos++] = byte16;
							
							x += 4;
						}
					}
					else{
						flagc++; nc++; 
						
						data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_NORMAL_FLAG);
						
						AGMV_ENTRY norm_arr[5], *img_entryp;
						u32 color_arr[5], *frame_ptr;
						
						img_entryp = &img_entry[x+y*width];
						frame_ptr  = &frame[x+y*width];
						
						if(opt != AGMV_OPT_IV && opt != AGMV_OPT_GBA_IV && opt != AGMV_OPT_PSX_IV && opt != AGMV_OPT_NDS_IV){
							norm_arr[0] = img_entryp[0];
							norm_arr[1] = img_entryp[1];
							
							img_entryp += width;
							img_entryp += width;
							
							norm_arr[2] = img_entryp[0];
							norm_arr[3] = img_entryp[1];
							
							img_entryp += width;
							
							norm_arr[4] = img_entryp[0];
							
							for(i = 0; i < 5; i++){
								if(norm_arr[i].index < 127){
									data[bitpos++] = norm_arr[i].pal_num << 7 | norm_arr[i].index;
								}
								else{
									data[bitpos++] = norm_arr[i].pal_num << 7 | 127;
									data[bitpos++] = norm_arr[i].index;
								}
							}
						}
						else{
							color_arr[0] = frame_ptr[0];
							color_arr[1] = frame_ptr[1];
							
							frame_ptr += width;
							frame_ptr += width;
							
							color_arr[2] = frame_ptr[0];
							color_arr[3] = frame_ptr[1];
							
							frame_ptr += width;
							
							color_arr[4] = frame_ptr[0];
							
							for(i = 0; i < 5; i++){
								color16 = AGMV_ToRGB16(color_arr[i]);
								
								data[bitpos++] = AGMV_GetMSB(color16);
								data[bitpos++] = AGMV_GetLSB(color16);
							}
						}
						
						x -= 2;
					}
				}
			}
			
			for(n = 0; n < agmv->db.curr_pos; n++){
				agmv->db.read_table[n] = agmv->db.curr_table[n];
			}
			
			agmv->db.read_pos = agmv->db.curr_pos;
			agmv->db.curr_pos = 0;
		}
	}
	else{
		for(y = 0; y < height; y += 4){
			offset1 = y*width;
			for(x = 0; x < width; x += 4){
				skip = FALSE;

				for(n = 0; n < agmv->db.read_pos; n++){
					if(x == agmv->db.read_table[n]){
						data[bitpos++] = AGMV_SKIP_8;
						x += 4;
						skip = TRUE;
						break;
					}
				}
				
				if(skip == FALSE){
					entry = img_entry[x+offset1];
					color = palette0[entry.index];

					fill_count  = AGMV_GetFillBlockCount(agmv,x,y,color);
					copy_count  = AGMV_GetCopyBlockCount(agmv,x,y);
					pcopy_count = AGMV_GetPCopyBlockCount(agmv,x,y);

					r1 = AGMV_GetR(color);
					g1 = AGMV_GetG(color);
					b1 = AGMV_GetB(color);
					
					r2 = AGMV_GetR(agmv->prev_fill_color);
					g2 = AGMV_GetG(agmv->prev_fill_color);
					b2 = AGMV_GetB(agmv->prev_fill_color);
					
					diffr = AGMV_Abs(r1-r2);
					diffg = AGMV_Abs(g1-b2);
					diffb = AGMV_Abs(b1-b2);

					pmv_count.b4x4 = 0; mv_count.b4x4 = 0;
					pmv_count.b8x4 = 0; mv_count.b8x4 = 0;
					pmv_count.b2x4 = 0; mv_count.b2x4 = 0;
					pmv_count.b8x8 = 0; mv_count.b8x8 = 0;
					
					icopy_count.b4x4 = 0; icopy_count.copyx1 = 0; icopy_count.copyy1 = 0;
					icopy_count.b2x4 = 0; icopy_count.copyx2 = 0; icopy_count.copyy2 = 0;
					icopy_count.b8x4 = 0; icopy_count.copyx3 = 0; icopy_count.copyy3 = 0;
					icopy_count.b8x8 = 0; icopy_count.copyx4 = 0; icopy_count.copyy4 = 0;
					
					icopyr_count.b4x4 = 0; icopyr_count.copyx1 = 0; icopyr_count.copyy1 = 0;
					icopyr_count.b2x4 = 0; icopyr_count.copyx2 = 0; icopyr_count.copyy2 = 0;
					icopyr_count.b8x4 = 0; icopyr_count.copyx3 = 0; icopyr_count.copyy3 = 0;
					icopyr_count.b8x8 = 0; icopyr_count.copyx4 = 0; icopyr_count.copyy4 = 0;
					
					vq_count.c4x4.num = 0; vq_count.c8x4.num = 0; vq_count.c2x4.num = 0; vq_count.c8x8.num = 0;
					
					sblock_count.sector1 = 0;    sblock_count.sector2 = 0; 	  sblock_count.sector3 = 0;    sblock_count.sector4 = 0; 
					sblock_count4x4.sector1 = 0; sblock_count4x4.sector2 = 0; sblock_count4x4.sector3 = 0; sblock_count4x4.sector4 = 0; 
					
					if(copy_count.b2x4 < AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_COPY_FLAG,quality,opt) && pcopy_count.b2x4 < AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_PCOPY_FLAG,quality,opt)){
						
						AGMV_ThreeStepSearch(agmv,x,y);
			
						if(fill_count.b2x4 < fill2x4 && fill_count.b4x4 < fill4x4 && icopy_count.b2x4 < AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_ICOPY_FLAG,quality,opt) && 
						mv_count.b2x4 < AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_MV_FLAG,quality,opt) && pmv_count.b2x4 < AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_PMV_FLAG,quality,opt)){
							
							icopy_count = AGMV_GetIntraCopyBlockCount(agmv,x,y);
							icopyr_count = AGMV_GetRightIntraCopyBlockCount(agmv,x,y);
							vq_count = AGMV_GetVQBlockCount(agmv,x,y);
							
							if(icopy_count.b2x4 < AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_ICOPY_FLAG,quality,opt) && icopyr_count.b2x4 < AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_ICOPY_FLAG,quality,opt)){
							
								AGMV_ThreeStepSearch2x2(agmv,&sector.quad[0],&sector.quad[1],&sector.quad[2],&sector.quad[3],x,y);

								if(pframe_count >= 1){
									sblock_count.sector1 = AGMV_GetPMVSBlockCount(agmv,sector.quad[0].pmv,x,y,2,2);
									sblock_count.sector2 = AGMV_GetPMVSBlockCount(agmv,sector.quad[1].pmv,x+2,y,2,2);
									sblock_count.sector3 = AGMV_GetPMVSBlockCount(agmv,sector.quad[2].pmv,x,y+2,2,2);
									sblock_count.sector4 = AGMV_GetPMVSBlockCount(agmv,sector.quad[3].pmv,x+2,y+2,2,2);
								}
								else{
									sblock_count.sector1 = AGMV_GetMVSBlockCount(agmv,sector.quad[0].mv,x,y,2,2);
									sblock_count.sector2 = AGMV_GetMVSBlockCount(agmv,sector.quad[1].mv,x+2,y,2,2);
									sblock_count.sector3 = AGMV_GetMVSBlockCount(agmv,sector.quad[2].mv,x,y+2,2,2);
									sblock_count.sector4 = AGMV_GetMVSBlockCount(agmv,sector.quad[3].mv,x+2,y+2,2,2);
								}
								
								if(sblock_count.sector1 < 4 && sblock_count.sector2 < 3 && sblock_count.sector3 < 4 && sblock_count.sector4 < 3){
								
									AGMV_ThreeStepSearch4x4(agmv,&sector4x4.quad[0],&sector4x4.quad[1],x,y);
									
									if(pframe_count >= 1){
										sblock_count4x4.sector1 = AGMV_GetPMVSBlockCount(agmv,sector4x4.quad[0].pmv,x,y,4,4);
										sblock_count4x4.sector2 = AGMV_GetPMVSBlockCount(agmv,sector4x4.quad[1].pmv,x+4,y,4,4);
									}
									else{
										sblock_count4x4.sector1 = AGMV_GetMVSBlockCount(agmv,sector4x4.quad[0].mv,x,y,4,4);
										sblock_count4x4.sector2 = AGMV_GetMVSBlockCount(agmv,sector4x4.quad[1].mv,x+4,y,4,4);
									}
								}	
							}
						}
					}
					
					if((pcopy_count.b4x4 >= pcopy4x4 || pcopy_count.b8x4 >= pcopy8x4 || pcopy_count.b2x4 >= pcopy2x4 || pcopy_count.b8x8 >= pcopy8x8) && pframe_count >= 1){
						
						if(pcopy_count.b8x8 >= pcopy8x8){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_PCOPY_FLAG);
							x += 4;
						}
						else if(pcopy_count.b8x4 >= pcopy8x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_PCOPY_FLAG);
							x += 4;
						}
						else if(pcopy_count.b4x4 >= pcopy4x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_PCOPY_FLAG);
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_PCOPY_FLAG);
							x -= 2;
						}
						
						flagc++; pcopyc++;
					}
					else if(copy_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_COPY_FLAG,quality,opt) || copy_count.b8x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_COPY_FLAG,quality,opt)
					|| copy_count.b2x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_COPY_FLAG,quality,opt) || copy_count.b8x8 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_COPY_FLAG,quality,opt)){
						
						if(copy_count.b8x8 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_COPY_FLAG,quality,opt)){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_COPY_FLAG);
							x += 4;
						}						
						else if(copy_count.b8x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_COPY_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_COPY_FLAG);
							x += 4;
						}
						else if(copy_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_COPY_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_COPY_FLAG);
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_COPY_FLAG);
							x -= 2;
						}
					}
					else if((diffr <= max_diff && diffg <= max_diff && diffb <= max_diff) && (fill_count.b4x4 >= fill4x4 || fill_count.b8x4 >= fill8x4 || fill_count.b2x4 >= fill2x4 || fill_count.b8x8 >= fill8x8)){
						if(fill_count.b8x8 >= fill8x8){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_PFILL_FLAG);
							x += 4;
						}
						else if(fill_count.b8x4 >= fill8x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_PFILL_FLAG);
							x += 4;
						}
						else if(fill_count.b4x4 >= fill4x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_PFILL_FLAG);
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_PFILL_FLAG);
							x -= 2;
						}
					}
					else if(fill_count.b4x4 >= fill4x4 || fill_count.b8x4 >= fill8x4 || fill_count.b2x4 >= fill2x4 || fill_count.b8x8 >= fill8x8){
						
						if(fill_count.b8x8 >= fill8x8){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_FILL_FLAG);
							x += 4;
						}
						else if(fill_count.b8x4 >= fill8x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_FILL_FLAG);
							x += 4;
						}
						else if(fill_count.b4x4 >= fill4x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_FILL_FLAG);
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_FILL_FLAG);
							x -= 2;
						}

						data[bitpos++] = entry.index;
						agmv->prev_fill_color = color;
					}
					else if((pmv_count.b4x4 >= pmv4x4|| pmv_count.b8x4 >= pmv8x4 || pmv_count.b2x4 >= pmv2x4 || pmv_count.b8x8 >= pmv8x8) && pframe_count >= 1){
						
						if(pmv_count.b8x8 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_PMV_FLAG,quality,opt)){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_PMV_FLAG);
							
							u1 = agmv->pmv8x8.x + 8;
							u2 = agmv->pmv8x8.y + 8;
							
							data[bitpos++] =  (u1 << 4) | u2;
		
							x += 4;
						}
						else if(pmv_count.b8x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_PMV_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_PMV_FLAG);
							
							u1 = agmv->pmv8x4.x + 8;
							u2 = agmv->pmv8x4.y + 8;
							
							data[bitpos++] =  (u1 << 4) | u2;
							
							x += 4;
						}
						else if(pmv_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_PMV_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_PMV_FLAG);
							
							u1 = agmv->pmv.x + 8;
							u2 = agmv->pmv.y + 8;
							
							data[bitpos++] =  (u1 << 4) | u2;
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_PMV_FLAG);
							
							u1 = agmv->pmv2x4.x + 8;
							u2 = agmv->pmv2x4.y + 8;
							
							data[bitpos++] =  (u1 << 4) | u2;
							
							x -= 2;
						}
					}
					else if(mv_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_MV_FLAG,quality,opt) || mv_count.b8x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_MV_FLAG,quality,opt)
					|| mv_count.b2x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_MV_FLAG,quality,opt) || mv_count.b8x8 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_MV_FLAG,quality,opt)){
						
						if(mv_count.b8x8 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_PMV_FLAG,quality,opt)){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_MV_FLAG);
							
							u1 = agmv->mv8x8.x + 8;
							u2 = agmv->mv8x8.y + 8;
							
							data[bitpos++] =  (u1 << 4) | u2;
							
							x += 4;
						}
						else if(mv_count.b8x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_MV_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_MV_FLAG);
							
							u1 = agmv->mv8x4.x + 8;
							u2 = agmv->mv8x4.y + 8;
							
							data[bitpos++] =  (u1 << 4) | u2;
							
							x += 4;
						}
						else if(mv_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_MV_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_MV_FLAG);
							
							u1 = agmv->mv.x + 8;
							u2 = agmv->mv.y + 8;
							
							data[bitpos++] =  (u1 << 4) | u2;
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_MV_FLAG);
							
							u1 = agmv->mv2x4.x + 8;
							u2 = agmv->mv2x4.y + 8;
							
							data[bitpos++] =  (u1 << 4) | u2;
							
							x -= 2;
						}
					}
					else if(icopy_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_ICOPY_FLAG,quality,opt) || icopy_count.b2x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_ICOPY_FLAG,quality,opt)){
						if(icopy_count.b8x8 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_ICOPY_FLAG,quality,opt)){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_ICOPY_FLAG);
							data[bitpos++] = (x - icopy_count.copyx4) << 4 | (y - icopy_count.copyy4);
							
							x += 4;
						}
						else if(icopy_count.b8x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_ICOPY_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_ICOPY_FLAG);
							data[bitpos++] = (x - icopy_count.copyx2) << 4 | (y - icopy_count.copyy2);
							
							x += 4;
						}
						else if(icopy_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_ICOPY_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_ICOPY_FLAG);
							data[bitpos++] = (x - icopy_count.copyx1) << 4 | (y - icopy_count.copyy1);
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_ICOPY_FLAG);
							data[bitpos++] = (x - icopy_count.copyx3) << 4 | (y - icopy_count.copyy3);
							
							x -= 2;
						}
					}
					else if(icopyr_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_ICOPY_FLAG,quality,opt) || icopyr_count.b2x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_2x4,AGMV_ICOPY_FLAG,quality,opt)){
						if(icopyr_count.b8x8 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x8,AGMV_ICOPY_FLAG,quality,opt)){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_ICOPYR_FLAG);
							data[bitpos++] = (icopyr_count.copyx4 - x) << 4 | (y - icopyr_count.copyy4);

							x += 4;
						}
						else if(icopyr_count.b8x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_8x4,AGMV_ICOPY_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_ICOPYR_FLAG);
							data[bitpos++] = (icopyr_count.copyx2 - x) << 4 | (y - icopyr_count.copyy2);

							x += 4;
						}
						else if(icopyr_count.b4x4 >= AGMV_GetCount(agmv,AGMV_BLOCK_4x4,AGMV_ICOPY_FLAG,quality,opt)){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_ICOPYR_FLAG);
							data[bitpos++] = (icopyr_count.copyx1 - x) << 4 | (y - icopyr_count.copyy1);
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_ICOPYR_FLAG);
							data[bitpos++] = (icopyr_count.copyx3 - x) << 4 | (y - icopyr_count.copyy3);
							
							x -= 2;
						}
					}
					else if((sblock_count.sector1 >= 4 && sblock_count.sector3 >= 4) || (sblock_count4x4.sector1 >= 16 && sblock_count4x4.sector2 >= 16)){

						if(sblock_count4x4.sector1 >= 16 && sblock_count4x4.sector2 >= 16){
							
							if(pframe_count >= 1){
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_SMV_FLAG);
								
								for(i = 0; i < 2; i++){
									data[bitpos++] = sector4x4.quad[i].pmv.x;
									data[bitpos++] = sector4x4.quad[i].pmv.y;
								}
							}
							else{
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_SMV_FLAG);
								
								for(i = 0; i < 2; i++){
									data[bitpos++] = sector4x4.quad[i].mv.x;
									data[bitpos++] = sector4x4.quad[i].mv.y;
								}
							}
							
							x += 4;
						}
						else if(sblock_count.sector1 >= 4 && sblock_count.sector2 >= 4 && sblock_count.sector3 >= 4 && sblock_count.sector4 >= 4){
							
							if(pframe_count >= 1){
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_SMV_FLAG);
								
								for(i = 0; i < 4; i++){
									u1 = sector.quad[i].pmv.x + 8;
									u2 = sector.quad[i].pmv.y + 8;
								
									data[bitpos++] =  (u1 << 4) | u2;
								}
							}
							else{
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_SMV_FLAG);
								
								for(i = 0; i < 4; i++){
									u1 = sector.quad[i].mv.x + 8;
									u2 = sector.quad[i].mv.y + 8;
								
									data[bitpos++] =  (u1 << 4) | u2;
								}
							}
						}
						else{
							if(pframe_count >= 1){
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_SMV_FLAG);

								u1 = sector.quad[0].pmv.x + 8;
								u2 = sector.quad[0].pmv.y + 8;
							
								data[bitpos++] =  (u1 << 4) | u2;
								
								u1 = sector.quad[2].pmv.x + 8;
								u2 = sector.quad[2].pmv.y + 8;
							
								data[bitpos++] =  (u1 << 4) | u2;
							}
							else{
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_SMV_FLAG);
								
								u1 = sector.quad[0].mv.x + 8;
								u2 = sector.quad[0].mv.y + 8;
							
								data[bitpos++] =  (u1 << 4) | u2;
								
								u1 = sector.quad[2].mv.x + 8;
								u2 = sector.quad[2].mv.y + 8;
							
								data[bitpos++] =  (u1 << 4) | u2;
							}
							
							x -= 2;
						}
					}
					else if(vq_count.c2x4.num > 0 && vq_count.c2x4.num <= vq22x4){
						u32* frame_ptr, color1, color2, dist1, dist2;
						u32 block_w, block_h;
						u8 index1, index2;
						int r, g, b, r1, g1, b1, r2, g2, b2, rdiff1, gdiff1, bdiff1, rdiff2, gdiff2, bdiff2;
						u8 count = 0, byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8;
						Bool col1, col2;
						u8 indices[64];
						
						col1 = FALSE;
						col2 = FALSE;

						if(vq_count.c8x8.num > 0 && vq_count.c8x8.num <= vq28x8){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							
							color1 = vq_count.c8x8.coffre[vq_count.c8x8.num-1].color;
							color2 = vq_count.c8x8.coffre[vq_count.c8x8.num-2].color;
							
							r1 = AGMV_GetR(color1);
							g1 = AGMV_GetG(color1);
							b1 = AGMV_GetB(color1);
							
							r2 = AGMV_GetR(agmv->vq2color1);
							g2 = AGMV_GetG(agmv->vq2color1);
							b2 = AGMV_GetB(agmv->vq2color1);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col1 = TRUE;
							}
							
							r1 = AGMV_GetR(color2);
							g1 = AGMV_GetG(color2);
							b1 = AGMV_GetB(color2);
							
							r2 = AGMV_GetR(agmv->vq2color2);
							g2 = AGMV_GetG(agmv->vq2color2);
							b2 = AGMV_GetB(agmv->vq2color2);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col2 = TRUE;
							}
							
							if(col1 == TRUE && col2 == TRUE){
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_PVQ2_FLAG);
							}
							else{
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_VQ2_FLAG);
							}
							
							block_w = 8; block_h = 8;
						}
						else if(vq_count.c8x4.num > 0 && vq_count.c8x4.num <= vq28x4){	
							color1 = vq_count.c8x4.coffre[vq_count.c8x4.num-1].color;
							color2 = vq_count.c8x4.coffre[vq_count.c8x4.num-2].color;
							
							r1 = AGMV_GetR(color1);
							g1 = AGMV_GetG(color1);
							b1 = AGMV_GetB(color1);
							
							r2 = AGMV_GetR(agmv->vq2color1);
							g2 = AGMV_GetG(agmv->vq2color1);
							b2 = AGMV_GetB(agmv->vq2color1);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col1 = TRUE;
							}
							
							r1 = AGMV_GetR(color2);
							g1 = AGMV_GetG(color2);
							b1 = AGMV_GetB(color2);
							
							r2 = AGMV_GetR(agmv->vq2color2);
							g2 = AGMV_GetG(agmv->vq2color2);
							b2 = AGMV_GetB(agmv->vq2color2);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col2 = TRUE;
							}
							
							if(col1 == TRUE && col2 == TRUE){
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_PVQ2_FLAG);
							}
							else{
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_VQ2_FLAG);
							}
							
							block_w = 8; block_h = 4;
						}
						else if(vq_count.c4x4.num > 0 && vq_count.c4x4.num <= vq24x4){
							color1 = vq_count.c4x4.coffre[vq_count.c4x4.num-1].color;
							color2 = vq_count.c4x4.coffre[vq_count.c4x4.num-2].color;
							
							r1 = AGMV_GetR(color1);
							g1 = AGMV_GetG(color1);
							b1 = AGMV_GetB(color1);
							
							r2 = AGMV_GetR(agmv->vq2color1);
							g2 = AGMV_GetG(agmv->vq2color1);
							b2 = AGMV_GetB(agmv->vq2color1);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col1 = TRUE;
							}
							
							r1 = AGMV_GetR(color2);
							g1 = AGMV_GetG(color2);
							b1 = AGMV_GetB(color2);
							
							r2 = AGMV_GetR(agmv->vq2color2);
							g2 = AGMV_GetG(agmv->vq2color2);
							b2 = AGMV_GetB(agmv->vq2color2);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col2 = TRUE;
							}
							
							if(col1 == TRUE && col2 == TRUE){
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_PVQ2_FLAG);
							}
							else{
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_VQ2_FLAG);
							}
							
							block_w = 4; block_h = 4;
						}
						else{
							color1 = vq_count.c2x4.coffre[vq_count.c2x4.num-1].color;
							color2 = vq_count.c2x4.coffre[vq_count.c2x4.num-2].color;
						
							r1 = AGMV_GetR(color1);
							g1 = AGMV_GetG(color1);
							b1 = AGMV_GetB(color1);
							
							r2 = AGMV_GetR(agmv->vq2color1);
							g2 = AGMV_GetG(agmv->vq2color1);
							b2 = AGMV_GetB(agmv->vq2color1);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col1 = TRUE;
							}
							
							r1 = AGMV_GetR(color2);
							g1 = AGMV_GetG(color2);
							b1 = AGMV_GetB(color2);
							
							r2 = AGMV_GetR(agmv->vq2color2);
							g2 = AGMV_GetG(agmv->vq2color2);
							b2 = AGMV_GetB(agmv->vq2color2);
							
							diffr = AGMV_Abs(r1-r2);
							diffg = AGMV_Abs(g1-g2);
							diffb = AGMV_Abs(b1-b2);
							
							if(diffr <= max_diff && diffg <= max_diff && diffb <= max_diff){
								col2 = TRUE;
							}
							
							if(col1 == TRUE && col2 == TRUE){
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_PVQ2_FLAG);
							}
							else{
								data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_VQ2_FLAG);
							}
							
							block_w = 2; block_h = 4;
						}
						
						agmv->vq2color1 = color1;
						agmv->vq2color2 = color2;
						
						index1 = AGMV_FindNearestColor(palette0,color1);
						index2 = AGMV_FindNearestColor(palette0,color2);
						
						if(col1 == FALSE || col2 == FALSE){
							data[bitpos++] = index1;
							data[bitpos++] = index2;
						}
						
						frame_ptr = &frame[x+y*width];
				
						r1 = AGMV_GetR(color1);
						g1 = AGMV_GetG(color1);
						b1 = AGMV_GetB(color1);
						
						r2 = AGMV_GetR(color2);
						g2 = AGMV_GetG(color2);
						b2 = AGMV_GetB(color2);
		
						for(j = 0; j < block_h; j++){
							for(i = 0; i < block_w; i++){
								color = frame_ptr[i];
								
								r = AGMV_GetR(color);
								g = AGMV_GetG(color);
								b = AGMV_GetB(color);
								
								rdiff1 = r1-r;
								gdiff1 = g1-g;
								bdiff1 = b1-b;
								
								rdiff2 = r2-r;
								gdiff2 = g2-g;
								bdiff2 = b2-b;
								
								dist1 = rdiff1*rdiff1 + gdiff1*gdiff1 + bdiff1*bdiff1;
								dist2 = rdiff2*rdiff2 + gdiff2*gdiff2 + bdiff2*bdiff2;
								
								if(dist1 < dist2){
									indices[count++] = 0;
								}
								else{
									indices[count++] = 1;
								}
							}
							
							frame_ptr += width;
						}
						
						if(block_w == 8 && block_h == 8){
							byte1 = indices[0] << 7 | indices[1] << 6 | indices[2] << 5 | indices[3] << 4 | indices[4] << 3 | indices[5] << 2 | indices[6] << 1 | indices[7];
							byte2 = indices[8] << 7 | indices[9] << 6 | indices[10] << 5 | indices[11] << 4 | indices[12] << 3 | indices[13] << 2 | indices[14] << 1 | indices[15];
							byte3 = indices[16] << 7 | indices[17] << 6 | indices[18] << 5 | indices[19] << 4 | indices[20] << 3 | indices[21] << 2 | indices[22] << 1 | indices[23];
							byte4 = indices[24] << 7 | indices[25] << 6 | indices[26] << 5 | indices[27] << 4 | indices[28] << 3 | indices[29] << 2 | indices[30] << 1 | indices[31];
							byte5 = indices[32] << 7 | indices[33] << 6 | indices[34] << 5 | indices[35] << 4 | indices[36] << 3 | indices[37] << 2 | indices[38] << 1 | indices[39];
							byte6 = indices[40] << 7 | indices[41] << 6 | indices[42] << 5 | indices[43] << 4 | indices[44] << 3 | indices[45] << 2 | indices[46] << 1 | indices[47];
							byte7 = indices[48] << 7 | indices[49] << 6 | indices[50] << 5 | indices[51] << 4 | indices[52] << 3 | indices[53] << 2 | indices[54] << 1 | indices[55];
							byte8 = indices[56] << 7 | indices[57] << 6 | indices[58] << 5 | indices[59] << 4 | indices[60] << 3 | indices[61] << 2 | indices[62] << 1 | indices[63];

							
							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
							data[bitpos++] = byte3;
							data[bitpos++] = byte4;
							data[bitpos++] = byte5;
							data[bitpos++] = byte6;
							data[bitpos++] = byte7;
							data[bitpos++] = byte8;
							
							x += 4;
						}
						else if(block_w == 8){
							byte1 = indices[0] << 7 | indices[1] << 6 | indices[2] << 5 | indices[3] << 4 | indices[4] << 3 | indices[5] << 2 | indices[6] << 1 | indices[7];
							byte2 = indices[8] << 7 | indices[9] << 6 | indices[10] << 5 | indices[11] << 4 | indices[12] << 3 | indices[13] << 2 | indices[14] << 1 | indices[15];
							byte3 = indices[16] << 7 | indices[17] << 6 | indices[18] << 5 | indices[19] << 4 | indices[20] << 3 | indices[21] << 2 | indices[22] << 1 | indices[23];
							byte4 = indices[24] << 7 | indices[25] << 6 | indices[26] << 5 | indices[27] << 4 | indices[28] << 3 | indices[29] << 2 | indices[30] << 1 | indices[31];
						
							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
							data[bitpos++] = byte3;
							data[bitpos++] = byte4;
							
							x += 4;
						}
						else if(block_w == 4){
							byte1 = indices[0] << 7 | indices[1] << 6 | indices[2] << 5 | indices[3] << 4 | indices[4] << 3 | indices[5] << 2 | indices[6] << 1 | indices[7];
							byte2 = indices[8] << 7 | indices[9] << 6 | indices[10] << 5 | indices[11] << 4 | indices[12] << 3 | indices[13] << 2 | indices[14] << 1 | indices[15];
						
							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
						}
						else{
							byte1 = indices[0] << 7 | indices[1] << 6 | indices[2] << 5 | indices[3] << 4 | indices[4] << 3 | indices[5] << 2 | indices[6] << 1 | indices[7];					
							data[bitpos++] = byte1;
							
							x -= 2;
						}
					}
					else if(vq_count.c4x4.num > 0 && vq_count.c4x4.num <= vq44x4){
						u32* frame_ptr, color1, color2, color3, color4, dist1, dist2, dist3, dist4, min, block_w, block_h;
						u8 index1, index2, index3, index4;
						int r, g, b, r1, g1, b1, r2, g2, b2, r3, g3, b3, r4, g4, b4, rdiff1, gdiff1, bdiff1, rdiff2, gdiff2, bdiff2;
						int rdiff3, gdiff3, bdiff3, rdiff4, gdiff4, bdiff4;
						u8 count = 0, byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, byte9, byte10, byte11, byte12, byte13, byte14, byte15, byte16;
						u8 indices[64];
						
						if(vq_count.c8x8.num > 0 && vq_count.c8x8.num <= vq48x8){
							agmv->db.curr_table[agmv->db.curr_pos++] = x;
							
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x8,AGMV_VQ4_FLAG);
						
							color1 = vq_count.c8x8.coffre[vq_count.c8x8.num-1].color;
							color2 = vq_count.c8x8.coffre[vq_count.c8x8.num-2].color;
							color3 = vq_count.c8x8.coffre[vq_count.c8x8.num-3].color;
							color4 = vq_count.c8x8.coffre[vq_count.c8x8.num-4].color;

							block_w = 8; block_h = 8;
						}
						else if(vq_count.c8x4.num > 0 && vq_count.c8x4.num <= vq48x4){
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_8x4,AGMV_VQ4_FLAG);
						
							color1 = vq_count.c8x4.coffre[vq_count.c8x4.num-1].color;
							color2 = vq_count.c8x4.coffre[vq_count.c8x4.num-2].color;
							color3 = vq_count.c8x4.coffre[vq_count.c8x4.num-3].color;
							color4 = vq_count.c8x4.coffre[vq_count.c8x4.num-4].color;

							block_w = 8; block_h = 4;
						}
						else{
							data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_4x4,AGMV_VQ4_FLAG);
							
							color1 = vq_count.c4x4.coffre[vq_count.c4x4.num-1].color;
							color2 = vq_count.c4x4.coffre[vq_count.c4x4.num-2].color;
							color3 = vq_count.c4x4.coffre[vq_count.c4x4.num-3].color;
							color4 = vq_count.c4x4.coffre[vq_count.c4x4.num-4].color;
							
							block_w = 4; block_h = 4;
						}
						
						index1 = AGMV_FindNearestColor(palette0,color1);
						index2 = AGMV_FindNearestColor(palette0,color2);
						index3 = AGMV_FindNearestColor(palette0,color3);
						index4 = AGMV_FindNearestColor(palette0,color4);
					
						data[bitpos++] = index1;
						data[bitpos++] = index2;
						data[bitpos++] = index3;
						data[bitpos++] = index4;
						
						frame_ptr = &frame[x+y*width];
						
						r1 = AGMV_GetR(color1);
						g1 = AGMV_GetG(color1);
						b1 = AGMV_GetB(color1);
						
						r2 = AGMV_GetR(color2);
						g2 = AGMV_GetG(color2);
						b2 = AGMV_GetB(color2);
						
						r3 = AGMV_GetR(color3);
						g3 = AGMV_GetG(color3);
						b3 = AGMV_GetB(color3);
						
						r4 = AGMV_GetR(color4);
						g4 = AGMV_GetG(color4);
						b4 = AGMV_GetB(color4);
						
						for(j = 0; j < block_h; j++){
							for(i = 0; i < block_w; i++){
								color = frame_ptr[i];
								
								r = AGMV_GetR(color);
								g = AGMV_GetG(color);
								b = AGMV_GetB(color);
								
								rdiff1 = r1-r;
								gdiff1 = g1-g;
								bdiff1 = b1-b;
								
								rdiff2 = r2-r;
								gdiff2 = g2-g;
								bdiff2 = b2-b;
								
								rdiff3 = r3-r;
								gdiff3 = g3-g;
								bdiff3 = b3-b;
								
								rdiff4 = r4-r;
								gdiff4 = g4-g;
								bdiff4 = b4-b;
								
								dist1 = rdiff1*rdiff1 + gdiff1*gdiff1 + bdiff1*bdiff1;
								dist2 = rdiff2*rdiff2 + gdiff2*gdiff2 + bdiff2*bdiff2;
								dist3 = rdiff3*rdiff3 + gdiff3*gdiff3 + bdiff3*bdiff3;
								dist4 = rdiff4*rdiff4 + gdiff4*gdiff4 + bdiff4*bdiff4;
								
								min = AGMV_Min(dist1,dist2);
								min = AGMV_Min(dist3,min);
								min = AGMV_Min(dist4,min);
								
								if(min == dist1){
									indices[count++] = 0;
								}
								else if(min == dist2){
									indices[count++] = 1;
								}
								else if(min == dist3){
									indices[count++] = 2;
								}
								else{
									indices[count++] = 3;
								}
							}
							
							frame_ptr += width;
						}
						
						if(block_w == 4){
							byte1 = indices[0] << 6 | indices[1] << 4 | indices[2] << 2 | indices[3];
							byte2 = indices[4] << 6 | indices[5] << 4 | indices[6] << 2 | indices[7];
							byte3 = indices[8] << 6 | indices[9] << 4 | indices[10] << 2 | indices[11];
							byte4 = indices[12] << 6 | indices[13] << 4 | indices[14] << 2 | indices[15];
							
							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
							data[bitpos++] = byte3;
							data[bitpos++] = byte4;
						}
						else if(block_w == 8 && block_h == 4){
							byte1 = indices[0] << 6 | indices[1] << 4 | indices[2] << 2 | indices[3];
							byte2 = indices[4] << 6 | indices[5] << 4 | indices[6] << 2 | indices[7];
							byte3 = indices[8] << 6 | indices[9] << 4 | indices[10] << 2 | indices[11];
							byte4 = indices[12] << 6 | indices[13] << 4 | indices[14] << 2 | indices[15];
							byte5 = indices[16] << 6 | indices[17] << 4 | indices[18] << 2 | indices[19];
							byte6 = indices[20] << 6 | indices[21] << 4 | indices[22] << 2 | indices[23];
							byte7 = indices[24] << 6 | indices[25] << 4 | indices[26] << 2 | indices[27];
							byte8 = indices[28] << 6 | indices[29] << 4 | indices[30] << 2 | indices[31];
							
							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
							data[bitpos++] = byte3;
							data[bitpos++] = byte4;
							data[bitpos++] = byte5;
							data[bitpos++] = byte6;
							data[bitpos++] = byte7;
							data[bitpos++] = byte8;
							
							x += 4;
						}
						else{
							byte1 = indices[0] << 6 | indices[1] << 4 | indices[2] << 2 | indices[3];
							byte2 = indices[4] << 6 | indices[5] << 4 | indices[6] << 2 | indices[7];
							byte3 = indices[8] << 6 | indices[9] << 4 | indices[10] << 2 | indices[11];
							byte4 = indices[12] << 6 | indices[13] << 4 | indices[14] << 2 | indices[15];
							byte5 = indices[16] << 6 | indices[17] << 4 | indices[18] << 2 | indices[19];
							byte6 = indices[20] << 6 | indices[21] << 4 | indices[22] << 2 | indices[23];
							byte7 = indices[24] << 6 | indices[25] << 4 | indices[26] << 2 | indices[27];
							byte8 = indices[28] << 6 | indices[29] << 4 | indices[30] << 2 | indices[31];
							byte9 = indices[32] << 6 | indices[33] << 4 | indices[34] << 2 | indices[35];
							byte10 = indices[36] << 6 | indices[37] << 4 | indices[38] << 2 | indices[39];
							byte11 = indices[40] << 6 | indices[41] << 4 | indices[42] << 2 | indices[43];
							byte12 = indices[44] << 6 | indices[45] << 4 | indices[46] << 2 | indices[47];
							byte13 = indices[48] << 6 | indices[49] << 4 | indices[50] << 2 | indices[51];
							byte14 = indices[52] << 6 | indices[53] << 4 | indices[54] << 2 | indices[55];
							byte15 = indices[56] << 6 | indices[57] << 4 | indices[58] << 2 | indices[59];
							byte16 = indices[60] << 6 | indices[61] << 4 | indices[62] << 2 | indices[63];
							
							data[bitpos++] = byte1;
							data[bitpos++] = byte2;
							data[bitpos++] = byte3;
							data[bitpos++] = byte4;
							data[bitpos++] = byte5;
							data[bitpos++] = byte6;
							data[bitpos++] = byte7;
							data[bitpos++] = byte8;
							data[bitpos++] = byte9;
							data[bitpos++] = byte10;
							data[bitpos++] = byte11;
							data[bitpos++] = byte12;
							data[bitpos++] = byte13;
							data[bitpos++] = byte14;
							data[bitpos++] = byte15;
							data[bitpos++] = byte16;
							
							x += 4;
						}
					}
					else{
						data[bitpos++] = AGMV_SetBitstreamFlag(AGMV_BLOCK_2x4,AGMV_NORMAL_FLAG);
					
						AGMV_ENTRY norm_arr[5], *img_entryp;
						
						img_entryp = &img_entry[x+y*width];
	
						norm_arr[0] = img_entryp[0];
						norm_arr[1] = img_entryp[1];
						
						img_entryp += width;
						img_entryp += width;
						
						norm_arr[2] = img_entryp[0];
						norm_arr[3] = img_entryp[1];
						
						img_entryp += width;
						
						norm_arr[4] = img_entryp[0];

						for(i = 0; i < 5; i++){
							data[bitpos++] = norm_arr[i].index;
						}
						
						x -= 2;
					}
				}
			}
			
			for(n = 0; n < agmv->db.curr_pos; n++){
				agmv->db.read_table[n] = agmv->db.curr_table[n];
			}
			
			agmv->db.read_pos = agmv->db.curr_pos;
			agmv->db.curr_pos = 0;
		}
	}
	
	agmv->normal_distribution = nc /(f32)flagc;

	agmv->bitstream->pos = bitpos;
}

void AGMV_EncodeFrame(FILE* file, AGMV* agmv, u32* img_data){
	AGMV_OPT opt = AGMV_GetOPT(agmv);
	AGMV_COMPRESSION compression = AGMV_GetCompression(agmv);
	AGMV_ENTRY* iframe_entries, *img_entry;
	u32* fimg_data, *iframe_data, frame_type;
	f32 ratio = 0;
	Bool iframe_constructed;

	int i, csize, pos, cpos, size;
	u32 palette0[256], palette1[256];
	
	for(i = 0; i < 256; i++){
		palette0[i] = agmv->header.palette0[i];
		palette1[i] = agmv->header.palette1[i];
	}
	
	size     = AGMV_GetWidth(agmv)*AGMV_GetHeight(agmv);
	frame_type = AGMV_PFRAME;
	
	iframe_entries = agmv->iframe_entries;
	img_entry = agmv->image_entries;

	AGMV_WriteFourCC(file,'A','G','F','C');
	AGMV_WriteLong(file,agmv->frame_count+1);
	
	fimg_data   = agmv->frame->img_data;
	iframe_data = agmv->iframe->img_data;
	
	agmv->bitstream->pos = 0;

	if(opt != AGMV_OPT_II && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_II){
		if(opt != AGMV_OPT_IV && opt != AGMV_OPT_GBA_IV && opt != AGMV_OPT_PSX_IV && opt != AGMV_OPT_NDS_IV){
			for(i = 0; i < size; i++){
				img_entry[i] = AGMV_FindNearestEntry(palette0,palette1,img_data[i]);
				
				if(img_entry[i].pal_num == 0){
					fimg_data[i] = palette0[img_entry[i].index];
				}
				else{
					fimg_data[i] = palette1[img_entry[i].index];
				}
			}
		}
		else{
			AGMV_Memcpy32(fimg_data,img_data,size);
		}
	
		if(agmv->frame_count == 0){
			iframe_constructed = TRUE;
			frame_type = AGMV_IFRAME;
			AGMV_AssembleIFrameBitstream(agmv,img_entry);
			agmv->pframe_count = 0;
		}
		else{
			ratio = AGMV_CompareFrameSimilarity(fimg_data,agmv->iframe->img_data,agmv->frame->width,agmv->frame->height);
			
			if((ratio >= 0.25f && agmv->pframe_count < 17) || agmv->iframe_count >= 1){
				iframe_constructed = FALSE;
				frame_type = AGMV_PFRAME;
				agmv->lenient = FALSE;
				AGMV_AssemblePFrameBitstream(agmv,img_entry);
				
				if(agmv->normal_distribution >= AGMV_MIN_DISTR || agmv->bitstream->pos >= size * 0.7){
					agmv->bitstream->pos = 0;
					agmv->lenient = TRUE;
					AGMV_AssemblePFrameBitstream(agmv,img_entry);
				}
				
				AGMV_Memcpy32(agmv->prev_frame->img_data,agmv->frame->img_data,size);
				agmv->iframe_count = 0;
				agmv->pframe_count++;
			}
			else{
				iframe_constructed = TRUE;
				frame_type = AGMV_IFRAME;
				agmv->pframe_count = 0;
				agmv->iframe_count++;
				AGMV_AssembleIFrameBitstream(agmv,img_entry);
			}
		}
		
		AGMV_WriteShort(file,frame_type);
		AGMV_WriteLong(file,agmv->bitstream->pos);
		AGMV_WriteLong(file,0);

		pos = ftell(file);
		
		if(compression == AGMV_LZSS_COMPRESSION){
			csize = AGMV_LZSS(file,agmv->bitstream);
		}
		else{
			csize = agmv->bitstream->pos;
			AGMV_WriteNBytes(file,agmv->bitstream->data,csize);
		}
		
		cpos = ftell(file);
		
		fseek(file,pos-4,SEEK_SET);
		AGMV_WriteLong(file,csize);
		fseek(file,cpos,SEEK_SET);
		
	}
	else{
		for(i = 0; i < size; i++){
			img_entry[i].index = AGMV_FindNearestColor(palette0,img_data[i]);
			img_entry[i].pal_num = 0;
			fimg_data[i] = palette0[img_entry[i].index];
		}
		
		if(agmv->frame_count == 0){
			iframe_constructed = TRUE;
			frame_type = AGMV_IFRAME;
			AGMV_AssembleIFrameBitstream(agmv,img_entry);
			agmv->pframe_count = 0;
		}
		else{
			ratio = AGMV_CompareFrameSimilarity(fimg_data,agmv->iframe->img_data,agmv->frame->width,agmv->frame->height);
			
			if((ratio >= 0.25f && agmv->pframe_count < 17) || agmv->iframe_count >= 1){
				iframe_constructed = FALSE;
				frame_type = AGMV_PFRAME;
				agmv->lenient = FALSE;
				AGMV_AssemblePFrameBitstream(agmv,img_entry);
				
				if(agmv->normal_distribution >= AGMV_MIN_DISTR || agmv->bitstream->pos >= size * 0.7){
					agmv->bitstream->pos = 0;
					agmv->lenient = TRUE;
					AGMV_AssemblePFrameBitstream(agmv,img_entry);
				}
				
				AGMV_Memcpy32(agmv->prev_frame->img_data,agmv->frame->img_data,size);
				agmv->iframe_count = 0;
				agmv->pframe_count++;
			}
			else{
				iframe_constructed = TRUE;
				frame_type = AGMV_IFRAME;
				agmv->pframe_count = 0;
				agmv->iframe_count++;
				AGMV_AssembleIFrameBitstream(agmv,img_entry);
			}
		}
		
		AGMV_WriteShort(file,frame_type);
		AGMV_WriteLong(file,agmv->bitstream->pos);
		AGMV_WriteLong(file,0);
		
		pos = ftell(file);

		if(compression == AGMV_LZSS_COMPRESSION){
			csize = AGMV_LZSS(file,agmv->bitstream);
		}
		else{
			csize = agmv->bitstream->pos;
			AGMV_WriteNBytes(file,agmv->bitstream->data,csize);
		}
		
		cpos = ftell(file);
		
		fseek(file,pos-4,SEEK_SET);
		AGMV_WriteLong(file,csize);
		fseek(file,cpos,SEEK_SET);
	}
	
	for(i = 0; i < 8; i++){
		AGMV_WriteByte(file,0xff);
	}
	
	if(opt != AGMV_OPT_IV && opt != AGMV_OPT_GBA_IV && opt != AGMV_OPT_PSX_IV && opt != AGMV_OPT_NDS_IV){
		if(iframe_constructed){
			for(i = 0; i < size; i++){
				iframe_entries[i] = img_entry[i];
			}
			AGMV_ConstructFrameFromEntries(agmv);
		}
	}
	else{
		if(iframe_constructed){
			AGMV_Memcpy32(iframe_data,img_data,size);
		}
	}

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

f32 AGMV_Round(f32 x) {
    if (x >= 0.0)
        return floor(x + 0.5);
    else
        return ceil(x - 0.5);
}

void AGMV_FillAudioBuffer(FILE* file, AGMV* agmv, AGMV_AUDIO_TYPE audio_type){
	if(audio_type == AGMV_AUDIO_WAV){
		if(AGMV_GetBitsPerSample(agmv) == 8){
			AGMV_ReadNBytes(file,agmv->audio_track->pcm8,agmv->audio_chunk->size);
		}
		else{
			AGMV_ReadNShorts(file,agmv->audio_track->pcm,agmv->audio_chunk->size);
		}
	}
	else if(audio_type == AGMV_AUDIO_AIFC){
		if(AGMV_GetBitsPerSample(agmv) == 8){
			AGMV_ReadNBytes(file,agmv->audio_track->pcm8,agmv->audio_chunk->size);
			AGMV_SignedToUnsignedPCM(agmv->audio_track->pcm8,agmv->audio_chunk->size);
		}
		else{
			if(agmv->sowt == TRUE){
				AGMV_ReadNShorts(file,agmv->audio_track->pcm,agmv->audio_chunk->size);
			}
			else{
				AGMV_ReadSwappedNShorts(file,agmv->audio_track->pcm,agmv->audio_chunk->size);
			}
		}
	}
	else if(audio_type == AGMV_AUDIO_AIFF){
		if(AGMV_GetBitsPerSample(agmv) == 8){
			AGMV_ReadNBytes(file,agmv->audio_track->pcm8,agmv->audio_chunk->size);
			AGMV_SignedToUnsignedPCM(agmv->audio_track->pcm8,agmv->audio_chunk->size);
		}
		else{
			AGMV_ReadSwappedNShorts(file,agmv->audio_track->pcm,agmv->audio_chunk->size);
		}
	}
	else{
		AGMV_ReadNBytes(file,agmv->audio_track->pcm8,agmv->audio_chunk->size);
		AGMV_SignedToUnsignedPCM(agmv->audio_track->pcm8,agmv->audio_chunk->size);
	}
}

void AGMV_CompressAudioBuffer(AGMV* agmv){
	int i, resamp1, resamp2, resamp3, size = agmv->audio_chunk->size;
	u32 dist1, dist2, dist3, dist;
	u8 ssqrt1, ssqrt2, shift, *atsample = agmv->audio_chunk->atsample;
	u16* pcm = agmv->audio_track->pcm;
	u8* pcm8 = agmv->audio_track->pcm8;
	
	if(AGMV_GetBitsPerSample(agmv) == 16){
		for(i = 0; i < size; i++){
			int samp = pcm[i];
			
			ssqrt1 = sqrt(samp);
			ssqrt2 = AGMV_Round(sqrt(samp));
			shift = samp >> 8;
			
			roundUpEven(&ssqrt1);
			roundUpEven(&ssqrt2);
			roundUpOdd(&shift);
			
			resamp1 = ssqrt1 * ssqrt1;
			resamp2 = ssqrt2 * ssqrt2;
			resamp3 = shift << 8;
			
			dist1 = AGMV_Abs(resamp1-samp);
			dist2 = AGMV_Abs(resamp2-samp);
			dist3 = AGMV_Abs(resamp3-samp);	    
			
			dist = AGMV_Min(dist1,dist2);
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
	else{
		for(i = 0; i < size; i++){
			atsample[i] = pcm8[i] >> 1;
		}
	}
}

void AGMV_EncodeAudioChunk(FILE* file, AGMV* agmv){
	int i, size = agmv->audio_chunk->size;
	u32 csize, pos, cpos;
	u8* atsample = agmv->audio_chunk->atsample, *bitstream = agmv->bitstream->data;
	u8 current_sample, previous_sample = 0;
	s8 difference;
		
	AGMV_WriteFourCC(file,'A','G','A','C');
	AGMV_WriteLong(file,agmv->audio_chunk->size);
	
	pos = ftell(file);
	
	AGMV_WriteLong(file,0);
	
	if(AGMV_GetBitsPerSample(agmv) == 16){
		AGMV_Memcpy8(bitstream,atsample,size);
	}
	else{			
		for(i = 0; i < size; i++){
			current_sample = atsample[i];
			difference = current_sample - previous_sample;
			bitstream[i] = difference;
			previous_sample = current_sample;
		}
	}

	agmv->bitstream->pos = size;
	csize = AGMV_LZSS(file,agmv->bitstream);
	cpos = ftell(file);
	
	fseek(file,pos,SEEK_SET);
	AGMV_WriteLong(file,csize);
	fseek(file,cpos,SEEK_SET);
	
	for(i = 0; i < 8; i++){
		AGMV_WriteByte(file,0xff);
	}
}

void AGMV_LoadHistogramImageData(const char* dir, const char* basename, char* ext, u32 start_frame, u32 end_frame, u32 size, u32* histogram, u32* colorgram, u8 img_type, AGMV_QUALITY quality){
	int i;
	for(i = start_frame; i <= end_frame; i++){
		char filename[60];
		if(dir[0] != 'c' || dir[1] != 'u' || dir[2] != 'r'){
			sprintf(filename,"%s/%s%ld%s",dir,basename,i,ext);
		}
		else{
			sprintf(filename,"%s%ld%s",basename,i,ext);
		}
		
		switch(img_type){
			case AGIDL_IMG_BMP:{
				u32* pixels;
				
				AGIDL_BMP* bmp = AGIDL_LoadBMP(filename);
				AGIDL_ColorConvertBMP(bmp,AGIDL_RGB_888);
				
				pixels = bmp->pixels.pix32;
				
				int n;
				for(n = 0; n < size; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}
	
				AGIDL_FreeBMP(bmp);
			}break;
			case AGIDL_IMG_TGA:{
				u32* pixels;
				
				AGIDL_TGA* tga = AGIDL_LoadTGA(filename);
				AGIDL_ColorConvertTGA(tga,AGIDL_RGB_888);
				
				pixels = tga->pixels.pix32;
				
				int n;
				for(n = 0; n < size; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}
				
				AGIDL_FreeTGA(tga);
			}break;
			case AGIDL_IMG_TIM:{
				u32* pixels;
				
				AGIDL_TIM* tim = AGIDL_LoadTIM(filename);
				
				pixels = tim->pixels.pix32;
				
				int n;
				for(n = 0; n < size; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}
				
				AGIDL_FreeTIM(tim);
			}break;
			case AGIDL_IMG_PCX:{
				u32* pixels;
				
				AGIDL_PCX* pcx = AGIDL_LoadPCX(filename);
				AGIDL_ColorConvertPCX(pcx,AGIDL_RGB_888);
				
				pixels = pcx->pixels.pix32;
				
				int n;
				for(n = 0; n < size; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}
						
				AGIDL_FreePCX(pcx);
			}break;
			case AGIDL_IMG_LMP:{
				u32* pixels;
				
				AGIDL_LMP* lmp = AGIDL_LoadLMP(filename);
				AGIDL_ColorConvertLMP(lmp,AGIDL_RGB_888);
				
				pixels = lmp->pixels.pix32;
				
				int n;
				for(n = 0; n < size; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}
						
				AGIDL_FreeLMP(lmp);
			}break;
			case AGIDL_IMG_PVR:{
				u32* pixels;
				
				AGIDL_PVR* pvr = AGIDL_LoadPVR(filename);
				AGIDL_ColorConvertPVR(pvr,AGIDL_RGB_888);
				
				pixels = pvr->pixels.pix32;
				
				int n;
				for(n = 0; n < size; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}
						
				AGIDL_FreePVR(pvr);
			}break;
			case AGIDL_IMG_GXT:{
				u32* pixels;
				
				AGIDL_GXT* gxt = AGIDL_LoadGXT(filename);
				AGIDL_ColorConvertGXT(gxt,AGIDL_RGB_888);
				
				pixels = gxt->pixels.pix32;
				
				int n;
				for(n = 0; n < size; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}
						
				AGIDL_FreeGXT(gxt);
			}break;
			case AGIDL_IMG_BTI:{
				u32* pixels;
				
				AGIDL_BTI* bti = AGIDL_LoadBTI(filename);
				AGIDL_ColorConvertBTI(bti,AGIDL_RGB_888);
				
				pixels = bti->pixels.pix32;
				
				int n;
				for(n = 0; n < size; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}
						
				AGIDL_FreeBTI(bti);
			}break;
			case AGIDL_IMG_3DF:{
				u32* pixels;
				
				AGIDL_3DF* glide = AGIDL_Load3DF(filename);
				AGIDL_ColorConvert3DF(glide,AGIDL_RGB_888);
				
				pixels = glide->pixels.pix32;
				
				int n;
				for(n = 0; n < size; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}
				
				AGIDL_Free3DF(glide);
			}break;
			case AGIDL_IMG_PPM:{
				u32* pixels;
				
				AGIDL_PPM* ppm = AGIDL_LoadPPM(filename);
				AGIDL_ColorConvertPPM(ppm,AGIDL_RGB_888);
				
				pixels = ppm->pixels.pix32;
				
				int n;
				for(n = 0; n < size; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}
				
				AGIDL_FreePPM(ppm);
			}break;
			case AGIDL_IMG_LBM:{
				u32* pixels;
				
				AGIDL_LBM* lbm = AGIDL_LoadLBM(filename);
				AGIDL_ColorConvertLBM(lbm,AGIDL_RGB_888);
				
				pixels = lbm->pixels.pix32;
				
				int n;
				for(n = 0; n < size; n++){
					u32 color = pixels[n];
					u32 hcolor = AGMV_QuantizeColor(color,quality);
					histogram[hcolor] = histogram[hcolor] + 1;
				}
				
				AGIDL_FreeLBM(lbm);
			}break;
		}
	}
}

void AGMV_DistributePaletteData(u32* colorgram, u32* pal, u32* palette0, u32* palette1, u32 max_clr, AGMV_OPT opt, AGMV_QUALITY quality){
	int n, count = 0;
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
				rdiff = -rdiff;
			}
			
			if(gdiff < 0){
				gdiff = -gdiff;
			}
			
			if(bdiff < 0){
				bdiff = -bdiff;
			}
			
			if(quality == AGMV_HIGH_QUALITY){
				if(rdiff <= 1 && gdiff <= 3 && bdiff <= 3){
					skip = TRUE;
				}
			}
			else if(quality == AGMV_MID_QUALITY){
				if(rdiff <= 1 && gdiff <= 3 && bdiff <= 1){
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
	
	if(opt == AGMV_OPT_I || opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_III || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_III){
		for(n = 0; n < 512; n++){
			u32 clr = pal[n];
			u32 invclr = AGMV_ReverseQuantizeColor(clr,quality);
			
			if(n < 126){
				palette0[n] = invclr;
			}
			else if(n >= 126 && n <= 152){
				palette1[n-126] = invclr;
			}
			
			if(n > 252 && n <= 381){
				palette0[n-126] = invclr;
			}
			
			if(n > 381 && (n-255) < 256){
				palette1[n-255] = invclr;
			}
		}
	}
	
	if(opt == AGMV_OPT_II || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_ANIM || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_NDS_II){
		for(n = 0; n < 256; n++){
			u32 clr = pal[n];
			u32 invclr = AGMV_ReverseQuantizeColor(clr,quality);
			
			palette0[n] = invclr;
		}
	}
}

void AGMV_EncodeVideo(const char* filename, const char* dir, const char* basename, u8 img_type, u32 start_frame, u32 end_frame, u32 width, u32 height, u32 frames_per_second, AGMV_OPT opt, AGMV_QUALITY quality, AGMV_COMPRESSION compression, AGMV_CUSTOMIZATION* customization){
	u32 i, palette0[256], palette1[256], num_of_frames_encoded = 0, w, h, max_clr, size = width*height, AGMV_GBA_W, AGMV_GBA_H;
	u32 pal[512];
	AGMV_QUALITY org_quality;
	
	org_quality = quality;
	
	AGMV* agmv = CreateAGMV(end_frame-start_frame,width,height,frames_per_second);
	AGMV_SetOPT(agmv,opt);
	AGMV_SetCompression(agmv,compression);
	AGMV_SetQuality(agmv,quality);
	
	if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
		agmv->max_diff = AGMV_MAX_15BPP_DIFF;
	}
	else{
		agmv->max_diff = AGMV_MAX_DIFF;
	}
	
	if(quality == AGMV_CUSTOM_QUALITY){
		agmv->custom.b4x4.fill  = customization->b4x4.fill;
		agmv->custom.b4x4.copy  = customization->b4x4.copy;
		agmv->custom.b4x4.icopy = customization->b4x4.icopy;
		agmv->custom.b4x4.pcopy = customization->b4x4.pcopy;
		agmv->custom.b4x4.mv    = customization->b4x4.mv;
		agmv->custom.b4x4.pmv   = customization->b4x4.pmv;
		agmv->custom.b4x4.vq2   = customization->b4x4.vq2;
		agmv->custom.b4x4.vq4   = customization->b4x4.vq4;
		
		agmv->custom.b8x4.fill  = customization->b8x4.fill;
		agmv->custom.b8x4.copy  = customization->b8x4.copy;
		agmv->custom.b8x4.icopy = customization->b8x4.icopy;
		agmv->custom.b8x4.pcopy = customization->b8x4.pcopy;
		agmv->custom.b8x4.mv    = customization->b8x4.mv;
		agmv->custom.b8x4.pmv   = customization->b8x4.pmv;
		agmv->custom.b8x4.vq2   = customization->b8x4.vq2;
		agmv->custom.b8x4.vq4   = customization->b8x4.vq4;
		
		agmv->custom.b2x4.fill  = customization->b2x4.fill;
		agmv->custom.b2x4.copy  = customization->b2x4.copy;
		agmv->custom.b2x4.icopy = customization->b2x4.icopy;
		agmv->custom.b2x4.pcopy = customization->b2x4.pcopy;
		agmv->custom.b2x4.mv    = customization->b2x4.mv;
		agmv->custom.b2x4.pmv   = customization->b2x4.pmv;
		agmv->custom.b2x4.vq2   = customization->b2x4.vq2;
		agmv->custom.b2x4.vq4   = customization->b2x4.vq4;
		
		agmv->custom.b8x8.fill  = customization->b8x8.fill;
		agmv->custom.b8x8.copy  = customization->b8x8.copy;
		agmv->custom.b8x8.icopy = customization->b8x8.icopy;
		agmv->custom.b8x8.pcopy = customization->b8x8.pcopy;
		agmv->custom.b8x8.mv    = customization->b8x8.mv;
		agmv->custom.b8x8.pmv   = customization->b8x8.pmv;
		agmv->custom.b8x8.vq2   = customization->b8x8.vq2;
		agmv->custom.b8x8.vq4   = customization->b8x8.vq4;
	}
	
	if(quality == AGMV_LOW_QUALITY || quality == AGMV_MID_QUALITY){
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
	
	switch(opt){
		case AGMV_OPT_I:
		case AGMV_OPT_II:
		case AGMV_OPT_III:
		case AGMV_OPT_IV:
		case AGMV_OPT_ANIM:{
			AGMV_SetLeniency(agmv,0.2282);
		}break;
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
		default:{
			AGMV_SetLeniency(agmv,0.2282);
		}break;
	}
	
	char* ext = AGIDL_GetImgExtension(img_type);
	
	if(opt != AGMV_OPT_IV && opt != AGMV_OPT_GBA_IV && opt != AGMV_OPT_PSX_IV && opt != AGMV_OPT_NDS_IV){
		u32* colorgram = (u32*)malloc(sizeof(u32)*max_clr+5);
		u32* histogram = (u32*)malloc(sizeof(u32)*max_clr+5);
		
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

		AGMV_LoadHistogramImageData(dir,basename,ext,start_frame,end_frame,size,histogram,colorgram,img_type,quality);
		AGMV_BubbleSort(histogram,colorgram,max_clr);
		AGMV_DistributePaletteData(colorgram,pal,palette0,palette1,max_clr,opt,quality);
		
		quality = org_quality;
		
		free(colorgram);
		free(histogram);
	}
	
	FILE* file = fopen(filename,"wb");
	
	AGMV_SetICP0(agmv,palette0);
	AGMV_SetICP1(agmv,palette1);
	
	AGMV_Printf("Encoding AGMV Header...\n");
	AGMV_EncodeHeader(file,agmv);
	AGMV_Printf("Encoded AGMV Header...\n");
	
	u32* interp = (u32*)malloc(sizeof(u32)*AGMV_GetWidth(agmv)*AGMV_GetHeight(agmv));
	char filename1[60],filename2[60],filename3[60],filename4[60];
	
	for(i = start_frame; i <= end_frame;){
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
				AGMV_Printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_BMP* frame1 = AGIDL_LoadBMP(filename1);
				AGIDL_BMP* frame2 = AGIDL_LoadBMP(filename2);
				AGIDL_BMP* frame3 = AGIDL_LoadBMP(filename3);
				AGIDL_BMP* frame4 = AGIDL_LoadBMP(filename4);
				AGMV_Printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				
				AGIDL_ColorConvertBMP(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertBMP(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertBMP(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertBMP(frame4,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertBMP(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertBMP(frame2,AGIDL_RGB_555);
					AGIDL_ColorConvertBMP(frame3,AGIDL_RGB_555);
					AGIDL_ColorConvertBMP(frame4,AGIDL_RGB_555);
					
					AGIDL_ColorConvertBMP(frame1,AGIDL_RGB_888);
					AGIDL_ColorConvertBMP(frame2,AGIDL_RGB_888);
					AGIDL_ColorConvertBMP(frame3,AGIDL_RGB_888);
					AGIDL_ColorConvertBMP(frame4,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_BMPGetWidth(frame1), h = AGIDL_BMPGetHeight(frame1);
					AGIDL_FastScaleBMP(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}		
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_BMPGetWidth(frame1), h = AGIDL_BMPGetHeight(frame1);
					AGIDL_FastScaleBMP(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame2,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame3,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame4,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					AGIDL_FlipHorzBMP(frame1);
					AGIDL_FlipHorzBMP(frame2);
					AGIDL_FlipHorzBMP(frame3);
					AGIDL_FlipHorzBMP(frame4);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_BMPGetWidth(frame1), h = AGIDL_BMPGetHeight(frame1);
					AGIDL_FastScaleBMP(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);

					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
						AGMV_DitherImageData(frame2->pixels.pix32,width,height);
						AGMV_DitherImageData(frame3->pixels.pix32,width,height);
						AGMV_DitherImageData(frame4->pixels.pix32,width,height);
					}
				}
				
				w = AGIDL_BMPGetWidth(frame2), h = AGIDL_BMPGetHeight(frame2);
				
				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_I && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_I && opt != AGMV_OPT_NDS_II){

					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
					
					f32 ratio = AGMV_CompareFrameSimilarity(frame2->pixels.pix32,frame3->pixels.pix32,w,h);
					
					if(ratio >= AGMV_GetLeniency(agmv)){
						AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);
						
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
						AGMV_EncodeFrame(file,agmv,interp);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						
						num_of_frames_encoded += 3; i += 4;
					}
					else{
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
					
					f32 ratio = AGMV_CompareFrameSimilarity(frame1->pixels.pix32,frame2->pixels.pix32,w,h);
					
					if(ratio >= AGMV_GetLeniency(agmv)){
						AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);
						
						AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,interp);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);

						num_of_frames_encoded++; i += 2;
					}
					else{
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}
					
				    AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeBMP(frame1);
				AGIDL_FreeBMP(frame2);
				AGIDL_FreeBMP(frame3);
				AGIDL_FreeBMP(frame4);
			}break;
			case AGIDL_IMG_TGA:{
				AGMV_Printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_TGA* frame1 = AGIDL_LoadTGA(filename1);
				AGIDL_TGA* frame2 = AGIDL_LoadTGA(filename2);
				AGIDL_TGA* frame3 = AGIDL_LoadTGA(filename3);
				AGIDL_TGA* frame4 = AGIDL_LoadTGA(filename4);
				AGMV_Printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				
				AGIDL_ColorConvertTGA(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertTGA(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertTGA(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertTGA(frame4,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertTGA(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertTGA(frame2,AGIDL_RGB_555);
					AGIDL_ColorConvertTGA(frame3,AGIDL_RGB_555);
					AGIDL_ColorConvertTGA(frame4,AGIDL_RGB_555);
					
					AGIDL_ColorConvertTGA(frame1,AGIDL_RGB_888);
					AGIDL_ColorConvertTGA(frame2,AGIDL_RGB_888);
					AGIDL_ColorConvertTGA(frame3,AGIDL_RGB_888);
					AGIDL_ColorConvertTGA(frame4,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_TGAGetWidth(frame1), h = AGIDL_TGAGetHeight(frame1);
					AGIDL_FastScaleTGA(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTGA(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTGA(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTGA(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}		
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_TGAGetWidth(frame1), h = AGIDL_TGAGetHeight(frame1);
					AGIDL_FastScaleTGA(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTGA(frame2,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTGA(frame3,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTGA(frame4,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_TGAGetWidth(frame1), h = AGIDL_TGAGetHeight(frame1);
					AGIDL_FastScaleTGA(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTGA(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTGA(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTGA(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
						AGMV_DitherImageData(frame2->pixels.pix32,width,height);
						AGMV_DitherImageData(frame3->pixels.pix32,width,height);
						AGMV_DitherImageData(frame4->pixels.pix32,width,height);
					}
				}
				
				w = AGIDL_TGAGetWidth(frame2), h = AGIDL_TGAGetHeight(frame2);
				
				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_I && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_I && opt != AGMV_OPT_NDS_II){

					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
					
					f32 ratio = AGMV_CompareFrameSimilarity(frame2->pixels.pix32,frame3->pixels.pix32,w,h);
					
					if(ratio >= AGMV_GetLeniency(agmv)){
						AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);
						
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
						AGMV_EncodeFrame(file,agmv,interp);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						
						num_of_frames_encoded += 3; i += 4;
					}
					else{
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
					
					f32 ratio = AGMV_CompareFrameSimilarity(frame1->pixels.pix32,frame2->pixels.pix32,w,h);
					
					if(ratio >= AGMV_GetLeniency(agmv)){
						AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);
						
						AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,interp);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);

						num_of_frames_encoded++; i += 2;
					}
					else{
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeTGA(frame1);
				AGIDL_FreeTGA(frame2);
				AGIDL_FreeTGA(frame3);
				AGIDL_FreeTGA(frame4);
			}break;
			case AGIDL_IMG_TIM:{
				AGMV_Printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_TIM* frame1 = AGIDL_LoadTIM(filename1);
				AGIDL_TIM* frame2 = AGIDL_LoadTIM(filename2);
				AGIDL_TIM* frame3 = AGIDL_LoadTIM(filename3);
				AGIDL_TIM* frame4 = AGIDL_LoadTIM(filename4);
				AGMV_Printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				
				AGIDL_ColorConvertTIM(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertTIM(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertTIM(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertTIM(frame4,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertTIM(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertTIM(frame2,AGIDL_RGB_555);
					AGIDL_ColorConvertTIM(frame3,AGIDL_RGB_555);
					AGIDL_ColorConvertTIM(frame4,AGIDL_RGB_555);
					
					AGIDL_ColorConvertTIM(frame1,AGIDL_RGB_888);
					AGIDL_ColorConvertTIM(frame2,AGIDL_RGB_888);
					AGIDL_ColorConvertTIM(frame3,AGIDL_RGB_888);
					AGIDL_ColorConvertTIM(frame4,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_TIMGetWidth(frame1), h = AGIDL_TIMGetHeight(frame1);
					AGIDL_FastScaleTIM(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTIM(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTIM(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTIM(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}		
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_TIMGetWidth(frame1), h = AGIDL_TIMGetHeight(frame1);
					AGIDL_FastScaleTIM(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTIM(frame2,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTIM(frame3,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTIM(frame4,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_TIMGetWidth(frame1), h = AGIDL_TIMGetHeight(frame1);
					AGIDL_FastScaleTIM(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTIM(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTIM(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTIM(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
						AGMV_DitherImageData(frame2->pixels.pix32,width,height);
						AGMV_DitherImageData(frame3->pixels.pix32,width,height);
						AGMV_DitherImageData(frame4->pixels.pix32,width,height);
					}
				}
				
				w = AGIDL_TIMGetWidth(frame2), h = AGIDL_TIMGetHeight(frame2);

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_I && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_I && opt != AGMV_OPT_NDS_II){

					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
					
					f32 ratio = AGMV_CompareFrameSimilarity(frame2->pixels.pix32,frame3->pixels.pix32,w,h);
					
					if(ratio >= AGMV_GetLeniency(agmv)){
						AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);
						
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
						AGMV_EncodeFrame(file,agmv,interp);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						
						num_of_frames_encoded += 3; i += 4;
					}
					else{
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
					
					f32 ratio = AGMV_CompareFrameSimilarity(frame1->pixels.pix32,frame2->pixels.pix32,w,h);
					
					if(ratio >= AGMV_GetLeniency(agmv)){
						AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);
						
						AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,interp);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);

						num_of_frames_encoded++; i += 2;
					}
					else{
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeTIM(frame1);
				AGIDL_FreeTIM(frame2);
				AGIDL_FreeTIM(frame3);
				AGIDL_FreeTIM(frame4);
			}break;
			case AGIDL_IMG_PCX:{
				AGMV_Printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_PCX* frame1 = AGIDL_LoadPCX(filename1);
				AGIDL_PCX* frame2 = AGIDL_LoadPCX(filename2);
				AGIDL_PCX* frame3 = AGIDL_LoadPCX(filename3);
				AGIDL_PCX* frame4 = AGIDL_LoadPCX(filename4);
				AGMV_Printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				
				AGIDL_ColorConvertPCX(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertPCX(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertPCX(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertPCX(frame4,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertPCX(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertPCX(frame2,AGIDL_RGB_555);
					AGIDL_ColorConvertPCX(frame3,AGIDL_RGB_555);
					AGIDL_ColorConvertPCX(frame4,AGIDL_RGB_555);
					
					AGIDL_ColorConvertPCX(frame1,AGIDL_RGB_888);
					AGIDL_ColorConvertPCX(frame2,AGIDL_RGB_888);
					AGIDL_ColorConvertPCX(frame3,AGIDL_RGB_888);
					AGIDL_ColorConvertPCX(frame4,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_PCXGetWidth(frame1), h = AGIDL_PCXGetHeight(frame1);
					AGIDL_FastScalePCX(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePCX(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePCX(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePCX(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}		
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_PCXGetWidth(frame1), h = AGIDL_PCXGetHeight(frame1);
					AGIDL_FastScalePCX(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePCX(frame2,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePCX(frame3,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePCX(frame4,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_PCXGetWidth(frame1), h = AGIDL_PCXGetHeight(frame1);
					AGIDL_FastScalePCX(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePCX(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePCX(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePCX(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
						AGMV_DitherImageData(frame2->pixels.pix32,width,height);
						AGMV_DitherImageData(frame3->pixels.pix32,width,height);
						AGMV_DitherImageData(frame4->pixels.pix32,width,height);
					}
				}
				
				w = AGIDL_PCXGetWidth(frame2), h = AGIDL_PCXGetHeight(frame2);
				
				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_I && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_I && opt != AGMV_OPT_NDS_II){

					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
					
					f32 ratio = AGMV_CompareFrameSimilarity(frame2->pixels.pix32,frame3->pixels.pix32,w,h);
					
					if(ratio >= AGMV_GetLeniency(agmv)){
						AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);
						
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
						AGMV_EncodeFrame(file,agmv,interp);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						
						num_of_frames_encoded += 3; i += 4;
					}
					else{
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
					
					f32 ratio = AGMV_CompareFrameSimilarity(frame1->pixels.pix32,frame2->pixels.pix32,w,h);
					
					if(ratio >= AGMV_GetLeniency(agmv)){
						AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);
						
						AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,interp);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);

						num_of_frames_encoded++; i += 2;
					}
					else{
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreePCX(frame1);
				AGIDL_FreePCX(frame2);
				AGIDL_FreePCX(frame3);
				AGIDL_FreePCX(frame4);
			}break;
			case AGIDL_IMG_LMP:{
				AGMV_Printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_LMP* frame1 = AGIDL_LoadLMP(filename1);
				AGIDL_LMP* frame2 = AGIDL_LoadLMP(filename2);
				AGIDL_LMP* frame3 = AGIDL_LoadLMP(filename3);
				AGIDL_LMP* frame4 = AGIDL_LoadLMP(filename4);
				AGMV_Printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				
				AGIDL_ColorConvertLMP(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertLMP(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertLMP(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertLMP(frame4,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertLMP(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertLMP(frame2,AGIDL_RGB_555);
					AGIDL_ColorConvertLMP(frame3,AGIDL_RGB_555);
					AGIDL_ColorConvertLMP(frame4,AGIDL_RGB_555);
					
					AGIDL_ColorConvertLMP(frame1,AGIDL_RGB_888);
					AGIDL_ColorConvertLMP(frame2,AGIDL_RGB_888);
					AGIDL_ColorConvertLMP(frame3,AGIDL_RGB_888);
					AGIDL_ColorConvertLMP(frame4,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_LMPGetWidth(frame1), h = AGIDL_LMPGetHeight(frame1);
					AGIDL_FastScaleLMP(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLMP(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLMP(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLMP(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}		
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_LMPGetWidth(frame1), h = AGIDL_LMPGetHeight(frame1);
					AGIDL_FastScaleLMP(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLMP(frame2,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLMP(frame3,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLMP(frame4,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_LMPGetWidth(frame1), h = AGIDL_LMPGetHeight(frame1);
					AGIDL_FastScaleLMP(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLMP(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLMP(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLMP(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
						AGMV_DitherImageData(frame2->pixels.pix32,width,height);
						AGMV_DitherImageData(frame3->pixels.pix32,width,height);
						AGMV_DitherImageData(frame4->pixels.pix32,width,height);
					}
				}
				
				w = AGIDL_LMPGetWidth(frame2), h = AGIDL_LMPGetHeight(frame2);
				
				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_I && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_I && opt != AGMV_OPT_NDS_II){

					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
					
					f32 ratio = AGMV_CompareFrameSimilarity(frame2->pixels.pix32,frame3->pixels.pix32,w,h);
					
					if(ratio >= AGMV_GetLeniency(agmv)){
						AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);
						
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
						AGMV_EncodeFrame(file,agmv,interp);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						
						num_of_frames_encoded += 3; i += 4;
					}
					else{
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
					
					f32 ratio = AGMV_CompareFrameSimilarity(frame1->pixels.pix32,frame2->pixels.pix32,w,h);
					
					if(ratio >= AGMV_GetLeniency(agmv)){
						AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);
						
						AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,interp);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);

						num_of_frames_encoded++; i += 2;
					}
					else{
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeLMP(frame1);
				AGIDL_FreeLMP(frame2);
				AGIDL_FreeLMP(frame3);
				AGIDL_FreeLMP(frame4);
			}break;
			case AGIDL_IMG_PVR:{
				AGMV_Printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_PVR* frame1 = AGIDL_LoadPVR(filename1);
				AGIDL_PVR* frame2 = AGIDL_LoadPVR(filename2);
				AGIDL_PVR* frame3 = AGIDL_LoadPVR(filename3);
				AGIDL_PVR* frame4 = AGIDL_LoadPVR(filename4);
				AGMV_Printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				
				AGIDL_ColorConvertPVR(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertPVR(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertPVR(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertPVR(frame4,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertPVR(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertPVR(frame2,AGIDL_RGB_555);
					AGIDL_ColorConvertPVR(frame3,AGIDL_RGB_555);
					AGIDL_ColorConvertPVR(frame4,AGIDL_RGB_555);
					
					AGIDL_ColorConvertPVR(frame1,AGIDL_RGB_888);
					AGIDL_ColorConvertPVR(frame2,AGIDL_RGB_888);
					AGIDL_ColorConvertPVR(frame3,AGIDL_RGB_888);
					AGIDL_ColorConvertPVR(frame4,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_PVRGetWidth(frame1), h = AGIDL_PVRGetHeight(frame1);
					AGIDL_FastScalePVR(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePVR(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePVR(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePVR(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}		
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_PVRGetWidth(frame1), h = AGIDL_PVRGetHeight(frame1);
					AGIDL_FastScalePVR(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePVR(frame2,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePVR(frame3,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePVR(frame4,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_PVRGetWidth(frame1), h = AGIDL_PVRGetHeight(frame1);
					AGIDL_FastScalePVR(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePVR(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePVR(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePVR(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
						AGMV_DitherImageData(frame2->pixels.pix32,width,height);
						AGMV_DitherImageData(frame3->pixels.pix32,width,height);
						AGMV_DitherImageData(frame4->pixels.pix32,width,height);
					}
				}
				
				w = AGIDL_PVRGetWidth(frame2), h = AGIDL_PVRGetHeight(frame2);
				
				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_I && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_I && opt != AGMV_OPT_NDS_II){

					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
					
					f32 ratio = AGMV_CompareFrameSimilarity(frame2->pixels.pix32,frame3->pixels.pix32,w,h);
					
					if(ratio >= AGMV_GetLeniency(agmv)){
						AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);
						
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
						AGMV_EncodeFrame(file,agmv,interp);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						
						num_of_frames_encoded += 3; i += 4;
					}
					else{
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
					
					f32 ratio = AGMV_CompareFrameSimilarity(frame1->pixels.pix32,frame2->pixels.pix32,w,h);
					
					if(ratio >= AGMV_GetLeniency(agmv)){
						AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);
						
						AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,interp);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);

						num_of_frames_encoded++; i += 2;
					}
					else{
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreePVR(frame1);
				AGIDL_FreePVR(frame2);
				AGIDL_FreePVR(frame3);
				AGIDL_FreePVR(frame4);
			}break;
			case AGIDL_IMG_GXT:{
				AGMV_Printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_GXT* frame1 = AGIDL_LoadGXT(filename1);
				AGIDL_GXT* frame2 = AGIDL_LoadGXT(filename2);
				AGIDL_GXT* frame3 = AGIDL_LoadGXT(filename3);
				AGIDL_GXT* frame4 = AGIDL_LoadGXT(filename4);
				AGMV_Printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				
				AGIDL_ColorConvertGXT(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertGXT(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertGXT(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertGXT(frame4,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertGXT(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertGXT(frame2,AGIDL_RGB_555);
					AGIDL_ColorConvertGXT(frame3,AGIDL_RGB_555);
					AGIDL_ColorConvertGXT(frame4,AGIDL_RGB_555);
					
					AGIDL_ColorConvertGXT(frame1,AGIDL_RGB_888);
					AGIDL_ColorConvertGXT(frame2,AGIDL_RGB_888);
					AGIDL_ColorConvertGXT(frame3,AGIDL_RGB_888);
					AGIDL_ColorConvertGXT(frame4,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_GXTGetWidth(frame1), h = AGIDL_GXTGetHeight(frame1);
					AGIDL_FastScaleGXT(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleGXT(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleGXT(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleGXT(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}		
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_GXTGetWidth(frame1), h = AGIDL_GXTGetHeight(frame1);
					AGIDL_FastScaleGXT(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleGXT(frame2,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleGXT(frame3,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleGXT(frame4,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_GXTGetWidth(frame1), h = AGIDL_GXTGetHeight(frame1);
					AGIDL_FastScaleGXT(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleGXT(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleGXT(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleGXT(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
						AGMV_DitherImageData(frame2->pixels.pix32,width,height);
						AGMV_DitherImageData(frame3->pixels.pix32,width,height);
						AGMV_DitherImageData(frame4->pixels.pix32,width,height);
					}
				}
				
				w = AGIDL_GXTGetWidth(frame2), h = AGIDL_GXTGetHeight(frame2);
				
				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_I && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_I && opt != AGMV_OPT_NDS_II){

					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
					
					f32 ratio = AGMV_CompareFrameSimilarity(frame2->pixels.pix32,frame3->pixels.pix32,w,h);
					
					if(ratio >= AGMV_GetLeniency(agmv)){
						AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);
						
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
						AGMV_EncodeFrame(file,agmv,interp);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						
						num_of_frames_encoded += 3; i += 4;
					}
					else{
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
					
					f32 ratio = AGMV_CompareFrameSimilarity(frame1->pixels.pix32,frame2->pixels.pix32,w,h);
					
					if(ratio >= AGMV_GetLeniency(agmv)){
						AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);
						
						AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,interp);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);

						num_of_frames_encoded++; i += 2;
					}
					else{
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeGXT(frame1);
				AGIDL_FreeGXT(frame2);
				AGIDL_FreeGXT(frame3);
				AGIDL_FreeGXT(frame4);
			}break;
			case AGIDL_IMG_BTI:{
				AGMV_Printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_BTI* frame1 = AGIDL_LoadBTI(filename1);
				AGIDL_BTI* frame2 = AGIDL_LoadBTI(filename2);
				AGIDL_BTI* frame3 = AGIDL_LoadBTI(filename3);
				AGIDL_BTI* frame4 = AGIDL_LoadBTI(filename4);
				AGMV_Printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				
				AGIDL_ColorConvertBTI(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertBTI(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertBTI(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertBTI(frame4,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertBTI(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertBTI(frame2,AGIDL_RGB_555);
					AGIDL_ColorConvertBTI(frame3,AGIDL_RGB_555);
					AGIDL_ColorConvertBTI(frame4,AGIDL_RGB_555);
					
					AGIDL_ColorConvertBTI(frame1,AGIDL_RGB_888);
					AGIDL_ColorConvertBTI(frame2,AGIDL_RGB_888);
					AGIDL_ColorConvertBTI(frame3,AGIDL_RGB_888);
					AGIDL_ColorConvertBTI(frame4,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_BTIGetWidth(frame1), h = AGIDL_BTIGetHeight(frame1);
					AGIDL_FastScaleBTI(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBTI(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBTI(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBTI(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}		
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_BTIGetWidth(frame1), h = AGIDL_BTIGetHeight(frame1);
					AGIDL_FastScaleBTI(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBTI(frame2,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBTI(frame3,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBTI(frame4,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_BTIGetWidth(frame1), h = AGIDL_BTIGetHeight(frame1);
					AGIDL_FastScaleBTI(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBTI(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBTI(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBTI(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
						AGMV_DitherImageData(frame2->pixels.pix32,width,height);
						AGMV_DitherImageData(frame3->pixels.pix32,width,height);
						AGMV_DitherImageData(frame4->pixels.pix32,width,height);
					}
				}
				
				w = AGIDL_BTIGetWidth(frame2), h = AGIDL_BTIGetHeight(frame2);
				
				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_I && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_I && opt != AGMV_OPT_NDS_II){

					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
					
					f32 ratio = AGMV_CompareFrameSimilarity(frame2->pixels.pix32,frame3->pixels.pix32,w,h);
					
					if(ratio >= AGMV_GetLeniency(agmv)){
						AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);
						
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
						AGMV_EncodeFrame(file,agmv,interp);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						
						num_of_frames_encoded += 3; i += 4;
					}
					else{
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
					
					f32 ratio = AGMV_CompareFrameSimilarity(frame1->pixels.pix32,frame2->pixels.pix32,w,h);
					
					if(ratio >= AGMV_GetLeniency(agmv)){
						AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);
						
						AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,interp);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);

						num_of_frames_encoded++; i += 2;
					}
					else{
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeBTI(frame1);
				AGIDL_FreeBTI(frame2);
				AGIDL_FreeBTI(frame3);
				AGIDL_FreeBTI(frame4);
			}break;
			case AGIDL_IMG_3DF:{
				AGMV_Printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_3DF* frame1 = AGIDL_Load3DF(filename1);
				AGIDL_3DF* frame2 = AGIDL_Load3DF(filename2);
				AGIDL_3DF* frame3 = AGIDL_Load3DF(filename3);
				AGIDL_3DF* frame4 = AGIDL_Load3DF(filename4);
				AGMV_Printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				
				AGIDL_ColorConvert3DF(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvert3DF(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvert3DF(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvert3DF(frame4,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvert3DF(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvert3DF(frame2,AGIDL_RGB_555);
					AGIDL_ColorConvert3DF(frame3,AGIDL_RGB_555);
					AGIDL_ColorConvert3DF(frame4,AGIDL_RGB_555);
					
					AGIDL_ColorConvert3DF(frame1,AGIDL_RGB_888);
					AGIDL_ColorConvert3DF(frame2,AGIDL_RGB_888);
					AGIDL_ColorConvert3DF(frame3,AGIDL_RGB_888);
					AGIDL_ColorConvert3DF(frame4,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_3DFGetWidth(frame1), h = AGIDL_3DFGetHeight(frame1);
					AGIDL_FastScale3DF(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScale3DF(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScale3DF(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScale3DF(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}		
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_3DFGetWidth(frame1), h = AGIDL_3DFGetHeight(frame1);
					AGIDL_FastScale3DF(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScale3DF(frame2,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScale3DF(frame3,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScale3DF(frame4,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_3DFGetWidth(frame1), h = AGIDL_3DFGetHeight(frame1);
					AGIDL_FastScale3DF(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScale3DF(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScale3DF(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScale3DF(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
						AGMV_DitherImageData(frame2->pixels.pix32,width,height);
						AGMV_DitherImageData(frame3->pixels.pix32,width,height);
						AGMV_DitherImageData(frame4->pixels.pix32,width,height);
					}
				}
				
				w = AGIDL_3DFGetWidth(frame2), h = AGIDL_3DFGetHeight(frame2);

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_I && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_I && opt != AGMV_OPT_NDS_II){

					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
					
					f32 ratio = AGMV_CompareFrameSimilarity(frame2->pixels.pix32,frame3->pixels.pix32,w,h);
					
					if(ratio >= AGMV_GetLeniency(agmv)){
						AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);
						
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
						AGMV_EncodeFrame(file,agmv,interp);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						
						num_of_frames_encoded += 3; i += 4;
					}
					else{
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
					
					f32 ratio = AGMV_CompareFrameSimilarity(frame1->pixels.pix32,frame2->pixels.pix32,w,h);
					
					if(ratio >= AGMV_GetLeniency(agmv)){
						AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);
						
						AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,interp);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);

						num_of_frames_encoded++; i += 2;
					}
					else{
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_Free3DF(frame1);
				AGIDL_Free3DF(frame2);
				AGIDL_Free3DF(frame3);
				AGIDL_Free3DF(frame4);
			}break;
			case AGIDL_IMG_PPM:{
				AGMV_Printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_PPM* frame1 = AGIDL_LoadPPM(filename1);
				AGIDL_PPM* frame2 = AGIDL_LoadPPM(filename2);
				AGIDL_PPM* frame3 = AGIDL_LoadPPM(filename3);
				AGIDL_PPM* frame4 = AGIDL_LoadPPM(filename4);
				AGMV_Printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				
				AGIDL_ColorConvertPPM(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertPPM(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertPPM(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertPPM(frame4,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertPPM(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertPPM(frame2,AGIDL_RGB_555);
					AGIDL_ColorConvertPPM(frame3,AGIDL_RGB_555);
					AGIDL_ColorConvertPPM(frame4,AGIDL_RGB_555);
					
					AGIDL_ColorConvertPPM(frame1,AGIDL_RGB_888);
					AGIDL_ColorConvertPPM(frame2,AGIDL_RGB_888);
					AGIDL_ColorConvertPPM(frame3,AGIDL_RGB_888);
					AGIDL_ColorConvertPPM(frame4,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_PPMGetWidth(frame1), h = AGIDL_PPMGetHeight(frame1);
					AGIDL_FastScalePPM(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePPM(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePPM(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePPM(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}		
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_PPMGetWidth(frame1), h = AGIDL_PPMGetHeight(frame1);
					AGIDL_FastScalePPM(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePPM(frame2,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePPM(frame3,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePPM(frame4,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_PPMGetWidth(frame1), h = AGIDL_PPMGetHeight(frame1);
					AGIDL_FastScalePPM(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePPM(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePPM(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePPM(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
						AGMV_DitherImageData(frame2->pixels.pix32,width,height);
						AGMV_DitherImageData(frame3->pixels.pix32,width,height);
						AGMV_DitherImageData(frame4->pixels.pix32,width,height);
					}
				}
				
				w = AGIDL_PPMGetWidth(frame2), h = AGIDL_PPMGetHeight(frame2);
				
				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_I && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_I && opt != AGMV_OPT_NDS_II){

					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
					
					f32 ratio = AGMV_CompareFrameSimilarity(frame2->pixels.pix32,frame3->pixels.pix32,w,h);
					
					if(ratio >= AGMV_GetLeniency(agmv)){
						AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);
						
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
						AGMV_EncodeFrame(file,agmv,interp);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						
						num_of_frames_encoded += 3; i += 4;
					}
					else{
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
					
					f32 ratio = AGMV_CompareFrameSimilarity(frame1->pixels.pix32,frame2->pixels.pix32,w,h);
					
					if(ratio >= AGMV_GetLeniency(agmv)){
						AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);
						
						AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,interp);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);

						num_of_frames_encoded++; i += 2;
					}
					else{
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreePPM(frame1);
				AGIDL_FreePPM(frame2);
				AGIDL_FreePPM(frame3);
				AGIDL_FreePPM(frame4);
			}break;
			case AGIDL_IMG_LBM:{
				AGMV_Printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_LBM* frame1 = AGIDL_LoadLBM(filename1);
				AGIDL_LBM* frame2 = AGIDL_LoadLBM(filename2);
				AGIDL_LBM* frame3 = AGIDL_LoadLBM(filename3);
				AGIDL_LBM* frame4 = AGIDL_LoadLBM(filename4);
				AGMV_Printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				
				AGIDL_ColorConvertLBM(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertLBM(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertLBM(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertLBM(frame4,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertLBM(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertLBM(frame2,AGIDL_RGB_555);
					AGIDL_ColorConvertLBM(frame3,AGIDL_RGB_555);
					AGIDL_ColorConvertLBM(frame4,AGIDL_RGB_555);
					
					AGIDL_ColorConvertLBM(frame1,AGIDL_RGB_888);
					AGIDL_ColorConvertLBM(frame2,AGIDL_RGB_888);
					AGIDL_ColorConvertLBM(frame3,AGIDL_RGB_888);
					AGIDL_ColorConvertLBM(frame4,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_LBMGetWidth(frame1), h = AGIDL_LBMGetHeight(frame1);
					AGIDL_FastScaleLBM(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLBM(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLBM(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLBM(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}		
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_LBMGetWidth(frame1), h = AGIDL_LBMGetHeight(frame1);
					AGIDL_FastScaleLBM(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLBM(frame2,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLBM(frame3,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLBM(frame4,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_LBMGetWidth(frame1), h = AGIDL_LBMGetHeight(frame1);
					AGIDL_FastScaleLBM(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLBM(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLBM(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLBM(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
						AGMV_DitherImageData(frame2->pixels.pix32,width,height);
						AGMV_DitherImageData(frame3->pixels.pix32,width,height);
						AGMV_DitherImageData(frame4->pixels.pix32,width,height);
					}
				}
				
				w = AGIDL_LBMGetWidth(frame2), h = AGIDL_LBMGetHeight(frame2);
				
				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_I && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_I && opt != AGMV_OPT_NDS_II){

					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
					
					f32 ratio = AGMV_CompareFrameSimilarity(frame2->pixels.pix32,frame3->pixels.pix32,w,h);
					
					if(ratio >= AGMV_GetLeniency(agmv)){
						AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);
						
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
						AGMV_EncodeFrame(file,agmv,interp);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
						
						num_of_frames_encoded += 3; i += 4;
					}
					else{
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
					
					f32 ratio = AGMV_CompareFrameSimilarity(frame1->pixels.pix32,frame2->pixels.pix32,w,h);
					
					if(ratio >= AGMV_GetLeniency(agmv)){
						AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);
						
						AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,interp);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);

						num_of_frames_encoded++; i += 2;
					}
					else{
						AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
						AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
						AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
						num_of_frames_encoded++; i++;
					}
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
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
	free(interp);
	free(ext);
	DestroyAGMV(agmv); 
	
	if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
		FILE* file = fopen(filename,"rb");
		fseek(file,0,SEEK_END);
		u32 file_size = ftell(file);
		fseek(file,0,SEEK_SET);
		u8* data = (u8*)malloc(sizeof(u8)*file_size);
		fread(data,1,file_size,file);
		fclose(file);
		
		FILE* out = fopen("GBA_GEN_AGMV.h","w");
		
		fprintf(out,"#ifndef GBA_GEN_AGMV_H\n");
		fprintf(out,"#define GBA_GEN_AGMV_H\n\n");
		fprintf(out,"const unsigned char GBA_AGMV_FILE[%ld] = {\n",file_size);
		
		int i;
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
	u32 i, palette0[256], palette1[256], num_of_frames_encoded = 0, w, h, max_clr, size = width*height, AGMV_GBA_W, AGMV_GBA_H;
	u32 sample_size, adjusted_num_of_frames = end_frame-start_frame;
	u32 pal[512];
	AGMV_QUALITY org_quality;
	
	org_quality = quality;

	AGMV_SetOPT(agmv,opt);
	AGMV_SetCompression(agmv,compression);
	AGMV_SetQuality(agmv,quality);

	if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
		agmv->max_diff = AGMV_MAX_15BPP_DIFF;
	}
	else{
		agmv->max_diff = AGMV_MAX_DIFF;
	}
	
	if(quality == AGMV_LOW_QUALITY || quality == AGMV_MID_QUALITY){
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
	
	switch(opt){
		case AGMV_OPT_I:{
			AGMV_SetLeniency(agmv,0.2282); adjusted_num_of_frames /= 2;
		}break;
		case AGMV_OPT_II:{
			AGMV_SetLeniency(agmv,0.1282); adjusted_num_of_frames /= 2;
		}break;
		case AGMV_OPT_III:{
			AGMV_SetLeniency(agmv,0.2282); adjusted_num_of_frames *= 0.75;
		}break;
		case AGMV_OPT_IV:{
			AGMV_SetLeniency(agmv,0.2282); adjusted_num_of_frames *= 0.75;
		}break;
		case AGMV_OPT_ANIM:{
			AGMV_SetLeniency(agmv,0.2282); adjusted_num_of_frames /= 2;
		}break;
		case AGMV_OPT_GBA_I:
		case AGMV_OPT_GBA_II:{
			AGMV_SetLeniency(agmv,0.0);
			
			AGMV_SetWidth(agmv,AGMV_GBA_W);
			AGMV_SetHeight(agmv,AGMV_GBA_H);
			
			free(agmv->frame->img_data);
			free(agmv->iframe->img_data);
			free(agmv->prev_frame->img_data);
			
			agmv->frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_GBA_W*AGMV_GBA_H);
			agmv->iframe->img_data = (u32*)malloc(sizeof(u32)*AGMV_GBA_W*AGMV_GBA_H);
			agmv->prev_frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_GBA_W*AGMV_GBA_H);
			
			adjusted_num_of_frames /= 2;
		}break;
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
			
			adjusted_num_of_frames *= 0.75f;
		}break;
		case AGMV_OPT_PSX_I:
		case AGMV_OPT_PSX_II:{
			AGMV_SetLeniency(agmv,0.0);
			
			AGMV_SetWidth(agmv,AGMV_PSX_W);
			AGMV_SetHeight(agmv,AGMV_PSX_H);
			
			free(agmv->frame->img_data);
			free(agmv->iframe->img_data);
			free(agmv->prev_frame->img_data);
			
			agmv->frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_PSX_W*AGMV_PSX_H);
			agmv->iframe->img_data = (u32*)malloc(sizeof(u32)*AGMV_PSX_W*AGMV_PSX_H);
			agmv->prev_frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_PSX_W*AGMV_PSX_H);
			
			adjusted_num_of_frames /= 2;
		}break;
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
			
			adjusted_num_of_frames *= 0.75f;
		}break;
		case AGMV_OPT_NDS_I:
		case AGMV_OPT_NDS_II:{
			AGMV_SetLeniency(agmv,0.0);
			
			AGMV_SetWidth(agmv,AGMV_NDS_W);
			AGMV_SetHeight(agmv,AGMV_NDS_H);
			
			free(agmv->frame->img_data);
			free(agmv->iframe->img_data);
			free(agmv->prev_frame->img_data);
			
			agmv->frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_NDS_W*AGMV_NDS_H);
			agmv->iframe->img_data = (u32*)malloc(sizeof(u32)*AGMV_NDS_W*AGMV_NDS_H);
			agmv->prev_frame->img_data = (u32*)malloc(sizeof(u32)*AGMV_NDS_W*AGMV_NDS_H);
			
			adjusted_num_of_frames /= 2;
		}break;
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
			
			adjusted_num_of_frames *= 0.75f;
		}break;
		default:{
			AGMV_SetLeniency(agmv,0.2282);
		}break;
	}
	
	char* ext = AGIDL_GetImgExtension(img_type);
	
	if(opt != AGMV_OPT_IV && opt != AGMV_OPT_GBA_IV && opt != AGMV_OPT_PSX_IV && opt != AGMV_OPT_NDS_IV){
		
		u32* colorgram = (u32*)malloc(sizeof(u32)*max_clr+5);
		u32* histogram = (u32*)malloc(sizeof(u32)*max_clr+5);

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
		
		AGMV_LoadHistogramImageData(dir,basename,ext,start_frame,end_frame,size,histogram,colorgram,img_type,quality);
		AGMV_BubbleSort(histogram,colorgram,max_clr);
		AGMV_DistributePaletteData(colorgram,pal,palette0,palette1,max_clr,opt,quality);
		
		quality = org_quality;
		
		free(colorgram);
		free(histogram);
	}
	
	sample_size = agmv->header.audio_size / (f32)adjusted_num_of_frames;
	
	agmv->audio_chunk->size = sample_size;
	agmv->audio_chunk->atsample = (u8*)malloc(sizeof(u8)*agmv->audio_chunk->size);
	
	if(AGMV_GetBitsPerSample(agmv) == 16){
		agmv->audio_track->pcm = (u16*)malloc(sizeof(u16)*agmv->audio_chunk->size);
	}
	else{
		agmv->audio_track->pcm8 = (u8*)malloc(sizeof(u8)*agmv->audio_chunk->size);
	}

	FILE* file = fopen(filename,"wb");
	
	AGMV_SetICP0(agmv,palette0);
	AGMV_SetICP1(agmv,palette1);
	
	AGMV_Printf("Encoding AGMV Header...\n");
	AGMV_EncodeHeader(file,agmv);
	AGMV_Printf("Encoded AGMV Header...\n");
	
	u32* interp = (u32*)malloc(sizeof(u32)*AGMV_GetWidth(agmv)*AGMV_GetHeight(agmv));
	char filename1[60],filename2[60],filename3[60],filename4[60];
	
	for(i = start_frame; i <= end_frame;){
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
				AGMV_Printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_BMP* frame1 = AGIDL_LoadBMP(filename1);
				AGIDL_BMP* frame2 = AGIDL_LoadBMP(filename2);
				AGIDL_BMP* frame3 = AGIDL_LoadBMP(filename3);
				AGIDL_BMP* frame4 = AGIDL_LoadBMP(filename4);
				AGMV_Printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				
				AGIDL_ColorConvertBMP(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertBMP(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertBMP(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertBMP(frame4,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertBMP(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertBMP(frame2,AGIDL_RGB_555);
					AGIDL_ColorConvertBMP(frame3,AGIDL_RGB_555);
					AGIDL_ColorConvertBMP(frame4,AGIDL_RGB_555);
					
					AGIDL_ColorConvertBMP(frame1,AGIDL_RGB_888);
					AGIDL_ColorConvertBMP(frame2,AGIDL_RGB_888);
					AGIDL_ColorConvertBMP(frame3,AGIDL_RGB_888);
					AGIDL_ColorConvertBMP(frame4,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_BMPGetWidth(frame1), h = AGIDL_BMPGetHeight(frame1);
					AGIDL_FastScaleBMP(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}		
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_BMPGetWidth(frame1), h = AGIDL_BMPGetHeight(frame1);
					AGIDL_FastScaleBMP(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame2,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame3,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame4,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					AGIDL_FlipHorzBMP(frame1);
					AGIDL_FlipHorzBMP(frame2);
					AGIDL_FlipHorzBMP(frame3);
					AGIDL_FlipHorzBMP(frame4);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_BMPGetWidth(frame1), h = AGIDL_BMPGetHeight(frame1);
					AGIDL_FastScaleBMP(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBMP(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
						AGMV_DitherImageData(frame2->pixels.pix32,width,height);
						AGMV_DitherImageData(frame3->pixels.pix32,width,height);
						AGMV_DitherImageData(frame4->pixels.pix32,width,height);
					}
				}
				
				w = AGIDL_BMPGetWidth(frame2), h = AGIDL_BMPGetHeight(frame2);
		
				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_I && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_I && opt != AGMV_OPT_NDS_II){

					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);					

					AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);
					
					AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
					AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
					AGMV_EncodeFrame(file,agmv,interp);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					
					num_of_frames_encoded += 3; i += 4;

					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);
					
					AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,interp);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);

					num_of_frames_encoded++; i += 2;
				
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeBMP(frame1);
				AGIDL_FreeBMP(frame2);
				AGIDL_FreeBMP(frame3);
				AGIDL_FreeBMP(frame4);
			}break;
			case AGIDL_IMG_TGA:{
				AGMV_Printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_TGA* frame1 = AGIDL_LoadTGA(filename1);
				AGIDL_TGA* frame2 = AGIDL_LoadTGA(filename2);
				AGIDL_TGA* frame3 = AGIDL_LoadTGA(filename3);
				AGIDL_TGA* frame4 = AGIDL_LoadTGA(filename4);
				AGMV_Printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				
				AGIDL_ColorConvertTGA(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertTGA(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertTGA(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertTGA(frame4,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertTGA(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertTGA(frame2,AGIDL_RGB_555);
					AGIDL_ColorConvertTGA(frame3,AGIDL_RGB_555);
					AGIDL_ColorConvertTGA(frame4,AGIDL_RGB_555);
					
					AGIDL_ColorConvertTGA(frame1,AGIDL_RGB_888);
					AGIDL_ColorConvertTGA(frame2,AGIDL_RGB_888);
					AGIDL_ColorConvertTGA(frame3,AGIDL_RGB_888);
					AGIDL_ColorConvertTGA(frame4,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_TGAGetWidth(frame1), h = AGIDL_TGAGetHeight(frame1);
					AGIDL_FastScaleTGA(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTGA(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTGA(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTGA(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}		
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_TGAGetWidth(frame1), h = AGIDL_TGAGetHeight(frame1);
					AGIDL_FastScaleTGA(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTGA(frame2,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTGA(frame3,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTGA(frame4,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_TGAGetWidth(frame1), h = AGIDL_TGAGetHeight(frame1);
					AGIDL_FastScaleTGA(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTGA(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTGA(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTGA(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
						AGMV_DitherImageData(frame2->pixels.pix32,width,height);
						AGMV_DitherImageData(frame3->pixels.pix32,width,height);
						AGMV_DitherImageData(frame4->pixels.pix32,width,height);
					}
				}
				
				w = AGIDL_TGAGetWidth(frame2), h = AGIDL_TGAGetHeight(frame2);
				
				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_I && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_I && opt != AGMV_OPT_NDS_II){

					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);
					
					AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
					AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
					AGMV_EncodeFrame(file,agmv,interp);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					
					num_of_frames_encoded += 3; i += 4;

					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);
					
					AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,interp);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);

					num_of_frames_encoded++; i += 2;
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeTGA(frame1);
				AGIDL_FreeTGA(frame2);
				AGIDL_FreeTGA(frame3);
				AGIDL_FreeTGA(frame4);
			}break;
			case AGIDL_IMG_TIM:{
				AGMV_Printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_TIM* frame1 = AGIDL_LoadTIM(filename1);
				AGIDL_TIM* frame2 = AGIDL_LoadTIM(filename2);
				AGIDL_TIM* frame3 = AGIDL_LoadTIM(filename3);
				AGIDL_TIM* frame4 = AGIDL_LoadTIM(filename4);
				AGMV_Printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				
				AGIDL_ColorConvertTIM(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertTIM(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertTIM(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertTIM(frame4,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertTIM(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertTIM(frame2,AGIDL_RGB_555);
					AGIDL_ColorConvertTIM(frame3,AGIDL_RGB_555);
					AGIDL_ColorConvertTIM(frame4,AGIDL_RGB_555);
					
					AGIDL_ColorConvertTIM(frame1,AGIDL_RGB_888);
					AGIDL_ColorConvertTIM(frame2,AGIDL_RGB_888);
					AGIDL_ColorConvertTIM(frame3,AGIDL_RGB_888);
					AGIDL_ColorConvertTIM(frame4,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_TIMGetWidth(frame1), h = AGIDL_TIMGetHeight(frame1);
					AGIDL_FastScaleTIM(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTIM(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTIM(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTIM(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}		
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_TIMGetWidth(frame1), h = AGIDL_TIMGetHeight(frame1);
					AGIDL_FastScaleTIM(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTIM(frame2,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTIM(frame3,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTIM(frame4,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_TIMGetWidth(frame1), h = AGIDL_TIMGetHeight(frame1);
					AGIDL_FastScaleTIM(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTIM(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTIM(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleTIM(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
						AGMV_DitherImageData(frame2->pixels.pix32,width,height);
						AGMV_DitherImageData(frame3->pixels.pix32,width,height);
						AGMV_DitherImageData(frame4->pixels.pix32,width,height);
					}
				}
				
				w = AGIDL_TIMGetWidth(frame2), h = AGIDL_TIMGetHeight(frame2);
				
				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_I && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_I && opt != AGMV_OPT_NDS_II){

					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);
					
					AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
					AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
					AGMV_EncodeFrame(file,agmv,interp);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					
					num_of_frames_encoded += 3; i += 4;
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);
					
					AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,interp);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);

					num_of_frames_encoded++; i += 2;
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeTIM(frame1);
				AGIDL_FreeTIM(frame2);
				AGIDL_FreeTIM(frame3);
				AGIDL_FreeTIM(frame4);
			}break;
			case AGIDL_IMG_PCX:{
				AGMV_Printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_PCX* frame1 = AGIDL_LoadPCX(filename1);
				AGIDL_PCX* frame2 = AGIDL_LoadPCX(filename2);
				AGIDL_PCX* frame3 = AGIDL_LoadPCX(filename3);
				AGIDL_PCX* frame4 = AGIDL_LoadPCX(filename4);
				AGMV_Printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				
				AGIDL_ColorConvertPCX(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertPCX(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertPCX(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertPCX(frame4,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertPCX(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertPCX(frame2,AGIDL_RGB_555);
					AGIDL_ColorConvertPCX(frame3,AGIDL_RGB_555);
					AGIDL_ColorConvertPCX(frame4,AGIDL_RGB_555);
					
					AGIDL_ColorConvertPCX(frame1,AGIDL_RGB_888);
					AGIDL_ColorConvertPCX(frame2,AGIDL_RGB_888);
					AGIDL_ColorConvertPCX(frame3,AGIDL_RGB_888);
					AGIDL_ColorConvertPCX(frame4,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_PCXGetWidth(frame1), h = AGIDL_PCXGetHeight(frame1);
					AGIDL_FastScalePCX(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePCX(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePCX(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePCX(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}		
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_PCXGetWidth(frame1), h = AGIDL_PCXGetHeight(frame1);
					AGIDL_FastScalePCX(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePCX(frame2,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePCX(frame3,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePCX(frame4,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_PCXGetWidth(frame1), h = AGIDL_PCXGetHeight(frame1);
					AGIDL_FastScalePCX(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePCX(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePCX(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePCX(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
						AGMV_DitherImageData(frame2->pixels.pix32,width,height);
						AGMV_DitherImageData(frame3->pixels.pix32,width,height);
						AGMV_DitherImageData(frame4->pixels.pix32,width,height);
					}
				}
				
				w = AGIDL_PCXGetWidth(frame2), h = AGIDL_PCXGetHeight(frame2);
				
				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_I && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_I && opt != AGMV_OPT_NDS_II){

					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
					
					AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);
					
					AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
					AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
					AGMV_EncodeFrame(file,agmv,interp);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					
					num_of_frames_encoded += 3; i += 4;;
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);
					
					AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,interp);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);

					num_of_frames_encoded++; i += 2;
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreePCX(frame1);
				AGIDL_FreePCX(frame2);
				AGIDL_FreePCX(frame3);
				AGIDL_FreePCX(frame4);
			}break;
			case AGIDL_IMG_LMP:{
				AGMV_Printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_LMP* frame1 = AGIDL_LoadLMP(filename1);
				AGIDL_LMP* frame2 = AGIDL_LoadLMP(filename2);
				AGIDL_LMP* frame3 = AGIDL_LoadLMP(filename3);
				AGIDL_LMP* frame4 = AGIDL_LoadLMP(filename4);
				AGMV_Printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				
				AGIDL_ColorConvertLMP(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertLMP(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertLMP(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertLMP(frame4,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertLMP(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertLMP(frame2,AGIDL_RGB_555);
					AGIDL_ColorConvertLMP(frame3,AGIDL_RGB_555);
					AGIDL_ColorConvertLMP(frame4,AGIDL_RGB_555);
					
					AGIDL_ColorConvertLMP(frame1,AGIDL_RGB_888);
					AGIDL_ColorConvertLMP(frame2,AGIDL_RGB_888);
					AGIDL_ColorConvertLMP(frame3,AGIDL_RGB_888);
					AGIDL_ColorConvertLMP(frame4,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_LMPGetWidth(frame1), h = AGIDL_LMPGetHeight(frame1);
					AGIDL_FastScaleLMP(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLMP(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLMP(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLMP(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}		
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_LMPGetWidth(frame1), h = AGIDL_LMPGetHeight(frame1);
					AGIDL_FastScaleLMP(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLMP(frame2,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLMP(frame3,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLMP(frame4,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_LMPGetWidth(frame1), h = AGIDL_LMPGetHeight(frame1);
					AGIDL_FastScaleLMP(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLMP(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLMP(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLMP(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
						AGMV_DitherImageData(frame2->pixels.pix32,width,height);
						AGMV_DitherImageData(frame3->pixels.pix32,width,height);
						AGMV_DitherImageData(frame4->pixels.pix32,width,height);
					}
				}
				
				w = AGIDL_LMPGetWidth(frame2), h = AGIDL_LMPGetHeight(frame2);

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_I && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_I && opt != AGMV_OPT_NDS_II){

					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);
					
					AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
					AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
					AGMV_EncodeFrame(file,agmv,interp);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					
					num_of_frames_encoded += 3; i += 4;
						
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);
					
					AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,interp);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);

					num_of_frames_encoded++; i += 2;
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeLMP(frame1);
				AGIDL_FreeLMP(frame2);
				AGIDL_FreeLMP(frame3);
				AGIDL_FreeLMP(frame4);
			}break;
			case AGIDL_IMG_PVR:{
				AGMV_Printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_PVR* frame1 = AGIDL_LoadPVR(filename1);
				AGIDL_PVR* frame2 = AGIDL_LoadPVR(filename2);
				AGIDL_PVR* frame3 = AGIDL_LoadPVR(filename3);
				AGIDL_PVR* frame4 = AGIDL_LoadPVR(filename4);
				AGMV_Printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				
				AGIDL_ColorConvertPVR(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertPVR(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertPVR(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertPVR(frame4,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertPVR(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertPVR(frame2,AGIDL_RGB_555);
					AGIDL_ColorConvertPVR(frame3,AGIDL_RGB_555);
					AGIDL_ColorConvertPVR(frame4,AGIDL_RGB_555);
					
					AGIDL_ColorConvertPVR(frame1,AGIDL_RGB_888);
					AGIDL_ColorConvertPVR(frame2,AGIDL_RGB_888);
					AGIDL_ColorConvertPVR(frame3,AGIDL_RGB_888);
					AGIDL_ColorConvertPVR(frame4,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_PVRGetWidth(frame1), h = AGIDL_PVRGetHeight(frame1);
					AGIDL_FastScalePVR(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePVR(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePVR(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePVR(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}		
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_PVRGetWidth(frame1), h = AGIDL_PVRGetHeight(frame1);
					AGIDL_FastScalePVR(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePVR(frame2,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePVR(frame3,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePVR(frame4,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_PVRGetWidth(frame1), h = AGIDL_PVRGetHeight(frame1);
					AGIDL_FastScalePVR(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePVR(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePVR(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePVR(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
						AGMV_DitherImageData(frame2->pixels.pix32,width,height);
						AGMV_DitherImageData(frame3->pixels.pix32,width,height);
						AGMV_DitherImageData(frame4->pixels.pix32,width,height);
					}
				}
				
				w = AGIDL_PVRGetWidth(frame2), h = AGIDL_PVRGetHeight(frame2);

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_I && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_I && opt != AGMV_OPT_NDS_II){

					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
					
					AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);
					
					AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
					AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
					AGMV_EncodeFrame(file,agmv,interp);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					
					num_of_frames_encoded += 3; i += 4;
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);
					
					AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,interp);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);

					num_of_frames_encoded++; i += 2;
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreePVR(frame1);
				AGIDL_FreePVR(frame2);
				AGIDL_FreePVR(frame3);
				AGIDL_FreePVR(frame4);
			}break;
			case AGIDL_IMG_GXT:{
				AGMV_Printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_GXT* frame1 = AGIDL_LoadGXT(filename1);
				AGIDL_GXT* frame2 = AGIDL_LoadGXT(filename2);
				AGIDL_GXT* frame3 = AGIDL_LoadGXT(filename3);
				AGIDL_GXT* frame4 = AGIDL_LoadGXT(filename4);
				AGMV_Printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				
				AGIDL_ColorConvertGXT(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertGXT(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertGXT(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertGXT(frame4,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertGXT(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertGXT(frame2,AGIDL_RGB_555);
					AGIDL_ColorConvertGXT(frame3,AGIDL_RGB_555);
					AGIDL_ColorConvertGXT(frame4,AGIDL_RGB_555);
					
					AGIDL_ColorConvertGXT(frame1,AGIDL_RGB_888);
					AGIDL_ColorConvertGXT(frame2,AGIDL_RGB_888);
					AGIDL_ColorConvertGXT(frame3,AGIDL_RGB_888);
					AGIDL_ColorConvertGXT(frame4,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_GXTGetWidth(frame1), h = AGIDL_GXTGetHeight(frame1);
					AGIDL_FastScaleGXT(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleGXT(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleGXT(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleGXT(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}		
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_GXTGetWidth(frame1), h = AGIDL_GXTGetHeight(frame1);
					AGIDL_FastScaleGXT(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleGXT(frame2,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleGXT(frame3,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleGXT(frame4,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_GXTGetWidth(frame1), h = AGIDL_GXTGetHeight(frame1);
					AGIDL_FastScaleGXT(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleGXT(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleGXT(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleGXT(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
						AGMV_DitherImageData(frame2->pixels.pix32,width,height);
						AGMV_DitherImageData(frame3->pixels.pix32,width,height);
						AGMV_DitherImageData(frame4->pixels.pix32,width,height);
					}
				}
				
				w = AGIDL_GXTGetWidth(frame2), h = AGIDL_GXTGetHeight(frame2);

				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_I && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_I && opt != AGMV_OPT_NDS_II){

					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);
					
					AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
					AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
					AGMV_EncodeFrame(file,agmv,interp);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);					
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					
					num_of_frames_encoded += 3; i += 4;
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);
					
					AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,interp);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);

					num_of_frames_encoded++; i += 2;
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeGXT(frame1);
				AGIDL_FreeGXT(frame2);
				AGIDL_FreeGXT(frame3);
				AGIDL_FreeGXT(frame4);
			}break;
			case AGIDL_IMG_BTI:{
				AGMV_Printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_BTI* frame1 = AGIDL_LoadBTI(filename1);
				AGIDL_BTI* frame2 = AGIDL_LoadBTI(filename2);
				AGIDL_BTI* frame3 = AGIDL_LoadBTI(filename3);
				AGIDL_BTI* frame4 = AGIDL_LoadBTI(filename4);
				AGMV_Printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				
				AGIDL_ColorConvertBTI(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertBTI(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertBTI(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertBTI(frame4,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertBTI(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertBTI(frame2,AGIDL_RGB_555);
					AGIDL_ColorConvertBTI(frame3,AGIDL_RGB_555);
					AGIDL_ColorConvertBTI(frame4,AGIDL_RGB_555);
					
					AGIDL_ColorConvertBTI(frame1,AGIDL_RGB_888);
					AGIDL_ColorConvertBTI(frame2,AGIDL_RGB_888);
					AGIDL_ColorConvertBTI(frame3,AGIDL_RGB_888);
					AGIDL_ColorConvertBTI(frame4,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_BTIGetWidth(frame1), h = AGIDL_BTIGetHeight(frame1);
					AGIDL_FastScaleBTI(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBTI(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBTI(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBTI(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}		
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_BTIGetWidth(frame1), h = AGIDL_BTIGetHeight(frame1);
					AGIDL_FastScaleBTI(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBTI(frame2,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBTI(frame3,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBTI(frame4,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_BTIGetWidth(frame1), h = AGIDL_BTIGetHeight(frame1);
					AGIDL_FastScaleBTI(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBTI(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBTI(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleBTI(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
						AGMV_DitherImageData(frame2->pixels.pix32,width,height);
						AGMV_DitherImageData(frame3->pixels.pix32,width,height);
						AGMV_DitherImageData(frame4->pixels.pix32,width,height);
					}
				}
				
				w = AGIDL_BTIGetWidth(frame2), h = AGIDL_BTIGetHeight(frame2);
				
				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_I && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_I && opt != AGMV_OPT_NDS_II){

					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);
					
					AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
					AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
					AGMV_EncodeFrame(file,agmv,interp);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					
					num_of_frames_encoded += 3; i += 4;
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);
					
					AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,interp);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);

					num_of_frames_encoded++; i += 2;
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreeBTI(frame1);
				AGIDL_FreeBTI(frame2);
				AGIDL_FreeBTI(frame3);
				AGIDL_FreeBTI(frame4);
			}break;
			case AGIDL_IMG_3DF:{
				AGMV_Printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_3DF* frame1 = AGIDL_Load3DF(filename1);
				AGIDL_3DF* frame2 = AGIDL_Load3DF(filename2);
				AGIDL_3DF* frame3 = AGIDL_Load3DF(filename3);
				AGIDL_3DF* frame4 = AGIDL_Load3DF(filename4);
				AGMV_Printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				
				AGIDL_ColorConvert3DF(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvert3DF(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvert3DF(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvert3DF(frame4,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvert3DF(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvert3DF(frame2,AGIDL_RGB_555);
					AGIDL_ColorConvert3DF(frame3,AGIDL_RGB_555);
					AGIDL_ColorConvert3DF(frame4,AGIDL_RGB_555);
					
					AGIDL_ColorConvert3DF(frame1,AGIDL_RGB_888);
					AGIDL_ColorConvert3DF(frame2,AGIDL_RGB_888);
					AGIDL_ColorConvert3DF(frame3,AGIDL_RGB_888);
					AGIDL_ColorConvert3DF(frame4,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_3DFGetWidth(frame1), h = AGIDL_3DFGetHeight(frame1);
					AGIDL_FastScale3DF(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScale3DF(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScale3DF(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScale3DF(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}		
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_3DFGetWidth(frame1), h = AGIDL_3DFGetHeight(frame1);
					AGIDL_FastScale3DF(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScale3DF(frame2,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScale3DF(frame3,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScale3DF(frame4,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_3DFGetWidth(frame1), h = AGIDL_3DFGetHeight(frame1);
					AGIDL_FastScale3DF(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScale3DF(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScale3DF(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScale3DF(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
						AGMV_DitherImageData(frame2->pixels.pix32,width,height);
						AGMV_DitherImageData(frame3->pixels.pix32,width,height);
						AGMV_DitherImageData(frame4->pixels.pix32,width,height);
					}
				}
				
				w = AGIDL_3DFGetWidth(frame2), h = AGIDL_3DFGetHeight(frame2);
				
				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_I && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_I && opt != AGMV_OPT_NDS_II){

					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);
					
					AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
					AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
					AGMV_EncodeFrame(file,agmv,interp);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					
					num_of_frames_encoded += 3; i += 4;

					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);
					
					AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,interp);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);

					num_of_frames_encoded++; i += 2;
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_Free3DF(frame1);
				AGIDL_Free3DF(frame2);
				AGIDL_Free3DF(frame3);
				AGIDL_Free3DF(frame4);
			}break;
			case AGIDL_IMG_PPM:{
				AGMV_Printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_PPM* frame1 = AGIDL_LoadPPM(filename1);
				AGIDL_PPM* frame2 = AGIDL_LoadPPM(filename2);
				AGIDL_PPM* frame3 = AGIDL_LoadPPM(filename3);
				AGIDL_PPM* frame4 = AGIDL_LoadPPM(filename4);
				AGMV_Printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				
				AGIDL_ColorConvertPPM(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertPPM(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertPPM(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertPPM(frame4,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertPPM(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertPPM(frame2,AGIDL_RGB_555);
					AGIDL_ColorConvertPPM(frame3,AGIDL_RGB_555);
					AGIDL_ColorConvertPPM(frame4,AGIDL_RGB_555);
					
					AGIDL_ColorConvertPPM(frame1,AGIDL_RGB_888);
					AGIDL_ColorConvertPPM(frame2,AGIDL_RGB_888);
					AGIDL_ColorConvertPPM(frame3,AGIDL_RGB_888);
					AGIDL_ColorConvertPPM(frame4,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_PPMGetWidth(frame1), h = AGIDL_PPMGetHeight(frame1);
					AGIDL_FastScalePPM(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePPM(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePPM(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePPM(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}		
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_PPMGetWidth(frame1), h = AGIDL_PPMGetHeight(frame1);
					AGIDL_FastScalePPM(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePPM(frame2,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePPM(frame3,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePPM(frame4,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_PPMGetWidth(frame1), h = AGIDL_PPMGetHeight(frame1);
					AGIDL_FastScalePPM(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePPM(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePPM(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScalePPM(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
						AGMV_DitherImageData(frame2->pixels.pix32,width,height);
						AGMV_DitherImageData(frame3->pixels.pix32,width,height);
						AGMV_DitherImageData(frame4->pixels.pix32,width,height);
					}
				}
				
				w = AGIDL_PPMGetWidth(frame2), h = AGIDL_PPMGetHeight(frame2);
				
				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_I && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_I && opt != AGMV_OPT_NDS_II){

					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);
					
					AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);					
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
					AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
					AGMV_EncodeFrame(file,agmv,interp);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					
					num_of_frames_encoded += 3; i += 4;

					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);
					
					AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,interp);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);

					num_of_frames_encoded++; i += 2;
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
				}

				AGIDL_FreePPM(frame1);
				AGIDL_FreePPM(frame2);
				AGIDL_FreePPM(frame3);
				AGIDL_FreePPM(frame4);
			}break;
			case AGIDL_IMG_LBM:{
				AGMV_Printf("Loading Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				AGIDL_LBM* frame1 = AGIDL_LoadLBM(filename1);
				AGIDL_LBM* frame2 = AGIDL_LoadLBM(filename2);
				AGIDL_LBM* frame3 = AGIDL_LoadLBM(filename3);
				AGIDL_LBM* frame4 = AGIDL_LoadLBM(filename4);
				AGMV_Printf("Loaded Group of AGIDL Image Frames - %ld - %ld...\n",i,i+3);
				
				AGIDL_ColorConvertLBM(frame1,AGIDL_RGB_888);
				AGIDL_ColorConvertLBM(frame2,AGIDL_RGB_888);
				AGIDL_ColorConvertLBM(frame3,AGIDL_RGB_888);
				AGIDL_ColorConvertLBM(frame4,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertLBM(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertLBM(frame2,AGIDL_RGB_555);
					AGIDL_ColorConvertLBM(frame3,AGIDL_RGB_555);
					AGIDL_ColorConvertLBM(frame4,AGIDL_RGB_555);
					
					AGIDL_ColorConvertLBM(frame1,AGIDL_RGB_888);
					AGIDL_ColorConvertLBM(frame2,AGIDL_RGB_888);
					AGIDL_ColorConvertLBM(frame3,AGIDL_RGB_888);
					AGIDL_ColorConvertLBM(frame4,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_LBMGetWidth(frame1), h = AGIDL_LBMGetHeight(frame1);
					AGIDL_FastScaleLBM(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLBM(frame2,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLBM(frame3,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLBM(frame4,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}		
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_LBMGetWidth(frame1), h = AGIDL_LBMGetHeight(frame1);
					AGIDL_FastScaleLBM(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLBM(frame2,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLBM(frame3,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLBM(frame4,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_LBMGetWidth(frame1), h = AGIDL_LBMGetHeight(frame1);
					AGIDL_FastScaleLBM(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLBM(frame2,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLBM(frame3,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					AGIDL_FastScaleLBM(frame4,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame2->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame3->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
						AGMV_DitherImageData(frame4->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
						AGMV_DitherImageData(frame2->pixels.pix32,width,height);
						AGMV_DitherImageData(frame3->pixels.pix32,width,height);
						AGMV_DitherImageData(frame4->pixels.pix32,width,height);
					}
				}
				
				w = AGIDL_LBMGetWidth(frame2), h = AGIDL_LBMGetHeight(frame2);
				
				if(opt != AGMV_OPT_I && opt != AGMV_OPT_ANIM && opt != AGMV_OPT_GBA_I && opt != AGMV_OPT_GBA_II && opt != AGMV_OPT_PSX_I && opt != AGMV_OPT_PSX_II && opt != AGMV_OPT_NDS_I && opt != AGMV_OPT_NDS_II){

					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);

					AGMV_InterpFrame(interp,frame2->pixels.pix32,frame3->pixels.pix32,w,h);
					
					AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);					
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
					AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i+1);
					AGMV_EncodeFrame(file,agmv,interp);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					AGMV_EncodeFrame(file,agmv,frame4->pixels.pix32);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i+3);
					
					num_of_frames_encoded += 3; i += 4;
					
					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+3);
				}
				else{
					AGMV_Printf("Performing Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);

					AGMV_InterpFrame(interp,frame1->pixels.pix32,frame2->pixels.pix32,w,h);
					
					AGMV_Printf("Encoding Interpolated Image Frame - %ld...\n",i);
					AGMV_EncodeFrame(file,agmv,interp);	
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);

					num_of_frames_encoded++; i += 2;

					AGMV_Printf("Performed Progressive Differential Interpolated Frame Skipping - %ld - %ld...\n",i,i+1);
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
	
	if(AGMV_GetTotalAudioDuration(agmv) != 0){
		fclose(agmv->audio);
	}

	fclose(file);
	free(interp);
	free(ext);
	DestroyAGMV(agmv); 
	
	if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
		FILE* file = fopen(filename,"rb");
		fseek(file,0,SEEK_END);
		u32 file_size = ftell(file);
		fseek(file,0,SEEK_SET);
		u8* data = (u8*)malloc(sizeof(u8)*file_size);
		fread(data,1,file_size,file);
		fclose(file);
		
		FILE* out = fopen("GBA_GEN_AGMV.h","w");
		
		fprintf(out,"#ifndef GBA_GEN_AGMV_H\n");
		fprintf(out,"#define GBA_GEN_AGMV_H\n\n");
		fprintf(out,"const unsigned char GBA_AGMV_FILE[%ld] = {\n",file_size);
		
		int i;
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
	u32 i, palette0[256], palette1[256], max_clr, size = width*height, AGMV_GBA_W, AGMV_GBA_H;
	u32 sample_size;
	u32 pal[512];
	AGMV_QUALITY org_quality;
	
	org_quality = quality;

	AGMV_SetOPT(agmv,opt);
	AGMV_SetCompression(agmv,compression);
	AGMV_SetQuality(agmv,quality);

	if(start_frame != end_frame){
		end_frame -= 1;
	}
	
	if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
		agmv->max_diff = AGMV_MAX_15BPP_DIFF;
	}
	else{
		agmv->max_diff = AGMV_MAX_DIFF;
	}
	
	if(quality == AGMV_LOW_QUALITY || quality == AGMV_MID_QUALITY){
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
	
	char* ext = AGIDL_GetImgExtension(img_type);
	
	if(opt != AGMV_OPT_IV && opt != AGMV_OPT_GBA_IV && opt != AGMV_OPT_PSX_IV && opt != AGMV_OPT_NDS_IV){
	
		u32* colorgram = (u32*)malloc(sizeof(u32)*max_clr+5);
		u32* histogram = (u32*)malloc(sizeof(u32)*max_clr+5);

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

		AGMV_LoadHistogramImageData(dir,basename,ext,start_frame,end_frame,size,histogram,colorgram,img_type,quality);
		AGMV_BubbleSort(histogram,colorgram,max_clr);
		AGMV_DistributePaletteData(colorgram,pal,palette0,palette1,max_clr,opt,quality);
		
		free(colorgram);
		free(histogram);
	}
	
	if(AGMV_GetTotalAudioDuration(agmv) != 0){
		sample_size = agmv->header.audio_size / (f32)(end_frame-start_frame);
		agmv->audio_chunk->size = sample_size;
		agmv->audio_chunk->atsample = (u8*)malloc(sizeof(u8)*agmv->audio_chunk->size);
		
		if(AGMV_GetBitsPerSample(agmv) == 16){
			agmv->audio_track->pcm = (u16*)malloc(sizeof(u16)*agmv->audio_chunk->size);
		}
		else{
			agmv->audio_track->pcm8 = (u8*)malloc(sizeof(u8)*agmv->audio_chunk->size);
		}
	}
	
	quality = org_quality;
	
	FILE* file = fopen(filename,"wb");
	
	AGMV_SetICP0(agmv,palette0);
	AGMV_SetICP1(agmv,palette1);
	
	AGMV_Printf("Encoding AGMV Header...\n");
	AGMV_EncodeHeader(file,agmv);
	AGMV_Printf("Encoded AGMV Header...\n");
	
	char filename1[60];
	
	for(i = start_frame; i <= end_frame; i++){
		if(dir[0] != 'c' || dir[1] != 'u' || dir[2] != 'r'){
			sprintf(filename1,"%s/%s%ld%s",dir,basename,i,ext);
		}
		else{
			sprintf(filename1,"%s%ld%s",basename,i,ext);
		}
		
		switch(img_type){
			case AGIDL_IMG_BMP:{
				AGMV_Printf("Loading AGIDL Image Frame - %ld\n",i);
				AGIDL_BMP* frame1 = AGIDL_LoadBMP(filename1);
				AGMV_Printf("Loaded AGIDL Image Frame - %ld\n",i);
				
				AGIDL_ColorConvertBMP(frame1,AGIDL_RGB_888);

				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertBMP(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertBMP(frame1,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_BMPGetWidth(frame1), h = AGIDL_BMPGetHeight(frame1);
					AGIDL_FastScaleBMP(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_BMPGetWidth(frame1), h = AGIDL_BMPGetHeight(frame1);
					AGIDL_FastScaleBMP(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					AGIDL_FlipHorzBMP(frame1);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_BMPGetWidth(frame1), h = AGIDL_BMPGetHeight(frame1);
					AGIDL_FastScaleBMP(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
					}
				}
					
				AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
				AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
				AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
				
				if(AGMV_GetTotalAudioDuration(agmv) != 0){
					AGMV_Printf("Encoding AGMV Audio Chunk - %ld...\n",i);
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGMV Audio Chunk - %ld...\n",i);	
				}
				
				AGIDL_FreeBMP(frame1);
			}break;
			case AGIDL_IMG_TGA:{
				AGMV_Printf("Loading AGIDL Image Frame - %ld\n",i);
				AGIDL_TGA* frame1 = AGIDL_LoadTGA(filename1);
				AGMV_Printf("Loaded AGIDL Image Frame - %ld\n",i);
				
				AGIDL_ColorConvertTGA(frame1,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertTGA(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertTGA(frame1,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_TGAGetWidth(frame1), h = AGIDL_TGAGetHeight(frame1);
					AGIDL_FastScaleTGA(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_TGAGetWidth(frame1), h = AGIDL_TGAGetHeight(frame1);
					AGIDL_FastScaleTGA(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_TGAGetWidth(frame1), h = AGIDL_TGAGetHeight(frame1);
					AGIDL_FastScaleTGA(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
					}
				}
					
				AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
				AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
				AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
				
				if(AGMV_GetTotalAudioDuration(agmv) != 0){
					AGMV_Printf("Encoding AGMV Audio Chunk - %ld...\n",i);
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGMV Audio Chunk - %ld...\n",i);	
				}
				
				AGIDL_FreeTGA(frame1);
			}break;
			case AGIDL_IMG_TIM:{
				AGMV_Printf("Loading AGIDL Image Frame - %ld\n",i);
				AGIDL_TIM* frame1 = AGIDL_LoadTIM(filename1);
				AGMV_Printf("Loaded AGIDL Image Frame - %ld\n",i);
				
				AGIDL_ColorConvertTIM(frame1,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertTIM(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertTIM(frame1,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_TIMGetWidth(frame1), h = AGIDL_TIMGetHeight(frame1);
					AGIDL_FastScaleTIM(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_TIMGetWidth(frame1), h = AGIDL_TIMGetHeight(frame1);
					AGIDL_FastScaleTIM(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_TIMGetWidth(frame1), h = AGIDL_TIMGetHeight(frame1);
					AGIDL_FastScaleTIM(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
					}
				}
					
				AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
				AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
				AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
				
				if(AGMV_GetTotalAudioDuration(agmv) != 0){
					AGMV_Printf("Encoding AGMV Audio Chunk - %ld...\n",i);
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGMV Audio Chunk - %ld...\n",i);	
				}
				
				AGIDL_FreeTIM(frame1);
			}break;
			case AGIDL_IMG_PCX:{
				AGMV_Printf("Loading AGIDL Image Frame - %ld\n",i);
				AGIDL_PCX* frame1 = AGIDL_LoadPCX(filename1);
				AGMV_Printf("Loaded AGIDL Image Frame - %ld\n",i);
				
				AGIDL_ColorConvertPCX(frame1,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertPCX(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertPCX(frame1,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_PCXGetWidth(frame1), h = AGIDL_PCXGetHeight(frame1);
					AGIDL_FastScalePCX(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_PCXGetWidth(frame1), h = AGIDL_PCXGetHeight(frame1);
					AGIDL_FastScalePCX(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_PCXGetWidth(frame1), h = AGIDL_PCXGetHeight(frame1);
					AGIDL_FastScalePCX(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
					}
				}
					
				AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
				AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
				AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
				
				if(AGMV_GetTotalAudioDuration(agmv) != 0){
					AGMV_Printf("Encoding AGMV Audio Chunk - %ld...\n",i);
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGMV Audio Chunk - %ld...\n",i);	
				}
				
				AGIDL_FreePCX(frame1);
			}break;
			case AGIDL_IMG_LMP:{
				AGMV_Printf("Loading AGIDL Image Frame - %ld\n",i);
				AGIDL_LMP* frame1 = AGIDL_LoadLMP(filename1);
				AGMV_Printf("Loaded AGIDL Image Frame - %ld\n",i);
				
				AGIDL_ColorConvertLMP(frame1,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertLMP(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertLMP(frame1,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_LMPGetWidth(frame1), h = AGIDL_LMPGetHeight(frame1);
					AGIDL_FastScaleLMP(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_LMPGetWidth(frame1), h = AGIDL_LMPGetHeight(frame1);
					AGIDL_FastScaleLMP(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_LMPGetWidth(frame1), h = AGIDL_LMPGetHeight(frame1);
					AGIDL_FastScaleLMP(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
					}
				}
					
				AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
				AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
				AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
				
				if(AGMV_GetTotalAudioDuration(agmv) != 0){
					AGMV_Printf("Encoding AGMV Audio Chunk - %ld...\n",i);
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGMV Audio Chunk - %ld...\n",i);	
				}
				
				AGIDL_FreeLMP(frame1);
			}break;
			case AGIDL_IMG_PVR:{
				AGMV_Printf("Loading AGIDL Image Frame - %ld\n",i);
				AGIDL_PVR* frame1 = AGIDL_LoadPVR(filename1);
				AGMV_Printf("Loaded AGIDL Image Frame - %ld\n",i);
				
				AGIDL_ColorConvertPVR(frame1,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertPVR(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertPVR(frame1,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_PVRGetWidth(frame1), h = AGIDL_PVRGetHeight(frame1);
					AGIDL_FastScalePVR(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_PVRGetWidth(frame1), h = AGIDL_PVRGetHeight(frame1);
					AGIDL_FastScalePVR(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_PVRGetWidth(frame1), h = AGIDL_PVRGetHeight(frame1);
					AGIDL_FastScalePVR(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
					}
				}
					
				AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
				AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
				AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
				
				if(AGMV_GetTotalAudioDuration(agmv) != 0){
					AGMV_Printf("Encoding AGMV Audio Chunk - %ld...\n",i);
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGMV Audio Chunk - %ld...\n",i);	
				}
				
				AGIDL_FreePVR(frame1);
			}break;
			case AGIDL_IMG_GXT:{
				AGMV_Printf("Loading AGIDL Image Frame - %ld\n",i);
				AGIDL_GXT* frame1 = AGIDL_LoadGXT(filename1);
				AGMV_Printf("Loaded AGIDL Image Frame - %ld\n",i);
				
				AGIDL_ColorConvertGXT(frame1,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertGXT(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertGXT(frame1,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_GXTGetWidth(frame1), h = AGIDL_GXTGetHeight(frame1);
					AGIDL_FastScaleGXT(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_GXTGetWidth(frame1), h = AGIDL_GXTGetHeight(frame1);
					AGIDL_FastScaleGXT(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_GXTGetWidth(frame1), h = AGIDL_GXTGetHeight(frame1);
					AGIDL_FastScaleGXT(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
					}
				}

				AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
				AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
				AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
				
				if(AGMV_GetTotalAudioDuration(agmv) != 0){
					AGMV_Printf("Encoding AGMV Audio Chunk - %ld...\n",i);
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGMV Audio Chunk - %ld...\n",i);	
				}
				
				AGIDL_FreeGXT(frame1);
			}break;
			case AGIDL_IMG_BTI:{
				AGMV_Printf("Loading AGIDL Image Frame - %ld\n",i);
				AGIDL_BTI* frame1 = AGIDL_LoadBTI(filename1);
				AGMV_Printf("Loaded AGIDL Image Frame - %ld\n",i);
				
				AGIDL_ColorConvertBTI(frame1,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertBTI(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertBTI(frame1,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_BTIGetWidth(frame1), h = AGIDL_BTIGetHeight(frame1);
					AGIDL_FastScaleBTI(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_BTIGetWidth(frame1), h = AGIDL_BTIGetHeight(frame1);
					AGIDL_FastScaleBTI(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_BTIGetWidth(frame1), h = AGIDL_BTIGetHeight(frame1);
					AGIDL_FastScaleBTI(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
					}
				}
				
				AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
				AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);	
				if(AGMV_GetTotalAudioDuration(agmv) != 0){
					AGMV_Printf("Encoding AGMV Audio Chunk - %ld...\n",i);
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGMV Audio Chunk - %ld...\n",i);	
				}
				AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
				
				AGIDL_FreeBTI(frame1);
			}break;
			case AGIDL_IMG_3DF:{
				AGMV_Printf("Loading AGIDL Image Frame - %ld\n",i);
				AGIDL_3DF* frame1 = AGIDL_Load3DF(filename1);
				AGMV_Printf("Loaded AGIDL Image Frame - %ld\n",i);
				
				AGIDL_ColorConvert3DF(frame1,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvert3DF(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvert3DF(frame1,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_3DFGetWidth(frame1), h = AGIDL_3DFGetHeight(frame1);
					AGIDL_FastScale3DF(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_3DFGetWidth(frame1), h = AGIDL_3DFGetHeight(frame1);
					AGIDL_FastScale3DF(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_3DFGetWidth(frame1), h = AGIDL_3DFGetHeight(frame1);
					AGIDL_FastScale3DF(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
					}
				}
					
				AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
				AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
				AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
				
				if(AGMV_GetTotalAudioDuration(agmv) != 0){
					AGMV_Printf("Encoding AGMV Audio Chunk - %ld...\n",i);
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGMV Audio Chunk - %ld...\n",i);	
				}
				
				AGIDL_Free3DF(frame1);
			}break;
			case AGIDL_IMG_PPM:{
				AGMV_Printf("Loading AGIDL Image Frame - %ld\n",i);
				AGIDL_PPM* frame1 = AGIDL_LoadPPM(filename1);
				AGMV_Printf("Loaded AGIDL Image Frame - %ld\n",i);
				
				AGIDL_ColorConvertPPM(frame1,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertPPM(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertPPM(frame1,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_PPMGetWidth(frame1), h = AGIDL_PPMGetHeight(frame1);
					AGIDL_FastScalePPM(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_PPMGetWidth(frame1), h = AGIDL_PPMGetHeight(frame1);
					AGIDL_FastScalePPM(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_PPMGetWidth(frame1), h = AGIDL_PPMGetHeight(frame1);
					AGIDL_FastScalePPM(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
					}
				}
					
				AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
				AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
				AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
				
				if(AGMV_GetTotalAudioDuration(agmv) != 0){
					AGMV_Printf("Encoding AGMV Audio Chunk - %ld...\n",i);
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGMV Audio Chunk - %ld...\n",i);	
				}
				
				AGIDL_FreePPM(frame1);
			}break;
			case AGIDL_IMG_LBM:{
				AGMV_Printf("Loading AGIDL Image Frame - %ld\n",i);
				AGIDL_LBM* frame1 = AGIDL_LoadLBM(filename1);
				AGMV_Printf("Loaded AGIDL Image Frame - %ld\n",i);
				
				AGIDL_ColorConvertLBM(frame1,AGIDL_RGB_888);
				
				if(opt == AGMV_OPT_IV || opt == AGMV_OPT_GBA_IV || opt == AGMV_OPT_PSX_IV || opt == AGMV_OPT_NDS_IV){
					AGIDL_ColorConvertLBM(frame1,AGIDL_RGB_555);
					AGIDL_ColorConvertLBM(frame1,AGIDL_RGB_888);
				}
				
				if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
					u32 w = AGIDL_LBMGetWidth(frame1), h = AGIDL_LBMGetHeight(frame1);
					AGIDL_FastScaleLBM(frame1,((f32)AGMV_GBA_W/w)+0.001f,((f32)AGMV_GBA_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_GBA_W,AGMV_GBA_H);
					}
				}
				else if(opt == AGMV_OPT_PSX_I || opt == AGMV_OPT_PSX_II || opt == AGMV_OPT_PSX_III || opt == AGMV_OPT_PSX_IV){
					u32 w = AGIDL_LBMGetWidth(frame1), h = AGIDL_LBMGetHeight(frame1);
					AGIDL_FastScaleLBM(frame1,((f32)AGMV_PSX_W/w)+0.001f,((f32)AGMV_PSX_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_PSX_W,AGMV_PSX_H);
					}
				}
				else if(opt == AGMV_OPT_NDS_I || opt == AGMV_OPT_NDS_II || opt == AGMV_OPT_NDS_III || opt == AGMV_OPT_NDS_IV){
					u32 w = AGIDL_LBMGetWidth(frame1), h = AGIDL_LBMGetHeight(frame1);
					AGIDL_FastScaleLBM(frame1,((f32)AGMV_NDS_W/w)+0.001f,((f32)AGMV_NDS_H/h)+0.001f,AGIDL_SCALE_NEAREST);
					
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,AGMV_NDS_W,AGMV_NDS_H);
					}
				}
				else{
					if(dither){
						AGMV_DitherImageData(frame1->pixels.pix32,width,height);
					}
				}

				AGMV_Printf("Encoding AGIDL Image Frame - %ld...\n",i);
				AGMV_EncodeFrame(file,agmv,frame1->pixels.pix32);
				AGMV_Printf("Encoded AGIDL Image Frame - %ld...\n",i);
				
				if(AGMV_GetTotalAudioDuration(agmv) != 0){
					AGMV_Printf("Encoding AGMV Audio Chunk - %ld...\n",i);
					AGMV_FillAudioBuffer(agmv->audio,agmv,agmv->audio_type);
					AGMV_CompressAudioBuffer(agmv);
					AGMV_EncodeAudioChunk(file,agmv);
					AGMV_Printf("Encoded AGMV Audio Chunk - %ld...\n",i);	
				}
				
				AGIDL_FreeLBM(frame1);
			}break;
		}
	}

	fclose(file);
	
	if(AGMV_GetTotalAudioDuration(agmv) != 0){
		fclose(agmv->audio);
	}
	
	free(ext);
	DestroyAGMV(agmv); 
	
	if(opt == AGMV_OPT_GBA_I || opt == AGMV_OPT_GBA_II || opt == AGMV_OPT_GBA_III || opt == AGMV_OPT_GBA_IV){
		FILE* file = fopen(filename,"rb");
		fseek(file,0,SEEK_END);
		u32 file_size = ftell(file);
		fseek(file,0,SEEK_SET);
		u8* data = (u8*)malloc(sizeof(u8)*file_size);
		fread(data,1,file_size,file);
		fclose(file);
		
		FILE* out = fopen("GBA_GEN_AGMV.h","w");
		
		fprintf(out,"#ifndef GBA_GEN_AGMV_H\n");
		fprintf(out,"#define GBA_GEN_AGMV_H\n\n");
		fprintf(out,"const unsigned char GBA_AGMV_FILE[%ld] = {\n",file_size);
		
		int i;
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