/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_3df.c
*   Date: 2/12/2024
*   Version: 0.4b
*   Updated: 6/10/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <stdlib.h>
#include <string.h>
#include <agidl_img_3df.h>
#include <agidl_img_error.h>
#include <agidl_file_utils.h>
#include <agidl_cc_converter.h>
#include <agidl_mmu_utils.h>
#include <agidl_math_utils.h>

void AGIDL_Set3DFFilename(AGIDL_3DF* glide, const char* filename){
	if(glide->filename != NULL){
		free(glide->filename);
		glide->filename = (char*)malloc(strlen(filename)+1);
		AGIDL_FilenameCpy(glide->filename,filename);
	}
	else{
		glide->filename = (char*)malloc(strlen(filename)+1);
		AGIDL_FilenameCpy(glide->filename,filename);
	}
}

void AGIDL_3DFSetWidth(AGIDL_3DF* glide, u32 width){
	glide->width = width;
}

void AGIDL_3DFSetHeight(AGIDL_3DF* glide, u32 height){
	glide->height = height;
}

void AGIDL_3DFSetClrFmt(AGIDL_3DF* glide, AGIDL_CLR_FMT fmt){
	glide->fmt = fmt;
}

void AGIDL_3DFSetICPMode(AGIDL_3DF* glide, AGIDL_Bool icp){
	glide->icp = icp;
}

void AGIDL_3DFSetICPEncoding(AGIDL_3DF* glide, AGIDL_ICP_ENCODE encode){
	glide->encode = encode;
}

void AGIDL_3DFSetMaxDiff(AGIDL_3DF* glide, int max_diff){
	glide->max_diff = max_diff;
}

void AGIDL_3DFBuildMipmap(AGIDL_3DF* glide, AGIDL_Bool mipped){
	glide->mipped = mipped;
}

void AGIDL_3DFSetClr(AGIDL_3DF* glide, int x, int y, COLOR clr){
	if(x >= 0 && y >= 0 && x < AGIDL_3DFGetWidth(glide) && y < AGIDL_3DFGetHeight(glide)){
		glide->pixels.pix32[x+y*AGIDL_3DFGetWidth(glide)] = clr;
	}
}

void AGIDL_3DFSetClr16(AGIDL_3DF* glide, int x, int y, COLOR16 clr){
	if(x >= 0 && y >= 0 && x < AGIDL_3DFGetWidth(glide) && y < AGIDL_3DFGetHeight(glide)){
		glide->pixels.pix16[x+y*AGIDL_3DFGetWidth(glide)] = clr;
	}
}

void AGIDL_3DFSetRGB(AGIDL_3DF* glide, int x, int y, u8 r, u8 g, u8 b){
	if(x >= 0 && y >= 0 && x < AGIDL_3DFGetWidth(glide) && y < AGIDL_3DFGetHeight(glide)){
		int bits = AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide));
		if(bits == 24 || bits == 32){
			glide->pixels.pix32[x+y*AGIDL_3DFGetWidth(glide)] = AGIDL_RGB(r,g,b,AGIDL_3DFGetClrFmt(glide));
		}
		else{
			glide->pixels.pix16[x+y*AGIDL_3DFGetWidth(glide)] = AGIDL_RGB16(r,g,b,AGIDL_3DFGetClrFmt(glide));
		}
	}
}

void AGIDL_3DFSetRGBA(AGIDL_3DF* glide, int x, int y, u8 r, u8 g, u8 b, u8 a){
	if(x >= 0 && y >= 0 && x < AGIDL_3DFGetWidth(glide) && y < AGIDL_3DFGetHeight(glide)){
		if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 32){
			glide->pixels.pix32[x+y*AGIDL_3DFGetWidth(glide)] = AGIDL_RGBA(r,g,b,a,AGIDL_3DFGetClrFmt(glide));
		}
	}
}
void AGIDL_3DFSetColor3f(AGIDL_3DF* glide, int x, int y, f32 r, f32 g, f32 b){
if(x >= 0 && y >= 0 && x < AGIDL_3DFGetWidth(glide) && y < AGIDL_3DFGetHeight(glide)){
		int bits = AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide));
		if(bits == 24 || bits == 32){
			glide->pixels.pix32[x+y*AGIDL_3DFGetWidth(glide)] = AGIDL_Color3f(r,g,b,AGIDL_3DFGetClrFmt(glide));
		}
		else{
			glide->pixels.pix16[x+y*AGIDL_3DFGetWidth(glide)] = AGIDL_Color3f(r,g,b,AGIDL_3DFGetClrFmt(glide));
		}
	}
}

void AGIDL_3DFSetColor4f(AGIDL_3DF* glide, int x, int y, f32 r, f32 g, f32 b, f32 a){
	if(x >= 0 && y >= 0 && x < AGIDL_3DFGetWidth(glide) && y < AGIDL_3DFGetHeight(glide)){
		if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 32){
			glide->pixels.pix32[x+y*AGIDL_3DFGetWidth(glide)] = AGIDL_Color4f(r,g,b,a,AGIDL_3DFGetClrFmt(glide));
		}
	}
}

u32 AGIDL_3DFGetWidth(AGIDL_3DF* glide){
	return glide->width;
}

u32 AGIDL_3DFGetHeight(AGIDL_3DF* glide){
	return glide->height;
}

u32 AGIDL_3DFGetSize(AGIDL_3DF* glide){
	return AGIDL_3DFGetWidth(glide) * AGIDL_3DFGetHeight(glide);
}

AGIDL_CLR_FMT AGIDL_3DFGetClrFmt(AGIDL_3DF* glide){
	return glide->fmt;
}

int AGIDL_3DFGetMaxDiff(AGIDL_3DF* glide){
	return glide->max_diff;
}

COLOR AGIDL_3DFGetClr(AGIDL_3DF* glide, int x, int y){
	if(x >= 0 && y >= 0 && x < AGIDL_3DFGetWidth(glide) && y < AGIDL_3DFGetHeight(glide)){
		return glide->pixels.pix32[x+y*AGIDL_3DFGetWidth(glide)];
	}
	else return 0;
}

COLOR16 AGIDL_3DFGetClr16(AGIDL_3DF* glide, int x, int y){
	if(x >= 0 && y >= 0 && x < AGIDL_3DFGetWidth(glide) && y < AGIDL_3DFGetHeight(glide)){
		return glide->pixels.pix16[x+y*AGIDL_3DFGetWidth(glide)];
	}
	else return 0;
}

void AGIDL_Clear3DF(AGIDL_3DF *glide, COLOR clr){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) != 16){
		AGIDL_ClrMemset(glide->pixels.pix32,clr,AGIDL_3DFGetSize(glide));
	}
	else{
		AGIDL_ClrMemset16(glide->pixels.pix16,(COLOR16)clr,AGIDL_3DFGetSize(glide));
	}
}

void AGIDL_Clear3DF16(AGIDL_3DF *glide, COLOR16 clr){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 16){
		AGIDL_ClrMemset16(glide->pixels.pix16,clr,AGIDL_3DFGetSize(glide));
	}
	else{
		AGIDL_ClrMemset(glide->pixels.pix32,AGIDL_CLR16_TO_CLR(clr,AGIDL_RGB_555,AGIDL_3DFGetClrFmt(glide)),AGIDL_3DFGetSize(glide));
	}
}

void AGIDL_ClearColor3DF(AGIDL_3DF* glide, float r, float g, float b){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 16){
		AGIDL_ClearColorBuffer(glide->pixels.pix16,r,g,b,AGIDL_3DFGetClrFmt(glide),AGIDL_3DFGetSize(glide));
	}
	else{
		AGIDL_ClearColorBuffer(glide->pixels.pix32,r,g,b,AGIDL_3DFGetClrFmt(glide),AGIDL_3DFGetSize(glide));
	}
}

void AGIDL_Flush3DF(AGIDL_3DF* glide){
	AGIDL_Clear3DF(glide,0);
}

void AGIDL_3DFSyncPix(AGIDL_3DF *glide, COLOR *clrs){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) != 16){
		AGIDL_ClrMemcpy(glide->pixels.pix32,clrs,AGIDL_3DFGetSize(glide));
	}
}

void AGIDL_3DFSyncPix16(AGIDL_3DF *glide, COLOR16 *clrs){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 16){
		AGIDL_ClrMemcpy16(glide->pixels.pix16,clrs,AGIDL_3DFGetSize(glide));
	}
}

void AGIDL_3DFCopyPix(AGIDL_3DF* glide, COLOR* clrs, u32 count){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) != 16){
		AGIDL_ClrMemcpy(glide->pixels.pix32,clrs,count);
	}
}

void AGIDL_3DFCopyPix16(AGIDL_3DF* glide, COLOR16* clrs, u32 count){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 16){
		AGIDL_ClrMemcpy16(glide->pixels.pix16,clrs,count);
	}
}

void AGIDL_Free3DF(AGIDL_3DF* glide){
	if(glide->filename != NULL){
		free(glide->filename);
		glide->filename = NULL;
	}
	
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 16){
		if(glide->pixels.pix16 != NULL){
			free(glide->pixels.pix16);
			glide->pixels.pix16 = NULL;
		}
	}
	else{
		if(glide->pixels.pix32 != NULL){
			free(glide->pixels.pix32);
			glide->pixels.pix32 = NULL;
		}
	}
	
	if(glide != NULL){
		free(glide);
		glide = NULL;
	}
}

void AGIDL_3DFRGB2BGR(AGIDL_3DF *glide){
	if(glide->fmt == AGIDL_RGB_888){
		AGIDL_RGB2BGR(glide->pixels.pix32,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),&glide->fmt);
	}
	if(glide->fmt == AGIDL_RGB_555 || glide->fmt == AGIDL_RGB_565){
		AGIDL_RGB2BGR16(glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),&glide->fmt);
	}
}

void AGIDL_3DFBGR2RGB(AGIDL_3DF* glide){
	if(glide->fmt == AGIDL_BGR_888){
		AGIDL_BGR2RGB(glide->pixels.pix32,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),&glide->fmt);
	}
	if(glide->fmt == AGIDL_BGR_555 || glide->fmt == AGIDL_BGR_565){
		AGIDL_BGR2RGB16(glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),&glide->fmt);
	}
}

void AGIDL_3DFConvert16BPPTO24BPP(AGIDL_3DF *glide){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 16){
		glide->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_3DFGetHeight(glide)*AGIDL_3DFGetWidth(glide)));
		AGIDL_16BPPTO24BPP(glide->pixels.pix16,glide->pixels.pix32,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),&glide->fmt);
		free(glide->pixels.pix16);
	}
}

void AGIDL_3DFConvert24BPPTO16BPP(AGIDL_3DF *glide){
	if(glide->fmt == AGIDL_RGB_888 || glide->fmt == AGIDL_BGR_888 || glide->fmt == AGIDL_RGBA_8888 || glide->fmt == AGIDL_ARGB_8888){
		glide->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_3DFGetHeight(glide)*AGIDL_3DFGetWidth(glide)));
		AGIDL_24BPPTO16BPP(glide->pixels.pix32,glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),&glide->fmt);
		free(glide->pixels.pix32);
	}
}

void AGIDL_3DFConvert555TO565(AGIDL_3DF* glide){
	AGIDL_555TO565(glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),&glide->fmt);
}

void AGIDL_3DFConvert565TO555(AGIDL_3DF* glide){
	AGIDL_565TO555(glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),&glide->fmt);
}

void AGIDL_3DFConvertRGB2RGBA(AGIDL_3DF* glide){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 24){
		COLOR* cpy = AGIDL_AllocImgDataMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
		AGIDL_ClrMemcpy(cpy,glide->pixels.pix32,AGIDL_3DFGetSize(glide));
		AGIDL_ConvertRGB2RGBA(cpy,glide->pixels.pix32,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide),AGIDL_RGBA_8888);
		AGIDL_3DFSetClrFmt(glide,AGIDL_RGBA_8888);
		free(cpy);
	}
}

void AGIDL_3DFConvertRGBA2RGB(AGIDL_3DF* glide){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 32){
		COLOR* cpy = AGIDL_AllocImgDataMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
		AGIDL_ClrMemcpy(cpy,glide->pixels.pix32,AGIDL_3DFGetSize(glide));
		AGIDL_ConvertRGBA2RGB(cpy,glide->pixels.pix32,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide),AGIDL_RGB_888);
		AGIDL_3DFSetClrFmt(glide,AGIDL_RGB_888);
		free(cpy);
	}
}

void AGIDL_ColorConvert3DF(AGIDL_3DF* glide, AGIDL_CLR_FMT dest){
	u8 sbits = AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)), dbits = AGIDL_GetBitCount(dest);
	if(sbits == 16 && dbits == 16){
		AGIDL_ColorConvertImgData(glide->pixels.pix16,NULL,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide),dest);
		AGIDL_3DFSetClrFmt(glide,dest);
	}
	else if((sbits == 24 || sbits == 32) && (dbits == 24 || dbits == 32)){
		AGIDL_ColorConvertImgData(glide->pixels.pix32,NULL,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide),dest);
		AGIDL_3DFSetClrFmt(glide,dest);
	}
	else if(sbits == 16 && (dbits == 24 || dbits == 32)){
		glide->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),dest);
		AGIDL_ColorConvertImgData(glide->pixels.pix16,glide->pixels.pix32,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide),dest);
		AGIDL_3DFSetClrFmt(glide,dest);
		free(glide->pixels.pix16);
	}
	else{
		glide->pixels.pix16 = (COLOR16*)AGIDL_AllocImgDataMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),dest);
		AGIDL_ColorConvertImgData(glide->pixels.pix32,glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide),dest);
		AGIDL_3DFSetClrFmt(glide,dest);
		free(glide->pixels.pix32);
	}
}

AGIDL_3DF * AGIDL_Create3DF(const char* filename, int width, int height, AGIDL_CLR_FMT fmt){
	AGIDL_3DF* glide = (AGIDL_3DF*)malloc(sizeof(AGIDL_3DF));
	glide->filename = (char*)malloc(strlen(filename)+1);
	AGIDL_FilenameCpy(glide->filename,filename);
	AGIDL_3DFSetWidth(glide,width);
	AGIDL_3DFSetHeight(glide,height);
	AGIDL_3DFSetClrFmt(glide,fmt);
	AGIDL_3DFSetICPMode(glide,FALSE);
	AGIDL_3DFSetICPEncoding(glide,ICP_ENCODE_THRESHOLD);
	AGIDL_3DFSetMaxDiff(glide,8);
	AGIDL_3DFBuildMipmap(glide,FALSE);
	glide->mipmap = NULL;
	
	if(AGIDL_GetBitCount(fmt) == 16){
		glide->pixels.pix16 = (COLOR16*)AGIDL_AllocImgDataMMU(width,height,fmt);
	}
	else{
		glide->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(width,height,fmt);
	}
	
	return glide;
}

AGIDL_3DF* AGIDL_3DFCpyImg(AGIDL_3DF* glide){
	AGIDL_3DF* glidecpy = AGIDL_Create3DF(glide->filename,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
	AGIDL_3DFSetICPMode(glidecpy,glide->icp);
	AGIDL_3DFSetICPEncoding(glidecpy,glide->encode);
	AGIDL_3DFSetMaxDiff(glidecpy,glide->max_diff);
	AGIDL_3DFBuildMipmap(glidecpy,glide->mipped);
	
	memcpy(glidecpy->mipmap,glide->mipmap,sizeof(AGIDL_MIPMAP));
	
	if(AGIDL_3DFGetClrFmt(glide) == 16){
		AGIDL_3DFSyncPix16(glidecpy,glide->pixels.pix16);
	}
	else{
		AGIDL_3DFSyncPix(glidecpy,glide->pixels.pix32);
	}
	
	return glidecpy;
}

int AGIDL_3DFDecodePartialHeader(AGIDL_3DF* glide, FILE* file){
	glide->header.magic[0] = AGIDL_ReadByte(file);
	glide->header.magic[1] = AGIDL_ReadByte(file);
	glide->header.magic[2] = AGIDL_ReadByte(file);
	glide->header.magic[3] = AGIDL_ReadByte(file);
	
	glide->header.version[0] = AGIDL_ReadByte(file);
	glide->header.version[1] = AGIDL_ReadByte(file);
	glide->header.version[2] = AGIDL_ReadByte(file);
	glide->header.version[3] = AGIDL_ReadByte(file);
	glide->header.version[4] = AGIDL_ReadByte(file);
	
	if(glide->header.magic[0] != '3' || glide->header.magic[1] != 'd' || glide->header.magic[2] != 'f'){
		return INVALID_HEADER_FORMATTING_ERROR;
	}
	else if(glide->header.version[0] != 'v' || glide->header.version[1] != '1' || glide->header.version[2] != '.'){
		return INVALID_HEADER_FORMATTING_ERROR;
	}
	else return NO_IMG_ERROR;
}

int AGIDL_3DFDecodeHeader(AGIDL_3DF* glide, FILE* file){
	glide->header.magic[0] = AGIDL_ReadByte(file);
	glide->header.magic[1] = AGIDL_ReadByte(file);
	glide->header.magic[2] = AGIDL_ReadByte(file);
	glide->header.magic[3] = AGIDL_ReadByte(file);
	
	glide->header.version[0] = AGIDL_ReadByte(file);
	glide->header.version[1] = AGIDL_ReadByte(file);
	glide->header.version[2] = AGIDL_ReadByte(file);
	glide->header.version[3] = AGIDL_ReadByte(file);
	glide->header.version[4] = AGIDL_ReadByte(file);
	
	char format[10];
	int i = 0;
	u8 byte = 0;
	
	while(byte != 'l'){
		byte = AGIDL_ReadByte(file);
		format[i] = byte;
		i++;
		
		if(i >= 10){
			break;
		}
	}
	
	fseek(file,10,SEEK_CUR);
	
	AGIDL_ReadByte(file);
	
	char wlod[4];
	char hlod[4];
	
	int count = 0;
	
	while(byte != 32){
		byte = AGIDL_ReadByte(file);
		wlod[count++] = byte;
		
		if(count >= 4){
			break;
		}
	}

	if(count == 2){
		AGIDL_3DFSetWidth(glide,AGIDL_GetCharNum(wlod[0]));
	}
	else if(count == 3){
		AGIDL_3DFSetWidth(glide,(AGIDL_GetCharNum(wlod[0])*10)+AGIDL_GetCharNum(wlod[1]));
	}
	else{
		AGIDL_3DFSetWidth(glide,(AGIDL_GetCharNum(wlod[0])*100)+(AGIDL_GetCharNum(wlod[1])*10)+(AGIDL_GetCharNum(wlod[2])));
	}
	
	if(AGIDL_3DFGetWidth(glide) == 1){
		glide->mipped = TRUE;
	}
	else{
		glide->mipmap = NULL;
	}

	count = 0;
	byte = 0;
	
	while(byte != 10){
		byte = AGIDL_ReadByte(file);
		hlod[count++] = byte;
		
		if(count >= 4){
			break;
		}
	}
	
	if(count == 2){
		AGIDL_3DFSetHeight(glide,AGIDL_GetCharNum(hlod[0]));
	}
	else if(count == 3){
		AGIDL_3DFSetHeight(glide,(AGIDL_GetCharNum(hlod[0])*10)+AGIDL_GetCharNum(hlod[1]));
	}
	else{
		AGIDL_3DFSetHeight(glide,(AGIDL_GetCharNum(hlod[0])*100)+(AGIDL_GetCharNum(hlod[1])*10)+(AGIDL_GetCharNum(hlod[2])));
	}
	
	fseek(file,14,SEEK_CUR);
	
	char aspect[3];
	aspect[0] = AGIDL_ReadByte(file);
	aspect[1] = AGIDL_ReadByte(file);
	aspect[2] = AGIDL_ReadByte(file);
	
	AGIDL_ReadByte(file);
	
	glide->header.fmt = AGIDL_3DFGetGlideClrFmt(format);
	glide->header.aspect = AGIDL_3DFGetAspect(aspect);
	
	GLIDE_ASPECT_TABLE a = AGIDL_3DFGetAspect(aspect);
	
	switch(a){
		case GLIDE_ASPECT_1x1:{
			AGIDL_3DFSetWidth(glide,AGIDL_3DFGetHeight(glide));
		}break;
		case GLIDE_ASPECT_1x2:{
			AGIDL_3DFSetWidth(glide,AGIDL_3DFGetHeight(glide)*2);
		}break;
		case GLIDE_ASPECT_1x4:{
			AGIDL_3DFSetWidth(glide,AGIDL_3DFGetHeight(glide)*4);
		}break;
		case GLIDE_ASPECT_1x8:{
			AGIDL_3DFSetWidth(glide,AGIDL_3DFGetHeight(glide)*8);
		}break;
		case GLIDE_ASPECT_2x1:{
			AGIDL_3DFSetWidth(glide,AGIDL_3DFGetHeight(glide)/2);
		}break;
		case GLIDE_ASPECT_4x1:{
			AGIDL_3DFSetWidth(glide,AGIDL_3DFGetHeight(glide)/4);
		}break;
		case GLIDE_ASPECT_8x1:{
			AGIDL_3DFSetWidth(glide,AGIDL_3DFGetHeight(glide)/8);
		}break;
	}
	
	if(glide->header.magic[0] != '3' || glide->header.magic[1] != 'd' || glide->header.magic[2] != 'f'){
		return INVALID_HEADER_FORMATTING_ERROR;
	}
	else if(glide->header.fmt == GLIDE_UNKNOWN_FMT || glide->header.aspect == GLIDE_UNKNOWN_ASPECT){
		return INVALID_IMG_FORMATTING_ERROR;
	}
	else return NO_IMG_ERROR;
}


GLIDE_3DF_CLR_FMT AGIDL_3DFGetGlideClrFmt(char format[10]){
	switch(format[0]){
		case 'r':{
			if(format[1] != 'g' || format[2] != 'b'){
				return GLIDE_UNKNOWN_FMT;
			}
			else{
				if(format[3] == '3' && format[4] == '3' && format[5] == '2'){
					return GLIDE_RGB_332;
				}
				else if(format[3] == '5' && format[4] == '6' && format[5] == '5'){
					return GLIDE_RGB_565;
				}
				else return GLIDE_UNKNOWN_FMT;
			}
		}break;
		case 'a':{
			if(format[1] == '8'){
				return GLIDE_A8;
			}
			else if(format[1] == 'i' && format[2] == '4' && format[3] == '4'){
				return GLIDE_AI44;
			}
			else if(format[0] == 'a' && format[1] == 'r' && format[2] == 'g' && format[3] == 'b' && format[4] == '1' && format[5] == '5' && format[6] == '5' && format[7] == '5'){
				return GLIDE_ARGB_1555;
			}
			else if(format[0] == 'a' && format[1] == 'y' && format[2] == 'i' && format[3] == 'q' && format[4] == '8' && format[5] == '4' && format[6] == '4' && format[7] == '2'){
				return GLIDE_AYIQ8_422;
			}
			else if(format[0] == 'a' && format[1] == 'r' && format[2] == 'g' && format[3] == 'b' && format[4] == '4' && format[5] == '4' && format[6] == '4' && format[7] == '4'){
				return GLIDE_ARGB_4444;
			}
			else if(format[1] == 'r' && format[2] == 'g' && format[3] == 'b' && format[4] == '8' && format[5] == '3' && format[6] == '3' && format[7] == '2'){
					return GLIDE_ARGB_8332;
			}
			else if(format[0] == 'a' && format[1] == 'i' && format[2] == '8' && format[3] == '8'){
				return GLIDE_AI_88;
			}
			else if(format[0] == 'a' && format[1] == 'r' && format[2] == 'g' && format[3] == 'b' && format[4] == '8' && format[5] == '8' && format[6] == '8' && format[7] == '8'){
				return GLIDE_ARGB_8888;
			}
			else return GLIDE_UNKNOWN_FMT;
		}break;
		case 'i':{
			if(format[1] == '8'){
				return GLIDE_I8;
			}
			else return GLIDE_UNKNOWN_FMT;
		}break;
		case 'y':{
			if(format[1] == 'y' && format[2] == 'i' && format[3] == 'q'){
				return GLIDE_YIQ;
			}
			else return GLIDE_UNKNOWN_FMT;
		}break;
		case 'p':{
			if(format[1] == '8'){
				return GLIDE_P8;
			}
			else return GLIDE_UNKNOWN_FMT;
		}break;
		default: return GLIDE_UNKNOWN_FMT; break;
	}	
}

u8 AGIDL_GetCharNum(char num){
	switch(num){
		case '0':{
			return 0;
		}break;
		case '1':{
			return 1;
		}break;
		case '2':{
			return 2;
		}break;
		case '3':{
			return 3;
		}break;
		case '4':{
			return 4;
		}break;
		case '5':{
			return 5;
		}break;
		case '6':{
			return 6;
		}break;
		case '7':{
			return 7;
		}break;
		case '8':{
			return 8;
		}break;
		case '9':{
			return 9;
		}break;
		default: return 0; break;
	}
}

char AGIDL_GetNumChar(u8 c){
	switch(c){
		case 0:{
			return '0';
		}break;
		case 1:{
			return '1';
		}break;
		case 2:{
			return '2';
		}break;
		case 3:{
			return '3';
		}break;
		case 4:{
			return '4';
		}break;
		case 5:{
			return '5';
		}break;
		case 6:{
			return '6';
		}break;
		case 7:{
			return '7';
		}break;
		case 8:{
			return '8';
		}break;
		case 9:{
			return '9';
		}break;
		default: return '0'; break;
	}
}

GLIDE_ASPECT_TABLE AGIDL_3DFGetAspect(char aspect[3]){
	if(aspect[0] == '1' && aspect[2] == '1'){
		return GLIDE_ASPECT_1x1;
	}
	else if(aspect[0] == '1' && aspect[2] == '2'){
		return GLIDE_ASPECT_1x2;
	}
	else if(aspect[0] == '1' && aspect[2] == '4'){
		return GLIDE_ASPECT_1x4;
	}
	else if(aspect[0] == '1' && aspect[2] == '8'){
		return GLIDE_ASPECT_1x8;
	}
	else if(aspect[0] == '2' && aspect[2] == '1'){
		return GLIDE_ASPECT_2x1;
	}
	else if(aspect[0] == '4' && aspect[2] == '1'){
		return GLIDE_ASPECT_4x1;
	}
	else if(aspect[0] == '8' && aspect[2] == '1'){
		return GLIDE_ASPECT_8x1;
	}
	else return GLIDE_UNKNOWN_ASPECT;
}

void AGIDL_3DFDecodeIMG(AGIDL_3DF* glide, FILE* file){
	if(glide->mipped != TRUE){
		switch(glide->header.fmt){
			case GLIDE_RGB_565:{
				AGIDL_3DFSetClrFmt(glide,AGIDL_RGB_565);
				glide->pixels.pix16 = (COLOR16*)AGIDL_AllocImgDataMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
				AGIDL_InitBigEndArch();
				AGIDL_ReadBufClr16(file,glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide));
				AGIDL_DisableBigEndArch();
			}break;
			case GLIDE_ARGB_1555:{
				AGIDL_3DFSetClrFmt(glide,AGIDL_RGB_555);
				glide->pixels.pix16 = (COLOR16*)AGIDL_AllocImgDataMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
				AGIDL_InitBigEndArch();
				AGIDL_ReadBufClr16(file,glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide));
				AGIDL_DisableBigEndArch();
			}break;
			case GLIDE_ARGB_4444:{
				u32 size = AGIDL_3DFGetSize(glide);
				AGIDL_CLR_FMT fmt = AGIDL_3DFGetClrFmt(glide);
				AGIDL_3DFSetClrFmt(glide,AGIDL_RGBA_8888);
				glide->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),fmt);
				int i;
				for(i = 0; i < size; i++){
					u8 byte1 = AGIDL_ReadByte(file), byte2 = AGIDL_ReadByte(file);
					
					u8 a = (byte1 >> 4);
					u8 r = (byte1 & 0xf);
					u8 g = (byte2 >> 4);
					u8 b = (byte2 & 0xf);
					
					a <<= 4; r <<= 4; g <<= 4; b <<= 4;
					
					glide->pixels.pix32[i] = AGIDL_RGBA(r,g,b,a,fmt);
				}
			}break;
			case GLIDE_RGB_332:{
				u32 size = AGIDL_3DFGetSize(glide);
				AGIDL_CLR_FMT fmt = AGIDL_3DFGetClrFmt(glide);
				AGIDL_3DFSetClrFmt(glide,AGIDL_RGB_555);
				glide->pixels.pix16 = (COLOR16*)AGIDL_AllocImgDataMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),fmt);
				int i;
				for(i = 0; i < size; i++){
					u8 byte = AGIDL_ReadByte(file);
					
					u8 r = (byte & 0xff) >> 5;
					u8 g = (byte & 0x1f) >> 2;
					u8 b = (byte & 0x3);
					
					r <<= 2;
					g <<= 2;
					b *= 10.67;
					
					glide->pixels.pix16[i] = AGIDL_RGB16(r,g,b,fmt);
				}
			}break;
			case GLIDE_ARGB_8332:{
				u32 size = AGIDL_3DFGetSize(glide);
				AGIDL_CLR_FMT fmt = AGIDL_3DFGetClrFmt(glide);
				AGIDL_3DFSetClrFmt(glide,AGIDL_ARGB_8888);
				glide->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),fmt);
				int i;
				for(i = 0; i < size; i++){
					u8 a = AGIDL_ReadByte(file);
					u8 byte = AGIDL_ReadByte(file);
					
					u8 r = (byte & 0xff) >> 5;
					u8 g = (byte & 0x1f) >> 2;
					u8 b = (byte & 0x3);
					
					r <<= 5;
					g <<= 5;
					b <<= 6;
					
					glide->pixels.pix32[i] = AGIDL_RGBA(r,g,b,a,fmt);
				}
			}break;
			case GLIDE_ARGB_8888:{
				AGIDL_3DFSetClrFmt(glide,AGIDL_ARGB_8888);
				glide->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
				AGIDL_InitBigEndArch();
				AGIDL_ReadBufRGBA(file,glide->pixels.pix32,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide));
				AGIDL_DisableBigEndArch();
			}break;
			case GLIDE_I8:{
				u32 size = AGIDL_3DFGetSize(glide);
				AGIDL_CLR_FMT fmt = AGIDL_3DFGetClrFmt(glide);
				AGIDL_3DFSetClrFmt(glide,AGIDL_RGB_888);
				glide->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),fmt);
				int i;
				for(i = 0; i < size; i++){
					u8 intensity = AGIDL_ReadByte(file);
					glide->pixels.pix32[i] = AGIDL_RGB(intensity,intensity,intensity,fmt);
				}
			}break;
			case GLIDE_AI_88:{
				u32 size = AGIDL_3DFGetSize(glide);
				AGIDL_CLR_FMT fmt = AGIDL_3DFGetClrFmt(glide);
				AGIDL_3DFSetClrFmt(glide,AGIDL_RGBA_8888);
				glide->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),fmt);
				int i;
				for(i = 0; i < size; i++){
					u8 alpha = AGIDL_ReadByte(file);
					u8 intensity = AGIDL_ReadByte(file);
					glide->pixels.pix32[i] = AGIDL_RGBA(intensity,intensity,intensity,alpha,fmt);
				}
			}break;
			case GLIDE_AI44:{
				u32 size = AGIDL_3DFGetSize(glide);
				AGIDL_CLR_FMT fmt = AGIDL_3DFGetClrFmt(glide);
				AGIDL_3DFSetClrFmt(glide,AGIDL_RGBA_8888);
				glide->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),fmt);
				int i;
				for(i = 0; i < size; i++){
					u8 alpha = AGIDL_ReadByte(file) << 4;
					u8 intensity = AGIDL_ReadByte(file) << 4;
					glide->pixels.pix32[i] = AGIDL_RGBA(intensity,intensity,intensity,alpha,fmt);
				}
			}break;
			case GLIDE_P8:{
				u32 size = AGIDL_3DFGetSize(glide);
				AGIDL_CLR_FMT fmt = AGIDL_3DFGetClrFmt(glide);
				AGIDL_3DFSetClrFmt(glide,AGIDL_RGBA_8888);
				glide->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),fmt);
				AGIDL_InitICP(&glide->palette,AGIDL_ICP_256);
				int i;
				for(i = 0; i < 256; i++){
					u8 a = AGIDL_ReadByte(file);
					u8 r = AGIDL_ReadByte(file);
					u8 g = AGIDL_ReadByte(file);
					u8 b = AGIDL_ReadByte(file);
					glide->palette.icp.palette_256[i] = AGIDL_RGBA(r,g,b,a,fmt);
				}
				for(i = 0; i < size; i++){
					u8 index = AGIDL_ReadByte(file);
					glide->pixels.pix32[i] = glide->palette.icp.palette_256[index];
				}
			}break;
		}
	}
	else{
		switch(glide->header.fmt){
			case GLIDE_RGB_565:{
				AGIDL_3DFSetClrFmt(glide,AGIDL_RGB_565);
				glide->pixels.pix16 = (COLOR16*)AGIDL_AllocImgDataMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
				int count = 0;
				int w = AGIDL_3DFGetWidth(glide), h = AGIDL_3DFGetHeight(glide);
				while(w >= 2 && h >= 2){
					w >>= 1;
					h >>= 1;
					count++;
				}
				AGIDL_InitBigEndArch();
				glide->mipmap = AGIDL_LoadMipmapImgData(file,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide),count,FALSE);
				AGIDL_ClrMemcpy16(glide->pixels.pix16,glide->mipmap->mipmap[0].img_data,AGIDL_3DFGetSize(glide));
				AGIDL_DisableBigEndArch();
			}break;
			case GLIDE_ARGB_1555:{
				AGIDL_3DFSetClrFmt(glide,AGIDL_RGB_555);
				glide->pixels.pix16 = (COLOR16*)AGIDL_AllocImgDataMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
				int count = 0;
				int w = AGIDL_3DFGetWidth(glide), h = AGIDL_3DFGetHeight(glide);
				while(w >= 2 && h >= 2){
					w >>= 1;
					h >>= 1;
					count++;
				}
				AGIDL_InitBigEndArch();
				glide->mipmap = AGIDL_LoadMipmapImgData(file,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide),count,FALSE);
				AGIDL_ClrMemcpy16(glide->pixels.pix16,glide->mipmap->mipmap[0].img_data,AGIDL_3DFGetSize(glide));
				AGIDL_DisableBigEndArch();
			}break;
			case GLIDE_ARGB_8888:{
				AGIDL_3DFSetClrFmt(glide,AGIDL_RGBA_8888);
				glide->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
				int count = 0;
				int w = AGIDL_3DFGetWidth(glide), h = AGIDL_3DFGetHeight(glide);
				while(w >= 2 && h >= 2){
					w >>= 1;
					h >>= 1;
					count++;
				}
				AGIDL_InitBigEndArch();
				glide->mipmap = AGIDL_LoadMipmapImgData(file,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide),count,FALSE);
				AGIDL_ClrMemcpy(glide->pixels.pix32,glide->mipmap->mipmap[0].img_data,AGIDL_3DFGetSize(glide));
				AGIDL_DisableBigEndArch();
			}break;
			case GLIDE_ARGB_4444:{
				AGIDL_3DFSetClrFmt(glide,AGIDL_RGBA_8888);
				glide->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
				int w = AGIDL_3DFGetWidth(glide), h = AGIDL_3DFGetHeight(glide);
				glide->mipmap = AGIDL_CreateMipmapMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide),FALSE);
			
				int i;
				for(i = 0; i < glide->mipmap->mipcount; i++){
					glide->mipmap->mipmap[i].width = w;
					glide->mipmap->mipmap[i].height = h;
					glide->mipmap->mipmap[i].fmt = AGIDL_3DFGetClrFmt(glide);
					glide->mipmap->mipmap[i].img_data = (COLOR*)AGIDL_AllocImgDataMMU(w,h,AGIDL_3DFGetClrFmt(glide));
					
					COLOR* img = (COLOR*)glide->mipmap->mipmap[i].img_data;
					
					int j;
					for(j = 0; j < w*h; j++){
						u8 byte1 = AGIDL_ReadByte(file), byte2 = AGIDL_ReadByte(file);
					
						u8 a = (byte1 >> 4);
						u8 r = (byte1 & 0xf);
						u8 g = (byte2 >> 4);
						u8 b = (byte2 & 0xf);
						
						a <<= 4; r <<= 4; g <<= 4; b <<= 4;
						img[j] = AGIDL_RGBA(r,g,b,a,AGIDL_3DFGetClrFmt(glide));
					}
					
					w >>= 1;
					h >>= 1;
				}
				COLOR* img = glide->mipmap->mipmap[0].img_data;
				AGIDL_ClrMemcpy(glide->pixels.pix32,img,AGIDL_3DFGetSize(glide));
			}break;
			case GLIDE_RGB_332:{
				AGIDL_3DFSetClrFmt(glide,AGIDL_RGB_555);
				glide->pixels.pix16 = (COLOR16*)AGIDL_AllocImgDataMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
				int w = AGIDL_3DFGetWidth(glide), h = AGIDL_3DFGetHeight(glide);
				glide->mipmap = AGIDL_CreateMipmapMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide),FALSE);
			
				int i;
				for(i = 0; i < glide->mipmap->mipcount; i++){
					glide->mipmap->mipmap[i].width = w;
					glide->mipmap->mipmap[i].height = h;
					glide->mipmap->mipmap[i].fmt = AGIDL_3DFGetClrFmt(glide);
					glide->mipmap->mipmap[i].img_data = (COLOR16*)AGIDL_AllocImgDataMMU(w,h,AGIDL_3DFGetClrFmt(glide));
					
					COLOR16* img = (COLOR16*)glide->mipmap->mipmap[i].img_data;
					
					int j;
					for(j = 0; j < w*h; j++){	
						u8 byte = AGIDL_ReadByte(file);
					
						u8 r = (byte & 0xff) >> 5;
						u8 g = (byte & 0x1f) >> 2;
						u8 b = (byte & 0x3);
						
						r <<= 2;
						g <<= 2;
						b *= 10.67;
					
						img[j] = AGIDL_RGB16(r,g,b,AGIDL_3DFGetClrFmt(glide));
					}
					
					w >>= 1;
					h >>= 1;
				}
				COLOR16* img = glide->mipmap->mipmap[0].img_data;
				AGIDL_ClrMemcpy16(glide->pixels.pix16,img,AGIDL_3DFGetSize(glide));
			}break;
			case GLIDE_ARGB_8332:{
				AGIDL_3DFSetClrFmt(glide,AGIDL_RGBA_8888);
				glide->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
				int w = AGIDL_3DFGetWidth(glide), h = AGIDL_3DFGetHeight(glide);
				glide->mipmap = AGIDL_CreateMipmapMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide),FALSE);
			
				int i;
				for(i = 0; i < glide->mipmap->mipcount; i++){
					glide->mipmap->mipmap[i].width = w;
					glide->mipmap->mipmap[i].height = h;
					glide->mipmap->mipmap[i].fmt = AGIDL_3DFGetClrFmt(glide);
					glide->mipmap->mipmap[i].img_data = (COLOR*)AGIDL_AllocImgDataMMU(w,h,AGIDL_3DFGetClrFmt(glide));
					
					COLOR* img = (COLOR*)glide->mipmap->mipmap[i].img_data;
					
					int j;
					for(j = 0; j < w*h; j++){
						u8 alpha = AGIDL_ReadByte(file);
						u8 byte = AGIDL_ReadByte(file);
						
						u8 r = (byte & 0xff) >> 5;
						u8 g = (byte & 0x1f) >> 2;
						u8 b = (byte & 0x3);
						
						r <<= 5;
						g <<= 5;
						b <<= 6;
						
						img[j] = AGIDL_RGBA(r,g,b,alpha,AGIDL_3DFGetClrFmt(glide));
					}
					
					w >>= 1;
					h >>= 1;
				}
				COLOR* img = glide->mipmap->mipmap[0].img_data;
				AGIDL_ClrMemcpy(glide->pixels.pix32,img,AGIDL_3DFGetSize(glide));
			}break;
			case GLIDE_I8:{
				AGIDL_3DFSetClrFmt(glide,AGIDL_RGB_888);
				glide->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
				int w = AGIDL_3DFGetWidth(glide), h = AGIDL_3DFGetHeight(glide);
				glide->mipmap = AGIDL_CreateMipmapMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide),FALSE);
			
				int i;
				for(i = 0; i < glide->mipmap->mipcount; i++){
					glide->mipmap->mipmap[i].width = w;
					glide->mipmap->mipmap[i].height = h;
					glide->mipmap->mipmap[i].fmt = AGIDL_3DFGetClrFmt(glide);
					glide->mipmap->mipmap[i].img_data = (COLOR*)AGIDL_AllocImgDataMMU(w,h,AGIDL_3DFGetClrFmt(glide));
					
					COLOR* img = (COLOR*)glide->mipmap->mipmap[i].img_data;
					
					int j;
					for(j = 0; j < w*h; j++){
						u8 intensity = AGIDL_ReadByte(file);
						
						img[j] = AGIDL_RGB(intensity,intensity,intensity,AGIDL_3DFGetClrFmt(glide));
					}
					
					w >>= 1;
					h >>= 1;
				}
				COLOR* img = glide->mipmap->mipmap[0].img_data;
				AGIDL_ClrMemcpy(glide->pixels.pix32,img,AGIDL_3DFGetSize(glide));
			}break;
			case GLIDE_AI_88:{
				AGIDL_3DFSetClrFmt(glide,AGIDL_RGBA_8888);
				glide->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
				int w = AGIDL_3DFGetWidth(glide), h = AGIDL_3DFGetHeight(glide);
				glide->mipmap = AGIDL_CreateMipmapMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide),FALSE);
			
				int i;
				for(i = 0; i < glide->mipmap->mipcount; i++){
					glide->mipmap->mipmap[i].width = w;
					glide->mipmap->mipmap[i].height = h;
					glide->mipmap->mipmap[i].fmt = AGIDL_3DFGetClrFmt(glide);
					glide->mipmap->mipmap[i].img_data = (COLOR*)AGIDL_AllocImgDataMMU(w,h,AGIDL_3DFGetClrFmt(glide));
					
					COLOR* img = (COLOR*)glide->mipmap->mipmap[i].img_data;
					
					int j;
					for(j = 0; j < w*h; j++){
						u8 alpha = AGIDL_ReadByte(file);
						u8 intensity = AGIDL_ReadByte(file);
						
						img[j] = AGIDL_RGBA(intensity,intensity,intensity,alpha,AGIDL_3DFGetClrFmt(glide));
					}
					
					w >>= 1;
					h >>= 1;
				}
				COLOR* img = glide->mipmap->mipmap[0].img_data;
				AGIDL_ClrMemcpy(glide->pixels.pix32,img,AGIDL_3DFGetSize(glide));
			}break;
			case GLIDE_P8:{
				AGIDL_3DFSetClrFmt(glide,AGIDL_RGBA_8888);
				glide->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
				AGIDL_InitICP(&glide->palette,AGIDL_ICP_256);
				int w = AGIDL_3DFGetWidth(glide), h = AGIDL_3DFGetHeight(glide);
				glide->mipmap = AGIDL_CreateMipmapMMU(AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide),FALSE);
				int i;
				for(i = 0; i < 256; i++){
					u8 a = AGIDL_ReadByte(file);
					u8 r = AGIDL_ReadByte(file);
					u8 g = AGIDL_ReadByte(file);
					u8 b = AGIDL_ReadByte(file);
					glide->palette.icp.palette_256[i] = AGIDL_RGBA(r,g,b,a,AGIDL_3DFGetClrFmt(glide));
				}
				for(i = 0; i < glide->mipmap->mipcount; i++){
					glide->mipmap->mipmap[i].width = w;
					glide->mipmap->mipmap[i].height = h;
					glide->mipmap->mipmap[i].fmt = AGIDL_3DFGetClrFmt(glide);
					glide->mipmap->mipmap[i].img_data = (COLOR*)AGIDL_AllocImgDataMMU(w,h,AGIDL_3DFGetClrFmt(glide));
					
					COLOR* img = (COLOR*)glide->mipmap->mipmap[i].img_data;
					
					int j;
					for(j = 0; j < w*h; j++){
						u8 index = AGIDL_ReadByte(file);
						
						img[j] = glide->palette.icp.palette_256[index];
					}
					
					w >>= 1;
					h >>= 1;
				}
				COLOR* img = glide->mipmap->mipmap[0].img_data;
				AGIDL_ClrMemcpy(glide->pixels.pix32,img,AGIDL_3DFGetSize(glide));
			}break;
		}
	}
}

void AGIDL_3DFEncodeHeader(AGIDL_3DF* glide, FILE* file){
	AGIDL_PrintFourCC(file,'3','d','f',32);
	AGIDL_PrintFourCC(file,'v','1','.','1');
	AGIDL_WriteByte(file,10);
	
	if(glide->icp != TRUE){
		switch(AGIDL_3DFGetClrFmt(glide)){
			case AGIDL_RGB_555:{
				char format[8] = "argb1555";
				fwrite(format,1,8,file);
				AGIDL_WriteByte(file,10);
			}break;
			case AGIDL_RGB_565:{
				char format[6] = "rgb565";
				fwrite(format,1,6,file);
				AGIDL_WriteByte(file,10);
			}break;
			case AGIDL_RGBA_8888:{
				char format[8] = "argb4444";
				fwrite(format,1,8,file);
				AGIDL_WriteByte(file,10);
			}break;
			case AGIDL_ARGB_8888:{
				char format[8] = "argb4444";
				fwrite(format,1,8,file);
				AGIDL_WriteByte(file,10);
			}break;
		}
	}
	else{
		char format[2] = "p8";
		fwrite(format,1,2,file);
		AGIDL_WriteByte(file,10);
	}
	
	u16 w = AGIDL_NearestPow2(AGIDL_3DFGetWidth(glide));
	u16 h = AGIDL_NearestPow2(AGIDL_3DFGetHeight(glide));
	
	w = AGIDL_Clamp(1,w,256);
	h = AGIDL_Clamp(1,h,256);
	
	char lod[11] = "lod range: ";
	fwrite(lod,1,11,file);
	
	if(glide->mipped == TRUE){
		if(w > h){
			switch(w){
				case 2:{
					char size[3] = "1 2";
					fwrite(size,1,3,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 4:{
					char size[3] = "1 4";
					fwrite(size,1,3,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 8:{
					char size[3] = "1 8";
					fwrite(size,1,3,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 16:{
					char size[4] = "1 16";
					fwrite(size,1,4,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 32:{
					char size[4] = "1 32";
					fwrite(size,1,4,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 64:{
					char size[4] = "1 64";
					fwrite(size,1,4,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 128:{
					char size[5] = "1 128";
					fwrite(size,1,5,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 256:{
					char size[5] = "1 256";
					fwrite(size,1,5,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 512:{
					char size[5] = "1 512";
					fwrite(size,1,5,file);
					AGIDL_WriteByte(file,10);
				}break;
			}
		}
		else{
			switch(h){
				case 2:{
					char size[3] = "1 2";
					fwrite(size,1,3,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 4:{
					char size[3] = "1 4";
					fwrite(size,1,3,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 8:{
					char size[3] = "1 8";
					fwrite(size,1,3,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 16:{
					char size[4] = "1 16";
					fwrite(size,1,4,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 32:{
					char size[4] = "1 32";
					fwrite(size,1,4,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 64:{
					char size[4] = "1 64";
					fwrite(size,1,4,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 128:{
					char size[5] = "1 128";
					fwrite(size,1,5,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 256:{
					char size[5] = "1 256";
					fwrite(size,1,5,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 512:{
					char size[5] = "1 512";
					fwrite(size,1,5,file);
					AGIDL_WriteByte(file,10);
				}break;
			}
		}
	}
	else{
		if(w > h){
			switch(w){
				case 2:{
					char size[3] = "2 2";
					fwrite(size,1,3,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 4:{
					char size[3] = "4 4";
					fwrite(size,1,3,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 8:{
					char size[3] = "8 8";
					fwrite(size,1,3,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 16:{
					char size[5] = "16 16";
					fwrite(size,1,5,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 32:{
					char size[5] = "32 32";
					fwrite(size,1,5,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 64:{
					char size[5] = "64 64";
					fwrite(size,1,5,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 128:{
					char size[7] = "128 128";
					fwrite(size,1,7,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 256:{
					char size[7] = "256 256";
					fwrite(size,1,7,file);
					AGIDL_WriteByte(file,10);
				}break;
			}
		}
		else{
			switch(h){
				case 2:{
					char size[3] = "2 2";
					fwrite(size,1,3,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 4:{
					char size[3] = "4 4";
					fwrite(size,1,3,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 8:{
					char size[3] = "8 8";
					fwrite(size,1,3,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 16:{
					char size[5] = "16 16";
					fwrite(size,1,5,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 32:{
					char size[5] = "32 32";
					fwrite(size,1,5,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 64:{
					char size[5] = "64 64";
					fwrite(size,1,5,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 128:{
					char size[7] = "128 128";
					fwrite(size,1,7,file);
					AGIDL_WriteByte(file,10);
				}break;
				case 256:{
					char size[7] = "256 256";
					fwrite(size,1,7,file);
					AGIDL_WriteByte(file,10);
				}break;
			}
		}
	}
	
	char aspect[14] = "aspect ratio: ";
	fwrite(aspect,1,14,file);
	
	if(w == h){
		char ratio[3] = "1 1";
		fwrite(ratio,1,3,file);
		AGIDL_WriteByte(file,10);
	}
	else if(w > h){
		u8 asp = w / h;
		AGIDL_WriteByte(file,AGIDL_GetNumChar(asp));
		AGIDL_WriteByte(file,' ');
		AGIDL_WriteByte(file,'1');
		AGIDL_WriteByte(file,10);
	}
	else{
		u8 asp = h / w;
		AGIDL_WriteByte(file,'1');
		AGIDL_WriteByte(file,' ');
		AGIDL_WriteByte(file,AGIDL_GetNumChar(asp));
		AGIDL_WriteByte(file,10);
	}
}

void AGIDL_3DFEncodeICP(AGIDL_3DF* glide, FILE* file){
	if(glide->encode == ICP_ENCODE_THRESHOLD){
		int pass = 0;
		u8 pal_index = 0;
		u32 w = AGIDL_3DFGetWidth(glide), h = AGIDL_3DFGetHeight(glide);
		
		AGIDL_InitICP(&glide->palette, AGIDL_ICP_256);
		
		int x,y;
		for(y = 0; y < h; y++){
			for(x = 0; x < w; x++){
				COLOR clr = AGIDL_3DFGetClr(glide,x,y);
	
				AGIDL_AddColorICP(&glide->palette,pal_index,clr,AGIDL_3DFGetClrFmt(glide),AGIDL_3DFGetMaxDiff(glide),&pass);
				
				if(pass == 1 && pal_index < 256){
					pal_index++;
				}
				
				pass = 0;
			}
		}
	}
	else{
		AGIDL_EncodeHistogramICP(&glide->palette,glide->pixels.pix32,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
	}
}

void AGIDL_3DFEncodeIMG(AGIDL_3DF* glide, FILE* file){
	if(glide->mipped != TRUE && glide->icp != TRUE){
		switch(AGIDL_3DFGetClrFmt(glide)){
			case AGIDL_RGB_565:{
				AGIDL_InitBigEndArch();
				AGIDL_WriteBufRGB16(file,glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide));
				AGIDL_DisableBigEndArch();
			}break;
			case AGIDL_RGB_555:{
				AGIDL_InitBigEndArch();
				AGIDL_WriteBufRGB16(file,glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide));
				AGIDL_DisableBigEndArch();
			}break;
			case AGIDL_RGBA_8888:{
				u32 size = AGIDL_3DFGetSize(glide);
				int i;
				for(i = 0; i < size; i++){
					COLOR clr = glide->pixels.pix32[i];
					u8 r = AGIDL_GetR(clr,AGIDL_RGBA_8888);
					u8 g = AGIDL_GetG(clr,AGIDL_RGBA_8888);
					u8 b = AGIDL_GetB(clr,AGIDL_RGBA_8888);
					u8 a = AGIDL_GetA(clr,AGIDL_RGBA_8888);
					
					r >>= 4;
					g >>= 4;
					b >>= 4;
					a >>= 4;
					
					u8 byte1 = a << 4 | r, byte2 = g << 4 | b;
					
					AGIDL_WriteByte(file,byte1); AGIDL_WriteByte(file,byte2);
				}
			}break;
			case AGIDL_ARGB_8888:{
				u32 size = AGIDL_3DFGetSize(glide);
				int i;
				for(i = 0; i < size; i++){
					COLOR clr = glide->pixels.pix32[i];
					u8 r = AGIDL_GetR(clr,AGIDL_ARGB_8888);
					u8 g = AGIDL_GetG(clr,AGIDL_ARGB_8888);
					u8 b = AGIDL_GetB(clr,AGIDL_ARGB_8888);
					u8 a = AGIDL_GetA(clr,AGIDL_ARGB_8888);
					
					r >>= 4;
					g >>= 4;
					b >>= 4;
					a >>= 4;
					
					u8 byte1 = a << 4 | r, byte2 = g << 4 | b;
					
					AGIDL_WriteByte(file,byte1); AGIDL_WriteByte(file,byte2);
				}
			}break;
		}
	}
	else if(glide->mipped == TRUE && glide->icp != TRUE){
		switch(AGIDL_3DFGetClrFmt(glide)){
			case AGIDL_RGB_565:{
				AGIDL_InitBigEndArch();
				AGIDL_WriteBufRGB16(file,glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide));
				glide->mipmap = AGIDL_GenerateMipmapFromImgData(glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
				int w = AGIDL_3DFGetWidth(glide), h = AGIDL_3DFGetHeight(glide);
				int i;
				for(i = 1; i < glide->mipmap->mipcount; i++){
					w >>= 1;
					h >>= 1;
					AGIDL_WriteBufRGB16(file,glide->mipmap->mipmap[i].img_data,w,h);
				}
				AGIDL_WriteShort(file,0);
				AGIDL_DisableBigEndArch();
			}break;
			case AGIDL_RGB_555:{
				AGIDL_InitBigEndArch();
				AGIDL_WriteBufRGB16(file,glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide));
				glide->mipmap = AGIDL_GenerateMipmapFromImgData(glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
				int w = AGIDL_3DFGetWidth(glide), h = AGIDL_3DFGetHeight(glide);
				int i;
				for(i = 1; i < glide->mipmap->mipcount; i++){
					w >>= 1;
					h >>= 1;
					AGIDL_WriteBufRGB16(file,glide->mipmap->mipmap[i].img_data,w,h);
				}
				AGIDL_WriteShort(file,0);
				AGIDL_DisableBigEndArch();
			}break;
			case AGIDL_RGBA_8888:{
				glide->mipmap = AGIDL_GenerateMipmapFromImgData(glide->pixels.pix32,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
				int w = AGIDL_3DFGetWidth(glide), h = AGIDL_3DFGetHeight(glide);
				int i,j;
				for(i = 0; i < glide->mipmap->mipcount; i++){
					for(j = 0; j < w*h; j++){
						COLOR* img = (COLOR*)glide->mipmap->mipmap[i].img_data;
						COLOR clr = img[j];
						u8 r = AGIDL_GetR(clr,AGIDL_RGBA_8888);
						u8 g = AGIDL_GetG(clr,AGIDL_RGBA_8888);
						u8 b = AGIDL_GetB(clr,AGIDL_RGBA_8888);
						u8 a = AGIDL_GetA(clr,AGIDL_RGBA_8888);
						
						r >>= 4;
						g >>= 4;
						b >>= 4;
						a >>= 4;
						
						u8 byte1 = a << 4 | r, byte2 = g << 4 | b;
						
						AGIDL_WriteByte(file,byte1); AGIDL_WriteByte(file,byte2);
					}
					
					w >>= 1;
					h >>= 1;
				}
				AGIDL_WriteShort(file,0);
			}break;
			case AGIDL_ARGB_8888:{
				glide->mipmap = AGIDL_GenerateMipmapFromImgData(glide->pixels.pix32,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
				int w = AGIDL_3DFGetWidth(glide), h = AGIDL_3DFGetHeight(glide);
				int i,j;
				for(i = 0; i < glide->mipmap->mipcount; i++){
					for(j = 0; j < w*h; j++){
						COLOR* img = (COLOR*)glide->mipmap->mipmap[i].img_data;
						COLOR clr = img[j];
						u8 r = AGIDL_GetR(clr,AGIDL_ARGB_8888);
						u8 g = AGIDL_GetG(clr,AGIDL_ARGB_8888);
						u8 b = AGIDL_GetB(clr,AGIDL_ARGB_8888);
						u8 a = AGIDL_GetA(clr,AGIDL_ARGB_8888);
						
						r >>= 4;
						g >>= 4;
						b >>= 4;
						a >>= 4;
						
						u8 byte1 = a << 4 | r, byte2 = g << 4 | b;
						
						AGIDL_WriteByte(file,byte1); AGIDL_WriteByte(file,byte2);
					}
					
					w >>= 1;
					h >>= 1;
				}
				AGIDL_WriteShort(file,0);
			}break;
		}
	}
	else if(glide->mipped != TRUE && glide->icp == TRUE){
		int i;
		for(i = 0; i < 256; i++){
			COLOR clr = glide->palette.icp.palette_256[i];
			
			u8 r = AGIDL_GetR(clr,AGIDL_3DFGetClrFmt(glide));
			u8 g = AGIDL_GetG(clr,AGIDL_3DFGetClrFmt(glide));
			u8 b = AGIDL_GetB(clr,AGIDL_3DFGetClrFmt(glide));
			u8 a = AGIDL_GetA(clr,AGIDL_3DFGetClrFmt(glide));
			
			AGIDL_WriteByte(file,a);
			AGIDL_WriteByte(file,r);
			AGIDL_WriteByte(file,g);
			AGIDL_WriteByte(file,b);
		}
		
		for(i = 0; i < AGIDL_3DFGetSize(glide); i++){
			COLOR clr = glide->pixels.pix32[i];
			u8 index = AGIDL_FindNearestColor(glide->palette,clr,AGIDL_3DFGetClrFmt(glide));
			AGIDL_WriteByte(file,index);
		}
	}
	else{
		glide->mipmap = AGIDL_GenerateMipmapFromImgData(glide->pixels.pix32,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
		
		int i;
		for(i = 0; i < 256; i++){
			COLOR clr = glide->palette.icp.palette_256[i];
			
			u8 r = AGIDL_GetR(clr,AGIDL_3DFGetClrFmt(glide));
			u8 g = AGIDL_GetG(clr,AGIDL_3DFGetClrFmt(glide));
			u8 b = AGIDL_GetB(clr,AGIDL_3DFGetClrFmt(glide));
			u8 a = AGIDL_GetA(clr,AGIDL_3DFGetClrFmt(glide));
			
			AGIDL_WriteByte(file,a);
			AGIDL_WriteByte(file,r);
			AGIDL_WriteByte(file,g);
			AGIDL_WriteByte(file,b);
		}
		
		int w = AGIDL_3DFGetWidth(glide), h = AGIDL_3DFGetHeight(glide);
		
		int j;
		for(i = 0; i < glide->mipmap->mipcount; i++){
			for(j = 0; j < w*h; j++){
				COLOR* img = (COLOR*)glide->mipmap->mipmap[i].img_data;
				COLOR clr = img[j];
				u8 index = AGIDL_FindNearestColor(glide->palette,clr,AGIDL_3DFGetClrFmt(glide));
				AGIDL_WriteByte(file,index);
			}
			w >>= 1;
			h >>= 1;
		}
		AGIDL_WriteShort(file,0);
	}
}

AGIDL_3DF * AGIDL_Load3DF(char* filename){
	FILE* file = fopen(filename,"rb");
	
	if(file == NULL){
		printf("%s - %s!\n",AGIDL_Error2Str(FILE_NOT_LOCATED_IMG_ERROR),filename);
		return NULL;
	}
	
	AGIDL_3DF* glide = (AGIDL_3DF*)malloc(sizeof(AGIDL_3DF));
	glide->filename = (char*)malloc(sizeof(filename)+1);
	AGIDL_FilenameCpy(glide->filename,filename);
	
	if(glide == NULL || glide->filename == NULL){
		printf("%s\n",AGIDL_Error2Str(MEMORY_IMG_ERROR));
		return NULL;
	}
	
	int error = AGIDL_3DFDecodeHeader(glide,file);
	
	if(error != NO_IMG_ERROR){
		printf("%s - %s!\n",AGIDL_Error2Str(error),filename);
		return NULL;
	}
	
	AGIDL_3DFDecodeIMG(glide,file);
	
	fclose(file);
	
	return glide;
}

void AGIDL_Export3DF(AGIDL_3DF* glide){
	FILE* file = fopen(glide->filename,"wb");
	
	if(file == NULL){
		printf("Could not open/create .3DF -> %s!\n",glide->filename);
		return;
	}
	
	AGIDL_3DFEncodeHeader(glide,file);
	
	if(glide->icp != TRUE){
		
		if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 16){
			glide->pixels.pix16 = (COLOR16*)AGIDL_NearestPow2ScaleImgData(glide->pixels.pix16,&glide->width,&glide->height,256,AGIDL_3DFGetClrFmt(glide));
		}
		else{
			glide->pixels.pix32 = (COLOR*)AGIDL_NearestPow2ScaleImgData(glide->pixels.pix32,&glide->width,&glide->height,256,AGIDL_3DFGetClrFmt(glide));
		}
		
		switch(AGIDL_3DFGetClrFmt(glide)){
			case AGIDL_RGB_555:{
				AGIDL_3DFEncodeIMG(glide,file);
			}break;
			case AGIDL_RGB_565:{
				AGIDL_3DFEncodeIMG(glide,file);
			}break;
			case AGIDL_BGR_555:{
				AGIDL_3DFBGR2RGB(glide);
				AGIDL_3DFEncodeIMG(glide,file);
				AGIDL_3DFRGB2BGR(glide);
			}break;
			case AGIDL_BGR_565:{
				AGIDL_3DFBGR2RGB(glide);
				AGIDL_3DFEncodeIMG(glide,file);
				AGIDL_3DFRGB2BGR(glide);
			}break;
			case AGIDL_RGB_888:{
				AGIDL_3DFConvert24BPPTO16BPP(glide);
				AGIDL_3DFConvert555TO565(glide);
				AGIDL_3DFEncodeIMG(glide,file);
				AGIDL_3DFConvert565TO555(glide);
				AGIDL_3DFConvert16BPPTO24BPP(glide);
			}break;
			case AGIDL_BGR_888:{
				AGIDL_3DFBGR2RGB(glide);
				AGIDL_3DFConvert24BPPTO16BPP(glide);
				AGIDL_3DFConvert555TO565(glide);
				AGIDL_3DFEncodeIMG(glide,file);
				AGIDL_3DFConvert565TO555(glide);
				AGIDL_3DFConvert16BPPTO24BPP(glide);
				AGIDL_3DFRGB2BGR(glide);
			}break;
			case AGIDL_RGBA_8888:{
				AGIDL_3DFEncodeIMG(glide,file);
			}break;
			case AGIDL_ARGB_8888:{
				AGIDL_3DFEncodeIMG(glide,file);
			}break;
		}
	}
	else{
		switch(AGIDL_3DFGetClrFmt(glide)){
			case AGIDL_RGB_555:{
				AGIDL_3DFConvert16BPPTO24BPP(glide);
				glide->pixels.pix32 = (COLOR*)AGIDL_NearestPow2ScaleImgData(glide->pixels.pix32,&glide->width,&glide->height,256,AGIDL_3DFGetClrFmt(glide));
				AGIDL_3DFConvertRGB2RGBA(glide);
				AGIDL_3DFEncodeICP(glide,file);
				AGIDL_3DFEncodeIMG(glide,file);
				AGIDL_3DFConvertRGBA2RGB(glide);
				AGIDL_3DFConvert24BPPTO16BPP(glide);
			}break;
			case AGIDL_RGB_565:{
				AGIDL_3DFConvert16BPPTO24BPP(glide);
				glide->pixels.pix32 = (COLOR*)AGIDL_NearestPow2ScaleImgData(glide->pixels.pix32,&glide->width,&glide->height,256,AGIDL_3DFGetClrFmt(glide));
				AGIDL_3DFConvertRGB2RGBA(glide);
				AGIDL_3DFEncodeICP(glide,file);
				AGIDL_3DFEncodeIMG(glide,file);
				AGIDL_3DFConvertRGBA2RGB(glide);
				AGIDL_3DFConvert24BPPTO16BPP(glide);
			}break;
			case AGIDL_BGR_555:{
				AGIDL_3DFBGR2RGB(glide);
				AGIDL_3DFConvert16BPPTO24BPP(glide);
				glide->pixels.pix32 = (COLOR*)AGIDL_NearestPow2ScaleImgData(glide->pixels.pix32,&glide->width,&glide->height,256,AGIDL_3DFGetClrFmt(glide));
				AGIDL_3DFConvertRGB2RGBA(glide);
				AGIDL_3DFEncodeICP(glide,file);
				AGIDL_3DFEncodeIMG(glide,file);
				AGIDL_3DFConvertRGBA2RGB(glide);
				AGIDL_3DFConvert24BPPTO16BPP(glide);
				AGIDL_3DFRGB2BGR(glide);
			}break;
			case AGIDL_BGR_565:{
				AGIDL_3DFBGR2RGB(glide);
				AGIDL_3DFConvert16BPPTO24BPP(glide);
				glide->pixels.pix32 = (COLOR*)AGIDL_NearestPow2ScaleImgData(glide->pixels.pix32,&glide->width,&glide->height,256,AGIDL_3DFGetClrFmt(glide));
				AGIDL_3DFConvertRGB2RGBA(glide);
				AGIDL_3DFEncodeICP(glide,file);
				AGIDL_3DFEncodeIMG(glide,file);
				AGIDL_3DFConvertRGBA2RGB(glide);
				AGIDL_3DFConvert24BPPTO16BPP(glide);
				AGIDL_3DFRGB2BGR(glide);
			}break;
			case AGIDL_RGB_888:{
				glide->pixels.pix32 = (COLOR*)AGIDL_NearestPow2ScaleImgData(glide->pixels.pix32,&glide->width,&glide->height,256,AGIDL_3DFGetClrFmt(glide));
				AGIDL_3DFConvertRGB2RGBA(glide);
				AGIDL_3DFEncodeICP(glide,file);
				AGIDL_3DFEncodeIMG(glide,file);
				AGIDL_3DFConvertRGBA2RGB(glide);
			}break;
			case AGIDL_BGR_888:{
				AGIDL_3DFBGR2RGB(glide);
				glide->pixels.pix32 = (COLOR*)AGIDL_NearestPow2ScaleImgData(glide->pixels.pix32,&glide->width,&glide->height,256,AGIDL_3DFGetClrFmt(glide));
				AGIDL_3DFConvertRGB2RGBA(glide);
				AGIDL_3DFEncodeICP(glide,file);
				AGIDL_3DFEncodeIMG(glide,file);
				AGIDL_3DFConvertRGBA2RGB(glide);
				AGIDL_3DFRGB2BGR(glide);
			}break;
			case AGIDL_RGBA_8888:{
				glide->pixels.pix32 = (COLOR*)AGIDL_NearestPow2ScaleImgData(glide->pixels.pix32,&glide->width,&glide->height,256,AGIDL_3DFGetClrFmt(glide));
				AGIDL_3DFEncodeICP(glide,file);
				AGIDL_3DFEncodeIMG(glide,file);
			}break;
			case AGIDL_ARGB_8888:{
				glide->pixels.pix32 = (COLOR*)AGIDL_NearestPow2ScaleImgData(glide->pixels.pix32,&glide->width,&glide->height,256,AGIDL_3DFGetClrFmt(glide));
				AGIDL_3DFEncodeICP(glide,file);
				AGIDL_3DFEncodeIMG(glide,file);
			}break;
		}
	}
	
}