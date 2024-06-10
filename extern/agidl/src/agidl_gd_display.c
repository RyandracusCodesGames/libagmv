/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_gd_display.c
*   Date: 2/1/2024
*   Version: 0.4b
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <agidl_gd_display.h>
#include <agidl_cc_manager.h>
#include <agidl_cc_mixer.h>

void AGIDL_DispImgData(void* vram, u32 width, u32 height, AGIDL_ATTR table, const void* spr, u32 widths, u32 heights, AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 16){
		COLOR16* vram16 = (COLOR16*)vram;
		COLOR16* spr16 = (COLOR16*)spr;
		
		if(table.enableScale != TRUE){
			u32 x,y;
			for(y = 0; y < heights; y++){
				for(x = 0; x < widths; x++){
					COLOR16 clr;
					
					if(table.flipHorz == TRUE && table.flipVert != TRUE){
						clr = AGIDL_GetClr16(spr16,x,(heights-y)-1,widths,heights);
						if(table.enableChroma == TRUE){
							if(clr != table.chromaKey){
								AGIDL_SetClr16(vram16,clr,x+table.tx,y+table.ty,width,height);
							}
						}
						else{
							AGIDL_SetClr16(vram16,clr,x+table.tx,y+table.ty,width,height);
						}
					}
					else if(table.flipHorz != TRUE && table.flipVert == TRUE){
						clr = AGIDL_GetClr16(spr16,(widths-x)-1,y,widths,heights);
						if(table.enableChroma == TRUE){
							if(clr != table.chromaKey){
								AGIDL_SetClr16(vram16,clr,x+table.tx,y+table.ty,width,height);
							}
						}
						else{
							AGIDL_SetClr16(vram16,clr,x+table.tx,y+table.ty,width,height);
						}
					}
					else if(table.flipHorz == TRUE && table.flipVert == TRUE){
						clr = AGIDL_GetClr16(spr16,(widths-x)-1,(heights-y)-1,widths,heights);
						if(table.enableChroma == TRUE){
							if(clr != table.chromaKey){
								AGIDL_SetClr16(vram16,clr,x+table.tx,y+table.ty,width,height);
							}
						}
						else{
							AGIDL_SetClr16(vram16,clr,x+table.tx,y+table.ty,width,height);
						}
					}
					else{
						clr = AGIDL_GetClr16(spr16,x,y,widths,heights);
						if(table.enableChroma == TRUE){	
							if(clr != table.chromaKey){
								AGIDL_SetClr16(vram16,clr,x+table.tx,y+table.ty,width,height);
							}
						}
						else{
							AGIDL_SetClr16(vram16,clr,x+table.tx,y+table.ty,width,height);
						}
					}
				}
			}
		}
		else{
			u32 w = widths*table.sx;
			u32 h = heights*table.sy;
			
			f32 xscale = (widths-1)/(float)(w);
			f32 yscale = (heights-1)/(float)(h);
			
			u32 x,y;
			for(y = 0; y < h; y++){
				for(x = 0; x < w; x++){
					u32 x2 = (x*xscale);
					u32 y2 = (y*yscale);
					
					COLOR16 clr; 
					
					if(table.flipHorz == TRUE && table.flipVert != TRUE){
						clr = AGIDL_GetClr16(spr16,x2,(heights-y2)-1,widths,heights);
						if(table.enableChroma == TRUE){
							if(clr != table.chromaKey){
								AGIDL_SetClr16(vram16,clr,x+table.tx,y+table.ty,width,height);
							}
						}
						else{
							AGIDL_SetClr16(vram16,clr,x+table.tx,y+table.ty,width,height);
						}
					}
					else if(table.flipHorz != TRUE && table.flipVert == TRUE){
						clr = AGIDL_GetClr16(spr16,(widths-x2)-1,y2,widths,heights);
						if(table.enableChroma == TRUE){
							if(clr != table.chromaKey){
								AGIDL_SetClr16(vram16,clr,x+table.tx,y+table.ty,width,height);
							}
						}
						else{
							AGIDL_SetClr16(vram16,clr,x+table.tx,y+table.ty,width,height);
						}
					}
					else if(table.flipHorz == TRUE && table.flipVert == TRUE){
						clr = AGIDL_GetClr16(spr16,(widths-x2)-1,(heights-y2)-1,widths,heights);
						if(table.enableChroma == TRUE){
							if(clr != table.chromaKey){
								AGIDL_SetClr16(vram16,clr,x+table.tx,y+table.ty,width,height);
							}
						}
						else{
							AGIDL_SetClr16(vram16,clr,x+table.tx,y+table.ty,width,height);
						}
					}
					else{
						clr = AGIDL_GetClr16(spr16,x2,y2,widths,heights);
						if(table.enableChroma == TRUE){
							if(clr != table.chromaKey){
								AGIDL_SetClr16(vram16,clr,x+table.tx,y+table.ty,width,height);
							}
						}
						else{
							AGIDL_SetClr16(vram16,clr,x+table.tx,y+table.ty,width,height);
						}
					}
				}
			}
		}
	}
	else if(AGIDL_GetBitCount(fmt) == 24){
		COLOR* vram16 = (COLOR*)vram;
		COLOR* spr16 = (COLOR*)spr;
		
		if(table.enableScale != TRUE){
			u32 x,y;
			for(y = 0; y < heights; y++){
				for(x = 0; x < widths; x++){
					COLOR clr;
					
					if(table.flipHorz == TRUE && table.flipVert != TRUE){
						clr = AGIDL_GetClr(spr16,x,(heights-y)-1,widths,heights);
						if(table.enableChroma == TRUE){
							if(clr != table.chromaKey){
								AGIDL_SetClr(vram16,clr,x+table.tx,y+table.ty,width,height);
							}
						}
						else{
							AGIDL_SetClr(vram16,clr,x+table.tx,y+table.ty,width,height);
						}
					}
					else if(table.flipHorz != TRUE && table.flipVert == TRUE){
						clr = AGIDL_GetClr(spr16,(widths-x)-1,y,widths,heights);
						if(table.enableChroma == TRUE){
							if(clr != table.chromaKey){
								AGIDL_SetClr(vram16,clr,x+table.tx,y+table.ty,width,height);
							}
						}
						else{
							AGIDL_SetClr(vram16,clr,x+table.tx,y+table.ty,width,height);
						}
					}
					else if(table.flipHorz == TRUE && table.flipVert == TRUE){
						clr = AGIDL_GetClr(spr16,(widths-x)-1,(heights-y)-1,widths,heights);
						if(table.enableChroma == TRUE){
							if(clr != table.chromaKey){
								AGIDL_SetClr(vram16,clr,x+table.tx,y+table.ty,width,height);
							}
						}
						else{
							AGIDL_SetClr(vram16,clr,x+table.tx,y+table.ty,width,height);
						}
					}
					else{
						clr = AGIDL_GetClr(spr16,x,y,widths,heights);
						if(table.enableChroma == TRUE){
							if(clr != table.chromaKey){
								AGIDL_SetClr(vram16,clr,x+table.tx,y+table.ty,width,height);
							}
						}
						else{
							AGIDL_SetClr(vram16,clr,x+table.tx,y+table.ty,width,height);
						}
					}
				}
			}
		}
		else{
			u32 w = widths*table.sx;
			u32 h = heights*table.sy;
			
			f32 xscale = (widths-1)/(float)(w);
			f32 yscale = (heights-1)/(float)(h);
			
			u32 x,y;
			for(y = 0; y < h; y++){
				for(x = 0; x < w; x++){
					u32 x2 = (x*xscale);
					u32 y2 = (y*yscale);
					
					COLOR clr; 
					
					if(table.flipHorz == TRUE && table.flipVert != TRUE){
						clr = AGIDL_GetClr(spr16,x2,(heights-y2)-1,widths,heights);
						if(table.enableChroma == TRUE){
							if(clr != table.chromaKey){
								AGIDL_SetClr(vram16,clr,x+table.tx,y+table.ty,width,height);
							}
						}
						else{
							AGIDL_SetClr(vram16,clr,x+table.tx,y+table.ty,width,height);
						}
					}
					else if(table.flipHorz != TRUE && table.flipVert == TRUE){
						clr = AGIDL_GetClr(spr16,(widths-x2)-1,y2,widths,heights);
						if(table.enableChroma == TRUE){
							if(clr != table.chromaKey){
								AGIDL_SetClr(vram16,clr,x+table.tx,y+table.ty,width,height);
							}
						}
						else{
							AGIDL_SetClr(vram16,clr,x+table.tx,y+table.ty,width,height);
						}
					}
					else if(table.flipHorz == TRUE && table.flipVert == TRUE){
						clr = AGIDL_GetClr(spr16,(widths-x2)-1,(heights-y2)-1,widths,heights);
						if(table.enableChroma == TRUE){
							if(clr != table.chromaKey){
								AGIDL_SetClr(vram16,clr,x+table.tx,y+table.ty,width,height);
							}
						}
						else{
							AGIDL_SetClr(vram16,clr,x+table.tx,y+table.ty,width,height);
						}
					}
					else{
						clr = AGIDL_GetClr(spr16,x2,y2,widths,heights);
						if(table.enableChroma == TRUE){
							if(clr != table.chromaKey){
								AGIDL_SetClr(vram16,clr,x+table.tx,y+table.ty,width,height);
							}
						}
						else{
							AGIDL_SetClr(vram16,clr,x+table.tx,y+table.ty,width,height);
						}
					}
				}
			}
		}
	}
	else{
		COLOR* vram32 = (COLOR*)vram;
		COLOR* spr32 = (COLOR*)spr;
		
		if(table.enableScale != TRUE){
			u32 x,y;
			for(y = 0; y < heights; y++){
				for(x = 0; x < widths; x++){
					if(table.flipHorz == TRUE && table.flipVert != TRUE){
						COLOR clr = AGIDL_GetClr(spr32,x,(heights-y)-1,widths,heights);
						COLOR vramclr = AGIDL_GetClr(vram32,x+table.tx,y+table.ty,width,height);
						
						COLOR blend = AGIDL_BlendColor(vramclr,clr,0,CC_BLEND_CLR_DESTINV,fmt);
						
						AGIDL_SetClr(vram32,blend,x+table.tx,y+table.ty,width,height);
					}
					else if(table.flipHorz != TRUE && table.flipVert == TRUE){
						COLOR clr = AGIDL_GetClr(spr32,(widths-x)-1,y,widths,heights);
						COLOR vramclr = AGIDL_GetClr(vram32,x+table.tx,y+table.ty,width,height);
						
						COLOR blend = AGIDL_BlendColor(vramclr,clr,0,CC_BLEND_CLR_DESTINV,fmt);
						
						AGIDL_SetClr(vram32,blend,x+table.tx,y+table.ty,width,height);
					}
					else if(table.flipHorz == TRUE && table.flipVert == TRUE){
						COLOR clr = AGIDL_GetClr(spr32,(widths-x)-1,(heights-y)-1,widths,heights);
						COLOR vramclr = AGIDL_GetClr(vram32,x+table.tx,y+table.ty,width,height);
						
						COLOR blend = AGIDL_BlendColor(vramclr,clr,0,CC_BLEND_CLR_DESTINV,fmt);
						
						AGIDL_SetClr(vram32,blend,x+table.tx,y+table.ty,width,height);
					}
					else{
						COLOR clr = AGIDL_GetClr(spr32,x,y,widths,heights);
						COLOR vramclr = AGIDL_GetClr(vram32,x+table.tx,y+table.ty,width,height);
						
						COLOR blend = AGIDL_BlendColor(vramclr,clr,0,CC_BLEND_CLR_DESTINV,fmt);
						
						AGIDL_SetClr(vram32,blend,x+table.tx,y+table.ty,width,height);
					}
				}
			}
		}
		else{
			u32 w = widths*table.sx;
			u32 h = heights*table.sy;
			
			f32 xscale = (widths-1)/(float)(w);
			f32 yscale = (heights-1)/(float)(h);
			
			u32 x,y;
			for(y = 0; y < h; y++){
				for(x = 0; x < w; x++){
					u32 x2 = (x*xscale);
					u32 y2 = (y*yscale);
					
					COLOR clr; 
					
					if(table.flipHorz == TRUE && table.flipVert != TRUE){
						clr = AGIDL_GetClr(spr32,x2,(heights-y2)-1,widths,heights);
						COLOR vramclr = AGIDL_GetClr(vram32,x+table.tx,y+table.ty,width,height);
						
						COLOR blend = AGIDL_BlendColor(vramclr,clr,0,CC_BLEND_CLR_DESTINV,fmt);
						
						AGIDL_SetClr(vram32,blend,x+table.tx,y+table.ty,width,height);
					}
					else if(table.flipHorz != TRUE && table.flipVert == TRUE){
						clr = AGIDL_GetClr(spr32,(widths-x2)-1,y2,widths,heights);
						COLOR vramclr = AGIDL_GetClr(vram32,x+table.tx,y+table.ty,width,height);
						
						COLOR blend = AGIDL_BlendColor(vramclr,clr,0,CC_BLEND_CLR_DESTINV,fmt);
						
						AGIDL_SetClr(vram32,blend,x+table.tx,y+table.ty,width,height);
					}
					else if(table.flipHorz == TRUE && table.flipVert == TRUE){
						clr = AGIDL_GetClr(spr32,(widths-x2)-1,(heights-y2)-1,widths,heights);
						COLOR vramclr = AGIDL_GetClr(vram32,x+table.tx,y+table.ty,width,height);
						
						COLOR blend = AGIDL_BlendColor(vramclr,clr,0,CC_BLEND_CLR_DESTINV,fmt);
						
						AGIDL_SetClr(vram32,blend,x+table.tx,y+table.ty,width,height);
					}
					else{
						clr = AGIDL_GetClr(spr32,x2,y2,widths,heights);
						COLOR vramclr = AGIDL_GetClr(vram32,x+table.tx,y+table.ty,width,height);
						
						COLOR blend = AGIDL_BlendColor(vramclr,clr,0,CC_BLEND_CLR_DESTINV,fmt);
						
						AGIDL_SetClr(vram32,blend,x+table.tx,y+table.ty,width,height);
					}
				}
			}
		}
	}
}

void AGIDL_DrawImgData(void* vram, u32 width, u32 height, int tx, int ty, const void* spr, u32 widths, u32 heights, AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 16){
		COLOR16* vram16 = (COLOR16*)vram;
		COLOR16* spr16 = (COLOR16*)spr;
		
		u32 x,y;
		for(y = 0; y < heights; y++){
			for(x = 0; x < widths; x++){
				COLOR16 clr = AGIDL_GetClr16(spr16,x,y,widths,heights);
				AGIDL_SetClr16(vram16,clr,x+tx,y+ty,width,height);
			}
		}
	}
	else if(AGIDL_GetBitCount(fmt) == 24){
		COLOR* vram24 = (COLOR*)vram;
		COLOR* spr24 = (COLOR*)spr;
		
		u32 x,y;
		for(y = 0; y < heights; y++){
			for(x = 0; x < widths; x++){
				COLOR clr = AGIDL_GetClr(spr24,x,y,widths,heights);
				AGIDL_SetClr(vram24,clr,x+tx,y+ty,width,height);
			}
		}
	}
	else{
		COLOR* vram32 = (COLOR*)vram;
		COLOR* spr32 = (COLOR*)spr;
		
		u32 x,y;
		for(y = 0; y < heights; y++){
			for(x = 0; x < widths; x++){
				COLOR clr = AGIDL_GetClr(spr32,x,y,widths,heights);
				COLOR vramclr = AGIDL_GetClr(vram32,x+tx,y+ty,width,height);
				
				COLOR blend = AGIDL_BlendColor(vramclr,clr,0,CC_BLEND_CLR_DESTINV,fmt);
						
				AGIDL_SetClr(vram32,blend,x+tx,y+ty,width,height);
			}
		}
	}
}

void AGIDL_DispScaleImgData(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty, const void* spr, u32 widths, u32 heights, AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 16){
		COLOR16* vram16 = (COLOR16*)vram;
		COLOR16* spr16 = (COLOR16*)spr;
		
		u32 w = widths*sx;
		u32 h = heights*sy;
		
		f32 xscale = (widths-1)/(float)(w);
		f32 yscale = (heights-1)/(float)(h);
		
		u32 x,y;
		for(y = 0; y < h; y++){
			for(x = 0; x < w; x++){
				u32 x2 = (x*xscale);
				u32 y2 = (y*yscale);
				
				COLOR16 clr = AGIDL_GetClr16(spr16,x2,y2,widths,heights);
				AGIDL_SetClr16(vram16,clr,x+tx,y+ty,width,height);
			}
		}
	}
	else if(AGIDL_GetBitCount(fmt) == 24){
		COLOR* vram24 = (COLOR*)vram;
		COLOR* spr24 = (COLOR*)spr;
		
		u32 w = widths*sx;
		u32 h = heights*sy;
		
		f32 xscale = (widths-1)/(float)(w);
		f32 yscale = (heights-1)/(float)(h);
		
		u32 x,y;
		for(y = 0; y < h; y++){
			for(x = 0; x < w; x++){
				u32 x2 = (x*xscale);
				u32 y2 = (y*yscale);
				
				COLOR clr = AGIDL_GetClr(spr24,x2,y2,widths,heights);
				AGIDL_SetClr(vram24,clr,x+tx,y+ty,width,height);
			}
		}
	}
	else{
		COLOR* vram32 = (COLOR*)vram;
		COLOR* spr32 = (COLOR*)spr;
		
		u32 w = widths*sx;
		u32 h = heights*sy;
		
		f32 xscale = (widths-1)/(float)(w);
		f32 yscale = (heights-1)/(float)(h);
		
		u32 x,y;
		for(y = 0; y < h; y++){
			for(x = 0; x < w; x++){
				u32 x2 = (x*xscale);
				u32 y2 = (y*yscale);
				
				COLOR clr = AGIDL_GetClr(spr32,x2,y2,widths,heights);
				COLOR vramclr = AGIDL_GetClr(vram32,x+tx,y+ty,width,height);
				
				COLOR blend = AGIDL_BlendColor(vramclr,clr,0,CC_BLEND_CLR_DESTINV,fmt);
						
				AGIDL_SetClr(vram32,blend,x+tx,y+ty,width,height);
			}
		}
	}
}

void AGIDL_DispFlipHorzImgData(void* vram, u32 width, u32 height, int tx, int ty, const void* spr, u32 widths, u32 heights, AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 16){
		COLOR16* vram16 = (COLOR16*)vram;
		COLOR16* spr16 = (COLOR16*)spr;
		
		int x,y;
		for(y = 0; y < heights; y++){
			for(x = 0; x < widths; x++){
				COLOR16 clr = AGIDL_GetClr16(spr16,x,(heights-y)-1,widths,heights);
				AGIDL_SetClr16(vram16,clr,x+tx,y+ty,width,height);
			}
		}
	}
	else if(AGIDL_GetBitCount(fmt) == 24){
		COLOR* vram24 = (COLOR*)vram;
		COLOR* spr24 = (COLOR*)spr;
		
		int x,y;
		for(y = 0; y < heights; y++){
			for(x = 0; x < widths; x++){
				COLOR clr = AGIDL_GetClr(spr24,x,(heights-y)-1,widths,heights);
				AGIDL_SetClr(vram24,clr,x+tx,y+ty,width,height);
			}
		}
	}
	else{
		COLOR* vram32 = (COLOR*)vram;
		COLOR* spr32 = (COLOR*)spr;
		
		int x,y;
		for(y = 0; y < heights; y++){
			for(x = 0; x < widths; x++){
				COLOR clr = AGIDL_GetClr(spr32,x,(heights-y)-1,widths,heights);
				COLOR vramclr = AGIDL_GetClr(vram32,x+tx,y+ty,width,height);
					
				COLOR blend = AGIDL_BlendColor(vramclr,clr,0,CC_BLEND_CLR_DESTINV,fmt);
						
				AGIDL_SetClr(vram32,blend,x+tx,y+ty,width,height);
			}
		}
	}
}

void AGIDL_DispFlipVertImgData(void* vram, u32 width, u32 height, int tx, int ty, const void* spr, u32 widths, u32 heights, AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 16){
		COLOR16* vram16 = (COLOR16*)vram;
		COLOR16* spr16 = (COLOR16*)spr;
		
		int x,y;
		for(y = 0; y < heights; y++){
			for(x = 0; x < widths; x++){
				COLOR16 clr = AGIDL_GetClr16(spr16,(widths-x)-1,y,widths,heights);
				AGIDL_SetClr16(vram16,clr,x+tx,y+ty,width,height);
			}
		}
	}
	else if(AGIDL_GetBitCount(fmt) == 24){
		COLOR* vram24 = (COLOR*)vram;
		COLOR* spr24 = (COLOR*)spr;
		
		int x,y;
		for(y = 0; y < heights; y++){
			for(x = 0; x < widths; x++){
				COLOR clr = AGIDL_GetClr(spr24,(widths-x)-1,y,widths,heights);
				AGIDL_SetClr(vram24,clr,x+tx,y+ty,width,height);
			}
		}
	}
	else{
		COLOR* vram32 = (COLOR*)vram;
		COLOR* spr32 = (COLOR*)spr;
		
		int x,y;
		for(y = 0; y < heights; y++){
			for(x = 0; x < widths; x++){
				COLOR clr = AGIDL_GetClr(spr32,(widths-x)-1,y,widths,heights);
				COLOR vramclr = AGIDL_GetClr(vram32,x+tx,y+ty,width,height);
						
				COLOR blend = AGIDL_BlendColor(vramclr,clr,0,CC_BLEND_CLR_DESTINV,fmt);
						
				AGIDL_SetClr(vram32,blend,x+tx,y+ty,width,height);
			}
		}
	}
}

void AGIDL_DispFlipHorzAndVertImgData(void* vram, u32 width, u32 height, int tx, int ty, const void* spr, u32 widths, u32 heights, AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 16){
		COLOR16* vram16 = (COLOR16*)vram;
		COLOR16* spr16 = (COLOR16*)spr;
		
		int x,y;
		for(y = 0; y < heights; y++){
			for(x = 0; x < widths; x++){
				COLOR16 clr = AGIDL_GetClr16(spr16,(widths-x)-1,(heights-y)-1,widths,heights);
				AGIDL_SetClr16(vram16,clr,x+tx,y+ty,width,height);
			}
		}
	}
	else if(AGIDL_GetBitCount(fmt) == 24){
		COLOR* vram24 = (COLOR*)vram;
		COLOR* spr24 = (COLOR*)spr;
		
		int x,y;
		for(y = 0; y < heights; y++){
			for(x = 0; x < widths; x++){
				COLOR clr = AGIDL_GetClr(spr24,(widths-x)-1,(heights-y)-1,widths,heights);
				AGIDL_SetClr(vram24,clr,x+tx,y+ty,width,height);
			}
		}
	}
	else{
		COLOR* vram32 = (COLOR*)vram;
		COLOR* spr32 = (COLOR*)spr;
		
		int x,y;
		for(y = 0; y < heights; y++){
			for(x = 0; x < widths; x++){
				COLOR clr = AGIDL_GetClr(spr32,(widths-x)-1,(heights-y)-1,widths,heights);
				COLOR vramclr = AGIDL_GetClr(vram32,x+tx,y+ty,width,height);
						
				COLOR blend = AGIDL_BlendColor(vramclr,clr,0,CC_BLEND_CLR_DESTINV,fmt);
						
				AGIDL_SetClr(vram32,blend,x+tx,y+ty,width,height);
			}
		}
	}
}

void AGIDL_DispBMP(void* vram, u32 width, u32 height, AGIDL_ATTR table, AGIDL_BMP* bmp){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 16){
		AGIDL_DispImgData(vram,width,height,table,bmp->pixels.pix16,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),AGIDL_BMPGetClrFmt(bmp));
	}
	else{
		AGIDL_DispImgData(vram,width,height,table,bmp->pixels.pix32,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),AGIDL_BMPGetClrFmt(bmp));
	}
}

void AGIDL_DispTGA(void* vram, u32 width, u32 height, AGIDL_ATTR table, AGIDL_TGA* tga){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		AGIDL_DispImgData(vram,width,height,table,tga->pixels.pix16,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),AGIDL_TGAGetClrFmt(tga));
	}
	else{
		AGIDL_DispImgData(vram,width,height,table,tga->pixels.pix32,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),AGIDL_TGAGetClrFmt(tga));
	}
}

void AGIDL_DispTIM(void* vram, u32 width, u32 height, AGIDL_ATTR table, AGIDL_TIM* tim){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 16){
		AGIDL_DispImgData(vram,width,height,table,tim->pixels.pix16,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),AGIDL_TIMGetClrFmt(tim));
	}
	else{
		AGIDL_DispImgData(vram,width,height,table,tim->pixels.pix32,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),AGIDL_TIMGetClrFmt(tim));
	}
}

void AGIDL_DispPCX(void* vram, u32 width, u32 height, AGIDL_ATTR table, AGIDL_PCX* pcx){
	if(AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx)) == 16){
		AGIDL_DispImgData(vram,width,height,table,pcx->pixels.pix16,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),AGIDL_PCXGetClrFmt(pcx));
	}
	else{
		AGIDL_DispImgData(vram,width,height,table,pcx->pixels.pix32,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),AGIDL_PCXGetClrFmt(pcx));
	}
}

void AGIDL_DispLMP(void* vram, u32 width, u32 height, AGIDL_ATTR table, AGIDL_LMP* lmp){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) == 16){
		AGIDL_DispImgData(vram,width,height,table,lmp->pixels.pix16,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),AGIDL_LMPGetClrFmt(lmp));
	}
	else{
		AGIDL_DispImgData(vram,width,height,table,lmp->pixels.pix32,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),AGIDL_LMPGetClrFmt(lmp));
	}
}

void AGIDL_DispPVR(void* vram, u32 width, u32 height, AGIDL_ATTR table, AGIDL_PVR* pvr){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) == 16){
		AGIDL_DispImgData(vram,width,height,table,pvr->pixels.pix16,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr));
	}
	else{
		AGIDL_DispImgData(vram,width,height,table,pvr->pixels.pix32,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr));
	}
}
 
void AGIDL_DispGXT(void* vram, u32 width, u32 height, AGIDL_ATTR table, AGIDL_GXT* gxt){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 16){
		AGIDL_DispImgData(vram,width,height,table,gxt->pixels.pix16,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),AGIDL_GXTGetClrFmt(gxt));
	}
	else{
		AGIDL_DispImgData(vram,width,height,table,gxt->pixels.pix32,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),AGIDL_GXTGetClrFmt(gxt));
	}
}

void AGIDL_DispBTI(void* vram, u32 width, u32 height, AGIDL_ATTR table, AGIDL_BTI* bti){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		AGIDL_DispImgData(vram,width,height,table,bti->pixels.pix16,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),AGIDL_BTIGetClrFmt(bti));
	}
	else{
		AGIDL_DispImgData(vram,width,height,table,bti->pixels.pix32,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),AGIDL_BTIGetClrFmt(bti));
	}
}

void AGIDL_Disp3DF(void* vram, u32 width, u32 height, AGIDL_ATTR table, AGIDL_3DF* glide){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 16){
		AGIDL_DispImgData(vram,width,height,table,glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
	}
	else{
		AGIDL_DispImgData(vram,width,height,table,glide->pixels.pix32,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
	}
}

void AGIDL_DispPPM(void* vram, u32 width, u32 height, AGIDL_ATTR table, AGIDL_PPM* ppm){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 16){
		AGIDL_DispImgData(vram,width,height,table,ppm->pixels.pix16,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),AGIDL_PPMGetClrFmt(ppm));
	}
	else{
		AGIDL_DispImgData(vram,width,height,table,ppm->pixels.pix32,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),AGIDL_PPMGetClrFmt(ppm));
	}
}

void AGIDL_DispLBM(void* vram, u32 width, u32 height, AGIDL_ATTR table, AGIDL_LBM* lbm){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 16){
		AGIDL_DispImgData(vram,width,height,table,lbm->pixels.pix16,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),AGIDL_LBMGetClrFmt(lbm));
	}
	else{
		AGIDL_DispImgData(vram,width,height,table,lbm->pixels.pix32,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),AGIDL_LBMGetClrFmt(lbm));
	}
}

void AGIDL_DrawBMP(void* vram, u32 width, u32 height, int tx, int ty, AGIDL_BMP* bmp){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 16){
		AGIDL_DrawImgData(vram,width,height,tx,ty,bmp->pixels.pix16,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),AGIDL_BMPGetClrFmt(bmp));
	}
	else{
		AGIDL_DrawImgData(vram,width,height,tx,ty,bmp->pixels.pix32,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),AGIDL_BMPGetClrFmt(bmp));
	}
}

void AGIDL_DrawTGA(void* vram, u32 width, u32 height, int tx, int ty, AGIDL_TGA* tga){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		AGIDL_DrawImgData(vram,width,height,tx,ty,tga->pixels.pix16,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),AGIDL_TGAGetClrFmt(tga));
	}
	else{
		AGIDL_DrawImgData(vram,width,height,tx,ty,tga->pixels.pix32,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),AGIDL_TGAGetClrFmt(tga));
	}
}

void AGIDL_DrawTIM(void* vram, u32 width, u32 height, int tx, int ty, AGIDL_TIM* tim){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 16){
		AGIDL_DrawImgData(vram,width,height,tx,ty,tim->pixels.pix16,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),AGIDL_TIMGetClrFmt(tim));
	}
	else{
		AGIDL_DrawImgData(vram,width,height,tx,ty,tim->pixels.pix32,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),AGIDL_TIMGetClrFmt(tim));
	}
}

void AGIDL_DrawPCX(void* vram, u32 width, u32 height, int tx, int ty, AGIDL_PCX* pcx){
	if(AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx)) == 16){
		AGIDL_DrawImgData(vram,width,height,tx,ty,pcx->pixels.pix16,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),AGIDL_PCXGetClrFmt(pcx));
	}
	else{
		AGIDL_DrawImgData(vram,width,height,tx,ty,pcx->pixels.pix32,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),AGIDL_PCXGetClrFmt(pcx));
	}
}

void AGIDL_DrawLMP(void* vram, u32 width, u32 height, int tx, int ty, AGIDL_LMP* lmp){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) == 16){
		AGIDL_DrawImgData(vram,width,height,tx,ty,lmp->pixels.pix16,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),AGIDL_LMPGetClrFmt(lmp));
	}
	else{
		AGIDL_DrawImgData(vram,width,height,tx,ty,lmp->pixels.pix32,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),AGIDL_LMPGetClrFmt(lmp));
	}
}

void AGIDL_DrawPVR(void* vram, u32 width, u32 height, int tx, int ty, AGIDL_PVR* pvr){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) == 16){
		AGIDL_DrawImgData(vram,width,height,tx,ty,pvr->pixels.pix16,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr));
	}
	else{
		AGIDL_DrawImgData(vram,width,height,tx,ty,pvr->pixels.pix32,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr));
	}
}

void AGIDL_DrawGXT(void* vram, u32 width, u32 height, int tx, int ty, AGIDL_GXT* gxt){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 16){
		AGIDL_DrawImgData(vram,width,height,tx,ty,gxt->pixels.pix16,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),AGIDL_GXTGetClrFmt(gxt));
	}
	else{
		AGIDL_DrawImgData(vram,width,height,tx,ty,gxt->pixels.pix32,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),AGIDL_GXTGetClrFmt(gxt));
	}
}

void AGIDL_DrawBTI(void* vram, u32 width, u32 height, int tx, int ty, AGIDL_BTI* bti){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		AGIDL_DrawImgData(vram,width,height,tx,ty,bti->pixels.pix16,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),AGIDL_BTIGetClrFmt(bti));
	}
	else{
		AGIDL_DrawImgData(vram,width,height,tx,ty,bti->pixels.pix32,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),AGIDL_BTIGetClrFmt(bti));
	}
}

void AGIDL_Draw3DF(void* vram, u32 width, u32 height, int tx, int ty, AGIDL_3DF* glide){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 16){
		AGIDL_DrawImgData(vram,width,height,tx,ty,glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
	}
	else{
		AGIDL_DrawImgData(vram,width,height,tx,ty,glide->pixels.pix32,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
	}
}

void AGIDL_DrawPPM(void* vram, u32 width, u32 height, int tx, int ty, AGIDL_PPM* ppm){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 16){
		AGIDL_DrawImgData(vram,width,height,tx,ty,ppm->pixels.pix16,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),AGIDL_PPMGetClrFmt(ppm));
	}
	else{
		AGIDL_DrawImgData(vram,width,height,tx,ty,ppm->pixels.pix32,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),AGIDL_PPMGetClrFmt(ppm));
	}
}

void AGIDL_DrawLBM(void* vram, u32 width, u32 height, int tx, int ty, AGIDL_LBM* lbm){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 16){
		AGIDL_DrawImgData(vram,width,height,tx,ty,lbm->pixels.pix16,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),AGIDL_LBMGetClrFmt(lbm));
	}
	else{
		AGIDL_DrawImgData(vram,width,height,tx,ty,lbm->pixels.pix32,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),AGIDL_LBMGetClrFmt(lbm));
	}
}

void AGIDL_DispScaleBMP(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty, AGIDL_BMP* bmp){
	if(AGIDL_GetBitCount(AGIDL_BMPGetClrFmt(bmp)) == 16){
		AGIDL_DispScaleImgData(vram,width,height,sx,sy,tx,ty,bmp->pixels.pix16,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),AGIDL_BMPGetClrFmt(bmp));
	}
	else{
		AGIDL_DispScaleImgData(vram,width,height,sx,sy,tx,ty,bmp->pixels.pix32,AGIDL_BMPGetWidth(bmp),AGIDL_BMPGetHeight(bmp),AGIDL_BMPGetClrFmt(bmp));
	}
}

void AGIDL_DispScaleTGA(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty, AGIDL_TGA* tga){
	if(AGIDL_GetBitCount(AGIDL_TGAGetClrFmt(tga)) == 16){
		AGIDL_DispScaleImgData(vram,width,height,sx,sy,tx,ty,tga->pixels.pix16,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),AGIDL_TGAGetClrFmt(tga));
	}
	else{
		AGIDL_DispScaleImgData(vram,width,height,sx,sy,tx,ty,tga->pixels.pix32,AGIDL_TGAGetWidth(tga),AGIDL_TGAGetHeight(tga),AGIDL_TGAGetClrFmt(tga));
	}
}

void AGIDL_DispScaleTIM(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty, AGIDL_TIM* tim){
	if(AGIDL_GetBitCount(AGIDL_TIMGetClrFmt(tim)) == 16){
		AGIDL_DispScaleImgData(vram,width,height,sx,sy,tx,ty,tim->pixels.pix16,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),AGIDL_TIMGetClrFmt(tim));
	}
	else{
		AGIDL_DispScaleImgData(vram,width,height,sx,sy,tx,ty,tim->pixels.pix32,AGIDL_TIMGetWidth(tim),AGIDL_TIMGetHeight(tim),AGIDL_TIMGetClrFmt(tim));
	}
}

void AGIDL_DispScalePCX(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty, AGIDL_PCX* pcx){
	if(AGIDL_GetBitCount(AGIDL_PCXGetClrFmt(pcx)) == 16){
		AGIDL_DispScaleImgData(vram,width,height,sx,sy,tx,ty,pcx->pixels.pix16,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),AGIDL_PCXGetClrFmt(pcx));
	}
	else{
		AGIDL_DispScaleImgData(vram,width,height,sx,sy,tx,ty,pcx->pixels.pix32,AGIDL_PCXGetWidth(pcx),AGIDL_PCXGetHeight(pcx),AGIDL_PCXGetClrFmt(pcx));
	}
}

void AGIDL_DispScaleLMP(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty, AGIDL_LMP* lmp){
	if(AGIDL_GetBitCount(AGIDL_LMPGetClrFmt(lmp)) == 16){
		AGIDL_DispScaleImgData(vram,width,height,sx,sy,tx,ty,lmp->pixels.pix16,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),AGIDL_LMPGetClrFmt(lmp));
	}
	else{
		AGIDL_DispScaleImgData(vram,width,height,sx,sy,tx,ty,lmp->pixels.pix32,AGIDL_LMPGetWidth(lmp),AGIDL_LMPGetHeight(lmp),AGIDL_LMPGetClrFmt(lmp));
	}
}

void AGIDL_DispScalePVR(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty, AGIDL_PVR* pvr){
	if(AGIDL_GetBitCount(AGIDL_PVRGetClrFmt(pvr)) == 16){
		AGIDL_DispScaleImgData(vram,width,height,sx,sy,tx,ty,pvr->pixels.pix16,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr));
	}
	else{
		AGIDL_DispScaleImgData(vram,width,height,sx,sy,tx,ty,pvr->pixels.pix32,AGIDL_PVRGetWidth(pvr),AGIDL_PVRGetHeight(pvr),AGIDL_PVRGetClrFmt(pvr));
	}
}

void AGIDL_DispScaleGXT(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty, AGIDL_GXT* gxt){
	if(AGIDL_GetBitCount(AGIDL_GXTGetClrFmt(gxt)) == 16){
		AGIDL_DispScaleImgData(vram,width,height,sx,sy,tx,ty,gxt->pixels.pix16,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),AGIDL_GXTGetClrFmt(gxt));
	}
	else{
		AGIDL_DispScaleImgData(vram,width,height,sx,sy,tx,ty,gxt->pixels.pix32,AGIDL_GXTGetWidth(gxt),AGIDL_GXTGetHeight(gxt),AGIDL_GXTGetClrFmt(gxt));
	}
}

void AGIDL_DispScaleBTI(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty, AGIDL_BTI* bti){
	if(AGIDL_GetBitCount(AGIDL_BTIGetClrFmt(bti)) == 16){
		AGIDL_DispScaleImgData(vram,width,height,sx,sy,tx,ty,bti->pixels.pix16,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),AGIDL_BTIGetClrFmt(bti));
	}
	else{
		AGIDL_DispScaleImgData(vram,width,height,sx,sy,tx,ty,bti->pixels.pix32,AGIDL_BTIGetWidth(bti),AGIDL_BTIGetHeight(bti),AGIDL_BTIGetClrFmt(bti));
	}
}

void AGIDL_DispScale3DF(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty, AGIDL_3DF* glide){
	if(AGIDL_GetBitCount(AGIDL_3DFGetClrFmt(glide)) == 16){
		AGIDL_DispScaleImgData(vram,width,height,sx,sy,tx,ty,glide->pixels.pix16,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
	}
	else{
		AGIDL_DispScaleImgData(vram,width,height,sx,sy,tx,ty,glide->pixels.pix32,AGIDL_3DFGetWidth(glide),AGIDL_3DFGetHeight(glide),AGIDL_3DFGetClrFmt(glide));
	}
}

void AGIDL_DispScalePPM(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty, AGIDL_PPM* ppm){
	if(AGIDL_GetBitCount(AGIDL_PPMGetClrFmt(ppm)) == 16){
		AGIDL_DispScaleImgData(vram,width,height,sx,sy,tx,ty,ppm->pixels.pix16,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),AGIDL_PPMGetClrFmt(ppm));
	}
	else{
		AGIDL_DispScaleImgData(vram,width,height,sx,sy,tx,ty,ppm->pixels.pix32,AGIDL_PPMGetWidth(ppm),AGIDL_PPMGetHeight(ppm),AGIDL_PPMGetClrFmt(ppm));
	}
}

void AGIDL_DispScaleLBM(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty, AGIDL_LBM* lbm){
	if(AGIDL_GetBitCount(AGIDL_LBMGetClrFmt(lbm)) == 16){
		AGIDL_DispScaleImgData(vram,width,height,sx,sy,tx,ty,lbm->pixels.pix16,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),AGIDL_LBMGetClrFmt(lbm));
	}
	else{
		AGIDL_DispScaleImgData(vram,width,height,sx,sy,tx,ty,lbm->pixels.pix32,AGIDL_LBMGetWidth(lbm),AGIDL_LBMGetHeight(lbm),AGIDL_LBMGetClrFmt(lbm));
	}
}

 