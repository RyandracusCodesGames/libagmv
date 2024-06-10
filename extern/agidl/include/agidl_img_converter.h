#ifndef AGIDL_IMG_CONVERTER_H
#define AGIDL_IMG_CONVERTER_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_converter.h
*   Date: 11/3/2023
*   Version: 0.1b
*   Updated: 2/25/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_img_3df.h>
#include <agidl_img_bmp.h>
#include <agidl_img_bti.h>
#include <agidl_img_gxt.h>
#include <agidl_img_lbm.h>
#include <agidl_img_pcx.h>
#include <agidl_img_ppm.h>
#include <agidl_img_pvr.h>
#include <agidl_img_quake.h>
#include <agidl_img_tga.h>
#include <agidl_img_tim.h>

/* BMP -> AGIDL_IMG_TYPE */
AGIDL_TGA* AGIDL_ConvertBMP2TGA(const AGIDL_BMP* bmp);
AGIDL_TIM* AGIDL_ConvertBMP2TIM(const AGIDL_BMP* bmp);
AGIDL_PCX* AGIDL_ConvertBMP2PCX(const AGIDL_BMP* bmp);
AGIDL_LMP* AGIDL_ConvertBMP2LMP(const AGIDL_BMP* bmp);
AGIDL_PVR* AGIDL_ConvertBMP2PVR(const AGIDL_BMP* bmp);
AGIDL_GXT* AGIDL_ConvertBMP2GXT(const AGIDL_BMP* bmp);
AGIDL_BTI* AGIDL_ConvertBMP2BTI(const AGIDL_BMP* bmp);
AGIDL_3DF* AGIDL_ConvertBMP23DF(const AGIDL_BMP* bmp);
AGIDL_PPM* AGIDL_ConvertBMP2PPM(const AGIDL_BMP* bmp);
AGIDL_LBM* AGIDL_ConvertBMP2LBM(const AGIDL_BMP* bmp);
/* TGA -> AGIDL_IMG_TYPE */
AGIDL_BMP* AGIDL_ConvertTGA2BMP(const AGIDL_TGA* tga);
AGIDL_TIM* AGIDL_ConvertTGA2TIM(const AGIDL_TGA* tga);
AGIDL_PCX* AGIDL_ConvertTGA2PCX(const AGIDL_TGA* tga);
AGIDL_LMP* AGIDL_ConvertTGA2LMP(const AGIDL_TGA* tga);
AGIDL_PVR* AGIDL_ConvertTGA2PVR(const AGIDL_TGA* tga);
AGIDL_GXT* AGIDL_ConvertTGA2GXT(const AGIDL_TGA* tga);
AGIDL_BTI* AGIDL_ConvertTGA2BTI(const AGIDL_TGA* tga);
AGIDL_3DF* AGIDL_ConvertTGA23DF(const AGIDL_TGA* tga);
AGIDL_PPM* AGIDL_ConvertTGA2PPM(const AGIDL_TGA* tga);
AGIDL_LBM* AGIDL_ConvertTGA2LBM(const AGIDL_TGA* tga);
/* TIM -> AGIDL_IMG_TYPE */
AGIDL_BMP* AGIDL_ConvertTIM2BMP(const AGIDL_TIM* tim);
AGIDL_TGA* AGIDL_ConvertTIM2TGA(const AGIDL_TIM* tim);
AGIDL_PCX* AGIDL_ConvertTIM2PCX(const AGIDL_TIM* tim);
AGIDL_LMP* AGIDL_ConvertTIM2LMP(const AGIDL_TIM* tim);
AGIDL_PVR* AGIDL_ConvertTIM2PVR(const AGIDL_TIM* tim);
AGIDL_GXT* AGIDL_ConvertTIM2GXT(const AGIDL_TIM* tim);
AGIDL_BTI* AGIDL_ConvertTIM2BTI(const AGIDL_TIM* tim);
AGIDL_3DF* AGIDL_ConvertTIM23DF(const AGIDL_TIM* tim);
AGIDL_PPM* AGIDL_ConvertTIM2PPM(const AGIDL_TIM* tim);
AGIDL_LBM* AGIDL_ConvertTIM2LBM(const AGIDL_TIM* tim);
/* PCX -> AGIDL_IMG_TYPE */
AGIDL_BMP* AGIDL_ConvertPCX2BMP(const AGIDL_PCX* pcx);
AGIDL_TGA* AGIDL_ConvertPCX2TGA(const AGIDL_PCX* pcx);
AGIDL_TIM* AGIDL_ConvertPCX2TIM(const AGIDL_PCX* pcx);
AGIDL_LMP* AGIDL_ConvertPCX2LMP(const AGIDL_PCX* pcx);
AGIDL_PVR* AGIDL_ConvertPCX2PVR(const AGIDL_PCX* pcx);
AGIDL_GXT* AGIDL_ConvertPCX2GXT(AGIDL_PCX* pcx);
AGIDL_BTI* AGIDL_ConvertPCX2BTI(AGIDL_PCX* pcx);
AGIDL_3DF* AGIDL_ConvertPCX23DF(AGIDL_PCX* pcx);
AGIDL_PPM* AGIDL_ConvertPCX2PPM(AGIDL_PCX* pcx);
AGIDL_LBM* AGIDL_ConvertPCX2LBM(const AGIDL_PCX* pcx);
/* LMP -> AGIDL_IMG_TYPE */
AGIDL_BMP* AGIDL_ConvertLMP2BMP(const AGIDL_LMP* lmp);
AGIDL_TGA* AGIDL_ConvertLMP2TGA(const AGIDL_LMP* lmp);
AGIDL_TIM* AGIDL_ConvertLMP2TIM(const AGIDL_LMP* lmp);
AGIDL_PCX* AGIDL_ConvertLMP2PCX(const AGIDL_LMP* lmp);
AGIDL_PVR* AGIDL_ConvertLMP2PVR(const AGIDL_LMP* lmp);
AGIDL_GXT* AGIDL_ConvertLMP2GXT(const AGIDL_LMP* lmp);
AGIDL_BTI* AGIDL_ConvertLMP2BTI(const AGIDL_LMP* lmp);
AGIDL_3DF* AGIDL_ConvertLMP23DF(const AGIDL_LMP* lmp);
AGIDL_PPM* AGIDL_ConvertLMP2PPM(const AGIDL_LMP* lmp);
AGIDL_LBM* AGIDL_ConvertLMP2LBM(const AGIDL_LMP* lmp);
/* PVR -> AGIDL_IMG_TYPE */
AGIDL_BMP* AGIDL_ConvertPVR2BMP(const AGIDL_PVR* pvr);
AGIDL_TGA* AGIDL_ConvertPVR2TGA(const AGIDL_PVR* pvr);
AGIDL_TIM* AGIDL_ConvertPVR2TIM(const AGIDL_PVR* pvr);
AGIDL_PCX* AGIDL_ConvertPVR2PCX(const AGIDL_PVR* pvr);
AGIDL_LMP* AGIDL_ConvertPVR2LMP(const AGIDL_PVR* pvr);
AGIDL_GXT* AGIDL_ConvertPVR2GXT(const AGIDL_PVR* pvr);
AGIDL_BTI* AGIDL_ConvertPVR2BTI(const AGIDL_PVR* pvr);
AGIDL_3DF* AGIDL_ConvertPVR23DF(const AGIDL_PVR* pvr);
AGIDL_PPM* AGIDL_ConvertPVR2PPM(const AGIDL_PVR* pvr);
AGIDL_LBM* AGIDL_ConvertPVR2LBM(const AGIDL_PVR* pvr);
/* GXT -> AGIDL_IMG_TYPE */
AGIDL_BMP* AGIDL_ConvertGXT2BMP(const AGIDL_GXT* gxt);
AGIDL_TGA* AGIDL_ConvertGXT2TGA(const AGIDL_GXT* gxt);
AGIDL_TIM* AGIDL_ConvertGXT2TIM(const AGIDL_GXT* gxt);
AGIDL_PCX* AGIDL_ConvertGXT2PCX(const AGIDL_GXT* gxt);
AGIDL_LMP* AGIDL_ConvertGXT2LMP(const AGIDL_GXT* gxt);
AGIDL_PVR* AGIDL_ConvertGXT2PVR(const AGIDL_GXT* gxt);
AGIDL_BTI* AGIDL_ConvertGXT2BTI(const AGIDL_GXT* gxt);
AGIDL_3DF* AGIDL_ConvertGXT23DF(const AGIDL_GXT* gxt);
AGIDL_PPM* AGIDL_ConvertGXT2PPM(const AGIDL_GXT* gxt);
AGIDL_LBM* AGIDL_ConvertGXT2LBM(const AGIDL_GXT* gxt);
/*BTI -> AGIDL_IMG_TYPE */
AGIDL_BMP* AGIDL_ConvertBTI2BMP(const AGIDL_BTI* bti);
AGIDL_TGA* AGIDL_ConvertBTI2TGA(const AGIDL_BTI* bti);
AGIDL_TIM* AGIDL_ConvertBTI2TIM(const AGIDL_BTI* bti);
AGIDL_PCX* AGIDL_ConvertBTI2PCX(const AGIDL_BTI* bti);
AGIDL_LMP* AGIDL_ConvertBTI2LMP(const AGIDL_BTI* bti);
AGIDL_PVR* AGIDL_ConvertBTI2PVR(const AGIDL_BTI* bti);
AGIDL_GXT* AGIDL_ConvertBTI2GXT(const AGIDL_BTI* bti);
AGIDL_3DF* AGIDL_ConvertBTI23DF(const AGIDL_BTI* bti);
AGIDL_PPM* AGIDL_ConvertBTI2PPM(const AGIDL_BTI* bti);
AGIDL_LBM* AGIDL_ConvertBTI2LBM(const AGIDL_BTI* bti);
/*3DF -> AGIDL_IMG_TYPE */
AGIDL_BMP* AGIDL_Convert3DF2BMP(const AGIDL_3DF* glide);
AGIDL_TGA* AGIDL_Convert3DF2TGA(const AGIDL_3DF* glide);
AGIDL_TIM* AGIDL_Convert3DF2TIM(const AGIDL_3DF* glide);
AGIDL_PCX* AGIDL_Convert3DF2PCX(const AGIDL_3DF* glide);
AGIDL_LMP* AGIDL_Convert3DF2LMP(const AGIDL_3DF* glide);
AGIDL_PVR* AGIDL_Convert3DF2PVR(const AGIDL_3DF* glide);
AGIDL_GXT* AGIDL_Convert3DF2GXT(const AGIDL_3DF* glide);
AGIDL_BTI* AGIDL_Convert3DF2BTI(const AGIDL_3DF* glide);
AGIDL_PPM* AGIDL_Convert3DF2PPM(const AGIDL_3DF* glide);
AGIDL_LBM* AGIDL_Convert3DF2LBM(const AGIDL_3DF* glide);
/*PPM -> AGIDL_IMG_TYPE */
AGIDL_BMP* AGIDL_ConvertPPM2BMP(const AGIDL_PPM* ppm);
AGIDL_TGA* AGIDL_ConvertPPM2TGA(const AGIDL_PPM* ppm);
AGIDL_TIM* AGIDL_ConvertPPM2TIM(const AGIDL_PPM* ppm);
AGIDL_PCX* AGIDL_ConvertPPM2PCX(const AGIDL_PPM* ppm);
AGIDL_LMP* AGIDL_ConvertPPM2LMP(const AGIDL_PPM* ppm);
AGIDL_PVR* AGIDL_ConvertPPM2PVR(const AGIDL_PPM* ppm);
AGIDL_GXT* AGIDL_ConvertPPM2GXT(const AGIDL_PPM* ppm);
AGIDL_BTI* AGIDL_ConvertPPM2BTI(const AGIDL_PPM* ppm);
AGIDL_3DF* AGIDL_ConvertPPM23DF(const AGIDL_PPM* ppm);
AGIDL_LBM* AGIDL_ConvertPPM2LBM(const AGIDL_PPM* ppm);
/*LBM -> AGIDL_IMG_TYPE */
AGIDL_BMP* AGIDL_ConvertLBM2BMP(const AGIDL_LBM* lbm);
AGIDL_TGA* AGIDL_ConvertLBM2TGA(const AGIDL_LBM* lbm);
AGIDL_TIM* AGIDL_ConvertLBM2TIM(const AGIDL_LBM* lbm);
AGIDL_PCX* AGIDL_ConvertLBM2PCX(const AGIDL_LBM* lbm);
AGIDL_LMP* AGIDL_ConvertLBM2LMP(const AGIDL_LBM* lbm);
AGIDL_PVR* AGIDL_ConvertLBM2PVR(const AGIDL_LBM* lbm);
AGIDL_GXT* AGIDL_ConvertLBM2GXT(const AGIDL_LBM* lbm);
AGIDL_BTI* AGIDL_ConvertLBM2BTI(AGIDL_LBM* lbm);
AGIDL_3DF* AGIDL_ConvertLBM23DF(const AGIDL_LBM* lbm);
AGIDL_PPM* AGIDL_ConvertLBM2PPM(const AGIDL_LBM* lbm);
#endif
