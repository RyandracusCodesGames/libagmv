#ifndef AGIDL_IMGP_IMPL_H
#define AGIDL_IMGP_IMPL_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_impl.h
*   Date: 12/17/2023
*   Version: 0.2b
*   Updated: 2/25/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_imgp_mirror.h>
#include <agidl_imgp_rotate.h>
#include <agidl_imgp_scale.h>
#include <agidl_img_converter.h>
#include <agidl_types.h>

void AGIDL_GrayscaleBMP(const AGIDL_BMP* bmp);
void AGIDL_GrayscaleTGA(const AGIDL_TGA* tga);
void AGIDL_GrayscaleTIM(const AGIDL_TIM* tim);
void AGIDL_GrayscalePCX(const AGIDL_PCX* pcx);
void AGIDL_GrayscaleLMP(const AGIDL_LMP* lmp);
void AGIDL_GrayscalePVR(const AGIDL_PVR* pvr);
void AGIDL_GrayscaleGXT(const AGIDL_GXT* gxt);
void AGIDL_GrayscaleBTI(const AGIDL_BTI* bti);
void AGIDL_Grayscale3DF(const AGIDL_3DF* glide);
void AGIDL_GrayscalePPM(const AGIDL_PPM* ppm);
void AGIDL_GrayscaleLBM(const AGIDL_LBM* lbm);

void AGIDL_FlipHorzBMP(const AGIDL_BMP* bmp);
void AGIDL_FlipHorzTGA(const AGIDL_TGA* tga);
void AGIDL_FlipHorzTIM(const AGIDL_TIM* tim);
void AGIDL_FlipHorzPCX(const AGIDL_PCX* pcx);
void AGIDL_FlipHorzLMP(const AGIDL_LMP* lmp);
void AGIDL_FlipHorzPVR(const AGIDL_PVR* pvr);
void AGIDL_FlipHorzGXT(const AGIDL_GXT* gxt);
void AGIDL_FlipHorzBTI(const AGIDL_BTI* bti);
void AGIDL_FlipHorz3DF(const AGIDL_3DF* glide);
void AGIDL_FlipHorzPPM(const AGIDL_PPM* ppm);
void AGIDL_FlipHorzLBM(const AGIDL_LBM* lbm);

void AGIDL_FlipVertBMP(const AGIDL_BMP* bmp);
void AGIDL_FlipVertTGA(const AGIDL_TGA* tga);
void AGIDL_FlipVertTIM(const AGIDL_TIM* tim);
void AGIDL_FlipVertPCX(const AGIDL_PCX* pcx);
void AGIDL_FlipVertLMP(const AGIDL_LMP* lmp);
void AGIDL_FlipVertPVR(const AGIDL_PVR* pvr);
void AGIDL_FlipVertGXT(const AGIDL_GXT* gxt);
void AGIDL_FlipVertBTI(const AGIDL_BTI* bti);
void AGIDL_FlipVert3DF(const AGIDL_3DF* glide);
void AGIDL_FlipVertPPM(const AGIDL_PPM* ppm);
void AGIDL_FlipVertLBM(const AGIDL_LBM* lbm);

void AGIDL_FlipHorzAndVertBMP(const AGIDL_BMP* bmp);
void AGIDL_FlipHorzAndVertTGA(const AGIDL_TGA* tga);
void AGIDL_FlipHorzAndVertTIM(const AGIDL_TIM* tim);
void AGIDL_FlipHorzAndVertPCX(const AGIDL_PCX* pcx);
void AGIDL_FlipHorzAndVertLMP(const AGIDL_LMP* lmp);
void AGIDL_FlipHorzAndVertPVR(const AGIDL_PVR* pvr);
void AGIDL_FlipHorzAndVertGXT(const AGIDL_GXT* gxt);
void AGIDL_FlipHorzAndVertBTI(const AGIDL_BTI* bti);
void AGIDL_FlipHorzAndVert3DF(const AGIDL_3DF* glide);
void AGIDL_FlipHorzAndVertPPM(const AGIDL_PPM* ppm);
void AGIDL_FlipHorzAndVertLBM(const AGIDL_LBM* lbm);

void AGIDL_RotateBMP(AGIDL_BMP* bmp, AGIDL_ANGLE angle);
void AGIDL_RotateTGA(AGIDL_TGA* tga, AGIDL_ANGLE angle);
void AGIDL_RotateTIM(AGIDL_TIM* tim, AGIDL_ANGLE angle);
void AGIDL_RotatePCX(AGIDL_PCX* pcx, AGIDL_ANGLE angle);
void AGIDL_RotateLMP(AGIDL_LMP* lmp, AGIDL_ANGLE angle);
void AGIDL_RotatePVR(AGIDL_PVR* pvr, AGIDL_ANGLE angle);
void AGIDL_RotateGXT(AGIDL_GXT* gxt, AGIDL_ANGLE angle);
void AGIDL_RotateBTI(AGIDL_BTI* bti, AGIDL_ANGLE angle);
void AGIDL_Rotate3DF(AGIDL_3DF* glide, AGIDL_ANGLE angle);
void AGIDL_RotatePPM(AGIDL_PPM* ppm, AGIDL_ANGLE angle);
void AGIDL_RotateLBM(AGIDL_LBM* lbm, AGIDL_ANGLE angle);

void AGIDL_ScaleBMP(AGIDL_BMP* bmp, float sx, float sy, AGIDL_SCALE scale);
void AGIDL_ScaleTGA(AGIDL_TGA* tga, float sx, float sy, AGIDL_SCALE scale);
void AGIDL_ScaleTIM(AGIDL_TIM* tim, float sx, float sy, AGIDL_SCALE scale);
void AGIDL_ScalePCX(AGIDL_PCX* pcx, float sx, float sy, AGIDL_SCALE scale);
void AGIDL_ScaleLMP(AGIDL_LMP* lmp, float sx, float sy, AGIDL_SCALE scale);
void AGIDL_ScalePVR(AGIDL_PVR* pvr, float sx, float sy, AGIDL_SCALE scale);
void AGIDL_ScaleGXT(AGIDL_GXT* gxt, float sx, float sy, AGIDL_SCALE scale);
void AGIDL_ScaleBTI(AGIDL_BTI* bti, float sx, float sy, AGIDL_SCALE scale);
void AGIDL_Scale3DF(AGIDL_3DF* glide, float sx, float sy, AGIDL_SCALE scale);
void AGIDL_ScalePPM(AGIDL_PPM* ppm, float sx, float sy, AGIDL_SCALE scale);
void AGIDL_ScaleLBM(AGIDL_LBM* lbm, float sx, float sy, AGIDL_SCALE scale);

void AGIDL_FastScaleBMP(AGIDL_BMP* bmp, float sx, float sy, AGIDL_SCALE scale);
void AGIDL_FastScaleTGA(AGIDL_TGA* tga, float sx, float sy, AGIDL_SCALE scale);
void AGIDL_FastScaleTIM(AGIDL_TIM* tim, float sx, float sy, AGIDL_SCALE scale);
void AGIDL_FastScalePCX(AGIDL_PCX* pcx, float sx, float sy, AGIDL_SCALE scale);
void AGIDL_FastScaleLMP(AGIDL_LMP* lmp, float sx, float sy, AGIDL_SCALE scale);
void AGIDL_FastScalePVR(AGIDL_PVR* pvr, float sx, float sy, AGIDL_SCALE scale);
void AGIDL_FastScaleGXT(AGIDL_GXT* gxt, float sx, float sy, AGIDL_SCALE scale);
void AGIDL_FastScaleBTI(AGIDL_BTI* bti, float sx, float sy, AGIDL_SCALE scale);
void AGIDL_FastScale3DF(AGIDL_3DF* glide, float sx, float sy, AGIDL_SCALE scale);
void AGIDL_FastScalePPM(AGIDL_PPM* ppm, float sx, float sy, AGIDL_SCALE scale);
void AGIDL_FastScaleLBM(AGIDL_LBM* lbm, float sx, float sy, AGIDL_SCALE scale);

void AGIDL_FilterBilerpBMP(const AGIDL_BMP* bmp);
void AGIDL_FilterBilerpTGA(const AGIDL_TGA* tga);
void AGIDL_FilterBilerpTIM(const AGIDL_TIM* tim);
void AGIDL_FilterBilerpPCX(const AGIDL_PCX* pcx);
void AGIDL_FilterBilerpLMP(const AGIDL_LMP* lmp);
void AGIDL_FilterBilerpPVR(const AGIDL_PVR* pvr);
void AGIDL_FilterBilerpGXT(const AGIDL_GXT* gxt);
void AGIDL_FilterBilerpBTI(const AGIDL_BTI* bti);
void AGIDL_FilterBilerp3DF(const AGIDL_3DF* glide);
void AGIDL_FilterBilerpPPM(const AGIDL_PPM* ppm);
void AGIDL_FilterBilerpLBM(const AGIDL_LBM* lbm);

void AGIDL_FilterTrilerpBMP(const AGIDL_BMP* bmp);
void AGIDL_FilterTrilerpTGA(const AGIDL_TGA* tga);
void AGIDL_FilterTrilerpTIM(const AGIDL_TIM* tim);
void AGIDL_FilterTrilerpPCX(const AGIDL_PCX* pcx);
void AGIDL_FilterTrilerpLMP(const AGIDL_LMP* lmp);
void AGIDL_FilterTrilerpPVR(const AGIDL_PVR* pvr);
void AGIDL_FilterTrilerpGXT(const AGIDL_GXT* gxt);
void AGIDL_FilterTrilerpBTI(const AGIDL_BTI* bti);
void AGIDL_FilterBilerp3DF(const AGIDL_3DF* glide);
void AGIDL_FilterBilerpPPM(const AGIDL_PPM* ppm);
void AGIDL_FilterBilerpLBM(const AGIDL_LBM* lbm);

void AGIDL_MirrorBMP(AGIDL_BMP* bmp, AGIDL_MIRROR mirror);
void AGIDL_MirrorTGA(AGIDL_TGA* tga, AGIDL_MIRROR mirror);
void AGIDL_MirrorTIM(AGIDL_TIM* tim, AGIDL_MIRROR mirror);
void AGIDL_MirrorPCX(AGIDL_PCX* pcx, AGIDL_MIRROR mirror);
void AGIDL_MirrorLMP(AGIDL_LMP* lmp, AGIDL_MIRROR mirror);
void AGIDL_MirrorPVR(AGIDL_PVR* pvr, AGIDL_MIRROR mirror);
void AGIDL_MirrorGXT(AGIDL_GXT* gxt, AGIDL_MIRROR mirror);
void AGIDL_MirrorBTI(AGIDL_BTI* bti, AGIDL_MIRROR mirror);
void AGIDL_Mirror3DF(AGIDL_3DF* glide, AGIDL_MIRROR mirror);
void AGIDL_MirrorPPM(AGIDL_PPM* ppm, AGIDL_MIRROR mirror);
void AGIDL_MirrorLBM(AGIDL_LBM* lbm, AGIDL_MIRROR mirror);

void AGIDL_WrapBMP(AGIDL_BMP* bmp, u8 num_of_wraps);
void AGIDL_WrapTGA(AGIDL_TGA* tga, u8 num_of_wraps);
void AGIDL_WrapTIM(AGIDL_TIM* tim, u8 num_of_wraps);
void AGIDL_WrapPCX(AGIDL_PCX* pcx, u8 num_of_wraps);
void AGIDL_WrapLMP(AGIDL_LMP* lmp, u8 num_of_wraps);
void AGIDL_WrapPVR(AGIDL_PVR* pvr, u8 num_of_wraps);
void AGIDL_WrapGXT(AGIDL_GXT* gxt, u8 num_of_wraps);
void AGIDL_WrapBTI(AGIDL_BTI* bti, u8 num_of_wraps);
void AGIDL_Wrap3DF(AGIDL_3DF* glide, u8 num_of_wraps);
void AGIDL_WrapPPM(AGIDL_PPM* ppm, u8 num_of_wraps);
void AGIDL_WrapLBM(AGIDL_LBM* lbm, u8 num_of_wraps);

void AGIDL_WrapAndMirrorBMP(AGIDL_BMP* bmp, u8 num_of_wraps, AGIDL_MIRROR mirror);
void AGIDL_WrapAndMirrorTGA(AGIDL_TGA* tga, u8 num_of_wraps, AGIDL_MIRROR mirror);
void AGIDL_WrapAndMirrorTIM(AGIDL_TIM* tim, u8 num_of_wraps, AGIDL_MIRROR mirror);
void AGIDL_WrapAndMirrorPCX(AGIDL_PCX* pcx, u8 num_of_wraps, AGIDL_MIRROR mirror);
void AGIDL_WrapAndMirrorLMP(AGIDL_LMP* lmp, u8 num_of_wraps, AGIDL_MIRROR mirror);
void AGIDL_WrapAndMirrorPVR(AGIDL_PVR* pvr, u8 num_of_wraps, AGIDL_MIRROR mirror);
void AGIDL_WrapAndMirrorGXT(AGIDL_GXT* gxt, u8 num_of_wraps, AGIDL_MIRROR mirror);
void AGIDL_WrapAndMirrorBTI(AGIDL_BTI* bti, u8 num_of_wraps, AGIDL_MIRROR mirror);
void AGIDL_WrapAndMirror3DF(AGIDL_3DF* glide, u8 num_of_wraps, AGIDL_MIRROR mirror);
void AGIDL_WrapAndMirrorPPM(AGIDL_PPM* ppm, u8 num_of_wraps, AGIDL_MIRROR mirror);
void AGIDL_WrapAndMirrorLBM(AGIDL_LBM* lbm, u8 num_of_wraps, AGIDL_MIRROR mirror);

void AGIDL_HalftoneBMP(const AGIDL_BMP* bmp);
void AGIDL_HalftoneTGA(const AGIDL_TGA* tga);
void AGIDL_HalftoneTIM(const AGIDL_TIM* tim);
void AGIDL_HalftonePCX(const AGIDL_PCX* pcx);
void AGIDL_HalftoneLMP(const AGIDL_LMP* lmp);
void AGIDL_HalftonePVR(const AGIDL_PVR* pvr);
void AGIDL_HalftoneGXT(const AGIDL_GXT* gxt);
void AGIDL_HalftoneBTI(const AGIDL_BTI* bti);
void AGIDL_Halftone3DF(const AGIDL_3DF* glide);
void AGIDL_HalftonePPM(const AGIDL_PPM* ppm);
void AGIDL_HalftoneLBM(const AGIDL_LBM* lbm);

#endif
