#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "agidl_img_types.h"

#include <agidl_math_utils.h>

#include "agidl_imgp_scale.h"
#include "agidl_cc_core.h"

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_types.c
*   Date: 9/16/2023
*   Version: 0.1b
*   Updated: 3/2/2024
*   Author: Ryandracus Chapman
*
********************************************/

void AGIDL_FilenameCpy(char *filedest, const char *filesrc){

	while(*filesrc != '\0'){
		*filedest = *filesrc;
		filedest++;
		filesrc++;
	}
	
	
	filedest[strlen(filesrc)] = '\0';
}

void AGIDL_StrCpy2(char* dest, char *a, char *b){
	int lenA = strlen(a);
	int lenB = strlen(b);
	
	memcpy(dest,a,lenA);
	memcpy(dest+lenA,b,lenB);
	dest[lenA+lenB] = '\0';
}

char* AGIDL_StrCpy(char *a, char *b){
	int lenA = strlen(a);
	int lenB = strlen(b);
	
	char* c = (char*)malloc(lenA+lenB+1);
	
	memcpy(c,a,lenA);
	memcpy(c+lenA,b,lenB);
	c[lenA+lenB] = '\0';
	
	return c;
}

char* AGIDL_GetImgExtension(AGIDL_IMG_TYPE img){
	char* ext = (char*)malloc(sizeof(char)*4);
	switch(img){
		case AGIDL_IMG_BMP:{
			strcpy(ext,".bmp");
		}break;
		case AGIDL_IMG_TGA:{
			strcpy(ext,".tga");
		}break;
		case AGIDL_IMG_TIM:{
			strcpy(ext,".tim");
		}break;
		case AGIDL_IMG_PCX:{
			strcpy(ext,".pcx");
		}break;
		case AGIDL_IMG_LMP:{
			strcpy(ext,".lmp");
		}break;
		case AGIDL_IMG_PVR:{
			strcpy(ext,".pvr");
		}break;
		case AGIDL_IMG_GXT:{
			strcpy(ext,".gxt");
		}break;
		case AGIDL_IMG_BTI:{
			strcpy(ext,".bti");
		}break;
		case AGIDL_IMG_3DF:{
			strcpy(ext,".3df");
		}break;
		case AGIDL_IMG_PPM:{
			strcpy(ext,".ppm");
		}break;
		case AGIDL_IMG_LBM:{
			strcpy(ext,".lbm");
		}break;
	}
	ext[4] = '\0';
	return ext;
}

char* AGIDL_GetImgName(char* filename){
	int i, count = 0, len = strlen(filename);
	for(i = 0; i < len; i++){
		if(filename[i] == '.'){
			count = i;
			break;
		}
	}
	char* name = (char*)malloc((sizeof(char)*(count)+1));
	memcpy(name,filename,count);
	name[count] = '\0';
	return name;
}

void AGIDL_ClrMemcpy(COLOR *dest, COLOR *src, u32 count){
	int i;
	for(i = 0; i < count; i++){
		*dest = *src;
		dest++;
		src++;
	}
}

void AGIDL_ClrMemcpy16(COLOR16 *dest, COLOR16 *src, u32 count){
	int i;
	for(i = 0; i < count; i++){
		*dest = *src;
		dest++;
		src++;
	}
}

void AGIDL_ClrMemset(COLOR *dest, COLOR clr, u32 count){
	int i;
	for(i = 0; i < count; i++){
		*dest++ = clr;
	}
}

void AGIDL_ClrMemset16(COLOR16 *dest, COLOR16 clr, u32 count){
	int i;
	for(i = 0; i < count; i++){
		*dest++ = clr;
	}
}

void AGIDL_ClearColorBuffer(void* data, float r, float g, float b, AGIDL_CLR_FMT fmt, u32 count){
	if(AGIDL_GetBitCount(fmt) == 16){
		COLOR16* clrdata = (COLOR16*)data;
		AGIDL_ClrMemset16(clrdata,AGIDL_Color3f(r,g,b,fmt),count);
	}
	else{
		COLOR* clrdata = (COLOR*)data;
		AGIDL_ClrMemset(clrdata,AGIDL_Color3f(r,g,b,fmt),count);
	}
}

int AGIDL_InsideClipBounds(u32 x, u32 y, u32 width, u32 height){
	if(x >= 0 && y >= 0 && x < width && y < height){
		return 1;
	}
	else return 0;
}

void AGIDL_CopyTile(void* dest, void* src, u32 destw, u32 desth, u32 srcw, u32 srch, AGIDL_CLR_FMT destfmt, AGIDL_CLR_FMT srcfmt, u16 xstart, u16 xend, u16 ystart, u16 yend, u16 dx, u16 dy){
	if((AGIDL_GetBitCount(destfmt) == AGIDL_GetBitCount(srcfmt)) && (AGIDL_GetBitCount(destfmt) == 24 || AGIDL_GetBitCount(destfmt) == 32)){
		COLOR* destclr = (COLOR*)dest;
		COLOR* srcclr = (COLOR*)src;
		
		u32 dxx = xend - xstart;
		u32 dyy = yend - ystart;
		
		u32 maxw = dx + dxx;
		u32 maxh = dy + dyy;
		
		if(AGIDL_InsideClipBounds(dxx,dyy,srcw,srch) && AGIDL_InsideClipBounds(maxw,maxh,destw,desth)){
			int x,y,incrx = 0, incry = 0;
			for(y = ystart; y < yend; y++){
				incrx = 0;
				for(x = xstart; x < xend; x++){
					COLOR clr = AGIDL_GetClr(srcclr,x,y,srcw,srch);
					AGIDL_SetClr(destclr,clr,dx+incrx,dy+incry,destw,desth);
					incrx++;
				}
				incry++;
			}
		}
	}
	else if((AGIDL_GetBitCount(destfmt) == AGIDL_GetBitCount(srcfmt)) && AGIDL_GetBitCount(destfmt) == 16){
		COLOR16* destclr = (COLOR16*)dest;
		COLOR16* srcclr = (COLOR16*)src;
		
		u32 dxx = xend - xstart;
		u32 dyy = yend - ystart;
		
		u32 maxw = dx + dxx;
		u32 maxh = dy + dyy;
		
		if(AGIDL_InsideClipBounds(dxx,dyy,srcw,srch) && AGIDL_InsideClipBounds(maxw,maxh,destw,desth)){
			int x,y,incrx = 0, incry = 0;
			for(y = ystart; y < yend; y++){
				incrx = 0;
				for(x = xstart; x < xend; x++){
					COLOR16 clr = AGIDL_GetClr16(srcclr,x,y,srcw,srch);
					AGIDL_SetClr16(destclr,clr,dx+incrx,dy+incry,destw,desth);
					incrx++;
				}
				incry++;
			}
		}
	}
	else if((AGIDL_GetBitCount(destfmt) != AGIDL_GetBitCount(srcfmt)) && (AGIDL_GetBitCount(destfmt) == 24 || AGIDL_GetBitCount(destfmt) == 32) && AGIDL_GetBitCount(srcfmt) == 16){
		COLOR* destclr = (COLOR*)dest;
		COLOR16* srcclr = (COLOR16*)src;
		
		u32 dxx = xend - xstart;
		u32 dyy = yend - ystart;
		
		u32 maxw = dx + dxx;
		u32 maxh = dy + dyy;
		
		if(AGIDL_InsideClipBounds(dxx,dyy,srcw,srch) && AGIDL_InsideClipBounds(maxw,maxh,destw,desth)){
			int x,y,incrx = 0, incry = 0;
			for(y = ystart; y < yend; y++){
				incrx = 0;
				for(x = xstart; x < xend; x++){
					COLOR16 clr16 = AGIDL_GetClr16(srcclr,x,y,srcw,srch);
					COLOR clr = AGIDL_CLR16_TO_CLR(clr16,srcfmt,destfmt);
					AGIDL_SetClr(destclr,clr,dx+incrx,dy+incry,destw,desth);
					incrx++;
				}
				incry++;
			}
		}
	}
	if((AGIDL_GetBitCount(destfmt) != AGIDL_GetBitCount(srcfmt)) && (AGIDL_GetBitCount(srcfmt) == 24 || AGIDL_GetBitCount(srcfmt) == 32) && AGIDL_GetBitCount(destfmt) == 16){
		COLOR16* destclr = (COLOR16*)dest;
		COLOR* srcclr = (COLOR*)src;
		
		u32 dxx = xend - xstart;
		u32 dyy = yend - ystart;
		
		u32 maxw = dx + dxx;
		u32 maxh = dy + dyy;
		
		if(AGIDL_InsideClipBounds(dxx,dyy,srcw,srch) && AGIDL_InsideClipBounds(maxw,maxh,destw,desth)){
			int x,y,incrx = 0, incry = 0;
			for(y = ystart; y < yend; y++){
				incrx = 0;
				for(x = xstart; x < xend; x++){
					COLOR clr24 = AGIDL_GetClr(srcclr,x,y,srcw,srch);
					COLOR16 clr = AGIDL_CLR_TO_CLR16(clr24,srcfmt,destfmt);
					AGIDL_SetClr16(destclr,clr,dx+incrx,dy+incry,destw,desth);
					incrx++;
				}
				incry++;
			}
		}
	}
}

void AGIDL_CopyScanline(void* dest, void* src, u32 destw, u32 desth, u32 srcw, u32 srch, AGIDL_CLR_FMT destfmt, AGIDL_CLR_FMT srcfmt, u16 destscanline, u16 srcscanline){
	if((AGIDL_GetBitCount(destfmt) == AGIDL_GetBitCount(srcfmt)) && (AGIDL_GetBitCount(srcfmt) == 24 || AGIDL_GetBitCount(srcfmt) == 32)){
		if(AGIDL_InsideClipBounds(0,srcscanline,srcw,srch) && AGIDL_InsideClipBounds(0,destscanline,destw,desth)){
			COLOR* destclr = (COLOR*)dest;
			COLOR* srcclr = (COLOR*)src;
			
			if(destw > srcw){
				COLOR* cpyrow = (COLOR*)malloc(sizeof(COLOR)*srcw);
				COLOR* cpydata = (COLOR*)malloc(sizeof(COLOR)*srcw*srch);
				AGIDL_ClrMemcpy(cpydata,srcclr,srcw*srch);
				
				u16 width = srcw;
				u16 height = srch;

				cpydata = (COLOR*)AGIDL_ScaleImgDataNearest(cpydata,&width,&height,destw/(float)srcw,1,srcfmt);
				
				int x;
				for(x = 0; x < destw; x++){
					COLOR clr = AGIDL_GetClr(cpydata,x,srcscanline,width,height);
					AGIDL_SetClr(cpyrow,clr,x,0,destw,1);
				}
				
				for(x = 0; x < destw; x++){
					COLOR clr = AGIDL_GetClr(cpyrow,x,0,srcw,srch);
					AGIDL_SetClr(destclr,clr,x,destscanline,destw,desth);
				}
				
				free(cpydata);
				free(cpyrow);
			}
			else{
				int x;
				for(x = 0; x < destw; x++){
					COLOR clr = AGIDL_GetClr(srcclr,x,srcscanline,srcw,srch);
					AGIDL_SetClr(destclr,clr,x,destscanline,destw,desth);
				}
			}
		}
		else if((AGIDL_GetBitCount(destfmt) == AGIDL_GetBitCount(srcfmt)) && AGIDL_GetBitCount(srcfmt) == 16){
			COLOR16* destclr = (COLOR16*)dest;
			COLOR16* srcclr = (COLOR16*)src;
			
			if(AGIDL_InsideClipBounds(0,srcscanline,srcw,srch) && AGIDL_InsideClipBounds(0,destscanline,destw,desth)){
				if(destw > srcw){
					COLOR16* cpyrow = (COLOR16*)malloc(sizeof(COLOR16)*srcw);
					COLOR16* cpydata = (COLOR16*)malloc(sizeof(COLOR16)*srcw*srch);
					AGIDL_ClrMemcpy16(cpydata,srcclr,srcw*srch);
					
					u16 width = srcw;
					u16 height = srch;

					cpydata = (COLOR16*)AGIDL_ScaleImgDataNearest(cpydata,&width,&height,destw/(float)srcw,1,srcfmt);
					
					int x;
					for(x = 0; x < destw; x++){
						COLOR16 clr = AGIDL_GetClr16(cpydata,x,srcscanline,width,height);
						AGIDL_SetClr16(cpyrow,clr,x,0,destw,1);
					}
					
					for(x = 0; x < destw; x++){
						COLOR16 clr = AGIDL_GetClr16(cpyrow,x,0,srcw,srch);
						AGIDL_SetClr16(destclr,clr,x,destscanline,destw,desth);
					}
					
					free(cpydata);
					free(cpyrow);
				}
				else{
					int x;
					for(x = 0; x < destw; x++){
						COLOR16 clr = AGIDL_GetClr16(srcclr,x,srcscanline,srcw,srch);
						AGIDL_SetClr16(destclr,clr,x,destscanline,destw,desth);
					}
				}
			}
		}
	}
	else if((AGIDL_GetBitCount(destfmt) == 24 || AGIDL_GetBitCount(destfmt) == 32) && AGIDL_GetBitCount(srcfmt) == 16){
		if(AGIDL_InsideClipBounds(0,srcscanline,srcw,srch) && AGIDL_InsideClipBounds(0,destscanline,destw,desth)){
			COLOR* destclr = (COLOR*)dest;
			COLOR16* srcclr16 = (COLOR16*)src;
			
			COLOR* srcclr = (COLOR*)malloc(sizeof(COLOR)*srcw*srch);
			
			AGIDL_CLR_FMT fmt = srcfmt;
			AGIDL_16BPPTO24BPP(srcclr16,srcclr,srcw,srch,&fmt);
			
			if(destw > srcw){
				COLOR* cpyrow = (COLOR*)malloc(sizeof(COLOR)*srcw);
				COLOR* cpydata = (COLOR*)malloc(sizeof(COLOR)*srcw*srch);
				AGIDL_ClrMemcpy(cpydata,srcclr,srcw*srch);
				
				u16 width = srcw;
				u16 height = srch;

				cpydata = (COLOR*)AGIDL_ScaleImgDataNearest(cpydata,&width,&height,destw/(float)srcw,1,fmt);
				
				int x;
				for(x = 0; x < destw; x++){
					COLOR clr = AGIDL_GetClr(cpydata,x,srcscanline,width,height);
					AGIDL_SetClr(cpyrow,clr,x,0,destw,1);
				}
				
				for(x = 0; x < destw; x++){
					COLOR clr = AGIDL_GetClr(cpyrow,x,0,srcw,srch);
					AGIDL_SetClr(destclr,clr,x,destscanline,destw,desth);
				}
				
				free(cpydata);
				free(cpyrow);
			}
			else{
				int x;
				for(x = 0; x < destw; x++){
					COLOR clr = AGIDL_GetClr(srcclr,x,srcscanline,srcw,srch);
					AGIDL_SetClr(destclr,clr,x,destscanline,destw,desth);
				}
			}
			
			free(srcclr);
		}
	}
	else if(AGIDL_GetBitCount(destfmt) == 16 && (AGIDL_GetBitCount(srcfmt) == 24 || AGIDL_GetBitCount(srcfmt) == 32)){
		if(AGIDL_InsideClipBounds(0,srcscanline,srcw,srch) && AGIDL_InsideClipBounds(0,destscanline,destw,desth)){
			COLOR16* destclr = (COLOR16*)dest;
			COLOR* srcclr24 = (COLOR*)src;
			
			COLOR16* srcclr = (COLOR16*)malloc(sizeof(COLOR16)*srcw*srch);
			
			AGIDL_CLR_FMT fmt = srcfmt;
			AGIDL_24BPPTO16BPP(srcclr24,srcclr,srcw,srch,&fmt);
			
			if(destw > srcw){
				COLOR16* cpyrow = (COLOR16*)malloc(sizeof(COLOR16)*srcw);
				COLOR16* cpydata = (COLOR16*)malloc(sizeof(COLOR16)*srcw*srch);
				AGIDL_ClrMemcpy16(cpydata,srcclr,srcw*srch);
				
				u16 width = srcw;
				u16 height = srch;

				cpydata = (COLOR16*)AGIDL_ScaleImgDataNearest(cpydata,&width,&height,destw/(float)srcw,1,fmt);
				
				int x;
				for(x = 0; x < destw; x++){
					COLOR16 clr = AGIDL_GetClr16(cpydata,x,srcscanline,width,height);
					AGIDL_SetClr16(cpyrow,clr,x,0,destw,1);
				}
				
				for(x = 0; x < destw; x++){
					COLOR16 clr = AGIDL_GetClr16(cpyrow,x,0,srcw,srch);
					AGIDL_SetClr16(destclr,clr,x,destscanline,destw,desth);
				}
				
				free(cpydata);
				free(cpyrow);
			}
			else{
				int x;
				for(x = 0; x < destw; x++){
					COLOR clr16 = AGIDL_GetClr16(srcclr,x,srcscanline,srcw,srch);
					AGIDL_SetClr16(destclr,clr16,x,destscanline,destw,desth);
				}
			}
			
			free(srcclr);
		}
	}
}

u8 AGIDL_GetBitCount(AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_RGB_888:{
			return 24;
		}break;
		case AGIDL_BGR_888:{
			return 24;
		}break;
		case AGIDL_RGB_555:{
			return 16;
		}break;
		case AGIDL_BGR_555:{
			return 16;
		}break;
		case AGIDL_RGB_565:{
			return 16;
		}break;
		case AGIDL_BGR_565:{
			return 16;
		}break;
		case AGIDL_RGBA_8888:{
			return 32;
		}break;
		case AGIDL_ARGB_8888:{
			return 32;
		}break;
	    default: return 24; break;
	}
}

void AGIDL_ExtractAndPrintBGR(FILE* file, COLOR clr, AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_BGR_888:{
			u8 r = AGIDL_GetR(clr,fmt);
			u8 g = AGIDL_GetG(clr,fmt);
			u8 b = AGIDL_GetB(clr,fmt);
			fwrite(&b,1,1,file);
			fwrite(&g,1,1,file);
			fwrite(&r,1,1,file);
		}break;
		case AGIDL_BGR_555:{
			fwrite(&clr,2,1,file);
		}break;
		case AGIDL_BGR_565:{
			fwrite(&clr,2,1,file);
		}break;
	}
}

void AGIDL_ExtractAndPrintRGB(FILE* file, COLOR clr, AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_RGB_888:{
			u8 r = AGIDL_GetR(clr,fmt);
			u8 g = AGIDL_GetG(clr,fmt);
			u8 b = AGIDL_GetB(clr,fmt);
			fwrite(&r,1,1,file);
			fwrite(&g,1,1,file);
			fwrite(&b,1,1,file);
		}break;
		case AGIDL_RGB_555:{
			fwrite(&clr,2,1,file);
		}break;
		case AGIDL_RGB_565:{
			fwrite(&clr,2,1,file);
		}break;
	}
}

void AGIDL_ExtractAndPrintRGBA(FILE* file, COLOR clr, AGIDL_CLR_FMT fmt){
	u8 r = AGIDL_GetR(clr,fmt);
	u8 g = AGIDL_GetG(clr,fmt);
	u8 b = AGIDL_GetB(clr,fmt);
	u8 a = AGIDL_GetA(clr,fmt);
	
	switch(fmt){
		case AGIDL_RGBA_8888:{
			fwrite(&b,1,1,file);
			fwrite(&g,1,1,file);
			fwrite(&r,1,1,file);
			fwrite(&a,1,1,file);
		}break;
		case AGIDL_ARGB_8888:{
			fwrite(&a,1,1,file);
			fwrite(&r,1,1,file);
			fwrite(&g,1,1,file);
			fwrite(&b,1,1,file);
		}break;
	}
}

u32 count = 0;

void AGIDL_ExportRawColors(void* data, u32 width, u32 height, AGIDL_CLR_FMT fmt, AGIDL_FILE_TYPE ftype, AGIDL_ARR_TYPE arrtype, u8 rgb){
	if(AGIDL_GetBitCount(fmt) == 24){
		COLOR* clrs = (COLOR*)data;
		if((ftype & F_HEADER) && (arrtype & ARR)){
			char filename[50];
			sprintf(filename,"imgdata_%d.h",count);
			
			FILE* file = fopen(filename,"w");
			
			fprintf(file,"#ifndef IMG_DATA_%d_H\n",count);
			fprintf(file,"#define IMG_DATA_%d_H\n",count);
			fputs("\n",file);
			fprintf(file,"int width = %d;\n",width);
			fprintf(file,"int height = %d;\n",height);
			fputs("\n",file);
			
			if(rgb != 2){
				fprintf(file,"int img[%d] = {\n",width*height*3);
			}
			else{
				fprintf(file,"int img[%d] = {\n",width*height);
			}
			
			int x,y;
			for(y = 0; y < height; y++){
				for(x = 0; x < width; x++){
					COLOR clr = AGIDL_GetClr(clrs,x,y,width,height);
					u8 r = AGIDL_GetR(clr,fmt);
					u8 g = AGIDL_GetG(clr,fmt);
					u8 b = AGIDL_GetB(clr,fmt);
					if(x > 0 && (x % 15) == 0){
						fputs("\n",file);
					}
					
					if(rgb == 1){
						fprintf(file,"%d,%d,%d,",r,g,b);
					}
					else if(rgb == 2){
						fprintf(file,"0x%x,",clr);
					}
					else{
						fprintf(file,"%d,%d,%d,",b,g,r);
					}
				}

				if(y != height-1){
					fputs("\n",file);
				}
			}
			
			fputs("\n};",file);
			fputs("\n\n",file);
			fputs("#endif",file);
			
			fclose(file);
			
			count++;
		}
		if((ftype & F_SOURCE) && (arrtype & ARR)){
			char filename[50];
			sprintf(filename,"imgdata_%d.c",count);
			
			FILE* file = fopen(filename,"w");

			fprintf(file,"int width = %d;\n",width);
			fprintf(file,"int height = %d;\n",height);
			fputs("\n",file);
			
			if(rgb != 2){
				fprintf(file,"int img[%d] = {\n",width*height*3);
			}
			else{
				fprintf(file,"int img[%d] = {\n",width*height);
			}

			
			int x,y;
			for(y = 0; y < height; y++){
				for(x = 0; x < width; x++){
					COLOR clr = AGIDL_GetClr(clrs,x,y,width,height);
					u8 r = AGIDL_GetR(clr,fmt);
					u8 g = AGIDL_GetG(clr,fmt);
					u8 b = AGIDL_GetB(clr,fmt);
					if(x > 0 && (x % 15) == 0){
						fputs("\n",file);
					}
					
					if(rgb == 1){
						fprintf(file,"%d,%d,%d,",r,g,b);
					}
					else if(rgb == 2){
						fprintf(file,"0x%x,",clr);
					}
					else{
						fprintf(file,"%d,%d,%d,",b,g,r);
					}
				}

				if(y != height-1){
					fputs("\n",file);
				}
			}
			
			fputs("\n};",file);
			
			fclose(file);
			
			count++;
		}
		
		if((ftype & F_BIN) && (arrtype & ARR)){
			char filename[50];
			sprintf(filename,"imgdata_%d.bin",count);
			FILE* file = fopen(filename,"wb");
			
			int x,y;
			for(y = 0; y < height; y++){
				for(x = 0; x < width; x++){
					COLOR clr = AGIDL_GetClr(clrs,x,y,width,height);
					
					if(rgb == 1){
						AGIDL_ExtractAndPrintRGB(file,clr,fmt);
					}
					else{
						AGIDL_ExtractAndPrintBGR(file,clr,fmt);
					}
				}
			}
			
			fclose(file);
			
			count++;
		}
	}
	else if(AGIDL_GetBitCount(fmt) == 16){
		COLOR16* clrs = (COLOR16*)data;
		if((ftype & F_HEADER) && (arrtype & ARR)){
			char filename[50];
			sprintf(filename,"imgdata_%d.h",count);
			
			FILE* file = fopen(filename,"w");
			
			fprintf(file,"#ifndef IMG_DATA_%d_H\n",count);
			fprintf(file,"#define IMG_DATA_%d_H\n",count);
			fputs("\n",file);
			fprintf(file,"int width = %d;\n",width);
			fprintf(file,"int height = %d;\n",height);
			fputs("\n",file);
			
			if(rgb != 2){
				fprintf(file,"unsigned char img[%d] = {\n",width*height*3);
			}
			else{
				fprintf(file,"unsigned short img[%d] = {\n",width*height);
			}

			
			int x,y;
			for(y = 0; y < height; y++){
				for(x = 0; x < width; x++){
					COLOR16 clr = AGIDL_GetClr16(clrs,x,y,width,height);
					u8 r = AGIDL_GetR(clr,fmt);
					u8 g = AGIDL_GetG(clr,fmt);
					u8 b = AGIDL_GetB(clr,fmt);
					if(x > 0 && (x % 15) == 0){
						fputs("\n",file);
					}
					
					if(rgb == 1){
						fprintf(file,"%d,%d,%d,",r,g,b);
					}
					else if(rgb == 2){
						fprintf(file,"0x%x,",clr);
					}
					else{
						fprintf(file,"%d,%d,%d,",b,g,r);
					}
				}

				if(y != height-1){
					fputs("\n",file);
				}
			}
			
			fputs("\n};",file);
			fputs("\n\n",file);
			fputs("#endif",file);
			
			fclose(file);
			
			count++;
		}
		if((ftype & F_SOURCE) && (arrtype & ARR)){
			char filename[50];
			sprintf(filename,"imgdata_%d.c",count);
			
			FILE* file = fopen(filename,"w");

			fprintf(file,"int width = %d;\n",width);
			fprintf(file,"int height = %d;\n",height);
			fputs("\n",file);
			
			if(rgb != 2){
				fprintf(file,"unsigned char img[%d] = {\n",width*height*3);
			}
			else{
				fprintf(file,"unsigned short img[%d] = {\n",width*height);
			}

			
			int x,y;
			for(y = 0; y < height; y++){
				for(x = 0; x < width; x++){
					COLOR16 clr = AGIDL_GetClr16(clrs,x,y,width,height);
					u8 r = AGIDL_GetR(clr,fmt);
					u8 g = AGIDL_GetG(clr,fmt);
					u8 b = AGIDL_GetB(clr,fmt);
					if(x > 0 && (x % 15) == 0){
						fputs("\n",file);
					}
					
					if(rgb == 1){
						fprintf(file,"%d,%d,%d,",r,g,b);
					}
					else if(rgb == 2){
						fprintf(file,"0x%x,",clr);
					}
					else{
						fprintf(file,"%d,%d,%d,",b,g,r);
					}
				}

				if(y != height-1){
					fputs("\n",file);
				}
			}
			
			fputs("\n};",file);
			
			fclose(file);
			
			count++;
		}
		
		if((ftype & F_BIN) && (arrtype & ARR)){
			char filename[50];
			sprintf(filename,"imgdata_%d.bin",count);
			FILE* file = fopen(filename,"wb");
			
			int x,y;
			for(y = 0; y < height; y++){
				for(x = 0; x < width; x++){
					COLOR16 clr = AGIDL_GetClr16(clrs,x,y,width,height);
					
					if(rgb == 1){
						AGIDL_ExtractAndPrintRGB(file,clr,fmt);
					}
					else{
						AGIDL_ExtractAndPrintBGR(file,clr,fmt);
					}
				}
			}
			
			fclose(file);
			
			count++;
		}
	}
}

u8 * AGIDL_GenerateImgDataFromICP(void* data, u32 width, u32 height, AGIDL_ICP icp, int max_diff){
	max_diff = AGIDL_Clamp(0,max_diff,255);
	
	if(AGIDL_GetBitCount(icp.fmt) != 16){
		COLOR* clrdata = (COLOR*)data;
		u8* buffer = (u8*)malloc(sizeof(u8)*(width*height));
		
		u32 x,y;
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){
				COLOR clr = AGIDL_GetClr(clrdata,x,y,width,height);
				u8 clricp = AGIDL_FindClosestColor(icp,clr,icp.fmt,max_diff);
				buffer[x+y*width] = clricp;
			}
		}
		
		return buffer;
	}
	else{
		COLOR16* clrdata = (COLOR16*)data;
		u8* buffer = (u8*)malloc(sizeof(u8)*(width*height));
		
		u32 x,y;
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){
				COLOR16 clr = AGIDL_GetClr16(clrdata,x,y,width,height);
				u8 clricp = AGIDL_FindClosestColor(icp,clr,icp.fmt,max_diff);
				buffer[x+y*width] = clricp;
			}
		}
		
		return buffer;
	}
}

COLOR AGIDL_GetClr(COLOR* clrs, int x, int y, int width, int height){
	if(x >= 0 && y >= 0 && x < width && y < height){
		return clrs[x+y*width];
	}
}

COLOR16 AGIDL_GetClr16(COLOR16* clrs, int x, int y, int width, int height){
	if(x >= 0 && y >= 0 && x < width && y < height){
		return clrs[x+y*width];
	}
}

void AGIDL_SetClr(COLOR* clrs, COLOR clr, int x, int y, int width, int height){
	if(x >= 0 && y >= 0 && x < width && y < height){
		clrs[x+y*width] = clr;
	}
}

void AGIDL_SetClr16(COLOR16* clrs, COLOR16 clr, int x, int y, int width, int height){
	if(x >= 0 && y >= 0 && x < width && y < height){
		clrs[x+y*width] = clr;
	}
}

void AGIDL_RGB2BGR(COLOR* clrs, int width, int height, AGIDL_CLR_FMT *fmt){
	int x,y;
	for(y = 0; y < height; y++){
		for(x = 0; x < width; x++){
			COLOR clr = AGIDL_GetClr(clrs,x,y,width,height);
			clr = AGIDL_RGB_TO_BGR(clr,AGIDL_RGB_888);
			AGIDL_SetClr(clrs,clr,x,y,width,height);
		}
	}
	*fmt = AGIDL_BGR_888;
}

void AGIDL_BGR2RGB(COLOR* clrs, int width, int height, AGIDL_CLR_FMT *fmt){
	int x,y;
	for(y = 0; y < height; y++){
		for(x = 0; x < width; x++){
			COLOR clr = AGIDL_GetClr(clrs,x,y,width,height);
			clr = AGIDL_BGR_TO_RGB(clr,AGIDL_BGR_888);
			AGIDL_SetClr(clrs,clr,x,y,width,height);
		}
	}
	*fmt = AGIDL_RGB_888;
}

void AGIDL_RGB2BGR16(COLOR16* clrs, int width, int height, AGIDL_CLR_FMT *fmt){
	if(*fmt == AGIDL_RGB_555){
		int x,y;
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){
				COLOR16 clr = AGIDL_GetClr16(clrs,x,y,width,height);
				clr = AGIDL_RGB_TO_BGR(clr,AGIDL_RGB_555);
				AGIDL_SetClr16(clrs,clr,x,y,width,height);
			}
		}
		*fmt = AGIDL_BGR_555;
	}
	else{
		int x,y;
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){
				COLOR16 clr = AGIDL_GetClr16(clrs,x,y,width,height);
				clr = AGIDL_RGB_TO_BGR(clr,AGIDL_RGB_565);
				AGIDL_SetClr16(clrs,clr,x,y,width,height);
			}
		}
		*fmt = AGIDL_BGR_565;
	}
}

void AGIDL_BGR2RGB16(COLOR16* clrs, int width, int height, AGIDL_CLR_FMT *fmt){
	if(*fmt == AGIDL_BGR_555){
		int x,y;
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){
				COLOR16 clr = AGIDL_GetClr16(clrs,x,y,width,height);
				clr = AGIDL_BGR_TO_RGB(clr,AGIDL_BGR_555);
				AGIDL_SetClr16(clrs,clr,x,y,width,height);
			}
		}
		*fmt = AGIDL_RGB_555;
	}
	else{
		int x,y;
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){
				COLOR16 clr = AGIDL_GetClr16(clrs,x,y,width,height);
				clr = AGIDL_BGR_TO_RGB(clr,AGIDL_BGR_565);
				AGIDL_SetClr16(clrs,clr,x,y,width,height);
			}
		}
		*fmt = AGIDL_RGB_565;
	}
}

void AGIDL_24BPPTO16BPP(COLOR* src, COLOR16* dest, int width, int height, AGIDL_CLR_FMT *fmt){
	if(*fmt == AGIDL_RGB_888){
		int x,y;
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){
				COLOR clr = AGIDL_GetClr(src,x,y,width,height);
				COLOR16 clr16 = AGIDL_CLR_TO_CLR16(clr, AGIDL_RGB_888, AGIDL_RGB_555);
				AGIDL_SetClr16(dest,clr16,x,y,width,height);
			}
		}
		*fmt = AGIDL_RGB_555;
	}
	if(*fmt == AGIDL_BGR_888){
		int x,y;
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){
				COLOR clr = AGIDL_GetClr(src,x,y,width,height);
				COLOR16 clr16 = AGIDL_CLR_TO_CLR16(clr, AGIDL_BGR_888, AGIDL_BGR_555);
				AGIDL_SetClr16(dest,clr16,x,y,width,height);
			}
		}
		*fmt = AGIDL_BGR_555;
	}
}

void AGIDL_16BPPTO24BPP(COLOR16 *src, COLOR* dest, int width, int height, AGIDL_CLR_FMT* fmt){
	if(*fmt == AGIDL_RGB_555){
		int x,y;
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){
				COLOR16 clr16 = AGIDL_GetClr16(src,x,y,width,height);
				COLOR clr = AGIDL_CLR16_TO_CLR(clr16, AGIDL_RGB_555, AGIDL_RGB_888);
				AGIDL_SetClr(dest,clr,x,y,width,height);
			}
		}
		*fmt = AGIDL_RGB_888;
	}
	if(*fmt == AGIDL_BGR_555){
		int x,y;
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){
				COLOR16 clr16 = AGIDL_GetClr16(src,x,y,width,height);
				COLOR clr = AGIDL_CLR16_TO_CLR(clr16, AGIDL_BGR_555, AGIDL_BGR_888);
				AGIDL_SetClr(dest,clr,x,y,width,height);
			}
		}
		*fmt = AGIDL_BGR_888;
	}
}

void AGIDL_555TO565(COLOR16* src, int width, int height, AGIDL_CLR_FMT *fmt){
	if(*fmt == AGIDL_RGB_555){
		int x,y;
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){
				COLOR16 clr = AGIDL_GetClr16(src,x,y,width,height);
				clr = AGIDL_555_TO_565(clr,AGIDL_RGB_555);
				AGIDL_SetClr16(src,clr,x,y,width,height);
			}
		}
		*fmt = AGIDL_RGB_565;
	}
	else{
		int x,y;
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){
				COLOR16 clr = AGIDL_GetClr16(src,x,y,width,height);
				clr = AGIDL_555_TO_565(clr,AGIDL_BGR_555);
				AGIDL_SetClr16(src,clr,x,y,width,height);
			}
		}
		*fmt = AGIDL_BGR_565;
	}
}

void AGIDL_565TO555(COLOR16* src, int width, int height, AGIDL_CLR_FMT *fmt){
	if(*fmt == AGIDL_RGB_565){
		int x,y;
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){
				COLOR16 clr = AGIDL_GetClr16(src,x,y,width,height);
				clr = AGIDL_565_TO_555(clr,AGIDL_RGB_565);
				AGIDL_SetClr16(src,clr,x,y,width,height);
			}
		}
		*fmt = AGIDL_RGB_555;
	}
	else{
		int x,y;
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){
				COLOR16 clr = AGIDL_GetClr16(src,x,y,width,height);
				clr = AGIDL_565_TO_555(clr,AGIDL_BGR_565);
				AGIDL_SetClr16(src,clr,x,y,width,height);
			}
		}
		*fmt = AGIDL_BGR_555;
	}
}

u8* AGIDL_GenerateRGBBuffer(void* data, int width, int height, AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) != 16){
		COLOR* clr_data = (COLOR*)data;
		u8* rgbbuf = (u8*)malloc(sizeof(u8)*(width*height*3));
		int i,j;
		for(i = 0; i < width * height; i++){
			COLOR clr = clr_data[i];
			u8 r = AGIDL_GetR(clr,fmt);
			u8 g = AGIDL_GetG(clr,fmt);
			u8 b = AGIDL_GetB(clr,fmt);
			for(j = i * 3; j < (i * 3) + 1; j++){
				rgbbuf[j] = r; rgbbuf[j+1] = g; rgbbuf[j+2] = b;
			}
		}
		return rgbbuf;
	}
	else{
		COLOR16* clr_data = (COLOR16*)data;
		u8* rgbbuf = (u8*)malloc(sizeof(u8)*(width*height*3));
		int i,j;
		for(i = 0; i < width * height; i++){
			COLOR16 clr = clr_data[i];
			u8 r = AGIDL_GetR(clr,fmt);
			u8 g = AGIDL_GetG(clr,fmt);
			u8 b = AGIDL_GetB(clr,fmt);
			for(j = i * 3; j < (i * 3) + 1; j++){
				rgbbuf[j] = r; rgbbuf[j+1] = g; rgbbuf[j+2] = b;
			}
		}
		return rgbbuf;
	}
}

u8* AGIDL_GenerateBGRBuffer(COLOR* clrs, int width, int height, AGIDL_CLR_FMT fmt){
	u8* rgbbuf = (u8*)malloc(sizeof(u8)*(width*height)*(3));
	int i,j;
	for(i = 0; i < width * height; i++){
		COLOR clr = clrs[i];
		u8 r = AGIDL_GetR(clr,fmt);
		u8 g = AGIDL_GetG(clr,fmt);
		u8 b = AGIDL_GetB(clr,fmt);
		for(j = i * 3; j < (i * 3) + 1; j++){
			rgbbuf[j] = b; rgbbuf[j+1] = g; rgbbuf[j+2] = r;
		}
	}
	return rgbbuf;
}

u8* AGIDL_GenerateRGBABuffer(COLOR* clrs, int width, int height, AGIDL_CLR_FMT fmt){
	u8* rgbbuf = (u8*)malloc(sizeof(u8)*(width*height*4));
	int i,j;
	for(i = 0; i < width * height; i++){
		COLOR clr = clrs[i];
		u8 r = AGIDL_GetR(clr,fmt);
		u8 g = AGIDL_GetG(clr,fmt);
		u8 b = AGIDL_GetB(clr,fmt);
		u8 a = AGIDL_GetA(clr,fmt);
		for(j = i * 4; j < (i * 4) + 1; j++){
			rgbbuf[j] = r; rgbbuf[j+1] = g; rgbbuf[j+2] = b; rgbbuf[j+3] = a;
		}
	}
	return rgbbuf;
}

COLOR* AGIDL_RGBSyncColor(u8* rgbbuf, int width, int height, AGIDL_CLR_FMT fmt){
	COLOR* clr = (COLOR*)malloc(sizeof(COLOR)*(width*height));
	int i, count;
	for(i = 0, count = 0; i < width*height*3; i+=3, count++){
		switch(fmt){
			case AGIDL_RGB_888:{
				clr[count] = AGIDL_RGB(rgbbuf[i],rgbbuf[i+1],rgbbuf[i+2],fmt);
			}break;
			case AGIDL_BGR_888:{
				clr[count] = AGIDL_RGB(rgbbuf[i+2],rgbbuf[i+1],rgbbuf[i],fmt);
			}break;
			case AGIDL_RGB_555:{
				clr[count] = AGIDL_RGB16(rgbbuf[i],rgbbuf[i+1],rgbbuf[i+2],fmt);
			}break;
			case AGIDL_BGR_555:{
				clr[count] = AGIDL_RGB16(rgbbuf[i+2],rgbbuf[i+1],rgbbuf[i],fmt);
			}break;
			case AGIDL_RGB_565:{
				clr[count] = AGIDL_RGB16(rgbbuf[i],rgbbuf[i+1],rgbbuf[i+2],fmt);
			}break;
			case AGIDL_BGR_565:{
				clr[count] = AGIDL_RGB16(rgbbuf[i+2],rgbbuf[i+1],rgbbuf[i],fmt);
			}break;
		}
	}
	return clr;
}

COLOR* AGIDL_RGBASyncClrs(u8* rgbbuf, int width, int height, AGIDL_CLR_FMT fmt){
	COLOR* clr = (COLOR*)malloc(sizeof(COLOR)*(width*height));
	int i, count;
	for(i = 0, count = 0; i < width*height*4; i+=4, count++){
		clr[count] = AGIDL_RGBA(rgbbuf[i],rgbbuf[i+1],rgbbuf[i+2],rgbbuf[i+3],fmt);
	}
	return clr;
}

void AGIDL_FreeClrs(COLOR* clrs){
	free(clrs);
}

void AGIDL_FreeClrs16(COLOR16* clrs){
	free(clrs);
}

void AGIDL_FreeRGBBuffer(u8* rgbbuf){
	free(rgbbuf);
}

void AGIDL_PrintRGBBuffer(u8* rgbbuf, int width, int height, AGIDL_CLR_FMT fmt){
	if(fmt == AGIDL_RGB_888 || fmt == AGIDL_RGB_555 || fmt == AGIDL_RGBA_8888){
		int i;
		for(i = 0; i < width*height*3; i+=3){
			printf("RGB(%d,%d,%d)\n",rgbbuf[i],rgbbuf[i+1],rgbbuf[i+2]);
		}
	}
	else{
		int i;
		for(i = 0; i < width*height*3; i+=3){
			printf("BGR(%d,%d,%d)\n",rgbbuf[i],rgbbuf[i+1],rgbbuf[i+2]);
		}
	}
}

void AGIDL_PrintRGBABuffer(u8* rgbbuf, int width, int height, AGIDL_CLR_FMT fmt){
	if(fmt == AGIDL_RGBA_8888 || fmt == AGIDL_ARGB_8888){
		int i;
		for(i = 0; i < width*height*4; i+=4){
			printf("RGBA(%d,%d,%d,%d)\n",rgbbuf[i],rgbbuf[i+1],rgbbuf[i+2],rgbbuf[i+3]);
		}
	}
}