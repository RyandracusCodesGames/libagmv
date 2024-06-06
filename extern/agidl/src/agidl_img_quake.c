#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "agidl_cc_core.h"
#include "agidl_img_quake.h"
#include "agidl_file_utils.h"
#include "agidl_mmu_utils.h"

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_quake.c
*   Date: 10/3/2023
*   Version: 0.1b
*   Updated: 4/7/2024
*   Author: Ryandracus Chapman
*
********************************************/

void AGIDL_SetLMPFilename(AGIDL_LMP *lmp, const char *filename){
	lmp->filename = (char*)realloc(lmp->filename,strlen(filename));
	AGIDL_FilenameCpy(lmp->filename,filename);
}

void AGIDL_LMPSetWidth(AGIDL_LMP *lmp, int width){
	lmp->width = width;
}

void AGIDL_LMPSetHeight(AGIDL_LMP *lmp, int height){
	lmp->height = height;
}

void AGIDL_LMPSetClrFmt(AGIDL_LMP *lmp, AGIDL_CLR_FMT fmt){
	lmp->fmt = fmt;
}

void AGIDL_LMPSetClr(AGIDL_LMP *lmp, int x, int y, COLOR clr){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) != 16){
		AGIDL_SetClr(lmp->pixels.pix32,clr,x,y,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp));
	}
	else{
		AGIDL_SetClr16(lmp->pixels.pix16,AGIDL_CLR_TO_CLR16(clr,AGIDL_BGR_888,AGIDL_LMPGetClrFmt(lmp)),x,y,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp));
	}
}

void AGIDL_LMPSetClr16(AGIDL_LMP *lmp, int x, int y, COLOR16 clr){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp) == 16)){
		AGIDL_SetClr16(lmp->pixels.pix16,clr,x,y,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp));
	}
	else{
		AGIDL_SetClr(lmp->pixels.pix32,AGIDL_CLR16_TO_CLR(clr,AGIDL_RGB_555,AGIDL_LMPGetClrFmt(lmp)),x,y,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp));
	}
}

void AGIDL_LMPSetRGB(AGIDL_LMP *lmp, int x, int y, u8 r, u8 g, u8 b){
	switch(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp))){
		case 24:{
			AGIDL_LMPSetClr(lmp,x,y,AGIDL_RGB(r,g,b,AGIDL_LMPGetClrFmt(lmp)));
		}break;
		case 16:{
			AGIDL_LMPSetClr16(lmp,x,y,AGIDL_RGB16(r,g,b,AGIDL_LMPGetClrFmt(lmp)));
		}break;
		case 32:{
			AGIDL_LMPSetClr(lmp,x,y,AGIDL_RGBA(r,g,b,0xff,AGIDL_LMPGetClrFmt(lmp)));
		}break;
	}
}

void AGIDL_ClearLMP(AGIDL_LMP *lmp, COLOR clr){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) != 16){
		AGIDL_ClrMemset(lmp->pixels.pix32,clr,AGIDL_LMPGetSize(lmp));
	}
	else{
		AGIDL_ClrMemset16(lmp->pixels.pix16,(COLOR16)clr,AGIDL_LMPGetSize(lmp));
	}
}

void AGIDL_ClearLMP16(AGIDL_LMP *lmp, COLOR16 clr){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) == 16){
		AGIDL_ClrMemset16(lmp->pixels.pix16,clr,AGIDL_LMPGetSize(lmp));
	}
	else{
		AGIDL_ClrMemset(lmp->pixels.pix32,AGIDL_CLR16_TO_CLR(clr,AGIDL_RGB_555,AGIDL_LMPGetClrFmt(lmp)),AGIDL_LMPGetSize(lmp));
	}
}

void AGIDL_ClearColorLMP(AGIDL_LMP* lmp, float r, float g, float b){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) == 16){
		AGIDL_ClearColorBuffer(lmp->pixels.pix16,r,g,b,AGIDL_LMPGetClrFmt(lmp),AGIDL_LMPGetSize(lmp));
	}
	else{
		AGIDL_ClearColorBuffer(lmp->pixels.pix32,r,g,b,AGIDL_LMPGetClrFmt(lmp),AGIDL_LMPGetSize(lmp));
	}
}

void AGIDL_FlushLMP(AGIDL_LMP* lmp){
	AGIDL_ClearLMP(lmp,0);
}

int AGIDL_LMPGetWidth(AGIDL_LMP *lmp){
	return lmp->width;
}

int AGIDL_LMPGetHeight(AGIDL_LMP *lmp){
	return lmp->height;
}

u32 AGIDL_LMPGetSize(AGIDL_LMP* lmp){
	return AGIDL_LMPGetWidth(lmp) * AGIDL_LMPGetHeight(lmp);
}

AGIDL_CLR_FMT AGIDL_LMPGetClrFmt(AGIDL_LMP* lmp){
	return lmp->fmt;
}

COLOR AGIDL_LMPGetClr(AGIDL_LMP *lmp, int x, int y){
	if(x >= 0 && y >= 0 && x < AGIDL_LMPGetWidth(lmp) && y < AGIDL_LMPGetHeight(lmp)){
		return lmp->pixels.pix32[x+y*AGIDL_LMPGetWidth(lmp)];
	}
}

COLOR16 AGIDL_LMPGetClr16(AGIDL_LMP *lmp, int x, int y){
	if(x >= 0 && y >= 0 && x < AGIDL_LMPGetWidth(lmp) && y < AGIDL_LMPGetHeight(lmp)){
		return lmp->pixels.pix16[x+y*AGIDL_LMPGetWidth(lmp)];
	}
}

void AGIDL_FreeLMP(AGIDL_LMP *lmp){
	free(lmp->filename);
	
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) == 16){
		free(lmp->pixels.pix16);
	}
	else{
		free(lmp->pixels.pix32);
	}
	
	free(lmp);
	
	if(lmp != NULL){
		lmp = NULL;
	}
}

void AGIDL_LMPRGB2BGR(AGIDL_LMP *lmp){
	if(lmp->fmt == AGIDL_RGB_888){
		AGIDL_RGB2BGR(lmp->pixels.pix32,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),&lmp->fmt);
	}
	
	if(lmp->fmt == AGIDL_RGB_555){
		AGIDL_RGB2BGR16(lmp->pixels.pix16,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),&lmp->fmt);
	}
}

void AGIDL_LMPBGR2RGB(AGIDL_LMP *lmp){
	if(lmp->fmt == AGIDL_BGR_888){
		AGIDL_BGR2RGB(lmp->pixels.pix32,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),&lmp->fmt);
	}
	
	if(lmp->fmt == AGIDL_BGR_555){
		AGIDL_BGR2RGB16(lmp->pixels.pix16,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),&lmp->fmt);
	}
}

void AGIDL_LMPConvert24BPPTO16BPP(AGIDL_LMP *lmp){
	if(lmp->fmt == AGIDL_RGB_888 || lmp->fmt == AGIDL_BGR_888 || lmp->fmt == AGIDL_RGBA_8888 || lmp->fmt == AGIDL_ARGB_8888){
		lmp->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_LMPGetWidth(lmp)*AGIDL_LMPGetHeight(lmp)));
		AGIDL_24BPPTO16BPP(lmp->pixels.pix32,lmp->pixels.pix16,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),&lmp->fmt);
		free(lmp->pixels.pix32);
	}
}

void AGIDL_LMPConvert16BPPTO24BPP(AGIDL_LMP *lmp){
	if(lmp->fmt == AGIDL_RGB_555 || lmp->fmt == AGIDL_BGR_555){
		lmp->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_LMPGetWidth(lmp)*AGIDL_LMPGetHeight(lmp)));
		AGIDL_16BPPTO24BPP(lmp->pixels.pix16,lmp->pixels.pix32,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),&lmp->fmt);
		free(lmp->pixels.pix16);
	}
}

void AGIDL_LMPConvert555TO565(AGIDL_LMP* lmp){
	AGIDL_555TO565(lmp->pixels.pix16,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),&lmp->fmt);
}

void AGIDL_LMPConvert565TO555(AGIDL_LMP* lmp){
	AGIDL_565TO555(lmp->pixels.pix16,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),&lmp->fmt);
}

void AGIDL_ColorConvertLMP(AGIDL_LMP* lmp, AGIDL_CLR_FMT dest){
	u8 sbits = AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)), dbits = AGIDL_GetBitCount(dest);
	if(sbits == 16 && dbits == 16){
		AGIDL_ColorConvertImgData(lmp->pixels.pix16,NULL,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),AGIDL_LMPGetClrFmt(lmp),dest);
		AGIDL_LMPSetClrFmt(lmp,dest);
	}
	else if((sbits == 24 || sbits == 32) && (dbits == 24 || dbits == 32)){
		AGIDL_ColorConvertImgData(lmp->pixels.pix32,NULL,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),AGIDL_LMPGetClrFmt(lmp),dest);
		AGIDL_LMPSetClrFmt(lmp,dest);
	}
	else if(sbits == 16 && (dbits == 24 || dbits == 32)){
		lmp->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),dest);
		AGIDL_ColorConvertImgData(lmp->pixels.pix16,lmp->pixels.pix32,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),AGIDL_LMPGetClrFmt(lmp),dest);
		AGIDL_LMPSetClrFmt(lmp,dest);
		free(lmp->pixels.pix16);
	}
	else{
		lmp->pixels.pix16 = (COLOR16*)AGIDL_AllocImgDataMMU(AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),dest);
		AGIDL_ColorConvertImgData(lmp->pixels.pix32,lmp->pixels.pix16,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),AGIDL_LMPGetClrFmt(lmp),dest);
		AGIDL_LMPSetClrFmt(lmp,dest);
		free(lmp->pixels.pix32);
	}
}

void AGIDL_LMPSyncPix(AGIDL_LMP *lmp, COLOR *clrs){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) != 16){
		AGIDL_ClrMemcpy(lmp->pixels.pix32,clrs,AGIDL_LMPGetSize(lmp));
	}
}

void AGIDL_LMPSyncPix16(AGIDL_LMP *lmp, COLOR16 *clrs){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) == 16){
		AGIDL_ClrMemcpy16(lmp->pixels.pix16,clrs,AGIDL_LMPGetSize(lmp));
	}
}

void AGIDL_LMPCopyPix(AGIDL_LMP* lmp, COLOR* clrs, u32 count){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) != 16){
		AGIDL_ClrMemcpy(lmp->pixels.pix32,clrs,count);
	}
}

void AGIDL_LMPCopyPix16(AGIDL_LMP* lmp, COLOR16* clrs, u32 count){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) == 16){
		AGIDL_ClrMemcpy16(lmp->pixels.pix16,clrs,count);
	}
}

AGIDL_LMP * AGIDL_CreateLMP(const char *filename, int width, int height, AGIDL_CLR_FMT fmt){
	AGIDL_LMP* lmp = (AGIDL_LMP*)malloc(sizeof(AGIDL_LMP));
	lmp->filename = (char*)malloc(strlen(filename)+1);
	AGIDL_FilenameCpy(lmp->filename,filename);
	AGIDL_LMPSetWidth(lmp,width);
	AGIDL_LMPSetHeight(lmp,height);
	AGIDL_LMPSetClrFmt(lmp,fmt);
	AGIDL_LMPSetMaxDiff(lmp,7);
	
	lmp->palette = AGIDL_GenerateQuakeICP();
	
	if(fmt == AGIDL_RGB_888 || fmt == AGIDL_BGR_888 || fmt == AGIDL_RGBA_8888 || fmt == AGIDL_ARGB_8888){
		lmp->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(width*height));
	}
	
	if(fmt == AGIDL_RGB_555 || fmt == AGIDL_BGR_555 || fmt == AGIDL_RGB_565 || fmt == AGIDL_BGR_565){
		lmp->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(width*height));
	}
	return lmp;
}

AGIDL_LMP* AGIDL_LMPCpyImg(AGIDL_LMP* lmp){
	AGIDL_LMP* lmpcpy = AGIDL_CreateLMP("lmpcpy.lmp",AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),lmp->fmt);
	AGIDL_LMPSetMaxDiff(lmpcpy,AGIDL_LMPGetMaxDiff(lmp));
	if(lmp->fmt == AGIDL_RGB_888 || lmp->fmt == AGIDL_BGR_888 || lmp->fmt == AGIDL_RGBA_8888 || lmp->fmt == AGIDL_ARGB_8888){
		AGIDL_LMPSyncPix(lmpcpy,lmp->pixels.pix32);
	}
	
	if(lmp->fmt == AGIDL_RGB_555 || lmp->fmt == AGIDL_BGR_555 || lmp->fmt == AGIDL_RGB_565 || lmp->fmt == AGIDL_BGR_565){
		AGIDL_LMPSyncPix16(lmpcpy,lmp->pixels.pix16);
	}
	return lmpcpy;
}

#define Q AGIDL_RGB_888

AGIDL_ICP AGIDL_GenerateQuakeICP(){
	COLOR icp[256];
	icp[0] = AGIDL_RGB(0,0,0,Q);          icp[1] = AGIDL_RGB(0x0f,0x0f,0x0f,Q); icp[2] = AGIDL_RGB(0x1f,0x1f,0x1f,Q);
	icp[3] = AGIDL_RGB(0x2f,0x2f,0x2f,Q); icp[4] = AGIDL_RGB(0x3f,0x3f,0x3f,Q); icp[5] = AGIDL_RGB(0x4b,0x4b,0x4b,Q);
	icp[6] = AGIDL_RGB(0x5b,0x5b,0x5b,Q); icp[7] = AGIDL_RGB(0x6b,0x6b,0x6b,Q); icp[8] = AGIDL_RGB(0x7b,0x7b,0x7b,Q);
	icp[9] = AGIDL_RGB(0x8b,0x8b,0x8b,Q); icp[10] = AGIDL_RGB(0x9b,0x9b,0x9b,Q); icp[11] = AGIDL_RGB(0xab,0xab,0xab,Q);
	icp[12] = AGIDL_RGB(0xbb,0xbb,0xbb,Q); icp[13] = AGIDL_RGB(0xcb,0xcb,0xcb,Q); icp[14] = AGIDL_RGB(0xdb,0xdb,0xdb,Q); icp[15] = AGIDL_RGB(0xeb,0xeb,0xeb,Q);
	
	icp[16] = AGIDL_RGB(15,11,7,Q);       icp[17] = AGIDL_RGB(23,15,11,Q); icp[18] = AGIDL_RGB(31,23,11,Q);
	icp[19] = AGIDL_RGB(39,27,15,Q); 	  icp[20] = AGIDL_RGB(47,35,19,Q); icp[21] = AGIDL_RGB(55,43,23,Q);
	icp[22] = AGIDL_RGB(63,47,23,Q);	  icp[23] = AGIDL_RGB(77,55,27,Q); icp[24] = AGIDL_RGB(83,59,27,Q);
	icp[25] = AGIDL_RGB(91,67,31,Q); 	  icp[26] = AGIDL_RGB(99,75,31,Q); icp[27] = AGIDL_RGB(107,83,31,Q);
	icp[28] = AGIDL_RGB(115,87,31,Q);	  icp[29] = AGIDL_RGB(123,95,35,Q); icp[30] = AGIDL_RGB(131,103,35,Q); icp[31] = AGIDL_RGB(143,111,35,Q);
	
	icp[32] = AGIDL_RGB(11,11,15,Q);      icp[33] = AGIDL_RGB(19,19,27,Q); icp[34] = AGIDL_RGB(27,27,39,Q);
	icp[35] = AGIDL_RGB(39,39,51,Q); 	  icp[36] = AGIDL_RGB(47,47,63,Q); icp[37] = AGIDL_RGB(55,55,75,Q);
	icp[38] = AGIDL_RGB(63,63,87,Q);	  icp[39] = AGIDL_RGB(71,71,103,Q); icp[40] = AGIDL_RGB(79,79,115,Q);
	icp[41] = AGIDL_RGB(91,91,127,Q); 	  icp[42] = AGIDL_RGB(99,99,139,Q); icp[43] = AGIDL_RGB(107,107,151,Q);
	icp[44] = AGIDL_RGB(115,115,163,Q);	  icp[45] = AGIDL_RGB(123,123,175,Q); icp[46] = AGIDL_RGB(131,131,187,Q); icp[47] = AGIDL_RGB(139,139,203,Q);
	
	icp[48] = AGIDL_RGB(0,0,0,Q);         icp[49] = AGIDL_RGB(7,7,0,Q); icp[50] = AGIDL_RGB(11,11,0,Q);
	icp[51] = AGIDL_RGB(19,19,0,Q); 	  icp[52] = AGIDL_RGB(27,27,0,Q); icp[53] = AGIDL_RGB(35,35,0,Q);
	icp[54] = AGIDL_RGB(43,43,7,Q);	      icp[55] = AGIDL_RGB(47,47,7,Q); icp[56] = AGIDL_RGB(55,55,7,Q);
	icp[57] = AGIDL_RGB(63,63,7,Q); 	  icp[58] = AGIDL_RGB(71,71,7,Q); icp[59] = AGIDL_RGB(75,75,11,Q);
	icp[60] = AGIDL_RGB(83,83,11,Q);	  icp[61] = AGIDL_RGB(91,91,11,Q); icp[62] = AGIDL_RGB(99,99,11,Q); icp[63] = AGIDL_RGB(107,107,15,Q);
	
	icp[64] = AGIDL_RGB(7,0,0,Q);         icp[65] = AGIDL_RGB(15,0,0,Q); icp[66] = AGIDL_RGB(23,0,0,Q);
	icp[67] = AGIDL_RGB(31,0,0,Q); 	      icp[68] = AGIDL_RGB(39,0,0,Q); icp[69] = AGIDL_RGB(47,0,0,Q);
	icp[70] = AGIDL_RGB(55,0,0,Q);	      icp[71] = AGIDL_RGB(63,0,0,Q); icp[72] = AGIDL_RGB(71,0,0,Q);
	icp[73] = AGIDL_RGB(79,0,0,Q); 	      icp[74] = AGIDL_RGB(87,0,0,Q); icp[75] = AGIDL_RGB(95,0,0,Q);
	icp[76] = AGIDL_RGB(103,0,0,Q);	      icp[77] = AGIDL_RGB(111,0,0,Q); icp[78] = AGIDL_RGB(119,0,0,Q); icp[79] = AGIDL_RGB(127,0,0,Q);
	
	icp[80] = AGIDL_RGB(19,19,0,Q);       icp[81] = AGIDL_RGB(27,27,0,Q); icp[82] = AGIDL_RGB(35,35,0,Q);
	icp[83] = AGIDL_RGB(47,43,0,Q); 	  icp[84] = AGIDL_RGB(55,47,0,Q); icp[85] = AGIDL_RGB(67,55,0,Q);
	icp[86] = AGIDL_RGB(75,59,7,Q);	      icp[87] = AGIDL_RGB(87,67,7,Q); icp[88] = AGIDL_RGB(95,71,7,Q);
	icp[89] = AGIDL_RGB(107,75,11,Q); 	  icp[90] = AGIDL_RGB(119,83,15,Q); icp[91] = AGIDL_RGB(131,87,19,Q);
	icp[92] = AGIDL_RGB(139,91,19,Q);	  icp[93] = AGIDL_RGB(151,95,27,Q); icp[94] = AGIDL_RGB(163,99,31,Q); icp[95] = AGIDL_RGB(175,103,35,Q);
	
	icp[96] = AGIDL_RGB(35,19,7,Q);       icp[97] = AGIDL_RGB(47,23,11,Q); icp[98] = AGIDL_RGB(59,31,15,Q);
	icp[99] = AGIDL_RGB(75,35,19,Q); 	  icp[100] = AGIDL_RGB(87,43,23,Q); icp[101] = AGIDL_RGB(99,47,31,Q);
	icp[102] = AGIDL_RGB(115,55,35,Q);	  icp[103] = AGIDL_RGB(127,59,43,Q); icp[104] = AGIDL_RGB(143,67,51,Q);
	icp[105] = AGIDL_RGB(159,79,51,Q); 	  icp[106] = AGIDL_RGB(175,99,47,Q); icp[107] = AGIDL_RGB(191,119,47,Q);
	icp[108] = AGIDL_RGB(207,143,43,Q);	  icp[109] = AGIDL_RGB(223,171,39,Q); icp[110] = AGIDL_RGB(239,203,31,Q); icp[111] = AGIDL_RGB(255,243,27,Q);
	
	icp[112] = AGIDL_RGB(11,7,0,Q);       icp[113] = AGIDL_RGB(27,19,0,Q); icp[114] = AGIDL_RGB(43,35,15,Q);
	icp[115] = AGIDL_RGB(55,43,19,Q); 	  icp[116] = AGIDL_RGB(71,51,27,Q); icp[117] = AGIDL_RGB(83,55,35,Q);
	icp[118] = AGIDL_RGB(99,63,43,Q);	  icp[119] = AGIDL_RGB(111,71,51,Q); icp[120] = AGIDL_RGB(127,83,63,Q);
	icp[121] = AGIDL_RGB(139,95,71,Q); 	  icp[122] = AGIDL_RGB(155,107,83,Q); icp[123] = AGIDL_RGB(167,123,95,Q);
	icp[124] = AGIDL_RGB(183,135,107,Q);  icp[125] = AGIDL_RGB(195,147,123,Q); icp[126] = AGIDL_RGB(211,163,139,Q); icp[127] = AGIDL_RGB(227,179,151,Q);
	
	icp[128] = AGIDL_RGB(171,139,163,Q);  icp[129] = AGIDL_RGB(159,127,151,Q); icp[130] = AGIDL_RGB(147,115,135,Q);
	icp[131] = AGIDL_RGB(139,103,123,Q);  icp[132] = AGIDL_RGB(127,91,111,Q); icp[133] = AGIDL_RGB(119,83,99,Q);
	icp[134] = AGIDL_RGB(107,75,87,Q);	  icp[135] = AGIDL_RGB(95,63,75,Q); icp[136] = AGIDL_RGB(87,55,67,Q);
	icp[137] = AGIDL_RGB(75,47,55,Q); 	  icp[138] = AGIDL_RGB(67,39,47,Q); icp[139] = AGIDL_RGB(55,31,35,Q);
	icp[140] = AGIDL_RGB(43,23,27,Q);     icp[141] = AGIDL_RGB(35,19,19,Q); icp[142] = AGIDL_RGB(23,11,11,Q); icp[143] = AGIDL_RGB(15,7,7,Q);
	
	
	icp[144] = AGIDL_RGB(187,115,159,Q);  icp[145] = AGIDL_RGB(175,107,143,Q); icp[146] = AGIDL_RGB(163,95,131,Q);
	icp[147] = AGIDL_RGB(151,87,119,Q);  icp[148] = AGIDL_RGB(139,79,107,Q); icp[149] = AGIDL_RGB(127,75,95,Q);
	icp[150] = AGIDL_RGB(115,111,95,Q);	  icp[151] = AGIDL_RGB(115,67,83,Q); icp[152] = AGIDL_RGB(107,59,75,Q);
	icp[153] = AGIDL_RGB(95,51,63,Q); 	  icp[154] = AGIDL_RGB(83,43,55,Q); icp[155] = AGIDL_RGB(71,35,43,Q);
	icp[156] = AGIDL_RGB(59,31,35,Q);     icp[157] = AGIDL_RGB(35,19,19,Q); icp[158] = AGIDL_RGB(23,11,11,Q); icp[159] = AGIDL_RGB(15,7,7,Q);
	
	icp[160] = AGIDL_RGB(219,195,187,Q);  icp[161] = AGIDL_RGB(203,179,167,Q); icp[162] = AGIDL_RGB(191,163,155,Q);
	icp[163] = AGIDL_RGB(175,151,139,Q);  icp[164] = AGIDL_RGB(163,135,123,Q); icp[165] = AGIDL_RGB(151,123,111,Q);
	icp[166] = AGIDL_RGB(135,111,95,Q);	  icp[167] = AGIDL_RGB(123,99,83,Q); icp[168] = AGIDL_RGB(107,87,71,Q);
	icp[169] = AGIDL_RGB(95,75,59,Q); 	  icp[170] = AGIDL_RGB(83,63,51,Q); icp[171] = AGIDL_RGB(67,51,39,Q);
	icp[172] = AGIDL_RGB(55,43,31,Q);     icp[173] = AGIDL_RGB(39,31,29,Q); icp[174] = AGIDL_RGB(27,19,15,Q); icp[175] = AGIDL_RGB(15,11,7,Q);
	
	icp[176] = AGIDL_RGB(111,131,123,Q);  icp[177] = AGIDL_RGB(103,123,111,Q); icp[178] = AGIDL_RGB(95,115,103,Q);
	icp[179] = AGIDL_RGB(87,107,95,Q);    icp[180] = AGIDL_RGB(79,99,87,Q); icp[181] = AGIDL_RGB(71,91,79,Q);
	icp[182] = AGIDL_RGB(63,83,71,Q);	  icp[183] = AGIDL_RGB(55,75,63,Q); icp[184] = AGIDL_RGB(47,67,55,Q);
	icp[185] = AGIDL_RGB(43,59,47,Q); 	  icp[186] = AGIDL_RGB(35,51,39,Q); icp[187] = AGIDL_RGB(31,43,31,Q);
	icp[188] = AGIDL_RGB(23,35,23,Q);     icp[189] = AGIDL_RGB(15,27,19,Q); icp[190] = AGIDL_RGB(11,19,11,Q); icp[191] = AGIDL_RGB(7,11,7,Q);
	
	icp[192] = AGIDL_RGB(255,243,27,Q);   icp[193] = AGIDL_RGB(239,223,23,Q); icp[194] = AGIDL_RGB(219,203,19,Q);
	icp[195] = AGIDL_RGB(203,183,15,Q);   icp[196] = AGIDL_RGB(187,167,15,Q); icp[197] = AGIDL_RGB(171,151,11,Q);
	icp[198] = AGIDL_RGB(155,131,7,Q);	  icp[199] = AGIDL_RGB(139,115,7,Q); icp[200] = AGIDL_RGB(123,99,7,Q);
	icp[201] = AGIDL_RGB(107,83,0,Q); 	  icp[202] = AGIDL_RGB(91,71,0,Q); icp[203] = AGIDL_RGB(75,55,0,Q);
	icp[204] = AGIDL_RGB(59,43,0,Q);      icp[205] = AGIDL_RGB(43,31,0,Q); icp[206] = AGIDL_RGB(27,15,0,Q); icp[207] = AGIDL_RGB(11,7,0,Q);
	
	icp[208] = AGIDL_RGB(0,0,255,Q);      icp[209] = AGIDL_RGB(11,11,239,Q); icp[210] = AGIDL_RGB(19,19,223,Q);
	icp[211] = AGIDL_RGB(27,27,207,Q);    icp[212] = AGIDL_RGB(35,35,191,Q); icp[213] = AGIDL_RGB(43,43,175,Q);
	icp[214] = AGIDL_RGB(47,47,159,Q);	  icp[215] = AGIDL_RGB(47,47,143,Q); icp[216] = AGIDL_RGB(47,47,127,Q);
	icp[217] = AGIDL_RGB(47,47,111,Q); 	  icp[218] = AGIDL_RGB(47,47,95,Q); icp[219] = AGIDL_RGB(43,43,79,Q);
	icp[220] = AGIDL_RGB(35,35,63,Q);     icp[221] = AGIDL_RGB(27,27,47,Q); icp[222] = AGIDL_RGB(19,19,31,Q); icp[223] = AGIDL_RGB(11,11,15,Q);
	
	icp[224] = AGIDL_RGB(43,0,0,Q);      icp[225] = AGIDL_RGB(59,0,0,Q); icp[226] = AGIDL_RGB(75,7,0,Q);
	icp[227] = AGIDL_RGB(95,7,0,Q);      icp[228] = AGIDL_RGB(111,15,191,Q); icp[229] = AGIDL_RGB(127,23,7,Q);
	icp[230] = AGIDL_RGB(147,31,7,Q);	 icp[231] = AGIDL_RGB(163,39,11,Q); icp[232] = AGIDL_RGB(183,51,15,Q);
	icp[233] = AGIDL_RGB(195,75,27,Q); 	 icp[234] = AGIDL_RGB(207,99,43,Q); icp[235] = AGIDL_RGB(219,127,59,Q);
	icp[236] = AGIDL_RGB(227,151,79,Q);  icp[237] = AGIDL_RGB(231,171,95,Q); icp[238] = AGIDL_RGB(239,191,119,Q); icp[239] = AGIDL_RGB(247,211,139,Q);
	
	icp[240] = AGIDL_RGB(167,123,59,Q);  icp[241] = AGIDL_RGB(183,155,55,Q); icp[242] = AGIDL_RGB(199,195,55,Q);
	icp[243] = AGIDL_RGB(231,227,87,Q);  icp[244] = AGIDL_RGB(127,191,255,Q); icp[245] = AGIDL_RGB(171,231,255,Q);
	icp[246] = AGIDL_RGB(215,255,255,Q); icp[247] = AGIDL_RGB(103,0,0,Q); icp[248] = AGIDL_RGB(139,0,0,Q);
	icp[249] = AGIDL_RGB(179,0,0,Q); 	 icp[250] = AGIDL_RGB(215,0,0,Q); icp[251] = AGIDL_RGB(255,0,0,Q);
	icp[252] = AGIDL_RGB(255,243,147,Q);  icp[253] = AGIDL_RGB(255,247,199,Q); icp[254] = AGIDL_RGB(255,255,255,Q); icp[255] = AGIDL_RGB(159,91,83,Q);
	
	AGIDL_ICP pal;
	pal.mode = AGIDL_ICP_256;
	pal.fmt = AGIDL_RGB_888;
	
	int i;
	for(i = 0; i < 256; i++){
		pal.icp.palette_256[i] = icp[i];
	}
	
	return pal;
}

void AGIDL_LMPDecodeHeader(AGIDL_LMP* lmp, FILE* file){
	lmp->width = AGIDL_ReadLong(file);
	lmp->height = AGIDL_ReadLong(file);
}

void AGIDL_LMPDecodeIMG(AGIDL_LMP* lmp, FILE* file){
	int x,y;
	for(y = AGIDL_LMPGetHeight(lmp)-1; y >= 0; y--){
		for(x = 0; x < AGIDL_LMPGetWidth(lmp); x++){
			u8 index = AGIDL_ReadByte(file);
			AGIDL_LMPSetClr(lmp,x,y,lmp->palette.icp.palette_256[index]);
		}
	}
}

AGIDL_LMP * AGIDL_LoadLMP(char *filename){
	FILE *file = fopen(filename,"rb");
	
	if(file == NULL){
		printf("Could not open/locate Quake LMP Image - %s!\n",filename);
	}
	
	AGIDL_LMP* lmp = (AGIDL_LMP*)malloc(sizeof(AGIDL_LMP));
	lmp->filename = (char*)malloc(strlen(filename)+1);
	AGIDL_FilenameCpy(lmp->filename,filename);
	lmp->palette = AGIDL_GenerateQuakeICP();
	AGIDL_LMPSetClrFmt(lmp,AGIDL_RGB_888);
	AGIDL_LMPDecodeHeader(lmp,file);
	
	lmp->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),AGIDL_LMPGetClrFmt(lmp));
	
	AGIDL_LMPDecodeIMG(lmp,file);
	
	fclose(file);
	
	return lmp;
}

void AGIDL_LMPSetMaxDiff(AGIDL_LMP* lmp, int max_diff){
	lmp->max_diff = max_diff;
}

int AGIDL_LMPGetMaxDiff(AGIDL_LMP* lmp){
	return lmp->max_diff;
}

void AGIDL_LMPEncodeICP(AGIDL_LMP* lmp, FILE* file){
	int x,y;
	for(y = AGIDL_LMPGetHeight(lmp) - 1; y >= 0; y--){
		for(x = 0; x < AGIDL_LMPGetWidth(lmp); x++){
			COLOR clr = AGIDL_LMPGetClr(lmp,x,y);
			u8 index = AGIDL_FindNearestColor(lmp->palette,clr,AGIDL_RGB_888);
			AGIDL_WriteByte(file,index);
		}
	}
}

void AGIDL_LMPEncodeHeader(AGIDL_LMP* lmp, FILE* file){
	u32 width = AGIDL_LMPGetWidth(lmp), height = AGIDL_LMPGetHeight(lmp);
	AGIDL_WriteLong(file,width);
	AGIDL_WriteLong(file,height);
}

void AGIDL_ExportLMP(AGIDL_LMP *lmp){
	FILE* file = fopen(lmp->filename,"wb");
	
	if(file == NULL){
		printf("Could not export Quake LMP - %s!\n",lmp->filename);
	}
	
	AGIDL_LMPEncodeHeader(lmp,file);
	
	switch(AGIDL_LMPGetClrFmt(lmp)){
		case AGIDL_RGB_888:{
			AGIDL_LMPEncodeICP(lmp,file);
		}break;
		case AGIDL_BGR_888:{
			AGIDL_LMPBGR2RGB(lmp);
			AGIDL_LMPEncodeICP(lmp,file);
			AGIDL_LMPRGB2BGR(lmp);
		}break;
		case AGIDL_RGB_555:{
			AGIDL_LMPConvert16BPPTO24BPP(lmp);
			AGIDL_LMPEncodeICP(lmp,file);
			AGIDL_LMPConvert24BPPTO16BPP(lmp);
		}break;
		case AGIDL_RGB_565:{
			AGIDL_LMPConvert565TO555(lmp);
			AGIDL_LMPConvert16BPPTO24BPP(lmp);
			AGIDL_LMPEncodeICP(lmp,file);
			AGIDL_LMPConvert24BPPTO16BPP(lmp);
			AGIDL_LMPConvert555TO565(lmp);
		}break;
		case AGIDL_BGR_555:{
			AGIDL_LMPBGR2RGB(lmp);
			AGIDL_LMPConvert16BPPTO24BPP(lmp);
			AGIDL_LMPEncodeICP(lmp,file);
			AGIDL_LMPRGB2BGR(lmp);
			AGIDL_LMPConvert24BPPTO16BPP(lmp);
		}break;
		case AGIDL_BGR_565:{
			AGIDL_LMPConvert565TO555(lmp);
			AGIDL_LMPBGR2RGB(lmp);
			AGIDL_LMPConvert16BPPTO24BPP(lmp);
			AGIDL_LMPEncodeICP(lmp,file);
			AGIDL_LMPRGB2BGR(lmp);
			AGIDL_LMPConvert24BPPTO16BPP(lmp);
			AGIDL_LMPConvert555TO565(lmp);
		}break;
		case AGIDL_RGBA_8888:{
			COLOR* buf = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),AGIDL_LMPGetClrFmt(lmp));
			AGIDL_ClrMemcpy(buf,lmp->pixels.pix32,AGIDL_LMPGetSize(lmp));
			AGIDL_CLR_FMT src = AGIDL_LMPGetClrFmt(lmp);
			AGIDL_ColorConvertLMP(lmp,AGIDL_RGB_888);
			AGIDL_LMPEncodeICP(lmp,file);
			AGIDL_ColorConvertLMP(lmp,src);
			AGIDL_ClrMemcpy(lmp->pixels.pix32,buf,AGIDL_LMPGetSize(lmp));
			AGIDL_LMPSetClrFmt(lmp,src);
			free(buf);
		}break;
		case AGIDL_ARGB_8888:{
			COLOR* buf = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),AGIDL_LMPGetClrFmt(lmp));
			AGIDL_ClrMemcpy(buf,lmp->pixels.pix32,AGIDL_LMPGetSize(lmp));
			AGIDL_CLR_FMT src = AGIDL_LMPGetClrFmt(lmp);
			AGIDL_ColorConvertLMP(lmp,AGIDL_RGB_888);
			AGIDL_LMPEncodeICP(lmp,file);
			AGIDL_ColorConvertLMP(lmp,src);
			AGIDL_ClrMemcpy(lmp->pixels.pix32,buf,AGIDL_LMPGetSize(lmp));
			AGIDL_LMPSetClrFmt(lmp,src);
			free(buf);
		}break;
	}
	
	fclose(file);
}