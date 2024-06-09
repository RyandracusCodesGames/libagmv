#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "agidl_img_gxt.h"

#include <agidl_mmu_utils.h>

#include "agidl_cc_core.h"
#include "agidl_math_utils.h"
#include "agidl_img_error.h"

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_gxt.c
*   Date: 11/19/2023
*   Version: 0.1b
*   Updated: 2/21/2024
*   Author: Ryandracus Chapman
*
********************************************/

void AGIDL_SetGXTFilename(AGIDL_GXT* gxt, const char* filename){
	gxt->filename = (char*)realloc(gxt->filename,strlen(filename));
	AGIDL_FilenameCpy(gxt->filename,filename);
}

void AGIDL_GXTSetWidth(AGIDL_GXT* gxt, int width){
	gxt->header.header.width = width;
}

void AGIDL_GXTSetHeight(AGIDL_GXT* gxt, int height){
	gxt->header.header.height = height;
}

void AGIDL_GXTSetClrFmt(AGIDL_GXT* gxt, AGIDL_CLR_FMT fmt){
	gxt->fmt = fmt;
}

void AGIDL_GXTSetICPMode(AGIDL_GXT* gxt, int mode){
	gxt->icp = mode;
}

void AGIDL_GXTSetMaxDiff(AGIDL_GXT* gxt, int max_diff){
	gxt->max_diff = max_diff;
}

void AGIDL_GXTSetClr(AGIDL_GXT *gxt, int x, int y, COLOR clr){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) != 16){
		AGIDL_SetClr(gxt->pixels.pix32,clr,x,y,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt));
	}
	else{
		AGIDL_SetClr16(gxt->pixels.pix16,AGIDL_CLR_TO_CLR16(clr,AGIDL_BGR_888,AGIDL_GXTGetClrFmt(gxt)),x,y,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt));
	}
}

void AGIDL_GXTSetClr16(AGIDL_GXT *gxt, int x, int y, COLOR16 clr){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt) == 16)){
		AGIDL_SetClr16(gxt->pixels.pix16,clr,x,y,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt));
	}
	else{
		AGIDL_SetClr(gxt->pixels.pix32,AGIDL_CLR16_TO_CLR(clr,AGIDL_RGB_555,AGIDL_GXTGetClrFmt(gxt)),x,y,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt));
	}
}

void AGIDL_GXTSetRGB(AGIDL_GXT* gxt, int x, int y, u8 r, u8 g, u8 b){
	switch(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt))){
		case 24:{
			AGIDL_GXTSetClr(gxt,x,y,AGIDL_RGB(r,g,b,gxt->fmt));
		}break;
		case 16:{
			AGIDL_GXTSetClr16(gxt,x,y,AGIDL_RGB16(r,g,b,gxt->fmt));
		}break;
		case 32:{
			AGIDL_GXTSetClr(gxt,x,y,AGIDL_RGBA(r,g,b,0xff,gxt->fmt));
		}break;
	}
}

void AGIDL_ClearGXT(AGIDL_GXT *gxt, COLOR clr){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) != 16){
		AGIDL_ClrMemset(gxt->pixels.pix32,clr,AGIDL_GXTGetSize(gxt));
	}
	else{
		AGIDL_ClrMemset16(gxt->pixels.pix16,(COLOR16)clr,AGIDL_GXTGetSize(gxt));
	}
}

void AGIDL_ClearGXT16(AGIDL_GXT *gxt, COLOR16 clr){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 16){
		AGIDL_ClrMemset16(gxt->pixels.pix16,clr,AGIDL_GXTGetSize(gxt));
	}
	else{
		AGIDL_ClrMemset(gxt->pixels.pix32,AGIDL_CLR16_TO_CLR(clr,AGIDL_RGB_555,AGIDL_GXTGetClrFmt(gxt)),AGIDL_GXTGetSize(gxt));
	}
}

void AGIDL_ClearColorGXT(AGIDL_GXT* gxt, float r, float g, float b){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 16){
		AGIDL_ClearColorBuffer(gxt->pixels.pix16,r,g,b,AGIDL_GXTGetClrFmt(gxt),AGIDL_GXTGetSize(gxt));
	}
	else{
		AGIDL_ClearColorBuffer(gxt->pixels.pix32,r,g,b,AGIDL_GXTGetClrFmt(gxt),AGIDL_GXTGetSize(gxt));
	}
}

void AGIDL_FlushGXT(AGIDL_GXT* gxt){
	AGIDL_ClearGXT(gxt,0);
}

int AGIDL_GXTGetWidth(AGIDL_GXT* gxt){
	return gxt->header.header.width;
}

int AGIDL_GXTGetHeight(AGIDL_GXT* gxt){
	return gxt->header.header.height;
}

u32 AGIDL_GXTGetSize(AGIDL_GXT* gxt){
	return AGIDL_GXTGetWidth(gxt) * AGIDL_GXTGetHeight(gxt);
}

AGIDL_CLR_FMT AGIDL_GXTGetClrFmt(AGIDL_GXT* gxt){
	return gxt->fmt;
}

int AGIDL_GXTGetMaxDiff(AGIDL_GXT* gxt){
	return gxt->max_diff;
}

COLOR AGIDL_GXTGetClr(AGIDL_GXT* gxt, int x, int y){
	if(x >= 0 && y >= 0 && x < AGIDL_GXTGetWidth(gxt) && y < AGIDL_GXTGetHeight(gxt)){
		return gxt->pixels.pix32[x+y*AGIDL_GXTGetWidth(gxt)];
	}
}

COLOR16 AGIDL_GXTGetClr16(AGIDL_GXT* gxt, int x, int y){
	if(x >= 0 && y >= 0 && x < AGIDL_GXTGetWidth(gxt) && y < AGIDL_GXTGetHeight(gxt)){
		return gxt->pixels.pix16[x+y*AGIDL_GXTGetWidth(gxt)];
	}
}

void AGIDL_GXTRGB2BGR(AGIDL_GXT* gxt){
	if(gxt->fmt == AGIDL_RGB_888){
		AGIDL_RGB2BGR(gxt->pixels.pix32,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),&gxt->fmt);
	}
	if(gxt->fmt == AGIDL_RGB_555 || gxt->fmt == AGIDL_RGB_565){
		AGIDL_RGB2BGR16(gxt->pixels.pix16,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),&gxt->fmt);
	}
}

void AGIDL_GXTBGR2RGB(AGIDL_GXT* gxt){
	if(gxt->fmt == AGIDL_BGR_888){
		AGIDL_BGR2RGB(gxt->pixels.pix32,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),&gxt->fmt);
	}
	if(gxt->fmt == AGIDL_BGR_555 || gxt->fmt == AGIDL_BGR_565){
		AGIDL_BGR2RGB16(gxt->pixels.pix16,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),&gxt->fmt);
	}
}

void AGIDL_GXTConvert24BPPTO16BPP(AGIDL_GXT* gxt){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 24){
		gxt->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*AGIDL_GXTGetSize(gxt));
		AGIDL_24BPPTO16BPP(gxt->pixels.pix32,gxt->pixels.pix16,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),&gxt->fmt);
		free(gxt->pixels.pix32);
	}
}

void AGIDL_GXTConvert16BPPTO24BPP(AGIDL_GXT* gxt){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 16){
		gxt->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*AGIDL_GXTGetSize(gxt));
		AGIDL_24BPPTO16BPP(gxt->pixels.pix32,gxt->pixels.pix16,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),&gxt->fmt);
		free(gxt->pixels.pix32);
	}
}

void AGIDL_GXTConvert555TO565(AGIDL_GXT* gxt){
	AGIDL_555TO565(gxt->pixels.pix16,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),&gxt->fmt);
}

void AGIDL_GXTConvert565TO555(AGIDL_GXT* gxt){
	AGIDL_565TO555(gxt->pixels.pix16,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),&gxt->fmt);
}

void AGIDL_ColorConvertGXT(AGIDL_GXT* gxt, AGIDL_CLR_FMT dest){
	u8 sbits = AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)), dbits = AGIDL_GetBitCount(dest);
	if(sbits == 16 && dbits == 16){
		AGIDL_ColorConvertImgData(gxt->pixels.pix16,NULL,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),AGIDL_GXTGetClrFmt(gxt),dest);
		AGIDL_GXTSetClrFmt(gxt,dest);
	}
	else if((sbits == 24 || sbits == 32) && (dbits == 24 || dbits == 32)){
		AGIDL_ColorConvertImgData(gxt->pixels.pix32,NULL,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),AGIDL_GXTGetClrFmt(gxt),dest);
		AGIDL_GXTSetClrFmt(gxt,dest);
	}
	else if(sbits == 16 && (dbits == 24 || dbits == 32)){
		gxt->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),dest);
		AGIDL_ColorConvertImgData(gxt->pixels.pix16,gxt->pixels.pix32,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),AGIDL_GXTGetClrFmt(gxt),dest);
		AGIDL_GXTSetClrFmt(gxt,dest);
		free(gxt->pixels.pix16);
	}
	else{
		gxt->pixels.pix16 = (COLOR16*)AGIDL_AllocImgDataMMU(AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),dest);
		AGIDL_ColorConvertImgData(gxt->pixels.pix32,gxt->pixels.pix16,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),AGIDL_GXTGetClrFmt(gxt),dest);
		AGIDL_GXTSetClrFmt(gxt,dest);
		free(gxt->pixels.pix32);
	}
}

void AGIDL_GXTSyncPix(AGIDL_GXT *gxt, COLOR *clrs){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) != 16){
		AGIDL_ClrMemcpy(gxt->pixels.pix32,clrs,AGIDL_GXTGetSize(gxt));
	}
}

void AGIDL_GXTSyncPix16(AGIDL_GXT *gxt, COLOR16 *clrs){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 16){
		AGIDL_ClrMemcpy16(gxt->pixels.pix16,clrs,AGIDL_GXTGetSize(gxt));
	}
}

void AGIDL_GXTCopyPix(AGIDL_GXT* gxt, COLOR* clrs, u32 count){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) != 16){
		AGIDL_ClrMemcpy(gxt->pixels.pix32,clrs,count);
	}
}

void AGIDL_GXTCopyPix16(AGIDL_GXT* gxt, COLOR16* clrs, u32 count){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 16){
		AGIDL_ClrMemcpy16(gxt->pixels.pix16,clrs,count);
	}
}

AGIDL_GXT* AGIDL_CreateGXT(const char* filename, int width, int height, AGIDL_CLR_FMT fmt){
	AGIDL_GXT* gxt = (AGIDL_GXT*)malloc(sizeof(AGIDL_GXT));
	gxt->filename = (char*)malloc(strlen(filename)+1);
	AGIDL_SetGXTFilename(gxt,filename);
	AGIDL_GXTSetWidth(gxt,width);
	AGIDL_GXTSetHeight(gxt,height);
	AGIDL_GXTSetClrFmt(gxt,fmt);
	AGIDL_GXTSetICPMode(gxt,0);
	AGIDL_GXTSetMaxDiff(gxt,7);
	
	if(AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32){
		gxt->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*AGIDL_GXTGetSize(gxt));
	}
	if(AGIDL_GetBitCount(fmt) == 16){
		gxt->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*AGIDL_GXTGetSize(gxt));
	}
	
	return gxt;
}

AGIDL_GXT* AGIDL_GXTCpyImg(AGIDL_GXT* gxt){
	AGIDL_GXT* cpygxt = AGIDL_CreateGXT("cpy.gxt",AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),
	AGIDL_GXTGetClrFmt(gxt));
	AGIDL_GXTSetICPMode(cpygxt,gxt->icp);
	AGIDL_GXTSetMaxDiff(cpygxt,AGIDL_GXTGetMaxDiff(gxt));
	
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 24 ||  AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 32){
		AGIDL_GXTSyncPix(cpygxt,gxt->pixels.pix32);
	}
	
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 16){
		AGIDL_GXTSyncPix16(cpygxt,gxt->pixels.pix16);
	}
	
	return cpygxt;
}

void AGIDL_FreeGXT(AGIDL_GXT* gxt){
	free(gxt->filename);
	
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 16){
		free(gxt->pixels.pix16);
	}
	else{
		free(gxt->pixels.pix32);
	}
	
	free(gxt);
	
	if(gxt != NULL){
		gxt = NULL;
	}
}

GXT_TEXTURE_TYPE AGIDL_GXTGetTextureType(u8 type){
	switch(type){
		case 0x00:{
			return SWIZZLED;
		}break;
		case 0x40:{
			return CUBE;
		}break;
		case 0x60:{
			return LINEAR;
		}break;
		case 0x80:{
			return TILED;
		}break;
		case 0x0C:{
			return LINEAR_STRIDDED;
		}break;
		default: return LINEAR; break;
	}
	return UNSUPPORTED_PVRT;
}

GXT_CLR_FMT AGIDL_GetGXTClrFmt(u8 type){
	switch(type){
		case 0x98:{
			return GXT_RGB_888;
		}break;
		case 0x95:{
			return GXT_ICP_256;
		}break;
		case 0x05:{
			return GXT_RGB_565;
		}break;
		case 0x04:{
			return GXT_RGB_555;
		}break;
		case 0x02:{
			return GXT_RGB_4444;
		}break;
		case 0x10:{
			return GXT_ARGB_8888;
		}break;
		case 0x80:{
			return GXT_PVRT2BPP;
		}break;
		case 0x81:{
			return GXT_PVRT4BPP;
		}break;
		default: return UNSUPPORTED_PVRT; break;
	}
}


char* bsr_dec_2_bin(u32 num){
	char* bin = (char*)malloc(sizeof(char)*33);
	int i;
	for(i = 31; i >= 0; i--){
		int k = num >> i;
		if(k & 1){
			bin[31-i] = '1';
		}
		else{
			bin[31-i] = '0';
		}
	}
	bin[32] = '\0';
	return bin;
}

u32 pow2(u32 num){
	u32 i, pow = 2;
	for(i = 1; i < num; i++){
		pow *= 2;
	}
	return pow;
}

u32 bsr_bin_2_dec(char* binary){
	u32 accumulation = 0;
	
	int i;
	for(i = strlen(binary)-1; i >= 0; i--){
		int bin;
		if(binary[i] == '0'){
			bin = 0;
		}
		if(binary[i] == '1'){
			bin = 1;
		}
		
		u8 mul = (strlen(binary)-1) - i;
		
		if(mul == 0 && bin == 1){
			accumulation += bin;
		}
		else{
			accumulation += bin * pow2(mul);
		}
	}
	return accumulation;
}

u32 bsr(u32 num){
	char* bin = bsr_dec_2_bin(num);
	char* binrev = (char*)malloc(strlen(bin)+1);
	
	int i, count = 0;
	for(i = strlen(bin)-1; i >= 0; i--){
		binrev[i] = bin[count];
		count++;
	}
	
	bin[32] = '\0';
	
	u32 bsr = bsr_bin_2_dec(binrev);

	if(bsr >= 2147483648){
		bsr = 2147483647;
	}
	
	free(bin);
	free(binrev);
	
	return bsr;
}

u32 nearpow2(u32 n){
	n--;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	n++;
	return n;
}

u32 morton(u32 x, u32 y, u32 width, u32 height){
	u32 bsrw = log(width);
	u32 bsrh = log(height);
	u32 d = AGIDL_Min(bsrw,bsrh);
	
	u32 morton = 0;
	int i;
	for(i = 0; i < d; i++){
		morton |= ((x & (1 << i)) << (i + 1)) | ((y & (1 << i)) << i);
	}
	
	if(width < height){
		morton |= ((y & ~(width - 1) << d));
	}
	else{
		morton |= ((x & ~(height - 1) << d));
	}
	
	return morton;
}

int AGIDL_GXTDecodeHeader(AGIDL_GXT* gxt, FILE* file){
	u8 g,x,t,blank;
	fread(&g,1,1,file);
	fread(&x,1,1,file);
	fread(&t,1,1,file);
	fread(&blank,1,1,file);
	gxt->header.magic = g << 24 | t << 16 | x << 8 | blank;
	fread(&gxt->header.version,4,1,file);
	fread(&gxt->header.num_of_tex,4,1,file);
	fread(&gxt->header.offset,4,1,file);
	fread(&gxt->header.size_of_tex,4,1,file);
	fread(&gxt->header.num_of_icp_16,4,1,file);
	fread(&gxt->header.num_of_icp_256,4,1,file);
	fread(&gxt->header.blank,4,1,file);
	
	if(g != 'G' || x != 'X' || t != 'T'){
		return INVALID_HEADER_FORMATTING_ERROR;
	}
	else return NO_IMG_ERROR;
}

int AGIDL_GXTDecodeTextureHeader(AGIDL_GXT* gxt, FILE* file){
	fread(&gxt->header.header.offset,4,1,file);
	fread(&gxt->header.header.size,4,1,file);
	fread(&gxt->header.header.index,4,1,file);
	fread(&gxt->header.header.tex_flags,4,1,file);
	
	u32 base = 0;
	u8 tex_type = 0;
	
	fread(&base,3,1,file);
	fread(&tex_type,1,1,file);
	
	gxt->header.header.type = AGIDL_GXTGetTextureType(tex_type);
	
	fread(&base,3,1,file);
	fread(&tex_type,1,1,file);
	
	gxt->header.header.fmt = AGIDL_GetGXTClrFmt(tex_type);
	
	fread(&gxt->header.header.width,2,1,file);
	fread(&gxt->header.header.height,2,1,file);
	fread(&gxt->header.header.mipmaps,2,1,file);
	fread(&gxt->header.header.blank,2,1,file);
	
	if(gxt->header.header.type == UNSUPPORTED_PVRT || gxt->header.header.fmt == UNSUPPORTED_PVRT){
		return INVALID_HEADER_FORMATTING_ERROR;
	}
	else return NO_IMG_ERROR;
}

void AGIDL_GXTDecodeIMG(AGIDL_GXT* gxt, FILE* file){
	if(gxt->header.header.type == LINEAR && gxt->header.header.fmt == GXT_RGB_888){
		AGIDL_GXTSetClrFmt(gxt,AGIDL_RGB_888);
		
		gxt->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*AGIDL_GXTGetSize(gxt));
		
		int x,y;
		for(y = 0; y < AGIDL_GXTGetHeight(gxt); y++){
			for(x = 0; x < AGIDL_GXTGetWidth(gxt); x++){
				COLOR clr;
				fread(&clr,3,1,file);
				AGIDL_GXTSetClr(gxt,x,y,clr);
			}
		}
	}
	else if(gxt->header.header.type == LINEAR && gxt->header.header.fmt == GXT_ARGB_8888){
		AGIDL_GXTSetClrFmt(gxt,AGIDL_RGBA_8888);
		
		gxt->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*AGIDL_GXTGetSize(gxt));
		
		int x,y;
		for(y = 0; y < AGIDL_GXTGetHeight(gxt); y++){
			for(x = 0; x < AGIDL_GXTGetWidth(gxt); x++){
				u8 r = 0, g = 0, b = 0, a = 0;
				
				fread(&a,1,1,file);
				fread(&r,1,1,file);
				fread(&g,1,1,file);
				fread(&b,1,1,file);
				
				COLOR clr = AGIDL_RGBA(r,g,b,a,AGIDL_RGBA_8888);
				
				AGIDL_GXTSetClr(gxt,x,y,clr);
			}
		}
	}
	else if(gxt->header.header.type == LINEAR && gxt->header.header.fmt == GXT_ARGB_4444){
		AGIDL_GXTSetClrFmt(gxt,AGIDL_RGBA_8888);
		
		gxt->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*AGIDL_GXTGetSize(gxt));
		
		int x,y;
		for(y = 0; y < AGIDL_GXTGetHeight(gxt); y++){
			for(x = 0; x < AGIDL_GXTGetWidth(gxt); x++){
				u16 byte1 = 0, byte2 = 0;
				
				fread(&byte1,1,1,file);
				fread(&byte2,1,1,file);
				
				u8 a = (byte1 & 0xf0) >> 4;
				u8 r = (byte1 & 0xf0);
				u8 g = (byte2 & 0xf0) >> 4;
				u8 b = (byte2 & 0xf0);
				
				a <<= 4;
				r <<= 4;
				g <<= 4;
				b <<= 4;
				
				COLOR clr = AGIDL_RGBA(r,g,b,a,AGIDL_RGBA_8888);
				
				AGIDL_GXTSetClr(gxt,x,y,clr);
			}
		}
	}
	else if(gxt->header.header.type == LINEAR && (gxt->header.header.fmt == GXT_RGB_555 || gxt->header.header.fmt == GXT_RGB_565)){
		if(gxt->header.header.fmt == GXT_RGB_555){
			AGIDL_GXTSetClrFmt(gxt,AGIDL_RGB_555);
		}
		else{
			AGIDL_GXTSetClrFmt(gxt,AGIDL_RGB_565);
		}
		
		gxt->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*AGIDL_GXTGetSize(gxt));
		fread(gxt->pixels.pix16,2,AGIDL_GXTGetSize(gxt),file);
	}
	else if(gxt->header.header.type == LINEAR && gxt->header.header.fmt == GXT_ICP_256){
		AGIDL_GXTSetClrFmt(gxt,AGIDL_BGR_888);
		
		gxt->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*AGIDL_GXTGetSize(gxt));
		
		u32 offset = (gxt->header.offset + gxt->header.size_of_tex) - 1024;

		fseek(file,offset,SEEK_SET);
		
		int i;
		for(i = 0; i < 256; i++){
			u8 r,g,b,blank;
			fread(&b,1,1,file);
			fread(&g,1,1,file);
			fread(&r,1,1,file);
			fread(&blank,1,1,file);
			gxt->palette.icp.palette_256[i] = AGIDL_RGB(r,g,b,AGIDL_BGR_888);
		}
		
		fseek(file,gxt->header.offset,SEEK_SET);
		
		u8* buf = (u8*)malloc(sizeof(u8)*AGIDL_GXTGetSize(gxt));
		
		fread(buf,1,AGIDL_GXTGetSize(gxt),file);

		int x,y;
		for(y = 0; y < AGIDL_GXTGetHeight(gxt); y++){
			for(x = 0; x < AGIDL_GXTGetWidth(gxt); x++){
				int offset = x + y * AGIDL_GXTGetWidth(gxt);
				AGIDL_GXTSetClr(gxt,x,y,gxt->palette.icp.palette_256[buf[offset]]);
			}

		}
		
		free(buf);
	}	
}

AGIDL_GXT* AGIDL_LoadGXT(char *filename){
	FILE* file = fopen(filename,"rb");
	
	if(file == NULL){
		printf("%s - %s!\n",AGIDL_Error2Str(FILE_NOT_LOCATED_IMG_ERROR),filename);
		return NULL;
	}
	
	AGIDL_GXT* gxt = (AGIDL_GXT*)malloc(sizeof(AGIDL_GXT));
	gxt->filename = (char*)malloc(strlen(filename)+1);
	AGIDL_FilenameCpy(gxt->filename,filename);
	
	if(gxt == NULL || gxt->filename == NULL){
		printf("%s\n",AGIDL_Error2Str(MEMORY_IMG_ERROR));
		return NULL;
	}
	
	int error1 = AGIDL_GXTDecodeHeader(gxt,file);
	int error2 = AGIDL_GXTDecodeTextureHeader(gxt,file);
	
	if(error1 != NO_IMG_ERROR){
		printf("%s\n",AGIDL_Error2Str(error1));
	}
	
	if(error2 != NO_IMG_ERROR){
		printf("%s\n",AGIDL_Error2Str(error2));
	}
	
	AGIDL_GXTDecodeIMG(gxt,file);
	
	fclose(file);
	
	return gxt;
}

void AGIDL_GXTEncodeHeader(AGIDL_GXT* gxt, FILE* file){
	u8 g = 'G', x = 'X', t = 'T', zero = 0, one = 1;
	fwrite(&g,1,1,file);
	fwrite(&x,1,1,file);
	fwrite(&t,1,1,file);
	fwrite(&zero,1,1,file);
	u8 three = 3, ten = 0x10;
	fwrite(&three,1,1,file);
	fwrite(&zero,1,1,file);
	fwrite(&zero,1,1,file);
	fwrite(&ten,1,1,file);
	fwrite(&one,1,1,file);
	fwrite(&zero,1,1,file);
	fwrite(&zero,1,1,file);
	fwrite(&zero,1,1,file);
	u8 fourty = 0x40;
	fwrite(&fourty,1,1,file);
	fwrite(&zero,1,1,file);
	fwrite(&zero,1,1,file);
	fwrite(&zero,1,1,file);
	u32 size = AGIDL_GXTGetWidth(gxt) * AGIDL_GXTGetHeight(gxt) * (AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) / 8);
	
	if(gxt->icp == 1){
		size = (AGIDL_GXTGetWidth(gxt) * AGIDL_GXTGetHeight(gxt)) + 1024;
	}
	
	gxt->header.size_of_tex = size;
	fwrite(&size,4,1,file);
	
	if(gxt->icp != 1){
		u32 bigzero = 0;
		fwrite(&bigzero,4,1,file);
		fwrite(&bigzero,4,1,file);
		fwrite(&bigzero,4,1,file);
	}
	else{
		u32 bigzero = 0;
		fwrite(&bigzero,4,1,file);
		fwrite(&one,4,1,file);
		fwrite(&zero,1,4,file);
	}
}

void AGIDL_GXTEncodeTextureHeader(AGIDL_GXT* gxt, FILE* file){
	u8 fourty = 0x40, zero = 0, sixty = 0x60, one = 1;
	fwrite(&fourty,1,1,file);
	fwrite(&zero,1,1,file);
	fwrite(&zero,1,1,file);
	fwrite(&zero,1,1,file);
	
	if(gxt->icp != 1){
		fwrite(&gxt->header.size_of_tex,4,1,file);
		
		u8 twof = 0xFF;
		fwrite(&twof,1,1,file);
		fwrite(&twof,1,1,file);
		fwrite(&twof,1,1,file);
		fwrite(&twof,1,1,file);
		
		int i;
		for(i = 0; i < 7; i++){
			fwrite(&zero,1,1,file);
		}
		
		fwrite(&sixty,1,1,file);
		
		switch(AGIDL_GXTGetClrFmt(gxt)){
			case AGIDL_RGB_888:{
				u8 ten = 0x10, ninetyeight = 0x98;
				fwrite(&zero,1,1,file);
				fwrite(&ten,1,1,file);
				fwrite(&zero,1,1,file);
				fwrite(&ninetyeight,1,1,file);
			}break;
			case AGIDL_RGB_565:{
				u8 ten = 0x10, five = 0x5;
				fwrite(&zero,1,1,file);
				fwrite(&ten,1,1,file);
				fwrite(&zero,1,1,file);
				fwrite(&five,1,1,file);
			}break;
			case AGIDL_RGB_555:{
				u8 ten = 0x10, four = 0x4;
				fwrite(&zero,1,1,file);
				fwrite(&ten,1,1,file);
				fwrite(&zero,1,1,file);
				fwrite(&four,1,1,file);
			}break;
			case AGIDL_BGR_888:{
				u8 ten = 0x10, ninetyeight = 0x98;
				fwrite(&zero,1,1,file);
				fwrite(&ten,1,1,file);
				fwrite(&zero,1,1,file);
				fwrite(&ninetyeight,1,1,file);
			}break;
			case AGIDL_BGR_565:{
				u8 ten = 0x10, five = 0x5;
				fwrite(&zero,1,1,file);
				fwrite(&ten,1,1,file);
				fwrite(&zero,1,1,file);
				fwrite(&five,1,1,file);
			}break;
			case AGIDL_BGR_555:{
				u8 ten = 0x10, four = 0x4;
				fwrite(&zero,1,1,file);
				fwrite(&ten,1,1,file);
				fwrite(&zero,1,1,file);
				fwrite(&four,1,1,file);
			}break;
			case AGIDL_ARGB_8888:{
				u8 ten = 0x10, twelve = 0x0C;
				fwrite(&zero,1,1,file);
				fwrite(&ten,1,1,file);
				fwrite(&zero,1,1,file);
				fwrite(&twelve,1,1,file);
			}break;
			case AGIDL_RGBA_8888:{
				u8 ten = 0x10, twelve = 0x0C;
				fwrite(&zero,1,1,file);
				fwrite(&ten,1,1,file);
				fwrite(&zero,1,1,file);
				fwrite(&twelve,1,1,file);
			}break;
		}
	}
	else{
		u32 size = gxt->header.size_of_tex - 1024;
		fwrite(&size,4,1,file);
		fwrite(&zero,11,1,file);
		fwrite(&sixty,1,1,file);
		u8 ten = 0x10, ninefive = 0x95;
		fwrite(&zero,1,1,file);
		fwrite(&ten,1,1,file);
		fwrite(&zero,1,1,file);
		fwrite(&ninefive,1,1,file);
	}
	
	fwrite(&gxt->header.header.width,2,1,file);
	fwrite(&gxt->header.header.height,2,1,file);
	fwrite(&one,1,1,file);
	fwrite(&zero,1,1,file);
	fwrite(&zero,1,1,file);
	fwrite(&zero,1,1,file);
}

void AGIDL_GXTEncodeICP(AGIDL_GXT* gxt, FILE* file){
	AGIDL_InitICP(&gxt->palette,AGIDL_ICP_256);
	
	int pass = 0;
	u8 pal_index = 0;
	
	int x,y;
	for(y = 0; y < AGIDL_GXTGetHeight(gxt); y++){
		for(x = 0; x < AGIDL_GXTGetWidth(gxt); x++){
			COLOR clr = AGIDL_GXTGetClr(gxt,x,y);
			if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 32){
				u8 r = AGIDL_GetR(clr,AGIDL_GXTGetClrFmt(gxt));
				u8 g = AGIDL_GetG(clr,AGIDL_GXTGetClrFmt(gxt));
				u8 b = AGIDL_GetB(clr,AGIDL_GXTGetClrFmt(gxt));
				clr = AGIDL_RGB(r,g,b,AGIDL_BGR_888);
				AGIDL_AddColorICP(&gxt->palette,pal_index,clr,AGIDL_BGR_888,AGIDL_GXTGetMaxDiff(gxt),&pass);
			}
			else{
				AGIDL_AddColorICP(&gxt->palette,pal_index,clr,AGIDL_BGR_888,AGIDL_GXTGetMaxDiff(gxt),&pass);
			}
						
			if(pass && pal_index < 256){
				pal_index++;
			}
			
			pass = 0;
		}
	}
	
	u32 offset = (0x40 + gxt->header.size_of_tex) - 1024;
	
	u32 index = ftell(file);
	
	fseek(file,offset,SEEK_SET);
	
	u8 blank = 0;
	
	int i;
	for(i = 0; i < 256; i++){
		COLOR clr = gxt->palette.icp.palette_256[i];
		AGIDL_ExtractAndPrintBGR(file,clr,AGIDL_BGR_888);
		fwrite(&blank,1,1,file);
	}
	
	fseek(file,index,SEEK_SET);
}

void AGIDL_GXTEncodeIMG(AGIDL_GXT* gxt, FILE* file){
	if(gxt->icp != 1){
		switch(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt))){
			case 24:{
				int x,y;
				for(y = 0; y < AGIDL_GXTGetHeight(gxt); y++){
					for(x = 0; x < AGIDL_GXTGetWidth(gxt); x++){
						COLOR clr = AGIDL_GXTGetClr(gxt,x,y);
						AGIDL_ExtractAndPrintRGB(file,clr,AGIDL_RGB_888);
					}
				}
			}break;
			case 16:{
				fwrite(gxt->pixels.pix16,2,AGIDL_GXTGetSize(gxt),file);
			}break;
			case 32:{
				int x,y;
				for(y = 0; y < AGIDL_GXTGetHeight(gxt); y++){
					for(x = 0; x < AGIDL_GXTGetWidth(gxt); x++){
						COLOR clr = AGIDL_GXTGetClr(gxt,x,y);
						u8 r = AGIDL_GetR(clr,AGIDL_GXTGetClrFmt(gxt));
						u8 g = AGIDL_GetG(clr,AGIDL_GXTGetClrFmt(gxt));
						u8 b = AGIDL_GetB(clr,AGIDL_GXTGetClrFmt(gxt));
						u8 a = AGIDL_GetA(clr,AGIDL_GXTGetClrFmt(gxt));
						fwrite(&a,1,1,file);
						fwrite(&r,1,1,file);
						fwrite(&g,1,1,file);
						fwrite(&b,1,1,file);
					}
				}
			}break;
		}
	}
	else{
		int x,y;
		for(y = 0; y < AGIDL_GXTGetHeight(gxt); y++){
			for(x = 0; x < AGIDL_GXTGetWidth(gxt); x++){
				COLOR clr = AGIDL_GXTGetClr(gxt,x,y);
				u8 index = AGIDL_FindClosestColor(gxt->palette,clr,gxt->fmt,AGIDL_GXTGetMaxDiff(gxt));
				fwrite(&index,1,1,file);
			}
		}
	}
}

void AGIDL_ExportGXT(AGIDL_GXT* gxt){
	FILE* file = fopen(gxt->filename,"wb");
	
	if(file == NULL){
		printf("Could not open/create Playstation Vita GXT image - %s!\n",gxt->filename);
	}
	
	AGIDL_GXTEncodeHeader(gxt,file);
	AGIDL_GXTEncodeTextureHeader(gxt,file);
	
	if(gxt->icp != 1){
		switch(AGIDL_GXTGetClrFmt(gxt)){
			case AGIDL_RGB_888:{
				AGIDL_GXTEncodeIMG(gxt,file);
			}break;
			case AGIDL_BGR_888:{
				AGIDL_GXTBGR2RGB(gxt);
				AGIDL_GXTEncodeIMG(gxt,file);
				AGIDL_GXTRGB2BGR(gxt);
			}break;
			case AGIDL_RGB_555:{
				AGIDL_GXTEncodeIMG(gxt,file);
			}break;
			case AGIDL_BGR_555:{
				AGIDL_GXTBGR2RGB(gxt);
				AGIDL_GXTEncodeIMG(gxt,file);
				AGIDL_GXTRGB2BGR(gxt);
			}break;
			
			case AGIDL_RGB_565:{
				AGIDL_GXTEncodeIMG(gxt,file);
			}break;
			case AGIDL_BGR_565:{
				AGIDL_GXTBGR2RGB(gxt);
				AGIDL_GXTEncodeIMG(gxt,file);
				AGIDL_GXTRGB2BGR(gxt);
			}break;
			case AGIDL_RGBA_8888:{
				AGIDL_GXTEncodeIMG(gxt,file);
			}break;
			case AGIDL_ARGB_8888:{
				AGIDL_GXTEncodeIMG(gxt,file);
			}break;
		}
	}
	else{
		switch(AGIDL_GXTGetClrFmt(gxt)){
			case AGIDL_RGB_888:{
				AGIDL_GXTRGB2BGR(gxt);
				AGIDL_GXTEncodeICP(gxt,file);
				AGIDL_GXTEncodeIMG(gxt,file);
				AGIDL_GXTBGR2RGB(gxt);
			}break;
			case AGIDL_BGR_888:{
				AGIDL_GXTEncodeICP(gxt,file);
				AGIDL_GXTEncodeIMG(gxt,file);
			}break;
			case AGIDL_RGB_555:{
				AGIDL_GXTConvert16BPPTO24BPP(gxt);
				AGIDL_GXTRGB2BGR(gxt);
				AGIDL_GXTEncodeICP(gxt,file);
				AGIDL_GXTEncodeIMG(gxt,file);
				AGIDL_GXTBGR2RGB(gxt);
				AGIDL_GXTConvert24BPPTO16BPP(gxt);
			}break;
			case AGIDL_BGR_555:{
				AGIDL_GXTConvert16BPPTO24BPP(gxt);
				AGIDL_GXTEncodeICP(gxt,file);
				AGIDL_GXTEncodeIMG(gxt,file);
				AGIDL_GXTConvert24BPPTO16BPP(gxt);
			}break;
			
			case AGIDL_RGB_565:{
				AGIDL_GXTConvert16BPPTO24BPP(gxt);
				AGIDL_GXTRGB2BGR(gxt);
				AGIDL_GXTEncodeICP(gxt,file);
				AGIDL_GXTEncodeIMG(gxt,file);
				AGIDL_GXTBGR2RGB(gxt);
				AGIDL_GXTConvert24BPPTO16BPP(gxt);
			}break;
			case AGIDL_BGR_565:{
				AGIDL_GXTConvert16BPPTO24BPP(gxt);
				AGIDL_GXTEncodeICP(gxt,file);
				AGIDL_GXTEncodeIMG(gxt,file);
				AGIDL_GXTConvert24BPPTO16BPP(gxt);
			}break;
			case AGIDL_RGBA_8888:{
				AGIDL_GXTEncodeICP(gxt,file);
				AGIDL_GXTEncodeIMG(gxt,file);
			}break;
			case AGIDL_ARGB_8888:{
				AGIDL_GXTEncodeICP(gxt,file);
				AGIDL_GXTEncodeIMG(gxt,file);
			}break;
			
		}
	}
	
	fclose(file);
}
