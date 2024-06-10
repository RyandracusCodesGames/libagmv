/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_lbm.c
*   Date: 2/24/2024
*   Version: 0.3b
*   Updated: 2/25/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_img_lbm.h>

#include <stdlib.h>
#include <string.h>

#include <agidl_cc_converter.h>
#include <agidl_file_utils.h>
#include <agidl_img_compression.h>
#include <agidl_img_error.h>
#include <agidl_math_utils.h>
#include <agidl_mmu_utils.h>

void AGIDL_SetLBMFilename(AGIDL_LBM* lbm, const char* filename){
	lbm->filename = (char*)realloc(lbm->filename,strlen(filename));
	AGIDL_FilenameCpy(lbm->filename,filename);
}

void AGIDL_LBMSetWidth(AGIDL_LBM* lbm, const int width){
	lbm->header.bmhd.width = width;
}

void AGIDL_LBMSetHeight(AGIDL_LBM* lbm, const int height){
	lbm->header.bmhd.height = height;
}

void AGIDL_LBMSetClrFmt(AGIDL_LBM* lbm, const AGIDL_CLR_FMT fmt){
	lbm->fmt = fmt;
}

void AGIDL_LBMSetICPMode(AGIDL_LBM* lbm, const AGIDL_Bool icp){
	lbm->icp = icp;
}

void AGIDL_LBMSetICPEncoding(AGIDL_LBM* lbm, const AGIDL_ICP_ENCODE encode){
	lbm->encode = encode;
}

void AGIDL_LBMSetMaxDiff(AGIDL_LBM* lbm, const int max_diff){
	lbm->max_diff = max_diff;
}

void AGIDL_LBMSetCompression(AGIDL_LBM* lbm, const AGIDL_Bool compress){
	lbm->compress = compress;
}

void AGIDL_LBMSetType(AGIDL_LBM* lbm, const AGIDL_ILBM_TYPE type){
	lbm->type = type;
}

void AGIDL_LBMSetClr(const AGIDL_LBM* lbm, const int x, const int y, const COLOR clr){
	if(x >= 0 && y >= 0 && x < AGIDL_LBMGetWidth(lbm) && y < AGIDL_LBMGetHeight(lbm)){
		lbm->pixels.pix32[x+y*AGIDL_LBMGetWidth(lbm)] = clr;
	}
}

void AGIDL_LBMSetClr16(const AGIDL_LBM* lbm, const int x, const int y, const COLOR16 clr){
	if(x >= 0 && y >= 0 && x < AGIDL_LBMGetWidth(lbm) && y < AGIDL_LBMGetHeight(lbm)){
		lbm->pixels.pix16[x+y*AGIDL_LBMGetWidth(lbm)] = clr;
	}
}

void AGIDL_LBMSetRGB(const AGIDL_LBM* lbm, const int x, const int y, const u8 r, const u8 g, const u8 b){
	const AGIDL_CLR_FMT fmt = AGIDL_LBMGetClrFmt(lbm);
	switch(AGIDL_GetBitCount(fmt)){
		case 16:{
			AGIDL_LBMSetClr16(lbm,x,y,AGIDL_RGB16(r,g,b,fmt));
		}break;
		case 24:{
			AGIDL_LBMSetClr(lbm,x,y,AGIDL_RGB(r,g,b,fmt));
		}break;
		case 32:{
			AGIDL_LBMSetClr(lbm,x,y,AGIDL_RGBA(r,g,b,0xff,fmt));
		}break;
	}
}

void AGIDL_LBMSetRGBA(const AGIDL_LBM* lbm, const int x, const int y, const u8 r, const u8 g, const u8 b, const u8 a){
	const AGIDL_CLR_FMT fmt = AGIDL_LBMGetClrFmt(lbm);
	if(AGIDL_GetBitCount(fmt) == 32){
		AGIDL_LBMSetClr(lbm,x,y,AGIDL_RGBA(r,g,b,a,fmt));
	}
}

void AGIDL_LBMSetColor3f(const AGIDL_LBM* lbm, const int x, const int y, const f32 r, const f32 g, const f32 b){
	const AGIDL_CLR_FMT fmt = AGIDL_LBMGetClrFmt(lbm);
	switch(AGIDL_GetBitCount(fmt)){
		case 16:{
			AGIDL_LBMSetClr16(lbm,x,y,AGIDL_Color3f(r,g,b,fmt));
		}break;
		case 24:{
			AGIDL_LBMSetClr(lbm,x,y,AGIDL_Color3f(r,g,b,fmt));
		}break;
		case 32:{
			AGIDL_LBMSetClr(lbm,x,y,AGIDL_Color4f(r,g,b,1.0f,fmt));
		}break;
	}
}

void AGIDL_LBMSetColor4f(const AGIDL_LBM* lbm, const int x, const int y, const f32 r, const f32 g, const f32 b, const f32 a){
	const AGIDL_CLR_FMT fmt = AGIDL_LBMGetClrFmt(lbm);
	if(AGIDL_GetBitCount(fmt) == 32){
		AGIDL_LBMSetClr(lbm,x,y,AGIDL_Color4f(r,g,b,a,fmt));
	}
}

u32 AGIDL_LBMGetWidth(const AGIDL_LBM* lbm){
	return lbm->header.bmhd.width;
}

u32 AGIDL_LBMGetHeight(const AGIDL_LBM* lbm){
	return lbm->header.bmhd.height;
}

u32 AGIDL_LBMGetSize(const AGIDL_LBM* lbm){
	return AGIDL_LBMGetWidth(lbm) * AGIDL_LBMGetHeight(lbm);
}

AGIDL_CLR_FMT AGIDL_LBMGetClrFmt(const AGIDL_LBM* lbm){
	return lbm->fmt;
}

int AGIDL_LBMGetMaxDiff(const AGIDL_LBM* lbm){
	return lbm->max_diff;
}

AGIDL_ILBM_TYPE AGIDL_LBMGetType(const AGIDL_LBM* lbm){
	return lbm->type;
}

COLOR AGIDL_LBMGetClr(const AGIDL_LBM* lbm, const int x, const int y){
	if(x >= 0 && y >= 0 && x < AGIDL_LBMGetWidth(lbm) && y < AGIDL_LBMGetHeight(lbm)){
		return lbm->pixels.pix32[x+y*AGIDL_LBMGetWidth(lbm)];
	}
	return 0;
}

COLOR16 AGIDL_LBMGetClr16(const AGIDL_LBM* lbm, const int x, const int y){
	if(x >= 0 && y >= 0 && x < AGIDL_LBMGetWidth(lbm) && y < AGIDL_LBMGetHeight(lbm)){
		return lbm->pixels.pix16[x+y*AGIDL_LBMGetWidth(lbm)];
	}
	return 0;
}

void AGIDL_ClearLBM(const AGIDL_LBM *lbm, const COLOR clr){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) != 16){
		AGIDL_ClrMemset(lbm->pixels.pix32,clr,AGIDL_LBMGetSize(lbm));
	}
	else{
		AGIDL_ClrMemset16(lbm->pixels.pix16,clr,AGIDL_LBMGetSize(lbm));
	}
}

void AGIDL_ClearLBM16(const AGIDL_LBM *lbm, const COLOR16 clr){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 16){
		AGIDL_ClrMemset16(lbm->pixels.pix16,clr,AGIDL_LBMGetSize(lbm));
	}
	else{
		AGIDL_ClrMemset(lbm->pixels.pix32,AGIDL_CLR16_TO_CLR(clr,AGIDL_RGB_555,AGIDL_LBMGetClrFmt(lbm)),AGIDL_LBMGetSize(lbm));
	}
}

void AGIDL_ClearColorLBM(const AGIDL_LBM* lbm, const float r, const float g, const float b){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 16){
		AGIDL_ClearColorBuffer(lbm->pixels.pix16,r,g,b,AGIDL_LBMGetClrFmt(lbm),AGIDL_LBMGetSize(lbm));
	}
	else{
		AGIDL_ClearColorBuffer(lbm->pixels.pix32,r,g,b,AGIDL_LBMGetClrFmt(lbm),AGIDL_LBMGetSize(lbm));
	}
}

void AGIDL_FlushLBM(const AGIDL_LBM* lbm){
	AGIDL_ClearLBM(lbm,0);
}

void AGIDL_LBMSyncPix(const AGIDL_LBM *lbm, const COLOR *clrs){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) != 16){
		AGIDL_ClrMemcpy(lbm->pixels.pix32,clrs,AGIDL_LBMGetSize(lbm));
	}
}

void AGIDL_LBMSyncPix16(const AGIDL_LBM *lbm, const COLOR16 *clrs){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 16){
		AGIDL_ClrMemcpy16(lbm->pixels.pix16,clrs,AGIDL_LBMGetSize(lbm));
	}
}

void AGIDL_LBMCopyPix(const AGIDL_LBM* lbm, const COLOR* clrs, const u32 count){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) != 16){
		AGIDL_ClrMemcpy(lbm->pixels.pix32,clrs,count);
	}
}

void AGIDL_LBMCopyPix16(const AGIDL_LBM* lbm, const COLOR16* clrs, const u32 count){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 16){
		AGIDL_ClrMemcpy16(lbm->pixels.pix16,clrs,count);
	}
}

void AGIDL_LBMRGB2BGR(AGIDL_LBM *lbm){
	if(lbm->fmt == AGIDL_RGB_888){
		AGIDL_RGB2BGR(lbm->pixels.pix32,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),&lbm->fmt);
	}
	if(lbm->fmt == AGIDL_RGB_555 || lbm->fmt == AGIDL_RGB_565){
		AGIDL_RGB2BGR16(lbm->pixels.pix16,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),&lbm->fmt);
	}
}

void AGIDL_LBMBGR2RGB(AGIDL_LBM* lbm){
	if(lbm->fmt == AGIDL_BGR_888){
		AGIDL_BGR2RGB(lbm->pixels.pix32,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),&lbm->fmt);
	}
	if(lbm->fmt == AGIDL_BGR_555 || lbm->fmt == AGIDL_BGR_565){
		AGIDL_BGR2RGB16(lbm->pixels.pix16,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),&lbm->fmt);
	}
}

void AGIDL_LBMConvert16BPPTO24BPP(AGIDL_LBM *lbm){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 16){
		lbm->pixels.pix32 = (COLOR*)malloc(sizeof(COLOR)*(AGIDL_LBMGetHeight(lbm)*AGIDL_LBMGetWidth(lbm)));
		AGIDL_16BPPTO24BPP(lbm->pixels.pix16,lbm->pixels.pix32,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),&lbm->fmt);
		free(lbm->pixels.pix16);
	}
}

void AGIDL_LBMConvert24BPPTO16BPP(AGIDL_LBM *lbm){
	if(lbm->fmt == AGIDL_RGB_888 || lbm->fmt == AGIDL_BGR_888 || lbm->fmt == AGIDL_RGBA_8888 || lbm->fmt == AGIDL_ARGB_8888){
		lbm->pixels.pix16 = (COLOR16*)malloc(sizeof(COLOR16)*(AGIDL_LBMGetHeight(lbm)*AGIDL_LBMGetWidth(lbm)));
		AGIDL_24BPPTO16BPP(lbm->pixels.pix32,lbm->pixels.pix16,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),&lbm->fmt);
		free(lbm->pixels.pix32);
	}
}

void AGIDL_LBMConvert555TO565(AGIDL_LBM* lbm){
	AGIDL_555TO565(lbm->pixels.pix16,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),&lbm->fmt);
}

void AGIDL_LBMConvert565TO555(AGIDL_LBM* lbm){
	AGIDL_565TO555(lbm->pixels.pix16,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),&lbm->fmt);
}

void AGIDL_LBMConvertRGB2RGBA(AGIDL_LBM* lbm){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 24){
		COLOR* cpy = AGIDL_AllocImgDataMMU(AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),AGIDL_LBMGetClrFmt(lbm));
		AGIDL_ClrMemcpy(cpy,lbm->pixels.pix32,AGIDL_LBMGetSize(lbm));
		AGIDL_ConvertRGB2RGBA(cpy,lbm->pixels.pix32,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),AGIDL_LBMGetClrFmt(lbm),AGIDL_RGBA_8888);
		AGIDL_LBMSetClrFmt(lbm,AGIDL_RGBA_8888);
		free(cpy);
	}
}

void AGIDL_LBMConvertRGBA2RGB(AGIDL_LBM* lbm){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 32){
		COLOR* cpy = AGIDL_AllocImgDataMMU(AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),AGIDL_LBMGetClrFmt(lbm));
		AGIDL_ClrMemcpy(cpy,lbm->pixels.pix32,AGIDL_LBMGetSize(lbm));
		AGIDL_ConvertRGBA2RGB(cpy,lbm->pixels.pix32,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),AGIDL_LBMGetClrFmt(lbm),AGIDL_RGB_888);
		AGIDL_LBMSetClrFmt(lbm,AGIDL_RGB_888);
		free(cpy);
	}
}

void AGIDL_ColorConvertLBM(AGIDL_LBM* lbm, const AGIDL_CLR_FMT dest){
	const u8 sbits = AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)), dbits = AGIDL_GetBitCount(dest);
	if(sbits == 16 && dbits == 16){
		AGIDL_ColorConvertImgData(lbm->pixels.pix16,NULL,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),AGIDL_LBMGetClrFmt(lbm),dest);
		AGIDL_LBMSetClrFmt(lbm,dest);
	}
	else if((sbits == 24 || sbits == 32) && (dbits == 24 || dbits == 32)){
		AGIDL_ColorConvertImgData(lbm->pixels.pix32,NULL,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),AGIDL_LBMGetClrFmt(lbm),dest);
		AGIDL_LBMSetClrFmt(lbm,dest);
	}
	else if(sbits == 16 && (dbits == 24 || dbits == 32)){
		lbm->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),dest);
		AGIDL_ColorConvertImgData(lbm->pixels.pix16,lbm->pixels.pix32,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),AGIDL_LBMGetClrFmt(lbm),dest);
		AGIDL_LBMSetClrFmt(lbm,dest);
		free(lbm->pixels.pix16);
	}
	else{
		lbm->pixels.pix16 = (COLOR16*)AGIDL_AllocImgDataMMU(AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),dest);
		AGIDL_ColorConvertImgData(lbm->pixels.pix32,lbm->pixels.pix16,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),AGIDL_LBMGetClrFmt(lbm),dest);
		AGIDL_LBMSetClrFmt(lbm,dest);
		free(lbm->pixels.pix32);
	}
}

int  Align(const int l){
	if (l&1)
		return l+1;
	return l;
}


int AGIDL_LBMDecodeHeader(AGIDL_LBM* lbm, FILE* file){
	AGIDL_InitBigEndArch();

	lbm->header.form.formid[0] = AGIDL_ReadByte(file);
	lbm->header.form.formid[1] = AGIDL_ReadByte(file);
	lbm->header.form.formid[2] = AGIDL_ReadByte(file);
	lbm->header.form.formid[3] = AGIDL_ReadByte(file);

	lbm->header.form.size = AGIDL_ReadLong(file);

	lbm->header.form.ilbmid[0] = AGIDL_ReadByte(file);
	lbm->header.form.ilbmid[1] = AGIDL_ReadByte(file);
	lbm->header.form.ilbmid[2] = AGIDL_ReadByte(file);
	lbm->header.form.ilbmid[3] = AGIDL_ReadByte(file);

	lbm->header.bmhd.bmhdid[0] = AGIDL_ReadByte(file);
	lbm->header.bmhd.bmhdid[1] = AGIDL_ReadByte(file);
	lbm->header.bmhd.bmhdid[2] = AGIDL_ReadByte(file);
	lbm->header.bmhd.bmhdid[3] = AGIDL_ReadByte(file);

	lbm->header.bmhd.size = AGIDL_ReadLong(file);
	lbm->header.bmhd.width = AGIDL_ReadShort(file);
	lbm->header.bmhd.height = AGIDL_ReadShort(file);
	lbm->header.bmhd.x_off = AGIDL_ReadShort(file);
	lbm->header.bmhd.y_off = AGIDL_ReadShort(file);
	lbm->header.bmhd.bpp = AGIDL_ReadByte(file);
	lbm->header.bmhd.mask = AGIDL_ReadByte(file);
	lbm->header.bmhd.compress = AGIDL_ReadByte(file);
	lbm->header.bmhd.pad = AGIDL_ReadByte(file);
	lbm->header.bmhd.chromakey = AGIDL_ReadShort(file);
	lbm->header.bmhd.x_aspect = AGIDL_ReadByte(file);
	lbm->header.bmhd.y_aspect = AGIDL_ReadByte(file);
	lbm->header.bmhd.horz_res = AGIDL_ReadShort(file);
	lbm->header.bmhd.vert_res = AGIDL_ReadShort(file);

	u8 fourcc[4] = {0,0,0,0};

	do{
		fourcc[0] = AGIDL_ReadByte(file);
		fourcc[1] = AGIDL_ReadByte(file);
		fourcc[2] = AGIDL_ReadByte(file);
		fourcc[3] = AGIDL_ReadByte(file);

		u32 size = AGIDL_ReadLong(file);

		if(fourcc[0] == 'B' && fourcc[1] == 'O' && fourcc[2] == 'D' && fourcc[3] == 'Y'){
			lbm->header.body.size = size;
			lbm->header.body.src = (u8*)malloc(size);
			fread(lbm->header.body.src,1,size,file);

		}
		else{
			if(fourcc[0] == 'C' && fourcc[1] == 'M' && fourcc[2] == 'A' && fourcc[3] == 'P'){
				const u16 num_of_colors = AGIDL_Pow2(lbm->header.bmhd.bpp);

				if(num_of_colors <= 0 || num_of_colors > 256){
					AGIDL_DisableBigEndArch();
					return INVALID_HEADER_FORMATTING_ERROR;
				}

				for(int i = 0; i < num_of_colors; i++){
					const u8 r = AGIDL_ReadByte(file);
					const u8 g = AGIDL_ReadByte(file);
					const u8 b = AGIDL_ReadByte(file);

					lbm->header.cmap.palette.icp.palette_256[i] = AGIDL_RGB(r,g,b,AGIDL_RGB_888);
				}
			}
			else if(fourcc[0] == 'C' && fourcc[1] == 'L' && fourcc[2] == 'U' && fourcc[3] == 'T'){
				const u32 cluttype = AGIDL_ReadLong(file);
				AGIDL_ReadLong(file);

				lbm->clut = TRUE;

				switch(cluttype){
					case 0:{
						for(int i = 0; i < 256; i++){
							lbm->header.clut.ilut[i] = AGIDL_ReadByte(file);
						}
					}break;
					case 1:{
						for(int i = 0; i < 256; i++){
							lbm->header.clut.rlut[i] = AGIDL_ReadByte(file);
						}
					}break;
					case 2:{
						for(int i = 0; i < 256; i++){
							lbm->header.clut.glut[i] = AGIDL_ReadByte(file);
						}
					}break;
					case 3:{
						for(int i = 0; i < 256; i++){
							lbm->header.clut.blut[i] = AGIDL_ReadByte(file);
						}
					}break;
				}
			}
			else{
				size = Align(size);
				fseek(file,size,SEEK_CUR);
			}
		}

		if(fourcc[0] == 'T' || fourcc[1] != 'I' || fourcc[2] != 'N' || fourcc[3] != 'Y'){
			fourcc[0] = 'C';
			fourcc[1] = 'R';
			fourcc[2] = 'N';
			fourcc[3] = 'G';
		}

	}while(fourcc[0] != 'B' && fourcc[1] != 'O' && fourcc[2] != 'D' && fourcc[3] != 'Y' && ftell(file) < 6000);

	if(lbm->header.form.size % 2 != 0){
		AGIDL_ReadByte(file);
	}

	AGIDL_DisableBigEndArch();

	if(lbm->header.form.ilbmid[0] == 'I' && lbm->header.form.ilbmid[1] == 'L' && lbm->header.form.ilbmid[2] == 'B' && lbm->header.form.ilbmid[3]){
		AGIDL_LBMSetType(lbm,TYPE_ILBM);
	}
	else{
		AGIDL_LBMSetType(lbm,TYPE_PBM);
	}

	if(lbm->header.form.formid[0] != 'F' || lbm->header.form.formid[1] != 'O' || lbm->header.form.formid[2] != 'R'  || lbm->header.form.formid[3] != 'M'
	  || !(lbm->header.form.ilbmid[0] == 'I' || lbm->header.form.ilbmid[1] == 'L' || lbm->header.form.ilbmid[2] == 'B' || lbm->header.form.ilbmid[3] == 'M' ||
	  lbm->header.form.ilbmid[0] == 'P' || lbm->header.form.ilbmid[1] == 'B' || lbm->header.form.ilbmid[2] == 'M' || lbm->header.form.ilbmid[3] == ' ')){
		return INVALID_HEADER_FORMATTING_ERROR;
	}
	if(lbm->header.bmhd.bmhdid[0] != 'B' || lbm->header.bmhd.bmhdid[1] != 'M' || lbm->header.bmhd.bmhdid[2] != 'H' || lbm->header.bmhd.bmhdid[3] != 'D' ||
	   lbm->header.bmhd.size != 0x14){
		return INVALID_HEADER_FORMATTING_ERROR;
	}
	if(lbm->header.bmhd.bpp > 8 || lbm->header.bmhd.bpp < 1 || !(lbm->header.bmhd.compress == 0 || lbm->header.bmhd.compress == 1)){
		return INVALID_HEADER_FORMATTING_ERROR;
	}
	return NO_IMG_ERROR;
}

void AGIDL_LBMDecompressData(AGIDL_LBM* lbm, const u8* src, u8* dest){
	AGIDL_LBMSetWidth(lbm,(AGIDL_LBMGetWidth(lbm) + 15 ) & 0xFFFFFFF0);
	const u32 bytesperline = ((AGIDL_LBMGetWidth(lbm) + 15) / 16) * 2;
	if(lbm->header.bmhd.bpp != 1){
		AGIDL_PackBits(src,dest,AGIDL_LBMGetSize(lbm));
	}
	else{
		for(int y = 0; y < AGIDL_LBMGetHeight(lbm); y++){
			u32 count = 0;

			do{
				const u8 byte = *src++;

				if(byte > 128){
					const u8 nextbyte = *src++;

					for(int i = 0; i < 257 - byte; i++, count++){
						*dest++ = nextbyte;
					}
				}
				else if(byte < 128){
					for(int i = 0; i < byte + 1; i++, count++){
						*dest++ = *src++;
					}
				}
				else{
					count = bytesperline;
				}
			}while(count < bytesperline);
		}
	}
}

void AGIDL_LBMDeinterleaveData(const AGIDL_LBM* lbm, const u8* src, u8* dest){
	u8 bpp = lbm->header.bmhd.bpp;
	const u32 bytesperline = ((AGIDL_LBMGetWidth(lbm) + 15) / 16) * 2;
	if (lbm->header.bmhd.mask == 1) {
		bpp += 1;
	}

	for(int y = 0; y < AGIDL_LBMGetHeight(lbm); y++){
		for(int p = 0; p < bpp; p++){
			const u16 plane_mask = 1 << p;
			for(int i = 0; i < bytesperline; i++){
				const u32 bit_offset = (y * bpp * bytesperline) + (p * bytesperline) + i;
				const u8 bit_value = src[bit_offset];
				for(int b = 0; b < 8; b++){
					const u32 pixel_mask = 1 << (7 - b);
					if(bit_value & pixel_mask){
						const u32 x = (i * 8) + b;
						dest[(y * AGIDL_LBMGetWidth(lbm)) + x] |= plane_mask;
					}
				}
			}
		}
	}
}

void AGIDL_LBMDecodeIMG(AGIDL_LBM* lbm){
	if(lbm->header.bmhd.bpp != 24 && lbm->header.bmhd.bpp != 32){
		AGIDL_LBMSetClrFmt(lbm,AGIDL_RGB_888);
		lbm->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),AGIDL_RGB_888);

		if(lbm->header.bmhd.compress == 1){

			lbm->header.body.dest = (u8*)malloc(AGIDL_LBMGetSize(lbm));

			if(AGIDL_LBMGetType(lbm) == TYPE_ILBM){

				if(lbm->header.bmhd.bpp != 1){
					lbm->header.body.interleave = (u8*)malloc(AGIDL_LBMGetSize(lbm));
				}
				else{
					lbm->header.body.interleave = (u8*)malloc((AGIDL_LBMGetSize(lbm)+7)/8);
				}

				AGIDL_LBMDecompressData(lbm,lbm->header.body.src,lbm->header.body.interleave);
				AGIDL_LBMDeinterleaveData(lbm,lbm->header.body.interleave,lbm->header.body.dest);

				for(int i = 0; i < AGIDL_LBMGetSize(lbm); i++){
					lbm->pixels.pix32[i] = lbm->header.cmap.palette.icp.palette_256[lbm->header.body.dest[i]];
				}

				free(lbm->header.body.interleave);
				free(lbm->header.body.dest);
				free(lbm->header.body.src);
			}
			else{
				AGIDL_LBMDecompressData(lbm,lbm->header.body.src,lbm->header.body.dest);

				for(int i = 0; i < AGIDL_LBMGetSize(lbm); i++){
					lbm->pixels.pix32[i] = lbm->header.cmap.palette.icp.palette_256[lbm->header.body.dest[i]];
				}

				free(lbm->header.body.dest);
				free(lbm->header.body.src);
			}
		}
		else{
			lbm->header.body.dest = (u8*)malloc(AGIDL_LBMGetSize(lbm));

			if(AGIDL_LBMGetType(lbm) == TYPE_ILBM){
				AGIDL_LBMDeinterleaveData(lbm,lbm->header.body.src,lbm->header.body.dest);
			}
			else{
				memcpy(lbm->header.body.dest,lbm->header.body.src,AGIDL_LBMGetSize(lbm));
			}

			for(int i = 0; i < AGIDL_LBMGetSize(lbm); i++){
				lbm->pixels.pix32[i] = lbm->header.cmap.palette.icp.palette_256[lbm->header.body.dest[i]];
			}

			free(lbm->header.body.dest);
			free(lbm->header.body.src);
		}
	}
}

void AGIDL_FreeLBM(AGIDL_LBM* lbm){
	if(lbm != NULL){
		free(lbm->filename);
		if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 16){
			free(lbm->pixels.pix16);
		}
		else{
			free(lbm->pixels.pix32);
		}
		free(lbm);
		lbm = NULL;
	}
}

AGIDL_LBM * AGIDL_CreateLBM(const char* filename, const int width, const int height, const AGIDL_CLR_FMT fmt){
	AGIDL_LBM* lbm = malloc(sizeof(AGIDL_LBM));
	AGIDL_LBMSetWidth(lbm,width);
	AGIDL_LBMSetHeight(lbm,height);
	AGIDL_LBMSetClrFmt(lbm,fmt);
	lbm->filename = (char*)malloc(strlen(filename)+1);
	AGIDL_FilenameCpy(lbm->filename,filename);
	AGIDL_LBMSetMaxDiff(lbm,19);
	AGIDL_LBMSetICPMode(lbm,FALSE);
	AGIDL_LBMSetICPEncoding(lbm,ICP_ENCODE_HISTOGRAM);
	AGIDL_LBMSetCompression(lbm,FALSE);
	AGIDL_LBMSetType(lbm,TYPE_PBM);
	
	if(AGIDL_GetBitCount(fmt) == 16){
		lbm->pixels.pix16 = (COLOR16*)AGIDL_AllocImgDataMMU(width,height,fmt);
	}
	else{
		lbm->pixels.pix32 = (COLOR*)AGIDL_AllocImgDataMMU(width,height,fmt);
	}
	
	return lbm;
}

AGIDL_LBM* AGIDL_LBMCpyImg(const AGIDL_LBM* lbm){
	AGIDL_LBM* lbmcpy = AGIDL_CreateLBM("lbmcpy.lbm",AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),lbm->fmt);
	AGIDL_LBMSetICPMode(lbmcpy,lbm->icp);
	AGIDL_LBMSetCompression(lbmcpy,lbm->header.bmhd.compress);
	AGIDL_LBMSetMaxDiff(lbmcpy,AGIDL_LBMGetMaxDiff(lbm));
	AGIDL_LBMSetICPEncoding(lbmcpy,lbm->encode);
	AGIDL_LBMSetType(lbmcpy,lbm->type);
	
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 24 || AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 32){
		AGIDL_LBMSyncPix(lbmcpy,lbm->pixels.pix32);
	}
	else{
		AGIDL_LBMSyncPix16(lbmcpy,lbm->pixels.pix16);
	}
	return lbmcpy;
}

AGIDL_LBM * AGIDL_LoadLBM(char* filename){
	FILE* file = fopen(filename,"rb");
	
	if(file == NULL){
		printf("%s - %s!\n",AGIDL_Error2Str(FILE_NOT_LOCATED_IMG_ERROR),filename);
		return NULL;
	}
	
	AGIDL_LBM* lbm = malloc(sizeof(AGIDL_LBM));
	lbm->filename = (char*)malloc(strlen(filename)+1);
	AGIDL_FilenameCpy(lbm->filename,filename);
	
	if(lbm == NULL || lbm->filename == NULL){
		printf("%s\n",AGIDL_Error2Str(MEMORY_IMG_ERROR));
		return NULL;
	}

	const int error = AGIDL_LBMDecodeHeader(lbm,file);
	
	if(error != NO_IMG_ERROR){
		printf("%s - %s!\n",AGIDL_Error2Str(error),filename);
		return NULL;
	}
	
	AGIDL_LBMDecodeIMG(lbm);
	
	fclose(file);
	
	return lbm;
}

void AGIDL_LBMEncodeHeader(const AGIDL_LBM* lbm, FILE* file){
	AGIDL_InitBigEndArch();
	
	AGIDL_PrintFourCC(file,'F','O','R','M');
	AGIDL_WriteLong(file,AGIDL_LBMGetSize(lbm)+768+14+12);
	AGIDL_PrintFourCC(file,'P','B','M',' ');
	AGIDL_PrintFourCC(file,'B','M','H','D');
	AGIDL_WriteLong(file,20);
	AGIDL_WriteShort(file,AGIDL_LBMGetWidth(lbm));
	AGIDL_WriteShort(file,AGIDL_LBMGetHeight(lbm));
	AGIDL_WriteShort(file,0);
	AGIDL_WriteShort(file,0);
	AGIDL_WriteByte(file,8);
	AGIDL_WriteByte(file,0);
	if(lbm->compress != TRUE){
		AGIDL_WriteByte(file,0);
	}
	else{
		AGIDL_WriteByte(file,1);
	}
	AGIDL_WriteByte(file,0);
	AGIDL_WriteShort(file,0);
	AGIDL_WriteByte(file,1);
	AGIDL_WriteByte(file,1);
	AGIDL_WriteShort(file,AGIDL_LBMGetWidth(lbm));
	AGIDL_WriteShort(file,AGIDL_LBMGetHeight(lbm));
	AGIDL_PrintFourCC(file,'C','M','A','P');
	AGIDL_WriteLong(file,768);

	for(int i = 0; i < 256; i++){
		const COLOR clr = lbm->header.cmap.palette.icp.palette_256[i];

		const u8 r = AGIDL_GetR(clr,AGIDL_LBMGetClrFmt(lbm));
		const u8 g = AGIDL_GetG(clr,AGIDL_LBMGetClrFmt(lbm));
		const u8 b = AGIDL_GetB(clr,AGIDL_LBMGetClrFmt(lbm));
		
		AGIDL_WriteByte(file,r);
		AGIDL_WriteByte(file,g);
		AGIDL_WriteByte(file,b);
	}
	
	AGIDL_PrintFourCC(file,'B','O','D','Y');
	AGIDL_WriteLong(file,AGIDL_LBMGetSize(lbm));

	AGIDL_DisableBigEndArch();
}

void AGIDL_LBMEncodeICP(AGIDL_LBM* lbm){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) != 16){
		if(lbm->encode == ICP_ENCODE_THRESHOLD){
			u8 pal_index = 0;
			int pass = 0;
			
			AGIDL_InitICP(&lbm->header.cmap.palette,AGIDL_ICP_256);

			for(int y = 0; y < AGIDL_LBMGetHeight(lbm); y++){
				for(int x = 0; x < AGIDL_LBMGetWidth(lbm); x++){
					const COLOR clr = AGIDL_LBMGetClr(lbm,x,y);
					
					AGIDL_AddColorICP(&lbm->header.cmap.palette,pal_index,clr,AGIDL_LBMGetClrFmt(lbm),AGIDL_LBMGetMaxDiff(lbm),&pass);
					
					if(pass == TRUE){
						pal_index++;
					}
				}
			}
		}
		else{
			AGIDL_EncodeHistogramICP(&lbm->header.cmap.palette,lbm->pixels.pix32,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),AGIDL_LBMGetClrFmt(lbm));
		}
	}
}

void AGIDL_LBMEncodeIMG(const AGIDL_LBM* lbm, FILE* file){
	if(lbm->compress != TRUE){
		for(int y = 0; y < AGIDL_LBMGetHeight(lbm); y++){
			for(int x = 0; x < AGIDL_LBMGetWidth(lbm); x++){
				const COLOR clr = AGIDL_LBMGetClr(lbm,x,y);
				const u8 index = AGIDL_FindNearestColor(lbm->header.cmap.palette,clr,AGIDL_LBMGetClrFmt(lbm));
				AGIDL_WriteByte(file,index);
			}
		}
	}
	else{
		u32 size = 0;

		for(int y = 0; y < AGIDL_LBMGetHeight(lbm); y++){
			for(int x = 0; x < AGIDL_LBMGetWidth(lbm); x++){
				COLOR clr = AGIDL_LBMGetClr(lbm,x,y);
				const u32 count = AGIDL_EncodeRLE(lbm->pixels.pix32,AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)),x,y,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),128);
				u8 index = AGIDL_FindNearestColor(lbm->header.cmap.palette,clr,AGIDL_LBMGetClrFmt(lbm));
				
				 if(count > 1){
					AGIDL_WriteByte(file,257-count);
					AGIDL_WriteByte(file,index);
					
					size += 2;
				 }
				 else{
					AGIDL_WriteByte(file,count-1);

					for (int i = 0; i < count; i++, size++) {
						clr = AGIDL_LBMGetClr(lbm,x+i,y);
						index = AGIDL_FindNearestColor(lbm->header.cmap.palette,clr,AGIDL_LBMGetClrFmt(lbm));
						AGIDL_WriteByte(file,index);
					}
					
					size++;
				 }
				
				x += count - 1;
			}
		}
		
		fseek(file,820,SEEK_SET);
		
		AGIDL_InitBigEndArch();
		AGIDL_WriteLong(file,size);
		AGIDL_DisableBigEndArch();
	}
}

void AGIDL_ExportLBM(AGIDL_LBM* lbm){
	FILE* file = fopen(lbm->filename,"wb");
	
	if(file == NULL){
		printf("Could not open/create ILBM image - %s!\n",lbm->filename);
		return;
	}
	
	switch(AGIDL_LBMGetClrFmt(lbm)){
		case AGIDL_RGB_888:{
			AGIDL_LBMEncodeICP(lbm);
			AGIDL_LBMEncodeHeader(lbm,file);
			AGIDL_LBMEncodeIMG(lbm,file);
		}break;
		case AGIDL_BGR_888:{
			AGIDL_LBMBGR2RGB(lbm);
			AGIDL_LBMEncodeICP(lbm);
			AGIDL_LBMEncodeHeader(lbm,file);
			AGIDL_LBMEncodeIMG(lbm,file);
			AGIDL_LBMRGB2BGR(lbm);
		}break;
		case AGIDL_RGB_555:{
			AGIDL_LBMConvert16BPPTO24BPP(lbm);
			AGIDL_LBMEncodeICP(lbm);
			AGIDL_LBMEncodeHeader(lbm,file);
			AGIDL_LBMEncodeIMG(lbm,file);
			AGIDL_LBMConvert24BPPTO16BPP(lbm);
		}break;
		case AGIDL_BGR_555:{
			AGIDL_LBMBGR2RGB(lbm);
			AGIDL_LBMConvert16BPPTO24BPP(lbm);
			AGIDL_LBMEncodeICP(lbm);
			AGIDL_LBMEncodeHeader(lbm,file);
			AGIDL_LBMEncodeIMG(lbm,file);
			AGIDL_LBMConvert24BPPTO16BPP(lbm);
			AGIDL_LBMRGB2BGR(lbm);
		}break;
		case AGIDL_RGB_565:{
			AGIDL_LBMConvert565TO555(lbm);
			AGIDL_LBMConvert16BPPTO24BPP(lbm);
			AGIDL_LBMEncodeICP(lbm);
			AGIDL_LBMEncodeHeader(lbm,file);
			AGIDL_LBMEncodeIMG(lbm,file);
			AGIDL_LBMConvert24BPPTO16BPP(lbm);
			AGIDL_LBMConvert565TO555(lbm);
		}break;
		case AGIDL_BGR_565:{
			AGIDL_LBMBGR2RGB(lbm);
			AGIDL_LBMConvert565TO555(lbm);
			AGIDL_LBMConvert16BPPTO24BPP(lbm);
			AGIDL_LBMEncodeICP(lbm);
			AGIDL_LBMEncodeHeader(lbm,file);
			AGIDL_LBMEncodeIMG(lbm,file);
			AGIDL_LBMConvert24BPPTO16BPP(lbm);
			AGIDL_LBMConvert565TO555(lbm);
			AGIDL_LBMRGB2BGR(lbm);
		}break;
		case AGIDL_RGBA_8888:{
			AGIDL_LBMEncodeICP(lbm);
			AGIDL_LBMEncodeHeader(lbm,file);
			AGIDL_LBMEncodeIMG(lbm,file);
		}break;
		case AGIDL_ARGB_8888:{
			AGIDL_LBMEncodeICP(lbm);
			AGIDL_LBMEncodeHeader(lbm,file);
			AGIDL_LBMEncodeIMG(lbm,file);
		}break;
	}
	
	fclose(file);
}
