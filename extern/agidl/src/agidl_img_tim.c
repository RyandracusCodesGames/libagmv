#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "agidl_cc_core.h"
#include "agidl_img_tim.h"

#include <agidl_mmu_utils.h>

#include "agidl_img_error.h"
#include "agidl_file_utils.h"
#include "agidl_imgp_scale.h"
/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_tim.c
*   Date: 9/19/2023
*   Version: 0.1b
*   Updated: 2/26/2024
*   Author: Ryandracus Chapman
*
********************************************/

void AGIDL_SetTIMFilename(AGIDL_TIM *tim, const char *filename){
	tim->filename = (char*)realloc(tim->filename,strlen(filename));
	AGIDL_FilenameCpy(tim->filename,filename);
}

void AGIDL_TIMSetWidth(AGIDL_TIM *tim, int width){
	tim->img_header.width = width;
}

void AGIDL_TIMSetHeight(AGIDL_TIM *tim, int height){
	tim->img_header.height = height;
}

void AGIDL_TIMSetClrFmt(AGIDL_TIM *tim, AGIDL_CLR_FMT fmt){
	tim->fmt = fmt;
}

void AGIDL_TIMSetClr(AGIDL_TIM *tim, int x, int y, COLOR clr){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) != 16){
		AGIDL_SetClr(tim->pixels.pix32,clr,x,y,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim));
	}
	else{
		AGIDL_SetClr16(tim->pixels.pix16,AGIDL_CLR_TO_CLR16(clr,AGIDL_BGR_888,AGIDL_TIMGetClrFmt(tim)),x,y,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim));
	}
}

void AGIDL_TIMSetClr16(AGIDL_TIM *tim, int x, int y, COLOR16 clr){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim) == 16)){
		AGIDL_SetClr16(tim->pixels.pix16,clr,x,y,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim));
	}
	else{
		AGIDL_SetClr(tim->pixels.pix32,AGIDL_CLR16_TO_CLR(clr,AGIDL_RGB_555,AGIDL_TIMGetClrFmt(tim)),x,y,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim));
	}
}

void AGIDL_TIMSetRGB(AGIDL_TIM *tim, int x, int y, u8 r, u8 g, u8 b){
	switch(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim))){
		case 24:{
			AGIDL_TIMSetClr(tim,x,y,AGIDL_RGB(r,g,b,AGIDL_TIMGetClrFmt(tim)));
		}break;
		case 16:{
			AGIDL_TIMSetClr16(tim,x,y,AGIDL_RGB16(r,g,b,AGIDL_TIMGetClrFmt(tim)));
		}break;
		case 32:{
			AGIDL_TIMSetClr(tim,x,y,AGIDL_RGBA(r,g,b,0xff,AGIDL_TIMGetClrFmt(tim)));
		}break;
	}
}

void AGIDL_TIMSetICPMode(AGIDL_TIM *tim, int mode){
	tim->icp = mode;
}

void AGIDL_TIMSetICPEncoding(AGIDL_TIM* tim, AGIDL_ICP_ENCODE encode){
	tim->encode = encode;
}

void AGIDL_TIMSetMaxDiff(AGIDL_TIM* tim, int max_diff){
	tim->max_diff = max_diff;
}

void AGIDL_TIMForce4BPPEncoding(AGIDL_TIM* tim, AGIDL_Bool force4bpp){
	tim->force4bpp = force4bpp;
}

void AGIDL_TIMInitAttributes(AGIDL_TIM* tim, u16 pal_mem_add_x, u16 pal_mem_add_y, u16 img_mem_add_x, u16 img_mem_add_y){
	tim->clut_header.pal_mem_add_x = pal_mem_add_x;
	tim->clut_header.pal_mem_add_y = pal_mem_add_y;
	tim->img_header.img_mem_add_x = img_mem_add_x;
	tim->img_header.img_mem_add_y = img_mem_add_y;
}

void AGIDL_ClearTIM(AGIDL_TIM *tim, COLOR clr){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) != 16){
		AGIDL_ClrMemset(tim->pixels.pix32,clr,AGIDL_TIMGetSize(tim));
	}
	else{
		AGIDL_ClrMemset16(tim->pixels.pix16,(COLOR16)clr,AGIDL_TIMGetSize(tim));
	}
}

void AGIDL_ClearTIM16(AGIDL_TIM *tim, COLOR16 clr){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 16){
		AGIDL_ClrMemset16(tim->pixels.pix16,clr,AGIDL_TIMGetSize(tim));
	}
	else{
		AGIDL_ClrMemset(tim->pixels.pix32,AGIDL_CLR16_TO_CLR(clr,AGIDL_RGB_555,AGIDL_TIMGetClrFmt(tim)),AGIDL_TIMGetSize(tim));
	}
}

void AGIDL_ClearColorTIM(AGIDL_TIM* tim, float r, float g, float b){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 16){
		AGIDL_ClearColorBuffer(tim->pixels.pix16,r,g,b,AGIDL_TIMGetClrFmt(tim),AGIDL_TIMGetSize(tim));
	}
	else{
		AGIDL_ClearColorBuffer(tim->pixels.pix32,r,g,b,AGIDL_TIMGetClrFmt(tim),AGIDL_TIMGetSize(tim));
	}
}

void AGIDL_FlushTIM(AGIDL_TIM* tim){
	AGIDL_ClearTIM(tim,0);
}

int AGIDL_TIMGetWidth(AGIDL_TIM *tim){
	return tim->img_header.width;
}

int AGIDL_TIMGetHeight(AGIDL_TIM *tim){
	return tim->img_header.height;
}

u32 AGIDL_TIMGetSize(AGIDL_TIM* tim){
	return AGIDL_TIMGetWidth(tim) * AGIDL_TIMGetHeight(tim);
}

int AGIDL_TIMGetMaxDiff(AGIDL_TIM* tim){
	return tim->max_diff;
}

AGIDL_CLR_FMT AGIDL_TIMGetClrFmt(AGIDL_TIM* tim){
	return tim->fmt;
}

COLOR AGIDL_TIMGetClr(AGIDL_TIM *tim, int x, int y){
	if(x >= 0 && y >= 0 && x < AGIDL_TIMGetWidth(tim) && y < AGIDL_TIMGetHeight(tim)){
		return tim->pixels.pix32[x+y*AGIDL_TIMGetWidth(tim)];
	}
}

COLOR16 AGIDL_TIMGetClr16(AGIDL_TIM *tim, int x, int y){
	if(x >= 0 && y >= 0 && x < AGIDL_TIMGetWidth(tim) && y < AGIDL_TIMGetHeight(tim)){
		return tim->pixels.pix16[x+y*AGIDL_TIMGetWidth(tim)];
	}
}

int AGIDL_IsTIM(AGIDL_TIM* tim){
	if(tim->header.version == TIM_8BPP || tim->header.version == TIM_4BPP){
		if(AGIDL_IsTIMHeader(tim) && AGIDL_TIMGetWidth(tim) > 0 && AGIDL_TIMGetHeight(tim) > 0 && AGIDL_TIMGetWidth(tim) < 1024
		&& AGIDL_TIMGetHeight(tim) < 1024 && tim->clut_header.num_icps > 0 && tim->clut_header.num_clrs >= 16
		&& tim->clut_header.num_clrs <= 256){
			return 1;
		}
		else return 0;
	}
	else{
		if(AGIDL_IsTIMHeader(tim) && AGIDL_TIMGetWidth(tim) > 0 && AGIDL_TIMGetHeight(tim) > 0 && AGIDL_TIMGetWidth(tim) < 1024
		&& AGIDL_TIMGetHeight(tim) < 1024){
			return 1;
		}
		else return 0;
	}
}

int AGIDL_IsTIMHeader(AGIDL_TIM* tim){
	if(tim->header.magic == 0x10 && (tim->header.version == 0x02
		|| tim->header.version == 0x03 || tim->header.version == 0x08 || tim->header.version == 0x09)){
			return 1;
		}
		else return 0;
}

void AGIDL_FreeTIM(AGIDL_TIM *tim){
	free(tim->filename);
	
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 16){
		free(tim->pixels.pix16);
	}
	else{
		free(tim->pixels.pix32);
	}
	
	free(tim);

	if(tim != NULL){
		tim = NULL;
	}
}

void AGIDL_TIMRGB2BGR(AGIDL_TIM *tim){
	if(tim->fmt == AGIDL_RGB_888){
		AGIDL_RGB2BGR(tim->pixels.pix32,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),&tim->fmt);
	}
	
	if(tim->fmt == AGIDL_RGB_555){
		AGIDL_RGB2BGR16(tim->pixels.pix16,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),&tim->fmt);
	}
}


void AGIDL_TIMBGR2RGB(AGIDL_TIM *tim){
	if(tim->fmt == AGIDL_BGR_888){
		AGIDL_BGR2RGB(tim->pixels.pix32,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),&tim->fmt);
	}
	
	if(tim->fmt == AGIDL_BGR_555){
		AGIDL_BGR2RGB16(tim->pixels.pix16,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),&tim->fmt);
	}
}

void AGIDL_TIMConvert16BPPTO24BPP(AGIDL_TIM *tim){
	if(tim->fmt == AGIDL_RGB_555 || tim->fmt == AGIDL_BGR_555){
		tim->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_TIMGetHeight(tim)*AGIDL_TIMGetWidth(tim)));
		AGIDL_16BPPTO24BPP(tim->pixels.pix16,tim->pixels.pix32,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),&tim->fmt);
		free(tim->pixels.pix16);
	}
}

void AGIDL_TIMConvert24BPPTO16BPP(AGIDL_TIM *tim){
	if(tim->fmt == AGIDL_RGB_888 || tim->fmt == AGIDL_BGR_888 || tim->fmt == AGIDL_RGBA_8888 || tim->fmt == AGIDL_ARGB_8888){
		tim->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_TIMGetHeight(tim)*AGIDL_TIMGetWidth(tim)));
		AGIDL_24BPPTO16BPP(tim->pixels.pix32,tim->pixels.pix16,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),&tim->fmt);
		free(tim->pixels.pix32);
	}
}

void AGIDL_TIMConvert555TO565(AGIDL_TIM *tim){
	AGIDL_555TO565(tim->pixels.pix16,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),&tim->fmt);
}

void AGIDL_TIMConvert565TO555(AGIDL_TIM *tim){
	AGIDL_565TO555(tim->pixels.pix16,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),&tim->fmt);
}

void AGIDL_ColorConvertTIM(AGIDL_TIM* tim, AGIDL_CLR_FMT dest){
	u8 sbits = AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)), dbits = AGIDL_GetBitCount(dest);
	if(sbits == 16 && dbits == 16){
		AGIDL_ColorConvertImgData(tim->pixels.pix16,NULL,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),AGIDL_TIMGetClrFmt(tim),dest);
		AGIDL_TIMSetClrFmt(tim,dest);
	}
	else if((sbits == 24 || sbits == 32) && (dbits == 24 || dbits == 32)){
		AGIDL_ColorConvertImgData(tim->pixels.pix32,NULL,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),AGIDL_TIMGetClrFmt(tim),dest);
		AGIDL_TIMSetClrFmt(tim,dest);
	}
	else if(sbits == 16 && (dbits == 24 || dbits == 32)){
		tim->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),dest);
		AGIDL_ColorConvertImgData(tim->pixels.pix16,tim->pixels.pix32,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),AGIDL_TIMGetClrFmt(tim),dest);
		AGIDL_TIMSetClrFmt(tim,dest);
		free(tim->pixels.pix16);
	}
	else{
		tim->pixels.pix16 = (COLOR16*)AGIDL_AllocImgDataMMU(AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),dest);
		AGIDL_ColorConvertImgData(tim->pixels.pix32,tim->pixels.pix16,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),AGIDL_TIMGetClrFmt(tim),dest);
		AGIDL_TIMSetClrFmt(tim,dest);
		free(tim->pixels.pix32);
	}
}

void AGIDL_TIMSyncPix(AGIDL_TIM *tim, COLOR *clrs){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) != 16){
		AGIDL_ClrMemcpy(tim->pixels.pix32,clrs,AGIDL_TIMGetSize(tim));
	}
}

void AGIDL_TIMSyncPix16(AGIDL_TIM *tim, COLOR16 *clrs){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 16){
		AGIDL_ClrMemcpy16(tim->pixels.pix16,clrs,AGIDL_TIMGetSize(tim));
	}
}

void AGIDL_TIMCopyPix(AGIDL_TIM* tim, COLOR* clrs, u32 count){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) != 16){
		AGIDL_ClrMemcpy(tim->pixels.pix32,clrs,count);
	}
}

void AGIDL_TIMCopyPix16(AGIDL_TIM* tim, COLOR16* clrs, u32 count){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 16){
		AGIDL_ClrMemcpy16(tim->pixels.pix16,clrs,count);
	}
}

AGIDL_TIM * AGIDL_CreateTIM(const char *filename, int width, int height, AGIDL_CLR_FMT fmt){
	AGIDL_TIM *tim = (AGIDL_TIM*)malloc(sizeof(AGIDL_TIM));
	tim->filename = (char*)malloc(strlen(filename)+1);
	AGIDL_FilenameCpy(tim->filename,filename);
	AGIDL_TIMSetWidth(tim,width);
	AGIDL_TIMSetHeight(tim,height);
	AGIDL_TIMSetClrFmt(tim,fmt);
	AGIDL_TIMSetMaxDiff(tim,7);
	AGIDL_TIMSetICPEncoding(tim,ICP_ENCODE_HISTOGRAM);
	AGIDL_TIMForce4BPPEncoding(tim,FALSE);
	AGIDL_TIMInitAttributes(tim,0,0,0,0);
	
	if(fmt == AGIDL_RGB_888 || fmt == AGIDL_BGR_888 || fmt == AGIDL_RGBA_8888 || fmt == AGIDL_ARGB_8888){
		tim->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(width*height));
	}
	
	if(fmt == AGIDL_RGB_555 || fmt == AGIDL_BGR_555 || fmt == AGIDL_RGB_565 || fmt == AGIDL_BGR_565){
		tim->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(width*height));
	}
	return tim;
}

AGIDL_TIM* AGIDL_TIMCpyImg(AGIDL_TIM* tim){
	AGIDL_TIM* timcpy = AGIDL_CreateTIM("timcpy.tim",AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),tim->fmt);
	AGIDL_TIMSetICPMode(timcpy,tim->icp);
	AGIDL_TIMSetMaxDiff(timcpy,AGIDL_TIMGetMaxDiff(tim));
	AGIDL_TIMSetICPEncoding(timcpy,tim->encode);
	
	if(tim->fmt == AGIDL_RGB_888 || tim->fmt == AGIDL_BGR_888 || tim->fmt == AGIDL_RGBA_8888 || tim->fmt == AGIDL_ARGB_8888){
		AGIDL_TIMSyncPix(timcpy,tim->pixels.pix32);
	}
	
	if(tim->fmt == AGIDL_RGB_555 || tim->fmt == AGIDL_BGR_555 || tim->fmt == AGIDL_RGB_565 || tim->fmt == AGIDL_BGR_565){
		AGIDL_TIMSyncPix16(timcpy,tim->pixels.pix16);
	}
	return timcpy;
}

int AGIDL_TIMDecodeHeader(AGIDL_TIM* tim, FILE* file){
	tim->header.magic = AGIDL_ReadLong(file);
	tim->header.version = AGIDL_ReadLong(file);
	
	if(!AGIDL_IsTIMHeader(tim)){
		return INVALID_HEADER_FORMATTING_ERROR;
	}
	else return NO_IMG_ERROR;
}

int AGIDL_TIMDecodeIMG(AGIDL_TIM* tim, FILE* file){
	if(tim->header.version == TIM_4BPP){
		AGIDL_TIMSetClrFmt(tim,AGIDL_BGR_555);
		
		tim->clut_header.clut_size = AGIDL_ReadLong(file);
		tim->clut_header.pal_mem_add_x = AGIDL_ReadShort(file);
		tim->clut_header.pal_mem_add_y = AGIDL_ReadShort(file);
		tim->clut_header.num_clrs = AGIDL_ReadShort(file);
		tim->clut_header.num_icps = AGIDL_ReadShort(file);
		
		if(tim->clut_header.num_clrs == 16 || tim->clut_header.num_icps == 16){
			int i;
			for(i = 0; i < 16; i++){
				COLOR16 clr = AGIDL_ReadShort(file);
				tim->palette.icp.palette_16b_16[i] = clr;
			}
			
			tim->img_header.img_size = AGIDL_ReadLong(file);
			tim->img_header.img_mem_add_x = AGIDL_ReadShort(file);
			tim->img_header.img_mem_add_y = AGIDL_ReadShort(file);
			tim->img_header.width = AGIDL_ReadShort(file);
			tim->img_header.height = AGIDL_ReadShort(file);
			
			tim->img_header.width *= 4;
			
			
			if(!AGIDL_IsTIM(tim)){
				return INVALID_IMG_FORMATTING_ERROR;
			}
			
			tim->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_TIMGetWidth(tim)*AGIDL_TIMGetHeight(tim)));
			
			int x,y;
			for(y = 0; y < AGIDL_TIMGetHeight(tim); y++){
				for(x = 0; x < AGIDL_TIMGetWidth(tim); x+=4){
					u16 index = AGIDL_ReadShort(file);
					
					u16 index1 = (index & 0xF);
					u16 index2 = (index & 0xF0) >> 4;
					u16 index3 = (index & 0xF00) >> 8;
					u16 index4 = (index & 0xF000) >> 12;
					
					COLOR16 clr  = tim->palette.icp.palette_16b_16[index1];
					COLOR16 clr2 = tim->palette.icp.palette_16b_16[index2];
					COLOR16 clr3 = tim->palette.icp.palette_16b_16[index3];
					COLOR16 clr4 = tim->palette.icp.palette_16b_16[index4];
					
					AGIDL_TIMSetClr16(tim,x,y,clr);
					AGIDL_TIMSetClr16(tim,x+1,y,clr2);
					AGIDL_TIMSetClr16(tim,x+2,y,clr3);
					AGIDL_TIMSetClr16(tim,x+3,y,clr4);
				}
			}
		}
	}
	else if(tim->header.version == TIM_8BPP){

		AGIDL_TIMSetClrFmt(tim,AGIDL_BGR_555);
		
		tim->clut_header.clut_size = AGIDL_ReadLong(file);
		tim->clut_header.pal_mem_add_x = AGIDL_ReadShort(file);
		tim->clut_header.pal_mem_add_y = AGIDL_ReadShort(file);
		tim->clut_header.num_clrs = AGIDL_ReadShort(file);
		tim->clut_header.num_icps = AGIDL_ReadShort(file);
	
		if(tim->clut_header.num_clrs == 256 || tim->clut_header.num_icps == 256){
	
			int i;
			for(i = 0; i < 256; i++){
				COLOR16 clr = 0;
				fread(&clr,2,1,file);
				tim->palette.icp.palette_16b_256[i] = clr;
			}
			
			tim->img_header.img_size = AGIDL_ReadLong(file);
			tim->img_header.img_mem_add_x = AGIDL_ReadShort(file);
			tim->img_header.img_mem_add_y = AGIDL_ReadShort(file);
			tim->img_header.width = AGIDL_ReadShort(file);
			tim->img_header.height = AGIDL_ReadShort(file);
			
			tim->img_header.width *= 2;
			
			if(!AGIDL_IsTIM(tim)){
				return INVALID_IMG_FORMATTING_ERROR;
			}
			
			tim->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_TIMGetWidth(tim)*AGIDL_TIMGetHeight(tim)));
			
			for(i = 0; i < AGIDL_TIMGetSize(tim); i++){
				u8 index = AGIDL_ReadByte(file);
				
				COLOR16 clr = tim->palette.icp.palette_16b_256[index];
				
				tim->pixels.pix16[i] = clr;
			}
		}
	}
	else if(tim->header.version == TIM_16BPP){
	
		AGIDL_TIMSetClrFmt(tim,AGIDL_BGR_555);
		
		tim->img_header.img_size = AGIDL_ReadLong(file);
		tim->img_header.img_mem_add_x = AGIDL_ReadShort(file);
		tim->img_header.img_mem_add_y = AGIDL_ReadShort(file);
		tim->img_header.width = AGIDL_ReadShort(file);
		tim->img_header.height = AGIDL_ReadShort(file);
		
		if(!AGIDL_IsTIM(tim)){
			return INVALID_IMG_FORMATTING_ERROR;
		}
		
		tim->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_TIMGetWidth(tim)*AGIDL_TIMGetHeight(tim)));
		AGIDL_ReadBufRGB16(file,tim->pixels.pix16,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim));
	}
	else{
		
		AGIDL_TIMSetClrFmt(tim,AGIDL_RGB_888);
	
		tim->img_header.img_size = AGIDL_ReadLong(file);
		tim->img_header.img_mem_add_x = AGIDL_ReadShort(file);
		tim->img_header.img_mem_add_y = AGIDL_ReadShort(file);
		tim->img_header.width = AGIDL_ReadShort(file);
		tim->img_header.height = AGIDL_ReadShort(file);
		
		if(!AGIDL_IsTIM(tim)){
			return INVALID_IMG_FORMATTING_ERROR;
		}
		
		tim->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_TIMGetWidth(tim)*AGIDL_TIMGetHeight(tim)));
		
		int x,y;
		for(y = 0; y < AGIDL_TIMGetHeight(tim); y++){
			for(x = 0; x < AGIDL_TIMGetWidth(tim); x += 2){
				u16 pix1 = AGIDL_ReadShort(file);
				u16 pix2 = AGIDL_ReadShort(file);
				u16 pix3 = AGIDL_ReadShort(file);
				
				u8 r0 = (pix1 & 0xff);
				u8 g0 = (pix1 & 0xff00) >> 8;
				u8 b0 = (pix2 & 0xff);
				u8 r1 = (pix2 & 0xff00) >> 8;
				u8 g1 = (pix3 & 0xff);
				u8 b1 = (pix3 & 0xff00) >> 8;
				
				AGIDL_TIMSetClr(tim,x,y,AGIDL_RGB(r0,g0,b0,AGIDL_RGB_888));
				AGIDL_TIMSetClr(tim,x+1,y,AGIDL_RGB(r1,g1,b1,AGIDL_RGB_888));
			}
		}
	}
	
	return NO_IMG_ERROR;
}

void AGIDL_TIMEncodeICP(AGIDL_TIM* tim){
	if(tim->encode == ICP_ENCODE_THRESHOLD){
		int pass = 0;
		u8 pal_index = 0;
		
		AGIDL_InitICP(&tim->palette, AGIDL_ICP_16b_256);
		
		int x,y;
		for(y = 0; y < AGIDL_TIMGetHeight(tim); y++){
			for(x = 0; x < AGIDL_TIMGetWidth(tim); x++){
				COLOR16 clr16;
				if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 32){
					COLOR clr = AGIDL_TIMGetClr(tim,x,y);
					u8 r = AGIDL_GetR(clr,AGIDL_TIMGetClrFmt(tim));
					u8 g = AGIDL_GetG(clr,AGIDL_TIMGetClrFmt(tim));
					u8 b = AGIDL_GetB(clr,AGIDL_TIMGetClrFmt(tim));
					clr16 = AGIDL_CLR_TO_CLR16(clr,AGIDL_TIMGetClrFmt(tim),AGIDL_BGR_555);
				}
				else{
					clr16 = AGIDL_TIMGetClr16(tim,x,y);
				}
				
				AGIDL_AddColorICP16(&tim->palette,pal_index,clr16,AGIDL_TIMGetClrFmt(tim),AGIDL_TIMGetMaxDiff(tim),&pass);
				
				if(pass == 1 && pal_index < 256){
					pal_index++;
				}
				
				pass = 0;
			}
		}
	}
	else{
		AGIDL_EncodeHistogramICP(&tim->palette,tim->pixels.pix16,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),AGIDL_TIMGetClrFmt(tim));
	}
}

void AGIDL_TIMEncodeNearestICP(AGIDL_TIM* tim, AGIDL_ICP palette, FILE* file){
	AGIDL_CLR_FMT palfmt = palette.fmt;
	
	if(AGIDL_GetBitCount(palfmt) == 16 && AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 16){
		u32 pos = ftell(file);
		fseek(file,20,SEEK_SET);
		if(palette.mode == AGIDL_ICP_16b_256){
			int i;
			for(i = 0; i < 256; i++){
				COLOR16 clr = palette.icp.palette_16b_256[i];
				AGIDL_WriteShort(file,clr);
			}
		}
		if(palette.mode == AGIDL_ICP_16b_16){
			int i;
			for(i = 0; i < 256; i++){
				if(i < 16){
					COLOR16 clr = palette.icp.palette_16b_16[i];
					AGIDL_WriteShort(file,clr);
				}
				else{
					AGIDL_WriteShort(file,0);
				}
			}
		}
		fseek(file,pos,SEEK_SET);
		
		int i;
		for(i = 0; i < AGIDL_TIMGetSize(tim); i++){
			COLOR16 clr = tim->pixels.pix16[i];
			u8 index = AGIDL_FindNearestColor(palette,clr,AGIDL_TIMGetClrFmt(tim));
			AGIDL_WriteByte(file,index);
		}
	}
}

void AGIDL_TIMEncodeHeader(AGIDL_TIM* tim, FILE* file){
	if(tim->icp == 1){
		tim->header.magic = TIM_MAGIC;
		
		if(tim->force4bpp != TRUE){
			tim->header.version = TIM_8BPP;
			tim->clut_header.clut_size = (2*256)+12;
			tim->clut_header.num_clrs = 256;
			tim->clut_header.num_icps = 1;
			
			AGIDL_WriteLong(file,tim->header.magic);
			AGIDL_WriteLong(file,tim->header.version);
			AGIDL_WriteLong(file,tim->clut_header.clut_size);
			AGIDL_WriteShort(file,tim->clut_header.pal_mem_add_x);
			AGIDL_WriteShort(file,tim->clut_header.pal_mem_add_y);
			AGIDL_WriteShort(file,tim->clut_header.num_clrs);
			AGIDL_WriteShort(file,tim->clut_header.num_icps);
			
			int i;
			for(i = 0; i < 256; i++){
				AGIDL_WriteShort(file,tim->palette.icp.palette_16b_256[i]);
			}
		}
		else{
			tim->header.version = TIM_4BPP;
			tim->clut_header.clut_size = (2*16)+12;
			tim->clut_header.num_clrs = 16;
			tim->clut_header.num_icps = 1;
			
			AGIDL_WriteLong(file,tim->header.magic);
			AGIDL_WriteLong(file,tim->header.version);
			AGIDL_WriteLong(file,tim->clut_header.clut_size);
			AGIDL_WriteShort(file,tim->clut_header.pal_mem_add_x);
			AGIDL_WriteShort(file,tim->clut_header.pal_mem_add_y);
			AGIDL_WriteShort(file,tim->clut_header.num_clrs);
			AGIDL_WriteShort(file,tim->clut_header.num_icps);
			
			int i;
			for(i = 0; i < 16; i++){
				AGIDL_WriteShort(file,tim->palette.icp.palette_16b_256[i]);
			}
		}
		
		tim->img_header.img_size = (AGIDL_TIMGetWidth(tim)*2)*(AGIDL_TIMGetHeight(tim))+12;
		
		u16 width = AGIDL_TIMGetWidth(tim);
		u16 height = AGIDL_TIMGetHeight(tim);
		
		if(tim->force4bpp != TRUE){
			while((width % 2) != 0){
				width++;
			}
			width /= 2;
		}
		else{
			while((width % 4) != 0){
				width++;
			}
			width /= 4;
		}
		
		AGIDL_WriteLong(file,tim->img_header.img_size);
		AGIDL_WriteShort(file,tim->img_header.img_mem_add_x);
		AGIDL_WriteShort(file,tim->img_header.img_mem_add_y);
		AGIDL_WriteShort(file,width);
		AGIDL_WriteShort(file,height);
	}
	else{
		tim->header.magic = TIM_MAGIC;
		tim->header.version = TIM_16BPP;
		tim->img_header.img_size = (2*AGIDL_TIMGetWidth(tim)*AGIDL_TIMGetHeight(tim)) + 12;
		tim->img_header.img_mem_add_x = 0;
		tim->img_header.img_mem_add_y = 0;
		
		AGIDL_WriteLong(file,tim->header.magic);
		AGIDL_WriteLong(file,tim->header.version);
		AGIDL_WriteLong(file,tim->img_header.img_size);
		AGIDL_WriteShort(file,tim->img_header.img_mem_add_x);
		AGIDL_WriteShort(file,tim->img_header.img_mem_add_y);
		AGIDL_WriteShort(file,tim->img_header.width);
		AGIDL_WriteShort(file,tim->img_header.height);
	}
}

void AGIDL_TIMEncodeIMG(AGIDL_TIM* tim, FILE* file){
	if(tim->icp == TRUE){
		if(tim->force4bpp != TRUE){
			u16 w = AGIDL_TIMGetWidth(tim);
			
			while(w % 2 != 0){
				w++;
			}
			
			float scale = (float)w/AGIDL_TIMGetWidth(tim);
			
			int x,y;
			for(y = 0; y < AGIDL_TIMGetHeight(tim); y++){
				for(x = 0; x < w; x++){
					u16 x2 = (scale*x);
					COLOR16 clr = AGIDL_TIMGetClr16(tim,x2,y);
					u8 index = AGIDL_FindNearestColor(tim->palette,clr,AGIDL_TIMGetClrFmt(tim));
					AGIDL_WriteByte(file,index);
				}
			}
		}
		else{
			AGIDL_ICP fourbpp;
			fourbpp.fmt = AGIDL_TIMGetClrFmt(tim);
			AGIDL_InitICP(&fourbpp,AGIDL_ICP_16b_16);
			
			int i;
			for(i = 0; i < 16; i++){
				fourbpp.icp.palette_16b_16[i] = tim->palette.icp.palette_16b_256[i];
			}
			
			u16 w = AGIDL_TIMGetWidth(tim);
			
			while(w % 4 != 0){
				w++;
			}
			
			float scale = (float)w/AGIDL_TIMGetWidth(tim);

			int x,y;
			for(y = 0; y < AGIDL_TIMGetHeight(tim); y++){
				for(x = 0; x < w; x += 4){
					
					u16 x2 = (scale*x);
					
					COLOR16 clr1 = AGIDL_TIMGetClr16(tim,x2,y);
					COLOR16 clr2 = AGIDL_TIMGetClr16(tim,x2+1,y);
					COLOR16 clr3 = AGIDL_TIMGetClr16(tim,x2+2,y);
					COLOR16 clr4 = AGIDL_TIMGetClr16(tim,x2+3,y);
					
					u8 index1 = AGIDL_FindNearestColor(fourbpp,clr1,AGIDL_TIMGetClrFmt(tim));
					u8 index2 = AGIDL_FindNearestColor(fourbpp,clr2,AGIDL_TIMGetClrFmt(tim));
					u8 index3 = AGIDL_FindNearestColor(fourbpp,clr3,AGIDL_TIMGetClrFmt(tim));
					u8 index4 = AGIDL_FindNearestColor(fourbpp,clr4,AGIDL_TIMGetClrFmt(tim));
					
					u8 byte1 = index1 << 4 | index2;
					u8 byte2 = index3 << 4 | index4;
					
					AGIDL_WriteByte(file,byte1);
					AGIDL_WriteByte(file,byte2);
				}
			}
		}
	}
	else{
		AGIDL_WriteBufClr16(file,tim->pixels.pix16,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim));
	}
}

AGIDL_TIM * AGIDL_LoadTIM(char *filename){
	FILE *file = fopen(filename,"rb");
	
	if(file == NULL){
		printf("%s - %s\n",AGIDL_Error2Str(FILE_NOT_LOCATED_IMG_ERROR),filename);
		return NULL;
	}
	
	AGIDL_TIM *tim = (AGIDL_TIM*)malloc(sizeof(AGIDL_TIM));
	tim->filename = (char*)malloc(strlen(filename)+1);
	AGIDL_TIMSetICPEncoding(tim,ICP_ENCODE_HISTOGRAM);
	AGIDL_TIMInitAttributes(tim,0,0,0,0);
	
	if(tim == NULL || tim->filename == NULL){
		printf("%s\n",AGIDL_Error2Str(MEMORY_IMG_ERROR));
		return NULL;
	}
	
	AGIDL_FilenameCpy(tim->filename,filename);
	
	int error1 = AGIDL_TIMDecodeHeader(tim,file);
	int error2 = AGIDL_TIMDecodeIMG(tim,file);
	
	if(error1 != NO_IMG_ERROR){
		printf("%s\n",AGIDL_Error2Str(error1));
	}
	
	if(error2 != NO_IMG_ERROR){
		printf("%s\n",AGIDL_Error2Str(error2));
	}
	
	fclose(file);
	
	return tim;
}

void AGIDL_ExportTIM(AGIDL_TIM *tim){
	FILE* file = fopen(tim->filename,"wb");
	
	if(file == NULL){
		printf("Could not create TIM image file - %s\n",tim->filename);
	}
	
	if(tim->icp == TRUE){
		if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 32){
			COLOR* buf = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),AGIDL_TIMGetClrFmt(tim));
			AGIDL_ClrMemcpy(buf,tim->pixels.pix32,AGIDL_TIMGetSize(tim));
			AGIDL_CLR_FMT src = AGIDL_TIMGetClrFmt(tim);
			AGIDL_ColorConvertTIM(tim,AGIDL_BGR_555);
			AGIDL_TIMSetClrFmt(tim,AGIDL_BGR_555);
			AGIDL_TIMEncodeICP(tim);
			AGIDL_TIMEncodeHeader(tim,file);
			AGIDL_TIMEncodeIMG(tim,file);
			AGIDL_ColorConvertTIM(tim,src);
			AGIDL_ClrMemcpy(tim->pixels.pix32,buf,AGIDL_TIMGetSize(tim));
			AGIDL_TIMSetClrFmt(tim,src);
			free(buf);
		}
		else{
			switch(tim->fmt){
				case AGIDL_RGB_888:{
					AGIDL_TIMRGB2BGR(tim);
					AGIDL_TIMConvert24BPPTO16BPP(tim);
					AGIDL_TIMEncodeICP(tim);
					AGIDL_TIMEncodeHeader(tim,file);	
					AGIDL_TIMEncodeIMG(tim,file);
					AGIDL_TIMConvert16BPPTO24BPP(tim);
					AGIDL_TIMBGR2RGB(tim);
					
				}break;
				case AGIDL_BGR_888:{
					AGIDL_TIMConvert24BPPTO16BPP(tim);
					AGIDL_TIMEncodeICP(tim);
					AGIDL_TIMEncodeHeader(tim,file);
					AGIDL_TIMEncodeIMG(tim,file);
					AGIDL_TIMConvert16BPPTO24BPP(tim);
					
				}break;
				case AGIDL_RGB_555:{		
					AGIDL_TIMRGB2BGR(tim);
					AGIDL_TIMEncodeICP(tim);
					AGIDL_TIMEncodeHeader(tim,file);
					AGIDL_TIMEncodeIMG(tim,file);
					AGIDL_TIMBGR2RGB(tim);
				}break;
				case AGIDL_BGR_555:{
					AGIDL_TIMEncodeICP(tim);		
					AGIDL_TIMEncodeHeader(tim,file);
					AGIDL_TIMEncodeIMG(tim,file);
				}break;
				case AGIDL_RGB_565:{					
					AGIDL_TIMConvert565TO555(tim);
					AGIDL_TIMRGB2BGR(tim);
					AGIDL_TIMEncodeICP(tim);
					AGIDL_TIMEncodeHeader(tim,file);
					AGIDL_TIMEncodeIMG(tim,file);
					AGIDL_TIMBGR2RGB(tim);
					AGIDL_TIMConvert555TO565(tim);
				}break;
				case AGIDL_BGR_565:{
					AGIDL_TIMConvert565TO555(tim);
					AGIDL_TIMEncodeICP(tim);
					AGIDL_TIMEncodeHeader(tim,file);
					AGIDL_TIMEncodeIMG(tim,file);
					AGIDL_TIMConvert555TO565(tim);
				}break;
			}
		}
	}else{
		if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 32){
			COLOR* buf = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),AGIDL_TIMGetClrFmt(tim));
			AGIDL_ClrMemcpy(buf,tim->pixels.pix32,AGIDL_TIMGetSize(tim));
			AGIDL_CLR_FMT src = AGIDL_TIMGetClrFmt(tim);
			AGIDL_ColorConvertTIM(tim,AGIDL_BGR_555);
			AGIDL_TIMSetClrFmt(tim,AGIDL_BGR_555);
			AGIDL_TIMEncodeHeader(tim,file);
			AGIDL_TIMEncodeIMG(tim,file);
			AGIDL_ColorConvertTIM(tim,src);
			AGIDL_ClrMemcpy(tim->pixels.pix32,buf,AGIDL_TIMGetSize(tim));
			AGIDL_TIMSetClrFmt(tim,src);
			free(buf);
		}
		else{
			switch(tim->fmt){
				case AGIDL_RGB_888:{
					AGIDL_TIMEncodeHeader(tim,file);
					AGIDL_TIMConvert24BPPTO16BPP(tim);
					AGIDL_TIMRGB2BGR(tim);
					AGIDL_TIMEncodeIMG(tim,file);
					AGIDL_TIMConvert16BPPTO24BPP(tim);
					AGIDL_TIMBGR2RGB(tim);
				}break;
				case AGIDL_BGR_888:{
					AGIDL_TIMEncodeHeader(tim,file);
					AGIDL_TIMConvert24BPPTO16BPP(tim);
					AGIDL_TIMEncodeIMG(tim,file);
					AGIDL_TIMConvert16BPPTO24BPP(tim);				
				}break;
				case AGIDL_RGB_555:{
					AGIDL_TIMEncodeHeader(tim,file);
					AGIDL_TIMRGB2BGR(tim);
					AGIDL_TIMEncodeIMG(tim,file);
					AGIDL_TIMBGR2RGB(tim);
				}break;
				case AGIDL_BGR_555:{
					AGIDL_TIMEncodeHeader(tim,file);
					AGIDL_TIMEncodeIMG(tim,file);
				}break;
				case AGIDL_RGB_565:{
					AGIDL_TIMEncodeHeader(tim,file);
					AGIDL_TIMConvert565TO555(tim);
					AGIDL_TIMRGB2BGR(tim);
					AGIDL_TIMEncodeIMG(tim,file);
					AGIDL_TIMBGR2RGB(tim);
					AGIDL_TIMConvert555TO565(tim);
				}break;
				case AGIDL_BGR_565:{
					AGIDL_TIMEncodeHeader(tim,file);
					AGIDL_TIMConvert565TO555(tim);
					AGIDL_TIMEncodeIMG(tim,file);
					AGIDL_TIMConvert555TO565(tim);
				}break;
			}
		}
	}
	
	fclose(file);
}