/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_file_utils.c
*   Date: 1/25/2024
*   Version: 0.4b
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <agidl_file_utils.h>
#include <agidl_cc_manager.h>

AGIDL_Bool useBigEndArch = FALSE;

void AGIDL_InitBigEndArch(){
	useBigEndArch = TRUE;
}

void AGIDL_DisableBigEndArch(){
	useBigEndArch = FALSE;
}

u8 AGIDL_ReadByte(FILE* file){
	u8 byte = getc(file);
	return byte;
}

u16 AGIDL_ReadShort(FILE* file){
	u8 msb = AGIDL_ReadByte(file);
	u8 lsb = AGIDL_ReadByte(file);
	
	if(useBigEndArch == TRUE){
		return msb << 8 | lsb;
	}
	else{
		return lsb << 8 | msb;
	}
	
}

u32 AGIDL_ReadLong(FILE* file){
	u8 dword[4];
	
	if(fread(dword,1,4,file) != 4){
		return 0;
	}
	
	if(useBigEndArch == TRUE){
		return dword[0] << 24 | dword[1] << 16 | dword[2] << 8 | dword[3];
	}
	else{
		return dword[3] << 24 | dword[2] << 16 | dword[1] << 8 | dword[0];
	}
}

void AGIDL_WriteByte(FILE* file, u8 byte){
	fputc(byte,file);
}

void AGIDL_WriteShort(FILE* file, u16 word){
	u8 msb = (word & 0xff00) >> 8;
	u8 lsb = (word & 0xff);
	
	if(useBigEndArch == TRUE){
		AGIDL_WriteByte(file,msb);
		AGIDL_WriteByte(file,lsb);
	}
	else{
		AGIDL_WriteByte(file,lsb);
		AGIDL_WriteByte(file,msb);
	}
}

void AGIDL_WriteLong(FILE* file, u32 dword){
	u8 msb = (dword >> 24) & 0xff;
	u8 msb2 = (dword & 0xff0000) >> 16;
	u8 lsb2 = (dword & 0xff00) >> 8;
	u8 lsb = (dword & 0xff);
	
	if(useBigEndArch == TRUE){
		AGIDL_WriteByte(file,msb);
		AGIDL_WriteByte(file,msb2);
		AGIDL_WriteByte(file,lsb2);
		AGIDL_WriteByte(file,lsb);
	}
	else{
		AGIDL_WriteByte(file,lsb);
		AGIDL_WriteByte(file,lsb2);
		AGIDL_WriteByte(file,msb2);
		AGIDL_WriteByte(file,msb);
	}
}

u32 AGIDL_ReadRGB(FILE* file, AGIDL_CLR_FMT fmt){
	if(fmt == AGIDL_RGB_888){
		u8 r = AGIDL_ReadByte(file);
		u8 g = AGIDL_ReadByte(file);
		u8 b = AGIDL_ReadByte(file);
		
		return AGIDL_RGB(r,g,b,fmt);
	}
	else{
		u8 b = AGIDL_ReadByte(file);
		u8 g = AGIDL_ReadByte(file);
		u8 r = AGIDL_ReadByte(file);
		
		return AGIDL_RGB(r,g,b,fmt);
	}
}

void AGIDL_WriteRGB(FILE* file, COLOR clr, AGIDL_CLR_FMT fmt, AGIDL_CLR_FMT write){
	u8 r = AGIDL_GetR(clr,fmt);
	u8 g = AGIDL_GetG(clr,fmt);
	u8 b = AGIDL_GetB(clr,fmt);
	
	if(write == AGIDL_RGB_888){
		AGIDL_WriteByte(file,r);
		AGIDL_WriteByte(file,g);
		AGIDL_WriteByte(file,b);
	}
	else{
		AGIDL_WriteByte(file,b);
		AGIDL_WriteByte(file,g);
		AGIDL_WriteByte(file,r);
	}
}

void AGIDL_ReadBufRGB16(FILE* file, COLOR16* buf, u32 width, u32 height){
	fread(buf,2,width*height,file);
}

void AGIDL_ReadBufClr16(FILE* file, COLOR16* buf, u32 width, u32 height){
	u32 size = width*height, i;
	for(i = 0; i < size; i++){
		buf[i] = AGIDL_ReadShort(file);
	}
}

void AGIDL_ReadBufRGB(FILE* file, COLOR* clr, u32 width, u32 height){
	u32 size = width*height, i;
	for(i = 0; i < size; i++){
		clr[i] = AGIDL_ReadRGB(file,AGIDL_RGB_888);
	}
}


void AGIDL_ReadBufBGR(FILE* file, COLOR* clr, u32 width, u32 height){
	u32 size = width*height, i;
	for(i = 0; i < size; i++){
		clr[i] = AGIDL_ReadRGB(file,AGIDL_BGR_888);
	}
}

void AGIDL_ReadBufRGBA(FILE* file, COLOR* clr, u32 width, u32 height){
	u32 size = width*height, i;
	for(i = 0; i <size; i++){
		clr[i] = AGIDL_ReadLong(file);
	}
}

void AGIDL_ReadBufBGRA(FILE* file, COLOR* clr, u32 width, u32 height){
	u32 size = width*height, i;
	for(i = 0; i < size; i++){
		u8 b = AGIDL_ReadByte(file);
		u8 g = AGIDL_ReadByte(file);
		u8 r = AGIDL_ReadByte(file);
		u8 a = AGIDL_ReadByte(file);
		
		clr[i] = AGIDL_RGBA(r,g,b,a,AGIDL_RGBA_8888);
	}
}


void AGIDL_WriteBufClr16(FILE* file, const COLOR16* buf, u32 width, u32 height){
	fwrite(buf,2,width*height,file);
}

void AGIDL_WriteBufRGB16(FILE* file, const COLOR16* buf, u32 width, u32 height){
	u32 size = width*height, i;
	for(i = 0; i < size; i++){
		AGIDL_WriteShort(file,buf[i]);
	}
}

void AGIDL_WriteBufRGB(FILE* file, const COLOR* clr, u32 width, u32 height){
	u32 size = width*height, i;
	for(i = 0; i < size; i++){
		AGIDL_WriteRGB(file,clr[i],AGIDL_RGB_888,AGIDL_RGB_888);
	}
}

void AGIDL_WriteBufBGR(FILE* file, const COLOR* clr, u32 width, u32 height){
	u32 size = width*height, i;
	for(i = 0; i < size; i++){
		AGIDL_WriteRGB(file,clr[i],AGIDL_BGR_888,AGIDL_BGR_888);
	}
}

void AGIDL_WriteBufRGBA(FILE* file, const COLOR* clr, u32 width, u32 height){
	u32 size = width*height, i;
	for(i = 0; i < size; i++){
		AGIDL_WriteLong(file,clr[i]);
	}
}

void AGIDL_WriteBufBGRA(FILE* file, const COLOR* clr, u32 width, u32 height, AGIDL_CLR_FMT fmt){
	u32 size = width*height, i;
	for(i = 0; i < size; i++){
		u32 color = clr[i];
		
		u8 r = AGIDL_GetR(color,fmt);
		u8 g = AGIDL_GetG(color,fmt);
		u8 b = AGIDL_GetB(color,fmt);
		u8 a = AGIDL_GetA(color,fmt);
		
		AGIDL_WriteByte(file,b);
		AGIDL_WriteByte(file,g);
		AGIDL_WriteByte(file,r);
		AGIDL_WriteByte(file,a);
	}
}

void AGIDL_PrintFourCC(FILE* file, char f, char o, char u, char r){
	AGIDL_WriteByte(file,f);
	AGIDL_WriteByte(file,o);
	AGIDL_WriteByte(file,u);
	AGIDL_WriteByte(file,r);
}

void AGIDL_ReadNBytes(FILE* file, u8* buf, int n){
	int i;
	for(i = 0; i < n; i++){
		buf[i] = AGIDL_ReadByte(file);
	}
}