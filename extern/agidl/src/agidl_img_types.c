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

#include <agidl_img_types.h>

#include <stdlib.h>
#include <string.h>

#include <agidl_cc_core.h>
#include <agidl_imgp_scale.h>
#include <agidl_math_utils.h>

void AGIDL_FilenameCpy(char *filedest, const char *filesrc){

	while(*filesrc != '\0'){
		*filedest = *filesrc;
		filedest++;
		filesrc++;
	}
	
	
	filedest[strlen(filesrc)] = '\0';
}

void AGIDL_StrCpy2(char* dest, const char *a, const char *b){
	const int lenA = strlen(a);
	const int lenB = strlen(b);

	memcpy(dest,a,lenA);
	memcpy(dest+lenA,b,lenB);
	dest[lenA+lenB] = '\0';
}

char* AGIDL_StrCpy(const char *a, const char *b){
	const int lenA = strlen(a);
	const int lenB = strlen(b);

	char* c = malloc(lenA+lenB+1);

	memcpy(c,a,lenA);
	memcpy(c+lenA,b,lenB);
	c[lenA+lenB] = '\0';

	return c;
}

char* AGIDL_GetImgExtension(const AGIDL_IMG_TYPE img){
	char* ext = malloc(sizeof(char)*5);
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

char* AGIDL_GetImgName(const char* filename){
	int count = 0;
	const int len = strlen(filename);
	for(int i = 0; i < len; i++){
		if(filename[i] == '.'){
			count = i;
			break;
		}
	}
	char* name = malloc((sizeof(char)*(count)+1));
	memcpy(name,filename,count);
	name[count] = '\0';
	return name;
}

void AGIDL_ClrMemcpy(COLOR *dest, const COLOR *src, const u32 count){
	for(int i = 0; i < count; i++){
		*dest = *src;
		dest++;
		src++;
	}
}

void AGIDL_ClrMemcpy16(COLOR16 *dest, const COLOR16 *src, const u32 count){
	for(int i = 0; i < count; i++){
		*dest = *src;
		dest++;
		src++;
	}
}

void AGIDL_ClrMemset(COLOR *dest, const COLOR clr, const u32 count){
	for(int i = 0; i < count; i++){
		*dest++ = clr;
	}
}

void AGIDL_ClrMemset16(COLOR16 *dest, const COLOR16 clr, const u32 count){
	for(int i = 0; i < count; i++){
		*dest++ = clr;
	}
}

void AGIDL_ClearColorBuffer(void* data, const float r, const float g, const float b, const AGIDL_CLR_FMT fmt, const u32 count){
	if(AGIDL_GetBitCount(fmt) == 16){
		COLOR16* clrdata = data;
		AGIDL_ClrMemset16(clrdata,AGIDL_Color3f(r,g,b,fmt),count);
	}
	else{
		COLOR* clrdata = data;
		AGIDL_ClrMemset(clrdata,AGIDL_Color3f(r,g,b,fmt),count);
	}
}

int AGIDL_InsideClipBounds(const u32 x, const u32 y, const u32 width, const u32 height){
	if(x < width && y < height){
		return 1;
	}
	return 0;
}

void AGIDL_CopyTile(void* dest, const void* src, const u32 destw, const u32 desth, const u32 srcw, const u32 srch, const AGIDL_CLR_FMT destfmt, const AGIDL_CLR_FMT srcfmt, const u16 xstart, const u16 xend, const u16 ystart, const u16 yend, const u16 dx, const u16 dy){
	if((AGIDL_GetBitCount(destfmt) == AGIDL_GetBitCount(srcfmt)) && (AGIDL_GetBitCount(destfmt) == 24 || AGIDL_GetBitCount(destfmt) == 32)){
		COLOR* destclr = dest;
		const COLOR* srcclr = src;

		const u32 dxx = xend - xstart;
		const u32 dyy = yend - ystart;

		const u32 maxw = dx + dxx;
		const u32 maxh = dy + dyy;
		if(AGIDL_InsideClipBounds(dxx,dyy,srcw,srch) && AGIDL_InsideClipBounds(maxw,maxh,destw,desth)){
			int incry = 0;
			for(int y = ystart; y < yend; y++){
				int incrx = 0;
				for(int x = xstart; x < xend; x++){
					const COLOR clr = AGIDL_GetClr(srcclr,x,y,srcw,srch);
					AGIDL_SetClr(destclr,clr,dx+incrx,dy+incry,destw,desth);
					incrx++;
				}
				incry++;
			}
		}
	}
	else if((AGIDL_GetBitCount(destfmt) == AGIDL_GetBitCount(srcfmt)) && AGIDL_GetBitCount(destfmt) == 16){
		COLOR16* destclr = dest;
		const COLOR16* srcclr = src;

		const u32 dxx = xend - xstart;
		const u32 dyy = yend - ystart;

		const u32 maxw = dx + dxx;
		const u32 maxh = dy + dyy;
		if(AGIDL_InsideClipBounds(dxx,dyy,srcw,srch) && AGIDL_InsideClipBounds(maxw,maxh,destw,desth)){
			int incry = 0;
			for(int y = ystart; y < yend; y++){
				int incrx = 0;
				for(int x = xstart; x < xend; x++){
					const COLOR16 clr = AGIDL_GetClr16(srcclr,x,y,srcw,srch);
					AGIDL_SetClr16(destclr,clr,dx+incrx,dy+incry,destw,desth);
					incrx++;
				}
				incry++;
			}
		}
	}
	else if((AGIDL_GetBitCount(destfmt) != AGIDL_GetBitCount(srcfmt)) && (AGIDL_GetBitCount(destfmt) == 24 || AGIDL_GetBitCount(destfmt) == 32) && AGIDL_GetBitCount(srcfmt) == 16){
		COLOR* destclr = dest;
		const COLOR16* srcclr = src;

		const u32 dxx = xend - xstart;
		const u32 dyy = yend - ystart;

		const u32 maxw = dx + dxx;
		const u32 maxh = dy + dyy;
		if(AGIDL_InsideClipBounds(dxx,dyy,srcw,srch) && AGIDL_InsideClipBounds(maxw,maxh,destw,desth)){
			int incry = 0;
			for(int y = ystart; y < yend; y++){
				int incrx = 0;
				for(int x = xstart; x < xend; x++){
					const COLOR16 clr16 = AGIDL_GetClr16(srcclr,x,y,srcw,srch);
					const COLOR clr = AGIDL_CLR16_TO_CLR(clr16,srcfmt,destfmt);
					AGIDL_SetClr(destclr,clr,dx+incrx,dy+incry,destw,desth);
					incrx++;
				}
				incry++;
			}
		}
	}
	if((AGIDL_GetBitCount(destfmt) != AGIDL_GetBitCount(srcfmt)) && (AGIDL_GetBitCount(srcfmt) == 24 || AGIDL_GetBitCount(srcfmt) == 32) && AGIDL_GetBitCount(destfmt) == 16){
		COLOR16* destclr = dest;
		const COLOR* srcclr = src;

		const u32 dxx = xend - xstart;
		const u32 dyy = yend - ystart;

		const u32 maxw = dx + dxx;
		const u32 maxh = dy + dyy;
		if(AGIDL_InsideClipBounds(dxx,dyy,srcw,srch) && AGIDL_InsideClipBounds(maxw,maxh,destw,desth)){
			int incry = 0;
			for(int y = ystart; y < yend; y++){
				int incrx = 0;
				for(int x = xstart; x < xend; x++){
					const COLOR clr24 = AGIDL_GetClr(srcclr,x,y,srcw,srch);
					const COLOR16 clr = AGIDL_CLR_TO_CLR16(clr24,srcfmt,destfmt);
					AGIDL_SetClr16(destclr,clr,dx+incrx,dy+incry,destw,desth);
					incrx++;
				}
				incry++;
			}
		}
	}
}

void AGIDL_CopyScanline(void* dest, const void* src, const u32 destw, const u32 desth, const u32 srcw, const u32 srch, const AGIDL_CLR_FMT destfmt, const AGIDL_CLR_FMT srcfmt, const u16 destscanline, const u16 srcscanline){
	if((AGIDL_GetBitCount(destfmt) == AGIDL_GetBitCount(srcfmt)) && (AGIDL_GetBitCount(srcfmt) == 24 || AGIDL_GetBitCount(srcfmt) == 32)){
		if(AGIDL_InsideClipBounds(0,srcscanline,srcw,srch) && AGIDL_InsideClipBounds(0,destscanline,destw,desth)){
			COLOR* destclr = dest;
			const COLOR* srcclr = src;

			if(destw > srcw){
				COLOR* cpyrow = malloc(sizeof(COLOR)*srcw);
				COLOR* cpydata = malloc(sizeof(COLOR)*srcw*srch);
				AGIDL_ClrMemcpy(cpydata,srcclr,srcw*srch);

				u16 width = srcw;
				u16 height = srch;

				cpydata = (COLOR*)AGIDL_ScaleImgDataNearest(cpydata,&width,&height,destw/(float)srcw,1,srcfmt);

				int x;
				for(x = 0; x < destw; x++){
					const COLOR clr = AGIDL_GetClr(cpydata,x,srcscanline,width,height);
					AGIDL_SetClr(cpyrow,clr,x,0,destw,1);
				}

				for(x = 0; x < destw; x++){
					const COLOR clr = AGIDL_GetClr(cpyrow,x,0,srcw,srch);
					AGIDL_SetClr(destclr,clr,x,destscanline,destw,desth);
				}

				free(cpydata);
				free(cpyrow);
			}
			else{
				for(int x = 0; x < destw; x++){
					const COLOR clr = AGIDL_GetClr(srcclr,x,srcscanline,srcw,srch);
					AGIDL_SetClr(destclr,clr,x,destscanline,destw,desth);
				}
			}
		}
		else if((AGIDL_GetBitCount(destfmt) == AGIDL_GetBitCount(srcfmt)) && AGIDL_GetBitCount(srcfmt) == 16){
			COLOR16* destclr = dest;
			const COLOR16* srcclr = src;
			if(AGIDL_InsideClipBounds(0,srcscanline,srcw,srch) && AGIDL_InsideClipBounds(0,destscanline,destw,desth)){
				if(destw > srcw){
					COLOR16* cpyrow = malloc(sizeof(COLOR16)*srcw);
					COLOR16* cpydata = malloc(sizeof(COLOR16)*srcw*srch);
					AGIDL_ClrMemcpy16(cpydata,srcclr,srcw*srch);

					u16 width = srcw;
					u16 height = srch;

					cpydata = (COLOR16*)AGIDL_ScaleImgDataNearest(cpydata,&width,&height,destw/(float)srcw,1,srcfmt);

					int x;
					for(x = 0; x < destw; x++){
						const COLOR16 clr = AGIDL_GetClr16(cpydata,x,srcscanline,width,height);
						AGIDL_SetClr16(cpyrow,clr,x,0,destw,1);
					}

					for(x = 0; x < destw; x++){
						const COLOR16 clr = AGIDL_GetClr16(cpyrow,x,0,srcw,srch);
						AGIDL_SetClr16(destclr,clr,x,destscanline,destw,desth);
					}

					free(cpydata);
					free(cpyrow);
				}
				else{
					for(int x = 0; x < destw; x++){
						const COLOR16 clr = AGIDL_GetClr16(srcclr,x,srcscanline,srcw,srch);
						AGIDL_SetClr16(destclr,clr,x,destscanline,destw,desth);
					}
				}
			}
		}
	}
	else if((AGIDL_GetBitCount(destfmt) == 24 || AGIDL_GetBitCount(destfmt) == 32) && AGIDL_GetBitCount(srcfmt) == 16){
		if(AGIDL_InsideClipBounds(0,srcscanline,srcw,srch) && AGIDL_InsideClipBounds(0,destscanline,destw,desth)){
			COLOR* destclr = dest;
			const COLOR16* srcclr16 = src;

			COLOR* srcclr = malloc(sizeof(COLOR)*srcw*srch);

			AGIDL_CLR_FMT fmt = srcfmt;
			AGIDL_16BPPTO24BPP(srcclr16,srcclr,srcw,srch,&fmt);

			if(destw > srcw){
				COLOR* cpyrow = malloc(sizeof(COLOR)*srcw);
				COLOR* cpydata = malloc(sizeof(COLOR)*srcw*srch);
				AGIDL_ClrMemcpy(cpydata,srcclr,srcw*srch);

				u16 width = srcw;
				u16 height = srch;

				cpydata = (COLOR*)AGIDL_ScaleImgDataNearest(cpydata,&width,&height,destw/(float)srcw,1,fmt);

				int x;
				for(x = 0; x < destw; x++){
					const COLOR clr = AGIDL_GetClr(cpydata,x,srcscanline,width,height);
					AGIDL_SetClr(cpyrow,clr,x,0,destw,1);
				}

				for(x = 0; x < destw; x++){
					const COLOR clr = AGIDL_GetClr(cpyrow,x,0,srcw,srch);
					AGIDL_SetClr(destclr,clr,x,destscanline,destw,desth);
				}

				free(cpydata);
				free(cpyrow);
			}
			else{
				for(int x = 0; x < destw; x++){
					const COLOR clr = AGIDL_GetClr(srcclr,x,srcscanline,srcw,srch);
					AGIDL_SetClr(destclr,clr,x,destscanline,destw,desth);
				}
			}

			free(srcclr);
		}
	}
	else if(AGIDL_GetBitCount(destfmt) == 16 && (AGIDL_GetBitCount(srcfmt) == 24 || AGIDL_GetBitCount(srcfmt) == 32)){
		if(AGIDL_InsideClipBounds(0,srcscanline,srcw,srch) && AGIDL_InsideClipBounds(0,destscanline,destw,desth)){
			COLOR16* destclr = dest;
			const COLOR* srcclr24 = src;

			COLOR16* srcclr = malloc(sizeof(COLOR16)*srcw*srch);

			AGIDL_CLR_FMT fmt = srcfmt;
			AGIDL_24BPPTO16BPP(srcclr24,srcclr,srcw,srch,&fmt);

			if(destw > srcw){
				COLOR16* cpyrow = malloc(sizeof(COLOR16)*srcw);
				COLOR16* cpydata = malloc(sizeof(COLOR16)*srcw*srch);
				AGIDL_ClrMemcpy16(cpydata,srcclr,srcw*srch);

				u16 width = srcw;
				u16 height = srch;

				cpydata = (COLOR16*)AGIDL_ScaleImgDataNearest(cpydata,&width,&height,destw/(float)srcw,1,fmt);

				int x;
				for(x = 0; x < destw; x++){
					const COLOR16 clr = AGIDL_GetClr16(cpydata,x,srcscanline,width,height);
					AGIDL_SetClr16(cpyrow,clr,x,0,destw,1);
				}

				for(x = 0; x < destw; x++){
					const COLOR16 clr = AGIDL_GetClr16(cpyrow,x,0,srcw,srch);
					AGIDL_SetClr16(destclr,clr,x,destscanline,destw,desth);
				}

				free(cpydata);
				free(cpyrow);
			}
			else{
				for(int x = 0; x < destw; x++){
					const COLOR clr16 = AGIDL_GetClr16(srcclr,x,srcscanline,srcw,srch);
					AGIDL_SetClr16(destclr,clr16,x,destscanline,destw,desth);
				}
			}

			free(srcclr);
		}
	}
}

u8 AGIDL_GetBitCount(const AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_RGB_888:{
			return 24;
		}
		case AGIDL_BGR_888:{
			return 24;
		}
		case AGIDL_RGB_555:{
			return 16;
		}
		case AGIDL_BGR_555:{
			return 16;
		}
		case AGIDL_RGB_565:{
			return 16;
		}
		case AGIDL_BGR_565:{
			return 16;
		}
		case AGIDL_RGBA_8888:{
			return 32;
		}
		case AGIDL_ARGB_8888:{
			return 32;
		}
	    default: return 24;
	}
}

void AGIDL_ExtractAndPrintBGR(FILE* file, const COLOR clr, const AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_BGR_888:{
			const u8 r = AGIDL_GetR(clr,fmt);
			const u8 g = AGIDL_GetG(clr,fmt);
			const u8 b = AGIDL_GetB(clr,fmt);
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

void AGIDL_ExtractAndPrintRGB(FILE* file, const COLOR clr, const AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_RGB_888:{
			const u8 r = AGIDL_GetR(clr,fmt);
			const u8 g = AGIDL_GetG(clr,fmt);
			const u8 b = AGIDL_GetB(clr,fmt);
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

void AGIDL_ExtractAndPrintRGBA(FILE* file, const COLOR clr, const AGIDL_CLR_FMT fmt){
	const u8 r = AGIDL_GetR(clr,fmt);
	const u8 g = AGIDL_GetG(clr,fmt);
	const u8 b = AGIDL_GetB(clr,fmt);
	const u8 a = AGIDL_GetA(clr,fmt);

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

void AGIDL_ExportRawColors(const void* data, const u32 width, const u32 height, const AGIDL_CLR_FMT fmt, const AGIDL_FILE_TYPE ftype, const AGIDL_ARR_TYPE arrtype, const u8 rgb){
	if(AGIDL_GetBitCount(fmt) == 24){
		const COLOR* clrs = data;
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

			for(int y = 0; y < height; y++){
				for(int x = 0; x < width; x++){
					const COLOR clr = AGIDL_GetClr(clrs,x,y,width,height);
					const u8 r = AGIDL_GetR(clr,fmt);
					const u8 g = AGIDL_GetG(clr,fmt);
					const u8 b = AGIDL_GetB(clr,fmt);
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


			for(int y = 0; y < height; y++){
				for(int x = 0; x < width; x++){
					const COLOR clr = AGIDL_GetClr(clrs,x,y,width,height);
					const u8 r = AGIDL_GetR(clr,fmt);
					const u8 g = AGIDL_GetG(clr,fmt);
					const u8 b = AGIDL_GetB(clr,fmt);
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

			for(int y = 0; y < height; y++){
				for(int x = 0; x < width; x++){
					const COLOR clr = AGIDL_GetClr(clrs,x,y,width,height);

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
		const COLOR16* clrs = data;
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


			for(int y = 0; y < height; y++){
				for(int x = 0; x < width; x++){
					const COLOR16 clr = AGIDL_GetClr16(clrs,x,y,width,height);
					const u8 r = AGIDL_GetR(clr,fmt);
					const u8 g = AGIDL_GetG(clr,fmt);
					const u8 b = AGIDL_GetB(clr,fmt);
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


			for(int y = 0; y < height; y++){
				for(int x = 0; x < width; x++){
					const COLOR16 clr = AGIDL_GetClr16(clrs,x,y,width,height);
					const u8 r = AGIDL_GetR(clr,fmt);
					const u8 g = AGIDL_GetG(clr,fmt);
					const u8 b = AGIDL_GetB(clr,fmt);
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

			for(int y = 0; y < height; y++){
				for(int x = 0; x < width; x++){
					const COLOR16 clr = AGIDL_GetClr16(clrs,x,y,width,height);

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

u8 * AGIDL_GenerateImgDataFromICP(const void* data, const u32 width, const u32 height, const AGIDL_ICP icp, int max_diff){
	max_diff = AGIDL_Clamp(0,max_diff,255);

	if(AGIDL_GetBitCount(icp.fmt) != 16){
		const COLOR* clrdata = data;
		u8* buffer = malloc(sizeof(u8)*(width*height));

		for(u32 y = 0; y < height; y++){
			for(u32 x = 0; x < width; x++){
				const COLOR clr = AGIDL_GetClr(clrdata,x,y,width,height);
				const u8 clricp = AGIDL_FindClosestColor(icp,clr,icp.fmt,max_diff);
				buffer[x+y*width] = clricp;
			}
		}

		return buffer;
	}
	const COLOR16* clrdata = data;
	u8* buffer = malloc(sizeof(u8)*(width*height));

	for(u32 y = 0; y < height; y++){
		for(u32 x = 0; x < width; x++){
			const COLOR16 clr = AGIDL_GetClr16(clrdata,x,y,width,height);
			const u8 clricp = AGIDL_FindClosestColor(icp,clr,icp.fmt,max_diff);
			buffer[x+y*width] = clricp;
		}
	}

	return buffer;
}

COLOR AGIDL_GetClr(const COLOR* clrs, const int x, const int y, const int width, const int height){
	if(x >= 0 && y >= 0 && x < width && y < height){
		return clrs[x+y*width];
	}
	fprintf(stderr, "%s: Index out of range", __FUNCTION__);
	abort();
}

COLOR16 AGIDL_GetClr16(const COLOR16* clrs, const int x, const int y, const int width, const int height){
	if(x >= 0 && y >= 0 && x < width && y < height){
		return clrs[x+y*width];
	}
	fprintf(stderr, "%s: Index out of range", __FUNCTION__);
	abort();
}

void AGIDL_SetClr(COLOR* clrs, const COLOR clr, const int x, const int y, const int width, const int height){
	if(x >= 0 && y >= 0 && x < width && y < height){
		clrs[x+y*width] = clr;
	}
}

void AGIDL_SetClr16(COLOR16* clrs, const COLOR16 clr, const int x, const int y, const int width, const int height){
	if(x >= 0 && y >= 0 && x < width && y < height){
		clrs[x+y*width] = clr;
	}
}

void AGIDL_RGB2BGR(COLOR* clrs, const int width, const int height, AGIDL_CLR_FMT *fmt){
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			COLOR clr = AGIDL_GetClr(clrs,x,y,width,height);
			clr = AGIDL_RGB_TO_BGR(clr,AGIDL_RGB_888);
			AGIDL_SetClr(clrs,clr,x,y,width,height);
		}
	}
	*fmt = AGIDL_BGR_888;
}

void AGIDL_BGR2RGB(COLOR* clrs, const int width, const int height, AGIDL_CLR_FMT *fmt){
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			COLOR clr = AGIDL_GetClr(clrs,x,y,width,height);
			clr = AGIDL_BGR_TO_RGB(clr,AGIDL_BGR_888);
			AGIDL_SetClr(clrs,clr,x,y,width,height);
		}
	}
	*fmt = AGIDL_RGB_888;
}

void AGIDL_RGB2BGR16(COLOR16* clrs, const int width, const int height, AGIDL_CLR_FMT *fmt){
	if(*fmt == AGIDL_RGB_555){
		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x++){
				COLOR16 clr = AGIDL_GetClr16(clrs,x,y,width,height);
				clr = AGIDL_RGB_TO_BGR(clr,AGIDL_RGB_555);
				AGIDL_SetClr16(clrs,clr,x,y,width,height);
			}
		}
		*fmt = AGIDL_BGR_555;
	}
	else{
		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x++){
				COLOR16 clr = AGIDL_GetClr16(clrs,x,y,width,height);
				clr = AGIDL_RGB_TO_BGR(clr,AGIDL_RGB_565);
				AGIDL_SetClr16(clrs,clr,x,y,width,height);
			}
		}
		*fmt = AGIDL_BGR_565;
	}
}

void AGIDL_BGR2RGB16(COLOR16* clrs, const int width, const int height, AGIDL_CLR_FMT *fmt){
	if(*fmt == AGIDL_BGR_555){
		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x++){
				COLOR16 clr = AGIDL_GetClr16(clrs,x,y,width,height);
				clr = AGIDL_BGR_TO_RGB(clr,AGIDL_BGR_555);
				AGIDL_SetClr16(clrs,clr,x,y,width,height);
			}
		}
		*fmt = AGIDL_RGB_555;
	}
	else{
		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x++){
				COLOR16 clr = AGIDL_GetClr16(clrs,x,y,width,height);
				clr = AGIDL_BGR_TO_RGB(clr,AGIDL_BGR_565);
				AGIDL_SetClr16(clrs,clr,x,y,width,height);
			}
		}
		*fmt = AGIDL_RGB_565;
	}
}

void AGIDL_24BPPTO16BPP(const COLOR* src, COLOR16* dest, const int width, const int height, AGIDL_CLR_FMT *fmt){
	if(*fmt == AGIDL_RGB_888){
		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x++){
				const COLOR clr = AGIDL_GetClr(src,x,y,width,height);
				const COLOR16 clr16 = AGIDL_CLR_TO_CLR16(clr, AGIDL_RGB_888, AGIDL_RGB_555);
				AGIDL_SetClr16(dest,clr16,x,y,width,height);
			}
		}
		*fmt = AGIDL_RGB_555;
	}
	if(*fmt == AGIDL_BGR_888){
		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x++){
				const COLOR clr = AGIDL_GetClr(src,x,y,width,height);
				const COLOR16 clr16 = AGIDL_CLR_TO_CLR16(clr, AGIDL_BGR_888, AGIDL_BGR_555);
				AGIDL_SetClr16(dest,clr16,x,y,width,height);
			}
		}
		*fmt = AGIDL_BGR_555;
	}
}

void AGIDL_16BPPTO24BPP(const COLOR16 *src, COLOR* dest, const int width, const int height, AGIDL_CLR_FMT* fmt){
	if(*fmt == AGIDL_RGB_555){
		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x++){
				const COLOR16 clr16 = AGIDL_GetClr16(src,x,y,width,height);
				const COLOR clr = AGIDL_CLR16_TO_CLR(clr16, AGIDL_RGB_555, AGIDL_RGB_888);
				AGIDL_SetClr(dest,clr,x,y,width,height);
			}
		}
		*fmt = AGIDL_RGB_888;
	}
	if(*fmt == AGIDL_BGR_555){
		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x++){
				const COLOR16 clr16 = AGIDL_GetClr16(src,x,y,width,height);
				const COLOR clr = AGIDL_CLR16_TO_CLR(clr16, AGIDL_BGR_555, AGIDL_BGR_888);
				AGIDL_SetClr(dest,clr,x,y,width,height);
			}
		}
		*fmt = AGIDL_BGR_888;
	}
}

void AGIDL_555TO565(COLOR16* src, const int width, const int height, AGIDL_CLR_FMT *fmt){
	if(*fmt == AGIDL_RGB_555){
		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x++){
				COLOR16 clr = AGIDL_GetClr16(src,x,y,width,height);
				clr = AGIDL_555_TO_565(clr,AGIDL_RGB_555);
				AGIDL_SetClr16(src,clr,x,y,width,height);
			}
		}
		*fmt = AGIDL_RGB_565;
	}
	else{
		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x++){
				COLOR16 clr = AGIDL_GetClr16(src,x,y,width,height);
				clr = AGIDL_555_TO_565(clr,AGIDL_BGR_555);
				AGIDL_SetClr16(src,clr,x,y,width,height);
			}
		}
		*fmt = AGIDL_BGR_565;
	}
}

void AGIDL_565TO555(COLOR16* src, const int width, const int height, AGIDL_CLR_FMT *fmt){
	if(*fmt == AGIDL_RGB_565){
		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x++){
				COLOR16 clr = AGIDL_GetClr16(src,x,y,width,height);
				clr = AGIDL_565_TO_555(clr,AGIDL_RGB_565);
				AGIDL_SetClr16(src,clr,x,y,width,height);
			}
		}
		*fmt = AGIDL_RGB_555;
	}
	else{
		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x++){
				COLOR16 clr = AGIDL_GetClr16(src,x,y,width,height);
				clr = AGIDL_565_TO_555(clr,AGIDL_BGR_565);
				AGIDL_SetClr16(src,clr,x,y,width,height);
			}
		}
		*fmt = AGIDL_BGR_555;
	}
}

u8* AGIDL_GenerateRGBBuffer(const void* data, const int width, const int height, const AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) != 16){
		const COLOR* clr_data = data;
		u8* rgbbuf = malloc(sizeof(u8)*(width*height*3));
		for(int i = 0; i < width * height; i++){
			const COLOR clr = clr_data[i];
			const u8 r = AGIDL_GetR(clr,fmt);
			const u8 g = AGIDL_GetG(clr,fmt);
			const u8 b = AGIDL_GetB(clr,fmt);
			for(int j = i * 3; j < (i * 3) + 1; j++){
				rgbbuf[j] = r; rgbbuf[j+1] = g; rgbbuf[j+2] = b;
			}
		}
		return rgbbuf;
	}
	const COLOR16* clr_data = data;
	u8* rgbbuf = malloc(sizeof(u8)*(width*height*3));
	for(int i = 0; i < width * height; i++){
		const COLOR16 clr = clr_data[i];
		const u8 r = AGIDL_GetR(clr,fmt);
		const u8 g = AGIDL_GetG(clr,fmt);
		const u8 b = AGIDL_GetB(clr,fmt);
		for(int j = i * 3; j < (i * 3) + 1; j++){
			rgbbuf[j] = r; rgbbuf[j+1] = g; rgbbuf[j+2] = b;
		}
	}
	return rgbbuf;
}

u8* AGIDL_GenerateBGRBuffer(const COLOR* src, const int width, const int height, const AGIDL_CLR_FMT fmt){
	u8* rgbbuf = malloc(sizeof(u8)*(width*height)*(3));
	for(int i = 0; i < width * height; i++){
		const COLOR clr = src[i];
		const u8 r = AGIDL_GetR(clr,fmt);
		const u8 g = AGIDL_GetG(clr,fmt);
		const u8 b = AGIDL_GetB(clr,fmt);
		for(int j = i * 3; j < (i * 3) + 1; j++){
			rgbbuf[j] = b; rgbbuf[j+1] = g; rgbbuf[j+2] = r;
		}
	}
	return rgbbuf;
}

u8* AGIDL_GenerateRGBABuffer(const COLOR* src, const int width, const int height, const AGIDL_CLR_FMT fmt){
	u8* rgbbuf = malloc(sizeof(u8)*(width*height*4));
	for(int i = 0; i < width * height; i++){
		const COLOR clr = src[i];
		const u8 r = AGIDL_GetR(clr,fmt);
		const u8 g = AGIDL_GetG(clr,fmt);
		const u8 b = AGIDL_GetB(clr,fmt);
		const u8 a = AGIDL_GetA(clr,fmt);
		for(int j = i * 4; j < (i * 4) + 1; j++){
			rgbbuf[j] = r; rgbbuf[j+1] = g; rgbbuf[j+2] = b; rgbbuf[j+3] = a;
		}
	}
	return rgbbuf;
}

COLOR* AGIDL_RGBSyncColor(const u8* rgbbuf, const int width, const int height, const AGIDL_CLR_FMT fmt){
	COLOR* clr = malloc(sizeof(COLOR)*(width*height));
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

COLOR* AGIDL_RGBASyncClrs(const u8* rgbbuf, const int width, const int height, const AGIDL_CLR_FMT fmt){
	COLOR* clr = malloc(sizeof(COLOR)*(width*height));
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

void AGIDL_PrintRGBBuffer(const u8* rgbbuf, const int width, const int height, const AGIDL_CLR_FMT fmt){
	if(fmt == AGIDL_RGB_888 || fmt == AGIDL_RGB_555 || fmt == AGIDL_RGBA_8888){
		for(int i = 0; i < width*height*3; i+=3){
			printf("RGB(%d,%d,%d)\n",rgbbuf[i],rgbbuf[i+1],rgbbuf[i+2]);
		}
	}
	else{
		for(int i = 0; i < width*height*3; i+=3){
			printf("BGR(%d,%d,%d)\n",rgbbuf[i],rgbbuf[i+1],rgbbuf[i+2]);
		}
	}
}

void AGIDL_PrintRGBABuffer(const u8* rgbbuf, const int width, const int height, const AGIDL_CLR_FMT fmt){
	if(fmt == AGIDL_RGBA_8888 || fmt == AGIDL_ARGB_8888){
		for(int i = 0; i < width*height*4; i+=4){
			printf("RGBA(%d,%d,%d,%d)\n",rgbbuf[i],rgbbuf[i+1],rgbbuf[i+2],rgbbuf[i+3]);
		}
	}
}
