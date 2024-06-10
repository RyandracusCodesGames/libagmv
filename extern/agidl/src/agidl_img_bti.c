/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_bti.c
*   Date: 11/22/2023
*   Version: 0.1b
*   Updated: 2/21/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_img_bti.h>

#include <stdlib.h>
#include <string.h>

#include <agidl_cc_core.h>
#include <agidl_file_utils.h>
#include <agidl_img_error.h>
#include <agidl_mmu_utils.h>

void AGIDL_SetBTIFilename(AGIDL_BTI* bti, const char* filename){
	bti->filename = (char*)realloc(bti->filename,strlen(filename));
	AGIDL_FilenameCpy(bti->filename,filename);
}

void AGIDL_BTISetWidth(AGIDL_BTI* bti, const u16 width){
	bti->header.width = width;
}

void AGIDL_BTISetHeight(AGIDL_BTI* bti, const u16 height){
	bti->header.height = height;
}

void AGIDL_BTISetClrFmt(AGIDL_BTI* bti, const AGIDL_CLR_FMT fmt){
	bti->fmt = fmt;
}

void AGIDL_BTISetMaxDiff(AGIDL_BTI* bti, const int max_diff){
	bti->max_diff = max_diff;
}

void AGIDL_BTISetICPMode(AGIDL_BTI* bti, const int mode){
	bti->icp = mode;
}

void AGIDL_BTISetICPEncoding(AGIDL_BTI* bti, const AGIDL_ICP_ENCODE encode){
	bti->encode = encode;
}

void AGIDL_BTISetCompression(AGIDL_BTI* bti, const int compress){
	bti->compression = compress;
}

void AGIDL_BTISetClr(const AGIDL_BTI *bti, const int x, const int y, const COLOR clr){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) != 16){
		AGIDL_SetClr(bti->pixels.pix32,clr,x,y,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti));
	}
	else{
		AGIDL_SetClr16(bti->pixels.pix16,AGIDL_CLR_TO_CLR16(clr,AGIDL_BGR_888,AGIDL_BTIGetClrFmt(bti)),x,y,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti));
	}
}

void AGIDL_BTISetClr16(const AGIDL_BTI *bti, const int x, const int y, const COLOR16 clr){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti) == 16)){
		AGIDL_SetClr16(bti->pixels.pix16,clr,x,y,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti));
	}
	else{
		AGIDL_SetClr(bti->pixels.pix32,AGIDL_CLR16_TO_CLR(clr,AGIDL_RGB_555,AGIDL_BTIGetClrFmt(bti)),x,y,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti));
	}
}

void AGIDL_BTISetRGB(const AGIDL_BTI* bti, const int x, const int y, const u8 r, const u8 g, const u8 b){
	switch(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti))){
		case 24:{
			AGIDL_BTISetClr(bti,x,y,AGIDL_RGB(r,g,b,AGIDL_BTIGetClrFmt(bti)));
		}break;
		case 16:{
			AGIDL_BTISetClr16(bti,x,y,AGIDL_RGB16(r,g,b,AGIDL_BTIGetClrFmt(bti)));
		}break;
		case 32:{
			AGIDL_BTISetRGBA(bti,x,y,r,g,b,0xff);
		}break;	
	}
}

void AGIDL_BTISetRGBA(const AGIDL_BTI* bti, const int x, const int y, const u8 r, const u8 g, const u8 b, const u8 a){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 32){
		AGIDL_BTISetClr(bti,x,y,AGIDL_RGBA(r,g,b,a,AGIDL_BTIGetClrFmt(bti)));
	}
}

void AGIDL_ClearBTI(const AGIDL_BTI *bti, const COLOR clr){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) != 16){
		AGIDL_ClrMemset(bti->pixels.pix32,clr,AGIDL_BTIGetSize(bti));
	}
	else{
		AGIDL_ClrMemset16(bti->pixels.pix16,clr,AGIDL_BTIGetSize(bti));
	}
}

void AGIDL_ClearBTI16(const AGIDL_BTI *bti, const COLOR16 clr){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		AGIDL_ClrMemset16(bti->pixels.pix16,clr,AGIDL_BTIGetSize(bti));
	}
	else{
		AGIDL_ClrMemset(bti->pixels.pix32,AGIDL_CLR16_TO_CLR(clr,AGIDL_RGB_555,AGIDL_BTIGetClrFmt(bti)),AGIDL_BTIGetSize(bti));
	}
}

void AGIDL_ClearColorBTI(const AGIDL_BTI* bti, const float r, const float g, const float b){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		AGIDL_ClearColorBuffer(bti->pixels.pix16,r,g,b,AGIDL_BTIGetClrFmt(bti),AGIDL_BTIGetSize(bti));
	}
	else{
		AGIDL_ClearColorBuffer(bti->pixels.pix32,r,g,b,AGIDL_BTIGetClrFmt(bti),AGIDL_BTIGetSize(bti));
	}
}

void AGIDL_FlushBTI(const AGIDL_BTI* bti){
	AGIDL_ClearBTI(bti,0);
}

int AGIDL_BTIGetWidth(const AGIDL_BTI* bti){
	return bti->header.width;
}

int AGIDL_BTIGetHeight(const AGIDL_BTI* bti){
	return bti->header.height;
}

u32 AGIDL_BTIGetSize(const AGIDL_BTI* bti){
	return AGIDL_BTIGetWidth(bti) * AGIDL_BTIGetHeight(bti);
}

AGIDL_CLR_FMT AGIDL_BTIGetClrFmt(const AGIDL_BTI* bti){
	return bti->fmt;
}

int AGIDL_BTIGetMaxDiff(const AGIDL_BTI* bti){
	return bti->max_diff;
}

COLOR AGIDL_BTIGetClr(const AGIDL_BTI* bti, const int x, const int y){
	return AGIDL_GetClr(bti->pixels.pix32,x,y,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti));
}
COLOR16 AGIDL_BTIGetClr16(const AGIDL_BTI* bti, const int x, const int y){
	return AGIDL_GetClr16(bti->pixels.pix16,x,y,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti));
}

void AGIDL_BTIRGB2BGR(AGIDL_BTI* bti){
	if(bti->fmt == AGIDL_RGB_888){
		AGIDL_RGB2BGR(bti->pixels.pix32,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),&bti->fmt);
	}
	if(bti->fmt == AGIDL_RGB_555 || bti->fmt == AGIDL_RGB_565){
		AGIDL_RGB2BGR16(bti->pixels.pix16,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),&bti->fmt);
	}
}

void AGIDL_BTIBGR2RGB(AGIDL_BTI* bti){
	if(bti->fmt == AGIDL_BGR_888){
		AGIDL_BGR2RGB(bti->pixels.pix32,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),&bti->fmt);
	}
	if(bti->fmt == AGIDL_BGR_555 || bti->fmt == AGIDL_BGR_565){
		AGIDL_BGR2RGB16(bti->pixels.pix16,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),&bti->fmt);
	}
}

void AGIDL_BTIConvert24BPPTO16BPP(AGIDL_BTI* bti){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 24){
		bti->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*AGIDL_BTIGetSize(bti));
		AGIDL_24BPPTO16BPP(bti->pixels.pix32,bti->pixels.pix16,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),&bti->fmt);
		free(bti->pixels.pix32);
	}
}

void AGIDL_BTIConvert16BPPTO24BPP(AGIDL_BTI* bti){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		bti->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*AGIDL_BTIGetSize(bti));
		AGIDL_24BPPTO16BPP(bti->pixels.pix32,bti->pixels.pix16,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),&bti->fmt);
		free(bti->pixels.pix32);
	}
}

void AGIDL_BTIRGBATORGB(AGIDL_BTI* bti){
	bti->fmt = AGIDL_RGB_888;
}

void AGIDL_BTI555TO565(AGIDL_BTI* bti){
	AGIDL_555TO565(bti->pixels.pix16,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),&bti->fmt);
}

void AGIDL_BTI565TO555(AGIDL_BTI* bti){
	AGIDL_565TO555(bti->pixels.pix16,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),&bti->fmt);
}

void AGIDL_ColorConvertBTI(AGIDL_BTI* bti, const AGIDL_CLR_FMT dest){
	const u8 sbits = AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)), dbits = AGIDL_GetBitCount(dest);
	if(sbits == 16 && dbits == 16){
		AGIDL_ColorConvertImgData(bti->pixels.pix16,NULL,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),AGIDL_BTIGetClrFmt(bti),dest);
		AGIDL_BTISetClrFmt(bti,dest);
	}
	else if((sbits == 24 || sbits == 32) && (dbits == 24 || dbits == 32)){
		AGIDL_ColorConvertImgData(bti->pixels.pix32,NULL,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),AGIDL_BTIGetClrFmt(bti),dest);
		AGIDL_BTISetClrFmt(bti,dest);
	}
	else if(sbits == 16 && (dbits == 24 || dbits == 32)){
		bti->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),dest);
		AGIDL_ColorConvertImgData(bti->pixels.pix16,bti->pixels.pix32,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),AGIDL_BTIGetClrFmt(bti),dest);
		AGIDL_BTISetClrFmt(bti,dest);
		free(bti->pixels.pix16);
	}
	else{
		bti->pixels.pix16 = (COLOR16*)AGIDL_AllocImgDataMMU(AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),dest);
		AGIDL_ColorConvertImgData(bti->pixels.pix32,bti->pixels.pix16,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),AGIDL_BTIGetClrFmt(bti),dest);
		AGIDL_BTISetClrFmt(bti,dest);
		free(bti->pixels.pix32);
	}
}

void AGIDL_BTISyncPix(const AGIDL_BTI *bti, const COLOR *clrs){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) != 16){
		AGIDL_ClrMemcpy(bti->pixels.pix32,clrs,AGIDL_BTIGetSize(bti));
	}
}

void AGIDL_BTISyncPix16(const AGIDL_BTI *bti, const COLOR16 *clrs){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		AGIDL_ClrMemcpy16(bti->pixels.pix16,clrs,AGIDL_BTIGetSize(bti));
	}
}

void AGIDL_BTICopyPix(const AGIDL_BTI* bti, const COLOR* clrs, const u32 count){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) != 16){
		AGIDL_ClrMemcpy(bti->pixels.pix32,clrs,count);
	}
}

void AGIDL_BTICopyPix16(const AGIDL_BTI* bti, const COLOR16* clrs, const u32 count){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		AGIDL_ClrMemcpy16(bti->pixels.pix16,clrs,count);
	}
}

AGIDL_BTI* AGIDL_CreateBTI(const char* filename, const int width, const int height, const AGIDL_CLR_FMT fmt){
	AGIDL_BTI* bti = malloc(sizeof(AGIDL_BTI));
	bti->filename = (char*)malloc(strlen(filename)+1);
	AGIDL_FilenameCpy(bti->filename,filename);
	AGIDL_BTISetWidth(bti,width);
	AGIDL_BTISetHeight(bti,height);
	AGIDL_BTISetClrFmt(bti,fmt);
	AGIDL_BTISetICPMode(bti,0);
	AGIDL_BTISetICPEncoding(bti,ICP_ENCODE_THRESHOLD);
	AGIDL_BTISetMaxDiff(bti,7);
	AGIDL_BTISetCompression(bti,0);
	
	if(AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32){
		bti->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*AGIDL_BTIGetSize(bti));
	}
	if(AGIDL_GetBitCount(fmt) == 16){
		bti->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*AGIDL_BTIGetSize(bti));
	}
	
	return bti;
}

AGIDL_BTI* AGIDL_BTICpyImg(const AGIDL_BTI* bti){
	AGIDL_BTI* cpybti = AGIDL_CreateBTI("cpy.bti",AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),
	AGIDL_BTIGetClrFmt(bti));
	AGIDL_BTISetICPMode(cpybti,bti->icp);
	AGIDL_BTISetMaxDiff(cpybti,AGIDL_BTIGetMaxDiff(bti));
	AGIDL_BTISetCompression(cpybti,bti->compression);
	AGIDL_BTISetICPEncoding(cpybti,ICP_ENCODE_THRESHOLD);
	
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 24 ||  AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 32){
		AGIDL_BTISyncPix(cpybti,bti->pixels.pix32);
	}
	
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		AGIDL_BTISyncPix16(cpybti,bti->pixels.pix16);
	}
	
	return cpybti;
}

void AGIDL_FreeBTI(AGIDL_BTI* bti){
	free(bti->filename);
	
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		free(bti->pixels.pix16);
	}
	else{
		free(bti->pixels.pix32);
	}
	
	free(bti);
}

BTI_CLR_FMT AGIDL_GetBTIClrFmt(const u8 type){
	switch(type){
		case 0x00:{
			return BTI_I4;
		}
		case 0x01:{
			return BTI_I8;
		}
		case 0x02:{
			return BTI_IA4;
		}
		case 0x03:{
			return BTI_IA8;
		}
		case 0x04:{
			return BTI_RGB_565;
		}
		case 0x05:{
			return BTI_RGB5A3;
		}
		case 0x06:{
			return BTI_RGBA32;
		}
		case 0x08:{
			return BTI_CI4;
		}
		case 0x09:{
			return BTI_CI8;
		}
		case 0x0A:{
			return BTI_C14X2;
		}
		case 0x0E:{
			return BTI_CMPR;
		}
	}
	return 44;
}

BTI_ICP_FMT AGIDL_BTIGetICPFmt(const u8 type){
	switch(type){
		case 0x00:{
			return BTI_ICP_IA8;
		}
		case 0x01:{
			return BTI_ICP_565;
		}
		case 0x02:{
			return BTI_ICP_5A3;
		}
		default: return 44;
	}
}

int AGIDL_BTIDecodeHeader(AGIDL_BTI* bti, FILE* file){
	
	AGIDL_InitBigEndArch();

	const u8 fmt1 = AGIDL_ReadByte(file);
	bti->header.alpha = AGIDL_ReadByte(file);
	bti->header.width = AGIDL_ReadShort(file);
	bti->header.height = AGIDL_ReadShort(file);
	bti->header.wraps = AGIDL_ReadByte(file);
	bti->header.wrapt = AGIDL_ReadByte(file);
	const u16 fmt2 = AGIDL_ReadShort(file);
	bti->header.num_of_icps = AGIDL_ReadShort(file);
	bti->header.offset_icp = AGIDL_ReadLong(file);
	fseek(file,4,SEEK_CUR);
	bti->header.filter_type = AGIDL_ReadByte(file);
	bti->header.filter_type2 = AGIDL_ReadByte(file);
	bti->header.blank2 = AGIDL_ReadShort(file);
	bti->header.mipmapcount = AGIDL_ReadByte(file);
	bti->header.blank3 = AGIDL_ReadLong(file);
	bti->header.blank4 = AGIDL_ReadShort(file);
	bti->header.offset = AGIDL_ReadByte(file);
	
	bti->header.fmt = AGIDL_GetBTIClrFmt(fmt1);
	bti->header.icp_fmt = AGIDL_BTIGetICPFmt(fmt2);
	
	AGIDL_DisableBigEndArch();
	
	if(bti->header.fmt == 44 || bti->header.icp_fmt == 44){
		return INVALID_HEADER_FORMATTING_ERROR;
	}
	return NO_IMG_ERROR;
}

void AGIDL_BTIDecodeIMG(AGIDL_BTI* bti, FILE* file){
	if(bti->header.num_of_icps == 0){
		switch(bti->header.fmt){
			case BTI_RGB5A3:{
				AGIDL_BTISetClrFmt(bti,AGIDL_RGBA_8888);
				bti->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),AGIDL_BTIGetClrFmt(bti));

				int x,y;
				for(y = 0; y < AGIDL_BTIGetHeight(bti); y += 4){
					for(x = 0; x < AGIDL_BTIGetWidth(bti); x += 4){
					
						u8 i,j;
						for(j = 0; j < 4; j++){
							for(i = 0; i < 4; i++){
								u8 byte1 = AGIDL_ReadByte(file);
								u8 byte2 = AGIDL_ReadByte(file);

								if(((byte1 >> 7) & 0x1) == 0){
									u8 a = ((byte1 >> 4) & 0x0C) << 4;
									
									COLOR clr = AGIDL_RGBA(155,155,155,a,AGIDL_RGBA_8888);							
									AGIDL_BTISetClr(bti,x+i,y+j,clr);
								}
								else{
									COLOR16 clr16 = byte1 << 8 | byte2;
									COLOR clr = AGIDL_CLR16_TO_CLR(clr16,AGIDL_RGB_555,AGIDL_RGBA_8888);
									clr = AGIDL_SetA(clr,0xff,bti->fmt);
									AGIDL_BTISetClr(bti,x+i,y+j,clr);
								}
							}
						}
					}
				}
			}break;
			case BTI_RGB_565:{
				AGIDL_InitBigEndArch();
				
				AGIDL_BTISetClrFmt(bti,AGIDL_RGB_565);
				bti->pixels.pix16 = (COLOR16*)AGIDL_AllocImgDataMMU(AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),AGIDL_BTIGetClrFmt(bti));

				int x,y;
				for(y = 0; y < AGIDL_BTIGetHeight(bti); y += 4){
					for(x = 0; x < AGIDL_BTIGetWidth(bti); x += 4){
					
						u8 i,j;
						for(j = 0; j < 4; j++){
							for(i = 0; i < 4; i++){
								COLOR16 clr = AGIDL_ReadShort(file);
								AGIDL_BTISetClr16(bti,x+i,y+j,clr);
							}
						}
					}
				}
				
				AGIDL_DisableBigEndArch();
			}break;
			case BTI_RGBA32:{
				AGIDL_BTISetClrFmt(bti,AGIDL_RGBA_8888);
				bti->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),AGIDL_BTIGetClrFmt(bti));
				
				int x,y;
				for(y = 0; y < AGIDL_BTIGetHeight(bti); y += 4){
					for(x = 0; x < AGIDL_BTIGetWidth(bti); x += 4){
						u8 byte1,byte2,byte3,byte4,byte5,byte6,byte7,byte8,
						byte9,byte10,byte11,byte12,byte13,byte14,byte15,byte16;
						
						byte1 = AGIDL_ReadByte(file); byte2 = AGIDL_ReadByte(file); byte3 = AGIDL_ReadByte(file);
						byte4 = AGIDL_ReadByte(file); byte5 = AGIDL_ReadByte(file); byte6 = AGIDL_ReadByte(file);
						byte7 = AGIDL_ReadByte(file); byte8 = AGIDL_ReadByte(file); byte9 = AGIDL_ReadByte(file);
						byte10 = AGIDL_ReadByte(file); byte11 = AGIDL_ReadByte(file); byte12 = AGIDL_ReadByte(file);
						byte13 = AGIDL_ReadByte(file); byte14 = AGIDL_ReadByte(file); byte15 = AGIDL_ReadByte(file); byte16 = AGIDL_ReadByte(file);
						
						u8 a1 = byte1, a2 = byte3, a3 = byte5, a4 = byte7;
						u8 r1 = byte2, r2 = byte4, r3 = byte6, r4 = byte8;
						u8 aa1 = byte9, aa2 = byte11, aa3 = byte13, aa4 = byte15;
						u8 rr1 = byte10, rr2 = byte12, rr3 = byte14, rr4 = byte16;
											
						byte1 = AGIDL_ReadByte(file); byte2 = AGIDL_ReadByte(file); byte3 = AGIDL_ReadByte(file);
						byte4 = AGIDL_ReadByte(file); byte5 = AGIDL_ReadByte(file); byte6 = AGIDL_ReadByte(file);
						byte7 = AGIDL_ReadByte(file); byte8 = AGIDL_ReadByte(file); byte9 = AGIDL_ReadByte(file);
						byte10 = AGIDL_ReadByte(file); byte11 = AGIDL_ReadByte(file); byte12 = AGIDL_ReadByte(file);
						byte13 = AGIDL_ReadByte(file); byte14 = AGIDL_ReadByte(file); byte15 = AGIDL_ReadByte(file); byte16 = AGIDL_ReadByte(file);
						
						u8 aaa1 = byte1, aaa2 = byte3, aaa3 = byte5, aaa4 = byte7;
						u8 rrr1 = byte2, rrr2 = byte4, rrr3 = byte6, rrr4 = byte8;
						u8 aaaa1 = byte9, aaaa2 = byte11, aaaa3 = byte13, aaaa4 = byte15;
						u8 rrrr1 = byte10, rrrr2 = byte12, rrrr3 = byte14, rrrr4 = byte16;
						
						byte1 = AGIDL_ReadByte(file); byte2 = AGIDL_ReadByte(file); byte3 = AGIDL_ReadByte(file);
						byte4 = AGIDL_ReadByte(file); byte5 = AGIDL_ReadByte(file); byte6 = AGIDL_ReadByte(file);
						byte7 = AGIDL_ReadByte(file); byte8 = AGIDL_ReadByte(file); byte9 = AGIDL_ReadByte(file);
						byte10 = AGIDL_ReadByte(file); byte11 = AGIDL_ReadByte(file); byte12 = AGIDL_ReadByte(file);
						byte13 = AGIDL_ReadByte(file); byte14 = AGIDL_ReadByte(file); byte15 = AGIDL_ReadByte(file); byte16 = AGIDL_ReadByte(file);
						
						u8 g1 = byte1, g2 = byte3, g3 = byte5, g4 = byte7;
						u8 b1 = byte2, b2 = byte4, b3 = byte6, b4 = byte8;
						u8 gg1 = byte9, gg2 = byte11, gg3 = byte13, gg4 = byte15;
						u8 bb1 = byte10, bb2 = byte12, bb3 = byte14, bb4 = byte16;
						
						byte1 = AGIDL_ReadByte(file); byte2 = AGIDL_ReadByte(file); byte3 = AGIDL_ReadByte(file);
						byte4 = AGIDL_ReadByte(file); byte5 = AGIDL_ReadByte(file); byte6 = AGIDL_ReadByte(file);
						byte7 = AGIDL_ReadByte(file); byte8 = AGIDL_ReadByte(file); byte9 = AGIDL_ReadByte(file);
						byte10 = AGIDL_ReadByte(file); byte11 = AGIDL_ReadByte(file); byte12 = AGIDL_ReadByte(file);
						byte13 = AGIDL_ReadByte(file); byte14 = AGIDL_ReadByte(file); byte15 = AGIDL_ReadByte(file); byte16 = AGIDL_ReadByte(file);
						
						u8 ggg1 = byte1, ggg2 = byte3, ggg3 = byte5, ggg4 = byte7;
						u8 bbb1 = byte2, bbb2 = byte4, bbb3 = byte6, bbb4 = byte8;
						u8 gggg1 = byte9, gggg2 = byte11, gggg3 = byte13, gggg4 = byte15;
						u8 bbbb1 = byte10, bbbb2 = byte12, bbbb3 = byte14, bbbb4 = byte16;
						
						COLOR clr1 = AGIDL_RGBA(r1,g1,b1,a2,AGIDL_BTIGetClrFmt(bti));
						COLOR clr2 = AGIDL_RGBA(r2,g2,b2,a2,AGIDL_BTIGetClrFmt(bti));
						COLOR clr3 = AGIDL_RGBA(r3,g3,b3,a3,AGIDL_BTIGetClrFmt(bti));
						COLOR clr4 = AGIDL_RGBA(r4,g4,b4,a4,AGIDL_BTIGetClrFmt(bti));
						
						COLOR clr5 = AGIDL_RGBA(rr1,gg1,bb1,aa2,AGIDL_BTIGetClrFmt(bti));
						COLOR clr6 = AGIDL_RGBA(rr2,gg2,bb2,aa2,AGIDL_BTIGetClrFmt(bti));
						COLOR clr7 = AGIDL_RGBA(rr3,gg3,bb3,aa3,AGIDL_BTIGetClrFmt(bti));
						COLOR clr8 = AGIDL_RGBA(rr4,gg4,bb4,aa4,AGIDL_BTIGetClrFmt(bti));
						
						COLOR clr9 = AGIDL_RGBA(rrr1,ggg1,bbb1,aaa2,AGIDL_BTIGetClrFmt(bti));
						COLOR clr10 = AGIDL_RGBA(rrr2,ggg2,bbb2,aaa2,AGIDL_BTIGetClrFmt(bti));
						COLOR clr11 = AGIDL_RGBA(rrr3,ggg3,bbb3,aaa3,AGIDL_BTIGetClrFmt(bti));
						COLOR clr12 = AGIDL_RGBA(rrr4,ggg4,bbb4,aaa4,AGIDL_BTIGetClrFmt(bti));
						
						COLOR clr13 = AGIDL_RGBA(rrrr1,gggg1,bbbb1,aaaa2,AGIDL_BTIGetClrFmt(bti));
						COLOR clr14 = AGIDL_RGBA(rrrr2,gggg2,bbbb2,aaaa2,AGIDL_BTIGetClrFmt(bti));
						COLOR clr15 = AGIDL_RGBA(rrrr3,gggg3,bbbb3,aaaa3,AGIDL_BTIGetClrFmt(bti));
						COLOR clr16 = AGIDL_RGBA(rrrr4,gggg4,bbbb4,aaaa4,AGIDL_BTIGetClrFmt(bti));
						
						AGIDL_BTISetClr(bti,0+x,0+y,clr1);AGIDL_BTISetClr(bti,1+x,0+y,clr2);
						AGIDL_BTISetClr(bti,2+x,0+y,clr3);AGIDL_BTISetClr(bti,3+x,0+y,clr4);
						
						AGIDL_BTISetClr(bti,0+x,1+y,clr5);AGIDL_BTISetClr(bti,1+x,1+y,clr6);
						AGIDL_BTISetClr(bti,2+x,1+y,clr7);AGIDL_BTISetClr(bti,3+x,1+y,clr8);
						
						AGIDL_BTISetClr(bti,0+x,2+y,clr9);AGIDL_BTISetClr(bti,1+x,2+y,clr10);
						AGIDL_BTISetClr(bti,2+x,2+y,clr11);AGIDL_BTISetClr(bti,3+x,2+y,clr12);
						
						AGIDL_BTISetClr(bti,0+x,3+y,clr13);AGIDL_BTISetClr(bti,1+x,3+y,clr14);
						AGIDL_BTISetClr(bti,2+x,3+y,clr15);AGIDL_BTISetClr(bti,3+x,3+y,clr16);
					}
				}
			}break;
			case BTI_I4:{
				AGIDL_BTISetClrFmt(bti,AGIDL_RGB_888);
				bti->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),AGIDL_BTIGetClrFmt(bti));
				
				int x,y;
				for(y = 0; y < AGIDL_BTIGetHeight(bti); y += 8){
					for(x = 0; x < AGIDL_BTIGetWidth(bti); x += 8){
						u8 i,j;
						for(j = 0; j < 8; j++){
							for(i = 0; i < 8; i+=2){
								u8 byte = AGIDL_ReadByte(file);
								
								u8 rgb1 = ((byte >> 4) & 0xf) * 0x11, rgb2 = (byte & 0xf) * 0x11;
								
								AGIDL_BTISetClr(bti,x+i,y+j,AGIDL_RGB(rgb1,rgb1,rgb1,AGIDL_BTIGetClrFmt(bti)));
								AGIDL_BTISetClr(bti,(x+i)+1,y+j,AGIDL_RGB(rgb2,rgb2,rgb2,AGIDL_BTIGetClrFmt(bti)));
							}
						}
					}
				}
			}break;
			case BTI_I8:{
				AGIDL_BTISetClrFmt(bti,AGIDL_RGB_888);
				bti->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*AGIDL_BTIGetSize(bti));
				
				int x,y;
				for(y = 0; y < AGIDL_BTIGetHeight(bti); y += 4){
					for(x = 0; x < AGIDL_BTIGetWidth(bti); x += 8){
						u8 i,j;
						for(j = 0; j < 4; j++){
							for(i = 0; i < 8; i++){	
								u8 rgb = AGIDL_ReadByte(file);
								
								AGIDL_BTISetClr(bti,x+i,y+j,AGIDL_RGB(rgb,rgb,rgb,AGIDL_BTIGetClrFmt(bti)));
							}
						}
					}
				}
			}break;
			case BTI_IA4:{
				AGIDL_BTISetClrFmt(bti,AGIDL_RGBA_8888);
				bti->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),AGIDL_BTIGetClrFmt(bti));
				
				int x,y;
				for(y = 0; y < AGIDL_BTIGetHeight(bti); y += 4){
					for(x = 0; x < AGIDL_BTIGetWidth(bti); x += 8){
						u8 i,j;
						for(j = 0; j < 4; j++){
							for(i = 0; i < 8; i++){
								u8 byte = AGIDL_ReadByte(file);
								
								u8 alpha = ((byte >> 4) & 0xf) * 0x11;
								u8 rgb = (byte & 0xf) * 0x11;
								
								AGIDL_BTISetClr(bti,x+i,y+j,AGIDL_RGBA(rgb,rgb,rgb,alpha,AGIDL_BTIGetClrFmt(bti)));
							}
						}
					}
				}
			}break;
			case BTI_IA8:{
				AGIDL_BTISetClrFmt(bti,AGIDL_RGBA_8888);
				bti->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),AGIDL_BTIGetClrFmt(bti));
				
				int x,y;
				for(y = 0; y < AGIDL_BTIGetHeight(bti); y += 4){
					for(x = 0; x < AGIDL_BTIGetWidth(bti); x += 4){
						u8 i,j;
						for(j = 0; j < 4; j++){
							for(i = 0; i < 4; i++){
								u8 byte1 = AGIDL_ReadByte(file);
								u8 byte2 = AGIDL_ReadByte(file);
								
								u8 alpha = byte1;
								u8 rgb = byte2;
								
								AGIDL_BTISetClr(bti,x+i,y+j,AGIDL_RGBA(rgb,rgb,rgb,alpha,AGIDL_BTIGetClrFmt(bti)));
							}
						}
					}
				}
			}break;
		}
	}
	else{
		if(bti->header.fmt == BTI_CI8 && bti->header.icp_fmt == BTI_ICP_5A3){
			AGIDL_BTISetClrFmt(bti,AGIDL_RGBA_8888);
			bti->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),AGIDL_BTIGetClrFmt(bti));
			
			u32 offset = bti->header.offset_icp;
			
			u32 pos = ftell(file);
			fseek(file,offset,SEEK_SET);

			AGIDL_InitICP(&bti->palette,AGIDL_ICP_256);
			
			int i;
			for(i = 0; i < bti->header.num_of_icps; i++){
				u8 byte1 = AGIDL_ReadByte(file);
				u8 byte2 = AGIDL_ReadByte(file);

				if(((byte1 >> 7) & 0x1) == 0){
					u8 a = ((byte1 >> 4) & 0x0C) << 4;
					
					COLOR clr = AGIDL_RGBA(155,155,155,a,AGIDL_RGBA_8888);	

					bti->palette.icp.palette_256[i] = clr;
				}
				else{
					COLOR16 clr16 = byte1 << 8 | byte2;
					COLOR clr = AGIDL_CLR16_TO_CLR(clr16,AGIDL_RGB_555,AGIDL_RGBA_8888);
					clr = AGIDL_SetA(clr,0xff,bti->fmt);
					bti->palette.icp.palette_256[i] = clr;
				}
			}
			
			fseek(file,pos,SEEK_SET);
			
			int x,y;
			for(y = 0; y < AGIDL_BTIGetHeight(bti); y += 4){
				for(x = 0; x < AGIDL_BTIGetWidth(bti); x += 8){
					u8 j;
					for(j = 0; j < 4; j++){
						for(i = 0; i < 8; i++){
							u8 byte = AGIDL_ReadByte(file);
														
							COLOR clr = bti->palette.icp.palette_256[byte];
							
							AGIDL_BTISetClr(bti,x+i,y+j,clr);
						}
					}
				}
			}
			
		}
		else if(bti->header.fmt == BTI_CI8 && bti->header.icp_fmt == BTI_ICP_565){
			AGIDL_BTISetClrFmt(bti,AGIDL_RGB_565);
			bti->pixels.pix16 = (COLOR16*)AGIDL_AllocImgDataMMU(AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),AGIDL_BTIGetClrFmt(bti));
			
			u32 offset = bti->header.offset_icp;

			u32 pos = ftell(file);
			fseek(file,offset,SEEK_SET);

			AGIDL_InitICP(&bti->palette,AGIDL_ICP_16b_256);
			
			AGIDL_InitBigEndArch();
			
			int i;
			for(i = 0; i < bti->header.num_of_icps; i++){
				COLOR16 clr = AGIDL_ReadShort(file);
				bti->palette.icp.palette_16b_256[i] = clr;
			}
			
			AGIDL_DisableBigEndArch();
			
			fseek(file,pos,SEEK_SET);
			
			int x,y;
			for(y = 0; y < AGIDL_BTIGetHeight(bti); y += 4){
				for(x = 0; x < AGIDL_BTIGetWidth(bti); x += 8){
					u8 j;
					for(j = 0; j < 4; j++){
						for(i = 0; i < 8; i++){
							u8 byte = AGIDL_ReadByte(file);
														
							COLOR16 clr = bti->palette.icp.palette_16b_256[byte];
							
							AGIDL_BTISetClr16(bti,x+i,y+j,clr);
						}
					}
				}
			}
		}
	}
}

AGIDL_BTI* AGIDL_LoadBTI(char* filename){
	FILE* file = fopen(filename,"rb");
	
	if(file == NULL){
		printf("Could not open/locate Nintendo Gamecube BTI image - %s!\n",filename);
	}
	
	AGIDL_BTI* bti = malloc(sizeof(AGIDL_BTI));
	bti->filename = (char*)malloc(strlen(filename)+1);
	AGIDL_FilenameCpy(bti->filename,filename);
	AGIDL_BTISetICPEncoding(bti,ICP_ENCODE_THRESHOLD);
	
	if(bti->filename == NULL){
		printf("%s\n",AGIDL_Error2Str(MEMORY_IMG_ERROR));
		return NULL;
	}

	const int error = AGIDL_BTIDecodeHeader(bti,file);
	
	if(error != NO_IMG_ERROR){
		printf("%s - %s\n",AGIDL_Error2Str(error),filename);
		return NULL;
	}
	
	AGIDL_BTIDecodeIMG(bti,file);
	
	fclose(file);
	
	return bti;
}

void AGIDL_BTIEncodeHeader(const AGIDL_BTI* bti, FILE* file){
	if(bti->icp != 1){
		if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 24 || AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){	
			AGIDL_InitBigEndArch();
			
			AGIDL_WriteByte(file,BTI_RGB_565);
			AGIDL_WriteByte(file,0);
			AGIDL_WriteShort(file,AGIDL_BTIGetWidth(bti));
			AGIDL_WriteShort(file,AGIDL_BTIGetHeight(bti));
			AGIDL_WriteByte(file,0);
			AGIDL_WriteByte(file,0);
			AGIDL_WriteShort(file,0);
			AGIDL_WriteShort(file,0);
			AGIDL_WriteLong(file,0);
			AGIDL_WriteLong(file,0);
			AGIDL_WriteByte(file,0);
			AGIDL_WriteByte(file,0);
			AGIDL_WriteShort(file,0);
			AGIDL_WriteByte(file,0);
			AGIDL_WriteShort(file,0);
			AGIDL_WriteLong(file,0);
			AGIDL_WriteByte(file,0x20);
			
			AGIDL_DisableBigEndArch();
		}
		else{
			AGIDL_InitBigEndArch();
			
			AGIDL_WriteByte(file,BTI_RGBA32);
			AGIDL_WriteByte(file,1);
			AGIDL_WriteShort(file,AGIDL_BTIGetWidth(bti));
			AGIDL_WriteShort(file,AGIDL_BTIGetHeight(bti));
			AGIDL_WriteByte(file,0);
			AGIDL_WriteByte(file,0);
			AGIDL_WriteShort(file,0);
			AGIDL_WriteShort(file,0);
			AGIDL_WriteLong(file,0);
			AGIDL_WriteLong(file,0);
			AGIDL_WriteByte(file,0);
			AGIDL_WriteByte(file,0);
			AGIDL_WriteShort(file,0);
			AGIDL_WriteByte(file,0);
			AGIDL_WriteShort(file,0);
			AGIDL_WriteLong(file,0);
			AGIDL_WriteByte(file,0x20);
			
			AGIDL_DisableBigEndArch();
		}
	}
	else{
			AGIDL_InitBigEndArch();
			
			AGIDL_WriteByte(file,BTI_CI8);
			AGIDL_WriteByte(file,0);
			AGIDL_WriteShort(file,AGIDL_BTIGetWidth(bti));
			AGIDL_WriteShort(file,AGIDL_BTIGetHeight(bti));
			AGIDL_WriteByte(file,0);
			AGIDL_WriteByte(file,0);
			AGIDL_WriteShort(file,BTI_ICP_565);
			AGIDL_WriteShort(file,256);
			AGIDL_WriteLong(file,0);
			AGIDL_WriteLong(file,0);
			AGIDL_WriteByte(file,0);
			AGIDL_WriteByte(file,0);
			AGIDL_WriteShort(file,0);
			AGIDL_WriteByte(file,0);
			AGIDL_WriteShort(file,0);
			AGIDL_WriteLong(file,0);
			AGIDL_WriteByte(file,0x20);
			
			AGIDL_DisableBigEndArch();
	}
}

void AGIDL_BTIEncodeICP(AGIDL_BTI* bti){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		if(bti->encode == ICP_ENCODE_THRESHOLD){
			AGIDL_InitICP(&bti->palette,AGIDL_ICP_16b_256);
		
			int pass = 0;
			u8 pal_index = 0;

			for(int y = 0; y < AGIDL_BTIGetHeight(bti); y++){
				for(int x = 0; x < AGIDL_BTIGetWidth(bti); x++){
					const COLOR16 clr = AGIDL_BTIGetClr16(bti,x,y);
					
					AGIDL_AddColorICP16(&bti->palette,pal_index,clr,AGIDL_BTIGetClrFmt(bti),AGIDL_BTIGetMaxDiff(bti),&pass);
					
					if(pass == 1 && pal_index < 256){
						pal_index++;
					}
					
					pass = 0;
				}
			}
		}
		else{
			AGIDL_EncodeHistogramICP(&bti->palette,bti->pixels.pix16,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),AGIDL_BTIGetClrFmt(bti));
		}
	}
}

void AGIDL_BTIEncodeIMG(AGIDL_BTI* bti, FILE* file){
	if(bti->icp != 1){
		if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
			AGIDL_InitBigEndArch();
			int x,y;
			for(y = 0; y < AGIDL_BTIGetHeight(bti); y += 4){
				for(x = 0; x < AGIDL_BTIGetWidth(bti); x += 4){
					u8 i,j;
					for(j = 0; j < 4; j++){
						for(i = 0; i < 4; i++){
							COLOR16 clr = AGIDL_BTIGetClr16(bti,x+i,y+j);
							AGIDL_WriteShort(file,clr);
						}
					}
				}
			}
			AGIDL_DisableBigEndArch();
		}
		else{
			int x,y;
			for(y = 0; y < AGIDL_BTIGetHeight(bti); y += 4){
				for(x = 0; x < AGIDL_BTIGetWidth(bti); x += 4){
					COLOR clr1 = AGIDL_BTIGetClr(bti,x+0,y+0);
					COLOR clr2 = AGIDL_BTIGetClr(bti,x+1,y+0);
					COLOR clr3 = AGIDL_BTIGetClr(bti,x+2,y+0);
					COLOR clr4 = AGIDL_BTIGetClr(bti,x+3,y+0);
					COLOR clr5 = AGIDL_BTIGetClr(bti,x+0,y+1);
					COLOR clr6 = AGIDL_BTIGetClr(bti,x+1,y+1);
					COLOR clr7 = AGIDL_BTIGetClr(bti,x+2,y+1);
					COLOR clr8 = AGIDL_BTIGetClr(bti,x+3,y+1);
					COLOR clr9 = AGIDL_BTIGetClr(bti,x+0,y+2);
					COLOR clr10 = AGIDL_BTIGetClr(bti,x+1,y+2);
					COLOR clr11 = AGIDL_BTIGetClr(bti,x+2,y+2);
					COLOR clr12 = AGIDL_BTIGetClr(bti,x+3,y+2);
					COLOR clr13 = AGIDL_BTIGetClr(bti,x+0,y+3);
					COLOR clr14 = AGIDL_BTIGetClr(bti,x+1,y+3);
					COLOR clr15 = AGIDL_BTIGetClr(bti,x+2,y+3);
					COLOR clr16 = AGIDL_BTIGetClr(bti,x+3,y+3);
					
					u8 a1 = AGIDL_GetA(clr1,AGIDL_BTIGetClrFmt(bti));
					u8 a2 = AGIDL_GetA(clr2,AGIDL_BTIGetClrFmt(bti));
					u8 a3 = AGIDL_GetA(clr3,AGIDL_BTIGetClrFmt(bti));
					u8 a4 = AGIDL_GetA(clr4,AGIDL_BTIGetClrFmt(bti));
					
					u8 r1 = AGIDL_GetR(clr1,AGIDL_BTIGetClrFmt(bti));
					u8 r2 = AGIDL_GetR(clr2,AGIDL_BTIGetClrFmt(bti));
					u8 r3 = AGIDL_GetR(clr3,AGIDL_BTIGetClrFmt(bti));
					u8 r4 = AGIDL_GetR(clr4,AGIDL_BTIGetClrFmt(bti));
					
					AGIDL_WriteByte(file,a1);
					AGIDL_WriteByte(file,r1);
					AGIDL_WriteByte(file,a2);
					AGIDL_WriteByte(file,r2);
					AGIDL_WriteByte(file,a3);
					AGIDL_WriteByte(file,r3);
					AGIDL_WriteByte(file,a4);
					AGIDL_WriteByte(file,r4);
					
					a1 = AGIDL_GetA(clr5,AGIDL_BTIGetClrFmt(bti));
					a2 = AGIDL_GetA(clr6,AGIDL_BTIGetClrFmt(bti));
					a3 = AGIDL_GetA(clr7,AGIDL_BTIGetClrFmt(bti));
					a4 = AGIDL_GetA(clr8,AGIDL_BTIGetClrFmt(bti));
					
					r1 = AGIDL_GetR(clr5,AGIDL_BTIGetClrFmt(bti));
					r2 = AGIDL_GetR(clr6,AGIDL_BTIGetClrFmt(bti));
					r3 = AGIDL_GetR(clr7,AGIDL_BTIGetClrFmt(bti));
					r4 = AGIDL_GetR(clr8,AGIDL_BTIGetClrFmt(bti));
					
					AGIDL_WriteByte(file,a1);
					AGIDL_WriteByte(file,r1);
					AGIDL_WriteByte(file,a2);
					AGIDL_WriteByte(file,r2);
					AGIDL_WriteByte(file,a3);
					AGIDL_WriteByte(file,r3);
					AGIDL_WriteByte(file,a4);
					AGIDL_WriteByte(file,r4);
					
					a1 = AGIDL_GetA(clr9,AGIDL_BTIGetClrFmt(bti));
					a2 = AGIDL_GetA(clr10,AGIDL_BTIGetClrFmt(bti));
					a3 = AGIDL_GetA(clr11,AGIDL_BTIGetClrFmt(bti));
					a4 = AGIDL_GetA(clr12,AGIDL_BTIGetClrFmt(bti));
					
					r1 = AGIDL_GetR(clr9,AGIDL_BTIGetClrFmt(bti));
					r2 = AGIDL_GetR(clr10,AGIDL_BTIGetClrFmt(bti));
					r3 = AGIDL_GetR(clr11,AGIDL_BTIGetClrFmt(bti));
					r4 = AGIDL_GetR(clr12,AGIDL_BTIGetClrFmt(bti));
					
					AGIDL_WriteByte(file,a1);
					AGIDL_WriteByte(file,r1);
					AGIDL_WriteByte(file,a2);
					AGIDL_WriteByte(file,r2);
					AGIDL_WriteByte(file,a3);
					AGIDL_WriteByte(file,r3);
					AGIDL_WriteByte(file,a4);
					AGIDL_WriteByte(file,r4);
					
					a1 = AGIDL_GetA(clr13,AGIDL_BTIGetClrFmt(bti));
					a2 = AGIDL_GetA(clr14,AGIDL_BTIGetClrFmt(bti));
					a3 = AGIDL_GetA(clr15,AGIDL_BTIGetClrFmt(bti));
					a4 = AGIDL_GetA(clr16,AGIDL_BTIGetClrFmt(bti));
					
					r1 = AGIDL_GetR(clr13,AGIDL_BTIGetClrFmt(bti));
					r2 = AGIDL_GetR(clr14,AGIDL_BTIGetClrFmt(bti));
					r3 = AGIDL_GetR(clr15,AGIDL_BTIGetClrFmt(bti));
					r4 = AGIDL_GetR(clr16,AGIDL_BTIGetClrFmt(bti));
					
					AGIDL_WriteByte(file,a1);
					AGIDL_WriteByte(file,r1);
					AGIDL_WriteByte(file,a2);
					AGIDL_WriteByte(file,r2);
					AGIDL_WriteByte(file,a3);
					AGIDL_WriteByte(file,r3);
					AGIDL_WriteByte(file,a4);
					AGIDL_WriteByte(file,r4);
					
					u8 g1 = AGIDL_GetG(clr1,AGIDL_BTIGetClrFmt(bti));
					u8 g2 = AGIDL_GetG(clr2,AGIDL_BTIGetClrFmt(bti));
					u8 g3 = AGIDL_GetG(clr3,AGIDL_BTIGetClrFmt(bti));
					u8 g4 = AGIDL_GetG(clr4,AGIDL_BTIGetClrFmt(bti));
					
					u8 b1 = AGIDL_GetB(clr1,AGIDL_BTIGetClrFmt(bti));
					u8 b2 = AGIDL_GetB(clr2,AGIDL_BTIGetClrFmt(bti));
					u8 b3 = AGIDL_GetB(clr3,AGIDL_BTIGetClrFmt(bti));
					u8 b4 = AGIDL_GetB(clr4,AGIDL_BTIGetClrFmt(bti));
					
					AGIDL_WriteByte(file,g1);
					AGIDL_WriteByte(file,b1);
					AGIDL_WriteByte(file,g2);
					AGIDL_WriteByte(file,b2);
					AGIDL_WriteByte(file,g3);
					AGIDL_WriteByte(file,b3);
					AGIDL_WriteByte(file,g4);
					AGIDL_WriteByte(file,b4);
					
					g1 = AGIDL_GetG(clr5,AGIDL_BTIGetClrFmt(bti));
					g2 = AGIDL_GetG(clr6,AGIDL_BTIGetClrFmt(bti));
					g3 = AGIDL_GetG(clr7,AGIDL_BTIGetClrFmt(bti));
					g4 = AGIDL_GetG(clr8,AGIDL_BTIGetClrFmt(bti));
					
					b1 = AGIDL_GetB(clr5,AGIDL_BTIGetClrFmt(bti));
					b2 = AGIDL_GetB(clr6,AGIDL_BTIGetClrFmt(bti));
					b3 = AGIDL_GetB(clr7,AGIDL_BTIGetClrFmt(bti));
					b4 = AGIDL_GetB(clr8,AGIDL_BTIGetClrFmt(bti));
					
					AGIDL_WriteByte(file,g1);
					AGIDL_WriteByte(file,b1);
					AGIDL_WriteByte(file,g2);
					AGIDL_WriteByte(file,b2);
					AGIDL_WriteByte(file,g3);
					AGIDL_WriteByte(file,b3);
					AGIDL_WriteByte(file,g4);
					AGIDL_WriteByte(file,b4);
					
					g1 = AGIDL_GetG(clr9,AGIDL_BTIGetClrFmt(bti));
					g2 = AGIDL_GetG(clr10,AGIDL_BTIGetClrFmt(bti));
					g3 = AGIDL_GetG(clr11,AGIDL_BTIGetClrFmt(bti));
					g4 = AGIDL_GetG(clr12,AGIDL_BTIGetClrFmt(bti));
					
					b1 = AGIDL_GetB(clr9,AGIDL_BTIGetClrFmt(bti));
					b2 = AGIDL_GetB(clr10,AGIDL_BTIGetClrFmt(bti));
					b3 = AGIDL_GetB(clr11,AGIDL_BTIGetClrFmt(bti));
					b4 = AGIDL_GetB(clr12,AGIDL_BTIGetClrFmt(bti));
					
					AGIDL_WriteByte(file,g1);
					AGIDL_WriteByte(file,b1);
					AGIDL_WriteByte(file,g2);
					AGIDL_WriteByte(file,b2);
					AGIDL_WriteByte(file,g3);
					AGIDL_WriteByte(file,b3);
					AGIDL_WriteByte(file,g4);
					AGIDL_WriteByte(file,b4);
					
					g1 = AGIDL_GetG(clr13,AGIDL_BTIGetClrFmt(bti));
					g2 = AGIDL_GetG(clr14,AGIDL_BTIGetClrFmt(bti));
					g3 = AGIDL_GetG(clr15,AGIDL_BTIGetClrFmt(bti));
					g4 = AGIDL_GetG(clr16,AGIDL_BTIGetClrFmt(bti));
					
					b1 = AGIDL_GetB(clr13,AGIDL_BTIGetClrFmt(bti));
					b2 = AGIDL_GetB(clr14,AGIDL_BTIGetClrFmt(bti));
					b3 = AGIDL_GetB(clr15,AGIDL_BTIGetClrFmt(bti));
					b4 = AGIDL_GetB(clr16,AGIDL_BTIGetClrFmt(bti));
					
					AGIDL_WriteByte(file,g1);
					AGIDL_WriteByte(file,b1);
					AGIDL_WriteByte(file,g2);
					AGIDL_WriteByte(file,b2);
					AGIDL_WriteByte(file,g3);
					AGIDL_WriteByte(file,b3);
					AGIDL_WriteByte(file,g4);
					AGIDL_WriteByte(file,b4);
				}
			}
		}
	}
	else{
		int x,y;
		for(y = 0; y < AGIDL_BTIGetHeight(bti); y += 4){
			for(x = 0; x < AGIDL_BTIGetWidth(bti); x += 8){
				u8 i,j;
				for(j = 0; j < 4; j++){
					for(i = 0; i < 8; i++){
						COLOR16 clr = AGIDL_BTIGetClr16(bti,x+i,y+j);
						u8 index = AGIDL_FindNearestColor(bti->palette,clr,AGIDL_BTIGetClrFmt(bti));
						AGIDL_WriteByte(file,index);
					}
				}
			}
		}
		
		u32 pos = ftell(file);
		
		fseek(file,12,SEEK_SET);
		
		AGIDL_InitBigEndArch();
		
		AGIDL_WriteLong(file,pos);
		
		fseek(file,pos,SEEK_SET);
		
		int i;
		for(i = 0; i < 256; i++){
			COLOR16 clr = bti->palette.icp.palette_16b_256[i];
			AGIDL_WriteShort(file,clr);
		}
		
		AGIDL_DisableBigEndArch();
	}
}

void AGIDL_ExportBTI(AGIDL_BTI* bti){
	FILE* file = fopen(bti->filename,"wb");
	
	if(file == NULL){
		printf("Could not open/create Nintendo Gamecube BTI image - %s!\n",bti->filename);
	}
	
	AGIDL_BTIEncodeHeader(bti,file);
	
	if(bti->icp != 1){
		switch(AGIDL_BTIGetClrFmt(bti)){
			case AGIDL_RGB_888:{
				AGIDL_BTIConvert24BPPTO16BPP(bti);
				AGIDL_BTI555TO565(bti);
				AGIDL_BTIEncodeIMG(bti,file);
				AGIDL_BTI565TO555(bti);
				AGIDL_BTIConvert16BPPTO24BPP(bti);
			}break;
			case AGIDL_BGR_888:{
				AGIDL_BTIBGR2RGB(bti);
				AGIDL_BTIConvert24BPPTO16BPP(bti);
				AGIDL_BTI555TO565(bti);
				AGIDL_BTIEncodeIMG(bti,file);
				AGIDL_BTI565TO555(bti);
				AGIDL_BTIConvert16BPPTO24BPP(bti);
				AGIDL_BTIBGR2RGB(bti);
			}break;
			case AGIDL_RGB_555:{
				AGIDL_BTI555TO565(bti);
				AGIDL_BTIEncodeIMG(bti,file);
				AGIDL_BTI565TO555(bti);
			}break;
			case AGIDL_BGR_555:{
				AGIDL_BTIBGR2RGB(bti);
				AGIDL_BTI555TO565(bti);
				AGIDL_BTIEncodeIMG(bti,file);
				AGIDL_BTI565TO555(bti);
				AGIDL_BTIBGR2RGB(bti);
			}break;
			case AGIDL_RGB_565:{
				AGIDL_BTIEncodeIMG(bti,file);
			}break;
			case AGIDL_BGR_565:{
				AGIDL_BTIBGR2RGB(bti);
				AGIDL_BTIEncodeIMG(bti,file);
				AGIDL_BTIBGR2RGB(bti);
			}break;
			case AGIDL_RGBA_8888:{
				AGIDL_BTIEncodeIMG(bti,file);
			}break;
			case AGIDL_ARGB_8888:{
				AGIDL_BTIEncodeIMG(bti,file);
			}break;
		}
	}
	else{
		switch(AGIDL_BTIGetClrFmt(bti)){
			case AGIDL_RGB_888:{
				AGIDL_BTIConvert24BPPTO16BPP(bti);
				AGIDL_BTI555TO565(bti);
				AGIDL_BTIEncodeICP(bti);
				AGIDL_BTIEncodeIMG(bti,file);
				AGIDL_BTI565TO555(bti);
				AGIDL_BTIConvert16BPPTO24BPP(bti);
			}break;
			case AGIDL_BGR_888:{
				AGIDL_BTIBGR2RGB(bti);
				AGIDL_BTIConvert24BPPTO16BPP(bti);
				AGIDL_BTI555TO565(bti);
				AGIDL_BTIEncodeICP(bti);
				AGIDL_BTIEncodeIMG(bti,file);
				AGIDL_BTI565TO555(bti);
				AGIDL_BTIConvert16BPPTO24BPP(bti);
				AGIDL_BTIBGR2RGB(bti);
			}break;
			case AGIDL_RGB_555:{
				AGIDL_BTI555TO565(bti);
				AGIDL_BTIEncodeICP(bti);
				AGIDL_BTIEncodeIMG(bti,file);
				AGIDL_BTI565TO555(bti);
			}break;
			case AGIDL_BGR_555:{
				AGIDL_BTIBGR2RGB(bti);
				AGIDL_BTI555TO565(bti);
				AGIDL_BTIEncodeICP(bti);
				AGIDL_BTIEncodeIMG(bti,file);
				AGIDL_BTI565TO555(bti);
				AGIDL_BTIBGR2RGB(bti);
			}break;
			case AGIDL_RGB_565:{
				AGIDL_BTIEncodeICP(bti);
				AGIDL_BTIEncodeIMG(bti,file);
			}break;
			case AGIDL_BGR_565:{
				AGIDL_BTIBGR2RGB(bti);
				AGIDL_BTIEncodeICP(bti);
				AGIDL_BTIEncodeIMG(bti,file);
				AGIDL_BTIBGR2RGB(bti);
			}break;
			case AGIDL_RGBA_8888:{
				AGIDL_BTIRGBATORGB(bti);
				AGIDL_BTIConvert24BPPTO16BPP(bti);
				AGIDL_BTI555TO565(bti);
				AGIDL_BTIEncodeICP(bti);
				AGIDL_BTIEncodeIMG(bti,file);
				AGIDL_BTI565TO555(bti);
				AGIDL_BTIConvert16BPPTO24BPP(bti);
			}break;
			case AGIDL_ARGB_8888:{
				AGIDL_BTIRGBATORGB(bti);
				AGIDL_BTIConvert24BPPTO16BPP(bti);
				AGIDL_BTI555TO565(bti);
				AGIDL_BTIEncodeICP(bti);
				AGIDL_BTIEncodeIMG(bti,file);
				AGIDL_BTI565TO555(bti);
				AGIDL_BTIConvert16BPPTO24BPP(bti);
			}break;
		}
	}
}
