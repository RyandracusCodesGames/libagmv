/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_mipmap.c
*   Date: 1/23/2024
*   Version: 0.4b
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <stdlib.h>
#include <agidl_imgp_mipmap.h>
#include <agidl_cc_manager.h>
#include <agidl_img_core.h>
#include <agidl_imgp_impl.h>
#include <agidl_mmu_utils.h>
#include <agidl_file_utils.h>

AGIDL_MIPMAP* AGIDL_CreateMipmapMMU(u32 width, u32 height, AGIDL_CLR_FMT fmt, AGIDL_Bool IsLinear){
	u32 w = width;
	u32 h = height;
	
	u32 mipcount = 0;
	
	while(w >= 2 && h >= 2){
		w >>= 1;
		h >>= 1;
		
		mipcount++;
	}
	
	AGIDL_MIPMAP* mipmap = (AGIDL_MIPMAP*)malloc(sizeof(AGIDL_MIPMAP));
	mipmap->width = width;
	mipmap->height = height;
	mipmap->fmt = fmt;
	mipmap->IsLinear = IsLinear;
	mipmap->mipcount = mipcount;
	
	return mipmap;
}

void AGIDL_DestroyMipmapMMU(AGIDL_MIPMAP* mipmap){
	int i;
	for(i = 0; i < mipmap->mipcount; i++){
		printf("started %dth mipmap level\n",i);
		if(mipmap->mipmap[i].img_data != NULL){
			free(mipmap->mipmap[i].img_data);
			mipmap->mipmap[i].img_data = NULL;
		}
		printf("finished %dth mipmap level\n",i);
	}
	
	free(mipmap);
	
	if(mipmap != NULL){
		mipmap = NULL;
	}
}

AGIDL_MIPMAP* AGIDL_GenerateMipmapFromImgData(void* data, u32 width, u32 height, AGIDL_CLR_FMT fmt){
	AGIDL_MIPMAP* mipmap = AGIDL_CreateMipmapMMU(width,height,fmt,FALSE);
	
	if(AGIDL_GetBitCount(fmt) == 16){
		COLOR16* clr_data = (COLOR16*)data;
		COLOR16* clr_cpy = (COLOR16*)AGIDL_AllocImgDataMMU(width,height,fmt);
		
		mipmap->mipmap[0].width = width;
		mipmap->mipmap[0].height = height;
		mipmap->mipmap[0].fmt = fmt;
		mipmap->mipmap[0].img_data = (COLOR16*)AGIDL_AllocImgDataMMU(width,height,fmt);
		
		AGIDL_ClrMemcpy16(mipmap->mipmap[0].img_data,clr_data,width*height);
		AGIDL_ClrMemcpy16(clr_cpy,clr_data,width*height);
		
		int i;
		for(i = 1; i < mipmap->mipcount; i++){
			
			width >>= 1;
			height >>= 1;
			
			u32 w = width << 1;
			u32 h = height << 1;
			
			mipmap->mipmap[i].width = width;
			mipmap->mipmap[i].height = height;
			mipmap->mipmap[i].fmt = fmt;
			mipmap->mipmap[i].img_data = (COLOR16*)AGIDL_AllocImgDataMMU(width,height,fmt);
			
			clr_cpy = (COLOR16*)AGIDL_HalfImgDataBilerp(clr_cpy,&w,&h,fmt);
			AGIDL_ClrMemcpy16(mipmap->mipmap[i].img_data,clr_cpy,width*height);
		}
		
		free(clr_cpy);
	}
	else{
		COLOR* clr_data = (COLOR*)data;
		COLOR* clr_cpy = (COLOR*)AGIDL_AllocImgDataMMU(width,height,fmt);
		
		mipmap->mipmap[0].width = width;
		mipmap->mipmap[0].height = height;
		mipmap->mipmap[0].fmt = fmt;
		mipmap->mipmap[0].img_data = (COLOR*)AGIDL_AllocImgDataMMU(width,height,fmt);
		
		AGIDL_ClrMemcpy(mipmap->mipmap[0].img_data,clr_data,width*height);
		AGIDL_ClrMemcpy(clr_cpy,clr_data,width*height);
		
		int i;
		for(i = 1; i < mipmap->mipcount; i++){
			
			width >>= 1;
			height >>= 1;
			
			u32 w = width << 1;
			u32 h = height << 1;
			
			mipmap->mipmap[i].width = width;
			mipmap->mipmap[i].height = height;
			mipmap->mipmap[i].fmt = fmt;
			mipmap->mipmap[i].img_data = (COLOR*)AGIDL_AllocImgDataMMU(width,height,fmt);
			
			clr_cpy = (COLOR*)AGIDL_HalfImgDataBilerp(clr_cpy,&w,&h,fmt);
			AGIDL_ClrMemcpy(mipmap->mipmap[i].img_data,clr_cpy,width*height);
		}
		
		free(clr_cpy);
	}
	
	return mipmap;
}

AGIDL_MIPMAP* AGIDL_LoadMipmapImgData(FILE* file, u32 width, u32 height, AGIDL_CLR_FMT fmt, u8 count, AGIDL_Bool IsLinear){
	AGIDL_MIPMAP* mipmap = (AGIDL_MIPMAP*)malloc(sizeof(AGIDL_MIPMAP));
	mipmap->width = width;
	mipmap->height = height;
	mipmap->fmt = fmt;
	mipmap->IsLinear = IsLinear;
	mipmap->mipcount = count;
	
	if(AGIDL_GetBitCount(fmt) == 16){
		int i;
		for(i = 0; i < mipmap->mipcount; i++){
			mipmap->mipmap[i].width = width;
			mipmap->mipmap[i].height = height;
			mipmap->mipmap[i].fmt = fmt;
			mipmap->mipmap[i].img_data = (COLOR16*)AGIDL_AllocImgDataMMU(width,height,fmt);
			AGIDL_ReadBufClr16(file,mipmap->mipmap[i].img_data,width,height);
			
			if(IsLinear == TRUE){
				int x,y;
				for(y = 0; y < height; y++){
					for(x = 0; x < width; x++){
						u32 clr = AGIDL_GetClr16(mipmap->mipmap[i].img_data,x,y,width,height);
						clr = AGIDL_GammaCorrectColor(clr,2.2f,mipmap->fmt);
						AGIDL_SetClr16(mipmap->mipmap[i].img_data,clr,x,y,width,height);
					}
				}
			}
			
			width >>= 1;
			height >>= 1;
		}
	}
	else{
		int i;
		for(i = 0; i < mipmap->mipcount; i++){
			mipmap->mipmap[i].width = width;
			mipmap->mipmap[i].height = height;
			mipmap->mipmap[i].fmt = fmt;
			mipmap->mipmap[i].img_data = (COLOR*)AGIDL_AllocImgDataMMU(width,height,fmt);
			
			if(AGIDL_GetBitCount(fmt) == 24){
				AGIDL_ReadBufRGB(file,mipmap->mipmap[i].img_data,width,height);
			}
			else{
				AGIDL_ReadBufRGBA(file,mipmap->mipmap[i].img_data,width,height);
			}
			
			if(IsLinear == TRUE){
				int x,y;
				for(y = 0; y < height; y++){
					for(x = 0; x < width; x++){
						u32 clr = AGIDL_GetClr(mipmap->mipmap[i].img_data,x,y,width,height);
						clr = AGIDL_GammaCorrectColor(clr,2.2f,mipmap->fmt);
						AGIDL_SetClr(mipmap->mipmap[i].img_data,clr,x,y,width,height);
					}
				}
			}
			
			width >>= 1;
			height >>= 1;
		}
	}
	
	return mipmap;
}

void AGIDL_ExportMipmap(AGIDL_MIPMAP* mipmap, AGIDL_IMG_TYPE img_type, AGIDL_Bool flip){
	int i;
	for(i = 0; i < mipmap->mipcount; i++){
		u32 w = mipmap->mipmap[i].width;
		u32 h = mipmap->mipmap[i].height;
		
		char filename[30];
		
		switch(img_type){
			case AGIDL_IMG_BMP:{
				sprintf(filename,"mipmap_%d.bmp",i+1);
				AGIDL_BMP* bmp = AGIDL_CreateBMP(filename,w,h,mipmap->fmt);
				if(AGIDL_GetBitCount(mipmap->fmt) == 16){
					COLOR16* img = (COLOR16*)mipmap->mipmap[i].img_data;
					AGIDL_BMPSyncPix16(bmp,img);
				}
				else{
					COLOR* img = (COLOR*)mipmap->mipmap[i].img_data;
					AGIDL_BMPSyncPix(bmp,img);
				}
				if(flip == TRUE){
					AGIDL_FlipHorzBMP(bmp);
				}
				AGIDL_ExportBMP(bmp);
				AGIDL_FreeBMP(bmp);
			}break;
			case AGIDL_IMG_TGA:{
				sprintf(filename,"mipmap_%d.tga",i+1);
				AGIDL_TGA* tga = AGIDL_CreateTGA(filename,w,h,mipmap->fmt);
				if(AGIDL_GetBitCount(mipmap->fmt) == 16){
					COLOR16* img = (COLOR16*)mipmap->mipmap[i].img_data;
					AGIDL_TGASyncPix16(tga,img);
				}
				else{
					COLOR* img = (COLOR*)mipmap->mipmap[i].img_data;
					AGIDL_TGASyncPix(tga,img);
				}
				if(flip == TRUE){
					AGIDL_FlipHorzTGA(tga);
				}
				AGIDL_ExportTGA(tga);
				AGIDL_FreeTGA(tga);
			}break;
			case AGIDL_IMG_TIM:{
				sprintf(filename,"mipmap_%d.tim",i+1);
				AGIDL_TIM* tim = AGIDL_CreateTIM(filename,w,h,mipmap->fmt);
				if(AGIDL_GetBitCount(mipmap->fmt) == 16){
					COLOR16* img = (COLOR16*)mipmap->mipmap[i].img_data;
					AGIDL_TIMSyncPix16(tim,img);
				}
				else{
					COLOR* img = (COLOR*)mipmap->mipmap[i].img_data;
					AGIDL_TIMSyncPix(tim,img);
				}
				if(flip == TRUE){
					AGIDL_FlipHorzTIM(tim);
				}
				AGIDL_ExportTIM(tim);
				AGIDL_FreeTIM(tim);
			}break;
			case AGIDL_IMG_PCX:{
				sprintf(filename,"mipmap_%d.pcx",i+1);
				AGIDL_PCX* pcx = AGIDL_CreatePCX(filename,w,h,mipmap->fmt);
				if(AGIDL_GetBitCount(mipmap->fmt) == 16){
					COLOR16* img = (COLOR16*)mipmap->mipmap[i].img_data;
					AGIDL_PCXSyncPix16(pcx,img);
				}
				else{
					COLOR* img = (COLOR*)mipmap->mipmap[i].img_data;
					AGIDL_PCXSyncPix(pcx,img);
				}
				if(flip == TRUE){
					AGIDL_FlipHorzPCX(pcx);
				}
				AGIDL_ExportPCX(pcx);
				AGIDL_FreePCX(pcx);
			}break;
			case AGIDL_IMG_LMP:{
				sprintf(filename,"mipmap_%d.lmp",i+1);
				AGIDL_LMP* lmp = AGIDL_CreateLMP(filename,w,h,mipmap->fmt);
				if(AGIDL_GetBitCount(mipmap->fmt) == 16){
					COLOR16* img = (COLOR16*)mipmap->mipmap[i].img_data;
					AGIDL_LMPSyncPix16(lmp,img);
				}
				else{
					COLOR* img = (COLOR*)mipmap->mipmap[i].img_data;
					AGIDL_LMPSyncPix(lmp,img);
				}
				if(flip == TRUE){
					AGIDL_FlipHorzLMP(lmp);
				}
				AGIDL_ExportLMP(lmp);
				AGIDL_FreeLMP(lmp);
			}break;
			case AGIDL_IMG_PVR:{
				sprintf(filename,"mipmap_%d.pvr",i+1);
				AGIDL_PVR* pvr = AGIDL_CreatePVR(filename,w,h,mipmap->fmt);
				if(AGIDL_GetBitCount(mipmap->fmt) == 16){
					COLOR16* img = (COLOR16*)mipmap->mipmap[i].img_data;
					AGIDL_PVRSyncPix16(pvr,img);
				}
				else{
					COLOR* img = (COLOR*)mipmap->mipmap[i].img_data;
					AGIDL_PVRSyncPix(pvr,img);
				}
				if(flip == TRUE){
					AGIDL_FlipHorzPVR(pvr);
				}
				AGIDL_ExportPVR(pvr);
				AGIDL_FreePVR(pvr);
			}break;
			case AGIDL_IMG_GXT:{
				sprintf(filename,"mipmap_%d.gxt",i+1);
				AGIDL_GXT* gxt = AGIDL_CreateGXT(filename,w,h,mipmap->fmt);
				if(AGIDL_GetBitCount(mipmap->fmt) == 16){
					COLOR16* img = (COLOR16*)mipmap->mipmap[i].img_data;
					AGIDL_GXTSyncPix16(gxt,img);
				}
				else{
					COLOR* img = (COLOR*)mipmap->mipmap[i].img_data;
					AGIDL_GXTSyncPix(gxt,img);
				}
				if(flip == TRUE){
					AGIDL_FlipHorzGXT(gxt);
				}
				AGIDL_ExportGXT(gxt);
				AGIDL_FreeGXT(gxt);
			}break;
			case AGIDL_IMG_BTI:{
				sprintf(filename,"mipmap_%d.bti",i+1);
				AGIDL_BTI* bti = AGIDL_CreateBTI(filename,w,h,mipmap->fmt);
				if(AGIDL_GetBitCount(mipmap->fmt) == 16){
					COLOR16* img = (COLOR16*)mipmap->mipmap[i].img_data;
					AGIDL_BTISyncPix16(bti,img);
				}
				else{
					COLOR* img = (COLOR*)mipmap->mipmap[i].img_data;
					AGIDL_BTISyncPix(bti,img);
				}
				if(flip == TRUE){
					AGIDL_FlipHorzBTI(bti);
				}
				AGIDL_ExportBTI(bti);
				AGIDL_FreeBTI(bti);
			}break;
			case AGIDL_IMG_3DF:{
				sprintf(filename,"mipmap_%d.3df",i+1);
				AGIDL_3DF* glide = AGIDL_Create3DF(filename,w,h,mipmap->fmt);
				if(AGIDL_GetBitCount(mipmap->fmt) == 16){
					COLOR16* img = (COLOR16*)mipmap->mipmap[i].img_data;
					AGIDL_3DFSyncPix16(glide,img);
				}
				else{
					COLOR* img = (COLOR*)mipmap->mipmap[i].img_data;
					AGIDL_3DFSyncPix(glide,img);
				}
				if(flip == TRUE){
					AGIDL_FlipHorz3DF(glide);
				}
				AGIDL_Export3DF(glide);
				AGIDL_Free3DF(glide);
			}break;
			case AGIDL_IMG_PPM:{
				sprintf(filename,"mipmap_%d.3df",i+1);
				AGIDL_PPM* ppm = AGIDL_CreatePPM(filename,w,h,mipmap->fmt);
				if(AGIDL_GetBitCount(mipmap->fmt) == 16){
					COLOR16* img = (COLOR16*)mipmap->mipmap[i].img_data;
					AGIDL_PPMSyncPix16(ppm,img);
				}
				else{
					COLOR* img = (COLOR*)mipmap->mipmap[i].img_data;
					AGIDL_PPMSyncPix(ppm,img);
				}
				if(flip == TRUE){
					AGIDL_FlipHorzPPM(ppm);
				}
				AGIDL_ExportPPM(ppm);
				AGIDL_FreePPM(ppm);
			}break;
			case AGIDL_IMG_LBM:{
				sprintf(filename,"mipmap_%d.3df",i+1);
				AGIDL_LBM* lbm = AGIDL_CreateLBM(filename,w,h,mipmap->fmt);
				AGIDL_LBMSetMaxDiff(lbm,19);
				AGIDL_LBMSetICPEncoding(lbm,ICP_ENCODE_THRESHOLD);
				if(AGIDL_GetBitCount(mipmap->fmt) == 16){
					COLOR16* img = (COLOR16*)mipmap->mipmap[i].img_data;
					AGIDL_LBMSyncPix16(lbm,img);
				}
				else{
					COLOR* img = (COLOR*)mipmap->mipmap[i].img_data;
					AGIDL_LBMSyncPix(lbm,img);
				}
				if(flip == TRUE){
					AGIDL_FlipHorzLBM(lbm);
				}
				AGIDL_ExportLBM(lbm);
				AGIDL_FreeLBM(lbm);
			}break;
		}
	}
}

void AGIDL_FilterBilerpMipmapLevel(AGIDL_MIPMAP* mipmap, u8 mip_lvl){
	if(mip_lvl <= mipmap->mipcount){
		AGIDL_FilterImgDataBilerp(mipmap->mipmap[mip_lvl].img_data,mipmap->mipmap[mip_lvl].width,mipmap->mipmap[mip_lvl].height,mipmap->mipmap[mip_lvl].fmt);
	}
}

void AGIDL_FilterBilerpMipmap(AGIDL_MIPMAP* mipmap){
	int i;
	for(i = 0; i < mipmap->mipcount; i++){
		AGIDL_FilterBilerpMipmapLevel(mipmap,i);
	}
}

void AGIDL_ClearMipmapLevel(AGIDL_MIPMAP* mipmap, u8 mip_lvl){
	if(mip_lvl <= mipmap->mipcount){
		if(AGIDL_GetBitCount(mipmap->mipmap[mip_lvl].fmt) == 16){
			AGIDL_ClrMemset16(mipmap->mipmap[mip_lvl].img_data,0,mipmap->mipmap[mip_lvl].width*mipmap->mipmap[mip_lvl].height);
		}
		else{
			AGIDL_ClrMemset(mipmap->mipmap[mip_lvl].img_data,0,mipmap->mipmap[mip_lvl].width*mipmap->mipmap[mip_lvl].height);
		}
	}
}

void AGIDL_RebuildMipmapLevel(AGIDL_MIPMAP* mipmap, u8 mip_lvl){
	if(mip_lvl <= mipmap->mipcount){
		if(mip_lvl == 0){
			if(AGIDL_GetBitCount(mipmap->fmt) == 16){
				COLOR16* data = (COLOR16*)mipmap->mipmap[mip_lvl+1].img_data;
				COLOR16* data_cpy = (COLOR16*)AGIDL_AllocImgDataMMU(mipmap->mipmap[mip_lvl+1].width,mipmap->mipmap[mip_lvl+1].height,mipmap->mipmap[mip_lvl+1].fmt);
				
				u32 w = mipmap->mipmap[mip_lvl+1].width;
				u32 h = mipmap->mipmap[mip_lvl+1].height;
				
				AGIDL_CLR_FMT fmt = mipmap->mipmap[mip_lvl+1].fmt;
				
				AGIDL_ClrMemcpy16(data_cpy,data,w*h);
				
				COLOR16* rebuild = (COLOR16*)AGIDL_DoubleImgDataNearest(data_cpy,&w,&h,fmt);
				
				AGIDL_ClrMemcpy16(mipmap->mipmap[mip_lvl].img_data,rebuild,w*h);
				
				free(rebuild);
				
			}
			else{
				COLOR* data = (COLOR*)mipmap->mipmap[mip_lvl+1].img_data;
				COLOR* data_cpy = (COLOR*)AGIDL_AllocImgDataMMU(mipmap->mipmap[mip_lvl+1].width,mipmap->mipmap[mip_lvl+1].height,mipmap->mipmap[mip_lvl+1].fmt);
				
				u32 w = mipmap->mipmap[mip_lvl+1].width;
				u32 h = mipmap->mipmap[mip_lvl+1].height;
				
				AGIDL_CLR_FMT fmt = mipmap->mipmap[mip_lvl+1].fmt;
				
				AGIDL_ClrMemcpy(data_cpy,data,w*h);
				
				COLOR* rebuild = (COLOR*)AGIDL_DoubleImgDataNearest(data_cpy,&w,&h,fmt);
				
				AGIDL_ClrMemcpy(mipmap->mipmap[mip_lvl].img_data,rebuild,w*h);
				
				free(rebuild);
			}
		}
		else{
			if(AGIDL_GetBitCount(mipmap->fmt) == 16){
				COLOR16* data = (COLOR16*)mipmap->mipmap[mip_lvl-1].img_data;
				COLOR16* data_cpy = (COLOR16*)AGIDL_AllocImgDataMMU(mipmap->mipmap[mip_lvl-1].width,mipmap->mipmap[mip_lvl-1].height,mipmap->mipmap[mip_lvl-1].fmt);
				
				u32 w = mipmap->mipmap[mip_lvl-1].width;
				u32 h = mipmap->mipmap[mip_lvl-1].height;
				
				AGIDL_CLR_FMT fmt = mipmap->mipmap[mip_lvl-1].fmt;
				
				AGIDL_ClrMemcpy16(data_cpy,data,w*h);
				
				COLOR16* rebuild = (COLOR16*)AGIDL_HalfImgDataBilerp(data_cpy,&w,&h,fmt);
				
				AGIDL_ClrMemcpy16(mipmap->mipmap[mip_lvl].img_data,rebuild,w*h);
				
				free(rebuild);
				
			}
			else{
				COLOR* data = (COLOR*)mipmap->mipmap[mip_lvl-1].img_data;
				COLOR* data_cpy = (COLOR*)AGIDL_AllocImgDataMMU(mipmap->mipmap[mip_lvl-1].width,mipmap->mipmap[mip_lvl-1].height,mipmap->mipmap[mip_lvl-1].fmt);
				
				u32 w = mipmap->mipmap[mip_lvl-1].width;
				u32 h = mipmap->mipmap[mip_lvl-1].height;
				
				AGIDL_CLR_FMT fmt = mipmap->mipmap[mip_lvl-1].fmt;
				
				AGIDL_ClrMemcpy(data_cpy,data,w*h);
				
				COLOR* rebuild = (COLOR*)AGIDL_HalfImgDataBilerp(data_cpy,&w,&h,fmt);
				
				AGIDL_ClrMemcpy(mipmap->mipmap[mip_lvl].img_data,rebuild,w*h);
				
				free(rebuild);
			}
		}
	}
}

void AGIDL_BlendMipmapLevel(AGIDL_MIPMAP* mipmap, u8 mip_lvl1, u8 mip_lvl2){
	if(mip_lvl1 != mip_lvl2 && mip_lvl1 <= mipmap->mipcount && mip_lvl2 <= mipmap->mipcount){
		if(AGIDL_GetBitCount(mipmap->fmt) == 16){
			COLOR16* img1 = (COLOR16*)mipmap->mipmap[mip_lvl1].img_data;
			u32 w1 = mipmap->mipmap[mip_lvl1].width;
			u32 h1 = mipmap->mipmap[mip_lvl1].height;
			
			COLOR16* img2 = (COLOR16*)mipmap->mipmap[mip_lvl2].img_data;
			u32 w2 = mipmap->mipmap[mip_lvl2].width;
			u32 h2 = mipmap->mipmap[mip_lvl2].height;
			
			COLOR16* img1_cpy = (COLOR16*)AGIDL_AllocImgDataMMU(w1,h1,mipmap->fmt);
			AGIDL_ClrMemcpy16(img1_cpy,img1,w1*h1);
			
			float xscale = (float)w2/w1;
			float yscale = (float)h2/h1;
			
			AGIDL_CLR_FMT fmt = mipmap->fmt;
			
			int x,y;
			for(y = 0; y < h1; y++){
				for(x = 0; x < w1; x++){
					u32 x2 = (x*xscale);
					u32 y2 = (y*yscale);
					
					COLOR16 clr1 = AGIDL_GetClr16(img1_cpy,x,y,w1,h1);
					COLOR16 clr2 = AGIDL_GetClr16(img1_cpy,x+1,y,w1,h1);
					COLOR16 clr3 = AGIDL_GetClr16(img1_cpy,x,y+1,w1,h1);
					COLOR16 clr4 = AGIDL_GetClr16(img1_cpy,x+1,y+1,w1,h1);
					
					COLOR16 clr12 = AGIDL_GetClr16(img2,x2,y2,w2,h2);
					COLOR16 clr22 = AGIDL_GetClr16(img2,x2+1,y2,w2,h2);
					COLOR16 clr32 = AGIDL_GetClr16(img2,x2,y2+1,w2,h2);
					COLOR16 clr42 = AGIDL_GetClr16(img2,x2+1,y2+1,w2,h2);
					
					u8 r1 = AGIDL_GetR(clr1,fmt);
					u8 g1 = AGIDL_GetG(clr1,fmt);
					u8 b1 = AGIDL_GetB(clr1,fmt);
					
					u8 r2 = AGIDL_GetR(clr2,fmt);
					u8 g2 = AGIDL_GetG(clr2,fmt);
					u8 b2 = AGIDL_GetB(clr2,fmt);
					
					u8 r3 = AGIDL_GetR(clr3,fmt);
					u8 g3 = AGIDL_GetG(clr3,fmt);
					u8 b3 = AGIDL_GetB(clr3,fmt);
					
					u8 r4 = AGIDL_GetR(clr4,fmt);
					u8 g4 = AGIDL_GetG(clr4,fmt);
					u8 b4 = AGIDL_GetB(clr4,fmt);
					
					u8 rtop = r1 + ((r2-r1) >> 1);
					u8 gtop = g1 + ((g2-g1) >> 1);
					u8 btop = b1 + ((b2-b1) >> 1);
					
					u8 rbot = r3 + ((r4-r3) >> 1);
					u8 gbot = g3 + ((g4-g3) >> 1);
					u8 bbot = b3 + ((b4-b3) >> 1);
					
					u8 rbilerp1 = rtop + ((rbot-rtop) >> 1);
					u8 gbilerp1 = gtop + ((gbot-gtop) >> 1);
					u8 bbilerp1 = btop + ((bbot-btop) >> 1);
					
					r1 = AGIDL_GetR(clr12,fmt);
					g1 = AGIDL_GetG(clr12,fmt);
					b1 = AGIDL_GetB(clr12,fmt);
					
					r2 = AGIDL_GetR(clr22,fmt);
					g2 = AGIDL_GetG(clr22,fmt);
					b2 = AGIDL_GetB(clr22,fmt);
					
					r3 = AGIDL_GetR(clr32,fmt);
					g3 = AGIDL_GetG(clr32,fmt);
					b3 = AGIDL_GetB(clr32,fmt);
					
					r4 = AGIDL_GetR(clr42,fmt);
					g4 = AGIDL_GetG(clr42,fmt);
					b4 = AGIDL_GetB(clr42,fmt);
					
					rtop = r1 + ((r2-r1) >> 1);
					gtop = g1 + ((g2-g1) >> 1);
					btop = b1 + ((b2-b1) >> 1);
					
					rbot = r3 + ((r4-r3) >> 1);
					gbot = g3 + ((g4-g3) >> 1);
					bbot = b3 + ((b4-b3) >> 1);
					
					u8 rbilerp2 = rtop + ((rbot-rtop) >> 1);
					u8 gbilerp2 = gtop + ((gbot-gtop) >> 1);
					u8 bbilerp2 = btop + ((bbot-btop) >> 1);
					
					u8 rfinal = rbilerp1 + ((rbilerp2-rbilerp1) >> 1);
					u8 gfinal = gbilerp1 + ((gbilerp2-gbilerp1) >> 1);
					u8 bfinal = bbilerp1 + ((bbilerp2-bbilerp1) >> 1);
					
					AGIDL_SetClr16(img1,AGIDL_RGB16(rfinal,gfinal,bfinal,fmt),x,y,w1,h1);
				}
			}
			
			free(img1_cpy);
		}
		else{
			COLOR* img1 = (COLOR*)mipmap->mipmap[mip_lvl1].img_data;
			u32 w1 = mipmap->mipmap[mip_lvl1].width;
			u32 h1 = mipmap->mipmap[mip_lvl1].height;
			
			COLOR* img2 = (COLOR*)mipmap->mipmap[mip_lvl2].img_data;
			u32 w2 = mipmap->mipmap[mip_lvl2].width;
			u32 h2 = mipmap->mipmap[mip_lvl2].height;
			
			COLOR* img1_cpy = (COLOR*)AGIDL_AllocImgDataMMU(w1,h1,mipmap->fmt);
			AGIDL_ClrMemcpy(img1_cpy,img1,w1*h1);
			
			float xscale = (float)w2/w1;
			float yscale = (float)h2/h1;
			
			AGIDL_CLR_FMT fmt = mipmap->fmt;
			
			int x,y;
			for(y = 0; y < h1; y++){
				for(x = 0; x < w1; x++){
					u32 x2 = (x*xscale);
					u32 y2 = (y*yscale);
					
					COLOR clr1 = AGIDL_GetClr(img1_cpy,x,y,w1,h1);
					COLOR clr2 = AGIDL_GetClr(img1_cpy,x+1,y,w1,h1);
					COLOR clr3 = AGIDL_GetClr(img1_cpy,x,y+1,w1,h1);
					COLOR clr4 = AGIDL_GetClr(img1_cpy,x+1,y+1,w1,h1);
					
					COLOR clr12 = AGIDL_GetClr(img2,x2,y2,w2,h2);
					COLOR clr22 = AGIDL_GetClr(img2,x2+1,y2,w2,h2);
					COLOR clr32 = AGIDL_GetClr(img2,x2,y2+1,w2,h2);
					COLOR clr42 = AGIDL_GetClr(img2,x2+1,y2+1,w2,h2);
					
					u8 r1 = AGIDL_GetR(clr1,fmt);
					u8 g1 = AGIDL_GetG(clr1,fmt);
					u8 b1 = AGIDL_GetB(clr1,fmt);
					
					u8 r2 = AGIDL_GetR(clr2,fmt);
					u8 g2 = AGIDL_GetG(clr2,fmt);
					u8 b2 = AGIDL_GetB(clr2,fmt);
					
					u8 r3 = AGIDL_GetR(clr3,fmt);
					u8 g3 = AGIDL_GetG(clr3,fmt);
					u8 b3 = AGIDL_GetB(clr3,fmt);
					
					u8 r4 = AGIDL_GetR(clr4,fmt);
					u8 g4 = AGIDL_GetG(clr4,fmt);
					u8 b4 = AGIDL_GetB(clr4,fmt);
					
					u8 rtop = r1 + ((r2-r1) >> 1);
					u8 gtop = g1 + ((g2-g1) >> 1);
					u8 btop = b1 + ((b2-b1) >> 1);
					
					u8 rbot = r3 + ((r4-r3) >> 1);
					u8 gbot = g3 + ((g4-g3) >> 1);
					u8 bbot = b3 + ((b4-b3) >> 1);
					
					u8 rbilerp1 = rtop + ((rbot-rtop) >> 1);
					u8 gbilerp1 = gtop + ((gbot-gtop) >> 1);
					u8 bbilerp1 = btop + ((bbot-btop) >> 1);
					
					r1 = AGIDL_GetR(clr12,fmt);
					g1 = AGIDL_GetG(clr12,fmt);
					b1 = AGIDL_GetB(clr12,fmt);
					
					r2 = AGIDL_GetR(clr22,fmt);
					g2 = AGIDL_GetG(clr22,fmt);
					b2 = AGIDL_GetB(clr22,fmt);
					
					r3 = AGIDL_GetR(clr32,fmt);
					g3 = AGIDL_GetG(clr32,fmt);
					b3 = AGIDL_GetB(clr32,fmt);
					
					r4 = AGIDL_GetR(clr42,fmt);
					g4 = AGIDL_GetG(clr42,fmt);
					b4 = AGIDL_GetB(clr42,fmt);
					
					rtop = r1 + ((r2-r1) >> 1);
					gtop = g1 + ((g2-g1) >> 1);
					btop = b1 + ((b2-b1) >> 1);
					
					rbot = r3 + ((r4-r3) >> 1);
					gbot = g3 + ((g4-g3) >> 1);
					bbot = b3 + ((b4-b3) >> 1);
					
					u8 rbilerp2 = rtop + ((rbot-rtop) >> 1);
					u8 gbilerp2 = gtop + ((gbot-gtop) >> 1);
					u8 bbilerp2 = btop + ((bbot-btop) >> 1);
					
					u8 rfinal = rbilerp1 + ((rbilerp2-rbilerp1) >> 1);
					u8 gfinal = gbilerp1 + ((gbilerp2-gbilerp1) >> 1);
					u8 bfinal = bbilerp1 + ((bbilerp2-bbilerp1) >> 1);
					
					AGIDL_SetClr(img1,AGIDL_RGB(rfinal,gfinal,bfinal,fmt),x,y,w1,h1);
				}
			}
			
			free(img1_cpy);
		}
	}
}
