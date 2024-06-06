#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <agidl.h>
#include <time.h>
#include <math.h>

//3DFX INTERACTIVE TEXTURE FILE -> .3DF [DONE]
//INTERLEAVED BITMAP -> .ILBM [DONE]
//PORTABLE PIXELMAP -> .PPM [DONE]
//VAVLE TEXTURE FORMAT -> .VTF
//VALUE 2 TEXTURE FORMAT -> .VTEX
//SILICON GRAPHICS IMAGE -> .SGI
//GRAPHICS INTERCHANGEABLE FORMAT -> .GIF
//TIM2 (PS2) -> .TIM
//JOINT PHOTOGRAPHICS EXPORT GROUP -> .JPEG
//3DO CEL IMAGE -> .CEL/.CCB
//BRENDER PIX -> .PIX
//RENDERWARE TEXTURE DICTIONARY -> .TXD
//DIRECT DRAW SURFACE -> .DDS 

int main(){

	float startTimeInterval = (float)clock() / CLOCKS_PER_SEC;

	/*PERFORM WORK*/
	
	//What is AGMV for GitHub repo? An open-source full-motion video codec optimized for real-time playback on low-end graphical devices.
	//I Programmed a Full-Motion Video Codec for the GBA And Nintendo DS
	
	AGIDL_BMP* bmp = AGIDL_LoadBMP("test.bmp");
	AGIDL_SetBMPFilename(bmp,"export.bmp");
	AGIDL_ExportBMP(bmp);
	AGIDL_FreeBMP(bmp);
	

	float endTimeInterval = (float)clock() / CLOCKS_PER_SEC;
	
	printf("deltaTime = %.4f\n",endTimeInterval-startTimeInterval);
	
	return 0;
}