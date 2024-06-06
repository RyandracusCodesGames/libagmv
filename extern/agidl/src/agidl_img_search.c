#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "agidl_img_search.h"
#include "agidl_img_error.h"
#include "agidl_file_utils.h"
#include "agidl_img_converter.h"

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_search.c
*   Date: 11/11/2023
*   Version: 0.1b
*   Updated: 3/2/2024
*   Author: Ryandracus Chapman
*
********************************************/

void AGIDL_TIMSearchFileOnDisk(const char* filename, AGIDL_IMG_TYPE type, int flip){
	FILE* file = fopen(filename,"rb");
	
	if(file == NULL){
		printf("Could not locate/open file - %s!\n",filename);
		return;
	}
	
	fseek(file,0,SEEK_END);
	
	u32 img_count = 0, file_size = ftell(file);
	
	fseek(file,0,SEEK_SET);
	
	while(!feof(file)){
	
		AGIDL_TIM *tim = (AGIDL_TIM*)malloc(sizeof(AGIDL_TIM));
		
		AGIDL_TIMDecodeHeader(tim,file);

		while(!feof(file) && !AGIDL_IsTIMHeader(tim)){
			u32 curr_coord = ftell(file);
			if(curr_coord >= file_size){
				printf("Exceeded file size! Closing file stream!\n");
				fclose(file);
				return;
			}
			fseek(file,curr_coord-7,SEEK_SET);
			AGIDL_TIMDecodeHeader(tim,file);
		}
		
		AGIDL_TIMDecodeIMG(tim,file);
		
		u32 curr = ftell(file);
		
		if(AGIDL_IsTIM(tim)){
			img_count++;
			switch(type){
				case AGIDL_IMG_TIM:{
					char file_name[25];
					sprintf(file_name,"tim_%ld.tim",img_count);
					tim->filename = file_name;
					AGIDL_ExportTIM(tim);
				}break;
				case AGIDL_IMG_BMP:{
					char file_name[25] = {0};
					sprintf(file_name,"tim_%ld.tim",img_count);
					tim->filename = (char*)malloc(strlen(file_name)+1);
					AGIDL_FilenameCpy(tim->filename,file_name);
					AGIDL_BMP* bmp = AGIDL_ConvertTIM2BMP(tim);
					if(flip == 1){
						AGIDL_FlipHorzBMP(bmp);
					}
					AGIDL_ExportBMP(bmp);
					AGIDL_FreeBMP(bmp);
				}break;
				case AGIDL_IMG_TGA:{
					char file_name[25] = {0};
					sprintf(file_name,"tim_%ld.tim",img_count);
					printf("%s\n",file_name);
					tim->filename = (char*)malloc(strlen(file_name)+1);
					AGIDL_FilenameCpy(tim->filename,file_name);
					printf("tim = %s\n",tim->filename);
					AGIDL_TGA* tga = AGIDL_ConvertTIM2TGA(tim);
					printf("converted to tga...\n");
					if(flip == 1){
						AGIDL_FlipHorzTGA(tga);
						printf("flipped tga...\n");
					}
					AGIDL_ExportTGA(tga);
					printf("exported tga...\n");
					AGIDL_FreeTGA(tga);
				}break;
				case AGIDL_IMG_PCX:{
					char file_name[25] = {0};
					sprintf(file_name,"tim_%ld.tim",img_count);
					tim->filename = (char*)malloc(strlen(file_name)+1);
					AGIDL_FilenameCpy(tim->filename,file_name);
					AGIDL_PCX* pcx = AGIDL_ConvertTIM2PCX(tim);
					if(flip == 1){
						AGIDL_FlipHorzPCX(pcx);
					}
					AGIDL_ExportPCX(pcx);
					AGIDL_FreePCX(pcx);
				}break;
				case AGIDL_IMG_LMP:{
					char file_name[25] = {0};
					sprintf(file_name,"tim_%ld.tim",img_count);
					tim->filename = (char*)malloc(strlen(file_name)+1);
					AGIDL_FilenameCpy(tim->filename,file_name);
					AGIDL_LMP* lmp = AGIDL_ConvertTIM2LMP(tim);
					if(flip == 1){
						AGIDL_FlipHorzLMP(lmp);
					}
					AGIDL_LMPSetMaxDiff(lmp,32);
					AGIDL_ExportLMP(lmp);
					AGIDL_FreeLMP(lmp);
				}break;
				case AGIDL_IMG_PVR:{
					char file_name[25] = {0};
					sprintf(file_name,"tim_%ld.tim",img_count);
					tim->filename = (char*)malloc(strlen(file_name)+1);
					AGIDL_FilenameCpy(tim->filename,file_name);
					AGIDL_PVR* pvr = AGIDL_ConvertTIM2PVR(tim);
					if(flip == 1){
						AGIDL_FlipHorzPVR(pvr);
					}
					AGIDL_ExportPVR(pvr);
					AGIDL_FreePVR(pvr);
				}break;
				case AGIDL_IMG_GXT:{
					char file_name[25] = {0};
					sprintf(file_name,"tim_%ld.tim",img_count);
					tim->filename = (char*)malloc(strlen(file_name)+1);
					AGIDL_FilenameCpy(tim->filename,file_name);
					AGIDL_GXT* gxt = AGIDL_ConvertTIM2GXT(tim);
					if(flip == 1){
						AGIDL_FlipHorzGXT(gxt);
					}
					AGIDL_ExportGXT(gxt);
					AGIDL_FreeGXT(gxt);
				}break;
				case AGIDL_IMG_BTI:{
					char file_name[25] = {0};
					sprintf(file_name,"tim_%ld.tim",img_count);
					tim->filename = (char*)malloc(strlen(file_name)+1);
					AGIDL_FilenameCpy(tim->filename,file_name);
					AGIDL_BTI* bti = AGIDL_ConvertTIM2BTI(tim);
					if(flip == 1){
						AGIDL_FlipHorzBTI(bti);
					}
					AGIDL_ExportBTI(bti);
					AGIDL_FreeBTI(bti);
				}break;
				case AGIDL_IMG_3DF:{
					char file_name[25] = {0};
					sprintf(file_name,"tim_%ld.tim",img_count);
					tim->filename = (char*)malloc(strlen(file_name)+1);
					AGIDL_FilenameCpy(tim->filename,file_name);
					AGIDL_3DF* glide = AGIDL_ConvertTIM23DF(tim);
					if(flip == 1){
						AGIDL_FlipHorz3DF(glide);
					}
					AGIDL_Export3DF(glide);
					AGIDL_Free3DF(glide);
				}break;
				case AGIDL_IMG_PPM:{
					char file_name[25] = {0};
					sprintf(file_name,"tim_%ld.tim",img_count);
					tim->filename = (char*)malloc(strlen(file_name)+1);
					AGIDL_FilenameCpy(tim->filename,file_name);
					AGIDL_PPM* ppm = AGIDL_ConvertTIM2PPM(tim);
					if(flip == 1){
						AGIDL_FlipHorzPPM(ppm);
					}
					AGIDL_ExportPPM(ppm);
					AGIDL_FreePPM(ppm);
				}break;
				case AGIDL_IMG_LBM:{
					char file_name[25] = {0};
					sprintf(file_name,"tim_%ld.tim",img_count);
					tim->filename = (char*)malloc(strlen(file_name)+1);
					AGIDL_FilenameCpy(tim->filename,file_name);
					AGIDL_ColorConvertTIM(tim,AGIDL_RGB_888);
					AGIDL_LBM* lbm = AGIDL_ConvertTIM2LBM(tim);
					AGIDL_LBMSetICPEncoding(lbm,ICP_ENCODE_THRESHOLD);
					AGIDL_LBMSetMaxDiff(lbm,19);
					if(flip == 1){
						AGIDL_FlipHorzLBM(lbm);
					}
					AGIDL_ExportLBM(lbm);
					AGIDL_FreeLBM(lbm);
				}break;
				
			}
			
			AGIDL_FreeTIM(tim);
		}
		else{
			if(tim->header.version == 0x08){
				if(tim->clut_header.num_clrs == 16 || tim->clut_header.num_icps == 16){
					fseek(file,curr-63,SEEK_SET);
				}
				else{
					fseek(file,curr-19,SEEK_SET);
				}
			}
			else if(tim->header.version == 0x09){
				if(tim->clut_header.num_clrs == 256 || tim->clut_header.num_icps == 256){
					fseek(file,curr-543,SEEK_SET);
				}
				else{
					fseek(file,curr-19,SEEK_SET);
				}
			}
			else{
				fseek(file,curr-19,SEEK_SET);
			}
			
			free(tim);
			
			if(curr >= file_size){
				printf("Exceeded file size! Closing file stream!\n");
				fclose(file);
				return;
			}
		}
	}

	fclose(file);
}

int AGIDL_3DFSearchFileOnDisk(const char* filename, AGIDL_IMG_TYPE img_type, int flip, u32 jump){
	FILE* file = fopen(filename,"rb");
	
	if(file == NULL){
		return FILE_NOT_LOCATED_IMG_ERROR;
	} 
	
	u32 img_count = 0, file_size = 0;
	
	fseek(file,0,SEEK_END);
	file_size = ftell(file);
	fseek(file,jump,SEEK_SET);
	
	while(!feof(file)){
		AGIDL_3DF* glide = (AGIDL_3DF*)malloc(sizeof(AGIDL_3DF));
		
		int error = AGIDL_3DFDecodePartialHeader(glide,file);

		while(!feof(file) && error != NO_IMG_ERROR){

			int pos = ftell(file);
			
			if(pos >= file_size){
				printf("Exceeded file size! Closing file stream!\n");
				fclose(file);
				return MEMORY_IMG_ERROR;
			}
			
			fseek(file,pos-8,SEEK_SET);
			error = AGIDL_3DFDecodePartialHeader(glide,file);
		}
		
		u32 curr = ftell(file);
		fseek(file,curr-9,SEEK_SET);

		AGIDL_3DFDecodeHeader(glide,file);
		AGIDL_3DFDecodeIMG(glide,file);
		
		img_count++; 
		
		switch(img_type){
			case AGIDL_IMG_BMP:{
				char filename[25];
				sprintf(filename,"3df_%ld.3df",img_count);
				glide->filename = (char*)malloc(strlen(filename)+1);
				AGIDL_FilenameCpy(glide->filename,filename);
				AGIDL_BMP* bmp = AGIDL_Convert3DF2BMP(glide);
				if(flip == TRUE){
					AGIDL_FlipHorzBMP(bmp);
				}
				AGIDL_ExportBMP(bmp);
				AGIDL_FreeBMP(bmp);
			}break;
			case AGIDL_IMG_TGA:{
				char filename[25];
				sprintf(filename,"3df_%ld.3df",img_count);
				glide->filename = (char*)malloc(strlen(filename)+1);
				AGIDL_FilenameCpy(glide->filename,filename);
				AGIDL_TGA* tga = AGIDL_Convert3DF2TGA(glide);
				if(flip == TRUE){
					AGIDL_FlipHorzTGA(tga);
				}
				AGIDL_ExportTGA(tga);
				AGIDL_FreeTGA(tga);
			}break;
			case AGIDL_IMG_TIM:{
				char filename[25];
				sprintf(filename,"3df_%ld.3df",img_count);
				glide->filename = (char*)malloc(strlen(filename)+1);
				AGIDL_FilenameCpy(glide->filename,filename);
				AGIDL_TIM* tim = AGIDL_Convert3DF2TIM(glide);
				if(flip == TRUE){
					AGIDL_FlipHorzTIM(tim);
				}
				AGIDL_ExportTIM(tim);
				AGIDL_FreeTIM(tim);
			}break;
			case AGIDL_IMG_PCX:{
				char filename[25];
				sprintf(filename,"3df_%ld.3df",img_count);
				glide->filename = (char*)malloc(strlen(filename)+1);
				AGIDL_FilenameCpy(glide->filename,filename);
				AGIDL_PCX* pcx = AGIDL_Convert3DF2PCX(glide);
				if(flip == TRUE){
					AGIDL_FlipHorzPCX(pcx);
				}
				AGIDL_ExportPCX(pcx);
				AGIDL_FreePCX(pcx);
			}break;
			case AGIDL_IMG_LMP:{
				char filename[25];
				sprintf(filename,"3df_%ld.3df",img_count);
				glide->filename = (char*)malloc(strlen(filename)+1);
				AGIDL_FilenameCpy(glide->filename,filename);
				AGIDL_LMP* lmp = AGIDL_Convert3DF2LMP(glide);
				if(flip == TRUE){
					AGIDL_FlipHorzLMP(lmp);
				}
				AGIDL_ExportLMP(lmp);
				AGIDL_FreeLMP(lmp);
			}break;
			case AGIDL_IMG_PVR:{
				char filename[25];
				sprintf(filename,"3df_%ld.3df",img_count);
				glide->filename = (char*)malloc(strlen(filename)+1);
				AGIDL_FilenameCpy(glide->filename,filename);
				AGIDL_PVR* pvr = AGIDL_Convert3DF2PVR(glide);
				if(flip == TRUE){
					AGIDL_FlipHorzPVR(pvr);
				}
				AGIDL_ExportPVR(pvr);
				AGIDL_FreePVR(pvr);
			}break;
			case AGIDL_IMG_GXT:{
				char filename[25];
				sprintf(filename,"3df_%ld.3df",img_count);
				glide->filename = (char*)malloc(strlen(filename)+1);
				AGIDL_FilenameCpy(glide->filename,filename);
				AGIDL_GXT* gxt = AGIDL_Convert3DF2GXT(glide);
				if(flip == TRUE){
					AGIDL_FlipHorzGXT(gxt);
				}
				AGIDL_ExportGXT(gxt);
				AGIDL_FreeGXT(gxt);
			}break;
			case AGIDL_IMG_BTI:{
				char filename[25];
				sprintf(filename,"3df_%ld.3df",img_count);
				glide->filename = (char*)malloc(strlen(filename)+1);
				AGIDL_FilenameCpy(glide->filename,filename);
				AGIDL_BTI* bti = AGIDL_Convert3DF2BTI(glide);
				if(flip == TRUE){
					AGIDL_FlipHorzBTI(bti);
				}
				AGIDL_ExportBTI(bti);
				AGIDL_FreeBTI(bti);
			}break;
			case AGIDL_IMG_3DF:{
				char filename[25];
				sprintf(filename,"3df_%ld.3df",img_count);
				glide->filename = (char*)malloc(strlen(filename)+1);
				AGIDL_FilenameCpy(glide->filename,filename);
				AGIDL_Export3DF(glide);
			}break;
			case AGIDL_IMG_PPM:{
				char filename[25];
				sprintf(filename,"3df_%ld.3df",img_count);
				glide->filename = (char*)malloc(strlen(filename)+1);
				AGIDL_FilenameCpy(glide->filename,filename);
				AGIDL_PPM* ppm = AGIDL_Convert3DF2PPM(glide);
				if(flip == TRUE){
					AGIDL_FlipHorzPPM(ppm);
				}
				AGIDL_ExportPPM(ppm);
				AGIDL_FreePPM(ppm);
			}break;
			case AGIDL_IMG_LBM:{
				char filename[25];
				sprintf(filename,"3df_%ld.3df",img_count);
				glide->filename = (char*)malloc(strlen(filename)+1);
				AGIDL_FilenameCpy(glide->filename,filename);
				AGIDL_ColorConvert3DF(glide,AGIDL_RGB_888);
				AGIDL_LBM* lbm = AGIDL_Convert3DF2LBM(glide);
				AGIDL_LBMSetMaxDiff(lbm,19);
				AGIDL_LBMSetICPEncoding(lbm,ICP_ENCODE_THRESHOLD);
				if(flip == TRUE){
					AGIDL_FlipHorzLBM(lbm);
				}
				AGIDL_ExportLBM(lbm);
				AGIDL_FreeLBM(lbm);
			}break;
		}
		
		AGIDL_Free3DF(glide);
	}
	
	fclose(file);
	
	return NO_IMG_ERROR;
}

void AGIDL_TGASearchFileOnDisk(const char* filename, AGIDL_IMG_TYPE img_type){
	FILE* file = fopen(filename,"rb");
	
	if(file == NULL){
		printf("Could not locate/open file - %s!\n");
		return;
	}	
	
	u32 img_count = 0;
	
	while(!feof(file)){
		AGIDL_TGA* tga = (AGIDL_TGA*)malloc(sizeof(AGIDL_TGA));
		
		AGIDL_TGADecodeHeader(tga,file);
		
		while(!feof(file) && !AGIDL_IsTGA(tga)){
			u32 curr_coord = ftell(file);
			fseek(file,0,SEEK_END);
			if(curr_coord >= ftell(file)){
				fclose(file);
				return;
			}
			else{
				fseek(file,curr_coord,SEEK_SET);
			}
			fseek(file,curr_coord-17,SEEK_SET);
			AGIDL_TGADecodeHeader(tga,file);
		}
		
		printf("width = %d\n",tga->header.width);
		printf("height = %d\n",tga->header.height);
		printf("bits = %d\n",tga->header.bits);
		printf("type = %d\n",tga->header.imgtype);
		printf("icp = %d\n",tga->header.clrmaptype);
		printf("flip = %d\n",tga->header.flip);
		
		TGA_ICP_TYPE icp = AGIDL_TGAGetICPType(tga->header.imgtype);
		TGA_IMG_TYPE tga_img_type = AGIDL_TGAGetIMGType(tga->header.bits);
	
		AGIDL_TGADecodeIMG(tga,file,icp,tga_img_type);
		AGIDL_TGADecodeRLE(tga,file,icp,tga_img_type);
		
		printf("reaches end of decoding...\n");
		
		img_count++;
		
		switch(img_type){
			case AGIDL_IMG_TGA:{
				char file_name[25];
				sprintf(file_name,"tga_%d.tga",img_count);
				tga->filename = file_name;
				AGIDL_ExportTGA(tga);
			}break;
			case AGIDL_IMG_TIM:{
				char file_name[25];
				sprintf(file_name,"tga_%d.tga",img_count);
				tga->filename = file_name;
				AGIDL_TIM* tim = AGIDL_ConvertTGA2TIM(tga);
				AGIDL_ExportTIM(tim);
				AGIDL_FreeTIM(tim);
			}break;
			case AGIDL_IMG_BMP:{
				char file_name[25];
				sprintf(file_name,"tga_%d.tga",img_count);
				tga->filename = file_name;
				AGIDL_BMP* bmp = AGIDL_ConvertTGA2BMP(tga);
				AGIDL_ExportBMP(bmp);
				AGIDL_FreeBMP(bmp);
			}break;
			case AGIDL_IMG_PCX:{
				char file_name[25];
				sprintf(file_name,"tga_%d.tga",img_count);
				tga->filename = file_name;
				AGIDL_PCX* pcx = AGIDL_ConvertTGA2PCX(tga);
				AGIDL_ExportPCX(pcx);
				AGIDL_FreePCX(pcx);
			}break;
			case AGIDL_IMG_LMP:{
				char file_name[25];
				sprintf(file_name,"tga_%d.tga",img_count);
				tga->filename = file_name;
				AGIDL_LMP* lmp = AGIDL_ConvertTGA2LMP(tga);
				AGIDL_LMPSetMaxDiff(lmp,32);
				AGIDL_ExportLMP(lmp);
				AGIDL_FreeLMP(lmp);
			}break;
			case AGIDL_IMG_PVR:{
				char file_name[25];
				sprintf(file_name,"tga_%d.tga",img_count);
				tga->filename = file_name;
				AGIDL_PVR* pvr = AGIDL_ConvertTGA2PVR(tga);
				AGIDL_ExportPVR(pvr);
				AGIDL_FreePVR(pvr);
			}break;
			case AGIDL_IMG_GXT:{
				char file_name[25];
				sprintf(file_name,"tga_%d.tga",img_count);
				tga->filename = file_name;
				AGIDL_GXT* gxt = AGIDL_ConvertTGA2GXT(tga);
				AGIDL_ExportGXT(gxt);
				AGIDL_FreeGXT(gxt);
			}break;
			case AGIDL_IMG_BTI:{
				char file_name[25];
				sprintf(file_name,"tga_%d.tga",img_count);
				tga->filename = file_name;
				AGIDL_BTI* bti = AGIDL_ConvertTGA2BTI(tga);
				AGIDL_ExportBTI(bti);
				AGIDL_FreeBTI(bti);
			}break;
		}
		
		AGIDL_FreeTGA(tga);
	}
	
	fclose(file);
}