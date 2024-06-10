/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_halftone.c
*   Date: 1/27/2024
*   Version: 0.4b
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <stdlib.h>
#include <string.h>
#include <agidl_imgp_grayscale.h>
#include <agidl_imgp_halftone.h>
#include <agidl_math_utils.h>
#include <agidl_cc_manager.h>

void AGIDL_HalftoneImgData(void* data, u32 width, u32 height, AGIDL_CLR_FMT fmt, u8 threshold){
	AGIDL_GrayscaleImgData(data,width,height,fmt);
	
	if(AGIDL_GetBitCount(fmt) == 16){
		COLOR16* clr_data = (COLOR16*)data;
		
		float c[2][3], sump = 0, t = 0;
		
		c[0][0] = 0.0f; c[0][1] = 0.2f; c[0][2] = 0.0f;
		c[1][0] = 0.6f; c[1][1] = 0.1f; c[1][2] = 0.1f;
		
		float* ep = (float*)malloc(sizeof(float)*width*height);
		float* eg = (float*)malloc(sizeof(float)*width*height);
		
		memset(ep,0,width*height);
		memset(eg,0,width*height);
		
		threshold = AGIDL_Clamp(0,threshold,31);
		
		int i,j,xx,yy;
		
		int x,y;
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){
				sump = 0;
				
				for(i = 0; i < 2; i++){
					for(j = 0; j < 3; j++){
						xx = x-i+1;
						yy = y-j+1;
						
						xx = AGIDL_Clamp(0,xx,width-1);
						yy = AGIDL_Clamp(0,yy,height-1);
						
						sump = sump + c[i][j] * eg[xx+yy*width];
					}
				}
				
				ep[x+y*width] = sump;
				
				COLOR16 clr = AGIDL_GetClr16(clr_data,x,y,width,height);
				
				t = AGIDL_GetR(clr,fmt) + ep[x+y*width];
				
				if(t > threshold){
					if(fmt != AGIDL_RGB_565 && fmt != AGIDL_BGR_565){
						AGIDL_SetClr16(clr_data,AGIDL_RGB16(31,31,31,fmt),x,y,width,height);
						eg[x+y*width] = t - 2*threshold;
					}
					else{
						AGIDL_SetClr16(clr_data,AGIDL_RGB16(31,63,31,fmt),x,y,width,height);
						eg[x+y*width] = t - 2*threshold;
					}
				}
				else{
					AGIDL_SetClr16(clr_data,AGIDL_RGB16(0,0,0,fmt),x,y,width,height);
					eg[x+y*width] = threshold;
				}
			}
		}
		
		free(ep);
		free(eg);
		
	}
	else{
		COLOR* clr_data = (COLOR*)data;
		
		float c[2][3], sump = 0, t = 0;
		
		c[0][0] = 0.0f; c[0][1] = 0.2f; c[0][2] = 0.0f;
		c[1][0] = 0.6f; c[1][1] = 0.1f; c[1][2] = 0.1f;
		
		float* ep = (float*)malloc(sizeof(float)*width*height);
		float* eg = (float*)malloc(sizeof(float)*width*height);
		
		memset(ep,0,width*height);
		memset(eg,0,width*height);
		
		int i,j,xx,yy;
		
		int x,y;
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){
				sump = 0;
				
				for(i = 0; i < 2; i++){
					for(j = 0; j < 3; j++){
						xx = x-i+1;
						yy = y-j+1;
						
						xx = AGIDL_Clamp(0,xx,width-1);
						yy = AGIDL_Clamp(0,yy,height-1);
						
						sump = sump + c[i][j] * eg[xx+yy*width];
					}
				}
				
				ep[x+y*width] = sump;
				
				COLOR clr = AGIDL_GetClr(clr_data,x,y,width,height);
				
				t = AGIDL_GetR(clr,fmt) + ep[x+y*width];
				
				if(t > threshold){
					if(AGIDL_GetBitCount(fmt) != 32){
						AGIDL_SetClr(clr_data,AGIDL_RGB(255,255,255,fmt),x,y,width,height);
					}
					else{
						AGIDL_SetClr(clr_data,AGIDL_RGBA(255,255,255,AGIDL_GetA(clr,fmt),fmt),x,y,width,height);
					}
					eg[x+y*width] = t - 2*threshold;
				}
				else{
					if(AGIDL_GetBitCount(fmt) != 32){
						AGIDL_SetClr(clr_data,AGIDL_RGB(0,0,0,fmt),x,y,width,height);
					}
					else{
						AGIDL_SetClr(clr_data,AGIDL_RGBA(0,0,0,AGIDL_GetA(clr,fmt),fmt),x,y,width,height);
					}
					eg[x+y*width] = threshold;
				}
			}
		}
		
		free(ep);
		free(eg);
	}
}