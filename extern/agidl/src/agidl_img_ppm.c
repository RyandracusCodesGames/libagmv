/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_ppm.c
*   Date: 2/19/2024
*   Version: 0.4b
*   Updated: 6/10/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <stdlib.h>
#include <string.h>
#include <agidl_img_ppm.h>
#include <agidl_img_error.h>
#include <agidl_file_utils.h>
#include <agidl_mmu_utils.h>
#include <agidl_cc_manager.h>
#include <agidl_cc_converter.h>

void AGIDL_SetPPMFilename(AGIDL_PPM* ppm, const char* filename){
	if(ppm->filename != NULL){
		free(ppm->filename);
		ppm->filename = (char*)malloc(strlen(filename)+1);
		AGIDL_FilenameCpy(ppm->filename,filename);
	}
	else{
		ppm->filename = (char*)malloc(strlen(filename)+1);
		AGIDL_FilenameCpy(ppm->filename,filename);
	}
}

void AGIDL_PPMSetWidth(AGIDL_PPM* ppm, u32 width){
	ppm->width = width;
}

void AGIDL_PPMSetHeight(AGIDL_PPM* ppm, u32 height){
	ppm->height = height;
}

void AGIDL_PPMSetClrFmt(AGIDL_PPM* ppm, AGIDL_CLR_FMT fmt){
	ppm->fmt = fmt;
}

void AGIDL_PPMSetType(AGIDL_PPM* ppm, AGIDL_PPM_TYPE type){
	ppm->type = type;
}

void AGIDL_PPMSetMaxDiff(AGIDL_PPM* ppm, int max_diff){
	ppm->max_diff = max_diff;
}

void AGIDL_PPMSetClr(AGIDL_PPM* ppm, int x, int y, COLOR clr){
	if(x >= 0 && y >= 0 && x < AGIDL_PPMGetWidth(ppm) && y < AGIDL_PPMGetHeight(ppm)){
		ppm->pixels.pix32[x+y*AGIDL_PPMGetWidth(ppm)] = clr;
	}
}

void AGIDL_PPMSetClr16(AGIDL_PPM* ppm, int x, int y, COLOR16 clr){
	if(x >= 0 && y >= 0 && x < AGIDL_PPMGetWidth(ppm) && y < AGIDL_PPMGetHeight(ppm)){
		ppm->pixels.pix16[x+y*AGIDL_PPMGetWidth(ppm)] = clr;
	}
}

void AGIDL_PPMSetRGB(AGIDL_PPM* ppm, int x, int y, u8 r, u8 g, u8 b){
	if(x >= 0 && y >= 0 && x < AGIDL_PPMGetWidth(ppm) && y < AGIDL_PPMGetHeight(ppm)){
		int bits = AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm));
		if(bits == 24 || bits == 32){
			ppm->pixels.pix32[x+y*AGIDL_PPMGetWidth(ppm)] = AGIDL_RGB(r,g,b,AGIDL_PPMGetClrFmt(ppm));
		}
		else{
			ppm->pixels.pix16[x+y*AGIDL_PPMGetWidth(ppm)] = AGIDL_RGB16(r,g,b,AGIDL_PPMGetClrFmt(ppm));
		}
	}
}

void AGIDL_PPMSetRGBA(AGIDL_PPM* ppm, int x, int y, u8 r, u8 g, u8 b, u8 a){
	if(x >= 0 && y >= 0 && x < AGIDL_PPMGetWidth(ppm) && y < AGIDL_PPMGetHeight(ppm)){
		if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 32){
			ppm->pixels.pix32[x+y*AGIDL_PPMGetWidth(ppm)] = AGIDL_RGBA(r,g,b,a,AGIDL_PPMGetClrFmt(ppm));
		}
	}
}
void AGIDL_PPMSetColor3f(AGIDL_PPM* ppm, int x, int y, f32 r, f32 g, f32 b){
if(x >= 0 && y >= 0 && x < AGIDL_PPMGetWidth(ppm) && y < AGIDL_PPMGetHeight(ppm)){
		int bits = AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm));
		if(bits == 24 || bits == 32){
			ppm->pixels.pix32[x+y*AGIDL_PPMGetWidth(ppm)] = AGIDL_Color3f(r,g,b,AGIDL_PPMGetClrFmt(ppm));
		}
		else{
			ppm->pixels.pix16[x+y*AGIDL_PPMGetWidth(ppm)] = AGIDL_Color3f(r,g,b,AGIDL_PPMGetClrFmt(ppm));
		}
	}
}

void AGIDL_PPMSetColor4f(AGIDL_PPM* ppm, int x, int y, f32 r, f32 g, f32 b, f32 a){
	if(x >= 0 && y >= 0 && x < AGIDL_PPMGetWidth(ppm) && y < AGIDL_PPMGetHeight(ppm)){
		if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 32){
			ppm->pixels.pix32[x+y*AGIDL_PPMGetWidth(ppm)] = AGIDL_Color4f(r,g,b,a,AGIDL_PPMGetClrFmt(ppm));
		}
	}
}

u32 AGIDL_PPMGetWidth(AGIDL_PPM* ppm){
	return ppm->width;
}

u32 AGIDL_PPMGetHeight(AGIDL_PPM* ppm){
	return ppm->height;
}

u32 AGIDL_PPMGetSize(AGIDL_PPM* ppm){
	return AGIDL_PPMGetWidth(ppm) * AGIDL_PPMGetHeight(ppm);
}

AGIDL_CLR_FMT AGIDL_PPMGetClrFmt(AGIDL_PPM* ppm){
	return ppm->fmt;
}

int AGIDL_PPMGetMaxDiff(AGIDL_PPM* ppm){
	return ppm->max_diff;
}

COLOR AGIDL_PPMGetClr(AGIDL_PPM* ppm, int x, int y){
	if(x >= 0 && y >= 0 && x < AGIDL_PPMGetWidth(ppm) && y < AGIDL_PPMGetHeight(ppm)){
		return ppm->pixels.pix32[x+y*AGIDL_PPMGetWidth(ppm)];
	}
	else return 0;
}

COLOR16 AGIDL_PPMGetClr16(AGIDL_PPM* ppm, int x, int y){
	if(x >= 0 && y >= 0 && x < AGIDL_PPMGetWidth(ppm) && y < AGIDL_PPMGetHeight(ppm)){
		return ppm->pixels.pix16[x+y*AGIDL_PPMGetWidth(ppm)];
	}
	else return 0;
}

void AGIDL_ClearPPM(AGIDL_PPM *ppm, COLOR clr){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) != 16){
		AGIDL_ClrMemset(ppm->pixels.pix32,clr,AGIDL_PPMGetSize(ppm));
	}
	else{
		AGIDL_ClrMemset16(ppm->pixels.pix16,(COLOR16)clr,AGIDL_PPMGetSize(ppm));
	}
}

void AGIDL_ClearPPM16(AGIDL_PPM *ppm, COLOR16 clr){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 16){
		AGIDL_ClrMemset16(ppm->pixels.pix16,clr,AGIDL_PPMGetSize(ppm));
	}
	else{
		AGIDL_ClrMemset(ppm->pixels.pix32,AGIDL_CLR16_TO_CLR(clr,AGIDL_RGB_555,AGIDL_PPMGetClrFmt(ppm)),AGIDL_PPMGetSize(ppm));
	}
}

void AGIDL_ClearColorPPM(AGIDL_PPM* ppm, float r, float g, float b){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 16){
		AGIDL_ClearColorBuffer(ppm->pixels.pix16,r,g,b,AGIDL_PPMGetClrFmt(ppm),AGIDL_PPMGetSize(ppm));
	}
	else{
		AGIDL_ClearColorBuffer(ppm->pixels.pix32,r,g,b,AGIDL_PPMGetClrFmt(ppm),AGIDL_PPMGetSize(ppm));
	}
}

void AGIDL_FlushPPM(AGIDL_PPM* ppm){
	AGIDL_ClearPPM(ppm,0);
}

void AGIDL_PPMSyncPix(AGIDL_PPM *ppm, COLOR *clrs){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) != 16){
		AGIDL_ClrMemcpy(ppm->pixels.pix32,clrs,AGIDL_PPMGetSize(ppm));
	}
}

void AGIDL_PPMSyncPix16(AGIDL_PPM *ppm, COLOR16 *clrs){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 16){
		AGIDL_ClrMemcpy16(ppm->pixels.pix16,clrs,AGIDL_PPMGetSize(ppm));
	}
}

void AGIDL_PPMCopyPix(AGIDL_PPM* ppm, COLOR* clrs, u32 count){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) != 16){
		AGIDL_ClrMemcpy(ppm->pixels.pix32,clrs,count);
	}
}

void AGIDL_PPMCopyPix16(AGIDL_PPM* ppm, COLOR16* clrs, u32 count){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 16){
		AGIDL_ClrMemcpy16(ppm->pixels.pix16,clrs,count);
	}
}

void AGIDL_FreePPM(AGIDL_PPM* ppm){
	if(ppm->filename != NULL){
		free(ppm->filename);
		ppm->filename = NULL;
	}
	
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 16){
		if(ppm->pixels.pix16 != NULL){
			free(ppm->pixels.pix16);
			ppm->pixels.pix16 = NULL;
		}
	}
	else{
		if(ppm->pixels.pix32 != NULL){
			free(ppm->pixels.pix32);
			ppm->pixels.pix32 = NULL;
		}
	}
	
	if(ppm != NULL){
		free(ppm);
		ppm = NULL;
	}
}

void AGIDL_PPMRGB2BGR(AGIDL_PPM *ppm){
	if(ppm->fmt == AGIDL_RGB_888){
		AGIDL_RGB2BGR(ppm->pixels.pix32,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),&ppm->fmt);
	}
	if(ppm->fmt == AGIDL_RGB_555 || ppm->fmt == AGIDL_RGB_565){
		AGIDL_RGB2BGR16(ppm->pixels.pix16,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),&ppm->fmt);
	}
}

void AGIDL_PPMBGR2RGB(AGIDL_PPM* ppm){
	if(ppm->fmt == AGIDL_BGR_888){
		AGIDL_BGR2RGB(ppm->pixels.pix32,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),&ppm->fmt);
	}
	if(ppm->fmt == AGIDL_BGR_555 || ppm->fmt == AGIDL_BGR_565){
		AGIDL_BGR2RGB16(ppm->pixels.pix16,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),&ppm->fmt);
	}
}

void AGIDL_PPMConvert16BPPTO24BPP(AGIDL_PPM *ppm){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 16){
		ppm->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_PPMGetHeight(ppm)*AGIDL_PPMGetWidth(ppm)));
		AGIDL_16BPPTO24BPP(ppm->pixels.pix16,ppm->pixels.pix32,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),&ppm->fmt);
		free(ppm->pixels.pix16);
	}
}

void AGIDL_PPMConvert24BPPTO16BPP(AGIDL_PPM *ppm){
	if(ppm->fmt == AGIDL_RGB_888 || ppm->fmt == AGIDL_BGR_888 || ppm->fmt == AGIDL_RGBA_8888 || ppm->fmt == AGIDL_ARGB_8888){
		ppm->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_PPMGetHeight(ppm)*AGIDL_PPMGetWidth(ppm)));
		AGIDL_24BPPTO16BPP(ppm->pixels.pix32,ppm->pixels.pix16,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),&ppm->fmt);
		free(ppm->pixels.pix32);
	}
}

void AGIDL_PPMConvert555TO565(AGIDL_PPM* ppm){
	AGIDL_555TO565(ppm->pixels.pix16,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),&ppm->fmt);
}

void AGIDL_PPMConvert565TO555(AGIDL_PPM* ppm){
	AGIDL_565TO555(ppm->pixels.pix16,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),&ppm->fmt);
}

void AGIDL_PPMConvertRGB2RGBA(AGIDL_PPM* ppm){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 24){
		COLOR* cpy = AGIDL_AllocImgDataMMU(AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),AGIDL_PPMGetClrFmt(ppm));
		AGIDL_ClrMemcpy(cpy,ppm->pixels.pix32,AGIDL_PPMGetSize(ppm));
		AGIDL_ConvertRGB2RGBA(cpy,ppm->pixels.pix32,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),AGIDL_PPMGetClrFmt(ppm),AGIDL_RGBA_8888);
		AGIDL_PPMSetClrFmt(ppm,AGIDL_RGBA_8888);
		free(cpy);
	}
}

void AGIDL_PPMConvertRGBA2RGB(AGIDL_PPM* ppm){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 32){
		COLOR* cpy = AGIDL_AllocImgDataMMU(AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),AGIDL_PPMGetClrFmt(ppm));
		AGIDL_ClrMemcpy(cpy,ppm->pixels.pix32,AGIDL_PPMGetSize(ppm));
		AGIDL_ConvertRGBA2RGB(cpy,ppm->pixels.pix32,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),AGIDL_PPMGetClrFmt(ppm),AGIDL_RGB_888);
		AGIDL_PPMSetClrFmt(ppm,AGIDL_RGB_888);
		free(cpy);
	}
}

void AGIDL_ColorConvertPPM(AGIDL_PPM* ppm, AGIDL_CLR_FMT dest){
	u8 sbits = AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)), dbits = AGIDL_GetBitCount(dest);
	if(sbits == 16 && dbits == 16){
		AGIDL_ColorConvertImgData(ppm->pixels.pix16,NULL,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),AGIDL_PPMGetClrFmt(ppm),dest);
		AGIDL_PPMSetClrFmt(ppm,dest);
	}
	else if((sbits == 24 || sbits == 32) && (dbits == 24 || dbits == 32)){
		AGIDL_ColorConvertImgData(ppm->pixels.pix32,NULL,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),AGIDL_PPMGetClrFmt(ppm),dest);
		AGIDL_PPMSetClrFmt(ppm,dest);
	}
	else if(sbits == 16 && (dbits == 24 || dbits == 32)){
		ppm->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),dest);
		AGIDL_ColorConvertImgData(ppm->pixels.pix16,ppm->pixels.pix32,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),AGIDL_PPMGetClrFmt(ppm),dest);
		AGIDL_PPMSetClrFmt(ppm,dest);
		free(ppm->pixels.pix16);
	}
	else{
		ppm->pixels.pix16 = (COLOR16*)AGIDL_AllocImgDataMMU(AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),dest);
		AGIDL_ColorConvertImgData(ppm->pixels.pix32,ppm->pixels.pix16,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),AGIDL_PPMGetClrFmt(ppm),dest);
		AGIDL_PPMSetClrFmt(ppm,dest);
		free(ppm->pixels.pix32);
	}
}

int AGIDL_PPMDecodeHeader(AGIDL_PPM* ppm, FILE* file){
	u8 byte1 = AGIDL_ReadByte(file), byte2 = AGIDL_ReadByte(file);
	
	ppm->type = AGIDL_PPMGetType(byte1,byte2);
	
	fseek(file,1,SEEK_CUR);
	
	u32 pos = ftell(file);
	
	u32 count = 0, byte = AGIDL_ReadByte(file);
	
	char* comment = (char*)malloc(60);
	
	do{
		fgets(comment,60,file);
		byte = AGIDL_ReadByte(file);
	}while(byte == 0x23);
	
	free(comment);
	
	count = 0; byte = 0;
	u8 lod[4] = {0,0,0,0};
	
	fseek(file,pos,SEEK_SET);
	
	while(byte != 32 && count < 4){
		byte = AGIDL_ReadByte(file);
		if(byte != 32){
			lod[count++] = byte;
		}
	}
	
	switch(count){
		case 1:{
			ppm->width = AGIDL_PPMGetNum(lod[0]);
		}break;
		case 2:{
			ppm->width = (AGIDL_PPMGetNum(lod[0])*10)+(AGIDL_PPMGetNum(lod[1]));
		}break;
		case 3:{
			ppm->width = (AGIDL_PPMGetNum(lod[0])*100)+(AGIDL_PPMGetNum(lod[1])*10)+(AGIDL_PPMGetNum(lod[2]));
		}break;
		case 4:{
			ppm->width = (AGIDL_PPMGetNum(lod[0])*1000)+(AGIDL_PPMGetNum(lod[1])*100)+(AGIDL_PPMGetNum(lod[2])*10)+(AGIDL_PPMGetNum(lod[3]));
		}break;
		default: return INVALID_IMG_FORMATTING_ERROR;
	}
	
	count = 0; byte = 0;
	
	while(byte != 10 && count < 4){
		byte = AGIDL_ReadByte(file);
		if(byte != 10){
			lod[count++] = byte;
		}
	}
	
	switch(count){
		case 1:{
			ppm->height = AGIDL_PPMGetNum(lod[0]);
		}break;
		case 2:{
			ppm->height = (AGIDL_PPMGetNum(lod[0])*10)+(AGIDL_PPMGetNum(lod[1]));
		}break;
		case 3:{
			ppm->height = (AGIDL_PPMGetNum(lod[0])*100)+(AGIDL_PPMGetNum(lod[1])*10)+(AGIDL_PPMGetNum(lod[2]));
		}break;
		case 4:{
			ppm->height = (AGIDL_PPMGetNum(lod[0])*1000)+(AGIDL_PPMGetNum(lod[1])*100)+(AGIDL_PPMGetNum(lod[2])*10)+(AGIDL_PPMGetNum(lod[3]));
		}break;
		default: return INVALID_IMG_FORMATTING_ERROR;
	}
	
	count = 0; byte = 0;
	u8 bpp[3] = {0,0,0};
	
	while(byte != 10 && count < 3){
		byte = AGIDL_ReadByte(file);
		if(count != 10){
			bpp[count++] = byte;
		}
	}
	
	AGIDL_ReadByte(file);
	
	switch(count){
		case 1:{
			ppm->bpp = AGIDL_PPMGetNum(bpp[0]);
		}break;
		case 2:{
			ppm->bpp = (AGIDL_PPMGetNum(bpp[0])*10)+(AGIDL_PPMGetNum(bpp[1]));
		}break;
		case 3:{
			ppm->bpp = (AGIDL_PPMGetNum(bpp[0])*100)+(AGIDL_PPMGetNum(bpp[1])*10)+(AGIDL_PPMGetNum(bpp[2]));
		}break;
		default: return INVALID_IMG_FORMATTING_ERROR;
	}
	
	if(ppm->type == PPM_UNKNOWN){
		return INVALID_HEADER_FORMATTING_ERROR;
	}
	return NO_IMG_ERROR;
}

AGIDL_PPM_TYPE AGIDL_PPMGetType(u8 byte1, u8 byte2){
	if(byte1 == 'P' && byte2 == '6'){
		return PPM_BIN;
	}
	else if(byte1 == 'P' && byte2 == '3'){
		return PPM_ASCII;
	}
	else return PPM_UNKNOWN;
}

u8 AGIDL_PPMGetNum(char c){
	switch(c){
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
	}
	return 0;
}

void AGIDL_PPMDecodeIMG(AGIDL_PPM* ppm, FILE* file){
	if(ppm->type == PPM_BIN){
		AGIDL_PPMSetClrFmt(ppm,AGIDL_RGB_888);
		ppm->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),AGIDL_PPMGetClrFmt(ppm));
		
		float scale = 255.0f/ppm->bpp;
		
		int i;
		for(i = 0; i < AGIDL_PPMGetSize(ppm); i++){
			u8 r = AGIDL_ReadByte(file)*scale;
			u8 g = AGIDL_ReadByte(file)*scale;
			u8 b = AGIDL_ReadByte(file)*scale;
			ppm->pixels.pix32[i] = AGIDL_RGB(r,g,b,AGIDL_RGB_888);
		}
	}
	else{
		AGIDL_PPMSetClrFmt(ppm,AGIDL_RGB_888);
		ppm->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),AGIDL_PPMGetClrFmt(ppm));
		
		float scale = 255.0f/ppm->bpp;
		
		int i;
		for(i = 0; i < AGIDL_PPMGetSize(ppm); i++){
			int r = 0, g = 0, b = 0;
			fscanf(file,"%d %d %d\n",&r,&g,&b);
			r *= scale;
			g *= scale;
			b *= scale;
			ppm->pixels.pix32[i] = AGIDL_RGB(r,g,b,AGIDL_RGB_888);
		}
	}
}

AGIDL_PPM* AGIDL_LoadPPM(const char* filename){
	FILE* file = fopen(filename,"rb");
	
	if(file == NULL){
		printf("%s - %s\n",AGIDL_Error2Str(FILE_NOT_LOCATED_IMG_ERROR),filename);
		return NULL;
	}
	
	AGIDL_PPM* ppm = (AGIDL_PPM*)malloc(sizeof(AGIDL_PPM));
	ppm->filename = (char*)malloc(strlen(filename)+1);
	AGIDL_FilenameCpy(ppm->filename,filename);
	
	if(ppm == NULL || ppm->filename == NULL){
		printf("%s - %s\n",AGIDL_Error2Str(MEMORY_IMG_ERROR),filename);
		return NULL;
	}
	
	int error = AGIDL_PPMDecodeHeader(ppm,file);
	
	if(error != NO_IMG_ERROR){
		printf("%s - %s\n",AGIDL_Error2Str(error),filename);
		return NULL;
	}
	
	AGIDL_PPMDecodeIMG(ppm,file);
	
	fclose(file);
	
	return ppm;
}

AGIDL_PPM* AGIDL_CreatePPM(const char* filename, int width, int height, AGIDL_CLR_FMT fmt){
	AGIDL_PPM* ppm = (AGIDL_PPM*)malloc(sizeof(AGIDL_PPM));
	AGIDL_PPMSetWidth(ppm,width);
	AGIDL_PPMSetHeight(ppm,height);
	AGIDL_PPMSetClrFmt(ppm,fmt);
	ppm->filename = (char*)malloc(strlen(filename)+1);
	AGIDL_FilenameCpy(ppm->filename,filename);
	AGIDL_PPMSetType(ppm,PPM_BIN);
	AGIDL_PPMSetMaxDiff(ppm,128);
	
	if(AGIDL_GetBitCount(fmt) == 16){
		ppm->pixels.pix16 = (COLOR16*)AGIDL_AllocImgDataMMU(width,height,fmt);
	}
	else{
		ppm->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(width,height,fmt);
	}
	
	return ppm;
}

AGIDL_PPM* AGIDL_PPMCpyImg(AGIDL_PPM* ppm){
	AGIDL_PPM* ppmcpy = AGIDL_CreatePPM(ppm->filename,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),AGIDL_PPMGetClrFmt(ppm));
	
	if(AGIDL_PPMGetClrFmt(ppm) == 16){
		AGIDL_PPMSyncPix16(ppmcpy,ppm->pixels.pix16);
	}
	else{
		AGIDL_PPMSyncPix(ppmcpy,ppm->pixels.pix32);
	}
	
	return ppmcpy;
}

void AGIDL_PPMEncodeHeader(AGIDL_PPM* ppm, FILE* file){
	if(ppm->type == PPM_BIN){
		AGIDL_WriteByte(file,'P');
		AGIDL_WriteByte(file,'6');
		AGIDL_WriteByte(file,10);
	}
	else{
		AGIDL_WriteByte(file,'P');
		AGIDL_WriteByte(file,'3');
		AGIDL_WriteByte(file,10);
	}
	
	u32 w = AGIDL_PPMGetWidth(ppm);
	u32 h = AGIDL_PPMGetHeight(ppm);
	u8 bpp = 255;
	
	fprintf(file,"%ld",w);
	AGIDL_WriteByte(file,32);
	fprintf(file,"%ld",h);
	AGIDL_WriteByte(file,10);
	fprintf(file,"%d",bpp);
	AGIDL_WriteByte(file,10);
}

void AGIDL_PPMEncodeIMG(AGIDL_PPM* ppm, FILE* file){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) != 16){
		if(ppm->type == PPM_BIN){
			AGIDL_CLR_FMT fmt = AGIDL_PPMGetClrFmt(ppm);
			
			int i;
			for(i = 0; i < AGIDL_PPMGetSize(ppm); i++){
				u32 color = ppm->pixels.pix32[i];
				
				u8 r = AGIDL_GetR(color,fmt);
				u8 g = AGIDL_GetG(color,fmt);
				u8 b = AGIDL_GetB(color,fmt);
				
				AGIDL_WriteByte(file,r);
				AGIDL_WriteByte(file,g);
				AGIDL_WriteByte(file,b);
			}
		}
		else{
			AGIDL_CLR_FMT fmt = AGIDL_PPMGetClrFmt(ppm);
			
			int i;
			for(i = 0; i < AGIDL_PPMGetSize(ppm); i++){
				u32 color = ppm->pixels.pix32[i];
				
				u8 r = AGIDL_GetR(color,fmt);
				u8 g = AGIDL_GetG(color,fmt);
				u8 b = AGIDL_GetB(color,fmt);
				
				fprintf(file,"%d\n",r);
				fprintf(file,"%d\n",g);
				fprintf(file,"%d\n",b);
			}
		}
	}
}

void AGIDL_ExportPPM(AGIDL_PPM* ppm){
	FILE* file = fopen(ppm->filename,"wb");
	
	if(file == NULL){
		printf("Cannot open/create PPM image - %s!\n",ppm->filename);
		return;
	}
	
	AGIDL_PPMEncodeHeader(ppm,file);
	
	switch(AGIDL_PPMGetClrFmt(ppm)){
		case AGIDL_RGB_888:{
			AGIDL_PPMEncodeIMG(ppm,file);
		}break;
		case AGIDL_BGR_888:{
			AGIDL_PPMEncodeIMG(ppm,file);
		}break;
		case AGIDL_RGBA_8888:{
			AGIDL_PPMEncodeIMG(ppm,file);
		}break;
		case AGIDL_ARGB_8888:{
			AGIDL_PPMEncodeIMG(ppm,file);
		}break;
		case AGIDL_RGB_555:{
			AGIDL_PPMConvert16BPPTO24BPP(ppm);
			AGIDL_PPMEncodeIMG(ppm,file);
			AGIDL_PPMConvert24BPPTO16BPP(ppm);
		}break;
		case AGIDL_BGR_555:{
			AGIDL_PPMBGR2RGB(ppm);
			AGIDL_PPMConvert16BPPTO24BPP(ppm);
			AGIDL_PPMEncodeIMG(ppm,file);
			AGIDL_PPMConvert24BPPTO16BPP(ppm);
			AGIDL_PPMRGB2BGR(ppm);
		}break;
		case AGIDL_RGB_565:{
			AGIDL_PPMConvert565TO555(ppm);
			AGIDL_PPMConvert16BPPTO24BPP(ppm);
			AGIDL_PPMEncodeIMG(ppm,file);
			AGIDL_PPMConvert24BPPTO16BPP(ppm);
			AGIDL_PPMConvert555TO565(ppm);
		}break;
		case AGIDL_BGR_565:{
			AGIDL_PPMBGR2RGB(ppm);
			AGIDL_PPMConvert565TO555(ppm);
			AGIDL_PPMConvert16BPPTO24BPP(ppm);
			AGIDL_PPMEncodeIMG(ppm,file);
			AGIDL_PPMConvert24BPPTO16BPP(ppm);
			AGIDL_PPMConvert555TO565(ppm);
			AGIDL_PPMRGB2BGR(ppm);
		}break;
	}
	
	fclose(file);
}