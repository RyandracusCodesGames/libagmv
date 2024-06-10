/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_tga.c
*   Date: 9/13/2023
*   Version: 0.1b
*   Updated: 2/21/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_img_tga.h>

#include <stdlib.h>
#include <string.h>

#include <agidl_cc_core.h>
#include <agidl_file_utils.h>
#include <agidl_img_compression.h>
#include <agidl_img_error.h>
#include <agidl_mmu_utils.h>

void AGIDL_SetTGAFilename(AGIDL_TGA *tga, const char *filename){
	tga->filename = (char*)realloc(tga->filename,strlen(filename));
	AGIDL_FilenameCpy(tga->filename,filename);
}

void AGIDL_TGASetWidth(AGIDL_TGA *tga, const int width){
	tga->header.width = width;
}

void AGIDL_TGASetHeight(AGIDL_TGA *tga, const int height){
	tga->header.height = height;
}

void AGIDL_TGASetClrFmt(AGIDL_TGA *tga, const AGIDL_CLR_FMT fmt){
	tga->fmt = fmt;
}

void AGIDL_TGASetClr(const AGIDL_TGA *tga, const int x, const int y, const COLOR clr){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) != 16){
		AGIDL_SetClr(tga->pixels.pix32,clr,x,y,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga));
	}
	else{
		AGIDL_SetClr16(tga->pixels.pix16,AGIDL_CLR_TO_CLR16(clr,AGIDL_BGR_888,AGIDL_TGAGetClrFmt(tga)),x,y,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga));
	}
}

void AGIDL_TGASetClr16(const AGIDL_TGA *tga, const int x, const int y, const COLOR16 clr){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga) == 16)){
		AGIDL_SetClr16(tga->pixels.pix16,clr,x,y,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga));
	}
	else{
		AGIDL_SetClr(tga->pixels.pix32,AGIDL_CLR16_TO_CLR(clr,AGIDL_RGB_555,AGIDL_TGAGetClrFmt(tga)),x,y,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga));
	}
}

void AGIDL_TGASetRGB(const AGIDL_TGA *tga, const int x, const int y, const u8 r, const u8 g, const u8 b){
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

void AGIDL_TGASetICPMode(AGIDL_TGA *tga, const int mode){
	tga->icp = mode;
}

void AGIDL_TGASetICPEncoding(AGIDL_TGA* tga, const AGIDL_ICP_ENCODE encode){
	tga->encode = encode;
}

void AGIDL_TGASetRGBA(const AGIDL_TGA *tga, const int x, const int y, const u8 r, const u8 g, const u8 b, const u8 a){
	switch(tga->fmt){
		case AGIDL_RGBA_8888:{
			AGIDL_TGASetClr(tga,x,y,AGIDL_RGBA(r,g,b,a,AGIDL_RGBA_8888));
		}break;
		case AGIDL_ARGB_8888:{
			AGIDL_TGASetClr(tga,x,y,AGIDL_RGBA(r,g,b,a,AGIDL_ARGB_8888));
		}break;
	}
}

void AGIDL_TGASetMaxDiff(AGIDL_TGA *tga, const int max_diff){
	tga->max_diff = max_diff;
}

void AGIDL_TGASetCompression(AGIDL_TGA* tga, const int compress){
	tga->compression = compress;
}

void AGIDL_ClearTGA(const AGIDL_TGA *tga, const COLOR clr){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) != 16){
		AGIDL_ClrMemset(tga->pixels.pix32,clr,AGIDL_TGAGetSize(tga));
	}
	else{
		AGIDL_ClrMemset16(tga->pixels.pix16,clr,AGIDL_TGAGetSize(tga));
	}
}

void AGIDL_ClearTGA16(const AGIDL_TGA *tga, const COLOR16 clr){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		AGIDL_ClrMemset16(tga->pixels.pix16,clr,AGIDL_TGAGetSize(tga));
	}
	else{
		AGIDL_ClrMemset(tga->pixels.pix32,AGIDL_CLR16_TO_CLR(clr,AGIDL_RGB_555,AGIDL_TGAGetClrFmt(tga)),AGIDL_TGAGetSize(tga));
	}
}

void AGIDL_ClearColorTGA(const AGIDL_TGA* tga, const float r, const float g, const float b){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		AGIDL_ClearColorBuffer(tga->pixels.pix16,r,g,b,AGIDL_TGAGetClrFmt(tga),AGIDL_TGAGetSize(tga));
	}
	else{
		AGIDL_ClearColorBuffer(tga->pixels.pix32,r,g,b,AGIDL_TGAGetClrFmt(tga),AGIDL_TGAGetSize(tga));
	}
}

void AGIDL_FlushTGA(const AGIDL_TGA* tga){
	AGIDL_ClearTGA(tga,0);
}

int AGIDL_TGAGetWidth(const AGIDL_TGA *tga){
	return tga->header.width;
}

int AGIDL_TGAGetHeight(const AGIDL_TGA *tga){
	return tga->header.height;
}

u32 AGIDL_TGAGetSize(const AGIDL_TGA* tga){
	return AGIDL_TGAGetWidth(tga) * AGIDL_TGAGetHeight(tga);
}

AGIDL_CLR_FMT AGIDL_TGAGetClrFmt(const AGIDL_TGA* tga){
	return tga->fmt;
}

COLOR AGIDL_TGAGetClr(const AGIDL_TGA *tga, const int x, const int y){
	if(x >= 0 && y >= 0 && x < AGIDL_TGAGetWidth(tga) && y < AGIDL_TGAGetHeight(tga)){
		return tga->pixels.pix32[x+y*AGIDL_TGAGetWidth(tga)];
	}
	fprintf(stderr, "%s: Index out of range", __FUNCTION__);
	abort();
}

int AGIDL_TGAGetMaxDiff(const AGIDL_TGA* tga){
	return tga->max_diff;
}

COLOR16 AGIDL_TGAGetClr16(const AGIDL_TGA *tga, const int x, const int y){
	if(x >= 0 && y >= 0 && x < AGIDL_TGAGetWidth(tga) && y < AGIDL_TGAGetHeight(tga)){
		return tga->pixels.pix16[x+y*AGIDL_TGAGetWidth(tga)];
	}
	fprintf(stderr, "%s: Index out of range", __FUNCTION__);
	abort();
}

void AGIDL_TGASyncPix(const AGIDL_TGA *tga, const COLOR *clrs){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) != 16){
		AGIDL_ClrMemcpy(tga->pixels.pix32,clrs,AGIDL_TGAGetSize(tga));
	}
}

void AGIDL_TGASyncPix16(const AGIDL_TGA *tga, const COLOR16 *clrs){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		AGIDL_ClrMemcpy16(tga->pixels.pix16,clrs,AGIDL_TGAGetSize(tga));
	}
}

void AGIDL_TGACopyPix(const AGIDL_TGA* tga, const COLOR* clrs, const u32 count){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) != 16){
		AGIDL_ClrMemcpy(tga->pixels.pix32,clrs,count);
	}
}

void AGIDL_TGACopyPix16(const AGIDL_TGA* tga, const COLOR16* clrs, const u32 count){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		AGIDL_ClrMemcpy16(tga->pixels.pix16,clrs,count);
	}
}

void AGIDL_FreeTGA(AGIDL_TGA *tga){
	free(tga->filename);

	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		free(tga->pixels.pix16);
	}
	else{
		free(tga->pixels.pix32);
	}

	free(tga);
}

AGIDL_TGA * AGIDL_CreateTGA(const char* filename, const int width, const int height, const AGIDL_CLR_FMT fmt){
	AGIDL_TGA *tga = malloc(sizeof(AGIDL_TGA));
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

AGIDL_TGA* AGIDL_TGACpyImg(const AGIDL_TGA* tga){
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

void AGIDL_ColorConvertTGA(AGIDL_TGA* tga, const AGIDL_CLR_FMT dest){
	const u8 sbits = AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)), dbits = AGIDL_GetBitCount(dest);
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

TGA_ICP_TYPE AGIDL_TGAGetICPType(const int num){
	switch(num){
		case 1:{
			return TGA_IMG_TYPE_ICP;
		}
		case 2:{
			return TGA_IMG_TYPE_NO_ICP;
		}
		case 9:{
			return TGA_IMG_TYPE_RLE_ICP;
		}
		case 10:{
			return TGA_IMG_TYPE_RLE_NO_ICP;
		}
		default:{
			fprintf(stderr, "%s: Unsupported num code", __FUNCTION__);
			abort();
		}
	}
}

TGA_IMG_TYPE AGIDL_TGAGetIMGType(const int bits){
	switch(bits){
		case 8:{
			return TGA_IMG_TYPE_ICP_CLR;
		}
		case 15:{
			return TGA_IMG_TYPE_HIGH_CLR;
		}
		case 16:{
			return TGA_IMG_TYPE_HIGH_CLR;
		}
		case 24:{
			return TGA_IMG_TYPE_TRUE_CLR;
		}
		case 32:{
			return TGA_IMG_TYPE_DEEP_CLR;
		}
		default:{
			fprintf(stderr, "%s: Unsupported bit size", __FUNCTION__);
			abort();
		}
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
	return INVALID_HEADER_FORMATTING_ERROR;
}

int AGIDL_IsTGA(const AGIDL_TGA* tga){
	if((tga->header.clrmaptype == 0 || tga->header.clrmaptype == 1) &&
	(tga->header.imgtype == 1 || tga->header.imgtype == 2 || tga->header.imgtype == 3 || tga->header.imgtype == 9 ||
	tga->header.imgtype == 10) && (tga->header.bits == 8 || tga->header.bits == 15 || tga->header.bits == 16 ||
	tga->header.bits == 24 || tga->header.bits == 32) && tga->header.width > 0 &&
	tga->header.width <= 512 && tga->header.height > 0 && tga->header.height <= 482
	&& (tga->header.flip == 0 || tga->header.flip == 1)){
		return 1;
	}
	return 0;
}

void AGIDL_TGADecodeIMG(AGIDL_TGA *tga, FILE* file, const TGA_ICP_TYPE icp, const TGA_IMG_TYPE img_type){
	if(icp == TGA_IMG_TYPE_NO_ICP){
		if(img_type == TGA_IMG_TYPE_HIGH_CLR){
			AGIDL_TGASetClrFmt(tga,AGIDL_RGB_555);
			tga->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_TGAGetWidth(tga)*AGIDL_TGAGetHeight(tga)));
			AGIDL_ReadBufRGB16(file,tga->pixels.pix16,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga));
		}
		else if(img_type == TGA_IMG_TYPE_DEEP_CLR){
			AGIDL_TGASetClrFmt(tga,AGIDL_RGBA_8888);
			tga->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_TGAGetWidth(tga)*AGIDL_TGAGetHeight(tga)));
			AGIDL_ReadBufBGRA(file,tga->pixels.pix32,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga));
		}
		else{
			AGIDL_TGASetClrFmt(tga,AGIDL_BGR_888);
			tga->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_TGAGetWidth(tga)*AGIDL_TGAGetHeight(tga)));
			AGIDL_ReadBufBGR(file,tga->pixels.pix32,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga));
		}
	}
	else if(icp != TGA_IMG_TYPE_RLE_ICP && icp != TGA_IMG_TYPE_RLE_NO_ICP){
		if(img_type == TGA_IMG_TYPE_ICP_CLR){
			AGIDL_TGASetClrFmt(tga,AGIDL_RGB_555);

			tga->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(tga->header.width*tga->header.height));

			int i;
			for(i = 0; i < tga->header.clrmaplength; i++){
				tga->palette.icp.palette_16b_256[i] = AGIDL_ReadShort(file);
			}

			for(i = 0; i < AGIDL_TGAGetSize(tga); i++){
				const u8 index = AGIDL_ReadByte(file);
				tga->pixels.pix16[i] = tga->palette.icp.palette_16b_256[index];
			}
		}
		else if(img_type == TGA_IMG_TYPE_HIGH_CLR){
			AGIDL_TGASetClrFmt(tga,AGIDL_RGB_555);

			tga->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(tga->header.width*tga->header.height));

			int i;
			for(i = 0; i < tga->header.clrmaplength; i++){
				tga->palette.icp.palette_16b_256[i] = AGIDL_ReadShort(file);
			}

			for(i = 0; i < AGIDL_TGAGetSize(tga); i++){
				const u8 index = AGIDL_ReadByte(file);
				tga->pixels.pix16[i] = tga->palette.icp.palette_16b_256[index];
			}
		}
		else if(img_type == TGA_IMG_TYPE_TRUE_CLR){
			AGIDL_TGASetClrFmt(tga,AGIDL_BGR_888);

			tga->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_TGAGetWidth(tga)*AGIDL_TGAGetHeight(tga)));

			int i;
			for(i = 0; i < tga->header.clrmaplength; i++){
				tga->palette.icp.palette_256[i] = AGIDL_ReadRGB(file,AGIDL_BGR_888);
			}

			for(i = 0; i < AGIDL_TGAGetSize(tga); i++){
				const u8 index = AGIDL_ReadByte(file);
				tga->pixels.pix32[i] = tga->palette.icp.palette_256[index];
			}
		}
		else{
			AGIDL_TGASetClrFmt(tga,AGIDL_RGBA_8888);

			tga->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_TGAGetWidth(tga)*AGIDL_TGAGetHeight(tga)));

			int i;
			for(i = 0; i < tga->header.clrmaplength; i++){
				tga->palette.icp.palette_256[i] = AGIDL_ReadRGB(file,AGIDL_BGR_888);
			}

			for(i = 0; i < AGIDL_TGAGetSize(tga); i++){
				const u8 index = AGIDL_ReadByte(file);
				tga->pixels.pix32[i] = tga->palette.icp.palette_256[index];
			}
		}
	}
}

u16 binmul2[9] = {1,2,4,8,16,32,64,128,256};

u16 bin2dec2(const char* binary){
	int accumulation = 0, count = 0;
	for(int i = strlen(binary) - 1; i >= 0; i--){
		int bin;
		if(binary[i] == '0'){
			bin = 0;
		}
		else if(binary[i] == '1'){
			bin = 1;
		}
		else {
			fprintf(stderr, "%s: Unexpected binary digit", __FUNCTION__);
			abort();
		}
		accumulation += bin * binmul2[count];
		count++;
	}
	return accumulation;
}

char* dec2bin2(const u16 number){
	char *bin = malloc(sizeof(char)*9);
	for(int i = 7; i >= 0; i--){
		const int k = number >> i;
		if(k & 1){
			bin[7-i] = '1';
		}
		else{
			bin[7-i] = '0';
		}
	}
	bin[8] = '\0';
	return bin;
}

char* tgarlebits(const char *binary){
	char *bin = malloc(sizeof(char)*9);
	for(int i = 1; i <= 7; i++){
		bin[i-1] = binary[i];
	}
	bin[7] = '\0';
	return bin;
}

void AGIDL_TGADecodeRLE(AGIDL_TGA* tga, FILE* file, TGA_ICP_TYPE icp, TGA_IMG_TYPE img_type){
	if(icp == TGA_IMG_TYPE_RLE_NO_ICP){
		switch(img_type){
			case TGA_IMG_TYPE_HIGH_CLR:{
				AGIDL_TGASetClrFmt(tga,AGIDL_BGR_555);
				tga->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_TGAGetWidth(tga)*AGIDL_TGAGetHeight(tga)));
				int x,y;
				for(y = 0; y < AGIDL_TGAGetHeight(tga); y++){
					for(x = 0; x < AGIDL_TGAGetWidth(tga); x++){
						u8 byte = AGIDL_ReadByte(file);

						char* binbyte = dec2bin2(byte);

						if(binbyte[0] == '1' && bin2dec2(tgarlebits(binbyte)) > 0){
							char* packrle = tgarlebits(binbyte);
							u16 rle = bin2dec2(packrle) + 1;
							COLOR16 col = AGIDL_ReadShort(file);
							u8 r = AGIDL_GetR(col,tga->fmt);
							u8 g = AGIDL_GetG(col,tga->fmt);
							u8 b = AGIDL_GetB(col,tga->fmt);
							col = AGIDL_RGB16(b,g,r,tga->fmt);
							int i;
							for(i = 0; i < rle; i++){
								AGIDL_TGASetClr16(tga,x+i,y,col);
							}
							x += rle - 1;
							free(packrle);
						}
						else{
							COLOR16 col = AGIDL_ReadShort(file);
							u8 r = AGIDL_GetR(col,tga->fmt);
							u8 g = AGIDL_GetG(col,tga->fmt);
							u8 b = AGIDL_GetB(col,tga->fmt);
							col = AGIDL_RGB16(b,g,r,tga->fmt);
							AGIDL_TGASetClr16(tga,x,y,col);
						}

						free(binbyte);
					}
				}
			}break;
			case TGA_IMG_TYPE_TRUE_CLR:{
				AGIDL_TGASetClrFmt(tga,AGIDL_BGR_888);
				tga->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_TGAGetWidth(tga)*AGIDL_TGAGetHeight(tga)));

				int x,y;
				for(y = 0; y < AGIDL_TGAGetHeight(tga); y++){
					for(x = 0; x < AGIDL_TGAGetWidth(tga); x++){
						u8 byte = AGIDL_ReadByte(file);

						char* binbyte = dec2bin2(byte);

						if(binbyte[0] == '1'  && bin2dec2(tgarlebits(binbyte)) > 0){
							char* packrle = tgarlebits(binbyte);
							u16 rle = bin2dec2(packrle) + 1;
							COLOR clr = AGIDL_ReadRGB(file,AGIDL_BGR_888);
							int i;
							for(i = 0; i < rle; i++){
								AGIDL_TGASetClr(tga,x+i,y,clr);
							}
							x += rle - 1;
							free(packrle);
						}
						else{
							COLOR clr = AGIDL_ReadRGB(file,AGIDL_BGR_888);
							AGIDL_TGASetClr(tga,x,y,clr);
						}

						free(binbyte);
					}
				}
			}break;
			case TGA_IMG_TYPE_DEEP_CLR:{
				AGIDL_TGASetClrFmt(tga,AGIDL_RGBA_8888);
				tga->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_TGAGetWidth(tga)*AGIDL_TGAGetHeight(tga)));
				int x,y;
				for(y = 0; y < AGIDL_TGAGetHeight(tga); y++){
					for(x = 0; x < AGIDL_TGAGetWidth(tga); x++){
						u8 byte = AGIDL_ReadByte(file);

						char* binbyte = dec2bin2(byte);

						if(binbyte[0] == '1' && bin2dec2(tgarlebits(binbyte)) > 0){
							char* packrle = tgarlebits(binbyte);
							u16 rle = bin2dec2(packrle) + 1;
							COLOR col;
							u8 r = AGIDL_ReadByte(file);
							u8 g = AGIDL_ReadByte(file);
							u8 b = AGIDL_ReadByte(file);
							u8 a = AGIDL_ReadByte(file);
							col = AGIDL_RGBA(b,g,r,a,AGIDL_RGBA_8888);
							int i;
							for(i = 0; i < rle; i++){
								AGIDL_TGASetClr(tga,x+i,y,col);
							}
							x += rle - 1;
							free(packrle);
						}
						else{
							COLOR col;
							u8 r = AGIDL_ReadByte(file);
							u8 g = AGIDL_ReadByte(file);
							u8 b = AGIDL_ReadByte(file);
							u8 a = AGIDL_ReadByte(file);
							col = AGIDL_RGBA(b,g,r,a,AGIDL_RGBA_8888);
							AGIDL_TGASetClr(tga,x,y,col);
						}

						free(binbyte);
					}
				}
			}break;
		}
	}
	else if(icp == TGA_IMG_TYPE_RLE_ICP){
		switch(img_type){
			case TGA_IMG_TYPE_TRUE_CLR:{
				AGIDL_TGASetClrFmt(tga,AGIDL_BGR_888);

				tga->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_TGAGetWidth(tga)*AGIDL_TGAGetHeight(tga)));

				int i;
				for(i = 0; i < tga->header.clrmaplength; i++){
					tga->palette.icp.palette_256[i] = AGIDL_ReadRGB(file,AGIDL_BGR_888);
				}

				int x,y;
				for(y = 0; y < AGIDL_TGAGetHeight(tga); y++){
					for(x = 0; x < AGIDL_TGAGetWidth(tga); x++){
						u8 index = AGIDL_ReadByte(file);
						AGIDL_TGASetClr(tga,x,y,tga->palette.icp.palette_256[index]);
					}
				}

				for(y = 0; y < AGIDL_TGAGetHeight(tga); y++){
					for(x = 0; x < AGIDL_TGAGetWidth(tga); x++){
						u8 byte = AGIDL_ReadByte(file);

						char* binbyte = dec2bin2(byte);

						if(binbyte[0] == '1' && bin2dec2(tgarlebits(binbyte)) > 0){
							char* packrle = tgarlebits(binbyte);
							u16 rle = bin2dec2(packrle) + 1;
							u8 index = AGIDL_ReadByte(file);

							for(i = 0; i < rle; i++){
								AGIDL_TGASetClr(tga,x+i,y,tga->palette.icp.palette_256[index]);
							}
							x += rle - 1;
							free(packrle);
						}
						else{
							u8 index = AGIDL_ReadByte(file);
							AGIDL_TGASetClr(tga,x,y,tga->palette.icp.palette_256[index]);
						}

						free(binbyte);
					}
				}
			}break;
			case TGA_IMG_TYPE_HIGH_CLR:{
				AGIDL_TGASetClrFmt(tga,AGIDL_BGR_555);

				tga->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_TGAGetWidth(tga)*AGIDL_TGAGetHeight(tga)));

				int i;
				for(i = 0; i < tga->header.clrmaplength; i++){
					COLOR16 clr = AGIDL_ReadShort(file);
					tga->palette.icp.palette_16b_256[i] = clr;
				}

				int x,y;
				for(y = 0; y < AGIDL_TGAGetHeight(tga); y++){
					for(x = 0; x < AGIDL_TGAGetWidth(tga); x++){
						u8 byte = AGIDL_ReadByte(file);

						char* binbyte = dec2bin2(byte);

						if(binbyte[0] == '1' && bin2dec2(tgarlebits(binbyte)) > 0){
							char* packrle = tgarlebits(binbyte);
							u16 rle = bin2dec2(packrle) + 1;
							u8 index = AGIDL_ReadByte(file);

							for(i = 0; i < rle; i++){
								AGIDL_TGASetClr16(tga,x+i,y,tga->palette.icp.palette_16b_256[index]);
							}
							x += rle - 1;
							free(packrle);
						}
						else{
							u8 index = AGIDL_ReadByte(file);
							AGIDL_TGASetClr16(tga,x,y,tga->palette.icp.palette_16b_256[index]);
						}

						free(binbyte);
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

void AGIDL_TGAEncodeIMG(const AGIDL_TGA* tga, FILE* file){
	const AGIDL_CLR_FMT fmt = AGIDL_TGAGetClrFmt(tga);

	if(tga->header.bits != 8){
		if(AGIDL_GetBitCount(fmt) == 24){
			AGIDL_WriteBufBGR(file,tga->pixels.pix32,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga));
		}
		else if(AGIDL_GetBitCount(fmt) == 32){
			AGIDL_WriteBufBGRA(file,tga->pixels.pix32,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),AGIDL_TGAGetClrFmt(tga));
		}
		else{
			AGIDL_WriteBufClr16(file,tga->pixels.pix16,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga));
		}
	}
	else{
		if(tga->encode == ICP_ENCODE_THRESHOLD){
			for(int y = 0; y < AGIDL_TGAGetHeight(tga); y++){
				for(int x = 0; x < AGIDL_TGAGetWidth(tga); x++){
					COLOR clr = AGIDL_TGAGetClr(tga,x,y);

					if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 32){
						const u8 r = AGIDL_GetR(clr,AGIDL_TGAGetClrFmt(tga));
						const u8 g = AGIDL_GetG(clr,AGIDL_TGAGetClrFmt(tga));
						const u8 b = AGIDL_GetB(clr,AGIDL_TGAGetClrFmt(tga));
						clr = AGIDL_RGB(r,g,b,AGIDL_BGR_888);
						const u8 index = AGIDL_FindClosestColor(tga->palette,clr,AGIDL_BGR_888,AGIDL_TGAGetMaxDiff(tga));
						AGIDL_WriteByte(file,index);
					}
					else{
						const u8 index = AGIDL_FindClosestColor(tga->palette,clr,tga->fmt,AGIDL_TGAGetMaxDiff(tga));
						AGIDL_WriteByte(file,index);
					}
				}
			}
		}
		else{
			for(int y = 0; y < AGIDL_TGAGetHeight(tga); y++){
				for(int x = 0; x < AGIDL_TGAGetWidth(tga); x++){
					const COLOR clr = AGIDL_TGAGetClr(tga,x,y);
					const u8 index = AGIDL_FindNearestColor(tga->palette,clr,AGIDL_TGAGetClrFmt(tga));
					AGIDL_WriteByte(file,index);
				}
			}
		}
	}
}

void AGIDL_TGAEncodeICP(AGIDL_TGA* tga, FILE* file){
	if(tga->fmt == AGIDL_RGB_555){
		if(tga->encode == ICP_ENCODE_THRESHOLD){
			int pass = 0;
			u8 pal_index = 0;

			AGIDL_InitICP(&tga->palette, AGIDL_ICP_16b_256);

			for(int y = 0; y < AGIDL_TGAGetHeight(tga); y++){
				for(int x = 0; x < AGIDL_TGAGetWidth(tga); x++){
					const COLOR16 clr = AGIDL_TGAGetClr16(tga,x,y);

					AGIDL_AddColorICP16(&tga->palette,pal_index,clr,AGIDL_TGAGetClrFmt(tga),AGIDL_TGAGetMaxDiff(tga),&pass);

					if(pass == 1 && pal_index < 256){
						pal_index++;
					}

					pass = 0;
				}
			}

			AGIDL_TGAEncodeHeader(tga,file);

			for(int i = 0; i < 256; i++){
				AGIDL_WriteShort(file,tga->palette.icp.palette_16b_256[i]);
			}
		}
		else{
			AGIDL_EncodeHistogramICP(&tga->palette,tga->pixels.pix16,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),AGIDL_TGAGetClrFmt(tga));
			AGIDL_TGAEncodeHeader(tga,file);

			for(int i = 0; i < 256; i++){
				AGIDL_WriteShort(file,tga->palette.icp.palette_16b_256[i]);
			}
		}
	}
	else{
		if(tga->encode == ICP_ENCODE_THRESHOLD){
			int pass = 0;
			u8 pal_index = 0;

			AGIDL_InitICP(&tga->palette, AGIDL_ICP_256);

			for(int y = 0; y < AGIDL_TGAGetHeight(tga); y++){
				for(int x = 0; x < AGIDL_TGAGetWidth(tga); x++){
					COLOR clr = AGIDL_TGAGetClr(tga,x,y);

					if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 32){
						const u8 r = AGIDL_GetR(clr,AGIDL_TGAGetClrFmt(tga));
						const u8 g = AGIDL_GetG(clr,AGIDL_TGAGetClrFmt(tga));
						const u8 b = AGIDL_GetB(clr,AGIDL_TGAGetClrFmt(tga));
						clr = AGIDL_RGB(r,g,b,AGIDL_BGR_888);

						AGIDL_AddColorICP(&tga->palette,pal_index,clr,AGIDL_BGR_888,AGIDL_TGAGetMaxDiff(tga),&pass);
					}
					else{
						AGIDL_AddColorICP(&tga->palette,pal_index,clr,tga->fmt,AGIDL_TGAGetMaxDiff(tga),&pass);
					}

					if(pass == 1 && pal_index < 256){
						pal_index++;
					}

					pass = 0;
				}
			}

			AGIDL_TGAEncodeHeader(tga,file);

			for(int i = 0; i < 256; i++){
				const COLOR clr = tga->palette.icp.palette_256[i];
				AGIDL_WriteRGB(file,clr,AGIDL_BGR_888,AGIDL_BGR_888);
			}
		}
		else{
			AGIDL_EncodeHistogramICP(&tga->palette,tga->pixels.pix32,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),AGIDL_TGAGetClrFmt(tga));
			AGIDL_TGAEncodeHeader(tga,file);
			for(int i = 0; i < 256; i++){
				const COLOR clr = tga->palette.icp.palette_256[i];
				AGIDL_WriteRGB(file,clr,AGIDL_BGR_888,AGIDL_BGR_888);
			}
		}
	}
}

void AGIDL_TGAEncodeNearestICP(const AGIDL_TGA* tga, const AGIDL_ICP palette, FILE* file){
	switch(palette.mode){
		case AGIDL_ICP_256:{
			for(int i = 0; i < 256; i++){
				const COLOR clr = palette.icp.palette_256[i];
				AGIDL_WriteRGB(file,clr,palette.fmt,AGIDL_BGR_888);
			}
		}break;
		case AGIDL_ICP_16b_256:{
			for(int i = 0; i < 256; i++){
				const COLOR16 clr = palette.icp.palette_256[i];
				AGIDL_WriteShort(file,clr);
			}
		}break;
	}

	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) != 16){
		for(int i = 0; i < AGIDL_TGAGetSize(tga); i++){
			const COLOR clr = tga->pixels.pix32[i];
			const u8 index = AGIDL_FindNearestColor(palette,clr,AGIDL_TGAGetClrFmt(tga));
			AGIDL_WriteByte(file,index);
		}
	}
	else{
		for(int i = 0; i < AGIDL_TGAGetSize(tga); i++){
			const COLOR16 clr = tga->pixels.pix16[i];
			const u8 index = AGIDL_FindNearestColor(palette,clr,AGIDL_TGAGetClrFmt(tga));
			AGIDL_WriteByte(file,index);
		}
	}
}

void AGIDL_TGAEncodeRLE(const AGIDL_TGA* tga, FILE* file){
	if(tga->compression == 1){
		switch(tga->fmt){
			case AGIDL_BGR_888:{
				for(int y = 0; y < AGIDL_TGAGetHeight(tga); y++){
					for(int x = 0; x < AGIDL_TGAGetWidth(tga); x++){
						const COLOR clr = AGIDL_TGAGetClr(tga,x,y);

						const u32 count = AGIDL_EncodeRLE(tga->pixels.pix32,24,x,y,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),127);

						x += count - 1;

						if(count > 0){
							char* rle = dec2bin2(count-1);
							rle[0] = '1';
							const u16 rleshort = bin2dec2(rle);
							AGIDL_WriteByte(file,rleshort);
							AGIDL_WriteRGB(file,clr,AGIDL_BGR_888,AGIDL_BGR_888);
							free(rle);
						}
						else{
							AGIDL_WriteByte(file,0);
							AGIDL_WriteRGB(file,clr,AGIDL_BGR_888,AGIDL_BGR_888);
						}
					}
				}
			}break;
			case AGIDL_RGB_555:{
				for(int y = 0; y < AGIDL_TGAGetHeight(tga); y++){
					for(int x = 0; x < AGIDL_TGAGetWidth(tga); x++){
						const COLOR16 clr = AGIDL_TGAGetClr16(tga,x,y);

						const u32 count = AGIDL_EncodeRLE(tga->pixels.pix16,16,x,y,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),127);

						x += count - 1;

						if(count > 0){
							char* rle = dec2bin2(count-1);
							rle[0] = '1';
							const u16 rleshort = bin2dec2(rle);
							AGIDL_WriteByte(file,rleshort);
							AGIDL_WriteShort(file,clr);
							free(rle);
						}
						else{
							AGIDL_WriteByte(file,0);
							AGIDL_WriteShort(file,clr);
						}
					}
				}
			}break;
			case AGIDL_RGBA_8888:{
				for(int y = 0; y < AGIDL_TGAGetHeight(tga); y++){
					for(int x = 0; x < AGIDL_TGAGetWidth(tga); x++){
						COLOR clr = AGIDL_TGAGetClr(tga,x,y);

						const u32 count = AGIDL_EncodeRLE(tga->pixels.pix32,32,x,y,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),AGIDL_TGAGetWidth(tga));

						x += count - 1;

						const u8 r = AGIDL_GetR(clr,AGIDL_RGBA_8888);
						const u8 g = AGIDL_GetG(clr,AGIDL_RGBA_8888);
						const u8 b = AGIDL_GetB(clr,AGIDL_RGBA_8888);

						clr = AGIDL_RGB(r,g,b,AGIDL_RGB_888);

						if(count > 0){
							char* rle = dec2bin2(count-1);
							rle[0] = '1';
							const u16 rleshort = bin2dec2(rle);
							AGIDL_WriteByte(file,rleshort);
							AGIDL_WriteRGB(file,clr,AGIDL_BGR_888,AGIDL_BGR_888);
							free(rle);
						}
						else{
							AGIDL_WriteByte(file,0);
							AGIDL_WriteRGB(file,clr,AGIDL_BGR_888,AGIDL_BGR_888);
						}
					}
				}
			}break;
			case AGIDL_ARGB_8888:{
				for(int y = 0; y < AGIDL_TGAGetHeight(tga); y++){
					for(int x = 0; x < AGIDL_TGAGetWidth(tga); x++){
						COLOR clr = AGIDL_TGAGetClr(tga,x,y);

						const u32 count = AGIDL_EncodeRLE(tga->pixels.pix32,32,x,y,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),AGIDL_TGAGetWidth(tga));

						x += count - 1;

						const u8 r = AGIDL_GetR(clr,AGIDL_ARGB_8888);
						const u8 g = AGIDL_GetG(clr,AGIDL_ARGB_8888);
						const u8 b = AGIDL_GetB(clr,AGIDL_ARGB_8888);

						clr = AGIDL_RGB(r,g,b,AGIDL_RGB_888);

						if(count > 0){
							char* rle = dec2bin2(count-1);
							rle[0] = '1';
							const u16 rleshort = bin2dec2(rle);
							AGIDL_WriteByte(file,rleshort);
							AGIDL_WriteRGB(file,clr,AGIDL_BGR_888,AGIDL_BGR_888);
							free(rle);
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

	AGIDL_TGA *tga = malloc(sizeof(AGIDL_TGA));
	tga->filename = (char*)malloc(strlen(filename)+1);
	AGIDL_FilenameCpy(tga->filename,filename);
	AGIDL_TGASetICPEncoding(tga,ICP_ENCODE_THRESHOLD);

	if(tga->filename == NULL){
		printf("%s\n",AGIDL_Error2Str(MEMORY_IMG_ERROR));
		return NULL;
	}

	const int error = AGIDL_TGADecodeHeader(tga,file);

	if(error != NO_IMG_ERROR){
		printf("%s\n",AGIDL_Error2Str(error));
		return NULL;
	}

	const TGA_ICP_TYPE icp = AGIDL_TGAGetICPType(tga->header.imgtype);
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
