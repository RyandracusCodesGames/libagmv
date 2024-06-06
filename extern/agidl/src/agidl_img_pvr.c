#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "agidl_img_pvr.h"
#include "agidl_cc_core.h"
#include "agidl_math_utils.h"
#include "agidl_img_error.h"
#include "agidl_file_utils.h"

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_pvr.c
*   Date: 10/28/2023
*   Version: 0.1b
*   Updated: 3/14/2024
*   Author: Ryandracus Chapman
*
********************************************/

void AGIDL_SetPVRFilename(AGIDL_PVR* pvr, const char* filename){
	pvr->filename = (char*)realloc(pvr->filename,strlen(filename));
	AGIDL_FilenameCpy(pvr->filename,filename);
}

void AGIDL_PVRSetWidth(AGIDL_PVR* pvr, int width){
	pvr->header.width = width;
	pvr->mheader.width = width;
}

void AGIDL_PVRSetHeight(AGIDL_PVR* pvr, int height){
	pvr->header.height = height;
	pvr->mheader.height = height;
}

void AGIDL_PVRSetClrFmt(AGIDL_PVR* pvr, AGIDL_CLR_FMT fmt){
	pvr->fmt = fmt;
}

void AGIDL_PVRSetMaxDiff(AGIDL_PVR* pvr, int max_diff){
	max_diff = AGIDL_Max(0,max_diff);
	pvr->max_diff = max_diff;
}

void AGIDL_PVRSetICPMode(AGIDL_PVR* pvr, int mode){ 
	pvr->icp = mode;
}

void AGIDL_PVRSetType(AGIDL_PVR* pvr, PVR_TYPE pvr_type){
	pvr->pvr_type = pvr_type;
}

void AGIDL_PVRSetCompression(AGIDL_PVR* pvr, AGIDL_Bool compression){
	pvr->compression = compression;
}

void AGIDL_PVRBuildMipmap(AGIDL_PVR* pvr, AGIDL_Bool mipped){
	pvr->mipped = mipped;
}

void AGIDL_PVRSetMipmapLevel(AGIDL_PVR* pvr, u8 mip_lvl){
	pvr->mip_lvl = mip_lvl;
}

void AGIDL_PVRSetClr(AGIDL_PVR *pvr, int x, int y, COLOR clr){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) != 16){
		AGIDL_SetClr(pvr->pixels.pix32,clr,x,y,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr));
	}
	else{
		AGIDL_SetClr16(pvr->pixels.pix16,AGIDL_CLR_TO_CLR16(clr,AGIDL_BGR_888,AGIDL_PVRGetClrFmt(pvr)),x,y,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr));
	}
}

void AGIDL_PVRSetClr16(AGIDL_PVR *pvr, int x, int y, COLOR16 clr){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr) == 16)){
		AGIDL_SetClr16(pvr->pixels.pix16,clr,x,y,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr));
	}
	else{
		AGIDL_SetClr(pvr->pixels.pix32,AGIDL_CLR16_TO_CLR(clr,AGIDL_RGB_555,AGIDL_PVRGetClrFmt(pvr)),x,y,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr));
	}
}

void AGIDL_PVRSetRGB(AGIDL_PVR *pvr, int x, int y, u8 r, u8 g, u8 b){
	switch(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr))){
		case 24:{
			AGIDL_PVRSetClr(pvr,x,y,AGIDL_RGB(r,g,b,AGIDL_PVRGetClrFmt(pvr)));
		}break;
		case 16:{
			AGIDL_PVRSetClr16(pvr,x,y,AGIDL_RGB16(r,g,b,AGIDL_PVRGetClrFmt(pvr)));
		}break;
		case 32:{
			AGIDL_PVRSetClr(pvr,x,y,AGIDL_RGBA(r,g,b,0xff,AGIDL_PVRGetClrFmt(pvr)));
		}break;
	}
}

void AGIDL_ClearPVR(AGIDL_PVR *pvr, COLOR clr){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) != 16){
		AGIDL_ClrMemset(pvr->pixels.pix32,clr,AGIDL_PVRGetSize(pvr));
	}
	else{
		AGIDL_ClrMemset16(pvr->pixels.pix16,(COLOR16)clr,AGIDL_PVRGetSize(pvr));
	}
}

void AGIDL_ClearPVR16(AGIDL_PVR *pvr, COLOR16 clr){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) == 16){
		AGIDL_ClrMemset16(pvr->pixels.pix16,clr,AGIDL_PVRGetSize(pvr));
	}
	else{
		AGIDL_ClrMemset(pvr->pixels.pix32,AGIDL_CLR16_TO_CLR(clr,AGIDL_RGB_555,AGIDL_PVRGetClrFmt(pvr)),AGIDL_PVRGetSize(pvr));
	}
}

void AGIDL_ClearColorPVR(AGIDL_PVR* pvr, float r, float g, float b){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) == 16){
		AGIDL_ClearColorBuffer(pvr->pixels.pix16,r,g,b,AGIDL_PVRGetClrFmt(pvr),AGIDL_PVRGetSize(pvr));
	}
	else{
		AGIDL_ClearColorBuffer(pvr->pixels.pix32,r,g,b,AGIDL_PVRGetClrFmt(pvr),AGIDL_PVRGetSize(pvr));
	}
}

void AGIDL_FlushPVR(AGIDL_PVR* pvr){
	AGIDL_ClearPVR(pvr,0);
}

void AGIDL_PVRSyncPix(AGIDL_PVR *pvr, COLOR *clrs){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) != 16){
		AGIDL_ClrMemcpy(pvr->pixels.pix32,clrs,AGIDL_PVRGetSize(pvr));
	}
}

void AGIDL_PVRSyncPix16(AGIDL_PVR *pvr, COLOR16 *clrs){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) == 16){
		AGIDL_ClrMemcpy16(pvr->pixels.pix16,clrs,AGIDL_PVRGetSize(pvr));
	}
}

void AGIDL_PVRCopyPix(AGIDL_PVR* pvr, COLOR* clrs, u32 count){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) != 16){
		AGIDL_ClrMemcpy(pvr->pixels.pix32,clrs,count);
	}
}

void AGIDL_PVRCopyPix16(AGIDL_PVR* pvr, COLOR16* clrs, u32 count){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) == 16){
		AGIDL_ClrMemcpy16(pvr->pixels.pix16,clrs,count);
	}
}

int AGIDL_PVRGetWidth(AGIDL_PVR* pvr){
	if(pvr->pvr_type == DREAMCAST_PVR){
		return pvr->header.width;
	}
	else return pvr->mheader.width;
}

u32 AGIDL_PVRGetSize(AGIDL_PVR* pvr){
	return AGIDL_PVRGetWidth(pvr) * AGIDL_PVRGetHeight(pvr);
}

int AGIDL_PVRGetHeight(AGIDL_PVR* pvr){
	if(pvr->pvr_type == DREAMCAST_PVR){
		return pvr->header.height;
	}
	else return pvr->mheader.height;
}

AGIDL_CLR_FMT AGIDL_PVRGetClrFmt(AGIDL_PVR* pvr){
	return pvr->fmt;
}

int AGIDL_PVRGetMaxDiff(AGIDL_PVR* pvr){
	return pvr->max_diff;
}

PVR_TYPE AGIDL_PVRGetType(AGIDL_PVR* pvr){
	return pvr->pvr_type;
}

COLOR AGIDL_PVRGetClr(AGIDL_PVR* pvr, int x, int y){
	if(x >= 0 && y >= 0 && x < AGIDL_PVRGetWidth(pvr) && y < AGIDL_PVRGetHeight(pvr)){
		return pvr->pixels.pix32[x+y*AGIDL_PVRGetWidth(pvr)];
	}
}

COLOR16 AGIDL_PVRGetClr16(AGIDL_PVR* pvr, int x, int y){
	if(x >= 0 && y >= 0 && x < AGIDL_PVRGetWidth(pvr) && y < AGIDL_PVRGetHeight(pvr)){
		return pvr->pixels.pix16[x+y*AGIDL_PVRGetWidth(pvr)];
	}
}

void AGIDL_FreePVR(AGIDL_PVR* pvr){
	free(pvr->filename);
	
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) == 16){
		free(pvr->pixels.pix16);
	}
	else{
		free(pvr->pixels.pix32);
	}
	
	if(pvr->mipmap != NULL && (pvr->mip_lvl > 1 || pvr->mheader.num_of_mipmaps > 1) || pvr->header.pvr_img_type == PVR_IMG_SQUARE_TWIDDLED_AND_MIPMAPPED){
		AGIDL_DestroyMipmapMMU(pvr->mipmap);
	}
	else{
		free(pvr);
	}

	if(pvr != NULL){
		pvr = NULL;
	}
}

void AGIDL_PVRRGB2BGR(AGIDL_PVR *pvr){
	if(pvr->fmt == AGIDL_RGB_888){
		AGIDL_RGB2BGR(pvr->pixels.pix32,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),&pvr->fmt);
	}
	if(pvr->fmt == AGIDL_RGB_555 || pvr->fmt == AGIDL_RGB_565){
		AGIDL_RGB2BGR16(pvr->pixels.pix16,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),&pvr->fmt);
	}
}

void AGIDL_PVRBGR2RGB(AGIDL_PVR* pvr){
	if(pvr->fmt == AGIDL_BGR_888){
		AGIDL_BGR2RGB(pvr->pixels.pix32,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),&pvr->fmt);
	}
	if(pvr->fmt == AGIDL_BGR_555 || pvr->fmt == AGIDL_BGR_565){
		AGIDL_BGR2RGB16(pvr->pixels.pix16,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),&pvr->fmt);
	}
}

void AGIDL_PVRConvert16BPPTO24BPP(AGIDL_PVR *pvr){
	if(pvr->fmt == AGIDL_RGB_555 || pvr->fmt == AGIDL_BGR_555 || pvr->fmt == AGIDL_RGB_565 || pvr->fmt == AGIDL_BGR_565){
		pvr->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_PVRGetHeight(pvr)*AGIDL_PVRGetWidth(pvr)));
		AGIDL_16BPPTO24BPP(pvr->pixels.pix16,pvr->pixels.pix32,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),&pvr->fmt);
		free(pvr->pixels.pix16);
	}
}

void AGIDL_PVRConvert24BPPTO16BPP(AGIDL_PVR *pvr){
	if(pvr->fmt == AGIDL_RGB_888 || pvr->fmt == AGIDL_BGR_888 || pvr->fmt == AGIDL_RGBA_8888 || pvr->fmt == AGIDL_ARGB_8888){
		pvr->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_PVRGetHeight(pvr)*AGIDL_PVRGetWidth(pvr)));
		AGIDL_24BPPTO16BPP(pvr->pixels.pix32,pvr->pixels.pix16,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),&pvr->fmt);
		free(pvr->pixels.pix32);
	}
}

void AGIDL_PVRConvert555TO565(AGIDL_PVR* pvr){
	AGIDL_555TO565(pvr->pixels.pix16,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),&pvr->fmt);
}

void AGIDL_PVRConvert565TO555(AGIDL_PVR* pvr){
	AGIDL_565TO555(pvr->pixels.pix16,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),&pvr->fmt);
}

void AGIDL_ColorConvertPVR(AGIDL_PVR* pvr, AGIDL_CLR_FMT dest){
	u8 sbits = AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)), dbits = AGIDL_GetBitCount(dest);
	if(sbits == 16 && dbits == 16){
		AGIDL_ColorConvertImgData(pvr->pixels.pix16,NULL,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr),dest);
		AGIDL_PVRSetClrFmt(pvr,dest);
	}
	else if((sbits == 24 || sbits == 32) && (dbits == 24 || dbits == 32)){
		AGIDL_ColorConvertImgData(pvr->pixels.pix32,NULL,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr),dest);
		AGIDL_PVRSetClrFmt(pvr,dest);
	}
	else if(sbits == 16 && (dbits == 24 || dbits == 32)){
		pvr->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),dest);
		AGIDL_ColorConvertImgData(pvr->pixels.pix16,pvr->pixels.pix32,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr),dest);
		AGIDL_PVRSetClrFmt(pvr,dest);
		free(pvr->pixels.pix16);
	}
	else{
		pvr->pixels.pix16 = (COLOR16*)AGIDL_AllocImgDataMMU(AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),dest);
		AGIDL_ColorConvertImgData(pvr->pixels.pix32,pvr->pixels.pix16,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr),dest);
		AGIDL_PVRSetClrFmt(pvr,dest);
		free(pvr->pixels.pix32);
	}
}

void AGIDL_PVRConvertRGBA2RGB(AGIDL_PVR* pvr){
	if(pvr->fmt == AGIDL_RGBA_8888 || pvr->fmt == AGIDL_ARGB_8888){
		int x,y;
		for(y = 0; y < AGIDL_PVRGetHeight(pvr); y++){
			for(x = 0; x < AGIDL_PVRGetWidth(pvr); x++){
				COLOR clr = AGIDL_PVRGetClr(pvr,x,y);
				
				u8 r = AGIDL_GetR(clr,pvr->fmt);
				u8 g = AGIDL_GetG(clr,pvr->fmt);
				u8 b = AGIDL_GetB(clr,pvr->fmt);
				
				clr = AGIDL_RGB(r,g,b,AGIDL_RGB_888);
				AGIDL_PVRSetClr(pvr,x,y,clr);
			}
		}
	}
	AGIDL_PVRSetClrFmt(pvr,AGIDL_RGB_888);
}

AGIDL_PVR * AGIDL_CreatePVR(const char* filename, int width, int height, AGIDL_CLR_FMT fmt){
	AGIDL_PVR* pvr = (AGIDL_PVR*)malloc(sizeof(AGIDL_PVR));
	pvr->filename = (char*)malloc(strlen(filename)+1);
	AGIDL_FilenameCpy(pvr->filename,filename);
	AGIDL_PVRSetType(pvr,DREAMCAST_PVR);
	AGIDL_PVRSetWidth(pvr,width);
	AGIDL_PVRSetHeight(pvr,height);
	AGIDL_PVRSetClrFmt(pvr,fmt);
	AGIDL_PVRSetMaxDiff(pvr,7);
	AGIDL_PVRSetCompression(pvr,FALSE);
	pvr->mipmap = NULL;
	
	AGIDL_PVRBuildMipmap(pvr,FALSE);
	
	if(fmt == AGIDL_RGB_888 || fmt == AGIDL_BGR_888 || fmt == AGIDL_RGBA_8888 || fmt == AGIDL_ARGB_8888){
		pvr->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(width*height));
	}
	
	if(fmt == AGIDL_RGB_555 || fmt == AGIDL_BGR_555 || fmt == AGIDL_RGB_565 || fmt == AGIDL_BGR_565){
		pvr->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(width*height));
	}
	return pvr;
}

AGIDL_PVR* AGIDL_PVRCpyImg(AGIDL_PVR* pvr){
	AGIDL_PVR* pvrcpy = AGIDL_CreatePVR("pvrcpy.pvr",AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr));
	AGIDL_PVRSetType(pvrcpy,pvr->pvr_type);
	AGIDL_PVRSetICPMode(pvrcpy,pvr->icp);
	AGIDL_PVRSetMaxDiff(pvrcpy,AGIDL_PVRGetMaxDiff(pvr));
	
	memcpy(pvrcpy->mipmap,pvr->mipmap,sizeof(AGIDL_MIPMAP));
	
	AGIDL_CLR_FMT fmt = AGIDL_PVRGetClrFmt(pvr);
	if(fmt == AGIDL_RGB_888 || fmt == AGIDL_BGR_888 || fmt == AGIDL_RGBA_8888 || fmt == AGIDL_ARGB_8888){
		AGIDL_PVRSyncPix(pvrcpy,pvr->pixels.pix32);
	}
	
	if(fmt == AGIDL_RGB_555 || fmt == AGIDL_BGR_555 || fmt == AGIDL_RGB_565 || fmt == AGIDL_BGR_565){
		AGIDL_PVRSyncPix16(pvrcpy,pvr->pixels.pix16);
	}
	return pvrcpy;
}

PVRClrFmt AGIDL_GetPVRClrFmt(u8 byte){
	switch(byte){
		case 0x00:{
			return PVR_RGB_555;
		}break;
		case 0x01:{
			return PVR_RGB_565;
		}break;
		case 0x02:{
			return PVR_ARGB_4444;
		}break;
		case 0x03:{
			return PVR_YUV_442;
		}break;
		case 0x05:{
			return PVR_ICP_16;
		}break;
		case 0x06:{
			return PVR_ICP_256;
		}break;
		default: return PVR_RGB_565; break;
	}
}

PVRImgType AGIDL_PVRGetImgType(u8 byte){
	switch(byte){
		case 0x01:{
			return PVR_IMG_SQUARE_TWIDDLED;
		}break;
		case 0x02:{
			return PVR_IMG_SQUARE_TWIDDLED_AND_MIPMAPPED;
		}break;
		case 0x05:{
			return PVR_IMG_ICP_256_TWIDDLED;
		}break;
		case 0x06:{
			return PVR_IMG_ICP_16_TWIDDLED;
		}break;
		case 0x07:{
			return PVR_IMG_ICP_256;
		}break;
		case 0x08:{
			return PVR_IMG_ICP_16;
		}break;
		case 0x09:{
			return PVR_IMG_RECTANGLE;
		}break;
		case 0x0D:{
			return PVR_IMG_RECTANGLE_TWIDDLED;
		}break;
		default: return PVR_IMG_SQUARE_TWIDDLED; break;
	}
}

PVRPxlFmt AGIDL_GetPVRPxlFmt(u8 long2[8]){
	if(long2[0] == 'r' && long2[3] != 'a'){
		if(long2[4] == 8 && long2[5] == 8 && long2[6] == 8){
			return PVRTC_RGB_888;
		}
		else if(long2[4] == 5 && long2[5] == 5 && long2[6] == 5){
			return PVRTC_RGB_555;
		}
		else if(long2[4] == 5 && long2[5] == 6 && long2[6] == 5){
			return PVRTC_RGB_565;
		}
	}
	else if(long2[3] == 'a'){
		if(long2[0] == 'r' && long2[4] != 5){
			return PVRTC_RGBA_8888;
		}
		else if(long2[4] == 5){
			return PVRTC_RGB_555;
		}
		else{
			return PVRTC_BGRA_8888;
		}
	}
	else{
		switch(long2[0]){
			case 0:{
				return PVRTC_2BPP_RGB;
			}break;
			case 1:{
				return PVRTC_2BPP_RGBA;
			}break;
			case 2:{
				return PVRTC_4BPP_RGB;
			}break;
			case 3:{
				return PVRTC_4BPP_RGBA;
			}break;
			case 4:{
				return PVRTC_II_2BPP_RGB;
			}break;
			case 5:{
				return PVRTC_II_4BPP_RGB;
			}break;
		}
	}
	return 100;
}

int isImgPVR(u32 gbix, u32 pvrt){
	u8 x = ((gbix & 0xFF));
	u8 i = ((gbix & 0xFF00) >> 8);
	u8 b = ((gbix & 0xFF0000) >> 16);
	u8 g = ((gbix & 0xFF000000) >> 24);
	
	u8 p = ((pvrt & 0xFF));
	u8 v = ((pvrt & 0xFF00) >> 8);
	u8 r = ((pvrt & 0xFF0000) >> 16);
	u8 t = ((pvrt & 0xFF000000) >> 24);
	
	if(g != 'G' || b != 'B' || i != 'I' || x != 'X'){
		return 0;
	}
	else if(p != 'P' || v != 'V' || r != 'R' || t != 'T'){
		return 0;
	}
	else return 1;
}

int AGIDL_IsModernPVR(PVRPxlFmt fmt){
	if(fmt == 0x1 || fmt == 0x2 || fmt == 0x3 || fmt == 0x4 || fmt == 0x5 || fmt == 0x6 ||
	fmt == 0x7 || fmt == 0x8 || fmt == 0x9 || fmt == 0x10){
		return TRUE;
	}
	else return FALSE;
}

int AGIDL_PVRDecodeHeader(AGIDL_PVR* pvr, FILE* file){
	u8 ulong[4];
	ulong[0] = AGIDL_ReadByte(file);
	ulong[1] = AGIDL_ReadByte(file);
	ulong[2] = AGIDL_ReadByte(file);
	ulong[3] = AGIDL_ReadByte(file);
	
	if(ulong[0] == 'G' && ulong[1] == 'B' && ulong[2] == 'I' && ulong[3] == 'X'){
		pvr->header.id1 = ulong[0] << 24 | ulong[1] << 16 | ulong[2] << 8 | ulong[3];
	
		pvr->header.offset = AGIDL_ReadLong(file);
		pvr->header.global_index_1 = AGIDL_ReadLong(file);
		pvr->header.global_index_2 = AGIDL_ReadLong(file);
		pvr->header.id2 = AGIDL_ReadLong(file);
		pvr->header.file_size = AGIDL_ReadLong(file);
		pvr->header.pvr_clr_fmt = AGIDL_ReadByte(file);
		pvr->header.pvr_img_type = AGIDL_ReadByte(file);
		fseek(file,2,SEEK_CUR);
		pvr->header.width = AGIDL_ReadShort(file);
		pvr->header.height = AGIDL_ReadShort(file);
		
		pvr->pvr_type = DREAMCAST_PVR;
		
		if(!isImgPVR(pvr->header.id1,pvr->header.id2)){
			return INVALID_HEADER_FORMATTING_ERROR;
		}
		else return NO_IMG_ERROR;
	}
	else if(ulong[0] == 'P' && ulong[1] == 'V' && ulong[2] == 'R'){
		pvr->mheader.version = ulong[0] << 24 | ulong[1] << 16 | ulong[2] << 8 | ulong[3];
		
		pvr->mheader.flags = AGIDL_ReadLong(file);
		fread(&pvr->mheader.ulong2,1,8,file);
		pvr->mheader.clr_fmt = AGIDL_ReadLong(file);
		pvr->mheader.channel_type = AGIDL_ReadLong(file);
		pvr->mheader.height = AGIDL_ReadLong(file);
		pvr->mheader.width = AGIDL_ReadLong(file);
		pvr->mheader.depth = AGIDL_ReadLong(file);
		pvr->mheader.num_of_surfaces = AGIDL_ReadLong(file);
		pvr->mheader.num_of_faces = AGIDL_ReadLong(file);
		pvr->mheader.num_of_mipmaps = AGIDL_ReadLong(file);
		pvr->mheader.meta_data_size = AGIDL_ReadLong(file);
		
		if(pvr->mheader.meta_data_size != 0){
			pvr->mheader.fourcc[0] = AGIDL_ReadByte(file);
			pvr->mheader.fourcc[1] = AGIDL_ReadByte(file);
			pvr->mheader.fourcc[2] = AGIDL_ReadByte(file);
			pvr->mheader.fourcc[3] = AGIDL_ReadByte(file);
			pvr->mheader.key = AGIDL_ReadLong(file);
			pvr->mheader.data_size = AGIDL_ReadLong(file);
			fseek(file,pvr->mheader.data_size,SEEK_CUR);
		}
		
		pvr->pxl_fmt = AGIDL_GetPVRPxlFmt(pvr->mheader.ulong2);
		
		pvr->pvr_type = MODERN_PVR;
		
		if(AGIDL_IsModernPVR(pvr->pxl_fmt) && (pvr->mheader.clr_fmt == 0 || pvr->mheader.clr_fmt == 1)){
			return NO_IMG_ERROR;
		}
		else return INVALID_HEADER_FORMATTING_ERROR;
		
	}
	else return INVALID_HEADER_FORMATTING_ERROR;
}

void AGIDL_PVREncodeHeader(AGIDL_PVR* pvr, FILE* file){
	if(AGIDL_PVRGetType(pvr) == DREAMCAST_PVR){
		u8 g = 'G', b = 'B', i = 'I', x = 'X';
		u8 p = 'P', v = 'V', r = 'R', t = 'T';
		
		pvr->header.id1 = x << 24 | i << 16 | b << 8 | g;
		pvr->header.offset = 8;
		pvr->header.global_index_1 = 0;
		pvr->header.global_index_2 = 0;
		pvr->header.id2 = t << 24 | r << 16 | v << 8 | p;
		pvr->header.file_size = 30 + (2 * AGIDL_PVRGetWidth(pvr) * AGIDL_PVRGetHeight(pvr));
		
		if(pvr->fmt == AGIDL_RGB_565 && pvr->fmt == AGIDL_BGR_565){
			pvr->header.pvr_clr_fmt = PVR_RGB_565;
		}
		else{
			pvr->header.pvr_clr_fmt = PVR_RGB_555;
		}
	
		if(pvr->compression == TRUE){
			pvr->header.pvr_img_type = PVR_IMG_SQUARE_TWIDDLED;
		}
		else{
			pvr->header.pvr_img_type = PVR_IMG_RECTANGLE;
		}
		
		AGIDL_WriteLong(file,pvr->header.id1);
		AGIDL_WriteLong(file,pvr->header.offset);
		AGIDL_WriteLong(file,pvr->header.global_index_1);
		AGIDL_WriteLong(file,pvr->header.global_index_2);
		AGIDL_WriteLong(file,pvr->header.id2);
		AGIDL_WriteLong(file,pvr->header.file_size);
		AGIDL_WriteByte(file,pvr->header.pvr_clr_fmt);
		AGIDL_WriteByte(file,pvr->header.pvr_img_type);
		AGIDL_WriteShort(file,0);
		AGIDL_WriteShort(file,pvr->header.width);
		AGIDL_WriteShort(file,pvr->header.height);
	}
	else{
		AGIDL_CLR_FMT fmt = AGIDL_PVRGetClrFmt(pvr);
		
		u8 pvrc[4] = {'P','V','R',3};
		u32 flags = 0;
		u8 rgb[4];
		u8 bits[4];
		
		if(AGIDL_GetBitCount(fmt) == 32){
			rgb[0] = 'r'; rgb[1] = 'g'; rgb[2] = 'b'; rgb[3] = 'a';
			bits[0] = 8; bits[1] = 8; bits[2] = 8; bits[3] = 8;
		}
		else{
			rgb[0] = 'r'; rgb[1] = 'g'; rgb[2] = 'b'; rgb[3] = 0;
			
			if(AGIDL_GetBitCount(fmt) == 24){
				bits[0] = 8; bits[1] = 8; bits[2] = 8; bits[3] = 0;
			}
			else{
				if(fmt == AGIDL_RGB_555){
					rgb[3] = 'a';
					bits[0] = 5; bits[1] = 5; bits[2] = 5; bits[3] = 1;
				}
				else{
					bits[0] = 5; bits[1] = 6; bits[2] = 5; bits[3] = 0;
				}
			}
		}
		
		u32 clr_fmt = 1;
		u32 channel_type = 0;
		
		u32 one = 1, zero = 0;
		
		u16 w = pvr->mheader.width;
		u16 h = pvr->mheader.height;
		
		u32 count = 0;
		
		while(w >= 2 && h >= 2){
			w >>= 1;
			h >>= 1;
			
			count++;
		}
		
		AGIDL_WriteByte(file,pvrc[0]);
		AGIDL_WriteByte(file,pvrc[1]);
		AGIDL_WriteByte(file,pvrc[2]);
		AGIDL_WriteByte(file,pvrc[3]);
		AGIDL_WriteLong(file,flags);
		AGIDL_WriteByte(file,rgb[0]);
		AGIDL_WriteByte(file,rgb[1]);
		AGIDL_WriteByte(file,rgb[2]);
		AGIDL_WriteByte(file,rgb[3]);
		AGIDL_WriteByte(file,bits[0]);
		AGIDL_WriteByte(file,bits[1]);
		AGIDL_WriteByte(file,bits[2]);
		AGIDL_WriteByte(file,bits[3]);
		AGIDL_WriteLong(file,clr_fmt);
		AGIDL_WriteLong(file,channel_type);
		AGIDL_WriteLong(file,pvr->mheader.height);
		AGIDL_WriteLong(file,pvr->mheader.width);
		AGIDL_WriteLong(file,1);
		AGIDL_WriteLong(file,1);
		AGIDL_WriteLong(file,1);
		if(pvr->mipped = TRUE){
			AGIDL_WriteLong(file,count);
		}
		else{
			AGIDL_WriteLong(file,1);
		}
		AGIDL_WriteLong(file,0);
	}
}

void AGIDL_PVRDecodeImg(AGIDL_PVR* pvr, PVRClrFmt fmt, PVRImgType img, FILE* file){
	if(AGIDL_PVRGetType(pvr) == DREAMCAST_PVR){
		if(fmt == PVR_RGB_555 && img == PVR_IMG_RECTANGLE){
			AGIDL_PVRSetClrFmt(pvr,AGIDL_RGB_555);
			pvr->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_PVRGetWidth(pvr)*AGIDL_PVRGetHeight(pvr)));
			AGIDL_ReadBufRGB16(file,pvr->pixels.pix16,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr));
		}
		if(fmt == PVR_RGB_565 && img == PVR_IMG_RECTANGLE){
			AGIDL_PVRSetClrFmt(pvr,AGIDL_RGB_565);
			pvr->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_PVRGetWidth(pvr)*AGIDL_PVRGetHeight(pvr)));
			AGIDL_ReadBufRGB16(file,pvr->pixels.pix16,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr));
		}	
		if(fmt == PVR_ARGB_4444 && img == PVR_IMG_RECTANGLE){
			AGIDL_PVRSetClrFmt(pvr,AGIDL_RGB_555);
			pvr->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_PVRGetWidth(pvr)*AGIDL_PVRGetHeight(pvr)));
			
			int x,y;
			for(y = 0; y < AGIDL_PVRGetHeight(pvr); y++){
				for(x = 0; x < AGIDL_PVRGetWidth(pvr); x++){
					COLOR16 clr = AGIDL_ReadShort(file);
					
					u8 r = ((clr & 0xf00) >> 8);
					u8 g = ((clr & 0xf0) >> 4);
					u8 b = ((clr & 0xf));
					
					r = r << 1; g = g << 1; b = b << 1;
					
					r |= r >> 4; g |= g >> 4; b |= b >> 4;
					
					COLOR16 color = AGIDL_RGB16(r,g,b,AGIDL_RGB_555);
					
					AGIDL_PVRSetClr16(pvr,x,y,color);
				}
			}
		}
	}
	else{
		PVRPxlFmt fmt = pvr->pxl_fmt;

		switch(fmt){
			case PVRTC_RGB_888:{
				AGIDL_PVRSetClrFmt(pvr,AGIDL_RGB_888);
				pvr->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*AGIDL_PVRGetSize(pvr));
				
				if(pvr->mheader.clr_fmt == 0){
					if(pvr->mheader.num_of_mipmaps > 1){
						pvr->mipmap = AGIDL_LoadMipmapImgData(file,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr),pvr->mheader.num_of_mipmaps,TRUE);
					}
				}
				else{
					if(pvr->mheader.num_of_mipmaps > 1){
						pvr->mipmap = AGIDL_LoadMipmapImgData(file,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr),pvr->mheader.num_of_mipmaps,FALSE);
					}
				}
				
				COLOR* clr = (COLOR*)pvr->mipmap->mipmap[0].img_data;
				AGIDL_ClrMemcpy(pvr->pixels.pix32,clr,AGIDL_PVRGetSize(pvr));
			}break;
			case PVRTC_RGB_565:{
				if(pvr->mheader.clr_fmt == 0){
					if(pvr->mheader.num_of_mipmaps > 1){
						pvr->mipmap = AGIDL_LoadMipmapImgData(file,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr),pvr->mheader.num_of_mipmaps,TRUE);
					}
				}
				else{
					if(pvr->mheader.num_of_mipmaps > 1){
						pvr->mipmap = AGIDL_LoadMipmapImgData(file,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr),pvr->mheader.num_of_mipmaps,FALSE);
					}
				}
				
				AGIDL_ClrMemcpy16(pvr->pixels.pix16,pvr->mipmap->mipmap[0].img_data,AGIDL_PVRGetSize(pvr));
				
			}break;
			case PVRTC_RGB_555:{
				AGIDL_PVRSetClrFmt(pvr,AGIDL_RGB_555);
				pvr->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*AGIDL_PVRGetSize(pvr));
				
				if(pvr->mheader.clr_fmt == 0){
					if(pvr->mheader.num_of_mipmaps > 1){
						pvr->mipmap = AGIDL_LoadMipmapImgData(file,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr),pvr->mheader.num_of_mipmaps,TRUE);
					}
				}
				else{
					if(pvr->mheader.num_of_mipmaps > 1){
						pvr->mipmap = AGIDL_LoadMipmapImgData(file,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr),pvr->mheader.num_of_mipmaps,FALSE);
					}
				}
				
				AGIDL_ClrMemcpy16(pvr->pixels.pix16,pvr->mipmap->mipmap[0].img_data,AGIDL_PVRGetSize(pvr));
			}break;
			case PVRTC_RGBA_8888:{
				AGIDL_PVRSetClrFmt(pvr,AGIDL_RGBA_8888);
				pvr->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*AGIDL_PVRGetSize(pvr));
				
				if(pvr->mheader.clr_fmt == 0){
					if(pvr->mheader.num_of_mipmaps > 1){
						pvr->mipmap = AGIDL_LoadMipmapImgData(file,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr),pvr->mheader.num_of_mipmaps,TRUE);
					}
				}
				else{
					if(pvr->mheader.num_of_mipmaps > 1){
						pvr->mipmap = AGIDL_LoadMipmapImgData(file,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr),pvr->mheader.num_of_mipmaps,FALSE);
					}
				}
				
				AGIDL_ClrMemcpy(pvr->pixels.pix32,pvr->mipmap->mipmap[0].img_data,AGIDL_PVRGetSize(pvr));
			}break;
			case PVRTC_BGRA_8888:{
				AGIDL_PVRSetClrFmt(pvr,AGIDL_RGBA_8888);
				pvr->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*AGIDL_PVRGetSize(pvr));
				
				int x,y;
				for(y = 0; y < AGIDL_PVRGetHeight(pvr); y++){
					for(x = 0; x < AGIDL_PVRGetWidth(pvr); x++){
						u8 b = AGIDL_ReadByte(file);
						u8 g = AGIDL_ReadByte(file);
						u8 r = AGIDL_ReadByte(file);
						u8 a = AGIDL_ReadByte(file);
						
						COLOR clr = AGIDL_GammaCorrectColor(AGIDL_RGBA(r,g,b,a,AGIDL_RGBA_8888),2.2f,AGIDL_RGBA_8888);
						
						if(pvr->mheader.clr_fmt == 0){
							AGIDL_PVRSetClr(pvr,x,y,clr);
						}
						else{
							AGIDL_PVRSetClr(pvr,x,y,AGIDL_RGBA(r,g,b,a,AGIDL_RGBA_8888));
						}
					}
				}
			}break;
		}
	}
}

void AGIDL_PVREncodeIMG(AGIDL_PVR* pvr, FILE* file){
	if(AGIDL_PVRGetType(pvr) == DREAMCAST_PVR){
		AGIDL_WriteBufClr16(file,pvr->pixels.pix16,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr));
	}
	else{
		switch(AGIDL_PVRGetClrFmt(pvr)){
			case AGIDL_RGB_888:{
				if(pvr->mipped != TRUE){
					AGIDL_WriteBufRGB(file,pvr->pixels.pix32,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr));
				}
				else{
					pvr->mipmap = AGIDL_GenerateMipmapFromImgData(pvr->pixels.pix32,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr));

					AGIDL_WriteBufRGB(file,pvr->pixels.pix32,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr));
					
					int i;
					for(i = 1; i < pvr->mipmap->mipcount; i++){
						COLOR* clr_data = (COLOR*)pvr->mipmap->mipmap[i].img_data;
						AGIDL_WriteBufRGB(file,clr_data,pvr->mipmap->mipmap[i].width,pvr->mipmap->mipmap[i].height);
					}
				}
			}break;
			case AGIDL_RGB_565:{
				if(pvr->mipped != TRUE){
					AGIDL_WriteBufClr16(file,pvr->pixels.pix16,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr));
				}
				else{
					pvr->mipmap = AGIDL_GenerateMipmapFromImgData(pvr->pixels.pix16,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr));
					
					AGIDL_WriteBufClr16(file,pvr->pixels.pix16,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr));
					
					int i;
					for(i = 1; i < pvr->mipmap->mipcount; i++){
						COLOR16* clr_data = (COLOR16*)pvr->mipmap->mipmap[i].img_data;
						AGIDL_WriteBufClr16(file,clr_data,pvr->mipmap->mipmap[i].width,pvr->mipmap->mipmap[i].height);
					}
				}
			}break;
			case AGIDL_RGB_555:{
				int x,y;
				for(y = 0; y < AGIDL_PVRGetHeight(pvr); y++){
					for(x = 0; x < AGIDL_PVRGetWidth(pvr); x++){
						COLOR16 clr = AGIDL_PVRGetClr16(pvr,x,y);
						u8 r = AGIDL_GetR(clr,AGIDL_RGB_555);
						u8 g = AGIDL_GetG(clr,AGIDL_RGB_555);
						u8 b = AGIDL_GetB(clr,AGIDL_RGB_555);
						u8 a = 1;
						clr = r << 11 | g << 6 | b << 1 | a;
						AGIDL_WriteShort(file,clr);
					}
				}
			}break;
			case AGIDL_RGBA_8888:{
				if(pvr->mipped != TRUE){
					AGIDL_WriteBufRGBA(file,pvr->pixels.pix32,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr));
				}
				else{
					pvr->mipmap = AGIDL_GenerateMipmapFromImgData(pvr->pixels.pix32,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr));

					AGIDL_WriteBufRGB(file,pvr->pixels.pix32,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr));
					
					int i;
					for(i = 1; i < pvr->mipmap->mipcount; i++){
						COLOR* clr_data = (COLOR*)pvr->mipmap->mipmap[i].img_data;
						AGIDL_WriteBufRGBA(file,clr_data,pvr->mipmap->mipmap[i].width,pvr->mipmap->mipmap[i].height);
					}
				}
			}break;
		}
	}
}

u32 AGIDL_GetTwiddleValue(u32 i){
	u32 result = 0, resultbit = 1;
	while(i != 0){
		if((i & 1) != 0){
			result |= resultbit;
		}
		
		i >>= 1;
		
		resultbit <<= 2;
	}
	return result;
}

u32 AGIDL_GetDetwiddledIndex(u32 x, u32 y){
	return (AGIDL_GetTwiddleValue(x) << 1) | AGIDL_GetTwiddleValue(y);
}

void AGIDL_PVRDecodeTwiddledImg(AGIDL_PVR* pvr, PVRClrFmt fmt, PVRImgType img, FILE* file){
	if(fmt == PVR_RGB_555 && img == PVR_IMG_SQUARE_TWIDDLED){
		AGIDL_PVRSetClrFmt(pvr,AGIDL_RGB_555);
		pvr->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_PVRGetWidth(pvr)*AGIDL_PVRGetHeight(pvr)));
		
		COLOR16* buf = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_PVRGetWidth(pvr)*AGIDL_PVRGetHeight(pvr)));
		
		int img_size = AGIDL_PVRGetWidth(pvr) * AGIDL_PVRGetHeight(pvr);
		
		fread(buf,2,img_size,file);
		
		int x,y;
		for(y = 0; y < AGIDL_PVRGetHeight(pvr); y++){
			for(x = 0; x < AGIDL_PVRGetWidth(pvr); x++){
				
				int index = AGIDL_GetDetwiddledIndex(x,y);
				
				AGIDL_PVRSetClr16(pvr,x,y,buf[index]);
			}
		}
		
		free(buf);
	}	
	else if(fmt == PVR_RGB_565 && img == PVR_IMG_SQUARE_TWIDDLED){
		AGIDL_PVRSetClrFmt(pvr,AGIDL_RGB_565);
		pvr->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_PVRGetWidth(pvr)*AGIDL_PVRGetHeight(pvr)));
		
		COLOR16* buf = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_PVRGetWidth(pvr)*AGIDL_PVRGetHeight(pvr)));
		
		int img_size = AGIDL_PVRGetWidth(pvr) * AGIDL_PVRGetHeight(pvr);
		
		fread(buf,2,img_size,file);
		
		int x,y;
		for(y = 0; y < AGIDL_PVRGetHeight(pvr); y++){
			for(x = 0; x < AGIDL_PVRGetWidth(pvr); x++){
				
				int index = AGIDL_GetDetwiddledIndex(x,y);
				
				AGIDL_PVRSetClr16(pvr,x,y,buf[index]);
			}
		}
		
		free(buf);
	}
	else if(fmt == PVR_ARGB_4444 && img == PVR_IMG_SQUARE_TWIDDLED){
		AGIDL_PVRSetClrFmt(pvr,AGIDL_RGB_555);
		pvr->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_PVRGetWidth(pvr)*AGIDL_PVRGetHeight(pvr)));
		
		COLOR16* buf = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_PVRGetWidth(pvr)*AGIDL_PVRGetHeight(pvr)));
		
		int img_size = AGIDL_PVRGetWidth(pvr) * AGIDL_PVRGetHeight(pvr);
		
		fread(buf,2,img_size,file);
		
		int x,y;
		for(y = 0; y < AGIDL_PVRGetHeight(pvr); y++){
			for(x = 0; x < AGIDL_PVRGetWidth(pvr); x++){
				
				int index = AGIDL_GetDetwiddledIndex(x,y);
				
				COLOR16 clr = buf[index];
				
				u8 r = ((clr & 0xf00) >> 8);
				u8 g = ((clr & 0xf0) >> 4);
				u8 b = ((clr & 0xf));
				
				r = r << 1; g = g << 1; b = b << 1;
				
				r |= r >> 4; g |= g >> 4; b |= b >> 4;
				
				COLOR16 color = AGIDL_RGB16(r,g,b,AGIDL_RGB_555);
				
				AGIDL_PVRSetClr16(pvr,x,y,color);
			}
		}
		
		free(buf);
	}
	else if(fmt == PVR_RGB_555 && img == PVR_IMG_SQUARE_TWIDDLED_AND_MIPMAPPED){
		AGIDL_PVRSetClrFmt(pvr,AGIDL_RGB_555);
		pvr->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_PVRGetWidth(pvr)*AGIDL_PVRGetHeight(pvr)));
		
		AGIDL_PVRSetClrFmt(pvr,AGIDL_RGB_565);
		pvr->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_PVRGetWidth(pvr)*AGIDL_PVRGetHeight(pvr)));
		
		u16 w = 1;
		u16 h = 1;
		
		fseek(file,4,SEEK_CUR);
		
		COLOR16 clr = 0x7C00;
		
		pvr->mipmap = AGIDL_CreateMipmapMMU(AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr),FALSE);
		
		int count = 0;
		
		while(w < AGIDL_PVRGetWidth(pvr) && h < AGIDL_PVRGetWidth(pvr)){
			w <<= 1;
			h <<= 1;
			
			COLOR16* buf = (COLOR16*)AGIDL_AllocImgDataMMU(w,h,AGIDL_PVRGetClrFmt(pvr));
			
			if(!fread(buf,2,w*h,file)){
				break;
			}
			
			pvr->mipmap->mipmap[count].width = w;
			pvr->mipmap->mipmap[count].height = h;
			pvr->mipmap->mipmap[count].fmt = fmt;
			pvr->mipmap->mipmap[count].img_data = (COLOR16*)AGIDL_AllocImgDataMMU(w,h,AGIDL_PVRGetClrFmt(pvr));
			
			int x,y;
			for(y = 0; y < h; y++){
				for(x = 0; x < w; x++){
					
					int index = AGIDL_GetDetwiddledIndex(x,y);
					
					AGIDL_SetClr16(pvr->mipmap->mipmap[count].img_data,buf[index],x,y,w,h);
				}
			}
			
			count++;
			
			free(buf);
		}
		
		pvr->mipmap->mipcount = count-1;
		
		COLOR16* img = (COLOR16*)pvr->mipmap->mipmap[count-1].img_data;
		AGIDL_ClrMemcpy16(pvr->pixels.pix16,img,AGIDL_PVRGetSize(pvr));
	}
	else if(fmt == PVR_RGB_565 && img == PVR_IMG_SQUARE_TWIDDLED_AND_MIPMAPPED){
		AGIDL_PVRSetClrFmt(pvr,AGIDL_RGB_565);
		pvr->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_PVRGetWidth(pvr)*AGIDL_PVRGetHeight(pvr)));
		
		u16 w = 1;
		u16 h = 1;
		
		fseek(file,4,SEEK_CUR);
		
		COLOR16 clr = 0x7C00;
		
		pvr->mipmap = AGIDL_CreateMipmapMMU(AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr),FALSE);
		
		int count = 0;
		
		while(w < AGIDL_PVRGetWidth(pvr) && h < AGIDL_PVRGetWidth(pvr)){
			w <<= 1;
			h <<= 1;
			
			COLOR16* buf = (COLOR16*)AGIDL_AllocImgDataMMU(w,h,AGIDL_PVRGetClrFmt(pvr));
			
			if(!fread(buf,2,w*h,file)){
				break;
			}
			
			pvr->mipmap->mipmap[count].width = w;
			pvr->mipmap->mipmap[count].height = h;
			pvr->mipmap->mipmap[count].fmt = fmt;
			pvr->mipmap->mipmap[count].img_data = (COLOR16*)AGIDL_AllocImgDataMMU(w,h,AGIDL_PVRGetClrFmt(pvr));
			
			int x,y;
			for(y = 0; y < h; y++){
				for(x = 0; x < w; x++){
					
					int index = AGIDL_GetDetwiddledIndex(x,y);
					
					AGIDL_SetClr16(pvr->mipmap->mipmap[count].img_data,buf[index],x,y,w,h);
				}
			}
			
			count++;
			
			free(buf);
		}
		
		pvr->mipmap->mipcount = count-1;
		
		COLOR16* img = (COLOR16*)pvr->mipmap->mipmap[count-1].img_data;
		AGIDL_ClrMemcpy16(pvr->pixels.pix16,img,AGIDL_PVRGetSize(pvr));
	}
}

AGIDL_PVR * AGIDL_LoadPVR(char* filename){
	FILE* file = fopen(filename,"rb");
	
	if(file == NULL){
		printf("%s- %s!\n",AGIDL_Error2Str(FILE_NOT_LOCATED_IMG_ERROR),filename);
		return NULL;
	}
	
	AGIDL_PVR* pvr = (AGIDL_PVR*)malloc(sizeof(AGIDL_PVR));
	pvr->filename = (char*)malloc(strlen(filename)+1);
	AGIDL_FilenameCpy(pvr->filename,filename);
	AGIDL_PVRBuildMipmap(pvr,FALSE);
	pvr->mipmap = NULL;
	
	if(pvr == NULL || pvr->filename == NULL){
		printf("%s\n",AGIDL_Error2Str(MEMORY_IMG_ERROR));
		return NULL;
	}
	
	int error = AGIDL_PVRDecodeHeader(pvr,file);
	
	if(error != NO_IMG_ERROR){
		printf("%s\n",AGIDL_Error2Str(error));
		return NULL;
	}
	
	if(AGIDL_PVRGetType(pvr) == DREAMCAST_PVR){
		PVRClrFmt fmt = AGIDL_GetPVRClrFmt(pvr->header.pvr_clr_fmt);
		PVRImgType img = AGIDL_PVRGetImgType(pvr->header.pvr_img_type);
		
		AGIDL_PVRDecodeImg(pvr,fmt,img,file);
		AGIDL_PVRDecodeTwiddledImg(pvr,fmt,img,file);
	}
	else{
		AGIDL_PVRDecodeImg(pvr,0,0,file);
	}
	
	fclose(file);
	
	return pvr;
}

void AGIDL_ExportPVR(AGIDL_PVR* pvr){
	FILE* file = fopen(pvr->filename,"wb");
	
	if(file == NULL){
		printf("Could not create PVR image - %s!\n",pvr->filename);
	}
	
	AGIDL_PVREncodeHeader(pvr,file);
	
	switch(pvr->fmt){
		case AGIDL_RGB_555:{
			AGIDL_PVREncodeIMG(pvr,file);
		}break;	
		case AGIDL_RGB_565:{
			AGIDL_PVREncodeIMG(pvr,file);
		}break;
		case AGIDL_BGR_555:{
			AGIDL_PVRBGR2RGB(pvr);
			AGIDL_PVREncodeIMG(pvr,file);
			AGIDL_PVRRGB2BGR(pvr);
		}break;
		case AGIDL_BGR_565:{
			AGIDL_PVRBGR2RGB(pvr);
			AGIDL_PVREncodeIMG(pvr,file);
			AGIDL_PVRRGB2BGR(pvr);
		}break;
		case AGIDL_RGB_888:{
			if(AGIDL_PVRGetType(pvr) == DREAMCAST_PVR){
				AGIDL_PVRConvert24BPPTO16BPP(pvr);
				AGIDL_PVREncodeIMG(pvr,file);
				AGIDL_PVRConvert16BPPTO24BPP(pvr);
			}
			else{
				AGIDL_PVREncodeIMG(pvr,file);
			}
		}break;
		case AGIDL_BGR_888:{
			if(AGIDL_PVRGetType(pvr) == DREAMCAST_PVR){
				AGIDL_PVRConvert24BPPTO16BPP(pvr);
				AGIDL_PVRBGR2RGB(pvr);
				AGIDL_PVREncodeIMG(pvr,file);
				AGIDL_PVRRGB2BGR(pvr);
				AGIDL_PVRConvert16BPPTO24BPP(pvr);
			}
			else{
				AGIDL_PVRBGR2RGB(pvr);
				AGIDL_PVREncodeIMG(pvr,file);
				AGIDL_PVRRGB2BGR(pvr);
			}
		}break;
		case AGIDL_RGBA_8888:{
			if(AGIDL_PVRGetType(pvr) == DREAMCAST_PVR){
				COLOR* buf = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr));
				AGIDL_ClrMemcpy(buf,pvr->pixels.pix32,AGIDL_PVRGetSize(pvr));
				AGIDL_CLR_FMT src = AGIDL_PVRGetClrFmt(pvr);
				AGIDL_ColorConvertPVR(pvr,AGIDL_RGB_555);
				AGIDL_PVREncodeIMG(pvr,file);
				AGIDL_ColorConvertPVR(pvr,src);
				AGIDL_ClrMemcpy(pvr->pixels.pix32,buf,AGIDL_PVRGetSize(pvr));
				free(buf);
			}
			else{
				AGIDL_PVREncodeIMG(pvr,file);
			}
		}break;
		case AGIDL_ARGB_8888:{
			if(AGIDL_PVRGetType(pvr) == DREAMCAST_PVR){
				COLOR* buf = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr));
				AGIDL_ClrMemcpy(buf,pvr->pixels.pix32,AGIDL_PVRGetSize(pvr));
				AGIDL_CLR_FMT src = AGIDL_PVRGetClrFmt(pvr);
				AGIDL_ColorConvertPVR(pvr,AGIDL_RGB_555);
				AGIDL_PVREncodeIMG(pvr,file);
				AGIDL_ColorConvertPVR(pvr,src);
				AGIDL_ClrMemcpy(pvr->pixels.pix32,buf,AGIDL_PVRGetSize(pvr));
				free(buf);
			}
			else{
				AGIDL_PVREncodeIMG(pvr,file);
			}
		}break;
	}
	
	fclose(file);
}