#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <agidl_img_tga.h>
#include <agidl_mmu_utils.h>
#include <agidl_cc_core.h>
#include <agidl_img_compression.h>
#include <agidl_img_error.h>
#include <agidl_file_utils.h>

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_tga.c
*   Date: 9/13/2023
*   Version: 0.4b
*   Updated: 6/10/2024
*   Author: Ryandracus Chapman
*
********************************************/

void AGIDL_SetTGAFilename(AGIDL_TGA *tga, const char *filename){
	if(tga->filename != NULL){
		free(tga->filename);
		tga->filename = (char*)malloc(strlen(filename)+1);
		AGIDL_FilenameCpy(tga->filename,filename);
	}
	else{
		tga->filename = (char*)malloc(strlen(filename)+1);
		AGIDL_FilenameCpy(tga->filename,filename);
	}
}

void AGIDL_TGASetWidth(AGIDL_TGA *tga, u32 width){
	tga->header.width = width;
}

void AGIDL_TGASetHeight(AGIDL_TGA *tga, u32 height){
	tga->header.height = height;
}

void AGIDL_TGASetClrFmt(AGIDL_TGA *tga, AGIDL_CLR_FMT fmt){
	tga->fmt = fmt;
}

void AGIDL_TGASetClr(AGIDL_TGA *tga, int x, int y, COLOR clr){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) != 16){
		AGIDL_SetClr(tga->pixels.pix32,clr,x,y,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga));
	}
	else{
		AGIDL_SetClr16(tga->pixels.pix16,AGIDL_CLR_TO_CLR16(clr,AGIDL_BGR_888,AGIDL_TGAGetClrFmt(tga)),x,y,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga));
	}
}

void AGIDL_TGASetClr16(AGIDL_TGA *tga, int x, int y, COLOR16 clr){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga) == 16)){
		AGIDL_SetClr16(tga->pixels.pix16,clr,x,y,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga));
	}
	else{
		AGIDL_SetClr(tga->pixels.pix32,AGIDL_CLR16_TO_CLR(clr,AGIDL_RGB_555,AGIDL_TGAGetClrFmt(tga)),x,y,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga));
	}
}

void AGIDL_TGASetRGB(AGIDL_TGA *tga, int x, int y, u8 r, u8 g, u8 b){
	switch(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga))){
		case 24:{
			AGIDL_TGASetClr(tga,x,y,AGIDL_RGB(r,g,b,AGIDL_TGAGetClrFmt(tga)));
		}break;
		case 16:{
			AGIDL_TGASetClr16(tga,x,y,AGIDL_RGB16(r,g,b,AGIDL_TGAGetClrFmt(tga)));
		}break;
		case 32:{
			AGIDL_TGASetClr(tga,x,y,AGIDL_RGBA(r,g,b,0xff,AGIDL_TGAGetClrFmt(tga)));
		}break;
	}
}

void AGIDL_TGASetICPMode(AGIDL_TGA *tga, int mode){
	tga->icp = mode;
}

void AGIDL_TGASetICPEncoding(AGIDL_TGA* tga, AGIDL_ICP_ENCODE encode){
	tga->encode = encode;
}

void AGIDL_TGASetRGBA(AGIDL_TGA *tga, int x, int y, u8 r, u8 g, u8 b, u8 a){
	switch(tga->fmt){
		case AGIDL_RGBA_8888:{
			AGIDL_TGASetClr(tga,x,y,AGIDL_RGBA(r,g,b,a,AGIDL_RGBA_8888));
		}break;
		case AGIDL_ARGB_8888:{
			AGIDL_TGASetClr(tga,x,y,AGIDL_RGBA(r,g,b,a,AGIDL_ARGB_8888));
		}break;
	}
}

void AGIDL_TGASetMaxDiff(AGIDL_TGA *tga, int max_diff){
	tga->max_diff = max_diff;
}

void AGIDL_TGASetCompression(AGIDL_TGA* tga, int compress){
	tga->compression = compress;
}

void AGIDL_ClearTGA(AGIDL_TGA *tga, COLOR clr){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) != 16){
		AGIDL_ClrMemset(tga->pixels.pix32,clr,AGIDL_TGAGetSize(tga));
	}
	else{
		AGIDL_ClrMemset16(tga->pixels.pix16,(COLOR16)clr,AGIDL_TGAGetSize(tga));
	}
}

void AGIDL_ClearTGA16(AGIDL_TGA *tga, COLOR16 clr){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		AGIDL_ClrMemset16(tga->pixels.pix16,clr,AGIDL_TGAGetSize(tga));
	}
	else{
		AGIDL_ClrMemset(tga->pixels.pix32,AGIDL_CLR16_TO_CLR(clr,AGIDL_RGB_555,AGIDL_TGAGetClrFmt(tga)),AGIDL_TGAGetSize(tga));
	}
}

void AGIDL_ClearColorTGA(AGIDL_TGA* tga, float r, float g, float b){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		AGIDL_ClearColorBuffer(tga->pixels.pix16,r,g,b,AGIDL_TGAGetClrFmt(tga),AGIDL_TGAGetSize(tga));
	}
	else{
		AGIDL_ClearColorBuffer(tga->pixels.pix32,r,g,b,AGIDL_TGAGetClrFmt(tga),AGIDL_TGAGetSize(tga));
	}
}

void AGIDL_FlushTGA(AGIDL_TGA* tga){
	AGIDL_ClearTGA(tga,0);
}

u32 AGIDL_TGAGetWidth(AGIDL_TGA *tga){
	return tga->header.width;
}

u32 AGIDL_TGAGetHeight(AGIDL_TGA *tga){
	return tga->header.height;
}

u32 AGIDL_TGAGetSize(AGIDL_TGA* tga){
	return AGIDL_TGAGetWidth(tga) * AGIDL_TGAGetHeight(tga);
}

AGIDL_CLR_FMT AGIDL_TGAGetClrFmt(AGIDL_TGA* tga){
	return tga->fmt;
}

COLOR AGIDL_TGAGetClr(AGIDL_TGA *tga, int x, int y){
	if(x >= 0 && y >= 0 && x < AGIDL_TGAGetWidth(tga) && y < AGIDL_TGAGetHeight(tga)){
		return tga->pixels.pix32[x+y*AGIDL_TGAGetWidth(tga)];
	}
	else return 0;
}

int AGIDL_TGAGetMaxDiff(AGIDL_TGA* tga){
	return tga->max_diff;
}

COLOR16 AGIDL_TGAGetClr16(AGIDL_TGA *tga, int x, int y){
	if(x >= 0 && y >= 0 && x < AGIDL_TGAGetWidth(tga) && y < AGIDL_TGAGetHeight(tga)){
		return tga->pixels.pix16[x+y*AGIDL_TGAGetWidth(tga)];
	}
	else return 0;
}

void AGIDL_TGASyncPix(AGIDL_TGA *tga, COLOR *clrs){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) != 16){
		AGIDL_ClrMemcpy(tga->pixels.pix32,clrs,AGIDL_TGAGetSize(tga));
	}
}

void AGIDL_TGASyncPix16(AGIDL_TGA *tga, COLOR16 *clrs){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		AGIDL_ClrMemcpy16(tga->pixels.pix16,clrs,AGIDL_TGAGetSize(tga));
	}
}

void AGIDL_TGACopyPix(AGIDL_TGA* tga, COLOR* clrs, u32 count){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) != 16){
		AGIDL_ClrMemcpy(tga->pixels.pix32,clrs,count);
	}
}

void AGIDL_TGACopyPix16(AGIDL_TGA* tga, COLOR16* clrs, u32 count){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		AGIDL_ClrMemcpy16(tga->pixels.pix16,clrs,count);
	}
}

void AGIDL_FreeTGA(AGIDL_TGA *tga){
	if(tga->filename != NULL){
		free(tga->filename);
		tga->filename = NULL;
	}
	
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		if(tga->pixels.pix16 != NULL){
			free(tga->pixels.pix16);
			tga->pixels.pix16 = NULL;
		}
	}
	else{
		if(tga->pixels.pix32 != NULL){
			free(tga->pixels.pix32);
			tga->pixels.pix32 = NULL;
		}
	}
	
	if(tga != NULL){
		free(tga);
		tga = NULL;
	}
}

AGIDL_TGA * AGIDL_CreateTGA(const char* filename, int width, int height, AGIDL_CLR_FMT fmt){
	AGIDL_TGA *tga = (AGIDL_TGA*)malloc(sizeof(AGIDL_TGA));
	tga->filename = (char*)malloc(strlen(filename)+1);
	AGIDL_FilenameCpy(tga->filename,filename);
	AGIDL_TGASetWidth(tga,width);
	AGIDL_TGASetHeight(tga,height);
	AGIDL_TGASetClrFmt(tga,fmt);
	AGIDL_TGASetMaxDiff(tga,7);
	AGIDL_TGASetICPEncoding(tga,ICP_ENCODE_THRESHOLD);
	
	if(fmt == AGIDL_RGB_888 || fmt == AGIDL_BGR_888 || fmt == AGIDL_RGBA_8888 || fmt == AGIDL_ARGB_8888){
		tga->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(width*height));
	}
	
	if(fmt == AGIDL_RGB_555 || fmt == AGIDL_BGR_555 || fmt == AGIDL_RGB_565 || fmt == AGIDL_BGR_565){
		tga->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(width*height));
	}
	
	return tga;
}

AGIDL_TGA* AGIDL_TGACpyImg(AGIDL_TGA* tga){
	AGIDL_TGA* tgacpy = AGIDL_CreateTGA("tgacpy.tga",AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),AGIDL_TGAGetClrFmt(tga));
	AGIDL_TGASetICPMode(tgacpy,tga->icp);
	AGIDL_TGASetCompression(tgacpy,tga->compression);
	AGIDL_TGASetMaxDiff(tgacpy,AGIDL_TGAGetMaxDiff(tga));
	AGIDL_TGASetICPEncoding(tgacpy,tga->encode);
	
	if(tga->fmt == AGIDL_RGB_888 || tga->fmt == AGIDL_BGR_888 || tga->fmt == AGIDL_RGBA_8888 || tga->fmt == AGIDL_ARGB_8888){
		AGIDL_TGASyncPix(tgacpy,tga->pixels.pix32);
	}
	
	if(tga->fmt == AGIDL_RGB_555 || tga->fmt == AGIDL_BGR_555 || tga->fmt == AGIDL_RGB_565 || tga->fmt == AGIDL_BGR_565){
		AGIDL_TGASyncPix16(tgacpy,tga->pixels.pix16);
	}
	return tgacpy;
}

void AGIDL_TGARGB2BGR(AGIDL_TGA *tga){
	if(tga->fmt == AGIDL_RGB_888){
		AGIDL_RGB2BGR(tga->pixels.pix32,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),&tga->fmt);
	}
	if(tga->fmt == AGIDL_RGB_555 || tga->fmt == AGIDL_RGB_565){
		AGIDL_RGB2BGR16(tga->pixels.pix16,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),&tga->fmt);
	}
}

void AGIDL_TGABGR2RGB(AGIDL_TGA *tga){
	if(tga->fmt == AGIDL_BGR_888){
		AGIDL_BGR2RGB(tga->pixels.pix32,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),&tga->fmt);
	}
	if(tga->fmt == AGIDL_BGR_555 || tga->fmt == AGIDL_BGR_565){
		AGIDL_BGR2RGB16(tga->pixels.pix16,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),&tga->fmt);
	}
}

void AGIDL_TGAConvert16BPPTO24BPP(AGIDL_TGA *tga){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		tga->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_TGAGetHeight(tga)*AGIDL_TGAGetWidth(tga)));
		AGIDL_16BPPTO24BPP(tga->pixels.pix16,tga->pixels.pix32,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),&tga->fmt);
		free(tga->pixels.pix16);
	}
}

void AGIDL_TGAConvert24BPPTO16BPP(AGIDL_TGA *tga){
	if(tga->fmt == AGIDL_RGB_888 || tga->fmt == AGIDL_BGR_888 || tga->fmt == AGIDL_RGBA_8888 || tga->fmt == AGIDL_ARGB_8888){
		tga->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_TGAGetHeight(tga)*AGIDL_TGAGetWidth(tga)));
		AGIDL_24BPPTO16BPP(tga->pixels.pix32,tga->pixels.pix16,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),&tga->fmt);
		free(tga->pixels.pix32);
	}
}

void AGIDL_TGAConvert555TO565(AGIDL_TGA* tga){
	AGIDL_555TO565(tga->pixels.pix16,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),&tga->fmt);
}

void AGIDL_TGAConvert565TO555(AGIDL_TGA* tga){
	AGIDL_565TO555(tga->pixels.pix16,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),&tga->fmt);
}

void AGIDL_ColorConvertTGA(AGIDL_TGA* tga, AGIDL_CLR_FMT dest){
	u8 sbits = AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)), dbits = AGIDL_GetBitCount(dest);
	if(sbits == 16 && dbits == 16){
		AGIDL_ColorConvertImgData(tga->pixels.pix16,NULL,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),AGIDL_TGAGetClrFmt(tga),dest);
		AGIDL_TGASetClrFmt(tga,dest);
	}
	else if((sbits == 24 || sbits == 32) && (dbits == 24 || dbits == 32)){
		AGIDL_ColorConvertImgData(tga->pixels.pix32,NULL,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),AGIDL_TGAGetClrFmt(tga),dest);
		AGIDL_TGASetClrFmt(tga,dest);
	}
	else if(sbits == 16 && (dbits == 24 || dbits == 32)){
		tga->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),dest);
		AGIDL_ColorConvertImgData(tga->pixels.pix16,tga->pixels.pix32,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),AGIDL_TGAGetClrFmt(tga),dest);
		AGIDL_TGASetClrFmt(tga,dest);
		free(tga->pixels.pix16);
	}
	else{
		tga->pixels.pix16 = (COLOR16*)AGIDL_AllocImgDataMMU(AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),dest);
		AGIDL_ColorConvertImgData(tga->pixels.pix32,tga->pixels.pix16,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),AGIDL_TGAGetClrFmt(tga),dest);
		AGIDL_TGASetClrFmt(tga,dest);
		free(tga->pixels.pix32);
	}
}

TGA_ICP_TYPE AGIDL_TGAGetICPType(int num){
	switch(num){
		case 1:{
			return TGA_IMG_TYPE_ICP;
		}break;
		case 2:{
			return TGA_IMG_TYPE_NO_ICP;
		}break;
		case 9:{
			return TGA_IMG_TYPE_RLE_ICP;
		}break;
		case 10:{
			return TGA_IMG_TYPE_RLE_NO_ICP;
		}break;
	}
}

TGA_IMG_TYPE AGIDL_TGAGetIMGType(int bits){
	switch(bits){
		case 8:{
			return TGA_IMG_TYPE_ICP_CLR;
		}break;
		case 15:{
			return TGA_IMG_TYPE_HIGH_CLR;
		}break;
		case 16:{
			return TGA_IMG_TYPE_HIGH_CLR;
		}break;
		case 24:{
			return TGA_IMG_TYPE_TRUE_CLR;
		}break;
		case 32:{
			return TGA_IMG_TYPE_DEEP_CLR;
		}break;
	}
}

int AGIDL_TGADecodeHeader(AGIDL_TGA* tga, FILE* file){
	tga->header.id = AGIDL_ReadByte(file);
	tga->header.clrmaptype = AGIDL_ReadByte(file);
	tga->header.imgtype = AGIDL_ReadByte(file);
	tga->header.clrmapstart = AGIDL_ReadShort(file);
	tga->header.clrmaplength = AGIDL_ReadShort(file);
	tga->header.icpbits = AGIDL_ReadByte(file);
	tga->header.xstart = AGIDL_ReadShort(file);
	tga->header.ystart = AGIDL_ReadShort(file);
	tga->header.width = AGIDL_ReadShort(file);
	tga->header.height = AGIDL_ReadShort(file);
	tga->header.bits = AGIDL_ReadByte(file);
	tga->header.flip = AGIDL_ReadByte(file);
	
	if(tga->header.id != 0){
		fseek(file,tga->header.id,SEEK_CUR);
	}
	
	if(AGIDL_IsTGA(tga) == TRUE){
		return NO_IMG_ERROR;
	}
	else return INVALID_HEADER_FORMATTING_ERROR;
}

int AGIDL_IsTGA(AGIDL_TGA* tga){
	if((tga->header.clrmaptype == 0 || tga->header.clrmaptype == 1) && 
	(tga->header.imgtype == 1 || tga->header.imgtype == 2 || tga->header.imgtype == 3 || tga->header.imgtype == 9 || 
	tga->header.imgtype == 10) && (tga->header.bits == 8 || tga->header.bits == 15 || tga->header.bits == 16 ||
	tga->header.bits == 24 || tga->header.bits == 32) && tga->header.width > 0 &&
	tga->header.width <= 512 && tga->header.height > 0 && tga->header.height <= 482){
		return 1;
	}
	else return 0;
}

void AGIDL_TGADecodeIMG(AGIDL_TGA *tga, FILE* file, TGA_ICP_TYPE icp, TGA_IMG_TYPE img_type){
	u32 width = AGIDL_TGAGetWidth(tga), height = AGIDL_TGAGetHeight(tga), size = AGIDL_TGAGetSize(tga);
	
	if(icp == TGA_IMG_TYPE_NO_ICP){
		if(img_type == TGA_IMG_TYPE_HIGH_CLR){
			AGIDL_TGASetClrFmt(tga,AGIDL_RGB_555);
			tga->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(size));
			AGIDL_ReadBufRGB16(file,tga->pixels.pix16,width,height);
			return;
		}
		else if(img_type == TGA_IMG_TYPE_DEEP_CLR){
			AGIDL_TGASetClrFmt(tga,AGIDL_RGBA_8888);
			tga->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(size));
			AGIDL_ReadBufBGRA(file,tga->pixels.pix32,width,height);
		}
		else{
			AGIDL_TGASetClrFmt(tga,AGIDL_BGR_888);
			tga->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(size));
			AGIDL_ReadBufBGR(file,tga->pixels.pix32,width,height);
		}
	}
	else if(icp != TGA_IMG_TYPE_RLE_ICP && icp != TGA_IMG_TYPE_RLE_NO_ICP){
		if(img_type == TGA_IMG_TYPE_ICP_CLR){
			AGIDL_TGASetClrFmt(tga,AGIDL_RGB_555);
				
			tga->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(size));
			
			int i;
			for(i = 0; i < tga->header.clrmaplength; i++){
				tga->palette.icp.palette_16b_256[i] = AGIDL_ReadShort(file);
			}
			
			for(i = 0; i < size; i++){
				u8 index = AGIDL_ReadByte(file);
				tga->pixels.pix16[i] = tga->palette.icp.palette_16b_256[index];
			}
		}
		else if(img_type == TGA_IMG_TYPE_HIGH_CLR){
			AGIDL_TGASetClrFmt(tga,AGIDL_RGB_555);
				
			tga->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(size));
			
			int i;
			for(i = 0; i < tga->header.clrmaplength; i++){
				tga->palette.icp.palette_16b_256[i] = AGIDL_ReadShort(file);
			}
			
			for(i = 0; i < size; i++){
				u8 index = AGIDL_ReadByte(file);
				tga->pixels.pix16[i] = tga->palette.icp.palette_16b_256[index];
			}
		}
		else if(img_type == TGA_IMG_TYPE_TRUE_CLR){
			AGIDL_TGASetClrFmt(tga,AGIDL_BGR_888);

			tga->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(size));

			int i;
			for(i = 0; i < tga->header.clrmaplength; i++){
				tga->palette.icp.palette_256[i] = AGIDL_ReadRGB(file,AGIDL_BGR_888);
			}
			
			for(i = 0; i < size; i++){
				u8 index = AGIDL_ReadByte(file);
				tga->pixels.pix32[i] = tga->palette.icp.palette_256[index];
			}
		}
		else{
			AGIDL_TGASetClrFmt(tga,AGIDL_RGBA_8888);
				
			tga->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(size));
			
			int i;
			for(i = 0; i < tga->header.clrmaplength; i++){
				tga->palette.icp.palette_256[i] = AGIDL_ReadRGB(file,AGIDL_BGR_888);
			}
			
			for(i = 0; i < size; i++){
				u8 index = AGIDL_ReadByte(file);
				tga->pixels.pix32[i] = tga->palette.icp.palette_256[index];
			}
		}
	}
}

void AGIDL_TGADecodeRLE(AGIDL_TGA* tga, FILE* file, TGA_ICP_TYPE icp, TGA_IMG_TYPE img_type){
	if(icp == TGA_IMG_TYPE_RLE_NO_ICP){
		switch(img_type){
			case TGA_IMG_TYPE_HIGH_CLR:{
				AGIDL_TGASetClrFmt(tga,AGIDL_BGR_555);
				tga->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_TGAGetWidth(tga)*AGIDL_TGAGetHeight(tga)));
				u32 w = AGIDL_TGAGetWidth(tga), h = AGIDL_TGAGetHeight(tga);
				
				int x,y;
				for(y = 0; y < h; y++){
					for(x = 0; x < w; x++){
						u8 byte = AGIDL_ReadByte(file);
						u8 fbit = (byte >> 7) & 1;
						u8 bot = (byte & 0x7f);

						if(fbit == 1 && bot > 0){
							u16 rle = bot;
							COLOR16 col = AGIDL_ReadShort(file);
							u8 r = AGIDL_GetR(col,tga->fmt);
							u8 g = AGIDL_GetG(col,tga->fmt);
							u8 b = AGIDL_GetB(col,tga->fmt);
							col = AGIDL_RGB16(b,g,r,tga->fmt);
							int i;
							for(i = 0; i <= rle; i++){
								AGIDL_TGASetClr16(tga,x+i,y,col);
							}
							x += rle;
						}
						else{
							int i;
							for(i = 0; i <= bot; i++){
								COLOR16 col = AGIDL_ReadShort(file);
								u8 r = AGIDL_GetR(col,tga->fmt);
								u8 g = AGIDL_GetG(col,tga->fmt);
								u8 b = AGIDL_GetB(col,tga->fmt);
								col = AGIDL_RGB16(b,g,r,tga->fmt);
								AGIDL_TGASetClr16(tga,x+i,y,col);
							}
							x += bot;
						}
					}
				}
			}break;
			case TGA_IMG_TYPE_TRUE_CLR:{
				AGIDL_TGASetClrFmt(tga,AGIDL_BGR_888);
				tga->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_TGAGetWidth(tga)*AGIDL_TGAGetHeight(tga)));
				u32 w = AGIDL_TGAGetWidth(tga), h = AGIDL_TGAGetHeight(tga);
				
				int x,y;
				for(y = 0; y < h; y++){
					for(x = 0; x < w; x++){
						u8 byte = AGIDL_ReadByte(file);
						u8 fbit = (byte >> 7) & 1;
						u8 bot = (byte & 0x7f);
						
						if(fbit == 1  && bot > 0){
							u16 rle = bot;
							COLOR clr = AGIDL_ReadRGB(file,AGIDL_BGR_888);
							int i;
							for(i = 0; i <= rle; i++){
								AGIDL_TGASetClr(tga,x+i,y,clr);
							}
							x += rle;
						}
						else{
							int i;
							for(i = 0; i <= bot; i++){
								COLOR clr = AGIDL_ReadRGB(file,AGIDL_BGR_888);
								AGIDL_TGASetClr(tga,x+i,y,clr);
							}
							x += bot;
						}
					}
				}
			}break;
			case TGA_IMG_TYPE_DEEP_CLR:{
				AGIDL_TGASetClrFmt(tga,AGIDL_RGBA_8888);
				tga->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_TGAGetWidth(tga)*AGIDL_TGAGetHeight(tga)));
				u32 w = AGIDL_TGAGetWidth(tga), h = AGIDL_TGAGetHeight(tga);
				
				int x,y;
				for(y = 0; y < h; y++){
					for(x = 0; x < w; x++){
						u8 byte = AGIDL_ReadByte(file);
						u8 fbit = (byte >> 7) & 1;
						u8 bot = (byte & 0x7f);
						
						if(fbit == 1 && bot > 0){
							u16 rle = bot;
							COLOR col;
							u8 r = AGIDL_ReadByte(file);
							u8 g = AGIDL_ReadByte(file);
							u8 b = AGIDL_ReadByte(file);
							u8 a = AGIDL_ReadByte(file);
							col = AGIDL_RGBA(b,g,r,a,AGIDL_RGBA_8888);
							int i;
							for(i = 0; i <= rle; i++){
								AGIDL_TGASetClr(tga,x+i,y,col);
							}
							x += rle;
						}
						else{
							int i;
							for(i = 0; i <= bot; i++){
								COLOR col;
								u8 r = AGIDL_ReadByte(file);
								u8 g = AGIDL_ReadByte(file);
								u8 b = AGIDL_ReadByte(file);
								u8 a = AGIDL_ReadByte(file);
								col = AGIDL_RGBA(b,g,r,a,AGIDL_RGBA_8888);
								AGIDL_TGASetClr(tga,x+i,y,col);
							}
							x += bot;
						}
					}
				}
			}break;
		}
	}
	else if(icp == TGA_IMG_TYPE_RLE_ICP){
		switch(img_type){
			case TGA_IMG_TYPE_TRUE_CLR:{
				u32 w = AGIDL_TGAGetWidth(tga), h = AGIDL_TGAGetHeight(tga);
				
				AGIDL_TGASetClrFmt(tga,AGIDL_BGR_888);
			
				tga->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(w*h));
				
				int i;
				for(i = 0; i < tga->header.clrmaplength; i++){
					tga->palette.icp.palette_256[i] = AGIDL_ReadRGB(file,AGIDL_BGR_888);
				}
				
				int x,y;
				for(y = 0; y < h; y++){
					for(x = 0; x < w; x++){
						u8 byte = AGIDL_ReadByte(file);
						u8 fbit = (byte >> 7) & 1;
						u8 bot = (byte & 0x7f);
						
						if(fbit == 1 && bot > 0){
							u16 rle = bot;
							u8 index = AGIDL_ReadByte(file);
							int i;
							for(i = 0; i <= rle; i++){
								AGIDL_TGASetClr(tga,x+i,y,tga->palette.icp.palette_256[index]);
							}
							x += rle;
						}
						else{
							int i;
							for(i = 0; i <= bot; i++){
								u8 index = AGIDL_ReadByte(file);
								AGIDL_TGASetClr(tga,x+i,y,tga->palette.icp.palette_256[index]);
							}
							x += bot;
						}
					}
				}
			}break;
			case TGA_IMG_TYPE_HIGH_CLR:{
				u32 w = AGIDL_TGAGetWidth(tga), h = AGIDL_TGAGetHeight(tga);
				
				AGIDL_TGASetClrFmt(tga,AGIDL_BGR_555);
			
				tga->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(w*h));
				
				int i;
				for(i = 0; i < tga->header.clrmaplength; i++){
					COLOR16 clr = AGIDL_ReadShort(file);
					tga->palette.icp.palette_16b_256[i] = clr;
				}
				
				int x,y;
				for(y = 0; y < h; y++){
					for(x = 0; x < w; x++){
						u8 byte = AGIDL_ReadByte(file);
						u8 fbit = (byte >> 7) & 1;
						u8 bot = (byte & 0x7f);
						
						if(fbit == 1 && bot > 0){
							u16 rle = bot;
							u8 index = AGIDL_ReadByte(file);
							int i;
							for(i = 0; i <= rle; i++){
								AGIDL_TGASetClr16(tga,x+i,y,tga->palette.icp.palette_16b_256[index]);
							}
							x += rle;
						}
						else{
							int i;
							for(i = 0; i <= bot; i++){
								u8 index = AGIDL_ReadByte(file);
								AGIDL_TGASetClr16(tga,x+1,y,tga->palette.icp.palette_16b_256[index]);
							}
							x += bot;
						}
					}
				}
			}break;
		}
	}
}

void AGIDL_TGAEncodeHeader(AGIDL_TGA* tga, FILE* file){
	if(tga->icp != 1 && tga->compression != 1){
		tga->header.id = 0;
		tga->header.clrmaptype = 0;
		tga->header.imgtype = 2;
		tga->header.clrmapstart = 0;
		tga->header.clrmaplength = 0;
		tga->header.icpbits = 0;
		
		if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 24){
			tga->header.bits = 24;
		}
		else if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
			tga->header.bits = 16;
		}
		else{
			tga->header.bits = 32;
		}
		
		tga->header.xstart = 0;
		tga->header.ystart = 0;
		tga->header.flip = 0;
	}
	else if(tga->icp == 1 && tga->compression != 1){
		if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
			tga->header.id = 0;
			tga->header.clrmaptype = 1;
			tga->header.imgtype = 1;
			tga->header.clrmapstart = 0;
			tga->header.clrmaplength = 256;
			tga->header.icpbits = 16;
			tga->header.xstart = 0;
			tga->header.ystart = 0;
			tga->header.bits = 8;
			tga->header.flip = 0;
		}
		else{
			tga->header.id = 0;
			tga->header.clrmaptype = 1;
			tga->header.imgtype = 1;
			tga->header.clrmapstart = 0;
			tga->header.clrmaplength = 256;
			tga->header.icpbits = 24;
			tga->header.xstart = 0;
			tga->header.ystart = 0;
			tga->header.bits = 8;
			tga->header.flip = 0;
		}
	}
	else{
		tga->header.id = 0;
		tga->header.clrmaptype = 0;
		tga->header.imgtype = 10;
		tga->header.clrmapstart = 0;
		tga->header.clrmaplength = 0;
		tga->header.icpbits = 0;
		
		if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 24 || AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 32){
			tga->header.bits = 24;
		}
		
		if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
			tga->header.bits = 16;
		}
		
		tga->header.xstart = 0;
		tga->header.ystart = 0;
		tga->header.flip = 0;
	}
	
	AGIDL_WriteByte(file,tga->header.id);
	AGIDL_WriteByte(file,tga->header.clrmaptype);
	AGIDL_WriteByte(file,tga->header.imgtype);
	AGIDL_WriteShort(file,tga->header.clrmapstart);
	AGIDL_WriteShort(file,tga->header.clrmaplength);
	AGIDL_WriteByte(file,tga->header.icpbits);
	AGIDL_WriteShort(file,tga->header.xstart);
	AGIDL_WriteShort(file,tga->header.ystart);
	AGIDL_WriteShort(file,tga->header.width);
	AGIDL_WriteShort(file,tga->header.height);
	AGIDL_WriteByte(file,tga->header.bits);
	AGIDL_WriteByte(file,tga->header.flip);
}

void AGIDL_TGAEncodeIMG(AGIDL_TGA* tga, FILE* file){
	AGIDL_CLR_FMT fmt = AGIDL_TGAGetClrFmt(tga);
	u32 width = AGIDL_TGAGetWidth(tga), height = AGIDL_TGAGetHeight(tga), bitcount = AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga));
	
	if(tga->header.bits != 8){
		if(AGIDL_GetBitCount(fmt) == 24){
			AGIDL_WriteBufBGR(file,tga->pixels.pix32,width,height);
		}	
		else if(AGIDL_GetBitCount(fmt) == 32){
			AGIDL_WriteBufBGRA(file,tga->pixels.pix32,width,height,fmt);	
		}
		else{
			AGIDL_WriteBufClr16(file,tga->pixels.pix16,width,height);
		}
	}
	else{
		if(tga->encode == ICP_ENCODE_THRESHOLD){
			int x,y;
			for(y = 0; y < height; y++){
				for(x = 0; x < width; x++){
					COLOR clr = AGIDL_TGAGetClr(tga,x,y);
					
					if(bitcount == 32){
						u8 r = AGIDL_GetR(clr,fmt);
						u8 g = AGIDL_GetG(clr,fmt);
						u8 b = AGIDL_GetB(clr,fmt);
						clr = AGIDL_RGB(r,g,b,AGIDL_BGR_888);
						u8 index = AGIDL_FindClosestColor(tga->palette,clr,AGIDL_BGR_888,AGIDL_TGAGetMaxDiff(tga));
						AGIDL_WriteByte(file,index);
					}
					else{
						u8 index = AGIDL_FindClosestColor(tga->palette,clr,fmt,AGIDL_TGAGetMaxDiff(tga));
						AGIDL_WriteByte(file,index);
					}
				}
			}
		}
		else{
			int x,y;
			for(y = 0; y < height; y++){
				for(x = 0; x < width; x++){
					COLOR clr = AGIDL_TGAGetClr(tga,x,y);
					u8 index = AGIDL_FindNearestColor(tga->palette,clr,fmt);
					AGIDL_WriteByte(file,index);
				}
			}
		}
	}
}

void AGIDL_TGAEncodeICP(AGIDL_TGA* tga, FILE* file){
	AGIDL_CLR_FMT fmt = AGIDL_TGAGetClrFmt(tga);
	u32 width = AGIDL_TGAGetWidth(tga), height = AGIDL_TGAGetHeight(tga), bitcount = AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga));
	u8 max_diff = AGIDL_TGAGetMaxDiff(tga);
	
	if(tga->fmt == AGIDL_RGB_555){
		if(tga->encode == ICP_ENCODE_THRESHOLD){
			int pass = 0;
			u8 pal_index = 0;
			
			AGIDL_InitICP(&tga->palette, AGIDL_ICP_16b_256);
			
			int x,y;
			for(y = 0; y < height; y++){
				for(x = 0; x < width; x++){
					COLOR16 clr = AGIDL_TGAGetClr16(tga,x,y);
					
					AGIDL_AddColorICP16(&tga->palette,pal_index,clr,fmt,max_diff,&pass);
					
					if(pass == 1 && pal_index < 256){
						pal_index++;
					}
					
					pass = 0;
				}
			}
			
			AGIDL_TGAEncodeHeader(tga,file);
			
			int i;
			for(i = 0; i < 256; i++){
				AGIDL_WriteShort(file,tga->palette.icp.palette_16b_256[i]);
			}
		}
		else{
			AGIDL_EncodeHistogramICP(&tga->palette,tga->pixels.pix16,width,height,fmt);
			AGIDL_TGAEncodeHeader(tga,file);
			
			int i;
			for(i = 0; i < 256; i++){
				AGIDL_WriteShort(file,tga->palette.icp.palette_16b_256[i]);
			}
		}
	}
	else{
		if(tga->encode == ICP_ENCODE_THRESHOLD){
			int pass = 0;
			u8 pal_index = 0;
			
			AGIDL_InitICP(&tga->palette, AGIDL_ICP_256);
			
			int x,y;
			for(y = 0; y < width; y++){
				for(x = 0; x < height; x++){
					COLOR clr = AGIDL_TGAGetClr(tga,x,y);
					
					if(bitcount == 32){
						u8 r = AGIDL_GetR(clr,fmt);
						u8 g = AGIDL_GetG(clr,fmt);
						u8 b = AGIDL_GetB(clr,fmt);
						clr = AGIDL_RGB(r,g,b,AGIDL_BGR_888);
						
						AGIDL_AddColorICP(&tga->palette,pal_index,clr,AGIDL_BGR_888,max_diff,&pass);
					}
					else{
						AGIDL_AddColorICP(&tga->palette,pal_index,clr,tga->fmt,max_diff,&pass);
					}
					
					if(pass == 1 && pal_index < 256){
						pal_index++;
					}
					
					pass = 0;
				}
			}
			
			AGIDL_TGAEncodeHeader(tga,file);
			
			int i;
			for(i = 0; i < 256; i++){
				COLOR clr = tga->palette.icp.palette_256[i];
				AGIDL_WriteRGB(file,clr,AGIDL_BGR_888,AGIDL_BGR_888);
			}
		}
		else{
			AGIDL_EncodeHistogramICP(&tga->palette,tga->pixels.pix32,width,height,fmt);
			AGIDL_TGAEncodeHeader(tga,file);
			int i;
			for(i = 0; i < 256; i++){
				COLOR clr = tga->palette.icp.palette_256[i];
				AGIDL_WriteRGB(file,clr,AGIDL_BGR_888,AGIDL_BGR_888);
			}
		}
	}
}

void AGIDL_TGAEncodeNearestICP(AGIDL_TGA* tga, AGIDL_ICP palette, FILE* file){
	u32 size = AGIDL_TGAGetSize(tga), bitcount = AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga));
	AGIDL_CLR_FMT fmt = AGIDL_TGAGetClrFmt(tga);
	
	switch(palette.mode){
		case AGIDL_ICP_256:{
			int i;
			for(i = 0; i < 256; i++){
				COLOR clr = palette.icp.palette_256[i];
				AGIDL_WriteRGB(file,clr,palette.fmt,AGIDL_BGR_888);
			}
		}break;
		case AGIDL_ICP_16b_256:{
			int i;
			for(i = 0; i < 256; i++){
				COLOR16 clr = palette.icp.palette_256[i];
				AGIDL_WriteShort(file,clr);
			}
		}break;
	}
	
	if(bitcount != 16){
		int i;
		for(i = 0; i < size; i++){
			COLOR clr = tga->pixels.pix32[i];
			u8 index = AGIDL_FindNearestColor(palette,clr,fmt);
			AGIDL_WriteByte(file,index);
		}
	}
	else{
		int i;
		for(i = 0; i < size; i++){
			COLOR16 clr = tga->pixels.pix16[i];
			u8 index = AGIDL_FindNearestColor(palette,clr,fmt);
			AGIDL_WriteByte(file,index);
		}
	}
}

void AGIDL_TGAEncodeRLE(AGIDL_TGA* tga, FILE* file){
	if(tga->compression == 1){
		switch(tga->fmt){
			case AGIDL_BGR_888:{
				u32 w = AGIDL_TGAGetWidth(tga), h = AGIDL_TGAGetHeight(tga);
				int x,y;
				for(y = 0; y < h; y++){
					for(x = 0; x < w; x++){
					
						COLOR clr = AGIDL_TGAGetClr(tga,x,y);
						u32 count = AGIDL_EncodeRLE(tga->pixels.pix32,24,x,y,w,h,127);
						
						x += count - 1;
						
						if(count > 0){
							u8 countdec = count - 1;
							u8 rle = 1 << 7 | countdec;
							AGIDL_WriteByte(file,rle);
							AGIDL_WriteRGB(file,clr,AGIDL_BGR_888,AGIDL_BGR_888);
						}
						else{
							AGIDL_WriteByte(file,0);
							AGIDL_WriteRGB(file,clr,AGIDL_BGR_888,AGIDL_BGR_888);
						}
					}
				}
			}break;
			case AGIDL_RGB_555:{
				u32 w = AGIDL_TGAGetWidth(tga), h = AGIDL_TGAGetHeight(tga);
				
				int x,y;
				for(y = 0; y < h; y++){
					for(x = 0; x < w; x++){
						COLOR16 clr = AGIDL_TGAGetClr16(tga,x,y);
						
						u32 count = AGIDL_EncodeRLE(tga->pixels.pix16,16,x,y,w,h,127);
						
						x += count - 1;
						
						if(count > 0){
							u8 countdec = count - 1;
							u8 rle = 1 << 7 | countdec;
							AGIDL_WriteByte(file,rle);
							AGIDL_WriteShort(file,clr);
						}
						else{
							AGIDL_WriteByte(file,0);
							AGIDL_WriteShort(file,clr);
						}
					}
				}
			}break;
			case AGIDL_RGBA_8888:{
				u32 w = AGIDL_TGAGetWidth(tga), h = AGIDL_TGAGetHeight(tga);
				
				int x,y;
				for(y = 0; y < h; y++){
					for(x = 0; x < w; x++){
						COLOR clr = AGIDL_TGAGetClr(tga,x,y);
						
						u32 count = AGIDL_EncodeRLE(tga->pixels.pix32,32,x,y,w,h,127);
						
						x += count - 1;
						
						u8 r = AGIDL_GetR(clr,AGIDL_RGBA_8888);
						u8 g = AGIDL_GetG(clr,AGIDL_RGBA_8888);
						u8 b = AGIDL_GetB(clr,AGIDL_RGBA_8888);
						
						clr = AGIDL_RGB(r,g,b,AGIDL_RGB_888);
						
						if(count > 0){
							u8 countdec = count - 1;
							u8 rle = 1 << 7 | countdec;
							AGIDL_WriteByte(file,rle);
							AGIDL_WriteRGB(file,clr,AGIDL_BGR_888,AGIDL_BGR_888);
						}
						else{
							AGIDL_WriteByte(file,0);
							AGIDL_WriteRGB(file,clr,AGIDL_BGR_888,AGIDL_BGR_888);
						}
					}
				}
			}break;
			case AGIDL_ARGB_8888:{
				int x,y;
				for(y = 0; y < AGIDL_TGAGetHeight(tga); y++){
					for(x = 0; x < AGIDL_TGAGetWidth(tga); x++){
						COLOR clr = AGIDL_TGAGetClr(tga,x,y);
						
						u32 count = AGIDL_EncodeRLE(tga->pixels.pix32,32,x,y,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),AGIDL_TGAGetWidth(tga));
						
						x += count - 1;
						
						u8 r = AGIDL_GetR(clr,AGIDL_ARGB_8888);
						u8 g = AGIDL_GetG(clr,AGIDL_ARGB_8888);
						u8 b = AGIDL_GetB(clr,AGIDL_ARGB_8888);
						
						clr = AGIDL_RGB(r,g,b,AGIDL_RGB_888);
						
						if(count > 0){
							u8 countdec = count - 1;
							u8 rle = 1 << 7 | countdec;
							AGIDL_WriteByte(file,rle);
							AGIDL_WriteRGB(file,clr,AGIDL_BGR_888,AGIDL_BGR_888);
						}
						else{
							AGIDL_WriteByte(file,0);
							AGIDL_WriteRGB(file,clr,AGIDL_BGR_888,AGIDL_BGR_888);
						}
					}
				}
			}break;
		}
	}
}

AGIDL_TGA * AGIDL_LoadTGA(char *filename){
	FILE* file = fopen(filename,"rb");
	
	if(file == NULL){
		printf("%s - %s\n",AGIDL_Error2Str(FILE_NOT_LOCATED_IMG_ERROR),filename);
		return NULL;
	}
	
	AGIDL_TGA *tga = (AGIDL_TGA*)malloc(sizeof(AGIDL_TGA));
	tga->filename = (char*)malloc(strlen(filename)+1);
	AGIDL_FilenameCpy(tga->filename,filename);
	AGIDL_TGASetICPEncoding(tga,ICP_ENCODE_THRESHOLD);
	
	if(tga == NULL || tga->filename == NULL){
		printf("%s\n",AGIDL_Error2Str(MEMORY_IMG_ERROR));
		return NULL;
	}
	
	int error = AGIDL_TGADecodeHeader(tga,file);
	
	if(error != NO_IMG_ERROR){
		printf("%s\n",AGIDL_Error2Str(error));
		return NULL;
	}
	
	TGA_ICP_TYPE icp = AGIDL_TGAGetICPType(tga->header.imgtype);
	TGA_IMG_TYPE img_type = AGIDL_TGAGetIMGType(tga->header.bits);
	
	if(tga->header.bits == 8){
		img_type = AGIDL_TGAGetIMGType(tga->header.icpbits);
	}
	
	AGIDL_TGADecodeIMG(tga,file,icp,img_type);
	AGIDL_TGADecodeRLE(tga,file,icp,img_type);
	
	fclose(file);
	
	return tga;
}

void AGIDL_ExportTGA(AGIDL_TGA *tga){
	FILE *file = fopen(tga->filename,"wb");
	
	if(file == NULL){
		printf("Could not create TGA image file - %s\n",tga->filename);
	}
	
	if(tga->icp != 1 && tga->compression != 1){
		AGIDL_TGAEncodeHeader(tga,file);
		
		switch(tga->fmt){
			case AGIDL_RGB_888:{
				AGIDL_TGARGB2BGR(tga);
				AGIDL_TGAEncodeIMG(tga,file);
				AGIDL_TGABGR2RGB(tga);
			}break;
			case AGIDL_BGR_888:{
				AGIDL_TGAEncodeIMG(tga,file);
			}break;
			case AGIDL_RGB_555:{
				AGIDL_TGAEncodeIMG(tga,file);
			}break;
			case AGIDL_BGR_555:{
				AGIDL_TGABGR2RGB(tga);
				AGIDL_TGAEncodeIMG(tga,file);
				AGIDL_TGABGR2RGB(tga);
			}break;
			case AGIDL_RGB_565:{
				AGIDL_TGAConvert565TO555(tga);
				AGIDL_TGAEncodeIMG(tga,file);
				AGIDL_TGAConvert555TO565(tga);
			}break;
			case AGIDL_BGR_565:{
				AGIDL_TGAConvert565TO555(tga);
				AGIDL_TGABGR2RGB(tga);
				AGIDL_TGAEncodeIMG(tga,file);
				AGIDL_TGABGR2RGB(tga);
				AGIDL_TGAConvert555TO565(tga);
			}break;
			case AGIDL_RGBA_8888:{
				AGIDL_TGAEncodeIMG(tga,file);
			}break;
			case AGIDL_ARGB_8888:{
				AGIDL_TGAEncodeIMG(tga,file);
			}break;
		}
	}
	else if(tga->compression != 1){
		switch(tga->fmt){
			case AGIDL_RGB_888:{
				AGIDL_TGARGB2BGR(tga);
				AGIDL_TGAEncodeICP(tga,file);
				AGIDL_TGAEncodeIMG(tga,file);
				AGIDL_TGABGR2RGB(tga);
			}break;
			case AGIDL_BGR_888:{
				AGIDL_TGAEncodeICP(tga,file);
				AGIDL_TGAEncodeIMG(tga,file);
			}break;
			case AGIDL_RGBA_8888:{
				AGIDL_TGAEncodeICP(tga,file);
				AGIDL_TGAEncodeIMG(tga,file);
			}break;
			case AGIDL_ARGB_8888:{
				AGIDL_TGAEncodeICP(tga,file);
				AGIDL_TGAEncodeIMG(tga,file);
			}break;
			case AGIDL_RGB_555:{
				AGIDL_TGARGB2BGR(tga);
				AGIDL_TGAConvert16BPPTO24BPP(tga);
				AGIDL_TGAEncodeICP(tga,file);
				AGIDL_TGAEncodeIMG(tga,file);
				AGIDL_TGAConvert24BPPTO16BPP(tga);
				AGIDL_TGABGR2RGB(tga);
			}break;
			case AGIDL_BGR_555:{
				AGIDL_TGAConvert16BPPTO24BPP(tga);
				AGIDL_TGAEncodeICP(tga,file);
				AGIDL_TGAEncodeIMG(tga,file);
				AGIDL_TGAConvert24BPPTO16BPP(tga);
			}break;
			case AGIDL_RGB_565:{
				AGIDL_TGAConvert565TO555(tga);
				AGIDL_TGARGB2BGR(tga);
				AGIDL_TGAConvert16BPPTO24BPP(tga);
				AGIDL_TGAEncodeICP(tga,file);
				AGIDL_TGAEncodeIMG(tga,file);
				AGIDL_TGAConvert24BPPTO16BPP(tga);
				AGIDL_TGABGR2RGB(tga);
				AGIDL_TGAConvert555TO565(tga);
			}break;
			case AGIDL_BGR_565:{
				AGIDL_TGAConvert565TO555(tga);
				AGIDL_TGAConvert16BPPTO24BPP(tga);
				AGIDL_TGAEncodeICP(tga,file);
				AGIDL_TGAEncodeIMG(tga,file);
				AGIDL_TGAConvert24BPPTO16BPP(tga);
				AGIDL_TGAConvert555TO565(tga);
			}break;
		}
	}
	else{
		
		AGIDL_TGAEncodeHeader(tga,file);
		
		switch(tga->fmt){
			case AGIDL_RGB_888:{
				AGIDL_TGARGB2BGR(tga);
				AGIDL_TGAEncodeRLE(tga,file);
				AGIDL_TGABGR2RGB(tga);
			}break;
			case AGIDL_BGR_888:{
				AGIDL_TGAEncodeRLE(tga,file);
			}break;
			case AGIDL_RGB_555:{
				AGIDL_TGAEncodeRLE(tga,file);
			}break;
			case AGIDL_BGR_555:{
				AGIDL_TGABGR2RGB(tga);
				AGIDL_TGAEncodeRLE(tga,file);
				AGIDL_TGARGB2BGR(tga);
			}break;
			case AGIDL_RGB_565:{
				AGIDL_TGAConvert565TO555(tga);
				AGIDL_TGAEncodeRLE(tga,file);
				AGIDL_TGAConvert555TO565(tga);
			}break;
			case AGIDL_BGR_565:{
				AGIDL_TGAConvert565TO555(tga);
				AGIDL_TGABGR2RGB(tga);
				AGIDL_TGAEncodeRLE(tga,file);
				AGIDL_TGARGB2BGR(tga);
				AGIDL_TGAConvert555TO565(tga);
			}break;
			case AGIDL_RGBA_8888:{
				AGIDL_TGAEncodeRLE(tga,file);
			}break;
			case AGIDL_ARGB_8888:{
				AGIDL_TGAEncodeRLE(tga,file);
			}break;
		}
	}
	
	fclose(file);
}