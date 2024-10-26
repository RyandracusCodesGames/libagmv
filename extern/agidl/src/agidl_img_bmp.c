#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <agidl_cc_core.h>
#include <agidl_img_bmp.h>
#include <agidl_math_utils.h>
#include <agidl_img_compression.h>
#include <agidl_img_error.h>
#include <agidl_file_utils.h>
#include <agidl_mmu_utils.h>

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_bmp.c
*   Date: 9/12/2023
*   Version: 0.4b
*   Updated: 6/23/2024
*   Author: Ryandracus Chapman
*
********************************************/
void AGIDL_SetBMPFilename(AGIDL_BMP *bmp, const char *filename){
	if(bmp->filename != NULL){
		free(bmp->filename);
		bmp->filename = (char*)malloc(strlen(filename)+1);
		AGIDL_FilenameCpy(bmp->filename,filename);
	}
	else{
		bmp->filename = (char*)malloc(strlen(filename)+1);
		AGIDL_FilenameCpy(bmp->filename,filename);
	}
}

void AGIDL_BMPSetWidth(AGIDL_BMP *bmp, int width){
	bmp->header.width = width;
}

void AGIDL_BMPSetHeight(AGIDL_BMP *bmp, int height){
	bmp->header.height = height;
}

void AGIDL_BMPSetClrFmt(AGIDL_BMP *bmp, AGIDL_CLR_FMT fmt){
	bmp->fmt = fmt;
}

void AGIDL_BMPSetPalette(AGIDL_BMP *bmp, AGIDL_ICP palette){
	bmp->palette = palette;
}

void AGIDL_BMPSetMaxDiff(AGIDL_BMP *bmp, u16 max_diff){
	bmp->max_diff = max_diff;
}

void AGIDL_BMPSetICPMode(AGIDL_BMP *bmp, int mode){
	bmp->icp = mode;
}

void AGIDL_BMPSetICPEncoding(AGIDL_BMP* bmp, AGIDL_ICP_ENCODE encode){
	bmp->encode = encode;
}

void AGIDL_BMPSetCompression(AGIDL_BMP *bmp, int compress){
	bmp->compression = compress;
}

void AGIDL_BMPSetRGB(AGIDL_BMP *bmp, int x, int y, u8 r, u8 g, u8 b){
	switch(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp))){
		case 24:{
			AGIDL_BMPSetClr(bmp,x,y,AGIDL_RGB(r,g,b,AGIDL_BMPGetClrFmt(bmp)));
		}break;
		case 16:{
			AGIDL_BMPSetClr16(bmp,x,y,AGIDL_RGB16(r,g,b,AGIDL_BMPGetClrFmt(bmp)));
		}break;
		case 32:{
			AGIDL_BMPSetClr(bmp,x,y,AGIDL_RGBA(r,g,b,0xff,AGIDL_BMPGetClrFmt(bmp)));
		}break;
	}
}

void AGIDL_BMPSetRGBA(AGIDL_BMP *bmp, int x, int y, u8 r, u8 g, u8 b, u8 a){
	AGIDL_CLR_FMT fmt = AGIDL_BMPGetClrFmt(bmp);
	
	if(AGIDL_GetBitCount(fmt) == 32){
		AGIDL_BMPSetClr(bmp,x,y,AGIDL_RGBA(r,g,b,a,fmt));
	}
}

void AGIDL_BMPSetClr(AGIDL_BMP *bmp, int x, int y, COLOR clr){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) != 16){
		AGIDL_SetClr(bmp->pixels.pix32,clr,x,y,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp));
	}
	else{
		AGIDL_SetClr16(bmp->pixels.pix16,AGIDL_CLR_TO_CLR16(clr,AGIDL_BGR_888,AGIDL_BMPGetClrFmt(bmp)),x,y,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp));
	}
}

void AGIDL_BMPSetClr16(AGIDL_BMP *bmp, int x, int y, COLOR16 clr){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp) == 16)){
		AGIDL_SetClr16(bmp->pixels.pix16,clr,x,y,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp));
	}
	else{
		AGIDL_SetClr(bmp->pixels.pix32,AGIDL_CLR16_TO_CLR(clr,AGIDL_RGB_555,AGIDL_BMPGetClrFmt(bmp)),x,y,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp));
	}
}

void AGIDL_ClearBMP(AGIDL_BMP *bmp, COLOR clr){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) != 16){
		AGIDL_ClrMemset(bmp->pixels.pix32,clr,AGIDL_BMPGetSize(bmp));
	}
	else{
		AGIDL_ClrMemset16(bmp->pixels.pix16,(COLOR16)clr,AGIDL_BMPGetSize(bmp));
	}
}

void AGIDL_ClearBMP16(AGIDL_BMP *bmp, COLOR16 clr){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 16){
		AGIDL_ClrMemset16(bmp->pixels.pix16,clr,AGIDL_BMPGetSize(bmp));
	}
	else{
		AGIDL_ClrMemset(bmp->pixels.pix32,AGIDL_CLR16_TO_CLR(clr,AGIDL_RGB_555,AGIDL_BMPGetClrFmt(bmp)),AGIDL_BMPGetSize(bmp));
	}
}

void AGIDL_ClearColorBMP(AGIDL_BMP* bmp, float r, float g, float b){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 16){
		AGIDL_ClearColorBuffer(bmp->pixels.pix16,r,g,b,AGIDL_BMPGetClrFmt(bmp),AGIDL_BMPGetSize(bmp));
	}
	else{
		AGIDL_ClearColorBuffer(bmp->pixels.pix32,r,g,b,AGIDL_BMPGetClrFmt(bmp),AGIDL_BMPGetSize(bmp));
	}
}

void AGIDL_FlushBMP(AGIDL_BMP* bmp){
	AGIDL_ClearBMP(bmp,0);
}

void AGIDL_BMPRGB2BGR(AGIDL_BMP *bmp){
	if(AGIDL_BMPGetClrFmt(bmp) == AGIDL_RGB_888){
		AGIDL_RGB2BGR(bmp->pixels.pix32,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),&bmp->fmt);
	}
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 16 && (bmp->fmt == AGIDL_RGB_555 || bmp->fmt == AGIDL_RGB_565)){
		AGIDL_RGB2BGR16(bmp->pixels.pix16,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),&bmp->fmt);
	}
}

void AGIDL_BMPBGR2RGB(AGIDL_BMP *bmp){
	if(AGIDL_BMPGetClrFmt(bmp) == AGIDL_BGR_888){
		AGIDL_BGR2RGB(bmp->pixels.pix32,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),&bmp->fmt);
	}
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 16 && (bmp->fmt == AGIDL_BGR_555 || bmp->fmt == AGIDL_BGR_565)){
		AGIDL_BGR2RGB16(bmp->pixels.pix16,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),&bmp->fmt);
	}
}

void AGIDL_BMPConvert16BPPTO24BPP(AGIDL_BMP *bmp){
	if(bmp->fmt == AGIDL_RGB_555 || bmp->fmt == AGIDL_BGR_555 || bmp->fmt == AGIDL_RGB_565 || bmp->fmt == AGIDL_BGR_565){
		bmp->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_BMPGetHeight(bmp)*AGIDL_BMPGetWidth(bmp)));
		AGIDL_16BPPTO24BPP(bmp->pixels.pix16,bmp->pixels.pix32,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),&bmp->fmt);
		free(bmp->pixels.pix16);
	}
}

void AGIDL_BMPConvert24BPPTO16BPP(AGIDL_BMP *bmp){	
	if(bmp->fmt == AGIDL_RGB_888 || bmp->fmt == AGIDL_BGR_888 || bmp->fmt == AGIDL_RGBA_8888 || bmp->fmt == AGIDL_ARGB_8888){
		bmp->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_BMPGetHeight(bmp)*AGIDL_BMPGetWidth(bmp)));
		AGIDL_24BPPTO16BPP(bmp->pixels.pix32,bmp->pixels.pix16,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),&bmp->fmt);
		free(bmp->pixels.pix32);
	}
}

void AGIDL_BMP555TO565(AGIDL_BMP* bmp){
	AGIDL_555TO565(bmp->pixels.pix16,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),&bmp->fmt);
}

void AGIDL_BMP565TO555(AGIDL_BMP* bmp){
	AGIDL_565TO555(bmp->pixels.pix16,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),&bmp->fmt);
}

void AGIDL_ColorConvertBMP(AGIDL_BMP* bmp, AGIDL_CLR_FMT dest){
	u8 sbits = AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)), dbits = AGIDL_GetBitCount(dest);
	if(sbits == 16 && dbits == 16){
		AGIDL_ColorConvertImgData(bmp->pixels.pix16,NULL,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),AGIDL_BMPGetClrFmt(bmp),dest);
		AGIDL_BMPSetClrFmt(bmp,dest);
	}
	else if((sbits == 24 || sbits == 32) && (dbits == 24 || dbits == 32)){
		AGIDL_ColorConvertImgData(bmp->pixels.pix32,NULL,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),AGIDL_BMPGetClrFmt(bmp),dest);
		AGIDL_BMPSetClrFmt(bmp,dest);
	}
	else if(sbits == 16 && (dbits == 24 || dbits == 32)){
		bmp->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),dest);
		AGIDL_ColorConvertImgData(bmp->pixels.pix16,bmp->pixels.pix32,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),AGIDL_BMPGetClrFmt(bmp),dest);
		AGIDL_BMPSetClrFmt(bmp,dest);
		free(bmp->pixels.pix16);
	}
	else{
		bmp->pixels.pix16 = (COLOR16*)AGIDL_AllocImgDataMMU(AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),dest);
		AGIDL_ColorConvertImgData(bmp->pixels.pix32,bmp->pixels.pix16,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),AGIDL_BMPGetClrFmt(bmp),dest);
		AGIDL_BMPSetClrFmt(bmp,dest);
		free(bmp->pixels.pix32);
	}
}

COLOR AGIDL_BMPGetClr(AGIDL_BMP *bmp, int x, int y){
	if(x >= 0 && y >= 0 && x < AGIDL_BMPGetWidth(bmp) && y < AGIDL_BMPGetHeight(bmp)){
		return bmp->pixels.pix32[x+y*AGIDL_BMPGetWidth(bmp)];
	}
	else return 0;
}

COLOR16 AGIDL_BMPGetClr16(AGIDL_BMP *bmp, int x, int y){
	if(x >= 0 && y >= 0 && x < AGIDL_BMPGetWidth(bmp) && y < AGIDL_BMPGetHeight(bmp)){
		return bmp->pixels.pix16[x+y*AGIDL_BMPGetWidth(bmp)];
	}
	else return 0;
}

void AGIDL_BMPSyncPix(AGIDL_BMP *bmp, COLOR *clrs){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) != 16){
		AGIDL_ClrMemcpy(bmp->pixels.pix32,clrs,AGIDL_BMPGetSize(bmp));
	}
}

void AGIDL_BMPSyncPix16(AGIDL_BMP *bmp, COLOR16 *clrs){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 16){
		AGIDL_ClrMemcpy16(bmp->pixels.pix16,clrs,AGIDL_BMPGetSize(bmp));
	}
}

void AGIDL_BMPCopyPix(AGIDL_BMP* bmp, COLOR* clrs, u32 count){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) != 16){
		AGIDL_ClrMemcpy(bmp->pixels.pix32,clrs,count);
	}
}

void AGIDL_BMPCopyPix16(AGIDL_BMP* bmp, COLOR16* clrs, u32 count){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 16){
		AGIDL_ClrMemcpy16(bmp->pixels.pix16,clrs,count);
	}
}

void AGIDL_FreeBMP(AGIDL_BMP *bmp){
	if(bmp->filename != NULL){
		free(bmp->filename);
		bmp->filename = NULL;
	}
	
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 16){
		if(bmp->pixels.pix16 != NULL){
			free(bmp->pixels.pix16);
			bmp->pixels.pix16 = NULL;
		}
	}
	else{
		if(bmp->pixels.pix32 != NULL){
			free(bmp->pixels.pix32);
			bmp->pixels.pix32 = NULL;
		}
	}
	
	if(bmp != NULL){
		free(bmp);
		bmp = NULL;
	}
}

int AGIDL_BMPGetWidth(AGIDL_BMP *bmp){
	return bmp->header.width;
}

int AGIDL_BMPGetHeight(AGIDL_BMP *bmp){
	return bmp->header.height;
}

u32 AGIDL_BMPGetSize(AGIDL_BMP* bmp){
	return AGIDL_BMPGetWidth(bmp) * AGIDL_BMPGetHeight(bmp);
}

u16 AGIDL_BMPGetMaxDiff(AGIDL_BMP *bmp){
	return bmp->max_diff;
}

AGIDL_CLR_FMT AGIDL_BMPGetClrFmt(AGIDL_BMP* bmp){
	return bmp->fmt;
}

BMP_IMG_TYPE AGIDL_BMPGetImgType(int bits){
	switch(bits){
		case 4:{
			return BMP_IMG_TYPE_ICP_16;
		}break;
		case 8:{
			return BMP_IMG_TYPE_ICP_256;
		}break;
		case 16:{
			return BMP_IMG_TYPE_HIGH_CLR;
		}break;
		case 24:{
			return BMP_IMG_TYPE_TRUE_CLR;
		}break;
		case 32:{
			return BMP_IMG_TYPE_DEEP_CLR;
		}break;
		case 1:{
			return BMP_IMG_TYPE_RLE;
		}break;
		case 2:{
			return BMP_IMG_TYPE_2BPP_ICP;
		}break;
	}
	return BMP_IMG_TYPE_TRUE_CLR;
}

void AGIDL_BMPEncodeICP(AGIDL_BMP* bmp, FILE* file){
	if(bmp->encode == ICP_ENCODE_THRESHOLD){
		int pass = 0;
		u8 pal_index = 0;
		u32 w = AGIDL_BMPGetWidth(bmp), h = AGIDL_BMPGetHeight(bmp), max_diff = AGIDL_BMPGetMaxDiff(bmp), bitcount = AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp));
		AGIDL_CLR_FMT fmt = AGIDL_BMPGetClrFmt(bmp);
		
		AGIDL_InitICP(&bmp->palette, AGIDL_ICP_256);
		
		int x,y;
		for(y = 0; y < h; y++){
			for(x = 0; x < w; x++){
				COLOR clr = AGIDL_BMPGetClr(bmp,x,y);
				
				if(bitcount == 32){
					u8 r = AGIDL_GetR(clr,fmt);
					u8 g = AGIDL_GetG(clr,fmt);
					u8 b = AGIDL_GetB(clr,fmt);
					clr = AGIDL_RGB(r,g,b,AGIDL_BGR_888);
					AGIDL_AddColorICP(&bmp->palette,pal_index,clr,AGIDL_BGR_888,max_diff,&pass);
				}
				else{
					AGIDL_AddColorICP(&bmp->palette,pal_index,clr,bmp->fmt,max_diff,&pass);
				}
				
				if(pass == 1 && pal_index < 256){
					pal_index++;
				}
				
				pass = 0;
			}
		}
	}
	else{
		AGIDL_EncodeHistogramICP(&bmp->palette,bmp->pixels.pix32,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),AGIDL_BMPGetClrFmt(bmp));
	}
	
	int i;
	for(i = 0; i < 256; i++){
		COLOR clr = bmp->palette.icp.palette_256[i];
		AGIDL_WriteRGB(file,clr,AGIDL_BGR_888,AGIDL_BGR_888);
		AGIDL_WriteByte(file,0);
	}
}

void AGIDL_BMPEncodeNearestICP(AGIDL_BMP* bmp, AGIDL_ICP palette, FILE* file){
	switch(palette.mode){
		case AGIDL_ICP_256:{
			int i;
			for(i = 0; i < 256; i++){
				COLOR clr = palette.icp.palette_256[i];
				AGIDL_WriteRGB(file,clr,palette.fmt,AGIDL_BGR_888);
				AGIDL_WriteByte(file,0);
			}
		}break;
		case AGIDL_ICP_16:{
			int i;
			for(i = 0; i < 16; i++){
				COLOR clr = palette.icp.palette_16[i];
				AGIDL_WriteRGB(file,clr,palette.fmt,AGIDL_BGR_888);
				AGIDL_WriteByte(file,0);
			}
		}break;
		case AGIDL_ICP_16b_256:{
			int i;
			for(i = 0; i < 256; i++){
				COLOR16 clr = palette.icp.palette_16b_256[i];
				AGIDL_WriteRGB(file,clr,palette.fmt,AGIDL_BGR_888);
				AGIDL_WriteByte(file,0);
			}
		}break;
		case AGIDL_ICP_16b_16:{
			int i;
			for(i = 0; i < 16; i++){
				COLOR16 clr = palette.icp.palette_16b_16[i];
				AGIDL_WriteRGB(file,clr,palette.fmt,AGIDL_BGR_888);
				AGIDL_WriteByte(file,0);
			}
		}break;
	}
	
	int padding = 0;
	int pad = AGIDL_BMPGetWidth(bmp);
	
	int count = 0;
	while((pad % 4) != 0){
		pad++;
		count++;
	}
	
	u32 w = AGIDL_BMPGetWidth(bmp), h = AGIDL_BMPGetHeight(bmp), bitcount = AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp));
	AGIDL_CLR_FMT fmt = AGIDL_BMPGetClrFmt(bmp);
	
	int x,y;
	for(y = 0; y < h; y++){
		for(x = 0; x < w; x++){
			if(bitcount == 24 || bitcount  == 32){
				COLOR clr = AGIDL_BMPGetClr(bmp,x,y);
				u8 index = AGIDL_FindNearestColor(palette,clr,fmt);
				AGIDL_WriteByte(file,index);
			}
			else{
				COLOR clr = AGIDL_BMPGetClr16(bmp,x,y);
				u8 index = AGIDL_FindNearestColor(palette,clr,fmt);
				AGIDL_WriteByte(file,index);
			}
		}
		if(pad != 0){
			fwrite(&padding,count,1,file);
		}
	}
}

void AGIDL_BMPEncodeRLE(AGIDL_BMP* bmp, FILE* file){
	u32 width, height, bitcount;
	AGIDL_CLR_FMT fmt;
	
	width = AGIDL_BMPGetWidth(bmp); height = AGIDL_BMPGetHeight(bmp);
	fmt = AGIDL_BMPGetClrFmt(bmp);
	bitcount = AGIDL_GetBitCount(fmt);
	
	int counter = 0;
	int x,y;
	for(y = 0; y < height; y++){
		for(x = 0; x < width; x++){
			COLOR clr = AGIDL_BMPGetClr(bmp,x,y);

			u32 count = AGIDL_EncodeRLE(bmp->pixels.pix32,24,x,y,width,height,255);
			
			x += count - 1;
			
			u8 index = 0;
			if(bitcount == 32){
				u8 r = AGIDL_GetR(clr,fmt);
				u8 g = AGIDL_GetG(clr,fmt);
				u8 b = AGIDL_GetB(clr,fmt);
				clr = AGIDL_RGB(r,g,b,AGIDL_BGR_888);
				index = AGIDL_FindNearestColor(bmp->palette,clr,AGIDL_BGR_888);			
			}
			else{
				index = AGIDL_FindNearestColor(bmp->palette,clr,fmt);
			}
			
			AGIDL_WriteByte(file,count);
			AGIDL_WriteByte(file,index);
			
			if(x == width-1){
				AGIDL_WriteShort(file,0);
				
				if(y == height-1){
					AGIDL_WriteShort(file,0);
					AGIDL_WriteShort(file,1);
				}
			}
		}
	}
}

void AGIDL_BMPEncodeIMG(AGIDL_BMP* bmp, FILE* file){
	u32 w = AGIDL_BMPGetWidth(bmp), h = AGIDL_BMPGetHeight(bmp), bitcount = AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)), max_diff = AGIDL_BMPGetMaxDiff(bmp);
	AGIDL_CLR_FMT fmt = AGIDL_BMPGetClrFmt(bmp);
	
	if(bmp->encode == ICP_ENCODE_THRESHOLD){
		int padding = 0;
		int pad = w % 4;
		int x,y;
		for(y = 0; y < h; y++){
			for(x = 0; x < w; x++){
				COLOR clr = AGIDL_BMPGetClr(bmp,x,y);
				
				u8 index;
				if(bitcount == 32){
					u8 r = AGIDL_GetR(clr,fmt);
					u8 g = AGIDL_GetG(clr,fmt);
					u8 b = AGIDL_GetB(clr,fmt);
					clr = AGIDL_RGB(r,g,b,AGIDL_BGR_888);
					index = AGIDL_FindClosestColor(bmp->palette,clr,AGIDL_BGR_888,max_diff);
				}
				else{
					index = AGIDL_FindClosestColor(bmp->palette,clr,fmt,max_diff);
				}
				AGIDL_WriteByte(file,index);
			}
			if(!(pad) == 0){
				fwrite(&padding,pad,1,file);
			}
		}
	}
	else{
		int padding = 0;
		int pad = w % 4;
		int x,y;
		for(y = 0; y < h; y++){
			for(x = 0; x < w; x++){
				COLOR clr = AGIDL_BMPGetClr(bmp,x,y);
				u8 index = AGIDL_FindNearestColor(bmp->palette,clr,fmt);
				AGIDL_WriteByte(file,index);
			}
			if(!(pad) == 0){
				fwrite(&padding,pad,1,file);
			}
		}
	}
}

void AGIDL_BMPEncodeIMG0(AGIDL_BMP* bmp, FILE* file){
	u32 size = AGIDL_BMPGetSize(bmp);
	u32 width = AGIDL_BMPGetWidth(bmp), height = AGIDL_BMPGetHeight(bmp);
	AGIDL_CLR_FMT fmt = AGIDL_BMPGetClrFmt(bmp);
	
	switch(bmp->fmt){
		case AGIDL_BGR_888:{
			int padding = 0;
			int pad = width % 4;
			
			int i, count;
			for(i = 0, count = 1; i < size; i++, count++){
				COLOR clr = bmp->pixels.pix32[i];
				
				u8 r = AGIDL_GetR(clr,fmt);
				u8 g = AGIDL_GetG(clr,fmt);
				u8 b = AGIDL_GetB(clr,fmt);
				
				AGIDL_WriteByte(file,b);
				AGIDL_WriteByte(file,g);
				AGIDL_WriteByte(file,r);
				
				if(count == width){
					count = 0;
					if(!(pad) == 0){
						fwrite(&padding,pad,1,file);
					}
				}
			}
		}break;
		case AGIDL_RGB_555:{
			if((width % 4) == 0){
				AGIDL_WriteBufClr16(file,bmp->pixels.pix16,width,height);
			}
			else{
				int padding = 0;
				int pad = width * 2;
				
				int pad_count = 0;
				
				while((pad % 4) != 0){
					pad++;
					pad_count++;
				}

				int i, count;
				for(i = 0, count = 1; i < size; i++, count++){
					COLOR16 clr = bmp->pixels.pix16[i];
					
					AGIDL_WriteShort(file,clr);
					
					if(count == width){
						count = 0;
						fwrite(&padding,pad_count,1,file);
					}
				}
			}
		}break;
		case AGIDL_RGBA_8888:{
			AGIDL_WriteBufBGRA(file,bmp->pixels.pix32,width,height,fmt);
		}break;
	}
}

int AGIDL_BMPDecodeHeader(AGIDL_BMP* bmp, FILE* file){
	bmp->header.magic = AGIDL_ReadShort(file);
	bmp->header.file_size = AGIDL_ReadLong(file);
	bmp->header.reserved1 = AGIDL_ReadShort(file);
	bmp->header.reserved2 = AGIDL_ReadShort(file);
	bmp->header.offset = AGIDL_ReadLong(file);
	bmp->header.header_size = AGIDL_ReadLong(file);
	
	if(bmp->header.header_size == 12){
		bmp->IsOS2 = TRUE;
		bmp->header.width = AGIDL_ReadShort(file);
		bmp->header.height = AGIDL_ReadShort(file);
		bmp->header.num_of_planes = AGIDL_ReadShort(file);
		bmp->header.bits = AGIDL_ReadShort(file);
		
		if(bmp->header.width > 65535 || bmp->header.height > 65535 || !(bmp->header.bits == 1 || bmp->header.bits == 2 || 
		bmp->header.bits == 4 || bmp->header.bits == 8 || bmp->header.bits == 16 || bmp->header.bits == 24 || bmp->header.bits != 32)){
			return INVALID_HEADER_FORMATTING_ERROR;
		}
		
		return NO_IMG_ERROR;
	}
	else{
		bmp->IsOS2 = FALSE;
		bmp->header.width = AGIDL_ReadLong(file);
		bmp->header.height = AGIDL_ReadLong(file);
		bmp->header.num_of_planes = AGIDL_ReadShort(file);
		bmp->header.bits = AGIDL_ReadShort(file);
		bmp->header.compress = AGIDL_ReadLong(file);
		bmp->header.img_size = AGIDL_ReadLong(file);
		bmp->header.x_resolution = AGIDL_ReadLong(file);
		bmp->header.y_resolution = AGIDL_ReadLong(file);
		bmp->header.num_of_colors = AGIDL_ReadLong(file);
		bmp->header.important_colors = AGIDL_ReadLong(file);
	}

	if(bmp->header.magic != 0x4d42 || bmp->header.file_size > 64000000 || bmp->header.num_of_colors > 512 || !(bmp->header.bits == 1 || bmp->header.bits == 2 || 
	bmp->header.bits == 4 || bmp->header.bits == 8 || bmp->header.bits == 16 || bmp->header.bits == 24 || bmp->header.bits == 32) ||
	bmp->header.reserved1 != 0 || bmp->header.reserved2 != 0){
		return INVALID_HEADER_FORMATTING_ERROR;
	}
	else if((bmp->header.compress == 1 && bmp->header.num_of_colors == 0) || (bmp->header.bits == 1 && bmp->header.num_of_colors > 2) || (bmp->header.bits == 2 && bmp->header.num_of_colors > 4) ||
	(bmp->header.bits == 4 && bmp->header.num_of_colors > 16)){
		return INVALID_IMG_FORMATTING_ERROR;
	}
	else return NO_IMG_ERROR;
}

void AGIDL_BMPDecodeIMG(AGIDL_BMP* bmp, FILE* file, BMP_IMG_TYPE img_type){
	if(bmp->header.compress != 1 && bmp->header.compress != 2){
		switch(img_type){
			case BMP_IMG_TYPE_TRUE_CLR:{
				AGIDL_BMPSetClrFmt(bmp,AGIDL_BGR_888);
				
				u32 size = AGIDL_BMPGetSize(bmp), width = AGIDL_BMPGetWidth(bmp);
				u32 padding = AGIDL_BMPGetWidth(bmp) % 4;
				
				bmp->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(size));
				
				u32* pixels = bmp->pixels.pix32;
				
				int i, count;
				for(i = 0, count = 1; i < size; i++, count++){
					COLOR clr = AGIDL_ReadRGB(file,AGIDL_BMPGetClrFmt(bmp));
					
					pixels[i] = clr;
					
					if(count == width){
						count = 0;
						fseek(file,padding,SEEK_CUR);
					}
				}
			}break;
			case BMP_IMG_TYPE_HIGH_CLR:{
				AGIDL_BMPSetClrFmt(bmp,AGIDL_RGB_555);
				bmp->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_BMPGetSize(bmp)));
				
				u32 size = AGIDL_BMPGetSize(bmp);
				u32 width = AGIDL_BMPGetWidth(bmp);
				
				u16* pixels = bmp->pixels.pix16;
				
				if((width % 4) == 0){
					AGIDL_ReadBufRGB16(file,bmp->pixels.pix16,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp));
				}
				else{
					int padding = width % 4;
					int i, count;
					for(i = 0, count = 1; i < size; i++, count++){
						COLOR16 clr = AGIDL_ReadShort(file);
						
						pixels[i] = clr;
						
						if(count == width){
							count = 0;
							fseek(file,2,SEEK_CUR);
						}
					}
				}
			}break;
			case BMP_IMG_TYPE_DEEP_CLR:{
				AGIDL_BMPSetClrFmt(bmp,AGIDL_RGBA_8888);
				bmp->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_BMPGetSize(bmp)));
				AGIDL_ReadBufBGRA(file,bmp->pixels.pix32,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp));
			}break;
			case BMP_IMG_TYPE_ICP_256:{
				AGIDL_BMPSetClrFmt(bmp,AGIDL_BGR_888);
				
				int pad = AGIDL_BMPGetWidth(bmp);
				int count = 0;
				
				while((pad % 4) != 0){
					pad++;
					count++;
				}
			
				bmp->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_BMPGetWidth(bmp)*AGIDL_BMPGetHeight(bmp)));
				
				
				if(bmp->IsOS2 == FALSE){
					int i;
					for(i = 0; i < 256; i++){
						COLOR clr = AGIDL_ReadRGB(file,AGIDL_BMPGetClrFmt(bmp));
						bmp->palette.icp.palette_256[i] = clr;
						fseek(file,1,SEEK_CUR);
					}
				}
				else{
					u16 num_of_colors = 1 << bmp->header.bits;
					
					if(num_of_colors == 0){
						num_of_colors = 256;
					}
					
					int i;
					for(i = 0; i < num_of_colors; i++){
						COLOR clr = AGIDL_ReadRGB(file,AGIDL_BMPGetClrFmt(bmp));
						bmp->palette.icp.palette_256[i] = clr;
					}
				}
				
				u32 w = AGIDL_BMPGetWidth(bmp);
				u32 h = AGIDL_BMPGetHeight(bmp);
				
				int x,y;
				for(y = 0; y < h; y++){
					for(x = 0; x < w; x++){
						u8 index = AGIDL_ReadByte(file);
						AGIDL_BMPSetClr(bmp,x,y,bmp->palette.icp.palette_256[index]);
					}
					if(pad){
						fseek(file,count,SEEK_CUR);
					}
				}
			}break;
			case BMP_IMG_TYPE_ICP_16:{
				AGIDL_BMPSetClrFmt(bmp,AGIDL_BGR_888);
				
				bmp->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_BMPGetWidth(bmp)*AGIDL_BMPGetHeight(bmp)));

				u32 palette[16];
				
				u32 width = AGIDL_BMPGetWidth(bmp);
				u32 height = AGIDL_BMPGetHeight(bmp);
				
				int i;
				for(i = 0; i < bmp->header.num_of_colors; i++){
					palette[i] = AGIDL_ReadRGB(file,AGIDL_BGR_888);
					fseek(file,1,SEEK_CUR);
				}
				
				int x,y;
				for(y = 0; y < height; y++){
					for(x = 0; x < width; x += 2){
						u8 index = AGIDL_ReadByte(file);
						
						int j, count;
						for(j = 7, count = 0; j >= 0; j -= 4, count++){
							u8 msb  = index >> j;
							u8 msb2 = index >> (j-1);
							u8 lsb2 = index >> (j-2);
							u8 lsb  = index >> (j-3);

							u8 index = ((msb & 1) << 3) + ((msb2 & 1) << 2) + ((lsb2 & 1) << 1) + (lsb & 1);

							AGIDL_BMPSetClr(bmp,x+count,y,palette[index]);
						}
					}
				}
			}break;
			case BMP_IMG_TYPE_1BPP_ICP:{
				AGIDL_BMPSetClrFmt(bmp,AGIDL_BGR_888);
				
				bmp->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_BMPGetSize(bmp)));
				
				u32 palette[2];
				
				int i;
				for(i = 0; i < 2; i++){
					palette[i] = AGIDL_ReadRGB(file,AGIDL_BGR_888);
					fseek(file,1,SEEK_CUR);
				}
				
				u32 width = AGIDL_BMPGetWidth(bmp);
				u32 height = AGIDL_BMPGetHeight(bmp);

				int x,y;
				for(y = 0; y < height; y++){
					for(x = 0; x < width; x += 8){
						u8 index = AGIDL_ReadByte(file);
						
						int j;
						for(j = 7; j >= 0; j--){
							u8 binary = index >> j;
							
							if(binary & 1){
								AGIDL_BMPSetClr(bmp,x+(8-j),y,palette[1]);
							}
							else{
								AGIDL_BMPSetClr(bmp,x+(8-j),y,palette[0]);
							}
						}
					}
				}
			}break;
			case BMP_IMG_TYPE_2BPP_ICP:{
				AGIDL_BMPSetClrFmt(bmp,AGIDL_BGR_888);
				
				bmp->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_BMPGetSize(bmp)));
				
				u32 palette[4];
				
				int i;
				for(i = 0; i < 4; i++){
					palette[i] = AGIDL_ReadRGB(file,AGIDL_BGR_888);
					fseek(file,1,SEEK_CUR);
				}
				
				u32 width = AGIDL_BMPGetWidth(bmp);
				u32 height = AGIDL_BMPGetHeight(bmp);

				int x,y;
				for(y = 0; y < height; y++){
					for(x = 0; x < width; x += 4){
						u8 index = AGIDL_ReadByte(file);
						
						int j, count;
						for(j = 7, count = 0; j >= 0; j -= 2, count++){
							u8 left = index >> j;
							u8 right = index >> (j-1);

							u8 index = ((left & 1) << 1) + (right & 1);

							AGIDL_BMPSetClr(bmp,x+count,y,palette[index]);
						}
					}
				}
			}break;
		}
	}	
}

void AGIDL_BMPDecodeRLE(AGIDL_BMP* bmp, FILE* file, BMP_IMG_TYPE img_type){
	if(bmp->header.compress){
		switch(img_type){
			case BMP_IMG_TYPE_ICP_256:{
				AGIDL_BMPSetClrFmt(bmp,AGIDL_BGR_888);
				int i;
				for(i = 0; i < bmp->header.num_of_colors; i++){
					bmp->palette.icp.palette_256[i] = AGIDL_ReadRGB(file,AGIDL_BGR_888);
					fseek(file,1,SEEK_CUR);
				}
				
				bmp->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_BMPGetWidth(bmp)*(AGIDL_BMPGetHeight(bmp))));
				
				u32 count = 0;
				
				u32 width = AGIDL_BMPGetWidth(bmp);
				u32 height = AGIDL_BMPGetHeight(bmp);
				
				int x,y;
				for(y = 0; y < height; y++){
					for(x = 0; x < width; x++){
						u8 rle = AGIDL_ReadByte(file);
						u8 index = AGIDL_ReadByte(file);
						
						if(rle != 0){
							int i;
							for(i = 0; i < rle; i++){
								AGIDL_BMPSetClr(bmp,x+i,y,bmp->palette.icp.palette_256[index]);
							}
							
							x += rle - 1;
						}
						else{
							int i;
							for(i = 0; i < index; i++){
								u8 byte = AGIDL_ReadByte(file);
								AGIDL_BMPSetClr(bmp,x+i,y,bmp->palette.icp.palette_256[byte]);
							}
							
							x += index - 1;

							if((index % 2) != 0){
								fseek(file,1,SEEK_CUR);
							}
						}
						
					} 
				}
			}break;
			case BMP_IMG_TYPE_ICP_16:{
				AGIDL_BMPSetClrFmt(bmp,AGIDL_RGB_888);
				
				int i;
				for(i = 0; i < bmp->header.num_of_colors; i++){
					bmp->palette.icp.palette_16[i] = AGIDL_ReadRGB(file,AGIDL_RGB_888);
					fseek(file,1,SEEK_CUR);
				}
				
				if(img_type == BMP_IMG_TYPE_ICP_16){
					bmp->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_BMPGetWidth(bmp)*(AGIDL_BMPGetHeight(bmp))));
					
					int x,y;
					for(y = 0; y < AGIDL_BMPGetHeight(bmp); y++){
						for(x = 0; x < AGIDL_BMPGetWidth(bmp); x++){
							u8 rle = AGIDL_ReadByte(file);
							u8 index = AGIDL_ReadByte(file);
				
							if(rle == 0 && index != 0){
								int i;
								for(i = 0; i < index / 2; i++){
									u8 ind = AGIDL_ReadByte(file);
									
									u8 backind = (ind & 0xf) , frontind = ((ind & 0xff) >> 4);
									AGIDL_BMPSetClr(bmp,x+(i*2),y,bmp->palette.icp.palette_16[frontind]);
									AGIDL_BMPSetClr(bmp,x+((i*2)+1),y,bmp->palette.icp.palette_16[backind]);
								}
								
								x += index - 1;
							}
							else{
								if((index & 0xf) == ((index & 0xff) >> 4)){
								int i;
								for(i = 0; i < rle; i++){
									AGIDL_BMPSetClr(bmp,x+i,y,bmp->palette.icp.palette_16[index&0xf]);
								}
								
								x += rle - 1;
								
								if(x == AGIDL_BMPGetWidth(bmp) && rle != 0 && index != 0){
									fread(&rle,1,1,file);
									fread(&index,1,1,file);
									
									if(rle == 0 && index == 0){
										break;
									}
								}

								else{
									u8 frontindex = (index & 0xff) >> 4, backindex = (index & 0xf);
									int i;
									for(i = 0; i < rle; i++){
										if((i % 2) == 0){	
											AGIDL_BMPSetClr(bmp,x+i,y,bmp->palette.icp.palette_16[frontindex]);
										}
										else{
											AGIDL_BMPSetClr(bmp,x+i,y,bmp->palette.icp.palette_16[backindex]);
										}
									}
									
									x += rle - 1;
									
									if(x == AGIDL_BMPGetWidth(bmp) && rle != 0 && index != 0){
										fread(&rle,1,1,file);
										fread(&index,1,1,file);
										
										if(rle == 0 && index == 0){
											break;
										}
									}
								}
							}								
						}
					}
				}
				}
			}break;
		}
	}
}

AGIDL_BMP * AGIDL_LoadBMP(char* filename){
	AGIDL_BMP *bmp = (AGIDL_BMP*)malloc(sizeof(AGIDL_BMP));
	
	bmp->filename = (char*)malloc(strlen(filename)+1);
	AGIDL_FilenameCpy(bmp->filename,filename);
	AGIDL_BMPSetICPEncoding(bmp,ICP_ENCODE_THRESHOLD);
	
	FILE* file = fopen(filename,"rb");
	
	if(file == NULL){
		printf("%s - %s!\n",AGIDL_Error2Str(FILE_NOT_LOCATED_IMG_ERROR),filename);
		return NULL;
	}
	
	int error = AGIDL_BMPDecodeHeader(bmp,file);
	
	if(error != NO_IMG_ERROR){
		printf("%s\n",AGIDL_Error2Str(error));
		return NULL;
	}
	
	BMP_IMG_TYPE img_type = AGIDL_BMPGetImgType(bmp->header.bits);
	
	AGIDL_BMPDecodeIMG(bmp,file,img_type);
	AGIDL_BMPDecodeRLE(bmp,file,img_type);

	fclose(file);
	
	return bmp;
}

AGIDL_BMP * AGIDL_CreateBMP(const char *filename, int width, int height, AGIDL_CLR_FMT fmt){
	AGIDL_BMP* bmp = (AGIDL_BMP*)malloc(sizeof(AGIDL_BMP));
	bmp->filename = (char*)malloc(strlen(filename)+1);
	AGIDL_FilenameCpy(bmp->filename,filename);
	AGIDL_BMPSetWidth(bmp,width);
	AGIDL_BMPSetHeight(bmp,height);
	AGIDL_BMPSetClrFmt(bmp,fmt);
	AGIDL_BMPSetICPMode(bmp,0);
	AGIDL_BMPSetCompression(bmp,0);
	AGIDL_BMPSetMaxDiff(bmp,7);
	AGIDL_BMPSetICPEncoding(bmp,ICP_ENCODE_THRESHOLD);
	
	if(AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32){
		bmp->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(width*height));
	}
	else{
		bmp->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(width*height));
	}
	
	return bmp;
}

AGIDL_BMP* AGIDL_BMPCpyImg(AGIDL_BMP* bmp){
	AGIDL_BMP* bmpcpy = AGIDL_CreateBMP("bmpcpy.bmp",AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),bmp->fmt);
	AGIDL_BMPSetICPMode(bmpcpy,bmp->icp);
	AGIDL_BMPSetCompression(bmpcpy,bmp->compression);
	AGIDL_BMPSetMaxDiff(bmpcpy,AGIDL_BMPGetMaxDiff(bmp));
	AGIDL_BMPSetICPEncoding(bmpcpy,bmp->encode);
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 24 || AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 32){
		AGIDL_BMPSyncPix(bmpcpy,bmp->pixels.pix32);
	}
	else{
		AGIDL_BMPSyncPix16(bmpcpy,bmp->pixels.pix16);
	}
	return bmpcpy;
}

void AGIDL_BMPEncodeHeader(AGIDL_BMP* bmp, FILE* file){
	bmp->header.magic = 0x4d42;
	bmp->header.reserved1 = 0;
	bmp->header.reserved2 = 0;
	bmp->header.offset = 54;
	bmp->header.header_size = 40;
	
	if(bmp->fmt == AGIDL_RGB_888 || bmp->fmt == AGIDL_BGR_888){
		bmp->header.bits = 24;
		bmp->header.file_size = 54 + (AGIDL_BMPGetWidth(bmp) * AGIDL_BMPGetHeight(bmp) * 3);
	}
	else if(AGIDL_GetBitCount(bmp->fmt) == 16){
		bmp->header.bits = 16;
		bmp->header.file_size = 54 + (AGIDL_BMPGetWidth(bmp) * AGIDL_BMPGetHeight(bmp) * 2);
	}
	else{
		bmp->header.bits = 32;
		bmp->header.file_size = 54 + (AGIDL_BMPGetWidth(bmp) * AGIDL_BMPGetHeight(bmp) * 4);
	}
	
	bmp->header.compress = 0;
	bmp->header.img_size = (AGIDL_BMPGetWidth(bmp) * AGIDL_BMPGetHeight(bmp) * 3);
	bmp->header.x_resolution = 0;
	bmp->header.y_resolution = 0;
	bmp->header.num_of_colors = 0;
	bmp->header.important_colors = 0;
	bmp->header.num_of_planes = 1;
	
	if(bmp->icp == YES_ICP){
		bmp->header.bits = 8;
		bmp->header.num_of_colors = 256;
		bmp->header.important_colors = 256;
		bmp->header.offset = 54 + (256*4);
		bmp->header.file_size = (54 + (AGIDL_BMPGetWidth(bmp)*AGIDL_BMPGetHeight(bmp)) + (256*4));
		bmp->header.img_size = (AGIDL_BMPGetWidth(bmp)*AGIDL_BMPGetHeight(bmp));
	}
	
	if(bmp->compression == 1){
		bmp->header.bits = 8;
		bmp->header.compress = 1;
		bmp->header.num_of_colors = 256;
		bmp->header.important_colors = 256;
		bmp->header.offset = 54 + (256*4);
		bmp->header.file_size = (54 + (AGIDL_BMPGetWidth(bmp)*AGIDL_BMPGetHeight(bmp)) + (256*4)) * 0.75f;
		bmp->header.img_size = (AGIDL_BMPGetWidth(bmp)*AGIDL_BMPGetHeight(bmp)) - bmp->header.offset;
	}
	
	if(bmp->compression != 1 && bmp->compression != 0){
		bmp->compression = 0;
	}
	
	AGIDL_WriteShort(file,bmp->header.magic);
	AGIDL_WriteLong(file,bmp->header.file_size);
	AGIDL_WriteShort(file,bmp->header.reserved1);
	AGIDL_WriteShort(file,bmp->header.reserved2);
	AGIDL_WriteLong(file,bmp->header.offset);
	AGIDL_WriteLong(file,bmp->header.header_size);
	AGIDL_WriteLong(file,bmp->header.width);
	AGIDL_WriteLong(file,bmp->header.height);
	AGIDL_WriteShort(file,bmp->header.num_of_planes);
	AGIDL_WriteShort(file,bmp->header.bits);
	AGIDL_WriteLong(file,bmp->header.compress);
	AGIDL_WriteLong(file,bmp->header.img_size);
	AGIDL_WriteLong(file,bmp->header.x_resolution);
	AGIDL_WriteLong(file,bmp->header.y_resolution);
	AGIDL_WriteLong(file,bmp->header.num_of_colors);
	AGIDL_WriteLong(file,bmp->header.important_colors);
}

void AGIDL_ExportBMP(AGIDL_BMP *bmp){
	FILE *file = fopen(bmp->filename,"wb");
	
	AGIDL_BMPEncodeHeader(bmp,file);
	
	if(bmp->icp == TRUE && bmp->compression != TRUE){
		switch(bmp->fmt){
			case AGIDL_RGB_888:{
				AGIDL_BMPRGB2BGR(bmp);
				AGIDL_BMPEncodeICP(bmp,file);
				AGIDL_BMPEncodeIMG(bmp,file);
				AGIDL_BMPBGR2RGB(bmp);
			}break;
			case AGIDL_BGR_888:{
				AGIDL_BMPEncodeICP(bmp,file);
				AGIDL_BMPEncodeIMG(bmp,file);
			}break;
			case AGIDL_RGBA_8888:{
				AGIDL_BMPEncodeICP(bmp,file);
				AGIDL_BMPEncodeIMG(bmp,file);
			}break;
			case AGIDL_ARGB_8888:{
				AGIDL_BMPEncodeICP(bmp,file);
				AGIDL_BMPEncodeIMG(bmp,file);
			}break;
			case AGIDL_RGB_555:{
				AGIDL_BMPConvert16BPPTO24BPP(bmp);
				AGIDL_BMPRGB2BGR(bmp);
				AGIDL_BMPEncodeICP(bmp,file);
				AGIDL_BMPEncodeIMG(bmp,file);
				AGIDL_BMPBGR2RGB(bmp);
				AGIDL_BMPConvert24BPPTO16BPP(bmp);	
			}break;
			case AGIDL_BGR_555:{
				AGIDL_BMPConvert16BPPTO24BPP(bmp);
				AGIDL_BMPEncodeICP(bmp,file);
				AGIDL_BMPEncodeIMG(bmp,file);
				AGIDL_BMPConvert24BPPTO16BPP(bmp);	
			}break;
			case AGIDL_RGB_565:{
				AGIDL_BMP565TO555(bmp);
				AGIDL_BMPConvert16BPPTO24BPP(bmp);
				AGIDL_BMPRGB2BGR(bmp);
				AGIDL_BMPEncodeICP(bmp,file);
				AGIDL_BMPEncodeIMG(bmp,file);
				AGIDL_BMPBGR2RGB(bmp);
				AGIDL_BMPConvert24BPPTO16BPP(bmp);
				AGIDL_BMP555TO565(bmp);
			}break;
			case AGIDL_BGR_565:{
				AGIDL_BMP565TO555(bmp);
				AGIDL_BMPConvert16BPPTO24BPP(bmp);
				AGIDL_BMPEncodeICP(bmp,file);
				AGIDL_BMPEncodeIMG(bmp,file);
				AGIDL_BMPConvert24BPPTO16BPP(bmp);
				AGIDL_BMP555TO565(bmp);
			}break;
		}
	}
	else if(bmp->compression != TRUE){
		switch(bmp->fmt){
			case AGIDL_BGR_888:{
				AGIDL_BMPEncodeIMG0(bmp,file);
			}break;
			case AGIDL_RGB_888:{
				AGIDL_BMPRGB2BGR(bmp);
				AGIDL_BMPEncodeIMG0(bmp,file);
				AGIDL_BMPBGR2RGB(bmp);
			}break;
			case AGIDL_RGB_555:{
				AGIDL_BMPEncodeIMG0(bmp,file);
			}break;
			case AGIDL_BGR_555:{
				AGIDL_BMPBGR2RGB(bmp);
				AGIDL_BMPEncodeIMG0(bmp,file);
				AGIDL_BMPRGB2BGR(bmp);
			}break;
			case AGIDL_RGB_565:{
				AGIDL_BMP565TO555(bmp);
				AGIDL_BMPEncodeIMG0(bmp,file);
				AGIDL_BMP555TO565(bmp);
			}break;
			case AGIDL_BGR_565:{
				AGIDL_BMP565TO555(bmp);
				AGIDL_BMPBGR2RGB(bmp);
				AGIDL_BMPEncodeIMG0(bmp,file);
				AGIDL_BMPRGB2BGR(bmp);
				AGIDL_BMP555TO565(bmp);
			}break;
			case AGIDL_RGBA_8888:{
				AGIDL_BMPEncodeIMG0(bmp,file);
			}break;
			case AGIDL_ARGB_8888:{
				fwrite(bmp->pixels.pix32,4,AGIDL_BMPGetSize(bmp),file);
			}break;
		}
	}
	else{
		switch(AGIDL_BMPGetClrFmt(bmp)){
			case AGIDL_RGB_888:{
				AGIDL_BMPRGB2BGR(bmp);
				AGIDL_BMPEncodeICP(bmp,file);
				AGIDL_BMPEncodeRLE(bmp,file);
				AGIDL_BMPBGR2RGB(bmp);
			}break;
			case AGIDL_BGR_888:{
				AGIDL_BMPEncodeICP(bmp,file);
				AGIDL_BMPEncodeRLE(bmp,file);
			}break;
			case AGIDL_RGBA_8888:{
				AGIDL_BMPEncodeICP(bmp,file);
				AGIDL_BMPEncodeRLE(bmp,file);
			}break;
			case AGIDL_ARGB_8888:{
				AGIDL_BMPEncodeICP(bmp,file);
				AGIDL_BMPEncodeRLE(bmp,file);
			}break;
			case AGIDL_RGB_555:{
				AGIDL_BMPRGB2BGR(bmp);
				AGIDL_BMPConvert16BPPTO24BPP(bmp);
				AGIDL_BMPEncodeICP(bmp,file);
				AGIDL_BMPEncodeRLE(bmp,file);
				AGIDL_BMPConvert24BPPTO16BPP(bmp);
				AGIDL_BMPBGR2RGB(bmp);
			}break;
			case AGIDL_BGR_555:{
				AGIDL_BMPConvert16BPPTO24BPP(bmp);
				AGIDL_BMPEncodeICP(bmp,file);
				AGIDL_BMPEncodeRLE(bmp,file);
				AGIDL_BMPConvert24BPPTO16BPP(bmp);
			}break;
			case AGIDL_RGB_565:{
				AGIDL_BMP565TO555(bmp);
				AGIDL_BMPRGB2BGR(bmp);
				AGIDL_BMPConvert16BPPTO24BPP(bmp);
				AGIDL_BMPEncodeICP(bmp,file);
				AGIDL_BMPEncodeRLE(bmp,file);
				AGIDL_BMPConvert24BPPTO16BPP(bmp);
				AGIDL_BMPBGR2RGB(bmp);
				AGIDL_BMP555TO565(bmp);
			}break;
			case AGIDL_BGR_565:{
				AGIDL_BMP565TO555(bmp);
				AGIDL_BMPConvert16BPPTO24BPP(bmp);
				AGIDL_BMPEncodeICP(bmp,file);
				AGIDL_BMPEncodeRLE(bmp,file);
				AGIDL_BMPConvert24BPPTO16BPP(bmp);
				AGIDL_BMP555TO565(bmp);
			}break;
		}
	}
	
	fclose(file);
}