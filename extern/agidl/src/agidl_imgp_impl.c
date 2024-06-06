/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_impl.c
*   Date: 12/17/2023
*   Version: 0.2b
*   Updated: 4/14/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include "agidl_imgp_impl.h"

void AGIDL_GrayscaleBMP(AGIDL_BMP* bmp){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 16){
		AGIDL_GrayscaleImgData(bmp->pixels.pix16,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),
		AGIDL_BMPGetClrFmt(bmp));
	}
	else{
		AGIDL_GrayscaleImgData(bmp->pixels.pix32,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),
		AGIDL_BMPGetClrFmt(bmp));
	}
}

void AGIDL_GrayscaleTGA(AGIDL_TGA* tga){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		AGIDL_GrayscaleImgData(tga->pixels.pix16,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),
		AGIDL_TGAGetClrFmt(tga));
	}
	else{
		AGIDL_GrayscaleImgData(tga->pixels.pix32,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),
		AGIDL_TGAGetClrFmt(tga));
	}
}

void AGIDL_GrayscaleTIM(AGIDL_TIM* tim){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 16){
		AGIDL_GrayscaleImgData(tim->pixels.pix16,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),
		AGIDL_TIMGetClrFmt(tim));
	}
	else{
		AGIDL_GrayscaleImgData(tim->pixels.pix32,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),
		AGIDL_TIMGetClrFmt(tim));
	}
}

void AGIDL_GrayscalePCX(AGIDL_PCX* pcx){
	if(AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx)) == 16){
		AGIDL_GrayscaleImgData(pcx->pixels.pix16,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),
		AGIDL_PCXGetClrFmt(pcx));
	}
	else{
		AGIDL_GrayscaleImgData(pcx->pixels.pix32,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),
		AGIDL_PCXGetClrFmt(pcx));
	}
}

void AGIDL_GrayscaleLMP(AGIDL_LMP* lmp){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) == 16){
		AGIDL_GrayscaleImgData(lmp->pixels.pix16,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),
		AGIDL_LMPGetClrFmt(lmp));
	}
	else{
		AGIDL_GrayscaleImgData(lmp->pixels.pix32,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),
		AGIDL_LMPGetClrFmt(lmp));
	}
}

void AGIDL_GrayscalePVR(AGIDL_PVR* pvr){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) == 16){
		AGIDL_GrayscaleImgData(pvr->pixels.pix16,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),
		AGIDL_PVRGetClrFmt(pvr));
	}
	else{
		AGIDL_GrayscaleImgData(pvr->pixels.pix32,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),
		AGIDL_PVRGetClrFmt(pvr));
	}
}

void AGIDL_GrayscaleGXT(AGIDL_GXT* gxt){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 16){
		AGIDL_GrayscaleImgData(gxt->pixels.pix16,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),
		AGIDL_GXTGetClrFmt(gxt));
	}
	else{
		AGIDL_GrayscaleImgData(gxt->pixels.pix32,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),
		AGIDL_GXTGetClrFmt(gxt));
	}
}

void AGIDL_GrayscaleBTI(AGIDL_BTI* bti){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		AGIDL_GrayscaleImgData(bti->pixels.pix16,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),
		AGIDL_BTIGetClrFmt(bti));
	}
	else{
		AGIDL_GrayscaleImgData(bti->pixels.pix32,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),
		AGIDL_BTIGetClrFmt(bti));
	}
}

void AGIDL_Grayscale3DF(AGIDL_3DF* glide){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 16){
		AGIDL_GrayscaleImgData(glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),
		AGIDL_3DFGetClrFmt(glide));
	}
	else{
		AGIDL_GrayscaleImgData(glide->pixels.pix32,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),
		AGIDL_3DFGetClrFmt(glide));
	}
}

void AGIDL_GrayscalePPM(AGIDL_PPM* ppm){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 16){
		AGIDL_GrayscaleImgData(ppm->pixels.pix16,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),
		AGIDL_PPMGetClrFmt(ppm));
	}
	else{
		AGIDL_GrayscaleImgData(ppm->pixels.pix32,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),
		AGIDL_PPMGetClrFmt(ppm));
	}
}

void AGIDL_GrayscaleLBM(AGIDL_LBM* lbm){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 16){
		AGIDL_GrayscaleImgData(lbm->pixels.pix16,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),
		AGIDL_LBMGetClrFmt(lbm));
	}
	else{
		AGIDL_GrayscaleImgData(lbm->pixels.pix32,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),
		AGIDL_LBMGetClrFmt(lbm));
	}
}

void AGIDL_FlipHorzBMP(AGIDL_BMP* bmp){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 16){
		AGIDL_FlipImgDataHorz(bmp->pixels.pix16,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),
		AGIDL_BMPGetClrFmt(bmp));
	}
	else{
		AGIDL_FlipImgDataHorz(bmp->pixels.pix32,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),
		AGIDL_BMPGetClrFmt(bmp));
	}
}

void AGIDL_FlipHorzTGA(AGIDL_TGA* tga){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		AGIDL_FlipImgDataHorz(tga->pixels.pix16,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),
		AGIDL_TGAGetClrFmt(tga));
	}
	else{
		AGIDL_FlipImgDataHorz(tga->pixels.pix32,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),
		AGIDL_TGAGetClrFmt(tga));
	}
}

void AGIDL_FlipHorzTIM(AGIDL_TIM* tim){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 16){
		AGIDL_FlipImgDataHorz(tim->pixels.pix16,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),
		AGIDL_TIMGetClrFmt(tim));
	}
	else{
		AGIDL_FlipImgDataHorz(tim->pixels.pix32,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),
		AGIDL_TIMGetClrFmt(tim));
	}
}

void AGIDL_FlipHorzPCX(AGIDL_PCX* pcx){
	if(AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx)) == 16){
		AGIDL_FlipImgDataHorz(pcx->pixels.pix16,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),
		AGIDL_PCXGetClrFmt(pcx));
	}
	else{
		AGIDL_FlipImgDataHorz(pcx->pixels.pix32,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),
		AGIDL_PCXGetClrFmt(pcx));
	}
}

void AGIDL_FlipHorzLMP(AGIDL_LMP* lmp){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) == 16){
		AGIDL_FlipImgDataHorz(lmp->pixels.pix16,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),
		AGIDL_LMPGetClrFmt(lmp));
	}
	else{
		AGIDL_FlipImgDataHorz(lmp->pixels.pix32,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),
		AGIDL_LMPGetClrFmt(lmp));
	}
}

void AGIDL_FlipHorzPVR(AGIDL_PVR* pvr){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) == 16){
		AGIDL_FlipImgDataHorz(pvr->pixels.pix16,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),
		AGIDL_PVRGetClrFmt(pvr));
	}
	else{
		AGIDL_FlipImgDataHorz(pvr->pixels.pix32,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),
		AGIDL_PVRGetClrFmt(pvr));
	}
}

void AGIDL_FlipHorzGXT(AGIDL_GXT* gxt){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 16){
		AGIDL_FlipImgDataHorz(gxt->pixels.pix16,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),
		AGIDL_GXTGetClrFmt(gxt));
	}
	else{
		AGIDL_FlipImgDataHorz(gxt->pixels.pix32,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),
		AGIDL_GXTGetClrFmt(gxt));
	}
}

void AGIDL_FlipHorzBTI(AGIDL_BTI* bti){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		AGIDL_FlipImgDataHorz(bti->pixels.pix16,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),
		AGIDL_BTIGetClrFmt(bti));
	}
	else{
		AGIDL_FlipImgDataHorz(bti->pixels.pix32,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),
		AGIDL_BTIGetClrFmt(bti));
	}
}

void AGIDL_FlipHorz3DF(AGIDL_3DF* glide){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 16){
		AGIDL_FlipImgDataHorz(glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),
		AGIDL_3DFGetClrFmt(glide));
	}
	else{
		AGIDL_FlipImgDataHorz(glide->pixels.pix32,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),
		AGIDL_3DFGetClrFmt(glide));
	}
}

void AGIDL_FlipHorzPPM(AGIDL_PPM* ppm){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 16){
		AGIDL_FlipImgDataHorz(ppm->pixels.pix16,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),
		AGIDL_PPMGetClrFmt(ppm));
	}
	else{
		AGIDL_FlipImgDataHorz(ppm->pixels.pix32,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),
		AGIDL_PPMGetClrFmt(ppm));
	}
}

void AGIDL_FlipHorzLBM(AGIDL_LBM* lbm){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 16){
		AGIDL_FlipImgDataHorz(lbm->pixels.pix16,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),
		AGIDL_LBMGetClrFmt(lbm));
	}
	else{
		AGIDL_FlipImgDataHorz(lbm->pixels.pix32,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),
		AGIDL_LBMGetClrFmt(lbm));
	}
}

void AGIDL_FlipVertBMP(AGIDL_BMP* bmp){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 16){
		AGIDL_FlipImgDataVert(bmp->pixels.pix16,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),
		AGIDL_BMPGetClrFmt(bmp));
	}
	else{
		AGIDL_FlipImgDataVert(bmp->pixels.pix32,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),
		AGIDL_BMPGetClrFmt(bmp));
	}
}

void AGIDL_FlipVertTGA(AGIDL_TGA* tga){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		AGIDL_FlipImgDataVert(tga->pixels.pix16,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),
		AGIDL_TGAGetClrFmt(tga));
	}
	else{
		AGIDL_FlipImgDataVert(tga->pixels.pix32,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),
		AGIDL_TGAGetClrFmt(tga));
	}
}

void AGIDL_FlipVertTIM(AGIDL_TIM* tim){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 16){
		AGIDL_FlipImgDataVert(tim->pixels.pix16,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),
		AGIDL_TIMGetClrFmt(tim));
	}
	else{
		AGIDL_FlipImgDataVert(tim->pixels.pix32,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),
		AGIDL_TIMGetClrFmt(tim));
	}
}

void AGIDL_FlipVertPCX(AGIDL_PCX* pcx){
	if(AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx)) == 16){
		AGIDL_FlipImgDataVert(pcx->pixels.pix16,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),
		AGIDL_PCXGetClrFmt(pcx));
	}
	else{
		AGIDL_FlipImgDataVert(pcx->pixels.pix32,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),
		AGIDL_PCXGetClrFmt(pcx));
	}
}

void AGIDL_FlipVertLMP(AGIDL_LMP* lmp){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) == 16){
		AGIDL_FlipImgDataVert(lmp->pixels.pix16,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),
		AGIDL_LMPGetClrFmt(lmp));
	}
	else{
		AGIDL_FlipImgDataVert(lmp->pixels.pix32,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),
		AGIDL_LMPGetClrFmt(lmp));
	}
}

void AGIDL_FlipVertPVR(AGIDL_PVR* pvr){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) == 16){
		AGIDL_FlipImgDataVert(pvr->pixels.pix16,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),
		AGIDL_PVRGetClrFmt(pvr));
	}
	else{
		AGIDL_FlipImgDataVert(pvr->pixels.pix32,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),
		AGIDL_PVRGetClrFmt(pvr));
	}
}

void AGIDL_FlipVertGXT(AGIDL_GXT* gxt){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 16){
		AGIDL_FlipImgDataVert(gxt->pixels.pix16,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),
		AGIDL_GXTGetClrFmt(gxt));
	}
	else{
		AGIDL_FlipImgDataVert(gxt->pixels.pix32,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),
		AGIDL_GXTGetClrFmt(gxt));
	}
}

void AGIDL_FlipVertBTI(AGIDL_BTI* bti){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		AGIDL_FlipImgDataVert(bti->pixels.pix16,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),
		AGIDL_BTIGetClrFmt(bti));
	}
	else{
		AGIDL_FlipImgDataVert(bti->pixels.pix32,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),
		AGIDL_BTIGetClrFmt(bti));
	}
}

void AGIDL_FlipVert3DF(AGIDL_3DF* glide){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 16){
		AGIDL_FlipImgDataVert(glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),
		AGIDL_3DFGetClrFmt(glide));
	}
	else{
		AGIDL_FlipImgDataVert(glide->pixels.pix32,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),
		AGIDL_3DFGetClrFmt(glide));
	}
}

void AGIDL_FlipVertPPM(AGIDL_PPM* ppm){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 16){
		AGIDL_FlipImgDataVert(ppm->pixels.pix16,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),
		AGIDL_PPMGetClrFmt(ppm));
	}
	else{
		AGIDL_FlipImgDataVert(ppm->pixels.pix32,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),
		AGIDL_PPMGetClrFmt(ppm));
	}
}

void AGIDL_FlipVertLBM(AGIDL_LBM* lbm){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 16){
		AGIDL_FlipImgDataVert(lbm->pixels.pix16,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),
		AGIDL_LBMGetClrFmt(lbm));
	}
	else{
		AGIDL_FlipImgDataVert(lbm->pixels.pix32,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),
		AGIDL_LBMGetClrFmt(lbm));
	}
}

void AGIDL_FlipHorzAndVertBMP(AGIDL_BMP* bmp){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 16){
		AGIDL_FlipImgDataHorzAndVert(bmp->pixels.pix16,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),
		AGIDL_BMPGetClrFmt(bmp));
	}
	else{
		AGIDL_FlipImgDataHorzAndVert(bmp->pixels.pix32,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),
		AGIDL_BMPGetClrFmt(bmp));
	}
}

void AGIDL_FlipHorzAndVertTGA(AGIDL_TGA* tga){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		AGIDL_FlipImgDataHorzAndVert(tga->pixels.pix16,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),
		AGIDL_TGAGetClrFmt(tga));
	}
	else{
		AGIDL_FlipImgDataHorzAndVert(tga->pixels.pix32,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),
		AGIDL_TGAGetClrFmt(tga));
	}
}

void AGIDL_FlipHorzAndVertTIM(AGIDL_TIM* tim){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 16){
		AGIDL_FlipImgDataHorzAndVert(tim->pixels.pix16,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),
		AGIDL_TIMGetClrFmt(tim));
	}
	else{
		AGIDL_FlipImgDataHorzAndVert(tim->pixels.pix32,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),
		AGIDL_TIMGetClrFmt(tim));
	}
}

void AGIDL_FlipHorzAndVertPCX(AGIDL_PCX* pcx){
	if(AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx)) == 16){
		AGIDL_FlipImgDataHorzAndVert(pcx->pixels.pix16,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),
		AGIDL_PCXGetClrFmt(pcx));
	}
	else{
		AGIDL_FlipImgDataHorzAndVert(pcx->pixels.pix32,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),
		AGIDL_PCXGetClrFmt(pcx));
	}
}

void AGIDL_FlipHorzAndVertLMP(AGIDL_LMP* lmp){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) == 16){
		AGIDL_FlipImgDataHorzAndVert(lmp->pixels.pix16,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),
		AGIDL_LMPGetClrFmt(lmp));
	}
	else{
		AGIDL_FlipImgDataHorzAndVert(lmp->pixels.pix32,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),
		AGIDL_LMPGetClrFmt(lmp));
	}
}

void AGIDL_FlipHorzAndVertPVR(AGIDL_PVR* pvr){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) == 16){
		AGIDL_FlipImgDataHorzAndVert(pvr->pixels.pix16,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),
		AGIDL_PVRGetClrFmt(pvr));
	}
	else{
		AGIDL_FlipImgDataHorzAndVert(pvr->pixels.pix32,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),
		AGIDL_PVRGetClrFmt(pvr));
	}
}

void AGIDL_FlipHorzAndVertGXT(AGIDL_GXT* gxt){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 16){
		AGIDL_FlipImgDataHorzAndVert(gxt->pixels.pix16,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),
		AGIDL_GXTGetClrFmt(gxt));
	}
	else{
		AGIDL_FlipImgDataHorzAndVert(gxt->pixels.pix32,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),
		AGIDL_GXTGetClrFmt(gxt));
	}
}

void AGIDL_FlipHorzAndVertBTI(AGIDL_BTI* bti){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		AGIDL_FlipImgDataHorzAndVert(bti->pixels.pix16,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),
		AGIDL_BTIGetClrFmt(bti));
	}
	else{
		AGIDL_FlipImgDataHorzAndVert(bti->pixels.pix32,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),
		AGIDL_BTIGetClrFmt(bti));
	}
}

void AGIDL_FlipHorzAndVert3DF(AGIDL_3DF* glide){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 16){
		AGIDL_FlipImgDataHorzAndVert(glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),
		AGIDL_3DFGetClrFmt(glide));
	}
	else{
		AGIDL_FlipImgDataHorzAndVert(glide->pixels.pix32,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),
		AGIDL_3DFGetClrFmt(glide));
	}
}

void AGIDL_FlipHorzAndVertPPM(AGIDL_PPM* ppm){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 16){
		AGIDL_FlipImgDataHorzAndVert(ppm->pixels.pix16,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),
		AGIDL_PPMGetClrFmt(ppm));
	}
	else{
		AGIDL_FlipImgDataHorzAndVert(ppm->pixels.pix32,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),
		AGIDL_PPMGetClrFmt(ppm));
	}
}

void AGIDL_FlipHorzAndVertLBM(AGIDL_LBM* lbm){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 16){
		AGIDL_FlipImgDataHorzAndVert(lbm->pixels.pix16,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),
		AGIDL_LBMGetClrFmt(lbm));
	}
	else{
		AGIDL_FlipImgDataHorzAndVert(lbm->pixels.pix32,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),
		AGIDL_LBMGetClrFmt(lbm));
	}
}

void AGIDL_RotateBMP(AGIDL_BMP* bmp, AGIDL_ANGLE angle){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 16){
		AGIDL_RotateImgData(bmp->pixels.pix16,&bmp->header.width,&bmp->header.height,
		AGIDL_BMPGetClrFmt(bmp),angle);
	}
	else{
		AGIDL_RotateImgData(bmp->pixels.pix32,&bmp->header.width,&bmp->header.height,
		AGIDL_BMPGetClrFmt(bmp),angle);
	}
}

void AGIDL_RotateTGA(AGIDL_TGA* tga, AGIDL_ANGLE angle){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		AGIDL_RotateImgData(tga->pixels.pix16,&tga->header.width,&tga->header.height,
		AGIDL_TGAGetClrFmt(tga),angle);
	}
	else{
		AGIDL_RotateImgData(tga->pixels.pix32,&tga->header.width,&tga->header.height,
		AGIDL_TGAGetClrFmt(tga),angle);
	}
}

void AGIDL_RotateTIM(AGIDL_TIM* tim, AGIDL_ANGLE angle){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 16){
		AGIDL_RotateImgData(tim->pixels.pix16,&tim->img_header.width,&tim->img_header.height,
		AGIDL_TIMGetClrFmt(tim),angle);
	}
	else{
		AGIDL_RotateImgData(tim->pixels.pix32,&tim->img_header.width,&tim->img_header.height,
		AGIDL_TIMGetClrFmt(tim),angle);
	}
}

void AGIDL_RotatePCX(AGIDL_PCX* pcx, AGIDL_ANGLE angle){
	if(AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx)) == 16){
		AGIDL_RotateImgData(pcx->pixels.pix16,&pcx->header.width,&pcx->header.height,
		AGIDL_PCXGetClrFmt(pcx),angle);
	}
	else{
		AGIDL_RotateImgData(pcx->pixels.pix32,&pcx->header.width,&pcx->header.height,
		AGIDL_PCXGetClrFmt(pcx),angle);
	}
}

void AGIDL_RotateLMP(AGIDL_LMP* lmp, AGIDL_ANGLE angle){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) == 16){
		AGIDL_RotateImgData(lmp->pixels.pix16,&lmp->width,&lmp->height,
		AGIDL_LMPGetClrFmt(lmp),angle);
	}
	else{
		AGIDL_RotateImgData(lmp->pixels.pix32,&lmp->width,&lmp->height,
		AGIDL_LMPGetClrFmt(lmp),angle);
	}
}

void AGIDL_RotatePVR(AGIDL_PVR* pvr, AGIDL_ANGLE angle){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) == 16){
		AGIDL_RotateImgData(pvr->pixels.pix16,&pvr->header.width,&pvr->header.height,
		AGIDL_PVRGetClrFmt(pvr),angle);
	}
	else{
		AGIDL_RotateImgData(pvr->pixels.pix32,&pvr->header.width,&pvr->header.height,
		AGIDL_PVRGetClrFmt(pvr),angle);
	}
}

void AGIDL_RotateGXT(AGIDL_GXT* gxt, AGIDL_ANGLE angle){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 16){
		AGIDL_RotateImgData(gxt->pixels.pix16,&gxt->header.header.width,&gxt->header.header.height,
		AGIDL_GXTGetClrFmt(gxt),angle);
	}
	else{
		AGIDL_RotateImgData(gxt->pixels.pix32,&gxt->header.header.width,&gxt->header.header.height,
		AGIDL_GXTGetClrFmt(gxt),angle);
	}
}

void AGIDL_RotateBTI(AGIDL_BTI* bti, AGIDL_ANGLE angle){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		AGIDL_RotateImgData(bti->pixels.pix16,&bti->header.width,&bti->header.height,
		AGIDL_BTIGetClrFmt(bti),angle);
	}
	else{
		AGIDL_RotateImgData(bti->pixels.pix32,&bti->header.width,&bti->header.height,
		AGIDL_BTIGetClrFmt(bti),angle);
	}
}

void AGIDL_Rotate3DF(AGIDL_3DF* glide, AGIDL_ANGLE angle){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 16){
		AGIDL_RotateImgData(glide->pixels.pix16,&glide->width,&glide->height,
		AGIDL_3DFGetClrFmt(glide),angle);
	}
	else{
		AGIDL_RotateImgData(glide->pixels.pix32,&glide->width,&glide->height,
		AGIDL_3DFGetClrFmt(glide),angle);
	}
}

void AGIDL_RotatePPM(AGIDL_PPM* ppm, AGIDL_ANGLE angle){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 16){
		AGIDL_RotateImgData(ppm->pixels.pix16,&ppm->width,&ppm->height,
		AGIDL_PPMGetClrFmt(ppm),angle);
	}
	else{
		AGIDL_RotateImgData(ppm->pixels.pix32,&ppm->width,&ppm->height,
		AGIDL_PPMGetClrFmt(ppm),angle);
	}
}

void AGIDL_RotateLBM(AGIDL_LBM* lbm, AGIDL_ANGLE angle){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 16){
		AGIDL_RotateImgData(lbm->pixels.pix16,&lbm->header.bmhd.width,&lbm->header.bmhd.height,
		AGIDL_LBMGetClrFmt(lbm),angle);
	}
	else{
		AGIDL_RotateImgData(lbm->pixels.pix32,&lbm->header.bmhd.width,&lbm->header.bmhd.height,
		AGIDL_LBMGetClrFmt(lbm),angle);
	}
}

void AGIDL_ScaleBMP(AGIDL_BMP* bmp, float sx, float sy, AGIDL_SCALE scale){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 16){
		u16* img_data = (u16*)malloc(sizeof(u16)*AGIDL_BMPGetSize(bmp));
		AGIDL_ClrMemcpy16(img_data,bmp->pixels.pix16,AGIDL_BMPGetSize(bmp));
		free(bmp->pixels.pix16);
		bmp->pixels.pix16 = (COLOR16*)AGIDL_ScaleImgData(img_data,&bmp->header.width,&bmp->header.height,
		sx,sy,scale,AGIDL_BMPGetClrFmt(bmp));
	}
	else{
		u32* img_data = (u32*)malloc(sizeof(u32)*AGIDL_BMPGetSize(bmp));
		AGIDL_ClrMemcpy(img_data,bmp->pixels.pix32,AGIDL_BMPGetSize(bmp));
		free(bmp->pixels.pix32);
		bmp->pixels.pix32 = (COLOR*)AGIDL_ScaleImgData(img_data,&bmp->header.width,&bmp->header.height,
		sx,sy,scale,AGIDL_BMPGetClrFmt(bmp));
	}
}

void AGIDL_ScaleTGA(AGIDL_TGA* tga, float sx, float sy, AGIDL_SCALE scale){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		u16* img_data = (u16*)malloc(sizeof(u16)*AGIDL_TGAGetSize(tga));
		AGIDL_ClrMemcpy16(img_data,tga->pixels.pix16,AGIDL_TGAGetSize(tga));
		free(tga->pixels.pix16);
		tga->pixels.pix16 = (COLOR16*)AGIDL_ScaleImgData(img_data,&tga->header.width,&tga->header.height,
		sx,sy,scale,AGIDL_TGAGetClrFmt(tga));
	}
	else{
		u32* img_data = (u32*)malloc(sizeof(u32)*AGIDL_TGAGetSize(tga));
		AGIDL_ClrMemcpy(img_data,tga->pixels.pix32,AGIDL_TGAGetSize(tga));
		free(tga->pixels.pix32);
		tga->pixels.pix32 = (COLOR*)AGIDL_ScaleImgData(img_data,&tga->header.width,&tga->header.height,
		sx,sy,scale,AGIDL_TGAGetClrFmt(tga));
	}
}

void AGIDL_ScaleTIM(AGIDL_TIM* tim, float sx, float sy, AGIDL_SCALE scale){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 16){
		u16* img_data = (u16*)malloc(sizeof(u16)*AGIDL_TIMGetSize(tim));
		AGIDL_ClrMemcpy16(img_data,tim->pixels.pix16,AGIDL_TIMGetSize(tim));
		free(tim->pixels.pix16);
		tim->pixels.pix16 = (COLOR16*)AGIDL_ScaleImgData(img_data,&tim->img_header.width,&tim->img_header.height,
		sx,sy,scale,AGIDL_TIMGetClrFmt(tim));
	}
	else{
		u32* img_data = (u32*)malloc(sizeof(u32)*AGIDL_TIMGetSize(tim));
		AGIDL_ClrMemcpy(img_data,tim->pixels.pix32,AGIDL_TIMGetSize(tim));
		free(tim->pixels.pix32);
		tim->pixels.pix32 = (COLOR*)AGIDL_ScaleImgData(img_data,&tim->img_header.width,&tim->img_header.height,
		sx,sy,scale,AGIDL_TIMGetClrFmt(tim));
	}
}

void AGIDL_ScalePCX(AGIDL_PCX* pcx, float sx, float sy, AGIDL_SCALE scale){
	if(AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx)) == 16){
		u16* img_data = (u16*)malloc(sizeof(u16)*AGIDL_PCXGetSize(pcx));
		AGIDL_ClrMemcpy16(img_data,pcx->pixels.pix16,AGIDL_PCXGetSize(pcx));
		free(pcx->pixels.pix16);
		pcx->pixels.pix16 = (COLOR16*)AGIDL_ScaleImgData(img_data,&pcx->header.width,&pcx->header.height,
		sx,sy,scale,AGIDL_PCXGetClrFmt(pcx));
	}
	else{
		u32* img_data = (u32*)malloc(sizeof(u32)*AGIDL_PCXGetSize(pcx));
		AGIDL_ClrMemcpy(img_data,pcx->pixels.pix32,AGIDL_PCXGetSize(pcx));
		free(pcx->pixels.pix32);
		pcx->pixels.pix32 = (COLOR*)AGIDL_ScaleImgData(img_data,&pcx->header.width,&pcx->header.height,
		sx,sy,scale,AGIDL_PCXGetClrFmt(pcx));
	}
}

void AGIDL_ScaleLMP(AGIDL_LMP* lmp, float sx, float sy, AGIDL_SCALE scale){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) == 16){
		u16* img_data = (u16*)malloc(sizeof(u16)*AGIDL_LMPGetSize(lmp));
		AGIDL_ClrMemcpy16(img_data,lmp->pixels.pix16,AGIDL_LMPGetSize(lmp));
		free(lmp->pixels.pix16);
		lmp->pixels.pix16 = (COLOR16*)AGIDL_ScaleImgData(img_data,&lmp->width,&lmp->height,
		sx,sy,scale,AGIDL_LMPGetClrFmt(lmp));
	}
	else{
		u32* img_data = (u32*)malloc(sizeof(u32)*AGIDL_LMPGetSize(lmp));
		AGIDL_ClrMemcpy(img_data,lmp->pixels.pix32,AGIDL_LMPGetSize(lmp));
		free(lmp->pixels.pix32);
		lmp->pixels.pix32 = (COLOR*)AGIDL_ScaleImgData(img_data,&lmp->width,&lmp->height,
		sx,sy,scale,AGIDL_LMPGetClrFmt(lmp));
	}
}

void AGIDL_ScalePVR(AGIDL_PVR* pvr, float sx, float sy, AGIDL_SCALE scale){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) == 16){
		u16* img_data = (u16*)malloc(sizeof(u16)*AGIDL_PVRGetSize(pvr));
		AGIDL_ClrMemcpy16(img_data,pvr->pixels.pix16,AGIDL_PVRGetSize(pvr));
		free(pvr->pixels.pix16);
		pvr->pixels.pix16 = (COLOR16*)AGIDL_ScaleImgData(img_data,&pvr->header.width,&pvr->header.height,
		sx,sy,scale,AGIDL_PVRGetClrFmt(pvr));
	}
	else{
		u32* img_data = (u32*)malloc(sizeof(u32)*AGIDL_PVRGetSize(pvr));
		AGIDL_ClrMemcpy(img_data,pvr->pixels.pix32,AGIDL_PVRGetSize(pvr));
		free(pvr->pixels.pix32);
		pvr->pixels.pix32 = (COLOR*)AGIDL_ScaleImgData(img_data,&pvr->header.width,&pvr->header.height,
		sx,sy,scale,AGIDL_PVRGetClrFmt(pvr));
	}
}

void AGIDL_ScaleGXT(AGIDL_GXT* gxt, float sx, float sy, AGIDL_SCALE scale){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 16){
		u16* img_data = (u16*)malloc(sizeof(u16)*AGIDL_GXTGetSize(gxt));
		AGIDL_ClrMemcpy16(img_data,gxt->pixels.pix16,AGIDL_GXTGetSize(gxt));
		free(gxt->pixels.pix16);
		gxt->pixels.pix16 = (COLOR16*)AGIDL_ScaleImgData(img_data,&gxt->header.header.width,&gxt->header.header.height,
		sx,sy,scale,AGIDL_GXTGetClrFmt(gxt));
	}
	else{
		u32* img_data = (u32*)malloc(sizeof(u32)*AGIDL_GXTGetSize(gxt));
		AGIDL_ClrMemcpy(img_data,gxt->pixels.pix32,AGIDL_GXTGetSize(gxt));
		free(gxt->pixels.pix32);
		gxt->pixels.pix32 = (COLOR*)AGIDL_ScaleImgData(img_data,&gxt->header.header.width,&gxt->header.header.height,
		sx,sy,scale,AGIDL_GXTGetClrFmt(gxt));
	}
}

void AGIDL_ScaleBTI(AGIDL_BTI* bti, float sx, float sy, AGIDL_SCALE scale){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		u16* img_data = (u16*)malloc(sizeof(u16)*AGIDL_BTIGetSize(bti));
		AGIDL_ClrMemcpy16(img_data,bti->pixels.pix16,AGIDL_BTIGetSize(bti));
		free(bti->pixels.pix16);
		bti->pixels.pix16 = (COLOR16*)AGIDL_ScaleImgData(img_data,&bti->header.width,&bti->header.height,
		sx,sy,scale,AGIDL_BTIGetClrFmt(bti));
	}
	else{
		u32* img_data = (u32*)malloc(sizeof(u32)*AGIDL_BTIGetSize(bti));
		AGIDL_ClrMemcpy(img_data,bti->pixels.pix32,AGIDL_BTIGetSize(bti));
		free(bti->pixels.pix32);
		bti->pixels.pix32 = (COLOR*)AGIDL_ScaleImgData(img_data,&bti->header.width,&bti->header.height,
		sx,sy,scale,AGIDL_BTIGetClrFmt(bti));
	}
}

void AGIDL_Scale3DF(AGIDL_3DF* glide, float sx, float sy, AGIDL_SCALE scale){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 16){
		u16* img_data = (u16*)malloc(sizeof(u16)*AGIDL_3DFGetSize(glide));
		AGIDL_ClrMemcpy16(img_data,glide->pixels.pix16,AGIDL_3DFGetSize(glide));
		free(glide->pixels.pix16);
		glide->pixels.pix16 = (COLOR16*)AGIDL_ScaleImgData(img_data,&glide->width,&glide->height,
		sx,sy,scale,AGIDL_3DFGetClrFmt(glide));
	}
	else{
		u32* img_data = (u32*)malloc(sizeof(u32)*AGIDL_3DFGetSize(glide));
		AGIDL_ClrMemcpy(img_data,glide->pixels.pix32,AGIDL_3DFGetSize(glide));
		free(glide->pixels.pix32);
		glide->pixels.pix32 = (COLOR*)AGIDL_ScaleImgData(img_data,&glide->width,&glide->height,
		sx,sy,scale,AGIDL_3DFGetClrFmt(glide));
	}
}

void AGIDL_ScalePPM(AGIDL_PPM* ppm, float sx, float sy, AGIDL_SCALE scale){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 16){
		u16* img_data = (u16*)malloc(sizeof(u16)*AGIDL_PPMGetSize(ppm));
		AGIDL_ClrMemcpy16(img_data,ppm->pixels.pix16,AGIDL_PPMGetSize(ppm));
		free(ppm->pixels.pix16);
		ppm->pixels.pix16 = (COLOR16*)AGIDL_ScaleImgData(img_data,&ppm->width,&ppm->height,
		sx,sy,scale,AGIDL_PPMGetClrFmt(ppm));
	}
	else{
		u32* img_data = (u32*)malloc(sizeof(u32)*AGIDL_PPMGetSize(ppm));
		AGIDL_ClrMemcpy(img_data,ppm->pixels.pix32,AGIDL_PPMGetSize(ppm));
		free(ppm->pixels.pix32);
		ppm->pixels.pix32 = (COLOR*)AGIDL_ScaleImgData(img_data,&ppm->width,&ppm->height,
		sx,sy,scale,AGIDL_PPMGetClrFmt(ppm));
	}
}

void AGIDL_ScaleLBM(AGIDL_LBM* lbm, float sx, float sy, AGIDL_SCALE scale){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 16){
		u16* img_data = (u16*)malloc(sizeof(u16)*AGIDL_LBMGetSize(lbm));
		AGIDL_ClrMemcpy16(img_data,lbm->pixels.pix16,AGIDL_LBMGetSize(lbm));
		free(lbm->pixels.pix16);
		lbm->pixels.pix16 = (COLOR16*)AGIDL_ScaleImgData(img_data,&lbm->header.bmhd.width,&lbm->header.bmhd.height,
		sx,sy,scale,AGIDL_LBMGetClrFmt(lbm));
	}
	else{
		u32* img_data = (u32*)malloc(sizeof(u32)*AGIDL_LBMGetSize(lbm));
		AGIDL_ClrMemcpy(img_data,lbm->pixels.pix32,AGIDL_LBMGetSize(lbm));
		free(lbm->pixels.pix32);
		lbm->pixels.pix32 = (COLOR*)AGIDL_ScaleImgData(img_data,&lbm->header.bmhd.width,&lbm->header.bmhd.height,
		sx,sy,scale,AGIDL_LBMGetClrFmt(lbm));
	}
}

void AGIDL_FastScaleBMP(AGIDL_BMP* bmp, float sx, float sy, AGIDL_SCALE scale){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 16){
		u16* img_data = (u16*)malloc(sizeof(u16)*AGIDL_BMPGetSize(bmp));
		AGIDL_ClrMemcpy16(img_data,bmp->pixels.pix16,AGIDL_BMPGetSize(bmp));
		free(bmp->pixels.pix16);
		bmp->pixels.pix16 = (COLOR16*)AGIDL_FastScaleImgData(img_data,&bmp->header.width,&bmp->header.height,
		sx,sy,scale,AGIDL_BMPGetClrFmt(bmp));
	}
	else{
		u32* img_data = (u32*)malloc(sizeof(u32)*AGIDL_BMPGetSize(bmp));
		AGIDL_ClrMemcpy(img_data,bmp->pixels.pix32,AGIDL_BMPGetSize(bmp));
		free(bmp->pixels.pix32);
		bmp->pixels.pix32 = (COLOR*)AGIDL_FastScaleImgData(img_data,&bmp->header.width,&bmp->header.height,
		sx,sy,scale,AGIDL_BMPGetClrFmt(bmp));
	}
}

void AGIDL_FastScaleTGA(AGIDL_TGA* tga, float sx, float sy, AGIDL_SCALE scale){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		u16* img_data = (u16*)malloc(sizeof(u16)*AGIDL_TGAGetSize(tga));
		AGIDL_ClrMemcpy16(img_data,tga->pixels.pix16,AGIDL_TGAGetSize(tga));
		free(tga->pixels.pix16);
		tga->pixels.pix16 = (COLOR16*)AGIDL_FastScaleImgData(img_data,&tga->header.width,&tga->header.height,
		sx,sy,scale,AGIDL_TGAGetClrFmt(tga));
	}
	else{
		u32* img_data = (u32*)malloc(sizeof(u32)*AGIDL_TGAGetSize(tga));
		AGIDL_ClrMemcpy(img_data,tga->pixels.pix32,AGIDL_TGAGetSize(tga));
		free(tga->pixels.pix32);
		tga->pixels.pix32 = (COLOR*)AGIDL_FastScaleImgData(img_data,&tga->header.width,&tga->header.height,
		sx,sy,scale,AGIDL_TGAGetClrFmt(tga));
	}
}

void AGIDL_FastScaleTIM(AGIDL_TIM* tim, float sx, float sy, AGIDL_SCALE scale){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 16){
		u16* img_data = (u16*)malloc(sizeof(u16)*AGIDL_TIMGetSize(tim));
		AGIDL_ClrMemcpy16(img_data,tim->pixels.pix16,AGIDL_TIMGetSize(tim));
		free(tim->pixels.pix16);
		tim->pixels.pix16 = (COLOR16*)AGIDL_FastScaleImgData(img_data,&tim->img_header.width,&tim->img_header.height,
		sx,sy,scale,AGIDL_TIMGetClrFmt(tim));
	}
	else{
		u32* img_data = (u32*)malloc(sizeof(u32)*AGIDL_TIMGetSize(tim));
		AGIDL_ClrMemcpy(img_data,tim->pixels.pix32,AGIDL_TIMGetSize(tim));
		free(tim->pixels.pix32);
		tim->pixels.pix32 = (COLOR*)AGIDL_FastScaleImgData(img_data,&tim->img_header.width,&tim->img_header.height,
		sx,sy,scale,AGIDL_TIMGetClrFmt(tim));
	}
}

void AGIDL_FastScalePCX(AGIDL_PCX* pcx, float sx, float sy, AGIDL_SCALE scale){
	if(AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx)) == 16){
		u16* img_data = (u16*)malloc(sizeof(u16)*AGIDL_PCXGetSize(pcx));
		AGIDL_ClrMemcpy16(img_data,pcx->pixels.pix16,AGIDL_PCXGetSize(pcx));
		free(pcx->pixels.pix16);
		pcx->pixels.pix16 = (COLOR16*)AGIDL_FastScaleImgData(img_data,&pcx->header.width,&pcx->header.height,
		sx,sy,scale,AGIDL_PCXGetClrFmt(pcx));
	}
	else{
		u32* img_data = (u32*)malloc(sizeof(u32)*AGIDL_PCXGetSize(pcx));
		AGIDL_ClrMemcpy(img_data,pcx->pixels.pix32,AGIDL_PCXGetSize(pcx));
		free(pcx->pixels.pix32);
		pcx->pixels.pix32 = (COLOR*)AGIDL_FastScaleImgData(img_data,&pcx->header.width,&pcx->header.height,
		sx,sy,scale,AGIDL_PCXGetClrFmt(pcx));
	}
}

void AGIDL_FastScaleLMP(AGIDL_LMP* lmp, float sx, float sy, AGIDL_SCALE scale){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) == 16){
		u16* img_data = (u16*)malloc(sizeof(u16)*AGIDL_LMPGetSize(lmp));
		AGIDL_ClrMemcpy16(img_data,lmp->pixels.pix16,AGIDL_LMPGetSize(lmp));
		free(lmp->pixels.pix16);
		lmp->pixels.pix16 = (COLOR16*)AGIDL_FastScaleImgData(img_data,&lmp->width,&lmp->height,
		sx,sy,scale,AGIDL_LMPGetClrFmt(lmp));
	}
	else{
		u32* img_data = (u32*)malloc(sizeof(u32)*AGIDL_LMPGetSize(lmp));
		AGIDL_ClrMemcpy(img_data,lmp->pixels.pix32,AGIDL_LMPGetSize(lmp));
		free(lmp->pixels.pix32);
		lmp->pixels.pix32 = (COLOR*)AGIDL_FastScaleImgData(img_data,&lmp->width,&lmp->height,
		sx,sy,scale,AGIDL_LMPGetClrFmt(lmp));
	}
}

void AGIDL_FastScalePVR(AGIDL_PVR* pvr, float sx, float sy, AGIDL_SCALE scale){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) == 16){
		u16* img_data = (u16*)malloc(sizeof(u16)*AGIDL_PVRGetSize(pvr));
		AGIDL_ClrMemcpy16(img_data,pvr->pixels.pix16,AGIDL_PVRGetSize(pvr));
		free(pvr->pixels.pix16);
		pvr->pixels.pix16 = (COLOR16*)AGIDL_FastScaleImgData(img_data,&pvr->header.width,&pvr->header.height,
		sx,sy,scale,AGIDL_PVRGetClrFmt(pvr));
	}
	else{
		u32* img_data = (u32*)malloc(sizeof(u32)*AGIDL_PVRGetSize(pvr));
		AGIDL_ClrMemcpy(img_data,pvr->pixels.pix32,AGIDL_PVRGetSize(pvr));
		free(pvr->pixels.pix32);
		pvr->pixels.pix32 = (COLOR*)AGIDL_FastScaleImgData(img_data,&pvr->header.width,&pvr->header.height,
		sx,sy,scale,AGIDL_PVRGetClrFmt(pvr));
	}
}

void AGIDL_FastScaleGXT(AGIDL_GXT* gxt, float sx, float sy, AGIDL_SCALE scale){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 16){
		u16* img_data = (u16*)malloc(sizeof(u16)*AGIDL_GXTGetSize(gxt));
		AGIDL_ClrMemcpy16(img_data,gxt->pixels.pix16,AGIDL_GXTGetSize(gxt));
		free(gxt->pixels.pix16);
		gxt->pixels.pix16 = (COLOR16*)AGIDL_FastScaleImgData(img_data,&gxt->header.header.width,&gxt->header.header.height,
		sx,sy,scale,AGIDL_GXTGetClrFmt(gxt));
	}
	else{
		u32* img_data = (u32*)malloc(sizeof(u32)*AGIDL_GXTGetSize(gxt));
		AGIDL_ClrMemcpy(img_data,gxt->pixels.pix32,AGIDL_GXTGetSize(gxt));
		free(gxt->pixels.pix32);
		gxt->pixels.pix32 = (COLOR*)AGIDL_FastScaleImgData(img_data,&gxt->header.header.width,&gxt->header.header.height,
		sx,sy,scale,AGIDL_GXTGetClrFmt(gxt));
	}
}

void AGIDL_FastScaleBTI(AGIDL_BTI* bti, float sx, float sy, AGIDL_SCALE scale){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		u16* img_data = (u16*)malloc(sizeof(u16)*AGIDL_BTIGetSize(bti));
		AGIDL_ClrMemcpy16(img_data,bti->pixels.pix16,AGIDL_BTIGetSize(bti));
		free(bti->pixels.pix16);
		bti->pixels.pix16 = (COLOR16*)AGIDL_FastScaleImgData(img_data,&bti->header.width,&bti->header.height,
		sx,sy,scale,AGIDL_BTIGetClrFmt(bti));
	}
	else{
		u32* img_data = (u32*)malloc(sizeof(u32)*AGIDL_BTIGetSize(bti));
		AGIDL_ClrMemcpy(img_data,bti->pixels.pix32,AGIDL_BTIGetSize(bti));
		free(bti->pixels.pix32);
		bti->pixels.pix32 = (COLOR*)AGIDL_FastScaleImgData(img_data,&bti->header.width,&bti->header.height,
		sx,sy,scale,AGIDL_BTIGetClrFmt(bti));
	}
}

void AGIDL_FastScale3DF(AGIDL_3DF* glide, float sx, float sy, AGIDL_SCALE scale){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 16){
		u16* img_data = (u16*)malloc(sizeof(u16)*AGIDL_3DFGetSize(glide));
		AGIDL_ClrMemcpy16(img_data,glide->pixels.pix16,AGIDL_3DFGetSize(glide));
		free(glide->pixels.pix16);
		glide->pixels.pix16 = (COLOR16*)AGIDL_FastScaleImgData(img_data,&glide->width,&glide->height,
		sx,sy,scale,AGIDL_3DFGetClrFmt(glide));
	}
	else{
		u32* img_data = (u32*)malloc(sizeof(u32)*AGIDL_3DFGetSize(glide));
		AGIDL_ClrMemcpy(img_data,glide->pixels.pix32,AGIDL_3DFGetSize(glide));
		free(glide->pixels.pix32);
		glide->pixels.pix32 = (COLOR*)AGIDL_FastScaleImgData(img_data,&glide->width,&glide->height,
		sx,sy,scale,AGIDL_3DFGetClrFmt(glide));
	}
}

void AGIDL_FastScalePPM(AGIDL_PPM* ppm, float sx, float sy, AGIDL_SCALE scale){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 16){
		u16* img_data = (u16*)malloc(sizeof(u16)*AGIDL_PPMGetSize(ppm));
		AGIDL_ClrMemcpy16(img_data,ppm->pixels.pix16,AGIDL_PPMGetSize(ppm));
		free(ppm->pixels.pix16);
		ppm->pixels.pix16 = (COLOR16*)AGIDL_FastScaleImgData(img_data,&ppm->width,&ppm->height,
		sx,sy,scale,AGIDL_PPMGetClrFmt(ppm));
	}
	else{
		u32* img_data = (u32*)malloc(sizeof(u32)*AGIDL_PPMGetSize(ppm));
		AGIDL_ClrMemcpy(img_data,ppm->pixels.pix32,AGIDL_PPMGetSize(ppm));
		free(ppm->pixels.pix32);
		ppm->pixels.pix32 = (COLOR*)AGIDL_FastScaleImgData(img_data,&ppm->width,&ppm->height,
		sx,sy,scale,AGIDL_PPMGetClrFmt(ppm));
	}
}

void AGIDL_FastScaleLBM(AGIDL_LBM* lbm, float sx, float sy, AGIDL_SCALE scale){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 16){
		u16* img_data = (u16*)malloc(sizeof(u16)*AGIDL_LBMGetSize(lbm));
		AGIDL_ClrMemcpy16(img_data,lbm->pixels.pix16,AGIDL_LBMGetSize(lbm));
		free(lbm->pixels.pix16);
		lbm->pixels.pix16 = (COLOR16*)AGIDL_FastScaleImgData(img_data,&lbm->header.bmhd.width,&lbm->header.bmhd.height,
		sx,sy,scale,AGIDL_LBMGetClrFmt(lbm));
	}
	else{
		u32* img_data = (u32*)malloc(sizeof(u32)*AGIDL_LBMGetSize(lbm));
		AGIDL_ClrMemcpy(img_data,lbm->pixels.pix32,AGIDL_LBMGetSize(lbm));
		free(lbm->pixels.pix32);
		lbm->pixels.pix32 = (COLOR*)AGIDL_FastScaleImgData(img_data,&lbm->header.bmhd.width,&lbm->header.bmhd.height,
		sx,sy,scale,AGIDL_LBMGetClrFmt(lbm));
	}
}

void AGIDL_FilterBilerpBMP(AGIDL_BMP* bmp){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 16){
		AGIDL_FilterImgDataBilerp(bmp->pixels.pix16,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),
		AGIDL_BMPGetClrFmt(bmp));
	}
	else{
		AGIDL_FilterImgDataBilerp(bmp->pixels.pix32,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),
		AGIDL_BMPGetClrFmt(bmp));
	}
}

void AGIDL_FilterBilerpTGA(AGIDL_TGA* tga){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		AGIDL_FilterImgDataBilerp(tga->pixels.pix16,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),
		AGIDL_TGAGetClrFmt(tga));
	}
	else{
		AGIDL_FilterImgDataBilerp(tga->pixels.pix32,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),
		AGIDL_TGAGetClrFmt(tga));
	}
}

void AGIDL_FilterBilerpTIM(AGIDL_TIM* tim){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 16){
		AGIDL_FilterImgDataBilerp(tim->pixels.pix16,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),
		AGIDL_TIMGetClrFmt(tim));
	}
	else{
		AGIDL_FilterImgDataBilerp(tim->pixels.pix32,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),
		AGIDL_TIMGetClrFmt(tim));
	}
}

void AGIDL_FilterBilerpPCX(AGIDL_PCX* pcx){
	if(AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx)) == 16){
		AGIDL_FilterImgDataBilerp(pcx->pixels.pix16,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),
		AGIDL_PCXGetClrFmt(pcx));
	}
	else{
		AGIDL_FilterImgDataBilerp(pcx->pixels.pix32,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),
		AGIDL_PCXGetClrFmt(pcx));
	}
}

void AGIDL_FilterBilerpLMP(AGIDL_LMP* lmp){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) == 16){
		AGIDL_FilterImgDataBilerp(lmp->pixels.pix16,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),
		AGIDL_LMPGetClrFmt(lmp));
	}
	else{
		AGIDL_FilterImgDataBilerp(lmp->pixels.pix32,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),
		AGIDL_LMPGetClrFmt(lmp));
	}
}

void AGIDL_FilterBilerpPVR(AGIDL_PVR* pvr){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) == 16){
		AGIDL_FilterImgDataBilerp(pvr->pixels.pix16,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),
		AGIDL_PVRGetClrFmt(pvr));
	}
	else{
		AGIDL_FilterImgDataBilerp(pvr->pixels.pix32,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),
		AGIDL_PVRGetClrFmt(pvr));
	}
}

void AGIDL_FilterBilerpGXT(AGIDL_GXT* gxt){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 16){
		AGIDL_FilterImgDataBilerp(gxt->pixels.pix16,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),
		AGIDL_GXTGetClrFmt(gxt));
	}
	else{
		AGIDL_FilterImgDataBilerp(gxt->pixels.pix32,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),
		AGIDL_GXTGetClrFmt(gxt));
	}
}

void AGIDL_FilterBilerpBTI(AGIDL_BTI* bti){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		AGIDL_FilterImgDataBilerp(bti->pixels.pix16,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),
		AGIDL_BTIGetClrFmt(bti));
	}
	else{
		AGIDL_FilterImgDataBilerp(bti->pixels.pix32,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),
		AGIDL_BTIGetClrFmt(bti));
	}
}

void AGIDL_FilterBilerp3DF(AGIDL_3DF* glide){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 16){
		AGIDL_FilterImgDataBilerp(glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),
		AGIDL_3DFGetClrFmt(glide));
	}
	else{
		AGIDL_FilterImgDataBilerp(glide->pixels.pix32,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),
		AGIDL_3DFGetClrFmt(glide));
	}
}

void AGIDL_FilterBilerpPPM(AGIDL_PPM* ppm){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 16){
		AGIDL_FilterImgDataBilerp(ppm->pixels.pix16,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),
		AGIDL_PPMGetClrFmt(ppm));
	}
	else{
		AGIDL_FilterImgDataBilerp(ppm->pixels.pix32,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),
		AGIDL_PPMGetClrFmt(ppm));
	}
}

void AGIDL_FilterBilerpLBM(AGIDL_LBM* lbm){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 16){
		AGIDL_FilterImgDataBilerp(lbm->pixels.pix16,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),
		AGIDL_LBMGetClrFmt(lbm));
	}
	else{
		AGIDL_FilterImgDataBilerp(lbm->pixels.pix32,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),
		AGIDL_LBMGetClrFmt(lbm));
	}
}

void AGIDL_FilterTrilerpBMP(AGIDL_BMP* bmp){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 16){
		AGIDL_FilterImgDataTrilerp(bmp->pixels.pix16,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),
		AGIDL_BMPGetClrFmt(bmp));
	}
	else{
		AGIDL_FilterImgDataTrilerp(bmp->pixels.pix32,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),
		AGIDL_BMPGetClrFmt(bmp));
	}
}

void AGIDL_FilterTrilerpTGA(AGIDL_TGA* tga){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		AGIDL_FilterImgDataTrilerp(tga->pixels.pix16,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),
		AGIDL_TGAGetClrFmt(tga));
	}
	else{
		AGIDL_FilterImgDataTrilerp(tga->pixels.pix32,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),
		AGIDL_TGAGetClrFmt(tga));
	}
}

void AGIDL_FilterTrilerpTIM(AGIDL_TIM* tim){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 16){
		AGIDL_FilterImgDataTrilerp(tim->pixels.pix16,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),
		AGIDL_TIMGetClrFmt(tim));
	}
	else{
		AGIDL_FilterImgDataTrilerp(tim->pixels.pix32,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),
		AGIDL_TIMGetClrFmt(tim));
	}
}

void AGIDL_FilterTrilerpPCX(AGIDL_PCX* pcx){
	if(AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx)) == 16){
		AGIDL_FilterImgDataTrilerp(pcx->pixels.pix16,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),
		AGIDL_PCXGetClrFmt(pcx));
	}
	else{
		AGIDL_FilterImgDataTrilerp(pcx->pixels.pix32,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),
		AGIDL_PCXGetClrFmt(pcx));
	}
}

void AGIDL_FilterTrilerpLMP(AGIDL_LMP* lmp){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) == 16){
		AGIDL_FilterImgDataTrilerp(lmp->pixels.pix16,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),
		AGIDL_LMPGetClrFmt(lmp));
	}
	else{
		AGIDL_FilterImgDataTrilerp(lmp->pixels.pix32,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),
		AGIDL_LMPGetClrFmt(lmp));
	}
}

void AGIDL_FilterTrilerpPVR(AGIDL_PVR* pvr){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) == 16){
		AGIDL_FilterImgDataTrilerp(pvr->pixels.pix16,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),
		AGIDL_PVRGetClrFmt(pvr));
	}
	else{
		AGIDL_FilterImgDataTrilerp(pvr->pixels.pix32,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),
		AGIDL_PVRGetClrFmt(pvr));
	}
}

void AGIDL_FilterTrilerpGXT(AGIDL_GXT* gxt){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 16){
		AGIDL_FilterImgDataTrilerp(gxt->pixels.pix16,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),
		AGIDL_GXTGetClrFmt(gxt));
	}
	else{
		AGIDL_FilterImgDataTrilerp(gxt->pixels.pix32,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),
		AGIDL_GXTGetClrFmt(gxt));
	}
}

void AGIDL_FilterTrilerpBTI(AGIDL_BTI* bti){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		AGIDL_FilterImgDataTrilerp(bti->pixels.pix16,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),
		AGIDL_BTIGetClrFmt(bti));
	}
	else{
		AGIDL_FilterImgDataTrilerp(bti->pixels.pix32,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),
		AGIDL_BTIGetClrFmt(bti));
	}
}

void AGIDL_FilterTrilerp3DF(AGIDL_3DF* glide){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 16){
		AGIDL_FilterImgDataTrilerp(glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),
		AGIDL_3DFGetClrFmt(glide));
	}
	else{
		AGIDL_FilterImgDataTrilerp(glide->pixels.pix32,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),
		AGIDL_3DFGetClrFmt(glide));
	}
}

void AGIDL_FilterTrilerpPPM(AGIDL_PPM* ppm){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 16){
		AGIDL_FilterImgDataTrilerp(ppm->pixels.pix16,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),
		AGIDL_PPMGetClrFmt(ppm));
	}
	else{
		AGIDL_FilterImgDataTrilerp(ppm->pixels.pix32,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),
		AGIDL_PPMGetClrFmt(ppm));
	}
}

void AGIDL_FilterTrilerpLBM(AGIDL_LBM* lbm){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 16){
		AGIDL_FilterImgDataTrilerp(lbm->pixels.pix16,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),
		AGIDL_LBMGetClrFmt(lbm));
	}
	else{
		AGIDL_FilterImgDataTrilerp(lbm->pixels.pix32,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),
		AGIDL_LBMGetClrFmt(lbm));
	}
}

void AGIDL_MirrorBMP(AGIDL_BMP* bmp, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 16){
		bmp->pixels.pix16 = (COLOR16*)AGIDL_MirrorImgData(bmp->pixels.pix16,&bmp->header.width,&bmp->header.height,AGIDL_BMPGetClrFmt(bmp),mirror);
	}
	else{
		bmp->pixels.pix32 = (COLOR*)AGIDL_MirrorImgData(bmp->pixels.pix32,&bmp->header.width,&bmp->header.height,AGIDL_BMPGetClrFmt(bmp),mirror);
	}
}

void AGIDL_MirrorTGA(AGIDL_TGA* tga, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		tga->pixels.pix16 = (COLOR16*)AGIDL_MirrorImgData(tga->pixels.pix16,&tga->header.width,&tga->header.height,AGIDL_TGAGetClrFmt(tga),mirror);
	}
	else{
		tga->pixels.pix32 = (COLOR*)AGIDL_MirrorImgData(tga->pixels.pix32,&tga->header.width,&tga->header.height,AGIDL_TGAGetClrFmt(tga),mirror);
	}
}

void AGIDL_MirrorTIM(AGIDL_TIM* tim, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 16){
		tim->pixels.pix16 = (COLOR16*)AGIDL_MirrorImgData(tim->pixels.pix16,&tim->img_header.width,&tim->img_header.height,AGIDL_TIMGetClrFmt(tim),mirror);
	}
	else{
		tim->pixels.pix32 = (COLOR*)AGIDL_MirrorImgData(tim->pixels.pix32,&tim->img_header.width,&tim->img_header.height,AGIDL_TIMGetClrFmt(tim),mirror);
	}
}

void AGIDL_MirrorPCX(AGIDL_PCX* pcx, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx)) == 16){
		pcx->pixels.pix16 = (COLOR16*)AGIDL_MirrorImgData(pcx->pixels.pix16,&pcx->header.width,&pcx->header.height,AGIDL_PCXGetClrFmt(pcx),mirror);
	}
	else{
		pcx->pixels.pix32 = (COLOR*)AGIDL_MirrorImgData(pcx->pixels.pix32,&pcx->header.width,&pcx->header.height,AGIDL_PCXGetClrFmt(pcx),mirror);
	}
}

void AGIDL_MirrorLMP(AGIDL_LMP* lmp, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) == 16){
		lmp->pixels.pix16 = (COLOR16*)AGIDL_MirrorImgData(lmp->pixels.pix16,&lmp->width,&lmp->height,AGIDL_LMPGetClrFmt(lmp),mirror);
	}
	else{
		lmp->pixels.pix32 = (COLOR*)AGIDL_MirrorImgData(lmp->pixels.pix32,&lmp->width,&lmp->height,AGIDL_LMPGetClrFmt(lmp),mirror);
	}
}

void AGIDL_MirrorPVR(AGIDL_PVR* pvr, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) == 16){
		pvr->pixels.pix16 = (COLOR16*)AGIDL_MirrorImgData(pvr->pixels.pix16,&pvr->header.width,&pvr->header.height,AGIDL_PVRGetClrFmt(pvr),mirror);
	}
	else{
		pvr->pixels.pix32 = (COLOR*)AGIDL_MirrorImgData(pvr->pixels.pix32,&pvr->header.width,&pvr->header.height,AGIDL_PVRGetClrFmt(pvr),mirror);
	}
}

void AGIDL_MirrorGXT(AGIDL_GXT* gxt, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 16){
		gxt->pixels.pix16 = (COLOR16*)AGIDL_MirrorImgData(gxt->pixels.pix16,&gxt->header.header.width,&gxt->header.header.height,AGIDL_GXTGetClrFmt(gxt),mirror);
	}
	else{
		gxt->pixels.pix32 = (COLOR*)AGIDL_MirrorImgData(gxt->pixels.pix32,&gxt->header.header.width,&gxt->header.header.height,AGIDL_GXTGetClrFmt(gxt),mirror);
	}
}

void AGIDL_MirrorBTI(AGIDL_BTI* bti, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		bti->pixels.pix16 = (COLOR16*)AGIDL_MirrorImgData(bti->pixels.pix16,&bti->header.width,&bti->header.height,AGIDL_BTIGetClrFmt(bti),mirror);
	}
	else{
		bti->pixels.pix32 = (COLOR*)AGIDL_MirrorImgData(bti->pixels.pix32,&bti->header.width,&bti->header.height,AGIDL_BTIGetClrFmt(bti),mirror);
	}
}

void AGIDL_Mirror3DF(AGIDL_3DF* glide, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 16){
		glide->pixels.pix16 = (COLOR16*)AGIDL_MirrorImgData(glide->pixels.pix16,&glide->width,&glide->height,AGIDL_3DFGetClrFmt(glide),mirror);
	}
	else{
		glide->pixels.pix32 = (COLOR*)AGIDL_MirrorImgData(glide->pixels.pix32,&glide->width,&glide->height,AGIDL_3DFGetClrFmt(glide),mirror);
	}
}

void AGIDL_MirrorPPM(AGIDL_PPM* ppm, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 16){
		ppm->pixels.pix16 = (COLOR16*)AGIDL_MirrorImgData(ppm->pixels.pix16,&ppm->width,&ppm->height,AGIDL_PPMGetClrFmt(ppm),mirror);
	}
	else{
		ppm->pixels.pix32 = (COLOR*)AGIDL_MirrorImgData(ppm->pixels.pix32,&ppm->width,&ppm->height,AGIDL_PPMGetClrFmt(ppm),mirror);
	}
}

void AGIDL_MirrorLBM(AGIDL_LBM* lbm, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 16){
		lbm->pixels.pix16 = (COLOR16*)AGIDL_MirrorImgData(lbm->pixels.pix16,&lbm->header.bmhd.width,&lbm->header.bmhd.height,AGIDL_LBMGetClrFmt(lbm),mirror);
	}
	else{
		lbm->pixels.pix32 = (COLOR*)AGIDL_MirrorImgData(lbm->pixels.pix32,&lbm->header.bmhd.width,&lbm->header.bmhd.height,AGIDL_LBMGetClrFmt(lbm),mirror);
	}
}

void AGIDL_WrapBMP(AGIDL_BMP* bmp, u8 num_of_wraps){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 16){
		bmp->pixels.pix16 = (COLOR16*)AGIDL_WrapImgData(bmp->pixels.pix16,&bmp->header.width,AGIDL_BMPGetHeight(bmp),AGIDL_BMPGetClrFmt(bmp),num_of_wraps);
	}
	else{
		bmp->pixels.pix32 = (COLOR*)AGIDL_WrapImgData(bmp->pixels.pix32,&bmp->header.width,AGIDL_BMPGetHeight(bmp),AGIDL_BMPGetClrFmt(bmp),num_of_wraps);
	}
}

void AGIDL_WrapTGA(AGIDL_TGA* tga, u8 num_of_wraps){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		tga->pixels.pix16 = (COLOR16*)AGIDL_WrapImgData(tga->pixels.pix16,&tga->header.width,AGIDL_TGAGetHeight(tga),AGIDL_TGAGetClrFmt(tga),num_of_wraps);
	}
	else{
		tga->pixels.pix32 = (COLOR*)AGIDL_WrapImgData(tga->pixels.pix32,&tga->header.width,AGIDL_TGAGetHeight(tga),AGIDL_TGAGetClrFmt(tga),num_of_wraps);
	}
}

void AGIDL_WrapTIM(AGIDL_TIM* tim, u8 num_of_wraps){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 16){
		tim->pixels.pix16 = (COLOR16*)AGIDL_WrapImgData(tim->pixels.pix16,&tim->img_header.width,AGIDL_TIMGetHeight(tim),AGIDL_TIMGetClrFmt(tim),num_of_wraps);
	}
	else{
		tim->pixels.pix32 = (COLOR*)AGIDL_WrapImgData(tim->pixels.pix32,&tim->img_header.width,AGIDL_TIMGetHeight(tim),AGIDL_TIMGetClrFmt(tim),num_of_wraps);
	}
}

void AGIDL_WrapPCX(AGIDL_PCX* pcx, u8 num_of_wraps){
	if(AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx)) == 16){
		pcx->pixels.pix16 = (COLOR16*)AGIDL_WrapImgData(pcx->pixels.pix16,&pcx->header.width,AGIDL_PCXGetHeight(pcx),AGIDL_PCXGetClrFmt(pcx),num_of_wraps);
	}
	else{
		pcx->pixels.pix32 = (COLOR*)AGIDL_WrapImgData(pcx->pixels.pix32,&pcx->header.width,AGIDL_PCXGetHeight(pcx),AGIDL_PCXGetClrFmt(pcx),num_of_wraps);
	}
}

void AGIDL_WrapLMP(AGIDL_LMP* lmp, u8 num_of_wraps){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) == 16){
		lmp->pixels.pix16 = (COLOR16*)AGIDL_WrapImgData(lmp->pixels.pix16,&lmp->width,AGIDL_LMPGetHeight(lmp),AGIDL_LMPGetClrFmt(lmp),num_of_wraps);
	}
	else{
		lmp->pixels.pix32 = (COLOR*)AGIDL_WrapImgData(lmp->pixels.pix32,&lmp->width,AGIDL_LMPGetHeight(lmp),AGIDL_LMPGetClrFmt(lmp),num_of_wraps);
	}
}

void AGIDL_WrapPVR(AGIDL_PVR* pvr, u8 num_of_wraps){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) == 16){
		pvr->pixels.pix16 = (COLOR16*)AGIDL_WrapImgData(pvr->pixels.pix16,&pvr->header.width,AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr),num_of_wraps);
	}
	else{
		pvr->pixels.pix32 = (COLOR*)AGIDL_WrapImgData(pvr->pixels.pix32,&pvr->header.width,AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr),num_of_wraps);
	}
}

void AGIDL_WrapGXT(AGIDL_GXT* gxt, u8 num_of_wraps){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 16){
		gxt->pixels.pix16 = (COLOR16*)AGIDL_WrapImgData(gxt->pixels.pix16,&gxt->header.header.width,AGIDL_GXTGetHeight(gxt),AGIDL_GXTGetClrFmt(gxt),num_of_wraps);
	}
	else{
		gxt->pixels.pix32 = (COLOR*)AGIDL_WrapImgData(gxt->pixels.pix32,&gxt->header.header.width,AGIDL_GXTGetHeight(gxt),AGIDL_GXTGetClrFmt(gxt),num_of_wraps);
	}
}

void AGIDL_WrapBTI(AGIDL_BTI* bti, u8 num_of_wraps){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		bti->pixels.pix16 = (COLOR16*)AGIDL_WrapImgData(bti->pixels.pix16,&bti->header.width,AGIDL_BTIGetHeight(bti),AGIDL_BTIGetClrFmt(bti),num_of_wraps);
	}
	else{
		bti->pixels.pix32 = (COLOR*)AGIDL_WrapImgData(bti->pixels.pix32,&bti->header.width,AGIDL_BTIGetHeight(bti),AGIDL_BTIGetClrFmt(bti),num_of_wraps);
	}
}

void AGIDL_Wrap3DF(AGIDL_3DF* glide, u8 num_of_wraps){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 16){
		glide->pixels.pix16 = (COLOR16*)AGIDL_WrapImgData(glide->pixels.pix16,&glide->width,AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide),num_of_wraps);
	}
	else{
		glide->pixels.pix32 = (COLOR*)AGIDL_WrapImgData(glide->pixels.pix32,&glide->width,AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide),num_of_wraps);
	}
}

void AGIDL_WrapPPM(AGIDL_PPM* ppm, u8 num_of_wraps){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 16){
		ppm->pixels.pix16 = (COLOR16*)AGIDL_WrapImgData(ppm->pixels.pix16,&ppm->width,AGIDL_PPMGetHeight(ppm),AGIDL_PPMGetClrFmt(ppm),num_of_wraps);
	}
	else{
		ppm->pixels.pix32 = (COLOR*)AGIDL_WrapImgData(ppm->pixels.pix32,&ppm->width,AGIDL_PPMGetHeight(ppm),AGIDL_PPMGetClrFmt(ppm),num_of_wraps);
	}
}

void AGIDL_WrapLBM(AGIDL_LBM* lbm, u8 num_of_wraps){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 16){
		lbm->pixels.pix16 = (COLOR16*)AGIDL_WrapImgData(lbm->pixels.pix16,&lbm->header.bmhd.width,AGIDL_LBMGetHeight(lbm),AGIDL_LBMGetClrFmt(lbm),num_of_wraps);
	}
	else{
		lbm->pixels.pix32 = (COLOR*)AGIDL_WrapImgData(lbm->pixels.pix32,&lbm->header.bmhd.width,AGIDL_LBMGetHeight(lbm),AGIDL_LBMGetClrFmt(lbm),num_of_wraps);
	}
}

void AGIDL_WrapAndMirrorBMP(AGIDL_BMP* bmp, u8 num_of_wraps, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 16){
		bmp->pixels.pix16 = (COLOR16*)AGIDL_WrapAndMirrorImgData(bmp->pixels.pix16,&bmp->header.width,&bmp->header.height,AGIDL_BMPGetClrFmt(bmp),num_of_wraps, mirror);
	}
	else{
		bmp->pixels.pix32 = (COLOR*)AGIDL_WrapAndMirrorImgData(bmp->pixels.pix32,&bmp->header.width,&bmp->header.height,AGIDL_BMPGetClrFmt(bmp),num_of_wraps, mirror);
	}
}

void AGIDL_WrapAndMirrorTGA(AGIDL_TGA* tga, u8 num_of_wraps, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		tga->pixels.pix16 = (COLOR16*)AGIDL_WrapAndMirrorImgData(tga->pixels.pix16,&tga->header.width,&tga->header.height,AGIDL_TGAGetClrFmt(tga),num_of_wraps, mirror);
	}
	else{
		tga->pixels.pix32 = (COLOR*)AGIDL_WrapAndMirrorImgData(tga->pixels.pix32,&tga->header.width,&tga->header.height,AGIDL_TGAGetClrFmt(tga),num_of_wraps, mirror);
	}
}

void AGIDL_WrapAndMirrorTIM(AGIDL_TIM* tim, u8 num_of_wraps, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 16){
		tim->pixels.pix16 = (COLOR16*)AGIDL_WrapAndMirrorImgData(tim->pixels.pix16,&tim->img_header.width,&tim->img_header.height,AGIDL_TIMGetClrFmt(tim),num_of_wraps, mirror);
	}
	else{
		tim->pixels.pix32 = (COLOR*)AGIDL_WrapAndMirrorImgData(tim->pixels.pix32,&tim->img_header.width,&tim->img_header.height,AGIDL_TIMGetClrFmt(tim),num_of_wraps, mirror);
	}
}

void AGIDL_WrapAndMirrorPCX(AGIDL_PCX* pcx, u8 num_of_wraps, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx)) == 16){
		pcx->pixels.pix16 = (COLOR16*)AGIDL_WrapAndMirrorImgData(pcx->pixels.pix16,&pcx->header.width,&pcx->header.height,AGIDL_PCXGetClrFmt(pcx),num_of_wraps, mirror);
	}
	else{
		pcx->pixels.pix32 = (COLOR*)AGIDL_WrapAndMirrorImgData(pcx->pixels.pix32,&pcx->header.width,&pcx->header.height,AGIDL_PCXGetClrFmt(pcx),num_of_wraps, mirror);
	}
}

void AGIDL_WrapAndMirrorLMP(AGIDL_LMP* lmp, u8 num_of_wraps, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) == 16){
		lmp->pixels.pix16 = (COLOR16*)AGIDL_WrapAndMirrorImgData(lmp->pixels.pix16,&lmp->width,&lmp->height,AGIDL_LMPGetClrFmt(lmp),num_of_wraps, mirror);
	}
	else{
		lmp->pixels.pix32 = (COLOR*)AGIDL_WrapAndMirrorImgData(lmp->pixels.pix32,&lmp->width,&lmp->height,AGIDL_LMPGetClrFmt(lmp),num_of_wraps, mirror);
	}
}

void AGIDL_WrapAndMirrorPVR(AGIDL_PVR* pvr, u8 num_of_wraps, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) == 16){
		pvr->pixels.pix16 = (COLOR16*)AGIDL_WrapAndMirrorImgData(pvr->pixels.pix16,&pvr->header.width,&pvr->header.height,AGIDL_PVRGetClrFmt(pvr),num_of_wraps, mirror);
	}
	else{
		pvr->pixels.pix32 = (COLOR*)AGIDL_WrapAndMirrorImgData(pvr->pixels.pix32,&pvr->header.width,&pvr->header.height,AGIDL_PVRGetClrFmt(pvr),num_of_wraps, mirror);
	}
}

void AGIDL_WrapAndMirrorGXT(AGIDL_GXT* gxt, u8 num_of_wraps, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 16){
		gxt->pixels.pix16 = (COLOR16*)AGIDL_WrapAndMirrorImgData(gxt->pixels.pix16,&gxt->header.header.width,&gxt->header.header.height,AGIDL_GXTGetClrFmt(gxt),num_of_wraps, mirror);
	}
	else{
		gxt->pixels.pix32 = (COLOR*)AGIDL_WrapAndMirrorImgData(gxt->pixels.pix32,&gxt->header.header.width,&gxt->header.header.height,AGIDL_GXTGetClrFmt(gxt),num_of_wraps, mirror);
	}
}

void AGIDL_WrapAndMirrorBTI(AGIDL_BTI* bti, u8 num_of_wraps, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		bti->pixels.pix16 = (COLOR16*)AGIDL_WrapAndMirrorImgData(bti->pixels.pix16,&bti->header.width,&bti->header.height,AGIDL_BTIGetClrFmt(bti),num_of_wraps, mirror);
	}
	else{
		bti->pixels.pix32 = (COLOR*)AGIDL_WrapAndMirrorImgData(bti->pixels.pix32,&bti->header.width,&bti->header.height,AGIDL_BTIGetClrFmt(bti),num_of_wraps, mirror);
	}
}

void AGIDL_WrapAndMirror3DF(AGIDL_3DF* glide, u8 num_of_wraps, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 16){
		glide->pixels.pix16 = (COLOR16*)AGIDL_WrapAndMirrorImgData(glide->pixels.pix16,&glide->width,&glide->height,AGIDL_3DFGetClrFmt(glide),num_of_wraps, mirror);
	}
	else{
		glide->pixels.pix32 = (COLOR*)AGIDL_WrapAndMirrorImgData(glide->pixels.pix32,&glide->width,&glide->height,AGIDL_3DFGetClrFmt(glide),num_of_wraps, mirror);
	}
}

void AGIDL_WrapAndMirrorPPM(AGIDL_PPM* ppm, u8 num_of_wraps, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 16){
		ppm->pixels.pix16 = (COLOR16*)AGIDL_WrapAndMirrorImgData(ppm->pixels.pix16,&ppm->width,&ppm->height,AGIDL_PPMGetClrFmt(ppm),num_of_wraps, mirror);
	}
	else{
		ppm->pixels.pix32 = (COLOR*)AGIDL_WrapAndMirrorImgData(ppm->pixels.pix32,&ppm->width,&ppm->height,AGIDL_PPMGetClrFmt(ppm),num_of_wraps, mirror);
	}
}

void AGIDL_WrapAndMirrorLBM(AGIDL_LBM* lbm, u8 num_of_wraps, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 16){
		lbm->pixels.pix16 = (COLOR16*)AGIDL_WrapAndMirrorImgData(lbm->pixels.pix16,&lbm->header.bmhd.width,&lbm->header.bmhd.height,AGIDL_LBMGetClrFmt(lbm),num_of_wraps, mirror);
	}
	else{
		lbm->pixels.pix32 = (COLOR*)AGIDL_WrapAndMirrorImgData(lbm->pixels.pix32,&lbm->header.bmhd.width,&lbm->header.bmhd.height,AGIDL_LBMGetClrFmt(lbm),num_of_wraps, mirror);
	}
}

void AGIDL_HalftoneBMP(AGIDL_BMP* bmp){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 16){
		AGIDL_HalftoneImgData(bmp->pixels.pix16,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),AGIDL_BMPGetClrFmt(bmp),AGIDL_BMPGetMaxDiff(bmp));
	}
	else{
		AGIDL_HalftoneImgData(bmp->pixels.pix32,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),AGIDL_BMPGetClrFmt(bmp),AGIDL_BMPGetMaxDiff(bmp));
	}
}

void AGIDL_HalftoneTGA(AGIDL_TGA* tga){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		AGIDL_HalftoneImgData(tga->pixels.pix16,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),AGIDL_TGAGetClrFmt(tga),AGIDL_TGAGetMaxDiff(tga));
	}
	else{
		AGIDL_HalftoneImgData(tga->pixels.pix32,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),AGIDL_TGAGetClrFmt(tga),AGIDL_TGAGetMaxDiff(tga));
	}
}

void AGIDL_HalftoneTIM(AGIDL_TIM* tim){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 16){
		AGIDL_HalftoneImgData(tim->pixels.pix16,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),AGIDL_TIMGetClrFmt(tim),AGIDL_TIMGetMaxDiff(tim));
	}
	else{
		AGIDL_HalftoneImgData(tim->pixels.pix32,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),AGIDL_TIMGetClrFmt(tim),AGIDL_TIMGetMaxDiff(tim));
	}
}

void AGIDL_HalftonePCX(AGIDL_PCX* pcx){
	if(AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx)) == 16){
		AGIDL_HalftoneImgData(pcx->pixels.pix16,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),AGIDL_PCXGetClrFmt(pcx),AGIDL_PCXGetMaxDiff(pcx));
	}
	else{
		AGIDL_HalftoneImgData(pcx->pixels.pix32,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),AGIDL_PCXGetClrFmt(pcx),AGIDL_PCXGetMaxDiff(pcx));
	}
}

void AGIDL_HalftoneLMP(AGIDL_LMP* lmp){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) == 16){
		AGIDL_HalftoneImgData(lmp->pixels.pix16,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),AGIDL_LMPGetClrFmt(lmp),AGIDL_LMPGetMaxDiff(lmp));
	}
	else{
		AGIDL_HalftoneImgData(lmp->pixels.pix32,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),AGIDL_LMPGetClrFmt(lmp),AGIDL_LMPGetMaxDiff(lmp));
	}
}

void AGIDL_HalftonePVR(AGIDL_PVR* pvr){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) == 16){
		AGIDL_HalftoneImgData(pvr->pixels.pix16,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr),AGIDL_PVRGetMaxDiff(pvr));
	}
	else{
		AGIDL_HalftoneImgData(pvr->pixels.pix32,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr),AGIDL_PVRGetMaxDiff(pvr));
	}
}

void AGIDL_HalftoneGXT(AGIDL_GXT* gxt){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 16){
		AGIDL_HalftoneImgData(gxt->pixels.pix16,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),AGIDL_GXTGetClrFmt(gxt),AGIDL_GXTGetMaxDiff(gxt));
	}
	else{
		AGIDL_HalftoneImgData(gxt->pixels.pix32,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),AGIDL_GXTGetClrFmt(gxt),AGIDL_GXTGetMaxDiff(gxt));
	}
}

void AGIDL_HalftoneBTI(AGIDL_BTI* bti){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		AGIDL_HalftoneImgData(bti->pixels.pix16,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),AGIDL_BTIGetClrFmt(bti),AGIDL_BTIGetMaxDiff(bti));
	}
	else{
		AGIDL_HalftoneImgData(bti->pixels.pix32,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),AGIDL_BTIGetClrFmt(bti),AGIDL_BTIGetMaxDiff(bti));
	}
}

void AGIDL_Halftone3DF(AGIDL_3DF* glide){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 16){
		AGIDL_HalftoneImgData(glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide),AGIDL_3DFGetMaxDiff(glide));
	}
	else{
		AGIDL_HalftoneImgData(glide->pixels.pix32,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide),AGIDL_3DFGetMaxDiff(glide));
	}
}

void AGIDL_HalftonePPM(AGIDL_PPM* ppm){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 16){
		AGIDL_HalftoneImgData(ppm->pixels.pix16,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),AGIDL_PPMGetClrFmt(ppm),AGIDL_PPMGetMaxDiff(ppm));
	}
	else{
		AGIDL_HalftoneImgData(ppm->pixels.pix32,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),AGIDL_PPMGetClrFmt(ppm),AGIDL_PPMGetMaxDiff(ppm));
	}
}

void AGIDL_HalftoneLBM(AGIDL_LBM* lbm){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 16){
		AGIDL_HalftoneImgData(lbm->pixels.pix16,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),AGIDL_LBMGetClrFmt(lbm),AGIDL_LBMGetMaxDiff(lbm));
	}
	else{
		AGIDL_HalftoneImgData(lbm->pixels.pix32,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),AGIDL_LBMGetClrFmt(lbm),AGIDL_LBMGetMaxDiff(lbm));
	}
}