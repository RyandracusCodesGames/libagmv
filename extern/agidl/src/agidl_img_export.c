/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_export.c
*   Date: 3/28/2024
*   Version: 0.4b
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <stdio.h>
#include <agidl_img_export.h>
#include <agidl_img_converter.h>

u32 expcount = 0;

void AGIDL_QuickExport(void* data, u32 width, u32 height, AGIDL_CLR_FMT fmt, AGIDL_IMG_TYPE img_type){
	expcount++;
	
	switch(img_type){
		case AGIDL_IMG_BMP:{
			char filename[50];
			sprintf(filename,"quick_export_%ld.bmp",expcount);
			
			AGIDL_BMP* bmp = AGIDL_CreateBMP(filename,width,height,fmt);
			
			if(AGIDL_GetBitCount(fmt) == 16){
				u16* img_data = (u16*)data;
				AGIDL_BMPSyncPix16(bmp,img_data);
			}
			else{
				u32* img_data = (u32*)data;
				AGIDL_BMPSyncPix(bmp,img_data);
			}
			
			AGIDL_ExportBMP(bmp);
			AGIDL_FreeBMP(bmp);
		}break;
		case AGIDL_IMG_TGA:{
			char filename[50];
			sprintf(filename,"quick_export_%ld.tga",expcount);
			
			AGIDL_TGA* tga = AGIDL_CreateTGA(filename,width,height,fmt);
			
			if(AGIDL_GetBitCount(fmt) == 16){
				u16* img_data = (u16*)data;
				AGIDL_TGASyncPix16(tga,img_data);
			}
			else{
				u32* img_data = (u32*)data;
				AGIDL_TGASyncPix(tga,img_data);
			}
			
			AGIDL_ExportTGA(tga);
			AGIDL_FreeTGA(tga);
		}break;
		case AGIDL_IMG_TIM:{
			char filename[50];
			sprintf(filename,"quick_export_%ld.tim",expcount);
			
			AGIDL_TIM* tim = AGIDL_CreateTIM(filename,width,height,fmt);
			
			if(AGIDL_GetBitCount(fmt) == 16){
				u16* img_data = (u16*)data;
				AGIDL_TIMSyncPix16(tim,img_data);
			}
			else{
				u32* img_data = (u32*)data;
				AGIDL_TIMSyncPix(tim,img_data);
			}
			
			AGIDL_ExportTIM(tim);
			AGIDL_FreeTIM(tim);
		}break;
		case AGIDL_IMG_PCX:{
			char filename[50];
			sprintf(filename,"quick_export_%ld.pcx",expcount);
			
			AGIDL_PCX* pcx = AGIDL_CreatePCX(filename,width,height,fmt);
			
			if(AGIDL_GetBitCount(fmt) == 16){
				u16* img_data = (u16*)data;
				AGIDL_PCXSyncPix16(pcx,img_data);
			}
			else{
				u32* img_data = (u32*)data;
				AGIDL_PCXSyncPix(pcx,img_data);
			}
			
			AGIDL_ExportPCX(pcx);
			AGIDL_FreePCX(pcx);
		}break;
		case AGIDL_IMG_LMP:{
			char filename[50];
			sprintf(filename,"quick_export_%ld.lmp",expcount);
			
			AGIDL_LMP* lmp = AGIDL_CreateLMP(filename,width,height,fmt);
			
			if(AGIDL_GetBitCount(fmt) == 16){
				u16* img_data = (u16*)data;
				AGIDL_LMPSyncPix16(lmp,img_data);
			}
			else{
				u32* img_data = (u32*)data;
				AGIDL_LMPSyncPix(lmp,img_data);
			}
			
			AGIDL_ExportLMP(lmp);
			AGIDL_FreeLMP(lmp);
		}break;
		case AGIDL_IMG_PVR:{
			char filename[50];
			sprintf(filename,"quick_export_%ld.pvr",expcount);
			
			AGIDL_PVR* pvr = AGIDL_CreatePVR(filename,width,height,fmt);
			
			if(AGIDL_GetBitCount(fmt) == 16){
				u16* img_data = (u16*)data;
				AGIDL_PVRSyncPix16(pvr,img_data);
			}
			else{
				u32* img_data = (u32*)data;
				AGIDL_PVRSyncPix(pvr,img_data);
			}
			
			AGIDL_ExportPVR(pvr);
			AGIDL_FreePVR(pvr);
		}break;
		case AGIDL_IMG_GXT:{
			char filename[50];
			sprintf(filename,"quick_export_%ld.gxt",expcount);
			
			AGIDL_GXT* gxt = AGIDL_CreateGXT(filename,width,height,fmt);
			
			if(AGIDL_GetBitCount(fmt) == 16){
				u16* img_data = (u16*)data;
				AGIDL_GXTSyncPix16(gxt,img_data);
			}
			else{
				u32* img_data = (u32*)data;
				AGIDL_GXTSyncPix(gxt,img_data);
			}
			
			AGIDL_ExportGXT(gxt);
			AGIDL_FreeGXT(gxt);
		}break;
		case AGIDL_IMG_BTI:{
			char filename[50];
			sprintf(filename,"quick_export_%ld.bti",expcount);
			
			AGIDL_BTI* bti = AGIDL_CreateBTI(filename,width,height,fmt);
			
			if(AGIDL_GetBitCount(fmt) == 16){
				u16* img_data = (u16*)data;
				AGIDL_BTISyncPix16(bti,img_data);
			}
			else{
				u32* img_data = (u32*)data;
				AGIDL_BTISyncPix(bti,img_data);
			}
			
			AGIDL_ExportBTI(bti);
			AGIDL_FreeBTI(bti);
		}break;
		case AGIDL_IMG_3DF:{
			char filename[50];
			sprintf(filename,"quick_export_%ld.glide",expcount);
			
			AGIDL_3DF* glide = AGIDL_Create3DF(filename,width,height,fmt);
			
			if(AGIDL_GetBitCount(fmt) == 16){
				u16* img_data = (u16*)data;
				AGIDL_3DFSyncPix16(glide,img_data);
			}
			else{
				u32* img_data = (u32*)data;
				AGIDL_3DFSyncPix(glide,img_data);
			}
			
			AGIDL_Export3DF(glide);
			AGIDL_Free3DF(glide);
		}break;
		case AGIDL_IMG_PPM:{
			char filename[50];
			sprintf(filename,"quick_export_%ld.ppm",expcount);
			
			AGIDL_PPM* ppm = AGIDL_CreatePPM(filename,width,height,fmt);
			
			if(AGIDL_GetBitCount(fmt) == 16){
				u16* img_data = (u16*)data;
				AGIDL_PPMSyncPix16(ppm,img_data);
			}
			else{
				u32* img_data = (u32*)data;
				AGIDL_PPMSyncPix(ppm,img_data);
			}
			
			AGIDL_ExportPPM(ppm);
			AGIDL_FreePPM(ppm);
		}break;
		case AGIDL_IMG_LBM:{
			char filename[50];
			sprintf(filename,"quick_export_%ld.lbm",expcount);
			
			AGIDL_LBM* lbm = AGIDL_CreateLBM(filename,width,height,fmt);
			
			if(AGIDL_GetBitCount(fmt) == 16){
				u16* img_data = (u16*)data;
				AGIDL_LBMSyncPix16(lbm,img_data);
			}
			else{
				u32* img_data = (u32*)data;
				AGIDL_LBMSyncPix(lbm,img_data);
			}
			
			AGIDL_ExportLBM(lbm);
			AGIDL_FreeLBM(lbm);
		}break;
	}
}