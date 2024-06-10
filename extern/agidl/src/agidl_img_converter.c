/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_converter.c
*   Date: 11/3/2023
*   Version: 0.1b
*   Updated: 3/2/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_img_converter.h>

#include <stdlib.h>

AGIDL_TGA* AGIDL_ConvertBMP2TGA(const AGIDL_BMP* bmp){
	char* imgname = AGIDL_GetImgName(bmp->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_TGA);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_TGA* tga = AGIDL_CreateTGA(filename,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),
	AGIDL_BMPGetClrFmt(bmp));
	AGIDL_TGASetICPMode(tga,bmp->icp);
	AGIDL_TGASetMaxDiff(tga,AGIDL_BMPGetMaxDiff(bmp));
	AGIDL_TGASetCompression(tga,bmp->compression);
	AGIDL_TGASetICPEncoding(tga,bmp->encode);
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp));
	
	if(bits == 24 || bits == 32){
		AGIDL_TGASyncPix(tga,bmp->pixels.pix32);
	}
	else{
		AGIDL_TGASyncPix16(tga,bmp->pixels.pix16);
	}
	return tga;
}

AGIDL_TIM* AGIDL_ConvertBMP2TIM(const AGIDL_BMP* bmp){
	char* imgname = AGIDL_GetImgName(bmp->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_TIM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_TIM* tim = AGIDL_CreateTIM(filename,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),
	AGIDL_BMPGetClrFmt(bmp));
	AGIDL_TIMSetICPMode(tim,bmp->icp);
	AGIDL_TIMSetMaxDiff(tim,AGIDL_BMPGetMaxDiff(bmp));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp));
	
	if(bits == 24 || bits == 32){
		AGIDL_TIMSyncPix(tim,bmp->pixels.pix32);
	}
	else{
		AGIDL_TIMSyncPix16(tim,bmp->pixels.pix16);
	}
	return tim;
}

AGIDL_PCX* AGIDL_ConvertBMP2PCX(const AGIDL_BMP* bmp){
	char* imgname = AGIDL_GetImgName(bmp->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PCX);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PCX* pcx = AGIDL_CreatePCX(filename,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),
	AGIDL_BMPGetClrFmt(bmp));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp));
	
	if(bits == 24 || bits == 32){
		AGIDL_PCXSyncPix(pcx,bmp->pixels.pix32);
	}
	else{
		AGIDL_PCXSyncPix16(pcx,bmp->pixels.pix16);
	}
	return pcx;
}

AGIDL_LMP* AGIDL_ConvertBMP2LMP(const AGIDL_BMP* bmp){
	char* imgname = AGIDL_GetImgName(bmp->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_LMP);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_LMP* lmp = AGIDL_CreateLMP(filename,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),
	AGIDL_BMPGetClrFmt(bmp));
	AGIDL_LMPSetMaxDiff(lmp,AGIDL_BMPGetMaxDiff(bmp));
	
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp));
	
	if(bits == 24 || bits == 32){
		AGIDL_LMPSyncPix(lmp,bmp->pixels.pix32);
	}
	else{
		AGIDL_LMPSyncPix16(lmp,bmp->pixels.pix16);
	}
	return lmp;
}

AGIDL_PVR* AGIDL_ConvertBMP2PVR(const AGIDL_BMP* bmp){
	char* imgname = AGIDL_GetImgName(bmp->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PVR);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PVR* pvr = AGIDL_CreatePVR(filename,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),
	AGIDL_BMPGetClrFmt(bmp));
	AGIDL_PVRSetMaxDiff(pvr,AGIDL_BMPGetMaxDiff(bmp));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp));
	
	if(bits == 24 || bits == 32){
		AGIDL_PVRSyncPix(pvr,bmp->pixels.pix32);
	}
	else{
		AGIDL_PVRSyncPix16(pvr,bmp->pixels.pix16);
	}
	return pvr;
}

AGIDL_GXT* AGIDL_ConvertBMP2GXT(const AGIDL_BMP* bmp){
	char* imgname = AGIDL_GetImgName(bmp->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_GXT);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_GXT* gxt = AGIDL_CreateGXT(filename,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),
	AGIDL_BMPGetClrFmt(bmp));
	AGIDL_GXTSetICPMode(gxt,bmp->icp);
	AGIDL_GXTSetMaxDiff(gxt,AGIDL_BMPGetMaxDiff(bmp));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp));
	
	if(bits == 24 || bits == 32){
		AGIDL_GXTSyncPix(gxt,bmp->pixels.pix32);
	}
	else{
		AGIDL_GXTSyncPix16(gxt,bmp->pixels.pix16);
	}
	return gxt;
}

AGIDL_BTI* AGIDL_ConvertBMP2BTI(const AGIDL_BMP* bmp){
	char* imgname = AGIDL_GetImgName(bmp->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_BTI);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_BTI* bti = AGIDL_CreateBTI(filename,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),
	AGIDL_BMPGetClrFmt(bmp));
	AGIDL_BTISetICPMode(bti,bmp->icp);
	AGIDL_BTISetMaxDiff(bti,AGIDL_BMPGetMaxDiff(bmp));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp));
	
	if(bits == 24 || bits == 32){
		AGIDL_BTISyncPix(bti,bmp->pixels.pix32);
	}
	else{
		AGIDL_BTISyncPix16(bti,bmp->pixels.pix16);
	}
	return bti;
}

AGIDL_3DF* AGIDL_ConvertBMP23DF(const AGIDL_BMP* bmp){
	char* imgname = AGIDL_GetImgName(bmp->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_3DF);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_3DF* glide = AGIDL_Create3DF(filename,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),
	AGIDL_BMPGetClrFmt(bmp));
	AGIDL_3DFSetICPMode(glide,bmp->icp);
	AGIDL_3DFSetMaxDiff(glide,AGIDL_BMPGetMaxDiff(bmp));
	AGIDL_3DFSetICPEncoding(glide,bmp->encode);
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp));
	
	if(bits == 24 || bits == 32){
		AGIDL_3DFSyncPix(glide,bmp->pixels.pix32);
	}
	else{
		AGIDL_3DFSyncPix16(glide,bmp->pixels.pix16);
	}
	return glide;
}

AGIDL_PPM* AGIDL_ConvertBMP2PPM(const AGIDL_BMP* bmp){
	char* imgname = AGIDL_GetImgName(bmp->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PPM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PPM* ppm = AGIDL_CreatePPM(filename,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),
	AGIDL_BMPGetClrFmt(bmp));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp));
	
	if(bits == 24 || bits == 32){
		AGIDL_PPMSyncPix(ppm,bmp->pixels.pix32);
	}
	else{
		AGIDL_PPMSyncPix16(ppm,bmp->pixels.pix16);
	}
	return ppm;
}

AGIDL_LBM* AGIDL_ConvertBMP2LBM(const AGIDL_BMP* bmp){
	char* imgname = AGIDL_GetImgName(bmp->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_LBM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_LBM* lbm = AGIDL_CreateLBM(filename,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),
	AGIDL_BMPGetClrFmt(bmp));
	AGIDL_LBMSetICPMode(lbm,bmp->icp);
	AGIDL_LBMSetCompression(lbm,bmp->compression);
	AGIDL_LBMSetMaxDiff(lbm,AGIDL_BMPGetMaxDiff(bmp));
	AGIDL_LBMSetICPEncoding(lbm,bmp->encode);
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp));
	
	if(bits == 24 || bits == 32){
		AGIDL_LBMSyncPix(lbm,bmp->pixels.pix32);
	}
	else{
		AGIDL_LBMSyncPix16(lbm,bmp->pixels.pix16);
	}
	return lbm;
}

AGIDL_BMP* AGIDL_ConvertTGA2BMP(const AGIDL_TGA* tga){
	char* imgname = AGIDL_GetImgName(tga->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_BMP);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_BMP* bmp = AGIDL_CreateBMP(filename,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),
	AGIDL_TGAGetClrFmt(tga));
	AGIDL_BMPSetICPMode(bmp,tga->icp);
	AGIDL_BMPSetMaxDiff(bmp,AGIDL_TGAGetMaxDiff(tga));
	AGIDL_BMPSetCompression(bmp,tga->compression);
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga));
	
	if(bits == 24 || bits == 32){
		AGIDL_BMPSyncPix(bmp,tga->pixels.pix32);
	}
	else{
		AGIDL_BMPSyncPix16(bmp,tga->pixels.pix16);
	}
	return bmp;
}

AGIDL_TIM* AGIDL_ConvertTGA2TIM(const AGIDL_TGA* tga){
	char* imgname = AGIDL_GetImgName(tga->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_TIM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_TIM* tim = AGIDL_CreateTIM(filename,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),
	AGIDL_TGAGetClrFmt(tga));
	AGIDL_TIMSetICPMode(tim,tga->icp);
	AGIDL_TIMSetMaxDiff(tim,AGIDL_TGAGetMaxDiff(tga));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga));
	
	if(bits == 24 || bits == 32){
		AGIDL_TIMSyncPix(tim,tga->pixels.pix32);
	}
	else{
		AGIDL_TIMSyncPix16(tim,tga->pixels.pix16);
	}
	return tim;
}

AGIDL_PCX* AGIDL_ConvertTGA2PCX(const AGIDL_TGA* tga){
	char* imgname = AGIDL_GetImgName(tga->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PCX);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PCX* pcx = AGIDL_CreatePCX(filename,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),
	AGIDL_TGAGetClrFmt(tga));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga));
	
	if(bits == 24 || bits == 32){
		AGIDL_PCXSyncPix(pcx,tga->pixels.pix32);
	}
	else{
		AGIDL_PCXSyncPix16(pcx,tga->pixels.pix16);
	}
	return pcx;
}

AGIDL_LMP* AGIDL_ConvertTGA2LMP(const AGIDL_TGA* tga){
	char* imgname = AGIDL_GetImgName(tga->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_LMP);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_LMP* lmp = AGIDL_CreateLMP(filename,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),
	AGIDL_TGAGetClrFmt(tga));
	AGIDL_LMPSetMaxDiff(lmp,AGIDL_TGAGetMaxDiff(tga));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga));
	
	if(bits == 24 || bits == 32){
		AGIDL_LMPSyncPix(lmp,tga->pixels.pix32);
	}
	else{
		AGIDL_LMPSyncPix16(lmp,tga->pixels.pix16);
	}
	return lmp;
}

AGIDL_PVR* AGIDL_ConvertTGA2PVR(const AGIDL_TGA* tga){
	char* imgname = AGIDL_GetImgName(tga->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PVR);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PVR* pvr = AGIDL_CreatePVR(filename,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),
	AGIDL_TGAGetClrFmt(tga));
	AGIDL_PVRSetMaxDiff(pvr,AGIDL_TGAGetMaxDiff(tga));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga));
	
	if(bits == 24 || bits == 32){
		AGIDL_PVRSyncPix(pvr,tga->pixels.pix32);
	}
	else{
		AGIDL_PVRSyncPix16(pvr,tga->pixels.pix16);
	}
	return pvr;
}

AGIDL_GXT* AGIDL_ConvertTGA2GXT(const AGIDL_TGA* tga){
	char* imgname = AGIDL_GetImgName(tga->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_GXT);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_GXT* gxt = AGIDL_CreateGXT(filename,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),
	AGIDL_TGAGetClrFmt(tga));
	AGIDL_GXTSetICPMode(gxt,tga->icp);
	AGIDL_GXTSetMaxDiff(gxt,AGIDL_TGAGetMaxDiff(tga));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga));
	
	if(bits == 24 || bits == 32){
		AGIDL_GXTSyncPix(gxt,tga->pixels.pix32);
	}
	else{
		AGIDL_GXTSyncPix16(gxt,tga->pixels.pix16);
	}
	return gxt;
}

AGIDL_BTI* AGIDL_ConvertTGA2BTI(const AGIDL_TGA* tga){
	char* imgname = AGIDL_GetImgName(tga->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_BTI);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_BTI* bti = AGIDL_CreateBTI(filename,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),
	AGIDL_TGAGetClrFmt(tga));
	AGIDL_BTISetICPMode(bti,tga->icp);
	AGIDL_BTISetMaxDiff(bti,AGIDL_TGAGetMaxDiff(tga));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga));
	
	if(bits == 24 || bits == 32){
		AGIDL_BTISyncPix(bti,tga->pixels.pix32);
	}
	else{
		AGIDL_BTISyncPix16(bti,tga->pixels.pix16);
	}
	return bti;
}

AGIDL_3DF* AGIDL_ConvertTGA23DF(const AGIDL_TGA* tga){
	char* imgname = AGIDL_GetImgName(tga->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_3DF);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_3DF* glide = AGIDL_Create3DF(filename,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),
	AGIDL_TGAGetClrFmt(tga));
	AGIDL_3DFSetICPMode(glide,tga->icp);
	AGIDL_3DFSetMaxDiff(glide,AGIDL_TGAGetMaxDiff(tga));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga));
	
	if(bits == 24 || bits == 32){
		AGIDL_3DFSyncPix(glide,tga->pixels.pix32);
	}
	else{
		AGIDL_3DFSyncPix16(glide,tga->pixels.pix16);
	}
	return glide;
}

AGIDL_PPM* AGIDL_ConvertTGA2PPM(const AGIDL_TGA* tga){
	const char* imgname = AGIDL_GetImgName(tga->filename);
	const char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PPM);
	const char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PPM* ppm = AGIDL_CreatePPM(filename,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),
	AGIDL_TGAGetClrFmt(tga));

	const int bits = AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga));
	
	if(bits == 24 || bits == 32){
		AGIDL_PPMSyncPix(ppm,tga->pixels.pix32);
	}
	else{
		AGIDL_PPMSyncPix16(ppm,tga->pixels.pix16);
	}
	return ppm;
}

AGIDL_LBM* AGIDL_ConvertTGA2LBM(const AGIDL_TGA* tga){
	char* imgname = AGIDL_GetImgName(tga->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_LBM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_LBM* lbm = AGIDL_CreateLBM(filename,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),
	AGIDL_TGAGetClrFmt(tga));
	AGIDL_LBMSetICPMode(lbm,tga->icp);
	AGIDL_LBMSetMaxDiff(lbm,AGIDL_TGAGetMaxDiff(tga));
	AGIDL_LBMSetCompression(lbm,tga->compression);
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga));
	
	if(bits == 24 || bits == 32){
		AGIDL_LBMSyncPix(lbm,tga->pixels.pix32);
	}
	else{
		AGIDL_LBMSyncPix16(lbm,tga->pixels.pix16);
	}
	return lbm;
}

AGIDL_BMP* AGIDL_ConvertTIM2BMP(const AGIDL_TIM* tim){
	char* imgname = AGIDL_GetImgName(tim->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_BMP);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_BMP* bmp = AGIDL_CreateBMP(filename,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),
	AGIDL_TIMGetClrFmt(tim));
	AGIDL_BMPSetICPMode(bmp,tim->icp);
	AGIDL_BMPSetMaxDiff(bmp,AGIDL_TIMGetMaxDiff(tim));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim));
	
	if(bits == 24 || bits == 32){
		AGIDL_BMPSyncPix(bmp,tim->pixels.pix32);
	}
	else{
		AGIDL_BMPSyncPix16(bmp,tim->pixels.pix16);
	}

	return bmp;
}

AGIDL_TGA* AGIDL_ConvertTIM2TGA(const AGIDL_TIM* tim){
	char* imgname = AGIDL_GetImgName(tim->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_TGA);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_TGA* tga = AGIDL_CreateTGA(filename,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),
	AGIDL_TIMGetClrFmt(tim));
	AGIDL_TGASetICPMode(tga,tim->icp);
	AGIDL_TGASetMaxDiff(tga,AGIDL_TIMGetMaxDiff(tim));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim));
	
	if(bits == 24 || bits == 32){
		AGIDL_TGASyncPix(tga,tim->pixels.pix32);
	}
	else{
		AGIDL_TGASyncPix16(tga,tim->pixels.pix16);
	}
	return tga;
}

AGIDL_PCX* AGIDL_ConvertTIM2PCX(const AGIDL_TIM* tim){
	char* imgname = AGIDL_GetImgName(tim->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PCX);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PCX* pcx = AGIDL_CreatePCX(filename,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),
	AGIDL_TIMGetClrFmt(tim));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim));
	
	if(bits == 24 || bits == 32){
		AGIDL_PCXSyncPix(pcx,tim->pixels.pix32);
	}
	else{
		AGIDL_PCXSyncPix16(pcx,tim->pixels.pix16);
	}
		
	return pcx;
}

AGIDL_LMP* AGIDL_ConvertTIM2LMP(const AGIDL_TIM* tim){
	char* imgname = AGIDL_GetImgName(tim->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_LMP);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_LMP* lmp = AGIDL_CreateLMP(filename,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),
	AGIDL_TIMGetClrFmt(tim));
	AGIDL_LMPSetMaxDiff(lmp,AGIDL_TIMGetMaxDiff(tim));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim));
	
	if(bits == 24 || bits == 32){
		AGIDL_LMPSyncPix(lmp,tim->pixels.pix32);
	}
	else{
		AGIDL_LMPSyncPix16(lmp,tim->pixels.pix16);
	}
	return lmp;
}

AGIDL_PVR* AGIDL_ConvertTIM2PVR(const AGIDL_TIM* tim){
	char* imgname = AGIDL_GetImgName(tim->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PVR);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PVR* pvr = AGIDL_CreatePVR(filename,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),
	AGIDL_TIMGetClrFmt(tim));
	AGIDL_PVRSetICPMode(pvr,tim->icp);
	AGIDL_PVRSetMaxDiff(pvr,AGIDL_TIMGetMaxDiff(tim));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim));
	
	if(bits == 24 || bits == 32){
		AGIDL_PVRSyncPix(pvr,tim->pixels.pix32);
	}
	else{
		AGIDL_PVRSyncPix16(pvr,tim->pixels.pix16);
	}
	return pvr;
}

AGIDL_GXT* AGIDL_ConvertTIM2GXT(const AGIDL_TIM* tim){
	char* imgname = AGIDL_GetImgName(tim->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_GXT);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_GXT* gxt = AGIDL_CreateGXT(filename,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),
	AGIDL_TIMGetClrFmt(tim));
	AGIDL_GXTSetICPMode(gxt,tim->icp);
	AGIDL_GXTSetMaxDiff(gxt,AGIDL_TIMGetMaxDiff(tim));

	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim));
	
	if(bits == 24 || bits == 32){
		AGIDL_GXTSyncPix(gxt,tim->pixels.pix32);
	}
	else{
		AGIDL_GXTSyncPix16(gxt,tim->pixels.pix16);
	}
	return gxt;
}

AGIDL_BTI* AGIDL_ConvertTIM2BTI(const AGIDL_TIM* tim){
	char* imgname = AGIDL_GetImgName(tim->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_BTI);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_BTI* bti = AGIDL_CreateBTI(filename,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),
	AGIDL_TIMGetClrFmt(tim));
	AGIDL_BTISetICPMode(bti,tim->icp);
	AGIDL_BTISetMaxDiff(bti,AGIDL_TIMGetMaxDiff(tim));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim));
	
	if(bits == 24 || bits == 32){
		AGIDL_BTISyncPix(bti,tim->pixels.pix32);
	}
	else{
		AGIDL_BTISyncPix16(bti,tim->pixels.pix16);
	}
	return bti;
}

AGIDL_3DF* AGIDL_ConvertTIM23DF(const AGIDL_TIM* tim){
	char* imgname = AGIDL_GetImgName(tim->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_3DF);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_3DF* glide = AGIDL_Create3DF(filename,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),
	AGIDL_TIMGetClrFmt(tim));
	AGIDL_3DFSetICPMode(glide,tim->icp);
	AGIDL_3DFSetMaxDiff(glide,AGIDL_TIMGetMaxDiff(tim));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim));
	
	if(bits == 24 || bits == 32){
		AGIDL_3DFSyncPix(glide,tim->pixels.pix32);
	}
	else{
		AGIDL_3DFSyncPix16(glide,tim->pixels.pix16);
	}
	return glide;
}

AGIDL_PPM* AGIDL_ConvertTIM2PPM(const AGIDL_TIM* tim){
	char* imgname = AGIDL_GetImgName(tim->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PPM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PPM* ppm = AGIDL_CreatePPM(filename,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),
	AGIDL_TIMGetClrFmt(tim));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim));
	
	if(bits == 24 || bits == 32){
		AGIDL_PPMSyncPix(ppm,tim->pixels.pix32);
	}
	else{
		AGIDL_PPMSyncPix16(ppm,tim->pixels.pix16);
	}
	return ppm;
}

AGIDL_LBM* AGIDL_ConvertTIM2LBM(const AGIDL_TIM* tim){
	char* imgname = AGIDL_GetImgName(tim->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_LBM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_LBM* lbm = AGIDL_CreateLBM(filename,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),
	AGIDL_TIMGetClrFmt(tim));
	AGIDL_LBMSetICPMode(lbm,tim->icp);
	AGIDL_LBMSetMaxDiff(lbm,AGIDL_TIMGetMaxDiff(tim));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim));
	
	if(bits == 24 || bits == 32){
		AGIDL_LBMSyncPix(lbm,tim->pixels.pix32);
	}
	else{
		AGIDL_LBMSyncPix16(lbm,tim->pixels.pix16);
	}

	return lbm;
}

AGIDL_BMP* AGIDL_ConvertPCX2BMP(const AGIDL_PCX* pcx){
	char* imgname = AGIDL_GetImgName(pcx->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_BMP);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_BMP* bmp = AGIDL_CreateBMP(filename,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),
	AGIDL_PCXGetClrFmt(pcx));
	AGIDL_BMPSetMaxDiff(bmp,15);
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx));
	
	if(bits == 24 || bits == 32){
		AGIDL_BMPSyncPix(bmp,pcx->pixels.pix32);
	}
	else{
		AGIDL_BMPSyncPix16(bmp,pcx->pixels.pix16);
	}
	return bmp;
}

AGIDL_TGA* AGIDL_ConvertPCX2TGA(const AGIDL_PCX* pcx){
	char* imgname = AGIDL_GetImgName(pcx->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_TGA);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_TGA* tga = AGIDL_CreateTGA(filename,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),
	AGIDL_PCXGetClrFmt(pcx));
	AGIDL_TGASetMaxDiff(tga,15);
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx));
	
	if(bits == 24 || bits == 32){
		AGIDL_TGASyncPix(tga,pcx->pixels.pix32);
	}
	else{
		AGIDL_TGASyncPix16(tga,pcx->pixels.pix16);
	}
	return tga;
}

AGIDL_TIM* AGIDL_ConvertPCX2TIM(const AGIDL_PCX* pcx){
	char* imgname = AGIDL_GetImgName(pcx->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_TIM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_TIM* tim = AGIDL_CreateTIM(filename,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),
	AGIDL_PCXGetClrFmt(pcx));
	AGIDL_TIMSetMaxDiff(tim,15);
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx));
	
	if(bits == 24 || bits == 32){
		AGIDL_TIMSyncPix(tim,pcx->pixels.pix32);
	}
	else{
		AGIDL_TIMSyncPix16(tim,pcx->pixels.pix16);
	}
	return tim;
}

AGIDL_LMP* AGIDL_ConvertPCX2LMP(const AGIDL_PCX* pcx){
	char* imgname = AGIDL_GetImgName(pcx->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_LMP);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_LMP* lmp = AGIDL_CreateLMP(filename,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),
	AGIDL_PCXGetClrFmt(pcx));
	AGIDL_LMPSetMaxDiff(lmp,15);
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx));
	
	if(bits == 24 || bits == 32){
		AGIDL_LMPSyncPix(lmp,pcx->pixels.pix32);
	}
	else{
		AGIDL_LMPSyncPix16(lmp,pcx->pixels.pix16);
	}
	return lmp;
}

AGIDL_PVR* AGIDL_ConvertPCX2PVR(const AGIDL_PCX* pcx){
	char* imgname = AGIDL_GetImgName(pcx->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PVR);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PVR* pvr = AGIDL_CreatePVR(filename,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),
	AGIDL_PCXGetClrFmt(pcx));
	AGIDL_PVRSetMaxDiff(pvr,15);
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx));
	
	if(bits == 24 || bits == 32){
		AGIDL_PVRSyncPix(pvr,pcx->pixels.pix32);
	}
	else{
		AGIDL_PVRSyncPix16(pvr,pcx->pixels.pix16);
	}
	return pvr;
}

AGIDL_GXT* AGIDL_ConvertPCXT2GXT(const AGIDL_PCX* pcx){
	char* imgname = AGIDL_GetImgName(pcx->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_GXT);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_GXT* gxt = AGIDL_CreateGXT(filename,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),
	AGIDL_PCXGetClrFmt(pcx));
	AGIDL_GXTSetICPMode(gxt,pcx->icp);
	AGIDL_GXTSetMaxDiff(gxt,AGIDL_PCXGetMaxDiff(pcx));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx));
	
	if(bits == 24 || bits == 32){
		AGIDL_GXTSyncPix(gxt,pcx->pixels.pix32);
	}
	else{
		AGIDL_GXTSyncPix16(gxt,pcx->pixels.pix16);
	}
	return gxt;
}

AGIDL_BTI* AGIDL_ConvertPCXT2BTI(const AGIDL_PCX* pcx){
	char* imgname = AGIDL_GetImgName(pcx->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_BTI);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_BTI* bti = AGIDL_CreateBTI(filename,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),
	AGIDL_PCXGetClrFmt(pcx));
	AGIDL_BTISetICPMode(bti,pcx->icp);
	AGIDL_BTISetMaxDiff(bti,AGIDL_PCXGetMaxDiff(pcx));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx));
	
	if(bits == 24 || bits == 32){
		AGIDL_BTISyncPix(bti,pcx->pixels.pix32);
	}
	else{
		AGIDL_BTISyncPix16(bti,pcx->pixels.pix16);
	}
	return bti;
}

AGIDL_3DF* AGIDL_ConvertPCXT23DF(const AGIDL_PCX* pcx){
	char* imgname = AGIDL_GetImgName(pcx->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_3DF);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_3DF* glide = AGIDL_Create3DF(filename,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),
	AGIDL_PCXGetClrFmt(pcx));
	AGIDL_3DFSetICPMode(glide,pcx->icp);
	AGIDL_3DFSetMaxDiff(glide,AGIDL_PCXGetMaxDiff(pcx));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx));
	
	if(bits == 24 || bits == 32){
		AGIDL_3DFSyncPix(glide,pcx->pixels.pix32);
	}
	else{
		AGIDL_3DFSyncPix16(glide,pcx->pixels.pix16);
	}
	return glide;
}

AGIDL_PPM* AGIDL_ConvertPCXT2PPM(const AGIDL_PCX* pcx){
	char* imgname = AGIDL_GetImgName(pcx->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PPM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PPM* ppm = AGIDL_CreatePPM(filename,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),
	AGIDL_PCXGetClrFmt(pcx));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx));
	
	if(bits == 24 || bits == 32){
		AGIDL_PPMSyncPix(ppm,pcx->pixels.pix32);
	}
	else{
		AGIDL_PPMSyncPix16(ppm,pcx->pixels.pix16);
	}
	return ppm;
}

AGIDL_LBM* AGIDL_ConvertPCX2LBM(const AGIDL_PCX* pcx){
	char* imgname = AGIDL_GetImgName(pcx->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_LBM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_LBM* lbm = AGIDL_CreateLBM(filename,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),
	AGIDL_PCXGetClrFmt(pcx));
	AGIDL_LBMSetMaxDiff(lbm,15);
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx));
	
	if(bits == 24 || bits == 32){
		AGIDL_LBMSyncPix(lbm,pcx->pixels.pix32);
	}
	else{
		AGIDL_LBMSyncPix16(lbm,pcx->pixels.pix16);
	}
	return lbm;
}

AGIDL_BMP* AGIDL_ConvertLMP2BMP(const AGIDL_LMP* lmp){
	char* imgname = AGIDL_GetImgName(lmp->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_BMP);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_BMP* bmp = AGIDL_CreateBMP(filename,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),
	AGIDL_LMPGetClrFmt(lmp));
	AGIDL_BMPSetMaxDiff(bmp,AGIDL_LMPGetMaxDiff(lmp));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp));
	
	if(bits == 24 || bits == 32){
		AGIDL_BMPSyncPix(bmp,lmp->pixels.pix32);
	}
	else{
		AGIDL_BMPSyncPix16(bmp,lmp->pixels.pix16);
	}
	return bmp;
}

AGIDL_TGA* AGIDL_ConvertLMP2TGA(const AGIDL_LMP* lmp){
	char* imgname = AGIDL_GetImgName(lmp->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_TGA);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_TGA* tga = AGIDL_CreateTGA(filename,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),
	AGIDL_LMPGetClrFmt(lmp));
	AGIDL_TGASetMaxDiff(tga,AGIDL_LMPGetMaxDiff(lmp));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp));
	
	if(bits == 24 || bits == 32){
		AGIDL_TGASyncPix(tga,lmp->pixels.pix32);
	}
	else{
		AGIDL_TGASyncPix16(tga,lmp->pixels.pix16);
	}
	return tga;
}

AGIDL_TIM* AGIDL_ConvertLMP2TIM(const AGIDL_LMP* lmp){
	char* imgname = AGIDL_GetImgName(lmp->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_TIM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_TIM* tim = AGIDL_CreateTIM(filename,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),
	AGIDL_LMPGetClrFmt(lmp));
	AGIDL_TIMSetMaxDiff(tim,AGIDL_LMPGetMaxDiff(lmp));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp));
	
	if(bits == 24 || bits == 32){
		AGIDL_TIMSyncPix(tim,lmp->pixels.pix32);
	}
	else{
		AGIDL_TIMSyncPix16(tim,lmp->pixels.pix16);
	}
	return tim;
}

AGIDL_PCX* AGIDL_ConvertLMP2PCX(const AGIDL_LMP* lmp){
	char* imgname = AGIDL_GetImgName(lmp->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PCX);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PCX* pcx = AGIDL_CreatePCX(filename,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),
	AGIDL_LMPGetClrFmt(lmp));
	AGIDL_PCXSetMaxDiff(pcx,AGIDL_LMPGetMaxDiff(lmp));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp));
	
	if(bits == 24 || bits == 32){
		AGIDL_PCXSyncPix(pcx,lmp->pixels.pix32);
	}
	else{
		AGIDL_PCXSyncPix16(pcx,lmp->pixels.pix16);
	}
	return pcx;
}

AGIDL_PVR* AGIDL_ConvertLMP2PVR(const AGIDL_LMP* lmp){
	char* imgname = AGIDL_GetImgName(lmp->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PVR);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PVR* pvr = AGIDL_CreatePVR(filename,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),
	AGIDL_LMPGetClrFmt(lmp));
	AGIDL_PVRSetMaxDiff(pvr,AGIDL_LMPGetMaxDiff(lmp));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp));
	
	if(bits == 24 || bits == 32){
		AGIDL_PVRSyncPix(pvr,lmp->pixels.pix32);
	}
	else{
		AGIDL_PVRSyncPix16(pvr,lmp->pixels.pix16);
	}
	return pvr;
}

AGIDL_GXT* AGIDL_ConvertLMP2GXT(const AGIDL_LMP* lmp){
	char* imgname = AGIDL_GetImgName(lmp->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_GXT);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_GXT* gxt = AGIDL_CreateGXT(filename,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),
	AGIDL_LMPGetClrFmt(lmp));
	AGIDL_GXTSetMaxDiff(gxt,AGIDL_LMPGetMaxDiff(lmp));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp));
	
	if(bits == 24 || bits == 32){
		AGIDL_GXTSyncPix(gxt,lmp->pixels.pix32);
	}
	else{
		AGIDL_GXTSyncPix16(gxt,lmp->pixels.pix16);
	}
	return gxt;
}

AGIDL_BTI* AGIDL_ConvertLMP2BTI(const AGIDL_LMP* lmp){
	char* imgname = AGIDL_GetImgName(lmp->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_BTI);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_BTI* bti = AGIDL_CreateBTI(filename,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),
	AGIDL_LMPGetClrFmt(lmp));
	AGIDL_BTISetMaxDiff(bti,AGIDL_LMPGetMaxDiff(lmp));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp));
	
	if(bits == 24 || bits == 32){
		AGIDL_BTISyncPix(bti,lmp->pixels.pix32);
	}
	else{
		AGIDL_BTISyncPix16(bti,lmp->pixels.pix16);
	}
	return bti;
}

AGIDL_3DF* AGIDL_ConvertLMP23DF(const AGIDL_LMP* lmp){
	char* imgname = AGIDL_GetImgName(lmp->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_3DF);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_3DF* glide = AGIDL_Create3DF(filename,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),
	AGIDL_LMPGetClrFmt(lmp));
	AGIDL_3DFSetMaxDiff(glide,AGIDL_LMPGetMaxDiff(lmp));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp));
	
	if(bits == 24 || bits == 32){
		AGIDL_3DFSyncPix(glide,lmp->pixels.pix32);
	}
	else{
		AGIDL_3DFSyncPix16(glide,lmp->pixels.pix16);
	}
	return glide;
}

AGIDL_PPM* AGIDL_ConvertLMP2PPM(const AGIDL_LMP* lmp){
	char* imgname = AGIDL_GetImgName(lmp->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PPM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PPM* ppm = AGIDL_CreatePPM(filename,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),
	AGIDL_LMPGetClrFmt(lmp));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp));
	
	if(bits == 24 || bits == 32){
		AGIDL_PPMSyncPix(ppm,lmp->pixels.pix32);
	}
	else{
		AGIDL_PPMSyncPix16(ppm,lmp->pixels.pix16);
	}
	return ppm;
}

AGIDL_LBM* AGIDL_ConvertLMP2LBM(const AGIDL_LMP* lmp){
	char* imgname = AGIDL_GetImgName(lmp->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_LBM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_LBM* lbm = AGIDL_CreateLBM(filename,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),
	AGIDL_LMPGetClrFmt(lmp));
	AGIDL_LBMSetMaxDiff(lbm,AGIDL_LMPGetMaxDiff(lmp));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp));
	
	if(bits == 24 || bits == 32){
		AGIDL_LBMSyncPix(lbm,lmp->pixels.pix32);
	}
	else{
		AGIDL_LBMSyncPix16(lbm,lmp->pixels.pix16);
	}
	return lbm;
}

AGIDL_BMP* AGIDL_ConvertPVR2BMP(const AGIDL_PVR* pvr){
	char* imgname = AGIDL_GetImgName(pvr->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_BMP);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_BMP* bmp = AGIDL_CreateBMP(filename,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),
	AGIDL_PVRGetClrFmt(pvr));
	AGIDL_BMPSetICPMode(bmp,NO_ICP);
	AGIDL_BMPSetMaxDiff(bmp,AGIDL_PVRGetMaxDiff(pvr));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr));
	
	if(bits == 24 || bits == 32){
		AGIDL_BMPSyncPix(bmp,pvr->pixels.pix32);
	}
	else{
		AGIDL_BMPSyncPix16(bmp,pvr->pixels.pix16);
	}
	
	return bmp;
}

AGIDL_TGA* AGIDL_ConvertPVR2TGA(const AGIDL_PVR* pvr){
	char* imgname = AGIDL_GetImgName(pvr->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_TGA);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_TGA* tga = AGIDL_CreateTGA(filename,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),
	AGIDL_PVRGetClrFmt(pvr));
	AGIDL_TGASetICPMode(tga,pvr->icp);
	AGIDL_TGASetMaxDiff(tga,AGIDL_PVRGetMaxDiff(pvr));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr));
	
	if(bits == 24 || bits == 32){
		AGIDL_TGASyncPix(tga,pvr->pixels.pix32);
	}
	else{
		AGIDL_TGASyncPix16(tga,pvr->pixels.pix16);
	}
	return tga;
}

AGIDL_TIM* AGIDL_ConvertPVR2TIM(const AGIDL_PVR* pvr){
	char* imgname = AGIDL_GetImgName(pvr->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_TIM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_TIM* tim = AGIDL_CreateTIM(filename,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),
	AGIDL_PVRGetClrFmt(pvr));
	AGIDL_TIMSetICPMode(tim,pvr->icp);
	AGIDL_TIMSetMaxDiff(tim,AGIDL_PVRGetMaxDiff(pvr));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr));
	
	if(bits == 24 || bits == 32){
		AGIDL_TIMSyncPix(tim,pvr->pixels.pix32);
	}
	else{
		AGIDL_TIMSyncPix16(tim,pvr->pixels.pix16);
	}
	return tim;
}

AGIDL_PCX* AGIDL_ConvertPVR2PCX(const AGIDL_PVR* pvr){
	char* imgname = AGIDL_GetImgName(pvr->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PCX);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PCX* pcx = AGIDL_CreatePCX(filename,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),
	AGIDL_PVRGetClrFmt(pvr));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr));
	
	if(bits == 24 || bits == 32){
		AGIDL_PCXSyncPix(pcx,pvr->pixels.pix32);
	}
	else{
		AGIDL_PCXSyncPix16(pcx,pvr->pixels.pix16);
	}
	return pcx;
}

AGIDL_LMP* AGIDL_ConvertPVR2LMP(const AGIDL_PVR* pvr){
	char* imgname = AGIDL_GetImgName(pvr->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_LMP);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_LMP* lmp = AGIDL_CreateLMP(filename,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),
	AGIDL_PVRGetClrFmt(pvr));
	AGIDL_LMPSetMaxDiff(lmp,AGIDL_PVRGetMaxDiff(pvr));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr));
	
	if(bits == 24 || bits == 32){
		AGIDL_LMPSyncPix(lmp,pvr->pixels.pix32);
	}
	else{
		AGIDL_LMPSyncPix16(lmp,pvr->pixels.pix16);
	}
	return lmp;
}

AGIDL_GXT* AGIDL_ConvertPVR2GXT(const AGIDL_PVR* pvr){
	char* imgname = AGIDL_GetImgName(pvr->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_GXT);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_GXT* gxt = AGIDL_CreateGXT(filename,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),
	AGIDL_PVRGetClrFmt(pvr));
	AGIDL_GXTSetICPMode(gxt,pvr->icp);
	AGIDL_GXTSetMaxDiff(gxt,AGIDL_PVRGetMaxDiff(pvr));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr));
	
	if(bits == 24 || bits == 32){
		AGIDL_GXTSyncPix(gxt,pvr->pixels.pix32);
	}
	else{
		AGIDL_GXTSyncPix16(gxt,pvr->pixels.pix16);
	}
	return gxt;
}

AGIDL_BTI* AGIDL_ConvertPVR2BTI(const AGIDL_PVR* pvr){
	char* imgname = AGIDL_GetImgName(pvr->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_BTI);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_BTI* bti = AGIDL_CreateBTI(filename,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),
	AGIDL_PVRGetClrFmt(pvr));
	AGIDL_BTISetICPMode(bti,pvr->icp);
	AGIDL_BTISetMaxDiff(bti,AGIDL_PVRGetMaxDiff(pvr));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr));
	
	if(bits == 24 || bits == 32){
		AGIDL_BTISyncPix(bti,pvr->pixels.pix32);
	}
	else{
		AGIDL_BTISyncPix16(bti,pvr->pixels.pix16);
	}
	return bti;
}

AGIDL_3DF* AGIDL_ConvertPVR23DF(const AGIDL_PVR* pvr){
	char* imgname = AGIDL_GetImgName(pvr->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_3DF);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_3DF* glide = AGIDL_Create3DF(filename,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),
	AGIDL_PVRGetClrFmt(pvr));
	AGIDL_3DFSetICPMode(glide,pvr->icp);
	AGIDL_3DFSetMaxDiff(glide,AGIDL_PVRGetMaxDiff(pvr));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr));
	
	if(bits == 24 || bits == 32){
		AGIDL_3DFSyncPix(glide,pvr->pixels.pix32);
	}
	else{
		AGIDL_3DFSyncPix16(glide,pvr->pixels.pix16);
	}
	return glide;
}

AGIDL_PPM* AGIDL_ConvertPVR2PPM(const AGIDL_PVR* pvr){
	char* imgname = AGIDL_GetImgName(pvr->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PPM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PPM* ppm = AGIDL_CreatePPM(filename,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),
	AGIDL_PVRGetClrFmt(pvr));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr));
	
	if(bits == 24 || bits == 32){
		AGIDL_PPMSyncPix(ppm,pvr->pixels.pix32);
	}
	else{
		AGIDL_PPMSyncPix16(ppm,pvr->pixels.pix16);
	}
	return ppm;
}

AGIDL_LBM* AGIDL_ConvertPVR2LBM(const AGIDL_PVR* pvr){
	char* imgname = AGIDL_GetImgName(pvr->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_LBM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_LBM* lbm = AGIDL_CreateLBM(filename,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),
	AGIDL_PVRGetClrFmt(pvr));
	AGIDL_LBMSetICPMode(lbm,NO_ICP);
	AGIDL_LBMSetMaxDiff(lbm,AGIDL_PVRGetMaxDiff(pvr));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr));
	
	if(bits == 24 || bits == 32){
		AGIDL_LBMSyncPix(lbm,pvr->pixels.pix32);
	}
	else{
		AGIDL_LBMSyncPix16(lbm,pvr->pixels.pix16);
	}
	
	return lbm;
}

AGIDL_BMP* AGIDL_ConvertGXT2BMP(const AGIDL_GXT* gxt){
	char* imgname = AGIDL_GetImgName(gxt->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_BMP);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_BMP* bmp = AGIDL_CreateBMP(filename,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),
	AGIDL_GXTGetClrFmt(gxt));
	AGIDL_BMPSetICPMode(bmp,gxt->icp);
	AGIDL_BMPSetMaxDiff(bmp,AGIDL_GXTGetMaxDiff(gxt));

	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt));
	
	if(bits == 24 || bits == 32){
		AGIDL_BMPSyncPix(bmp,gxt->pixels.pix32);
	}
	else{
		AGIDL_BMPSyncPix16(bmp,gxt->pixels.pix16);
	}
	return bmp;
}

AGIDL_TGA* AGIDL_ConvertGXT2TGA(const AGIDL_GXT* gxt){
	char* imgname = AGIDL_GetImgName(gxt->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_TGA);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_TGA* tga = AGIDL_CreateTGA(filename,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),
	AGIDL_GXTGetClrFmt(gxt));
	AGIDL_TGASetICPMode(tga,gxt->icp);
	AGIDL_TGASetMaxDiff(tga,AGIDL_GXTGetMaxDiff(gxt));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt));
	
	if(bits == 24 || bits == 32){
		AGIDL_TGASyncPix(tga,gxt->pixels.pix32);
	}
	else{
		AGIDL_TGASyncPix16(tga,gxt->pixels.pix16);
	}
	return tga;
}

AGIDL_TIM* AGIDL_ConvertGXT2TIM(const AGIDL_GXT* gxt){
	char* imgname = AGIDL_GetImgName(gxt->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_TIM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_TIM* tim = AGIDL_CreateTIM(filename,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),
	AGIDL_GXTGetClrFmt(gxt));
	AGIDL_TIMSetICPMode(tim,gxt->icp);
	AGIDL_TIMSetMaxDiff(tim,AGIDL_GXTGetMaxDiff(gxt));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt));
	
	if(bits == 24 || bits == 32){
		AGIDL_TIMSyncPix(tim,gxt->pixels.pix32);
	}
	else{
		AGIDL_TIMSyncPix16(tim,gxt->pixels.pix16);
	}
	return tim;
}

AGIDL_PCX* AGIDL_ConvertGXT2PCX(const AGIDL_GXT* gxt){
	char* imgname = AGIDL_GetImgName(gxt->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PCX);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PCX* pcx = AGIDL_CreatePCX(filename,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),
	AGIDL_GXTGetClrFmt(gxt));
	AGIDL_PCXSetICPMode(pcx,gxt->icp);
	AGIDL_PCXSetMaxDiff(pcx,AGIDL_GXTGetMaxDiff(gxt));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt));
	
	if(bits == 24 || bits == 32){
		AGIDL_PCXSyncPix(pcx,gxt->pixels.pix32);
	}
	else{
		AGIDL_PCXSyncPix16(pcx,gxt->pixels.pix16);
	}
	return pcx;
}

AGIDL_LMP* AGIDL_ConvertGXT2LMP(const AGIDL_GXT* gxt){
	char* imgname = AGIDL_GetImgName(gxt->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_LMP);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_LMP* lmp = AGIDL_CreateLMP(filename,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),
	AGIDL_GXTGetClrFmt(gxt));
	AGIDL_LMPSetMaxDiff(lmp,AGIDL_GXTGetMaxDiff(gxt));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt));
	
	if(bits == 24 || bits == 32){
		AGIDL_LMPSyncPix(lmp,gxt->pixels.pix32);
	}
	else{
		AGIDL_LMPSyncPix16(lmp,gxt->pixels.pix16);
	}
	return lmp;
}

AGIDL_PVR* AGIDL_ConvertGXT2PVR(const AGIDL_GXT* gxt){
	char* imgname = AGIDL_GetImgName(gxt->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_GXT);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PVR* pvr = AGIDL_CreatePVR(filename,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),
	AGIDL_GXTGetClrFmt(gxt));
	AGIDL_PVRSetICPMode(pvr,gxt->icp);
	AGIDL_PVRSetMaxDiff(pvr,AGIDL_GXTGetMaxDiff(gxt));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt));
	
	if(bits == 24 || bits == 32){
		AGIDL_PVRSyncPix(pvr,gxt->pixels.pix32);
	}
	else{
		AGIDL_PVRSyncPix16(pvr,gxt->pixels.pix16);
	}
	return pvr;
}

AGIDL_BTI* AGIDL_ConvertGXT2BTI(const AGIDL_GXT* gxt){
	char* imgname = AGIDL_GetImgName(gxt->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_BTI);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_BTI* bti = AGIDL_CreateBTI(filename,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),
	AGIDL_GXTGetClrFmt(gxt));
	AGIDL_BTISetICPMode(bti,gxt->icp);
	AGIDL_BTISetMaxDiff(bti,AGIDL_GXTGetMaxDiff(gxt));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt));
	
	if(bits == 24 || bits == 32){
		AGIDL_BTISyncPix(bti,gxt->pixels.pix32);
	}
	else{
		AGIDL_BTISyncPix16(bti,gxt->pixels.pix16);
	}
	return bti;
}

AGIDL_3DF* AGIDL_ConvertGXT23DF(const AGIDL_GXT* gxt){
	char* imgname = AGIDL_GetImgName(gxt->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_3DF);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_3DF* glide = AGIDL_Create3DF(filename,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),
	AGIDL_GXTGetClrFmt(gxt));
	AGIDL_3DFSetICPMode(glide,gxt->icp);
	AGIDL_3DFSetMaxDiff(glide,AGIDL_GXTGetMaxDiff(gxt));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt));
	
	if(bits == 24 || bits == 32){
		AGIDL_3DFSyncPix(glide,gxt->pixels.pix32);
	}
	else{
		AGIDL_3DFSyncPix16(glide,gxt->pixels.pix16);
	}
	return glide;
}

AGIDL_PPM* AGIDL_ConvertGXT2PPM(const AGIDL_GXT* gxt){
	char* imgname = AGIDL_GetImgName(gxt->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PPM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PPM* pmm = AGIDL_CreatePPM(filename,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),
	AGIDL_GXTGetClrFmt(gxt));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt));
	
	if(bits == 24 || bits == 32){
		AGIDL_PPMSyncPix(pmm,gxt->pixels.pix32);
	}
	else{
		AGIDL_PPMSyncPix16(pmm,gxt->pixels.pix16);
	}
	return pmm;
}

AGIDL_LBM* AGIDL_ConvertGXT2LBM(const AGIDL_GXT* gxt){
	char* imgname = AGIDL_GetImgName(gxt->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_LBM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_LBM* lbm = AGIDL_CreateLBM(filename,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),
	AGIDL_GXTGetClrFmt(gxt));
	AGIDL_LBMSetICPMode(lbm,gxt->icp);
	AGIDL_LBMSetMaxDiff(lbm,AGIDL_GXTGetMaxDiff(gxt));

	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt));
	
	if(bits == 24 || bits == 32){
		AGIDL_LBMSyncPix(lbm,gxt->pixels.pix32);
	}
	else{
		AGIDL_LBMSyncPix16(lbm,gxt->pixels.pix16);
	}
	return lbm;
}

AGIDL_BMP* AGIDL_ConvertBTI2BMP(const AGIDL_BTI* bti){
	char* imgname = AGIDL_GetImgName(bti->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_BMP);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_BMP* bmp = AGIDL_CreateBMP(filename,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),
	AGIDL_BTIGetClrFmt(bti));
	AGIDL_BMPSetICPMode(bmp,bti->icp);
	AGIDL_BMPSetMaxDiff(bmp,AGIDL_BTIGetMaxDiff(bti));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti));
	
	if(bits == 24 || bits == 32){
		AGIDL_BMPSyncPix(bmp,bti->pixels.pix32);
	}
	else{
		AGIDL_BMPSyncPix16(bmp,bti->pixels.pix16);
	}
	return bmp;
}

AGIDL_TGA* AGIDL_ConvertBTI2TGA(const AGIDL_BTI* bti){
	char* imgname = AGIDL_GetImgName(bti->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_TGA);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_TGA* tga = AGIDL_CreateTGA(filename,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),
	AGIDL_BTIGetClrFmt(bti));
	AGIDL_TGASetICPMode(tga,bti->icp);
	AGIDL_TGASetMaxDiff(tga,AGIDL_BTIGetMaxDiff(bti));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti));
	
	if(bits == 24 || bits == 32){
		AGIDL_TGASyncPix(tga,bti->pixels.pix32);
	}
	else{
		AGIDL_TGASyncPix16(tga,bti->pixels.pix16);
	}
	return tga;
}

AGIDL_TIM* AGIDL_ConvertBTI2TIM(const AGIDL_BTI* bti){
	char* imgname = AGIDL_GetImgName(bti->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_TIM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_TIM* tim = AGIDL_CreateTIM(filename,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),
	AGIDL_BTIGetClrFmt(bti));
	AGIDL_TIMSetICPMode(tim,bti->icp);
	AGIDL_TIMSetMaxDiff(tim,AGIDL_BTIGetMaxDiff(bti));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti));
	
	if(bits == 24 || bits == 32){
		AGIDL_TIMSyncPix(tim,bti->pixels.pix32);
	}
	else{
		AGIDL_TIMSyncPix16(tim,bti->pixels.pix16);
	}
	return tim;
}

AGIDL_PCX* AGIDL_ConvertBTI2PCX(const AGIDL_BTI* bti){
	char* imgname = AGIDL_GetImgName(bti->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PCX);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PCX* pcx = AGIDL_CreatePCX(filename,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),
	AGIDL_BTIGetClrFmt(bti));
	AGIDL_PCXSetICPMode(pcx,bti->icp);
	AGIDL_PCXSetMaxDiff(pcx,AGIDL_BTIGetMaxDiff(bti));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti));
	
	if(bits == 24 || bits == 32){
		AGIDL_PCXSyncPix(pcx,bti->pixels.pix32);
	}
	else{
		AGIDL_PCXSyncPix16(pcx,bti->pixels.pix16);
	}
	return pcx;
}

AGIDL_LMP* AGIDL_ConvertBTI2LMP(const AGIDL_BTI* bti){
	char* imgname = AGIDL_GetImgName(bti->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_LMP);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_LMP* lmp = AGIDL_CreateLMP(filename,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),
	AGIDL_BTIGetClrFmt(bti));
	AGIDL_LMPSetMaxDiff(lmp,AGIDL_BTIGetMaxDiff(bti));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti));
	
	if(bits == 24 || bits == 32){
		AGIDL_LMPSyncPix(lmp,bti->pixels.pix32);
	}
	else{
		AGIDL_LMPSyncPix16(lmp,bti->pixels.pix16);
	}
	return lmp;
}

AGIDL_PVR* AGIDL_ConvertBTI2PVR(const AGIDL_BTI* bti){
	char* imgname = AGIDL_GetImgName(bti->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PVR);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PVR* pvr = AGIDL_CreatePVR(filename,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),
	AGIDL_BTIGetClrFmt(bti));
	AGIDL_PVRSetICPMode(pvr,bti->icp);
	AGIDL_PVRSetMaxDiff(pvr,AGIDL_BTIGetMaxDiff(bti));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti));
	
	if(bits == 24 || bits == 32){
		AGIDL_PVRSyncPix(pvr,bti->pixels.pix32);
	}
	else{
		AGIDL_PVRSyncPix16(pvr,bti->pixels.pix16);
	}
	return pvr;
}

AGIDL_GXT* AGIDL_ConvertBTI2GXT(const AGIDL_BTI* bti){
	char* imgname = AGIDL_GetImgName(bti->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_GXT);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_GXT* gxt = AGIDL_CreateGXT(filename,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),
	AGIDL_BTIGetClrFmt(bti));
	AGIDL_GXTSetICPMode(gxt,bti->icp);
	AGIDL_GXTSetMaxDiff(gxt,AGIDL_BTIGetMaxDiff(bti));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti));
	
	if(bits == 24 || bits == 32){
		AGIDL_GXTSyncPix(gxt,bti->pixels.pix32);
	}
	else{
		AGIDL_GXTSyncPix16(gxt,bti->pixels.pix16);
	}
	return gxt;
}

AGIDL_3DF* AGIDL_ConvertBTI23DF(const AGIDL_BTI* bti){
	char* imgname = AGIDL_GetImgName(bti->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_3DF);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_3DF* glide = AGIDL_Create3DF(filename,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),
	AGIDL_BTIGetClrFmt(bti));
	AGIDL_3DFSetICPMode(glide,bti->icp);
	AGIDL_3DFSetMaxDiff(glide,AGIDL_BTIGetMaxDiff(bti));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti));
	
	if(bits == 24 || bits == 32){
		AGIDL_3DFSyncPix(glide,bti->pixels.pix32);
	}
	else{
		AGIDL_3DFSyncPix16(glide,bti->pixels.pix16);
	}
	return glide;
}

AGIDL_PPM* AGIDL_ConvertBTI2PPM(const AGIDL_BTI* bti){
	char* imgname = AGIDL_GetImgName(bti->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PPM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PPM* pmm = AGIDL_CreatePPM(filename,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),
	AGIDL_BTIGetClrFmt(bti));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti));
	
	if(bits == 24 || bits == 32){
		AGIDL_PPMSyncPix(pmm,bti->pixels.pix32);
	}
	else{
		AGIDL_PPMSyncPix16(pmm,bti->pixels.pix16);
	}
	return pmm;
}

AGIDL_LBM* AGIDL_ConvertBTI2LBM(const AGIDL_BTI* bti){
	char* imgname = AGIDL_GetImgName(bti->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_LBM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_LBM* lbm = AGIDL_CreateLBM(filename,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),
	AGIDL_BTIGetClrFmt(bti));
	AGIDL_LBMSetICPMode(lbm,bti->icp);
	AGIDL_LBMSetMaxDiff(lbm,AGIDL_BTIGetMaxDiff(bti));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti));
	
	if(bits == 24 || bits == 32){
		AGIDL_LBMSyncPix(lbm,bti->pixels.pix32);
	}
	else{
		AGIDL_LBMSyncPix16(lbm,bti->pixels.pix16);
	}
	return lbm;
}

AGIDL_BMP* AGIDL_Convert3DF2BMP(const AGIDL_3DF* glide){
	char* imgname = AGIDL_GetImgName(glide->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_BMP);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_BMP* bmp = AGIDL_CreateBMP(filename,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),
	AGIDL_3DFGetClrFmt(glide));
	AGIDL_BMPSetICPMode(bmp,glide->icp);
	AGIDL_BMPSetMaxDiff(bmp,AGIDL_3DFGetMaxDiff(glide));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide));
	
	if(bits == 24 || bits == 32){
		AGIDL_BMPSyncPix(bmp,glide->pixels.pix32);
	}
	else{
		AGIDL_BMPSyncPix16(bmp,glide->pixels.pix16);
	}
	return bmp;
}

AGIDL_TGA* AGIDL_Convert3DF2TGA(const AGIDL_3DF* glide){
	char* imgname = AGIDL_GetImgName(glide->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_TGA);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_TGA* tga = AGIDL_CreateTGA(filename,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),
	AGIDL_3DFGetClrFmt(glide));
	AGIDL_TGASetICPMode(tga,glide->icp);
	AGIDL_TGASetMaxDiff(tga,AGIDL_3DFGetMaxDiff(glide));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide));
	
	if(bits == 24 || bits == 32){
		AGIDL_TGASyncPix(tga,glide->pixels.pix32);
	}
	else{
		AGIDL_TGASyncPix16(tga,glide->pixels.pix16);
	}
	return tga;
}

AGIDL_TIM* AGIDL_Convert3DF2TIM(const AGIDL_3DF* glide){
	char* imgname = AGIDL_GetImgName(glide->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_TIM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_TIM* tim = AGIDL_CreateTIM(filename,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),
	AGIDL_3DFGetClrFmt(glide));
	AGIDL_TIMSetICPMode(tim,glide->icp);
	AGIDL_TIMSetMaxDiff(tim,AGIDL_3DFGetMaxDiff(glide));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide));
	
	if(bits == 24 || bits == 32){
		AGIDL_TIMSyncPix(tim,glide->pixels.pix32);
	}
	else{
		AGIDL_TIMSyncPix16(tim,glide->pixels.pix16);
	}
	return tim;
}

AGIDL_PCX* AGIDL_Convert3DF2PCX(const AGIDL_3DF* glide){
	char* imgname = AGIDL_GetImgName(glide->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PCX);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PCX* pcx = AGIDL_CreatePCX(filename,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),
	AGIDL_3DFGetClrFmt(glide));
	AGIDL_PCXSetICPMode(pcx,glide->icp);
	AGIDL_PCXSetMaxDiff(pcx,AGIDL_3DFGetMaxDiff(glide));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide));
	
	if(bits == 24 || bits == 32){
		AGIDL_PCXSyncPix(pcx,glide->pixels.pix32);
	}
	else{
		AGIDL_PCXSyncPix16(pcx,glide->pixels.pix16);
	}
	return pcx;
}

AGIDL_LMP* AGIDL_Convert3DF2LMP(const AGIDL_3DF* glide){
	char* imgname = AGIDL_GetImgName(glide->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_LMP);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_LMP* lmp = AGIDL_CreateLMP(filename,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),
	AGIDL_3DFGetClrFmt(glide));
	AGIDL_LMPSetMaxDiff(lmp,AGIDL_3DFGetMaxDiff(glide));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide));
	
	if(bits == 24 || bits == 32){
		AGIDL_LMPSyncPix(lmp,glide->pixels.pix32);
	}
	else{
		AGIDL_LMPSyncPix16(lmp,glide->pixels.pix16);
	}
	return lmp;
}

AGIDL_PVR* AGIDL_Convert3DF2PVR(const AGIDL_3DF* glide){
	char* imgname = AGIDL_GetImgName(glide->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PVR);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PVR* pvr = AGIDL_CreatePVR(filename,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),
	AGIDL_3DFGetClrFmt(glide));
	AGIDL_PVRSetICPMode(pvr,glide->icp);
	AGIDL_PVRSetMaxDiff(pvr,AGIDL_3DFGetMaxDiff(glide));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide));
	
	if(bits == 24 || bits == 32){
		AGIDL_PVRSyncPix(pvr,glide->pixels.pix32);
	}
	else{
		AGIDL_PVRSyncPix16(pvr,glide->pixels.pix16);
	}
	return pvr;
}

AGIDL_GXT* AGIDL_Convert3DF2GXT(const AGIDL_3DF* glide){
	char* imgname = AGIDL_GetImgName(glide->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_GXT);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_GXT* gxt = AGIDL_CreateGXT(filename,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),
	AGIDL_3DFGetClrFmt(glide));
	AGIDL_GXTSetICPMode(gxt,glide->icp);
	AGIDL_GXTSetMaxDiff(gxt,AGIDL_3DFGetMaxDiff(glide));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide));
	
	if(bits == 24 || bits == 32){
		AGIDL_GXTSyncPix(gxt,glide->pixels.pix32);
	}
	else{
		AGIDL_GXTSyncPix16(gxt,glide->pixels.pix16);
	}
	return gxt;
}

AGIDL_BTI* AGIDL_Convert3DF2BTI(const AGIDL_3DF* glide){
	char* imgname = AGIDL_GetImgName(glide->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_BTI);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_BTI* bti = AGIDL_CreateBTI(filename,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),
	AGIDL_3DFGetClrFmt(glide));
	AGIDL_BTISetICPMode(bti,glide->icp);
	AGIDL_BTISetMaxDiff(bti,AGIDL_3DFGetMaxDiff(glide));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide));
	
	if(bits == 24 || bits == 32){
		AGIDL_BTISyncPix(bti,glide->pixels.pix32);
	}
	else{
		AGIDL_BTISyncPix16(bti,glide->pixels.pix16);
	}
	return bti;
}

AGIDL_PPM* AGIDL_Convert3DF2PPM(const AGIDL_3DF* glide){
	char* imgname = AGIDL_GetImgName(glide->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PPM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PPM* ppm = AGIDL_CreatePPM(filename,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),
	AGIDL_3DFGetClrFmt(glide));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide));
	
	if(bits == 24 || bits == 32){
		AGIDL_PPMSyncPix(ppm,glide->pixels.pix32);
	}
	else{
		AGIDL_PPMSyncPix16(ppm,glide->pixels.pix16);
	}
	return ppm;
}

AGIDL_LBM* AGIDL_Convert3DF2LBM(const AGIDL_3DF* glide){
	char* imgname = AGIDL_GetImgName(glide->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_LBM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_LBM* lbm = AGIDL_CreateLBM(filename,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),
	AGIDL_3DFGetClrFmt(glide));
	AGIDL_LBMSetICPMode(lbm,glide->icp);
	AGIDL_LBMSetMaxDiff(lbm,AGIDL_3DFGetMaxDiff(glide));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide));
	
	if(bits == 24 || bits == 32){
		AGIDL_LBMSyncPix(lbm,glide->pixels.pix32);
	}
	else{
		AGIDL_LBMSyncPix16(lbm,glide->pixels.pix16);
	}
	return lbm;
}

AGIDL_BMP* AGIDL_ConvertPPM2BMP(const AGIDL_PPM* ppm){
	char* imgname = AGIDL_GetImgName(ppm->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_BMP);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_BMP* bmp = AGIDL_CreateBMP(filename,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),
	AGIDL_PPMGetClrFmt(ppm));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm));
	
	if(bits == 24 || bits == 32){
		AGIDL_BMPSyncPix(bmp,ppm->pixels.pix32);
	}
	else{
		AGIDL_BMPSyncPix16(bmp,ppm->pixels.pix16);
	}
	return bmp;
}

AGIDL_TGA* AGIDL_ConvertPPM2TGA(const AGIDL_PPM* ppm){
	char* imgname = AGIDL_GetImgName(ppm->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_TGA);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_TGA* tga = AGIDL_CreateTGA(filename,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),
	AGIDL_PPMGetClrFmt(ppm));

	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm));
	
	if(bits == 24 || bits == 32){
		AGIDL_TGASyncPix(tga,ppm->pixels.pix32);
	}
	else{
		AGIDL_TGASyncPix16(tga,ppm->pixels.pix16);
	}
	return tga;
}

AGIDL_TIM* AGIDL_ConvertPPM2TIM(const AGIDL_PPM* ppm){
	char* imgname = AGIDL_GetImgName(ppm->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_TIM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_TIM* tim = AGIDL_CreateTIM(filename,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),
	AGIDL_PPMGetClrFmt(ppm));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm));
	
	if(bits == 24 || bits == 32){
		AGIDL_TIMSyncPix(tim,ppm->pixels.pix32);
	}
	else{
		AGIDL_TIMSyncPix16(tim,ppm->pixels.pix16);
	}
	return tim;
}

AGIDL_PCX* AGIDL_ConvertPPM2PCX(const AGIDL_PPM* ppm){
	char* imgname = AGIDL_GetImgName(ppm->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PCX);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PCX* pcx = AGIDL_CreatePCX(filename,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),
	AGIDL_PPMGetClrFmt(ppm));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm));
	
	if(bits == 24 || bits == 32){
		AGIDL_PCXSyncPix(pcx,ppm->pixels.pix32);
	}
	else{
		AGIDL_PCXSyncPix16(pcx,ppm->pixels.pix16);
	}
	return pcx;
}

AGIDL_LMP* AGIDL_ConvertPPM2LMP(const AGIDL_PPM* ppm){
	char* imgname = AGIDL_GetImgName(ppm->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_LMP);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_LMP* lmp = AGIDL_CreateLMP(filename,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),
	AGIDL_PPMGetClrFmt(ppm));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm));
	
	if(bits == 24 || bits == 32){
		AGIDL_LMPSyncPix(lmp,ppm->pixels.pix32);
	}
	else{
		AGIDL_LMPSyncPix16(lmp,ppm->pixels.pix16);
	}
	return lmp;
}

AGIDL_PVR* AGIDL_ConvertPPM2PVR(const AGIDL_PPM* ppm){
	char* imgname = AGIDL_GetImgName(ppm->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PVR);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PVR* pvr = AGIDL_CreatePVR(filename,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),
	AGIDL_PPMGetClrFmt(ppm));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm));
	
	if(bits == 24 || bits == 32){
		AGIDL_PVRSyncPix(pvr,ppm->pixels.pix32);
	}
	else{
		AGIDL_PVRSyncPix16(pvr,ppm->pixels.pix16);
	}
	return pvr;
}

AGIDL_GXT* AGIDL_ConvertPPM2GXT(const AGIDL_PPM* ppm){
	char* imgname = AGIDL_GetImgName(ppm->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_GXT);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_GXT* gxt = AGIDL_CreateGXT(filename,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),
	AGIDL_PPMGetClrFmt(ppm));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm));
	
	if(bits == 24 || bits == 32){
		AGIDL_GXTSyncPix(gxt,ppm->pixels.pix32);
	}
	else{
		AGIDL_GXTSyncPix16(gxt,ppm->pixels.pix16);
	}
	return gxt;
}

AGIDL_BTI* AGIDL_ConvertPPM2BTI(const AGIDL_PPM* ppm){
	char* imgname = AGIDL_GetImgName(ppm->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_BTI);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_BTI* bti = AGIDL_CreateBTI(filename,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),
	AGIDL_PPMGetClrFmt(ppm));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm));
	
	if(bits == 24 || bits == 32){
		AGIDL_BTISyncPix(bti,ppm->pixels.pix32);
	}
	else{
		AGIDL_BTISyncPix16(bti,ppm->pixels.pix16);
	}
	return bti;
}

AGIDL_3DF* AGIDL_ConvertPPM23DF(const AGIDL_PPM* ppm){
	char* imgname = AGIDL_GetImgName(ppm->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_3DF);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_3DF* glide = AGIDL_Create3DF(filename,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),
	AGIDL_PPMGetClrFmt(ppm));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm));
	
	if(bits == 24 || bits == 32){
		AGIDL_3DFSyncPix(glide,ppm->pixels.pix32);
	}
	else{
		AGIDL_3DFSyncPix16(glide,ppm->pixels.pix16);
	}
	return glide;
}

AGIDL_LBM* AGIDL_ConvertPPM2LBM(const AGIDL_PPM* ppm){
	char* imgname = AGIDL_GetImgName(ppm->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_LBM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_LBM* lbm = AGIDL_CreateLBM(filename,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),
	AGIDL_PPMGetClrFmt(ppm));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm));
	
	if(bits == 24 || bits == 32){
		AGIDL_LBMSyncPix(lbm,ppm->pixels.pix32);
	}
	else{
		AGIDL_LBMSyncPix16(lbm,ppm->pixels.pix16);
	}
	return lbm;
}

AGIDL_BMP* AGIDL_ConvertLBM2BMP(const AGIDL_LBM* lbm){
	char* imgname = AGIDL_GetImgName(lbm->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_BMP);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_BMP* bmp = AGIDL_CreateBMP(filename,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),
	AGIDL_LBMGetClrFmt(lbm));
	AGIDL_BMPSetICPMode(bmp,lbm->icp);
	AGIDL_BMPSetMaxDiff(bmp,AGIDL_LBMGetMaxDiff(lbm));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm));
	
	if(bits == 24 || bits == 32){
		AGIDL_BMPSyncPix(bmp,lbm->pixels.pix32);
	}
	else{
		AGIDL_BMPSyncPix16(bmp,lbm->pixels.pix16);
	}
	return bmp;
}

AGIDL_TGA* AGIDL_ConvertLBM2TGA(const AGIDL_LBM* lbm){
	char* imgname = AGIDL_GetImgName(lbm->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_TGA);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_TGA* tga = AGIDL_CreateTGA(filename,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),
	AGIDL_LBMGetClrFmt(lbm));
	AGIDL_TGASetICPMode(tga,lbm->icp);
	AGIDL_TGASetMaxDiff(tga,AGIDL_LBMGetMaxDiff(lbm));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm));
	
	if(bits == 24 || bits == 32){
		AGIDL_TGASyncPix(tga,lbm->pixels.pix32);
	}
	else{
		AGIDL_TGASyncPix16(tga,lbm->pixels.pix16);
	}
	return tga;
}

AGIDL_TIM* AGIDL_ConvertLBM2TIM(const AGIDL_LBM* lbm){
	char* imgname = AGIDL_GetImgName(lbm->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_TIM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_TIM* tim = AGIDL_CreateTIM(filename,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),
	AGIDL_LBMGetClrFmt(lbm));
	AGIDL_TIMSetICPMode(tim,lbm->icp);
	AGIDL_TIMSetMaxDiff(tim,AGIDL_LBMGetMaxDiff(lbm));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm));
	
	if(bits == 24 || bits == 32){
		AGIDL_TIMSyncPix(tim,lbm->pixels.pix32);
	}
	else{
		AGIDL_TIMSyncPix16(tim,lbm->pixels.pix16);
	}
	return tim;
}

AGIDL_PCX* AGIDL_ConvertLBM2PCX(const AGIDL_LBM* lbm){
	char* imgname = AGIDL_GetImgName(lbm->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PCX);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PCX* pcx = AGIDL_CreatePCX(filename,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),
	AGIDL_LBMGetClrFmt(lbm));
	AGIDL_PCXSetICPMode(pcx,lbm->icp);
	AGIDL_PCXSetMaxDiff(pcx,AGIDL_LBMGetMaxDiff(lbm));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm));
	
	if(bits == 24 || bits == 32){
		AGIDL_PCXSyncPix(pcx,lbm->pixels.pix32);
	}
	else{
		AGIDL_PCXSyncPix16(pcx,lbm->pixels.pix16);
	}
	return pcx;
}

AGIDL_LMP* AGIDL_ConvertLBM2LMP(const AGIDL_LBM* lbm){
	char* imgname = AGIDL_GetImgName(lbm->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_LMP);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_LMP* lmp = AGIDL_CreateLMP(filename,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),
	AGIDL_LBMGetClrFmt(lbm));
	AGIDL_LMPSetMaxDiff(lmp,AGIDL_LBMGetMaxDiff(lbm));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm));
	
	if(bits == 24 || bits == 32){
		AGIDL_LMPSyncPix(lmp,lbm->pixels.pix32);
	}
	else{
		AGIDL_LMPSyncPix16(lmp,lbm->pixels.pix16);
	}
	return lmp;
}

AGIDL_PVR* AGIDL_ConvertLBM2PVR(const AGIDL_LBM* lbm){
	char* imgname = AGIDL_GetImgName(lbm->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PVR);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PVR* pvr = AGIDL_CreatePVR(filename,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),
	AGIDL_LBMGetClrFmt(lbm));
	AGIDL_PVRSetICPMode(pvr,lbm->icp);
	AGIDL_PVRSetMaxDiff(pvr,AGIDL_LBMGetMaxDiff(lbm));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm));
	
	if(bits == 24 || bits == 32){
		AGIDL_PVRSyncPix(pvr,lbm->pixels.pix32);
	}
	else{
		AGIDL_PVRSyncPix16(pvr,lbm->pixels.pix16);
	}
	return pvr;
}

AGIDL_GXT* AGIDL_ConvertLBM2GXT(const AGIDL_LBM* lbm){
	char* imgname = AGIDL_GetImgName(lbm->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_GXT);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_GXT* gxt = AGIDL_CreateGXT(filename,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),
	AGIDL_LBMGetClrFmt(lbm));
	AGIDL_GXTSetICPMode(gxt,lbm->icp);
	AGIDL_GXTSetMaxDiff(gxt,AGIDL_LBMGetMaxDiff(lbm));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm));
	
	if(bits == 24 || bits == 32){
		AGIDL_GXTSyncPix(gxt,lbm->pixels.pix32);
	}
	else{
		AGIDL_GXTSyncPix16(gxt,lbm->pixels.pix16);
	}
	return gxt;
}

AGIDL_3DF* AGIDL_ConvertLBM23DF(const AGIDL_LBM* lbm){
	char* imgname = AGIDL_GetImgName(lbm->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_3DF);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_3DF* glide = AGIDL_Create3DF(filename,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),
	AGIDL_LBMGetClrFmt(lbm));
	AGIDL_3DFSetICPMode(glide,lbm->icp);
	AGIDL_3DFSetMaxDiff(glide,AGIDL_LBMGetMaxDiff(lbm));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm));
	
	if(bits == 24 || bits == 32){
		AGIDL_3DFSyncPix(glide,lbm->pixels.pix32);
	}
	else{
		AGIDL_3DFSyncPix16(glide,lbm->pixels.pix16);
	}
	return glide;
}

AGIDL_PPM* AGIDL_ConvertLBM2PPM(const AGIDL_LBM* lbm){
	char* imgname = AGIDL_GetImgName(lbm->filename);
	char* ext = AGIDL_GetImgExtension(AGIDL_IMG_PPM);
	char* filename = AGIDL_StrCpy(imgname,ext);
	
	AGIDL_PPM* ppm = AGIDL_CreatePPM(filename,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),
	AGIDL_LBMGetClrFmt(lbm));
	
	free(imgname);
	free(ext);
	free(filename);

	const int bits = AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm));
	
	if(bits == 24 || bits == 32){
		AGIDL_PPMSyncPix(ppm,lbm->pixels.pix32);
	}
	else{
		AGIDL_PPMSyncPix16(ppm,lbm->pixels.pix16);
	}
	return ppm;
}