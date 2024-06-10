/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_cc_manager.c
*   Date: 9/8/2023
*   Version: 0.1b
*   Updated: 4/19/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_cc_manager.h>

#include <stdlib.h>
#include <tgmath.h>

#include <agidl_cc_converter.h>
#include <agidl_file_utils.h>
#include <agidl_img_error.h>
#include <agidl_img_types.h>
#include <agidl_math_utils.h>

u32 AGIDL_AcquireRedMask(const AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_RGBA_8888:{
			return 0xff000000;
		}
		case AGIDL_ARGB_8888:{
			return  0xff0000;
		}break;
		case AGIDL_RGB_888:{
			return 0xff0000;
		}
		case AGIDL_BGR_888:{
			return 0xff;
		}
		case AGIDL_RGB_555:{
			return 0x7c00;
		}
		case AGIDL_BGR_555:{
			return 0x1f;
		}
		case AGIDL_RGB_565:{
			return 0xf800;
		}
		case AGIDL_BGR_565:{
			return 0x1f;
		}
		default: return 0;
	}
}

u32 AGIDL_AcquireGreenMask(const AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_RGBA_8888:{
			return 0xff0000;
		}
		case AGIDL_ARGB_8888:{
			return 0xff00;
		}
		case AGIDL_RGB_888:{
			return 0xff00;
		}
		case AGIDL_BGR_888:{
			return 0xff00;
		}
		case AGIDL_RGB_555:{
			return 0x3e0;
		}
		case AGIDL_BGR_555:{
			return 0x3e0;
		}
		case AGIDL_RGB_565:{
			return 0x7e0;
		}
		case AGIDL_BGR_565:{
			return 0x7e0;
		}
		default: return 0;
	}
}

u32 AGIDL_AcquireBlueMask(const AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_RGBA_8888:{
			return 0xff00;
		}
		case AGIDL_ARGB_8888:{
			return 0xff000000;
		}
		case AGIDL_RGB_888:{
			return 0xff;
		}
		case AGIDL_BGR_888:{
			return 0xff0000;
		}
		case AGIDL_RGB_555:{
			return 0x1f;
		}
		case AGIDL_BGR_555:{
			return 0x7c00;
		}
		case AGIDL_RGB_565:{
			return 0x1f;
		}
		case AGIDL_BGR_565:{
			return 0xf800;
		}
		default: return 0;
	}
}

u32 AGIDL_AcquireColorMask(const AGIDL_CLR_FMT fmt, const AGIDL_CLR_COMPONENT component){
	switch(component){
		case CC_R:{
			return AGIDL_AcquireRedMask(fmt);
		}break;
		case CC_G:{
			return AGIDL_AcquireGreenMask(fmt);
		}break;
		case CC_B:{
			return AGIDL_AcquireBlueMask(fmt);
		}break;
		default: return 0;
	}
}

u8 AGIDL_GetR(const COLOR clr, const AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_RGB_888:{
			return ((clr & 0xff0000) >> 16);
		}break;
		case AGIDL_BGR_888:{
			return ((clr & 0xff));
		}break;
		case AGIDL_RGBA_8888:{
			return ((clr >> 24) & 0xff);
		}break;
		case AGIDL_ARGB_8888:{
			return ((clr >> 16) & 0xff);
		}break;
		case AGIDL_RGB_555:{
			return ((clr & 0x7C00) >> 10);
		}break;
		case AGIDL_BGR_555:{
			return ((clr & 0x001F));
		}break;
		case AGIDL_RGB_565:{
			return ((clr & 0xF800) >> 11);
		}break;
		case AGIDL_BGR_565:{
			return ((clr & 0x001F));
		}break;
		default: return 0;
	}
}

u8 AGIDL_GetG(const COLOR clr, const AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_RGB_888:{
			return ((clr & 0xff00) >> 8);
		}break;
		case AGIDL_BGR_888:{
			return ((clr & 0xff00) >> 8);
		}break;
		case AGIDL_RGBA_8888:{
			return ((clr & 0xff0000) >> 16);
		}break;
		case AGIDL_ARGB_8888:{
			return ((clr & 0xff) >> 8);
		}break;
		case AGIDL_RGB_555:{
			return ((clr & 0x3E0) >> 5);
		}break;
		case AGIDL_BGR_555:{
			return((clr & 0x3E0) >> 5);
		}break;
		case AGIDL_RGB_565:{
			return ((clr & 0x7E0) >> 5);
		}break;
		case AGIDL_BGR_565:{
			return((clr & 0x7E0) >> 5);
		}break;
		default: return 0;
	}
}

u8 AGIDL_GetB(const COLOR clr, const AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_RGB_888:{
			return ((clr & 0xff));
		}break;
		case AGIDL_BGR_888:{
			return ((clr & 0xff0000) >> 16);
		}break;
		case AGIDL_RGBA_8888:{
			return ((clr & 0xff00) >> 8);
		}break;
		case AGIDL_ARGB_8888:{
			return ((clr & 0xff));
		}break;
		case AGIDL_RGB_555:{
			return ((clr & 0x001F));
		}break;
		case AGIDL_BGR_555:{
			return ((clr & 0x7C00) >> 10);
		}break;
		case AGIDL_RGB_565:{
			return ((clr & 0x001F));
		}break;
		case AGIDL_BGR_565:{
			return ((clr & 0xF800) >> 11);
		}break;
		default: return 0;
	}
}

u8 AGIDL_GetA(const COLOR clr, const AGIDL_CLR_FMT fmt){
	if(fmt == AGIDL_RGBA_8888){
		return ((clr & 0xff));
	}
	if(fmt == AGIDL_ARGB_8888){
		return clr >> 24;
	}
	return 0;
}

COLOR AGIDL_SetR(const COLOR clr, const u8 newR, const AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_RGB_888:{
			const u8 r = newR;
			const u8 g = AGIDL_GetG(clr,fmt);
			const u8 b = AGIDL_GetB(clr,fmt);
			return AGIDL_RGB(r,g,b,fmt);
		}break;
		case AGIDL_BGR_888:{
			const u8 r = newR;
			const u8 g = AGIDL_GetG(clr,fmt);
			const u8 b = AGIDL_GetB(clr,fmt);
			return AGIDL_RGB(r,g,b,fmt);
		}break;
		case AGIDL_RGBA_8888:{
			const u8 r = newR;
			const u8 g = AGIDL_GetG(clr,fmt);
			const u8 b = AGIDL_GetB(clr,fmt);
			const u8 a = AGIDL_GetA(clr,fmt);
			return AGIDL_RGBA(r,g,b,a,fmt);
		}break;
		case AGIDL_ARGB_8888:{
			const u8 r = newR;
			const u8 g = AGIDL_GetG(clr,fmt);
			const u8 b = AGIDL_GetB(clr,fmt);
			const u8 a = AGIDL_GetA(clr,fmt);
			return AGIDL_RGBA(r,g,b,a,fmt);
		}break;
		case AGIDL_RGB_555:{
			const u8 r = AGIDL_Clamp(0,newR,31);
			const u8 g = AGIDL_GetG(clr,fmt);
			const u8 b = AGIDL_GetB(clr,fmt);
			return AGIDL_RGB(r,g,b,fmt);
		}break;
		case AGIDL_BGR_555:{
			const u8 r = AGIDL_Clamp(0,newR,31);
			const u8 g = AGIDL_GetG(clr,fmt);
			const u8 b = AGIDL_GetB(clr,fmt);
			return AGIDL_RGB(r,g,b,fmt);
		}break;
		case AGIDL_RGB_565:{
			const u8 r = AGIDL_Clamp(0,newR,31);
			const u8 g = AGIDL_GetG(clr,fmt);
			const u8 b = AGIDL_GetB(clr,fmt);
			return AGIDL_RGB(r,g,b,fmt);
		}break;
		case AGIDL_BGR_565:{
			const u8 r = AGIDL_Clamp(0,newR,31);
			const u8 g = AGIDL_GetG(clr,fmt);
			const u8 b = AGIDL_GetB(clr,fmt);
			return AGIDL_RGB(r,g,b,fmt);
		}break;
		default: return clr;
	}
}

COLOR AGIDL_SetG(const COLOR clr, const u8 newG, const AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_RGB_888:{
			const u8 r = AGIDL_GetR(clr,fmt);
			const u8 g = newG;
			const u8 b = AGIDL_GetB(clr,fmt);
			return AGIDL_RGB(r,g,b,fmt);
		}break;
		case AGIDL_BGR_888:{
			const u8 r = AGIDL_GetR(clr,fmt);
			const u8 g = newG;
			const u8 b = AGIDL_GetB(clr,fmt);
			return AGIDL_RGB(r,g,b,fmt);
		}break;
		case AGIDL_RGBA_8888:{
			const u8 r = AGIDL_GetR(clr,fmt);
			const u8 g = newG;
			const u8 b = AGIDL_GetB(clr,fmt);
			const u8 a = AGIDL_GetA(clr,fmt);
			return AGIDL_RGBA(r,g,b,a,fmt);
		}break;
		case AGIDL_ARGB_8888:{
			const u8 r = AGIDL_GetR(clr,fmt);
			const u8 g = newG;
			const u8 b = AGIDL_GetB(clr,fmt);
			const u8 a = AGIDL_GetA(clr,fmt);
			return AGIDL_RGBA(r,g,b,a,fmt);
		}break;
		case AGIDL_RGB_555:{
			const u8 r = AGIDL_GetR(clr,fmt);
			const u8 g = AGIDL_Clamp(0,newG,31);
			const u8 b = AGIDL_GetB(clr,fmt);
			return AGIDL_RGB(r,g,b,fmt);
		}break;
		case AGIDL_BGR_555:{
			const u8 r = AGIDL_GetR(clr,fmt);
			const u8 g = AGIDL_Clamp(0,newG,31);
			const u8 b = AGIDL_GetB(clr,fmt);
			return AGIDL_RGB(r,g,b,fmt);
		}break;
		case AGIDL_RGB_565:{
			const u8 r = AGIDL_GetR(clr,fmt);
			const u8 g = AGIDL_Clamp(0,newG,63);
			const u8 b = AGIDL_GetB(clr,fmt);
			return AGIDL_RGB(r,g,b,fmt);
		}break;
		case AGIDL_BGR_565:{
			const u8 r = AGIDL_GetR(clr,fmt);
			const u8 g = AGIDL_Clamp(0,newG,63);
			const u8 b = AGIDL_GetB(clr,fmt);
			return AGIDL_RGB(r,g,b,fmt);
		}break;
		default: return clr;
	}
}

COLOR AGIDL_SetB(const COLOR clr, const u8 newB, const AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_RGB_888:{
			const u8 r = AGIDL_GetR(clr,fmt);
			const u8 g = AGIDL_GetG(clr,fmt);
			const u8 b = newB;
			return AGIDL_RGB(r,g,b,fmt);
		}break;
		case AGIDL_BGR_888:{
			const u8 r = AGIDL_GetR(clr,fmt);
			const u8 g = AGIDL_GetG(clr,fmt);
			const u8 b = newB;
			return AGIDL_RGB(r,g,b,fmt);
		}break;
		case AGIDL_RGBA_8888:{
			const u8 r = AGIDL_GetR(clr,fmt);
			const u8 g = AGIDL_GetG(clr,fmt);
			const u8 b = newB;
			const u8 a = AGIDL_GetA(clr,fmt);
			return AGIDL_RGBA(r,g,b,a,fmt);
		}break;
		case AGIDL_ARGB_8888:{
			const u8 r = AGIDL_GetR(clr,fmt);
			const u8 g = AGIDL_GetG(clr,fmt);
			const u8 b = newB;
			const u8 a = AGIDL_GetA(clr,fmt);
			return AGIDL_RGBA(r,g,b,a,fmt);
		}break;
		case AGIDL_RGB_555:{
			const u8 r = AGIDL_GetR(clr,fmt);
			const u8 g = AGIDL_GetG(clr,fmt);
			const u8 b = AGIDL_Clamp(0,newB,31);
			return AGIDL_RGB(r,g,b,fmt);
		}break;
		case AGIDL_BGR_555:{
			const u8 r = AGIDL_GetR(clr,fmt);
			const u8 g = AGIDL_GetG(clr,fmt);
			const u8 b = AGIDL_Clamp(0,newB,31);
			return AGIDL_RGB(r,g,b,fmt);
		}break;
		case AGIDL_RGB_565:{
			const u8 r = AGIDL_GetR(clr,fmt);
			const u8 g = AGIDL_GetG(clr,fmt);
			const u8 b = AGIDL_Clamp(0,newB,31);
			return AGIDL_RGB(r,g,b,fmt);
		}break;
		case AGIDL_BGR_565:{
			const u8 r = AGIDL_GetR(clr,fmt);
			const u8 g = AGIDL_GetG(clr,fmt);
			const u8 b = AGIDL_Clamp(0,newB,31);
			return AGIDL_RGB(r,g,b,fmt);
		}break;
		default: return clr;
	}
}

COLOR AGIDL_SetA(const COLOR clr, const u8 newA, const AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_RGBA_8888:{
			const u8 r = AGIDL_GetR(clr,fmt);
			const u8 g = AGIDL_GetG(clr,fmt);
			const u8 b = AGIDL_GetB(clr,fmt);
			return AGIDL_RGBA(r,g,b,newA,fmt);
		}break;
		case AGIDL_ARGB_8888:{
			const u8 r = AGIDL_GetR(clr,fmt);
			const u8 g = AGIDL_GetG(clr,fmt);
			const u8 b = AGIDL_GetB(clr,fmt);
			return AGIDL_RGBA(r,g,b,newA,fmt);
		}break;
		default: return clr;
	}
}

COLOR AGIDL_RGB(u8 r, u8 g, u8 b, const AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_RGB_888:{
			return r << 16 | g << 8 | b;
		}break;
		case AGIDL_BGR_888:{
			return b << 16 | g << 8 | r;
		}break;
		case AGIDL_RGB_555:{
			r = AGIDL_Clamp(0,r,31);
			g = AGIDL_Clamp(0,g,31);
			b = AGIDL_Clamp(0,b,31);
			return r << 10 | g << 5 | b;
		}break;
		case AGIDL_BGR_555:{
			r = AGIDL_Clamp(0,r,31);
			g = AGIDL_Clamp(0,g,31);
			b = AGIDL_Clamp(0,b,31);
			return b << 10 | g << 5 | r;
		}break;
		case AGIDL_RGB_565:{
			r = AGIDL_Clamp(0,r,31);
			g = AGIDL_Clamp(0,g,63);
			b = AGIDL_Clamp(0,b,31);
			return r << 11 | g << 5 | b;
		}break;
		case AGIDL_BGR_565:{
			r = AGIDL_Clamp(0,r,31);
			g = AGIDL_Clamp(0,g,63);
			b = AGIDL_Clamp(0,b,31);
			return b << 11 | g << 5 | r;
		}break;
		case AGIDL_RGBA_8888:{
			return AGIDL_RGBA(r,g,b,0xff,fmt);
		}break;
		case AGIDL_ARGB_8888:{
			return AGIDL_RGBA(r,g,b,0xff,fmt);
		}break;
		default: return 0;
	}
}

COLOR AGIDL_RGBA(const u8 r, const u8 g, const u8 b, const u8 a, const AGIDL_CLR_FMT fmt){
	if(fmt == AGIDL_RGBA_8888){
		return r << 24 | g << 16 | b << 8 | a;
	}
	return a << 24 | r << 16 | g << 8 | b;
}

COLOR16 AGIDL_RGB16(u8 r, u8 g, u8 b, const AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_RGB_555:{
			r = AGIDL_Clamp(0,r,31);
			g = AGIDL_Clamp(0,g,31);
			b = AGIDL_Clamp(0,b,31);
			return r << 10 | g << 5 | b;
		}break;
		case AGIDL_BGR_555:{
			r = AGIDL_Clamp(0,r,31);
			g = AGIDL_Clamp(0,g,31);
			b = AGIDL_Clamp(0,b,31);
			return b << 10 | g << 5 | r;
		}break;
		case AGIDL_RGB_565:{
			r = AGIDL_Clamp(0,r,31);
			g = AGIDL_Clamp(0,g,63);
			b = AGIDL_Clamp(0,b,31);
			return r << 11 | g << 5 | b;
		}break;
		case AGIDL_BGR_565:{
			r = AGIDL_Clamp(0,r,31);
			g = AGIDL_Clamp(0,g,63);
			b = AGIDL_Clamp(0,b,31);
			return b << 11 | g << 5 | r;
		}break;
		default: return 0;
	}
}

COLOR AGIDL_Color3f(float r, float g, float b, const AGIDL_CLR_FMT fmt){
	r = AGIDL_Clampf(0.0f,r,1.0f);
	g = AGIDL_Clampf(0.0f,g,1.0f);
	b = AGIDL_Clampf(0.0f,b,1.0f);

	if(AGIDL_GetBitCount(fmt) == 24){
		return AGIDL_RGB(r*255.0f,g*255.0f,b*255.0f,fmt);
	}
	if(AGIDL_GetBitCount(fmt) == 32){
		return AGIDL_RGBA(r*255.0f,g*255.0f,b*255.0f,0xff,fmt);
	}
	if(fmt == AGIDL_RGB_565 || fmt == AGIDL_BGR_565){
		return AGIDL_RGB16(r*31.0f,g*63.0f,b*31.0f,fmt);
	}
	return AGIDL_RGB16(r*31.0f,g*31.0f,b*31.0f,fmt);
}

COLOR AGIDL_Color4f(float r, float g, float b, float a, const AGIDL_CLR_FMT fmt){
	r = AGIDL_Clampf(0.0f,r,1.0f);
	g = AGIDL_Clampf(0.0f,g,1.0f);
	b = AGIDL_Clampf(0.0f,b,1.0f);
	a = AGIDL_Clampf(0.0f,a,1.0f);

	if(AGIDL_GetBitCount(fmt) == 32){
		return AGIDL_RGBA(r*255.0f,g*255.0f,b*255.0f,a*255.0f,fmt);
	}
	return AGIDL_Color3f(r,g,b,fmt);
}

COLOR AGIDL_GetColor(const AGIDL_CLR clr, const AGIDL_CLR_FMT fmt){
	switch(clr){
		case BLACK:{
			switch(fmt){
				case AGIDL_RGB_888:{
					return BLACK_RGB_888;
				}break;
				case AGIDL_BGR_888:{
					return BLACK_BGR_888;
				}break;
				case AGIDL_RGBA_8888:{
					return BLACK_RGBA_8888;
				}break;
				case AGIDL_ARGB_8888:{
					return BLACK_ARGB_8888;
				}break;
				case AGIDL_RGB_555:{
					return BLACK_RGB_555;
				}break;
				case AGIDL_BGR_555:{
					return BLACK_BGR_555;
				}break;
				case AGIDL_RGB_565:{
					return BLACK_RGB_565;
				}break;
				case AGIDL_BGR_565:{
					return BLACK_BGR_565;
				}break;
			}
		}break;
		case WHITE:{
			switch(fmt){
				case AGIDL_RGB_888:{
					return WHITE_RGB_888;
				}break;
				case AGIDL_BGR_888:{
					return WHITE_BGR_888;
				}break;
				case AGIDL_RGBA_8888:{
					return WHITE_RGBA_8888;
				}break;
				case AGIDL_ARGB_8888:{
					return WHITE_ARGB_8888;
				}break;
				case AGIDL_RGB_555:{
					return WHITE_RGB_555;
				}break;
				case AGIDL_BGR_555:{
					return WHITE_BGR_555;
				}break;
				case AGIDL_RGB_565:{
					return WHITE_RGB_565;
				}break;
				case AGIDL_BGR_565:{
					return WHITE_BGR_565;
				}break;
			}
		}break;
		case GRAY:{
			switch(fmt){
				case AGIDL_RGB_888:{
					return GRAY_RGB_888;
				}break;
				case AGIDL_BGR_888:{
					return GRAY_BGR_888;
				}break;
				case AGIDL_RGBA_8888:{
					return GRAY_RGBA_8888;
				}break;
				case AGIDL_ARGB_8888:{
					return GRAY_ARGB_8888;
				}break;
				case AGIDL_RGB_555:{
					return GRAY_RGB_555;
				}break;
				case AGIDL_BGR_555:{
					return GRAY_BGR_555;
				}break;
				case AGIDL_RGB_565:{
					return GRAY_RGB_565;
				}break;
				case AGIDL_BGR_565:{
					return GRAY_BGR_565;
				}break;
			}
		}break;
		case RED:{
			switch(fmt){
				case AGIDL_RGB_888:{
					return RED_RGB_888;
				}break;
				case AGIDL_BGR_888:{
					return RED_BGR_888;
				}break;
				case AGIDL_RGBA_8888:{
					return RED_RGBA_8888;
				}break;
				case AGIDL_ARGB_8888:{
					return RED_ARGB_8888;
				}break;
				case AGIDL_RGB_555:{
					return RED_RGB_555;
				}break;
				case AGIDL_BGR_555:{
					return RED_BGR_555;
				}break;
				case AGIDL_RGB_565:{
					return RED_RGB_565;
				}break;
				case AGIDL_BGR_565:{
					return RED_BGR_565;
				}break;
			}
		}break;
		case GREEN:{
			switch(fmt){
				case AGIDL_RGB_888:{
					return GREEN_RGB_888;
				}break;
				case AGIDL_BGR_888:{
					return GREEN_BGR_888;
				}break;
				case AGIDL_RGBA_8888:{
					return GREEN_RGBA_8888;
				}break;
				case AGIDL_ARGB_8888:{
					return GREEN_ARGB_8888;
				}break;
				case AGIDL_RGB_555:{
					return GREEN_RGB_555;
				}break;
				case AGIDL_BGR_555:{
					return GREEN_BGR_555;
				}break;
				case AGIDL_RGB_565:{
					return GREEN_RGB_565;
				}break;
				case AGIDL_BGR_565:{
					return GREEN_BGR_565;
				}break;
			}
		}break;
		case BLUE:{
			switch(fmt){
				case AGIDL_RGB_888:{
					return BLUE_RGB_888;
				}break;
				case AGIDL_BGR_888:{
					return BLUE_BGR_888;
				}break;
				case AGIDL_RGBA_8888:{
					return BLUE_RGBA_8888;
				}break;
				case AGIDL_ARGB_8888:{
					return BLUE_ARGB_8888;
				}break;
				case AGIDL_RGB_555:{
					return BLUE_RGB_555;
				}break;
				case AGIDL_BGR_555:{
					return BLUE_BGR_555;
				}break;
				case AGIDL_RGB_565:{
					return BLUE_RGB_565;
				}break;
				case AGIDL_BGR_565:{
					return BLUE_BGR_565;
				}break;
			}
		}break;
		case YELLOW:{
			switch(fmt){
				case AGIDL_RGB_888:{
					return YELLOW_RGB_888;
				}break;
				case AGIDL_BGR_888:{
					return YELLOW_BGR_888;
				}break;
				case AGIDL_RGBA_8888:{
					return YELLOW_RGBA_8888;
				}break;
				case AGIDL_ARGB_8888:{
					return YELLOW_ARGB_8888;
				}break;
				case AGIDL_RGB_555:{
					return YELLOW_RGB_555;
				}break;
				case AGIDL_BGR_555:{
					return YELLOW_BGR_555;
				}break;
				case AGIDL_RGB_565:{
					return YELLOW_RGB_565;
				}break;
				case AGIDL_BGR_565:{
					return YELLOW_BGR_565;
				}break;
			}
		}break;
		case ORANGE:{
			switch(fmt){
				case AGIDL_RGB_888:{
					return ORANGE_RGB_888;
				}break;
				case AGIDL_BGR_888:{
					return ORANGE_BGR_888;
				}break;
				case AGIDL_RGBA_8888:{
					return ORANGE_RGBA_8888;
				}break;
				case AGIDL_ARGB_8888:{
					return ORANGE_ARGB_8888;
				}break;
				case AGIDL_RGB_555:{
					return ORANGE_RGB_555;
				}break;
				case AGIDL_BGR_555:{
					return ORANGE_BGR_555;
				}break;
				case AGIDL_RGB_565:{
					return ORANGE_RGB_565;
				}break;
				case AGIDL_BGR_565:{
					return ORANGE_BGR_565;
				}break;
			}
		}break;
		case PURPLE:{
			switch(fmt){
				case AGIDL_RGB_888:{
					return PURPLE_RGB_888;
				}break;
				case AGIDL_BGR_888:{
					return PURPLE_BGR_888;
				}break;
				case AGIDL_RGBA_8888:{
					return PURPLE_RGBA_8888;
				}break;
				case AGIDL_ARGB_8888:{
					return PURPLE_ARGB_8888;
				}break;
				case AGIDL_RGB_555:{
					return PURPLE_RGB_555;
				}break;
				case AGIDL_BGR_555:{
					return PURPLE_BGR_555;
				}break;
				case AGIDL_RGB_565:{
					return PURPLE_RGB_565;
				}break;
				case AGIDL_BGR_565:{
					return PURPLE_BGR_565;
				}break;
			}
		}break;
		case BROWN:{
			switch(fmt){
				case AGIDL_RGB_888:{
					return BROWN_RGB_888;
				}break;
				case AGIDL_BGR_888:{
					return BROWN_BGR_888;
				}break;
				case AGIDL_RGBA_8888:{
					return BROWN_RGBA_8888;
				}break;
				case AGIDL_ARGB_8888:{
					return BROWN_ARGB_8888;
				}break;
				case AGIDL_RGB_555:{
					return BROWN_RGB_555;
				}break;
				case AGIDL_BGR_555:{
					return BROWN_BGR_555;
				}break;
				case AGIDL_RGB_565:{
					return BROWN_RGB_565;
				}break;
				case AGIDL_BGR_565:{
					return BROWN_BGR_565;
				}break;
			}
		}break;
		case PINK:{
			switch(fmt){
				case AGIDL_RGB_888:{
					return PINK_RGB_888;
				}break;
				case AGIDL_BGR_888:{
					return PINK_BGR_888;
				}break;
				case AGIDL_RGBA_8888:{
					return PINK_RGBA_8888;
				}break;
				case AGIDL_ARGB_8888:{
					return PINK_ARGB_8888;
				}break;
				case AGIDL_RGB_555:{
					return PINK_RGB_555;
				}break;
				case AGIDL_BGR_555:{
					return PINK_BGR_555;
				}break;
				case AGIDL_RGB_565:{
					return PINK_RGB_565;
				}break;
				case AGIDL_BGR_565:{
					return PINK_BGR_565;
				}break;
			}
		}break;
	}
	return 0;
}

COLOR AGIDL_GammaCorrectColor(const COLOR clr, const f32 gamma, const AGIDL_CLR_FMT fmt){
	switch(AGIDL_GetBitCount(fmt)){
		case 24:{
			u8 r = AGIDL_GetR(clr,fmt);
			u8 g = AGIDL_GetG(clr,fmt);
			u8 b = AGIDL_GetB(clr,fmt);

			r = pow((r/255.0f),1/gamma)*255;
			g = pow((g/255.0f),1/gamma)*255;
			b = pow((b/255.0f),1/gamma)*255;
			return AGIDL_RGB(r,g,b,fmt);
		}break;
		case 16:{
			u8 r = AGIDL_GetR(clr,fmt);
			u8 g = AGIDL_GetG(clr,fmt);
			u8 b = AGIDL_GetB(clr,fmt);

			r = pow((r/31.0f),1/gamma)*31;
			g = pow((g/31.0f),1/gamma)*31;
			b = pow((b/31.0f),1/gamma)*31;
			return AGIDL_RGB16(r,g,b,fmt);
		}break;
		case 32:{
			u8 r = AGIDL_GetR(clr,fmt);
			u8 g = AGIDL_GetG(clr,fmt);
			u8 b = AGIDL_GetB(clr,fmt);
			const u8 a = AGIDL_GetA(clr,fmt);

			r = pow((r/255.0f),1/gamma)*255;
			g = pow((g/255.0f),1/gamma)*255;
			b = pow((b/255.0f),1/gamma)*255;
			return AGIDL_RGBA(r,g,b,a,fmt);
		}break;
	}
	return 0;
}

COLOR AGIDL_DarkenColor(const COLOR clr, const AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) != 32){
		u8 r = AGIDL_GetR(clr,fmt);
		u8 g = AGIDL_GetG(clr,fmt);
		u8 b = AGIDL_GetB(clr,fmt);

		r *= 0.75;
		g *= 0.75;
		b *= 0.75;

		return AGIDL_RGB(r,g,b,fmt);
	}
	u8 r = AGIDL_GetR(clr,fmt);
	u8 g = AGIDL_GetG(clr,fmt);
	u8 b = AGIDL_GetB(clr,fmt);
	const u8 a = AGIDL_GetA(clr,fmt);

	r *= 0.75;
	g *= 0.75;
	b *= 0.75;

	return AGIDL_RGBA(r,g,b,a,fmt);
}

COLOR AGIDL_BrightenColor(const COLOR clr, const AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) != 32){
		u8 r = AGIDL_GetR(clr,fmt);
		u8 g = AGIDL_GetG(clr,fmt);
		u8 b = AGIDL_GetB(clr,fmt);

		r *= 1.25;
		g *= 1.25;
		b *= 1.25;

		return AGIDL_RGB(r,g,b,fmt);
	}
	u8 r = AGIDL_GetR(clr,fmt);
	u8 g = AGIDL_GetG(clr,fmt);
	u8 b = AGIDL_GetB(clr,fmt);
	const u8 a = AGIDL_GetA(clr,fmt);

	r *= 1.25;
	g *= 1.25;
	b *= 1.25;

	return AGIDL_RGBA(r,g,b,a,fmt);
}

int AGIDL_IsInThreshold(COLOR clr1, COLOR clr2, const AGIDL_CLR_FMT fmt, const AGIDL_CLR_FMT fmt2, const u8 max_diff){
	if((AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32) && (AGIDL_GetBitCount(fmt2) == 24 || AGIDL_GetBitCount(fmt2) == 32)){
		const u8 r = AGIDL_GetR(clr1,fmt);
		const u8 g = AGIDL_GetG(clr1,fmt);
		const u8 b = AGIDL_GetB(clr1,fmt);

		const u8 r2 = AGIDL_GetR(clr2,fmt2);
		const u8 g2 = AGIDL_GetG(clr2,fmt2);
		const u8 b2 = AGIDL_GetB(clr2,fmt2);

		if(AGIDL_Abs(r-r2) <= max_diff && AGIDL_Abs(g-g2) <= max_diff && AGIDL_Abs(b-b2) <= max_diff){
			return 0;
		}
		return 1;
	}
	if(AGIDL_GetBitCount(fmt) == 16 && (AGIDL_GetBitCount(fmt2) == 24 || AGIDL_GetBitCount(fmt2) == 32)){
		clr2 = AGIDL_CLR_TO_CLR16(clr2,fmt2,fmt);

		const u8 r = AGIDL_GetR(clr1,fmt);
		const u8 g = AGIDL_GetG(clr1,fmt);
		const u8 b = AGIDL_GetB(clr1,fmt);

		const u8 r2 = AGIDL_GetR(clr2,fmt);
		const u8 g2 = AGIDL_GetG(clr2,fmt);
		const u8 b2 = AGIDL_GetB(clr2,fmt);

		if(AGIDL_Abs(r-r2) <= max_diff && AGIDL_Abs(g-g2) <= max_diff && AGIDL_Abs(b-b2) <= max_diff){
			return 0;
		}
		return 1;
	}
	clr1 = AGIDL_CLR_TO_CLR16(clr1,fmt,fmt2);

	const u8 r = AGIDL_GetR(clr1,fmt2);
	const u8 g = AGIDL_GetG(clr1,fmt2);
	const u8 b = AGIDL_GetB(clr1,fmt2);

	const u8 r2 = AGIDL_GetR(clr2,fmt2);
	const u8 g2 = AGIDL_GetG(clr2,fmt2);
	const u8 b2 = AGIDL_GetB(clr2,fmt2);

	if(AGIDL_Abs(r-r2) <= max_diff && AGIDL_Abs(g-g2) <= max_diff && AGIDL_Abs(b-b2) <= max_diff){
		return 0;
	}
	return 1;
}

void AGIDL_SetICPMode(AGIDL_ICP* palette, const int mode, const AGIDL_CLR_FMT fmt){
	palette->mode = mode;
	palette->fmt = fmt;
}

void AGIDL_ClearICP(AGIDL_ICP* palette, const COLOR clr){
	switch(palette->mode){
		case AGIDL_ICP_256:{
			for(int i = 0; i < 256; i++){
				palette->icp.palette_256[i] = clr;
			}
		}break;
		case AGIDL_ICP_16:{
			for(int i = 0; i < 16; i++){
				palette->icp.palette_16[i] = clr;
			}
		}break;
		case AGIDL_ICP_16b_256:{
			for(int i = 0; i < 256; i++){
				palette->icp.palette_16b_256[i] = (COLOR16)clr;
			}
		}break;
		case AGIDL_ICP_16b_16:{
			for(int i = 0; i < 16; i++){
				palette->icp.palette_16b_16[i] = (COLOR16)clr;
			}
		}break;
	}
}

void AGIDL_ClearColorICP(AGIDL_ICP* palette, const float r, const float g, const float b){
	switch(palette->mode){
		case AGIDL_ICP_256:{
			for(int i = 0; i < 256; i++){
				palette->icp.palette_256[i] = AGIDL_Color3f(r,g,b,palette->mode);
			}
		}break;
		case AGIDL_ICP_16:{
			for(int i = 0; i < 16; i++){
				palette->icp.palette_16[i] = AGIDL_Color3f(r,g,b,palette->mode);
			}
		}break;
		case AGIDL_ICP_16b_256:{
			for(int i = 0; i < 256; i++){
				palette->icp.palette_16b_256[i] = (COLOR16)AGIDL_Color3f(r,g,b,palette->mode);
			}
		}break;
		case AGIDL_ICP_16b_16:{
			for(int i = 0; i < 16; i++){
				palette->icp.palette_16b_16[i] = (COLOR16)AGIDL_Color3f(r,g,b,palette->mode);
			}
		}break;
	}
}

void AGIDL_ClearRGBICP(AGIDL_ICP* palette, const u8 r, const u8 g, const u8 b){
	switch(palette->mode){
		case AGIDL_ICP_256:{
			for(int i = 0; i < 256; i++){
				palette->icp.palette_256[i] = AGIDL_RGB(r,g,b,palette->mode);
			}
		}break;
		case AGIDL_ICP_16:{
			for(int i = 0; i < 16; i++){
				palette->icp.palette_16[i] = AGIDL_RGB(r,g,b,palette->mode);
			}
		}break;
		case AGIDL_ICP_16b_256:{
			for(int i = 0; i < 256; i++){
				palette->icp.palette_16b_256[i] = (COLOR16)AGIDL_RGB(r,g,b,palette->mode);
			}
		}break;
		case AGIDL_ICP_16b_16:{
			for(int i = 0; i < 16; i++){
				palette->icp.palette_16b_16[i] = (COLOR16)AGIDL_RGB(r,g,b,palette->mode);
			}
		}break;
	}
}

AGIDL_CLR_MDL AGIDL_GetClrMDL(const AGIDL_CLR_FMT fmt){
	if(fmt == AGIDL_RGB_888 || fmt == AGIDL_RGB_555 || fmt == AGIDL_RGB_565){
		return AGIDL_CLR_RGB;
	}
	if(fmt == AGIDL_BGR_888 || fmt == AGIDL_BGR_555 || fmt == AGIDL_BGR_565){
		return AGIDL_CLR_BGR;
	}
	if(fmt == AGIDL_RGBA_8888){
		return AGIDL_CLR_RGBA;
	}
	return AGIDL_CLR_ARGB;
}

AGIDL_CLR_FMT AGIDL_GetClrFmt(const AGIDL_CLR_MDL mdl, const AGIDL_BITS bits){
	if(mdl == AGIDL_CLR_RGB && bits == 16){
		return AGIDL_RGB_565;
	}
	if(mdl == AGIDL_CLR_RGB && bits == 15){
		return AGIDL_RGB_555;
	}
	if(mdl == AGIDL_CLR_BGR && bits == 16){
		return AGIDL_BGR_565;
	}
	if(mdl == AGIDL_CLR_RGB && bits == 15){
		return AGIDL_BGR_555;
	}
	if(mdl == AGIDL_CLR_RGB && bits == 24){
		return AGIDL_RGB_888;
	}
	if(mdl == AGIDL_CLR_BGR && bits == 24){
		return AGIDL_BGR_888;
	}
	if(mdl == AGIDL_CLR_RGBA && bits == 32){
		return AGIDL_RGBA_8888;
	}
	return AGIDL_ARGB_8888;
}

void AGIDL_ExportICP(const char* name, const AGIDL_ICP icp){
	char* filename = AGIDL_StrCpy(name,".icp");
	FILE* file = fopen(filename,"wb");

	const u8 clrmdl = AGIDL_GetClrMDL(icp.fmt);
	const u8 bits = AGIDL_GetBitCount(icp.fmt);

	AGIDL_WriteByte(file,clrmdl);
	AGIDL_WriteByte(file,bits);

	u16 num_of_clrs = 0;

	if(icp.mode == AGIDL_ICP_256 || icp.mode == AGIDL_ICP_16b_256){
		num_of_clrs = 256;
	}
	else{
		num_of_clrs = 16;
	}

	AGIDL_WriteShort(file,num_of_clrs);

	switch(icp.mode){
		case AGIDL_ICP_256:{
			for(int i = 0; i < 256; i++){
				const COLOR clr = icp.icp.palette_256[i];
				AGIDL_WriteRGB(file,clr,icp.fmt,icp.fmt);
			}
		}break;
		case AGIDL_ICP_16:{
			for(int i = 0; i < 16; i++){
				const COLOR clr = icp.icp.palette_256[i];
				AGIDL_WriteRGB(file,clr,icp.fmt,icp.fmt);
			}
		}break;
		case AGIDL_ICP_16b_256:{
			for(int i = 0; i < 256; i++){
				const COLOR16 clr = icp.icp.palette_16b_256[i];
				AGIDL_WriteShort(file,clr);
			}
		}break;
		case AGIDL_ICP_16b_16:{
			for(int i = 0; i < 16; i++){
				const COLOR16 clr = icp.icp.palette_16b_256[i];
				AGIDL_WriteShort(file,clr);
			}
		}break;
	}
	fclose(file);

	free(filename);
}

AGIDL_ICP AGIDL_LoadICP(const char* filename){
	FILE* file = fopen(filename,"rb");

	AGIDL_ICP icp = {};

	if(file == NULL){
		printf("%s - %s!\n",AGIDL_Error2Str(FILE_NOT_LOCATED_IMG_ERROR),filename);
		return icp;
	}

	const u8 clrmdl = AGIDL_ReadByte(file);
	const u8 bits = AGIDL_ReadByte(file);
	const u16 num_of_colors = AGIDL_ReadShort(file);

	if(!(bits == 16 || bits == 24 || bits == 32) || num_of_colors <= 0 || num_of_colors > 256 ||
	!(clrmdl == AGIDL_CLR_RGB || clrmdl == AGIDL_CLR_BGR || clrmdl == AGIDL_CLR_RGBA)){
		printf("%s - %s\n",AGIDL_Error2Str(INVALID_IMG_FORMATTING_ERROR),filename);
		return icp;
	}

	icp.fmt = AGIDL_GetClrFmt(clrmdl,bits);

	if(num_of_colors == 256 && bits != 16){
		icp.mode = AGIDL_ICP_256;

		for(int i = 0; i < num_of_colors; i++){
			icp.icp.palette_256[i] = AGIDL_ReadRGB(file,icp.fmt);
		}
	}
	else if(num_of_colors == 16 && bits != 16){
		icp.mode = AGIDL_ICP_16;

		for(int i = 0; i < num_of_colors; i++){
			icp.icp.palette_16[i] = AGIDL_ReadRGB(file,icp.fmt);
		}
	}
	else if(num_of_colors == 16 && bits != 256){
		icp.mode = AGIDL_ICP_16b_256;

		for(int i = 0; i < num_of_colors; i++){
			icp.icp.palette_16b_256[i] = AGIDL_ReadShort(file);
		}
	}
	else{
		icp.mode = AGIDL_ICP_16b_16;

		for(int i = 0; i < num_of_colors; i++){
			icp.icp.palette_16b_16[i] = AGIDL_ReadShort(file);
		}
	}

	fclose(file);

	return icp;
}

void AGIDL_InitICP(AGIDL_ICP *palette, const int mode){
	palette->mode = mode;

	if(mode == AGIDL_ICP_16){
		for(int i = 0; i < 16; i++){
			palette->icp.palette_16[i] = 0;
		}
	}
	else if(mode == AGIDL_ICP_256){
		for(int i = 0; i < 256; i++){
			palette->icp.palette_256[i] = 0;
		}
	}
	else if(mode == AGIDL_ICP_16b_16){
		for(int i = 0; i < 16; i++){
			palette->icp.palette_16b_16[i] = 0;
		}
	}
	else{
		for(int i = 0; i < 256; i++){
			palette->icp.palette_16b_256[i] = 0;
		}
	}
}

void QSwap(u32* a, u32* b){
	const u32 temp = *a;
	*a = *b;
	*b = temp;
}

int partition(u32* data, u32* gram, const int low, const int high)
{
    // choose the pivot
    const int pivot = data[high];

    // Index of smaller element and Indicate
    // the right position of pivot found so far
    int i = (low - 1);
    for (int j = low; j <= high; j++) {
        // If current element is smaller than the pivot
        if (data[j] < pivot) {
            // Increment index of smaller element
            i++;
            QSwap(&data[i], &data[j]);
			QSwap(&gram[i], &gram[j]);
        }
    }
    QSwap(&data[i + 1], &data[high]);
	QSwap(&gram[i + 1], &gram[high]);
    return (i + 1);
}

void quickSort(u32* data, u32* gram, const int low, const int high)
{
    // when low is less than high
    if (low < high) {
        // pi is the partition return index of pivot

        const int pi = partition(data, gram, low, high);

        // Recursion Call
        // smaller element than pivot goes left and
        // higher element goes right
        quickSort(data, gram, low, pi - 1);
        quickSort(data, gram, pi + 1, high);
    }
}

void AGIDL_EncodeHistogramICP(AGIDL_ICP* palette, const void* data, const u32 width, const u32 height, const AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 16){
		const u16* clr_data = data;

		u32 histogram[MAX_HIGH_CLR];
		u32 colorgram[MAX_HIGH_CLR];

		int i;
		for(i = 0; i < MAX_HIGH_CLR; i++){
			histogram[i] = 1;
			colorgram[i] = i;
		}

		//ITERATE THROUGH ENTIRE BUFFER OF IMAGE DATA AND TRACK ITS NUMBER OF OCCURENCES IN A HISTOGRAM
		for(i = 0; i < width*height; i++){
			const u32 color16 = clr_data[i];
			histogram[color16] = histogram[color16] + 1;
		}

		//PERFORM QUICK SORT TO LIST THE 256 MOST IMPORTANT COLORS IN ORDER OF LEAST TO MOST FREQUENTLY OCCURING COLORS
		quickSort(histogram,colorgram,0,MAX_HIGH_CLR);


		AGIDL_InitICP(palette,AGIDL_ICP_16b_256);

		int count = 0;
		for(i = MAX_HIGH_CLR; i > 0; i--){
			AGIDL_Bool skip = FALSE;

			const u16 clr = colorgram[i];

			const int r = AGIDL_GetR(clr,fmt);
			const int g = AGIDL_GetG(clr,fmt);
			const int b = AGIDL_GetB(clr,fmt);

			for(int j = 0; j < 256; j++){
				const u16 palclr = palette->icp.palette_16b_256[j];

				const int palr = AGIDL_GetR(palclr,fmt);
				const int palg = AGIDL_GetG(palclr,fmt);
				const int palb = AGIDL_GetB(palclr,fmt);

				int rdiff = r-palr;
				int gdiff = g-palg;
				int bdiff = b-palb;

				if(rdiff < 0){
					rdiff += (rdiff*rdiff);
				}

				if(gdiff < 0){
					gdiff += (gdiff*gdiff);
				}

				if(bdiff < 0){
					bdiff += (bdiff*bdiff);
				}

				if(rdiff <= 1 && gdiff <= 1 && bdiff <= 1){
					skip = TRUE;
				}
			}

			if(skip == FALSE){
				palette->icp.palette_16b_256[count] = clr;
				count++;
			}

			if(count >= 255){
				break;
			}
		}

		palette->fmt = fmt;
	}
	else{
		const u32* clr_data = data;

		u32 histogram[32767];
		u32 colorgram[32767];

		int i;
		for(i = 0; i < 32767; i++){
			histogram[i] = 1;
			colorgram[i] = i;
		}

		//ITERATE THROUGH ENTIRE BUFFER OF IMAGE DATA AND TRACK ITS NUMBER OF OCCURENCES IN A HISTOGRAM
		for(i = 0; i < width*height; i++){
			const u32 color = clr_data[i];
			const u16 color16 = AGIDL_CLR_TO_CLR16(color,fmt,AGIDL_RGB_555);
			histogram[color16] = histogram[color16] + 1;
		}

		//PERFORM QUICK SORT TO LIST THE 256 MOST IMPORTANT COLORS IN ORDER OF LEAST TO MOST FREQUENTLY OCCURING COLORS
		quickSort(histogram,colorgram,0,32767);


		AGIDL_InitICP(palette,AGIDL_ICP_256);

		int count = 0;
		for(i = 32767; i > 0; i--){
			AGIDL_Bool skip = FALSE;

			const u16 clr = colorgram[i];

			const int r = AGIDL_GetR(clr,AGIDL_RGB_555);
			const int g = AGIDL_GetG(clr,AGIDL_RGB_555);
			const int b = AGIDL_GetB(clr,AGIDL_RGB_555);

			for(int j = 0; j < 256; j++){
				const u16 palclr = palette->icp.palette_256[j];

				const int palr = AGIDL_GetR(palclr,AGIDL_RGB_555);
				const int palg = AGIDL_GetG(palclr,AGIDL_RGB_555);
				const int palb = AGIDL_GetB(palclr,AGIDL_RGB_555);

				int rdiff = r-palr;
				int gdiff = g-palg;
				int bdiff = b-palb;

				if(rdiff < 0){
					rdiff += (rdiff*rdiff);
				}

				if(gdiff < 0){
					gdiff += (gdiff*gdiff);
				}

				if(bdiff < 0){
					bdiff += (bdiff*bdiff);
				}
				
				if(rdiff <= 1 && gdiff <= 1 && bdiff <= 1){
					skip = TRUE;
				}
			}
			
			if(skip == FALSE){
				palette->icp.palette_256[count] = clr;
				count++;
			}
			
			if(count >= 255){
				break;
			}
		}

		for(int j = 0; j < 256; j++){
			palette->icp.palette_256[j] = AGIDL_CLR16_TO_CLR(palette->icp.palette_256[j],AGIDL_RGB_555,fmt);
		}
	}
	
	palette->fmt = fmt;
}

void AGIDL_AddColorICP(AGIDL_ICP *palette, const u8 index, const COLOR clr, const AGIDL_CLR_FMT fmt, const int max_diff, int *pass){
	/*Extract Individual RGB components of clr*/
	const u8 r = AGIDL_GetR(clr,fmt);
	const u8 g = AGIDL_GetG(clr,fmt);
	const u8 b = AGIDL_GetB(clr,fmt);
	
	switch(palette->mode){
		case AGIDL_ICP_16:{
			for(int i = 0; i < 16; i++){
				const COLOR palclr = palette->icp.palette_16[i];
				const u8 palr = AGIDL_GetR(palclr,fmt);
				const u8 palg = AGIDL_GetG(palclr,fmt);
				const u8 palb = AGIDL_GetB(palclr,fmt);
				if(AGIDL_Abs(r-palr) <= max_diff && AGIDL_Abs(g-palg) <= max_diff && AGIDL_Abs(b-palb) <= max_diff){
					*pass = 0;
					return;
				}
			}
			
			palette->icp.palette_16[index] = clr;
			
			*pass = 1;
		}break;
		case AGIDL_ICP_256:{
			for(int i = 0; i < 256; i++){
				const COLOR palclr = palette->icp.palette_256[i];
				const u8 palr = AGIDL_GetR(palclr,fmt);
				const u8 palg = AGIDL_GetG(palclr,fmt);
				const u8 palb = AGIDL_GetB(palclr,fmt);
				if(AGIDL_Abs(r-palr) <= max_diff && AGIDL_Abs(g-palg) <= max_diff && AGIDL_Abs(b-palb) <= max_diff){
					*pass = 0;
					return;
				}
			}
			
			palette->icp.palette_256[index] = clr;
			
			*pass = 1;
		}break;
		case AGIDL_ICP_16b_16:{
			for(int i = 0; i < 16; i++){
				const COLOR16 palclr = palette->icp.palette_16b_16[i];
				const u8 palr = AGIDL_GetR(palclr,fmt);
				const u8 palg = AGIDL_GetG(palclr,fmt);
				const u8 palb = AGIDL_GetB(palclr,fmt);
				if(AGIDL_Abs(r-palr) <= max_diff && AGIDL_Abs(g-palg) <= max_diff && AGIDL_Abs(b-palb) <= max_diff){
					*pass = 0;
					return;
				}
			}
			
			palette->icp.palette_16b_16[index] = (COLOR16)clr;
			
			*pass = 1;
		}break;
		case AGIDL_ICP_16b_256:{
			for(int i = 0; i < 256; i++){
				const COLOR16 palclr = palette->icp.palette_16b_256[i];
				const u8 palr = AGIDL_GetR(palclr,fmt);
				const u8 palg = AGIDL_GetG(palclr,fmt);
				const u8 palb = AGIDL_GetB(palclr,fmt);
				if(AGIDL_Abs(r-palr) <= max_diff && AGIDL_Abs(g-palg) <= max_diff && AGIDL_Abs(b-palb) <= max_diff){
					*pass = 0;
					return;
				}
			}
			
			palette->icp.palette_16b_256[index] = (COLOR16)clr;
			
			*pass = 1;
		}break;
	}
}

void AGIDL_AddColorICP16(AGIDL_ICP *palette, const u8 index, const COLOR16 clr, const AGIDL_CLR_FMT fmt, const int max_diff, int *pass){
	/*Extract Individual RGB components of clr*/
	const u8 r = AGIDL_GetR(clr,fmt);
	const u8 g = AGIDL_GetG(clr,fmt);
	const u8 b = AGIDL_GetB(clr,fmt);
	switch(palette->mode){
		case AGIDL_ICP_16b_16:{
			for(int i = 0; i < 16; i++){
				const COLOR16 palclr = palette->icp.palette_16b_16[i];
				const u8 palr = AGIDL_GetR(palclr,fmt);
				const u8 palg = AGIDL_GetG(palclr,fmt);
				const u8 palb = AGIDL_GetB(palclr,fmt);
				if(AGIDL_Abs(r-palr) <= max_diff && AGIDL_Abs(g-palg) <= max_diff && AGIDL_Abs(b-palb) <= max_diff){
					*pass = 0;
					return;
				}
			}
			
			palette->icp.palette_16b_16[index] = clr;
			
			*pass = 1;
		}break;
		case AGIDL_ICP_16b_256:{
			for(int i = 0; i < 256; i++){
				const COLOR palclr = palette->icp.palette_16b_256[i];
				const u8 palr = AGIDL_GetR(palclr,fmt);
				const u8 palg = AGIDL_GetG(palclr,fmt);
				const u8 palb = AGIDL_GetB(palclr,fmt);
				if(AGIDL_Abs(r-palr) <= max_diff && AGIDL_Abs(g-palg) <= max_diff && AGIDL_Abs(b-palb) <= max_diff){
					*pass = 0;
					return;
				}
			}
			
			palette->icp.palette_16b_256[index] = clr;
			
			*pass = 1;
		}break;
	}
}

void AGIDL_ForceAddColor(AGIDL_ICP* palette, const COLOR clr, u8 index){
	if(palette->mode == AGIDL_ICP_16 || palette->mode == AGIDL_ICP_16b_16){
		index = AGIDL_Max(index,16);
		
		if(palette->mode == AGIDL_ICP_16){
			palette->icp.palette_16[index] = (COLOR16)clr;
		}
		else{
			palette->icp.palette_16b_16[index] = (COLOR16)clr;
		}
	}
	else{
		if(palette->mode == AGIDL_ICP_256){
			palette->icp.palette_256[index] = clr;
		}
		else{
			palette->icp.palette_16b_256[index] = clr;
		}
	}
}

u8 AGIDL_FindClosestColor(const AGIDL_ICP palette, const COLOR clr, const AGIDL_CLR_FMT fmt, const int max_difference){
	/*Extract Individual RGB Components of Color*/
	const u8 r = AGIDL_GetR(clr,fmt);
	const u8 g = AGIDL_GetG(clr,fmt);
	const u8 b = AGIDL_GetB(clr,fmt);
	
	switch(palette.mode){
		case AGIDL_ICP_256:{
			for(int i = 0; i < 256; i++){
				const COLOR palclr = palette.icp.palette_256[i];

				const u8 palr = AGIDL_GetR(palclr,fmt);
				const u8 palg = AGIDL_GetG(palclr,fmt);
				const u8 palb = AGIDL_GetB(palclr,fmt);
				
				if(AGIDL_Abs(r-palr) <= max_difference && AGIDL_Abs(g-palg) <= max_difference && AGIDL_Abs(b-palb) <= max_difference){
					return i;
				}
				
				if(i == 255){
					return i;
				}
			}
		}break;
		case AGIDL_ICP_16:{
			for(int i = 0; i < 16; i++){
				const COLOR palclr = palette.icp.palette_16[i];

				const u8 palr = AGIDL_GetR(palclr,fmt);
				const u8 palg = AGIDL_GetG(palclr,fmt);
				const u8 palb = AGIDL_GetB(palclr,fmt);
				
				if(AGIDL_Abs(r-palr) <= max_difference && AGIDL_Abs(g-palg) <= max_difference && AGIDL_Abs(b-palb) <= max_difference){
					return i;
				}
			}
		}break;
		case AGIDL_ICP_16b_256:{
			for(int i = 0; i < 256; i++){
				const COLOR16 palclr = palette.icp.palette_16b_256[i];

				const u8 palr = AGIDL_GetR(palclr,fmt);
				const u8 palg = AGIDL_GetG(palclr,fmt);
				const u8 palb = AGIDL_GetB(palclr,fmt);
				
				if(AGIDL_Abs(r-palr) <= max_difference && AGIDL_Abs(g-palg) <= max_difference && AGIDL_Abs(b-palb) <= max_difference){
					return i;
				}
			}
		}break;
		case AGIDL_ICP_16b_16:{
			for(int i = 0; i < 16; i++){
				const COLOR16 palclr = palette.icp.palette_16b_16[i];

				const u8 palr = AGIDL_GetR(palclr,fmt);
				const u8 palg = AGIDL_GetG(palclr,fmt);
				const u8 palb = AGIDL_GetB(palclr,fmt);
				
				if(AGIDL_Abs(r-palr) <= max_difference && AGIDL_Abs(g-palg) <= max_difference && AGIDL_Abs(b-palb) <= max_difference){
					return i;
				}
			}
		}break;
		default: break;
	}
	return 0;
}

u8 AGIDL_FindNearestColor(const AGIDL_ICP palette, const COLOR clr, const AGIDL_CLR_FMT fmt){
	const int r = AGIDL_GetR(clr,fmt);
	const int g = AGIDL_GetG(clr,fmt);
	const int b = AGIDL_GetB(clr,fmt);
	
	switch(palette.mode){
		case AGIDL_ICP_256:{
			u32 min = 255*255 + 255*255 + 255*255 + 1;
			u8 index = 0;
			for(int i = 0; i < 256; i++){
				const COLOR palclr = palette.icp.palette_256[i];

				const int palr = AGIDL_GetR(palclr,fmt);
				const int palg = AGIDL_GetG(palclr,fmt);
				const int palb = AGIDL_GetB(palclr,fmt);

				const int rdiff = r-palr;
				const int gdiff = g-palg;
				const int bdiff = b-palb;

				const u32 dist = rdiff*rdiff + gdiff*gdiff + bdiff*bdiff;
				
				if(dist < min){
					min = dist;
					index = i;
				}
			}
			return index;
		}
		case AGIDL_ICP_16:{
			u32 min = 255*255 + 255*255 + 255*255 + 1;
			u8 index = 0;

			for(int i = 0; i < 16; i++){
				const COLOR palclr = palette.icp.palette_16[i];

				const int palr = AGIDL_GetR(palclr,fmt);
				const int palg = AGIDL_GetG(palclr,fmt);
				const int palb = AGIDL_GetB(palclr,fmt);

				const int rdiff = AGIDL_Abs(r-palr);
				const int gdiff = AGIDL_Abs(g-palg);
				const int bdiff = AGIDL_Abs(b-palb);

				const u32 dist = rdiff*rdiff + gdiff*gdiff + bdiff*bdiff;
				
				if(dist < min){
					min = dist;
					index = i;
				}
			}
			return index;
		}
		case AGIDL_ICP_16b_256:{
			u32 min = 31*31 + 31*31 + 31*31 + 1;
			u8 index = 0;

			for(int i = 0; i < 256; i++){
				const COLOR16 palclr = palette.icp.palette_16b_256[i];

				const int palr = AGIDL_GetR(palclr,fmt);
				const int palg = AGIDL_GetG(palclr,fmt);
				const int palb = AGIDL_GetB(palclr,fmt);

				const int rdiff = AGIDL_Abs(r-palr);
				const int gdiff = AGIDL_Abs(g-palg);
				const int bdiff = AGIDL_Abs(b-palb);

				const u32 dist = rdiff*rdiff + gdiff*gdiff + bdiff*bdiff;
				
				if(dist < min){
					min = dist;
					index = i;
				}
			}
			return index;
		}
		case AGIDL_ICP_16b_16:{
			u32 min = 31*31 + 31*31 + 31*31 + 1;
			u8 index = 0;

			for(int i = 0; i < 16; i++){
				const COLOR16 palclr = palette.icp.palette_16b_16[i];

				const int palr = AGIDL_GetR(palclr,fmt);
				const int palg = AGIDL_GetG(palclr,fmt);
				const int palb = AGIDL_GetB(palclr,fmt);

				const int rdiff = AGIDL_Abs(r-palr);
				const int gdiff = AGIDL_Abs(g-palg);
				const int bdiff = AGIDL_Abs(b-palb);

				const u32 dist = rdiff*rdiff + gdiff*gdiff + bdiff*bdiff;
				
				if(dist < min){
					min = dist;
					index = i;
				}
			}
			
			return index;
		}
	}
	
	return 0;
}

AGIDL_ICP AGIDL_GenerateVGAICP(){
	COLOR icp[256];
	
	icp[0] = AGIDL_RGB(0,0,0,AGIDL_RGB_888); icp[1] = AGIDL_RGB(0,0,170,AGIDL_RGB_888); icp[2] = AGIDL_RGB(0,170,0,AGIDL_RGB_888); icp[3] = AGIDL_RGB(0,170,170,AGIDL_RGB_888); icp[4] = AGIDL_RGB(170,0,0,AGIDL_RGB_888); icp[5] = AGIDL_RGB(170,0,170,AGIDL_RGB_888); icp[6] = AGIDL_RGB(170,85,0,AGIDL_RGB_888); icp[7] = AGIDL_RGB(172,172,172,AGIDL_RGB_888); icp[8] = AGIDL_RGB(86,86,86,AGIDL_RGB_888); icp[9] = AGIDL_RGB(24,24,77,AGIDL_RGB_888); icp[10] = AGIDL_RGB(2,0,15,AGIDL_RGB_888); icp[11] = AGIDL_RGB(73,218,73,AGIDL_RGB_888); icp[12] = AGIDL_RGB(88,255,255,AGIDL_RGB_888); icp[13] = AGIDL_RGB(255,85,85,AGIDL_RGB_888); icp[14] = AGIDL_RGB(255,85,255,AGIDL_RGB_888); icp[15] = AGIDL_RGB(255,255,85,AGIDL_RGB_888); icp[16] = AGIDL_RGB(255,255,255,AGIDL_RGB_888); 
	icp[17] = AGIDL_RGB(0,0,0,AGIDL_RGB_888); icp[18] = AGIDL_RGB(16,16,16,AGIDL_RGB_888); icp[19] = AGIDL_RGB(32,32,32,AGIDL_RGB_888); icp[20] = AGIDL_RGB(53,53,53,AGIDL_RGB_888); icp[21] = AGIDL_RGB(69,69,69,AGIDL_RGB_888); icp[22] = AGIDL_RGB(85,85,85,AGIDL_RGB_888); icp[23] = AGIDL_RGB(101,101,101,AGIDL_RGB_888); icp[24] = AGIDL_RGB(118,118,118,AGIDL_RGB_888); icp[25] = AGIDL_RGB(139,139,139,AGIDL_RGB_888); icp[26] = AGIDL_RGB(44,44,44,AGIDL_RGB_888); icp[27] = AGIDL_RGB(3,3,3,AGIDL_RGB_888); icp[28] = AGIDL_RGB(145,145,145,AGIDL_RGB_888); icp[29] = AGIDL_RGB(194,194,194,AGIDL_RGB_888); icp[30] = AGIDL_RGB(202,202,202,AGIDL_RGB_888); icp[31] = AGIDL_RGB(223,223,223,AGIDL_RGB_888); icp[32] = AGIDL_RGB(239,239,239,AGIDL_RGB_888); icp[33] = AGIDL_RGB(255,255,255,AGIDL_RGB_888); 
	icp[34] = AGIDL_RGB(0,0,255,AGIDL_RGB_888); icp[35] = AGIDL_RGB(65,0,255,AGIDL_RGB_888); icp[36] = AGIDL_RGB(130,0,255,AGIDL_RGB_888); icp[37] = AGIDL_RGB(190,0,255,AGIDL_RGB_888); icp[38] = AGIDL_RGB(255,0,255,AGIDL_RGB_888); icp[39] = AGIDL_RGB(255,0,190,AGIDL_RGB_888); icp[40] = AGIDL_RGB(255,0,130,AGIDL_RGB_888); icp[41] = AGIDL_RGB(255,0,66,AGIDL_RGB_888); icp[42] = AGIDL_RGB(255,0,0,AGIDL_RGB_888); icp[43] = AGIDL_RGB(72,20,0,AGIDL_RGB_888); icp[44] = AGIDL_RGB(6,0,0,AGIDL_RGB_888); icp[45] = AGIDL_RGB(218,111,0,AGIDL_RGB_888); icp[46] = AGIDL_RGB(255,198,0,AGIDL_RGB_888); icp[47] = AGIDL_RGB(255,255,0,AGIDL_RGB_888); icp[48] = AGIDL_RGB(190,255,0,AGIDL_RGB_888); icp[49] = AGIDL_RGB(130,255,0,AGIDL_RGB_888); icp[50] = AGIDL_RGB(65,255,0,AGIDL_RGB_888); 
	icp[51] = AGIDL_RGB(0,255,0,AGIDL_RGB_888); icp[52] = AGIDL_RGB(0,255,65,AGIDL_RGB_888); icp[53] = AGIDL_RGB(0,255,130,AGIDL_RGB_888); icp[54] = AGIDL_RGB(0,255,190,AGIDL_RGB_888); icp[55] = AGIDL_RGB(0,255,255,AGIDL_RGB_888); icp[56] = AGIDL_RGB(0,190,255,AGIDL_RGB_888); icp[57] = AGIDL_RGB(0,130,255,AGIDL_RGB_888); icp[58] = AGIDL_RGB(0,66,255,AGIDL_RGB_888); icp[59] = AGIDL_RGB(131,131,255,AGIDL_RGB_888); icp[60] = AGIDL_RGB(46,37,72,AGIDL_RGB_888); icp[61] = AGIDL_RGB(2,3,6,AGIDL_RGB_888); icp[62] = AGIDL_RGB(162,111,218,AGIDL_RGB_888); icp[63] = AGIDL_RGB(232,135,255,AGIDL_RGB_888); icp[64] = AGIDL_RGB(255,130,255,AGIDL_RGB_888); icp[65] = AGIDL_RGB(255,130,223,AGIDL_RGB_888); icp[66] = AGIDL_RGB(255,130,190,AGIDL_RGB_888); icp[67] = AGIDL_RGB(255,130,158,AGIDL_RGB_888); 
	icp[68] = AGIDL_RGB(255,130,130,AGIDL_RGB_888); icp[69] = AGIDL_RGB(255,158,130,AGIDL_RGB_888); icp[70] = AGIDL_RGB(255,190,130,AGIDL_RGB_888); icp[71] = AGIDL_RGB(255,223,130,AGIDL_RGB_888); icp[72] = AGIDL_RGB(255,255,130,AGIDL_RGB_888); icp[73] = AGIDL_RGB(223,255,130,AGIDL_RGB_888); icp[74] = AGIDL_RGB(190,255,130,AGIDL_RGB_888); icp[75] = AGIDL_RGB(160,255,132,AGIDL_RGB_888); icp[76] = AGIDL_RGB(131,255,131,AGIDL_RGB_888); icp[77] = AGIDL_RGB(37,72,46,AGIDL_RGB_888); icp[78] = AGIDL_RGB(3,6,2,AGIDL_RGB_888); icp[79] = AGIDL_RGB(111,218,162,AGIDL_RGB_888); icp[80] = AGIDL_RGB(135,255,232,AGIDL_RGB_888); icp[81] = AGIDL_RGB(130,255,255,AGIDL_RGB_888); icp[82] = AGIDL_RGB(130,223,255,AGIDL_RGB_888); icp[83] = AGIDL_RGB(130,190,255,AGIDL_RGB_888); icp[84] = AGIDL_RGB(130,158,255,AGIDL_RGB_888); 
	icp[85] = AGIDL_RGB(186,186,255,AGIDL_RGB_888); icp[86] = AGIDL_RGB(202,186,255,AGIDL_RGB_888); icp[87] = AGIDL_RGB(223,186,255,AGIDL_RGB_888); icp[88] = AGIDL_RGB(239,186,255,AGIDL_RGB_888); icp[89] = AGIDL_RGB(255,186,255,AGIDL_RGB_888); icp[90] = AGIDL_RGB(255,186,239,AGIDL_RGB_888); icp[91] = AGIDL_RGB(255,186,223,AGIDL_RGB_888); icp[92] = AGIDL_RGB(255,188,204,AGIDL_RGB_888); icp[93] = AGIDL_RGB(255,187,187,AGIDL_RGB_888); icp[94] = AGIDL_RGB(72,58,53,AGIDL_RGB_888); icp[95] = AGIDL_RGB(6,3,4,AGIDL_RGB_888); icp[96] = AGIDL_RGB(218,191,159,AGIDL_RGB_888); icp[97] = AGIDL_RGB(255,249,194,AGIDL_RGB_888); icp[98] = AGIDL_RGB(255,255,186,AGIDL_RGB_888); icp[99] = AGIDL_RGB(239,255,186,AGIDL_RGB_888); icp[100] = AGIDL_RGB(223,255,186,AGIDL_RGB_888); icp[101] = AGIDL_RGB(202,255,186,AGIDL_RGB_888); 
	icp[102] = AGIDL_RGB(186,255,186,AGIDL_RGB_888); icp[103] = AGIDL_RGB(186,255,203,AGIDL_RGB_888); icp[104] = AGIDL_RGB(186,255,224,AGIDL_RGB_888); icp[105] = AGIDL_RGB(186,255,240,AGIDL_RGB_888); icp[106] = AGIDL_RGB(186,255,255,AGIDL_RGB_888); icp[107] = AGIDL_RGB(186,240,255,AGIDL_RGB_888); icp[108] = AGIDL_RGB(186,224,255,AGIDL_RGB_888); icp[109] = AGIDL_RGB(188,205,255,AGIDL_RGB_888); icp[110] = AGIDL_RGB(0,0,114,AGIDL_RGB_888); icp[111] = AGIDL_RGB(9,0,32,AGIDL_RGB_888); icp[112] = AGIDL_RGB(0,0,3,AGIDL_RGB_888); icp[113] = AGIDL_RGB(49,0,97,AGIDL_RGB_888); icp[114] = AGIDL_RGB(88,0,118,AGIDL_RGB_888); icp[115] = AGIDL_RGB(113,0,113,AGIDL_RGB_888); icp[116] = AGIDL_RGB(113,0,85,AGIDL_RGB_888); icp[117] = AGIDL_RGB(113,0,57,AGIDL_RGB_888); icp[118] = AGIDL_RGB(113,0,28,AGIDL_RGB_888); 
	icp[119] = AGIDL_RGB(114,0,0,AGIDL_RGB_888); icp[120] = AGIDL_RGB(114,28,0,AGIDL_RGB_888); icp[121] = AGIDL_RGB(114,58,0,AGIDL_RGB_888); icp[122] = AGIDL_RGB(114,86,0,AGIDL_RGB_888); icp[123] = AGIDL_RGB(114,114,0,AGIDL_RGB_888); icp[124] = AGIDL_RGB(86,114,0,AGIDL_RGB_888); icp[125] = AGIDL_RGB(58,114,0,AGIDL_RGB_888); icp[126] = AGIDL_RGB(28,115,0,AGIDL_RGB_888); icp[127] = AGIDL_RGB(0,115,0,AGIDL_RGB_888); icp[128] = AGIDL_RGB(0,32,9,AGIDL_RGB_888); icp[129] = AGIDL_RGB(0,3,0,AGIDL_RGB_888); icp[130] = AGIDL_RGB(0,98,49,AGIDL_RGB_888); icp[131] = AGIDL_RGB(0,119,89,AGIDL_RGB_888); icp[132] = AGIDL_RGB(0,114,114,AGIDL_RGB_888); icp[133] = AGIDL_RGB(0,86,114,AGIDL_RGB_888); icp[134] = AGIDL_RGB(0,58,114,AGIDL_RGB_888); icp[135] = AGIDL_RGB(0,28,114,AGIDL_RGB_888); 
	icp[136] = AGIDL_RGB(6,3,14,AGIDL_RGB_888); icp[137] = AGIDL_RGB(7,3,13,AGIDL_RGB_888); icp[138] = AGIDL_RGB(10,3,12,AGIDL_RGB_888); icp[139] = AGIDL_RGB(11,3,12,AGIDL_RGB_888); icp[140] = AGIDL_RGB(14,3,11,AGIDL_RGB_888); icp[141] = AGIDL_RGB(14,4,9,AGIDL_RGB_888); icp[142] = AGIDL_RGB(14,4,7,AGIDL_RGB_888); icp[143] = AGIDL_RGB(14,5,4,AGIDL_RGB_888); icp[144] = AGIDL_RGB(13,4,3,AGIDL_RGB_888); icp[145] = AGIDL_RGB(4,2,1,AGIDL_RGB_888); icp[146] = AGIDL_RGB(0,0,0,AGIDL_RGB_888); icp[147] = AGIDL_RGB(10,7,2,AGIDL_RGB_888); icp[148] = AGIDL_RGB(12,11,3,AGIDL_RGB_888); icp[149] = AGIDL_RGB(11,13,3,AGIDL_RGB_888); icp[150] = AGIDL_RGB(9,13,3,AGIDL_RGB_888); icp[151] = AGIDL_RGB(7,13,4,AGIDL_RGB_888); icp[152] = AGIDL_RGB(4,13,4,AGIDL_RGB_888); 
	icp[153] = AGIDL_RGB(45,30,31,AGIDL_RGB_888); icp[154] = AGIDL_RGB(45,34,31,AGIDL_RGB_888); icp[155] = AGIDL_RGB(45,37,31,AGIDL_RGB_888); icp[156] = AGIDL_RGB(45,40,31,AGIDL_RGB_888); icp[157] = AGIDL_RGB(45,43,30,AGIDL_RGB_888); icp[158] = AGIDL_RGB(41,44,30,AGIDL_RGB_888); icp[159] = AGIDL_RGB(38,44,30,AGIDL_RGB_888); icp[160] = AGIDL_RGB(35,45,31,AGIDL_RGB_888); icp[161] = AGIDL_RGB(31,44,31,AGIDL_RGB_888); icp[162] = AGIDL_RGB(9,12,10,AGIDL_RGB_888); icp[163] = AGIDL_RGB(1,1,1,AGIDL_RGB_888); icp[164] = AGIDL_RGB(26,38,31,AGIDL_RGB_888); icp[165] = AGIDL_RGB(32,46,42,AGIDL_RGB_888); icp[166] = AGIDL_RGB(30,44,43,AGIDL_RGB_888); icp[167] = AGIDL_RGB(30,41,44,AGIDL_RGB_888); icp[168] = AGIDL_RGB(30,37,44,AGIDL_RGB_888); icp[169] = AGIDL_RGB(30,34,44,AGIDL_RGB_888); 
	icp[170] = AGIDL_RGB(0,0,69,AGIDL_RGB_888); icp[171] = AGIDL_RGB(17,0,69,AGIDL_RGB_888); icp[172] = AGIDL_RGB(34,0,69,AGIDL_RGB_888); icp[173] = AGIDL_RGB(52,0,69,AGIDL_RGB_888); icp[174] = AGIDL_RGB(69,0,69,AGIDL_RGB_888); icp[175] = AGIDL_RGB(69,0,52,AGIDL_RGB_888); icp[176] = AGIDL_RGB(69,0,34,AGIDL_RGB_888); icp[177] = AGIDL_RGB(70,0,17,AGIDL_RGB_888); icp[178] = AGIDL_RGB(70,0,0,AGIDL_RGB_888); icp[179] = AGIDL_RGB(20,5,0,AGIDL_RGB_888); icp[180] = AGIDL_RGB(1,0,0,AGIDL_RGB_888); icp[181] = AGIDL_RGB(59,29,0,AGIDL_RGB_888); icp[182] = AGIDL_RGB(72,54,0,AGIDL_RGB_888); icp[183] = AGIDL_RGB(69,69,0,AGIDL_RGB_888); icp[184] = AGIDL_RGB(52,69,0,AGIDL_RGB_888); icp[185] = AGIDL_RGB(34,69,0,AGIDL_RGB_888); icp[186] = AGIDL_RGB(17,69,0,AGIDL_RGB_888); 
	icp[187] = AGIDL_RGB(0,65,0,AGIDL_RGB_888); icp[188] = AGIDL_RGB(0,65,16,AGIDL_RGB_888); icp[189] = AGIDL_RGB(0,65,32,AGIDL_RGB_888); icp[190] = AGIDL_RGB(0,65,49,AGIDL_RGB_888); icp[191] = AGIDL_RGB(0,65,65,AGIDL_RGB_888); icp[192] = AGIDL_RGB(0,49,65,AGIDL_RGB_888); icp[193] = AGIDL_RGB(0,32,65,AGIDL_RGB_888); icp[194] = AGIDL_RGB(0,16,66,AGIDL_RGB_888); icp[195] = AGIDL_RGB(32,32,66,AGIDL_RGB_888); icp[196] = AGIDL_RGB(12,9,19,AGIDL_RGB_888); icp[197] = AGIDL_RGB(0,1,1,AGIDL_RGB_888); icp[198] = AGIDL_RGB(42,27,56,AGIDL_RGB_888); icp[199] = AGIDL_RGB(59,33,68,AGIDL_RGB_888); icp[200] = AGIDL_RGB(65,32,65,AGIDL_RGB_888); icp[201] = AGIDL_RGB(65,32,57,AGIDL_RGB_888); icp[202] = AGIDL_RGB(65,32,49,AGIDL_RGB_888); icp[203] = AGIDL_RGB(65,32,40,AGIDL_RGB_888); 
	icp[204] = AGIDL_RGB(65,32,32,AGIDL_RGB_888); icp[205] = AGIDL_RGB(65,40,32,AGIDL_RGB_888); icp[206] = AGIDL_RGB(65,49,32,AGIDL_RGB_888); icp[207] = AGIDL_RGB(65,57,32,AGIDL_RGB_888); icp[208] = AGIDL_RGB(65,65,32,AGIDL_RGB_888); icp[209] = AGIDL_RGB(57,65,32,AGIDL_RGB_888); icp[210] = AGIDL_RGB(49,65,32,AGIDL_RGB_888); icp[211] = AGIDL_RGB(40,66,32,AGIDL_RGB_888); icp[212] = AGIDL_RGB(32,66,32,AGIDL_RGB_888); icp[213] = AGIDL_RGB(9,19,12,AGIDL_RGB_888); icp[214] = AGIDL_RGB(1,1,0,AGIDL_RGB_888); icp[215] = AGIDL_RGB(27,56,42,AGIDL_RGB_888); icp[216] = AGIDL_RGB(33,68,59,AGIDL_RGB_888); icp[217] = AGIDL_RGB(32,65,65,AGIDL_RGB_888); icp[218] = AGIDL_RGB(32,57,65,AGIDL_RGB_888); icp[219] = AGIDL_RGB(32,49,65,AGIDL_RGB_888); icp[220] = AGIDL_RGB(32,40,65,AGIDL_RGB_888); 
	icp[221] = AGIDL_RGB(45,45,65,AGIDL_RGB_888); icp[222] = AGIDL_RGB(49,45,65,AGIDL_RGB_888); icp[223] = AGIDL_RGB(53,45,65,AGIDL_RGB_888); icp[224] = AGIDL_RGB(61,45,65,AGIDL_RGB_888); icp[225] = AGIDL_RGB(65,45,65,AGIDL_RGB_888); icp[226] = AGIDL_RGB(65,45,61,AGIDL_RGB_888); icp[227] = AGIDL_RGB(65,45,53,AGIDL_RGB_888); icp[228] = AGIDL_RGB(66,46,50,AGIDL_RGB_888); icp[229] = AGIDL_RGB(66,45,45,AGIDL_RGB_888); icp[230] = AGIDL_RGB(19,14,13,AGIDL_RGB_888); icp[231] = AGIDL_RGB(1,1,1,AGIDL_RGB_888); icp[232] = AGIDL_RGB(56,45,38,AGIDL_RGB_888); icp[233] = AGIDL_RGB(68,63,47,AGIDL_RGB_888); icp[234] = AGIDL_RGB(65,65,45,AGIDL_RGB_888); icp[235] = AGIDL_RGB(61,65,45,AGIDL_RGB_888); icp[236] = AGIDL_RGB(53,65,45,AGIDL_RGB_888); icp[237] = AGIDL_RGB(49,65,45,AGIDL_RGB_888); 
	icp[238] = AGIDL_RGB(45,65,45,AGIDL_RGB_888); icp[239] = AGIDL_RGB(45,65,49,AGIDL_RGB_888); icp[240] = AGIDL_RGB(45,65,53,AGIDL_RGB_888); icp[241] = AGIDL_RGB(45,65,61,AGIDL_RGB_888); icp[242] = AGIDL_RGB(45,65,65,AGIDL_RGB_888); icp[243] = AGIDL_RGB(45,61,65,AGIDL_RGB_888); icp[244] = AGIDL_RGB(45,53,65,AGIDL_RGB_888); icp[245] = AGIDL_RGB(46,50,66,AGIDL_RGB_888); icp[246] = AGIDL_RGB(0,0,0,AGIDL_RGB_888); icp[247] = AGIDL_RGB(0,0,0,AGIDL_RGB_888); icp[248] = AGIDL_RGB(0,0,0,AGIDL_RGB_888); icp[249] = AGIDL_RGB(0,0,0,AGIDL_RGB_888); icp[250] = AGIDL_RGB(0,0,0,AGIDL_RGB_888); icp[251] = AGIDL_RGB(0,0,0,AGIDL_RGB_888); icp[252] = AGIDL_RGB(0,0,0,AGIDL_RGB_888); icp[253] = AGIDL_RGB(0,0,0,AGIDL_RGB_888); icp[254] = AGIDL_RGB(0,0,0,AGIDL_RGB_888); 

	AGIDL_ICP pal;
	pal.mode = AGIDL_ICP_256;
	pal.fmt = AGIDL_RGB_888;


	for(int i = 0; i < 256; i++){
		pal.icp.palette_256[i] = icp[i];
	}
	
	return pal;
}

AGIDL_ICP AGIDL_GenerateAloneInTheDarkICP(){
	COLOR icp[256];
	
	icp[0] = AGIDL_RGB(0,0,0,AGIDL_RGB_888); 
	icp[1] = AGIDL_RGB(255,255,255,AGIDL_RGB_888); icp[2] = AGIDL_RGB(51,51,59,AGIDL_RGB_888); icp[3] = AGIDL_RGB(195,191,255,AGIDL_RGB_888); icp[4] = AGIDL_RGB(143,179,143,AGIDL_RGB_888); icp[5] = AGIDL_RGB(171,119,171,AGIDL_RGB_888); icp[6] = AGIDL_RGB(171,135,99,AGIDL_RGB_888); icp[7] = AGIDL_RGB(255,23,171,AGIDL_RGB_888); icp[8] = AGIDL_RGB(75,83,99,AGIDL_RGB_888); icp[9] = AGIDL_RGB(199,87,95,AGIDL_RGB_888); icp[10] = AGIDL_RGB(87,151,87,AGIDL_RGB_888); icp[11] = AGIDL_RGB(87,191,255,AGIDL_RGB_888); icp[12] = AGIDL_RGB(255,139,87,AGIDL_RGB_888); icp[13] = AGIDL_RGB(175,87,255,AGIDL_RGB_888); icp[14] = AGIDL_RGB(255,255,135,AGIDL_RGB_888); icp[15] = AGIDL_RGB(255,255,255,AGIDL_RGB_888); icp[16] = AGIDL_RGB(215,179,147,AGIDL_RGB_888); 
	icp[17] = AGIDL_RGB(203,167,135,AGIDL_RGB_888); icp[18] = AGIDL_RGB(187,151,123,AGIDL_RGB_888); icp[19] = AGIDL_RGB(175,139,115,AGIDL_RGB_888); icp[20] = AGIDL_RGB(163,127,103,AGIDL_RGB_888); icp[21] = AGIDL_RGB(151,119,95,AGIDL_RGB_888); icp[22] = AGIDL_RGB(139,107,83,AGIDL_RGB_888); icp[23] = AGIDL_RGB(127,95,75,AGIDL_RGB_888); icp[24] = AGIDL_RGB(115,83,67,AGIDL_RGB_888); icp[25] = AGIDL_RGB(103,75,59,AGIDL_RGB_888); icp[26] = AGIDL_RGB(91,63,51,AGIDL_RGB_888); icp[27] = AGIDL_RGB(79,55,43,AGIDL_RGB_888); icp[28] = AGIDL_RGB(67,43,35,AGIDL_RGB_888); icp[29] = AGIDL_RGB(55,35,27,AGIDL_RGB_888); icp[30] = AGIDL_RGB(43,27,19,AGIDL_RGB_888); icp[31] = AGIDL_RGB(31,19,15,AGIDL_RGB_888); icp[32] = AGIDL_RGB(223,179,163,AGIDL_RGB_888); 
	icp[33] = AGIDL_RGB(207,159,143,AGIDL_RGB_888); icp[34] = AGIDL_RGB(195,143,123,AGIDL_RGB_888); icp[35] = AGIDL_RGB(183,131,107,AGIDL_RGB_888); icp[36] = AGIDL_RGB(171,115,91,AGIDL_RGB_888); icp[37] = AGIDL_RGB(159,103,75,AGIDL_RGB_888); icp[38] = AGIDL_RGB(143,91,63,AGIDL_RGB_888); icp[39] = AGIDL_RGB(131,79,51,AGIDL_RGB_888); icp[40] = AGIDL_RGB(119,71,39,AGIDL_RGB_888); icp[41] = AGIDL_RGB(107,59,31,AGIDL_RGB_888); icp[42] = AGIDL_RGB(95,51,23,AGIDL_RGB_888); icp[43] = AGIDL_RGB(79,43,15,AGIDL_RGB_888); icp[44] = AGIDL_RGB(67,35,7,AGIDL_RGB_888); icp[45] = AGIDL_RGB(55,27,7,AGIDL_RGB_888); icp[46] = AGIDL_RGB(43,19,0,AGIDL_RGB_888); icp[47] = AGIDL_RGB(31,15,0,AGIDL_RGB_888); icp[48] = AGIDL_RGB(207,111,111,AGIDL_RGB_888); 
	icp[49] = AGIDL_RGB(195,99,99,AGIDL_RGB_888); icp[50] = AGIDL_RGB(183,91,91,AGIDL_RGB_888); icp[51] = AGIDL_RGB(171,83,79,AGIDL_RGB_888); icp[52] = AGIDL_RGB(159,75,71,AGIDL_RGB_888); icp[53] = AGIDL_RGB(147,67,63,AGIDL_RGB_888); icp[54] = AGIDL_RGB(135,63,55,AGIDL_RGB_888); icp[55] = AGIDL_RGB(123,55,47,AGIDL_RGB_888); icp[56] = AGIDL_RGB(111,47,43,AGIDL_RGB_888); icp[57] = AGIDL_RGB(99,43,35,AGIDL_RGB_888); icp[58] = AGIDL_RGB(87,35,31,AGIDL_RGB_888); icp[59] = AGIDL_RGB(75,31,23,AGIDL_RGB_888); icp[60] = AGIDL_RGB(63,23,19,AGIDL_RGB_888); icp[61] = AGIDL_RGB(51,19,15,AGIDL_RGB_888); icp[62] = AGIDL_RGB(43,15,11,AGIDL_RGB_888); icp[63] = AGIDL_RGB(31,11,7,AGIDL_RGB_888); icp[64] = AGIDL_RGB(115,123,107,AGIDL_RGB_888); 
	icp[65] = AGIDL_RGB(107,115,99,AGIDL_RGB_888); icp[66] = AGIDL_RGB(99,111,91,AGIDL_RGB_888); icp[67] = AGIDL_RGB(91,107,87,AGIDL_RGB_888); icp[68] = AGIDL_RGB(83,103,79,AGIDL_RGB_888); icp[69] = AGIDL_RGB(79,95,75,AGIDL_RGB_888); icp[70] = AGIDL_RGB(71,91,67,AGIDL_RGB_888); icp[71] = AGIDL_RGB(63,87,63,AGIDL_RGB_888); icp[72] = AGIDL_RGB(59,79,59,AGIDL_RGB_888); icp[73] = AGIDL_RGB(51,75,55,AGIDL_RGB_888); icp[74] = AGIDL_RGB(47,71,51,AGIDL_RGB_888); icp[75] = AGIDL_RGB(43,67,51,AGIDL_RGB_888); icp[76] = AGIDL_RGB(39,59,47,AGIDL_RGB_888); icp[77] = AGIDL_RGB(35,55,43,AGIDL_RGB_888); icp[78] = AGIDL_RGB(31,51,39,AGIDL_RGB_888); icp[79] = AGIDL_RGB(27,47,39,AGIDL_RGB_888); icp[80] = AGIDL_RGB(203,51,83,AGIDL_RGB_888); 
	icp[81] = AGIDL_RGB(191,43,75,AGIDL_RGB_888); icp[82] = AGIDL_RGB(179,39,71,AGIDL_RGB_888); icp[83] = AGIDL_RGB(167,31,67,AGIDL_RGB_888); icp[84] = AGIDL_RGB(155,27,59,AGIDL_RGB_888); icp[85] = AGIDL_RGB(143,23,55,AGIDL_RGB_888); icp[86] = AGIDL_RGB(131,19,51,AGIDL_RGB_888); icp[87] = AGIDL_RGB(123,15,47,AGIDL_RGB_888); icp[88] = AGIDL_RGB(111,11,39,AGIDL_RGB_888); icp[89] = AGIDL_RGB(99,7,35,AGIDL_RGB_888); icp[90] = AGIDL_RGB(87,7,31,AGIDL_RGB_888); icp[91] = AGIDL_RGB(75,0,27,AGIDL_RGB_888); icp[92] = AGIDL_RGB(63,0,23,AGIDL_RGB_888); icp[93] = AGIDL_RGB(51,0,19,AGIDL_RGB_888); icp[94] = AGIDL_RGB(39,0,15,AGIDL_RGB_888); icp[95] = AGIDL_RGB(31,0,11,AGIDL_RGB_888); icp[96] = AGIDL_RGB(127,167,171,AGIDL_RGB_888); 
	icp[97] = AGIDL_RGB(119,159,159,AGIDL_RGB_888); icp[98] = AGIDL_RGB(111,151,151,AGIDL_RGB_888); icp[99] = AGIDL_RGB(103,143,143,AGIDL_RGB_888); icp[100] = AGIDL_RGB(95,131,131,AGIDL_RGB_888); icp[101] = AGIDL_RGB(87,123,119,AGIDL_RGB_888); icp[102] = AGIDL_RGB(79,115,111,AGIDL_RGB_888); icp[103] = AGIDL_RGB(71,103,103,AGIDL_RGB_888); icp[104] = AGIDL_RGB(63,95,91,AGIDL_RGB_888); icp[105] = AGIDL_RGB(55,87,79,AGIDL_RGB_888); icp[106] = AGIDL_RGB(51,75,71,AGIDL_RGB_888); icp[107] = AGIDL_RGB(43,67,63,AGIDL_RGB_888); icp[108] = AGIDL_RGB(35,59,51,AGIDL_RGB_888); icp[109] = AGIDL_RGB(31,47,43,AGIDL_RGB_888); icp[110] = AGIDL_RGB(23,39,35,AGIDL_RGB_888); icp[111] = AGIDL_RGB(19,31,27,AGIDL_RGB_888); icp[112] = AGIDL_RGB(243,183,103,AGIDL_RGB_888); 
	icp[113] = AGIDL_RGB(227,171,95,AGIDL_RGB_888); icp[114] = AGIDL_RGB(215,159,87,AGIDL_RGB_888); icp[115] = AGIDL_RGB(199,147,79,AGIDL_RGB_888); icp[116] = AGIDL_RGB(183,131,71,AGIDL_RGB_888); icp[117] = AGIDL_RGB(171,123,63,AGIDL_RGB_888); icp[118] = AGIDL_RGB(155,111,55,AGIDL_RGB_888); icp[119] = AGIDL_RGB(143,99,47,AGIDL_RGB_888); icp[120] = AGIDL_RGB(127,87,39,AGIDL_RGB_888); icp[121] = AGIDL_RGB(115,75,35,AGIDL_RGB_888); icp[122] = AGIDL_RGB(99,67,31,AGIDL_RGB_888); icp[123] = AGIDL_RGB(87,55,23,AGIDL_RGB_888); icp[124] = AGIDL_RGB(71,47,19,AGIDL_RGB_888); icp[125] = AGIDL_RGB(59,35,15,AGIDL_RGB_888); icp[126] = AGIDL_RGB(43,27,11,AGIDL_RGB_888); icp[127] = AGIDL_RGB(31,19,7,AGIDL_RGB_888); icp[128] = AGIDL_RGB(227,131,75,AGIDL_RGB_888); 
	icp[129] = AGIDL_RGB(211,119,67,AGIDL_RGB_888); icp[130] = AGIDL_RGB(199,111,63,AGIDL_RGB_888); icp[131] = AGIDL_RGB(187,99,59,AGIDL_RGB_888); icp[132] = AGIDL_RGB(175,91,51,AGIDL_RGB_888); icp[133] = AGIDL_RGB(159,83,47,AGIDL_RGB_888); icp[134] = AGIDL_RGB(147,71,43,AGIDL_RGB_888); icp[135] = AGIDL_RGB(135,63,39,AGIDL_RGB_888); icp[136] = AGIDL_RGB(119,55,35,AGIDL_RGB_888); icp[137] = AGIDL_RGB(107,47,27,AGIDL_RGB_888); icp[138] = AGIDL_RGB(95,39,23,AGIDL_RGB_888); icp[139] = AGIDL_RGB(83,35,19,AGIDL_RGB_888); icp[140] = AGIDL_RGB(67,27,15,AGIDL_RGB_888); icp[141] = AGIDL_RGB(55,23,11,AGIDL_RGB_888); icp[142] = AGIDL_RGB(43,15,7,AGIDL_RGB_888); icp[143] = AGIDL_RGB(31,11,7,AGIDL_RGB_888); icp[144] = AGIDL_RGB(195,143,143,AGIDL_RGB_888); 
	icp[145] = AGIDL_RGB(183,131,131,AGIDL_RGB_888); icp[146] = AGIDL_RGB(171,123,123,AGIDL_RGB_888); icp[147] = AGIDL_RGB(163,111,111,AGIDL_RGB_888); icp[148] = AGIDL_RGB(151,103,103,AGIDL_RGB_888); icp[149] = AGIDL_RGB(143,91,91,AGIDL_RGB_888); icp[150] = AGIDL_RGB(131,83,83,AGIDL_RGB_888); icp[151] = AGIDL_RGB(119,75,75,AGIDL_RGB_888); icp[152] = AGIDL_RGB(111,67,67,AGIDL_RGB_888); icp[153] = AGIDL_RGB(99,59,59,AGIDL_RGB_888); icp[154] = AGIDL_RGB(91,51,51,AGIDL_RGB_888); icp[155] = AGIDL_RGB(79,43,43,AGIDL_RGB_888); icp[156] = AGIDL_RGB(67,35,35,AGIDL_RGB_888); icp[157] = AGIDL_RGB(59,31,31,AGIDL_RGB_888); icp[158] = AGIDL_RGB(47,23,23,AGIDL_RGB_888); icp[159] = AGIDL_RGB(39,19,19,AGIDL_RGB_888); icp[160] = AGIDL_RGB(59,119,103,AGIDL_RGB_888); 
	icp[161] = AGIDL_RGB(51,111,95,AGIDL_RGB_888); icp[162] = AGIDL_RGB(47,107,91,AGIDL_RGB_888); icp[163] = AGIDL_RGB(39,103,87,AGIDL_RGB_888); icp[164] = AGIDL_RGB(35,99,79,AGIDL_RGB_888); icp[165] = AGIDL_RGB(31,95,75,AGIDL_RGB_888); icp[166] = AGIDL_RGB(27,91,71,AGIDL_RGB_888); icp[167] = AGIDL_RGB(19,87,63,AGIDL_RGB_888); icp[168] = AGIDL_RGB(19,83,59,AGIDL_RGB_888); icp[169] = AGIDL_RGB(15,75,55,AGIDL_RGB_888); icp[170] = AGIDL_RGB(11,71,51,AGIDL_RGB_888); icp[171] = AGIDL_RGB(7,67,47,AGIDL_RGB_888); icp[172] = AGIDL_RGB(7,63,43,AGIDL_RGB_888); icp[173] = AGIDL_RGB(0,59,39,AGIDL_RGB_888); icp[174] = AGIDL_RGB(0,55,35,AGIDL_RGB_888); icp[175] = AGIDL_RGB(0,51,31,AGIDL_RGB_888); icp[176] = AGIDL_RGB(195,195,179,AGIDL_RGB_888); 
	icp[177] = AGIDL_RGB(183,183,167,AGIDL_RGB_888); icp[178] = AGIDL_RGB(171,171,155,AGIDL_RGB_888); icp[179] = AGIDL_RGB(159,159,143,AGIDL_RGB_888); icp[180] = AGIDL_RGB(147,147,131,AGIDL_RGB_888); icp[181] = AGIDL_RGB(135,135,123,AGIDL_RGB_888); icp[182] = AGIDL_RGB(123,123,111,AGIDL_RGB_888); icp[183] = AGIDL_RGB(115,115,99,AGIDL_RGB_888); icp[184] = AGIDL_RGB(103,103,87,AGIDL_RGB_888); icp[185] = AGIDL_RGB(91,91,79,AGIDL_RGB_888); icp[186] = AGIDL_RGB(79,79,67,AGIDL_RGB_888); icp[187] = AGIDL_RGB(67,67,59,AGIDL_RGB_888); icp[188] = AGIDL_RGB(55,55,47,AGIDL_RGB_888); icp[189] = AGIDL_RGB(43,43,39,AGIDL_RGB_888); icp[190] = AGIDL_RGB(31,31,27,AGIDL_RGB_888); icp[191] = AGIDL_RGB(0,0,0,AGIDL_RGB_888); icp[192] = AGIDL_RGB(151,107,111,AGIDL_RGB_888); 
	icp[193] = AGIDL_RGB(139,99,103,AGIDL_RGB_888); icp[194] = AGIDL_RGB(127,91,95,AGIDL_RGB_888); icp[195] = AGIDL_RGB(119,79,87,AGIDL_RGB_888); icp[196] = AGIDL_RGB(111,67,79,AGIDL_RGB_888); icp[197] = AGIDL_RGB(103,59,75,AGIDL_RGB_888); icp[198] = AGIDL_RGB(95,47,71,AGIDL_RGB_888); icp[199] = AGIDL_RGB(87,39,67,AGIDL_RGB_888); icp[200] = AGIDL_RGB(79,31,67,AGIDL_RGB_888); icp[201] = AGIDL_RGB(71,23,63,AGIDL_RGB_888); icp[202] = AGIDL_RGB(63,15,59,AGIDL_RGB_888); icp[203] = AGIDL_RGB(51,11,55,AGIDL_RGB_888); icp[204] = AGIDL_RGB(39,7,47,AGIDL_RGB_888); icp[205] = AGIDL_RGB(27,0,39,AGIDL_RGB_888); icp[206] = AGIDL_RGB(19,0,31,AGIDL_RGB_888); icp[207] = AGIDL_RGB(11,0,23,AGIDL_RGB_888); icp[208] = AGIDL_RGB(143,147,95,AGIDL_RGB_888); 
	icp[209] = AGIDL_RGB(131,139,83,AGIDL_RGB_888); icp[210] = AGIDL_RGB(119,127,71,AGIDL_RGB_888); icp[211] = AGIDL_RGB(111,119,63,AGIDL_RGB_888); icp[212] = AGIDL_RGB(99,111,51,AGIDL_RGB_888); icp[213] = AGIDL_RGB(87,103,43,AGIDL_RGB_888); icp[214] = AGIDL_RGB(75,95,35,AGIDL_RGB_888); icp[215] = AGIDL_RGB(67,87,27,AGIDL_RGB_888); icp[216] = AGIDL_RGB(59,79,23,AGIDL_RGB_888); icp[217] = AGIDL_RGB(47,71,15,AGIDL_RGB_888); icp[218] = AGIDL_RGB(39,63,11,AGIDL_RGB_888); icp[219] = AGIDL_RGB(31,55,7,AGIDL_RGB_888); icp[220] = AGIDL_RGB(23,47,7,AGIDL_RGB_888); icp[221] = AGIDL_RGB(15,39,0,AGIDL_RGB_888); icp[222] = AGIDL_RGB(11,31,0,AGIDL_RGB_888); icp[223] = AGIDL_RGB(7,23,0,AGIDL_RGB_888); icp[224] = AGIDL_RGB(195,195,179,AGIDL_RGB_888); 
	icp[225] = AGIDL_RGB(127,167,171,AGIDL_RGB_888); icp[226] = AGIDL_RGB(119,159,159,AGIDL_RGB_888); icp[227] = AGIDL_RGB(111,151,151,AGIDL_RGB_888); icp[228] = AGIDL_RGB(119,159,159,AGIDL_RGB_888); icp[229] = AGIDL_RGB(127,167,171,AGIDL_RGB_888); icp[230] = AGIDL_RGB(111,151,151,AGIDL_RGB_888); icp[231] = AGIDL_RGB(103,143,143,AGIDL_RGB_888); icp[232] = AGIDL_RGB(95,131,131,AGIDL_RGB_888); icp[233] = AGIDL_RGB(79,115,111,AGIDL_RGB_888); icp[234] = AGIDL_RGB(95,131,131,AGIDL_RGB_888); icp[235] = AGIDL_RGB(103,143,143,AGIDL_RGB_888); icp[236] = AGIDL_RGB(63,95,91,AGIDL_RGB_888); icp[237] = AGIDL_RGB(55,87,79,AGIDL_RGB_888); icp[238] = AGIDL_RGB(43,67,67,AGIDL_RGB_888); icp[239] = AGIDL_RGB(51,83,75,AGIDL_RGB_888); icp[240] = AGIDL_RGB(123,115,103,AGIDL_RGB_888); 
	icp[241] = AGIDL_RGB(123,115,103,AGIDL_RGB_888); icp[242] = AGIDL_RGB(123,115,103,AGIDL_RGB_888); icp[243] = AGIDL_RGB(123,115,103,AGIDL_RGB_888); icp[244] = AGIDL_RGB(123,115,103,AGIDL_RGB_888); icp[245] = AGIDL_RGB(123,115,103,AGIDL_RGB_888); icp[246] = AGIDL_RGB(123,115,103,AGIDL_RGB_888); icp[247] = AGIDL_RGB(123,115,103,AGIDL_RGB_888); icp[248] = AGIDL_RGB(123,115,103,AGIDL_RGB_888); icp[249] = AGIDL_RGB(123,115,103,AGIDL_RGB_888); icp[250] = AGIDL_RGB(123,115,103,AGIDL_RGB_888); icp[251] = AGIDL_RGB(123,115,103,AGIDL_RGB_888); icp[252] = AGIDL_RGB(123,115,103,AGIDL_RGB_888); icp[253] = AGIDL_RGB(123,115,103,AGIDL_RGB_888); icp[254] = AGIDL_RGB(123,115,103,AGIDL_RGB_888); icp[255] = AGIDL_RGB(123,115,103,AGIDL_RGB_888); 
	
	AGIDL_ICP pal;
	pal.mode = AGIDL_ICP_256;
	pal.fmt = AGIDL_RGB_888;


	for(int i = 0; i < 256; i++){
		pal.icp.palette_256[i] = icp[i];
	}
	
	return pal;
}

AGIDL_ICP AGIDL_GenerateWin95ICP(){
	COLOR icp[256];
	
	icp[0] = 0x000000; icp[1] = 0x800000; icp[2] = 0x008000; icp[3] = 0x808000; icp[4] = 0x000080; icp[5] = 0x800080; icp[6] = 0x008080; icp[7] = 0xc0c0c0; icp[8] = 0xc0dcc0; 
	icp[9] = 0xa6caf0; icp[10] = 0x2a3faa; icp[11] = 0x2a3fff; icp[12] = 0x2a5f00; icp[13] = 0x2a5f55; icp[14] = 0x2a5faa; icp[15] = 0x2a5fff; icp[16] = 0x2a7f00; 
	icp[17] = 0x2a7f55; icp[18] = 0x2a7faa; icp[19] = 0x2a7fff; icp[20] = 0x2a9f00; icp[21] = 0x2a9f55; icp[22] = 0x2a9faa; icp[23] = 0x2a9fff; icp[24] = 0x2abf00; 
	icp[25] = 0x2abf55; icp[26] = 0x2abfaa; icp[27] = 0x2abfff; icp[28] = 0x2adf00; icp[29] = 0x2adf55; icp[30] = 0x2adfaa; icp[31] = 0x2adfff; icp[32] = 0x2aff00; 
	icp[33] = 0x2aff55; icp[34] = 0x2affaa; icp[35] = 0x2affff; icp[36] = 0x550000; icp[37] = 0x550055; icp[38] = 0x5500aa; icp[39] = 0x5500ff; icp[40] = 0x551f00; 
	icp[41] = 0x551f55; icp[42] = 0x551faa; icp[43] = 0x551fff; icp[44] = 0x553f00; icp[45] = 0x553f55; icp[46] = 0x553faa; icp[47] = 0x553fff; icp[48] = 0x555f00; 
	icp[49] = 0x555f55; icp[50] = 0x555faa; icp[51] = 0x555fff; icp[52] = 0x557f00; icp[53] = 0x557f55; icp[54] = 0x557faa; icp[55] = 0x557fff; icp[56] = 0x559f00; 
	icp[57] = 0x559f55; icp[58] = 0x559faa; icp[59] = 0x559fff; icp[60] = 0x55bf00; icp[61] = 0x55bf55; icp[62] = 0x55bfaa; icp[63] = 0x55bfff; icp[64] = 0x55df00; 
	icp[65] = 0x55df55; icp[66] = 0x55dfaa; icp[67] = 0x55dfff; icp[68] = 0x55ff00; icp[69] = 0x55ff55; icp[70] = 0x55ffaa; icp[71] = 0x55ffff; icp[72] = 0x7f0000; 
	icp[73] = 0x7f0055; icp[74] = 0x7f00aa; icp[75] = 0x7f00ff; icp[76] = 0x7f1f00; icp[77] = 0x7f1f55; icp[78] = 0x7f1faa; icp[79] = 0x7f1fff; icp[80] = 0x7f3f00; 
	icp[81] = 0x7f3f55; icp[82] = 0x7f3faa; icp[83] = 0x7f3fff; icp[84] = 0x7f5f00; icp[85] = 0x7f5f55; icp[86] = 0x7f5faa; icp[87] = 0x7f5fff; icp[88] = 0x7f7f00; 
	icp[89] = 0x7f7f55; icp[90] = 0x7f7faa; icp[91] = 0x7f7fff; icp[92] = 0x7f9f00; icp[93] = 0x7f9f55; icp[94] = 0x7f9faa; icp[95] = 0x7f9fff; icp[96] = 0x7fbf00; 
	icp[97] = 0x7fbf55; icp[98] = 0x7fbfaa; icp[99] = 0x7fbfff; icp[100] = 0x7fdf00; icp[101] = 0x7fdf55; icp[102] = 0x7fdfaa; icp[103] = 0x7fdfff; icp[104] = 0x7fff00; 
	icp[105] = 0x7fff55; icp[106] = 0x7fffaa; icp[107] = 0x7fffff; icp[108] = 0xaa0000; icp[109] = 0xaa0055; icp[110] = 0xaa00aa; icp[111] = 0xaa00ff; icp[112] = 0xaa1f00; 
	icp[113] = 0xaa1f55; icp[114] = 0xaa1faa; icp[115] = 0xaa1fff; icp[116] = 0xaa3f00; icp[117] = 0xaa3f55; icp[118] = 0xaa3faa; icp[119] = 0xaa3fff; icp[120] = 0xaa5f00; 
	icp[121] = 0xaa5f55; icp[122] = 0xaa5faa; icp[123] = 0xaa5fff; icp[124] = 0xaa7f00; icp[125] = 0xaa7f55; icp[126] = 0xaa7faa; icp[127] = 0xaa7fff; icp[128] = 0xaa9f00; 
	icp[129] = 0xaa9f55; icp[130] = 0xaa9faa; icp[131] = 0xaa9fff; icp[132] = 0xaabf00; icp[133] = 0xaabf55; icp[134] = 0xaabfaa; icp[135] = 0xaabfff; icp[136] = 0xaadf00; 
	icp[137] = 0xaadf55; icp[138] = 0xaadfaa; icp[139] = 0xaadfff; icp[140] = 0xaaff00; icp[141] = 0xaaff55; icp[142] = 0xaaffaa; icp[143] = 0xaaffff; icp[144] = 0xd40000; 
	icp[145] = 0xd40055; icp[146] = 0xd400aa; icp[147] = 0xd400ff; icp[148] = 0xd41f00; icp[149] = 0xd41f55; icp[150] = 0xd41faa; icp[151] = 0xd41fff; icp[152] = 0xd43f00; 
	icp[153] = 0xd43f55; icp[154] = 0xd43faa; icp[155] = 0xd43fff; icp[156] = 0xd45f00; icp[157] = 0xd45f55; icp[158] = 0xd45faa; icp[159] = 0xd45fff; icp[160] = 0xd47f00; 
	icp[161] = 0xd47f55; icp[162] = 0xd47faa; icp[163] = 0xd47fff; icp[164] = 0xd49f00; icp[165] = 0xd49f55; icp[166] = 0xd49faa; icp[167] = 0xd49fff; icp[168] = 0xd4bf00; 
	icp[169] = 0xd4bf55; icp[170] = 0xd4bfaa; icp[171] = 0xd4bfff; icp[172] = 0xd4df00; icp[173] = 0xd4df55; icp[174] = 0xd4dfaa; icp[175] = 0xd4dfff; icp[176] = 0xd4ff00; 
	icp[177] = 0xd4ff55; icp[178] = 0xd4ffaa; icp[179] = 0xd4ffff; icp[180] = 0xff0055; icp[181] = 0xff00aa; icp[182] = 0xff1f00; icp[183] = 0xff1f55; icp[184] = 0xff1faa; 
	icp[185] = 0xff1fff; icp[186] = 0xff3f00; icp[187] = 0xff3f55; icp[188] = 0xff3faa; icp[189] = 0xff3fff; icp[190] = 0xff5f00; icp[191] = 0xff5f55; icp[192] = 0xff5faa; 
	icp[193] = 0xff5fff; icp[194] = 0xff7f00; icp[195] = 0xff7f55; icp[196] = 0xff7faa; icp[197] = 0xff7fff; icp[198] = 0xff9f00; icp[199] = 0xff9f55; icp[200] = 0xff9faa; 
	icp[201] = 0xff9fff; icp[202] = 0xffbf00; icp[203] = 0xffbf55; icp[204] = 0xffbfaa; icp[205] = 0xffbfff; icp[206] = 0xffdf00; icp[207] = 0xffdf55; icp[208] = 0xffdfaa; 
	icp[209] = 0xffdfff; icp[210] = 0xffff55; icp[211] = 0xffffaa; icp[212] = 0xccccff; icp[213] = 0xffccff; icp[214] = 0x33ffff; icp[215] = 0x66ffff; icp[216] = 0x99ffff; 
	icp[217] = 0xccffff; icp[218] = 0x007f00; icp[219] = 0x007f55; icp[220] = 0x007faa; icp[221] = 0x007fff; icp[222] = 0x009f00; icp[223] = 0x009f55; icp[224] = 0x009faa; 
	icp[225] = 0x009fff; icp[226] = 0x00bf00; icp[227] = 0x00bf55; icp[228] = 0x00bfaa; icp[229] = 0x00bfff; icp[230] = 0x00df00; icp[231] = 0x00df55; icp[232] = 0x00dfaa; 
	icp[233] = 0x00dfff; icp[234] = 0x00ff55; icp[235] = 0x00ffaa; icp[236] = 0x2a0000; icp[237] = 0x2a0055; icp[238] = 0x2a00aa; icp[239] = 0x2a00ff; icp[240] = 0x2a1f00; 
	icp[241] = 0x2a1f55; icp[242] = 0x2a1faa; icp[243] = 0x2a1fff; icp[244] = 0x2a3f00; icp[245] = 0x2a3f55; icp[246] = 0xfffbf0; icp[247] = 0xa0a0a4; icp[248] = 0x808080; 
	icp[249] = 0xff0000; icp[250] = 0x00ff00; icp[251] = 0xffff00; icp[252] = 0x0000ff; icp[253] = 0xff00ff; icp[254] = 0x00ffff; icp[255] = 0xffffff;
	
	AGIDL_ICP pal;
	pal.mode = AGIDL_ICP_256;
	pal.fmt = AGIDL_RGB_888;

	for(int i = 0; i < 256; i++){
		pal.icp.palette_256[i] = icp[i];
	}
	
	return pal;
}

AGIDL_ICP AGIDL_GenerateVGAICP16(){
	COLOR16 icp[256];
	
	icp[0] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[1] = AGIDL_RGB(0,0,21,AGIDL_RGB_555); icp[2] = AGIDL_RGB(0,21,0,AGIDL_RGB_555); icp[3] = AGIDL_RGB(0,21,21,AGIDL_RGB_555); icp[4] = AGIDL_RGB(21,0,0,AGIDL_RGB_555); icp[5] = AGIDL_RGB(21,0,21,AGIDL_RGB_555); icp[6] = AGIDL_RGB(21,10,0,AGIDL_RGB_555); icp[7] = AGIDL_RGB(21,21,21,AGIDL_RGB_555); icp[8] = AGIDL_RGB(10,10,10,AGIDL_RGB_555); icp[9] = AGIDL_RGB(3,3,9,AGIDL_RGB_555); icp[10] = AGIDL_RGB(0,0,1,AGIDL_RGB_555); icp[11] = AGIDL_RGB(9,27,9,AGIDL_RGB_555); icp[12] = AGIDL_RGB(11,31,31,AGIDL_RGB_555); icp[13] = AGIDL_RGB(31,10,10,AGIDL_RGB_555); icp[14] = AGIDL_RGB(31,10,31,AGIDL_RGB_555); icp[15] = AGIDL_RGB(31,31,10,AGIDL_RGB_555); icp[16] = AGIDL_RGB(31,31,31,AGIDL_RGB_555); 
	icp[17] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[18] = AGIDL_RGB(2,2,2,AGIDL_RGB_555); icp[19] = AGIDL_RGB(4,4,4,AGIDL_RGB_555); icp[20] = AGIDL_RGB(6,6,6,AGIDL_RGB_555); icp[21] = AGIDL_RGB(8,8,8,AGIDL_RGB_555); icp[22] = AGIDL_RGB(10,10,10,AGIDL_RGB_555); icp[23] = AGIDL_RGB(12,12,12,AGIDL_RGB_555); icp[24] = AGIDL_RGB(14,14,14,AGIDL_RGB_555); icp[25] = AGIDL_RGB(17,17,17,AGIDL_RGB_555); icp[26] = AGIDL_RGB(5,5,5,AGIDL_RGB_555); icp[27] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[28] = AGIDL_RGB(18,18,18,AGIDL_RGB_555); icp[29] = AGIDL_RGB(24,24,24,AGIDL_RGB_555); icp[30] = AGIDL_RGB(25,25,25,AGIDL_RGB_555); icp[31] = AGIDL_RGB(27,27,27,AGIDL_RGB_555); icp[32] = AGIDL_RGB(29,29,29,AGIDL_RGB_555); icp[33] = AGIDL_RGB(31,31,31,AGIDL_RGB_555); 
	icp[34] = AGIDL_RGB(0,0,31,AGIDL_RGB_555); icp[35] = AGIDL_RGB(8,0,31,AGIDL_RGB_555); icp[36] = AGIDL_RGB(16,0,31,AGIDL_RGB_555); icp[37] = AGIDL_RGB(23,0,31,AGIDL_RGB_555); icp[38] = AGIDL_RGB(31,0,31,AGIDL_RGB_555); icp[39] = AGIDL_RGB(31,0,23,AGIDL_RGB_555); icp[40] = AGIDL_RGB(31,0,16,AGIDL_RGB_555); icp[41] = AGIDL_RGB(31,0,8,AGIDL_RGB_555); icp[42] = AGIDL_RGB(31,0,0,AGIDL_RGB_555); icp[43] = AGIDL_RGB(9,2,0,AGIDL_RGB_555); icp[44] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[45] = AGIDL_RGB(27,13,0,AGIDL_RGB_555); icp[46] = AGIDL_RGB(31,24,0,AGIDL_RGB_555); icp[47] = AGIDL_RGB(31,31,0,AGIDL_RGB_555); icp[48] = AGIDL_RGB(23,31,0,AGIDL_RGB_555); icp[49] = AGIDL_RGB(16,31,0,AGIDL_RGB_555); icp[50] = AGIDL_RGB(8,31,0,AGIDL_RGB_555); 
	icp[51] = AGIDL_RGB(0,31,0,AGIDL_RGB_555); icp[52] = AGIDL_RGB(0,31,8,AGIDL_RGB_555); icp[53] = AGIDL_RGB(0,31,16,AGIDL_RGB_555); icp[54] = AGIDL_RGB(0,31,23,AGIDL_RGB_555); icp[55] = AGIDL_RGB(0,31,31,AGIDL_RGB_555); icp[56] = AGIDL_RGB(0,23,31,AGIDL_RGB_555); icp[57] = AGIDL_RGB(0,16,31,AGIDL_RGB_555); icp[58] = AGIDL_RGB(0,8,31,AGIDL_RGB_555); icp[59] = AGIDL_RGB(16,16,31,AGIDL_RGB_555); icp[60] = AGIDL_RGB(5,4,9,AGIDL_RGB_555); icp[61] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[62] = AGIDL_RGB(20,13,27,AGIDL_RGB_555); icp[63] = AGIDL_RGB(29,16,31,AGIDL_RGB_555); icp[64] = AGIDL_RGB(31,16,31,AGIDL_RGB_555); icp[65] = AGIDL_RGB(31,16,27,AGIDL_RGB_555); icp[66] = AGIDL_RGB(31,16,23,AGIDL_RGB_555); icp[67] = AGIDL_RGB(31,16,19,AGIDL_RGB_555); 
	icp[68] = AGIDL_RGB(31,16,16,AGIDL_RGB_555); icp[69] = AGIDL_RGB(31,19,16,AGIDL_RGB_555); icp[70] = AGIDL_RGB(31,23,16,AGIDL_RGB_555); icp[71] = AGIDL_RGB(31,27,16,AGIDL_RGB_555); icp[72] = AGIDL_RGB(31,31,16,AGIDL_RGB_555); icp[73] = AGIDL_RGB(27,31,16,AGIDL_RGB_555); icp[74] = AGIDL_RGB(23,31,16,AGIDL_RGB_555); icp[75] = AGIDL_RGB(20,31,16,AGIDL_RGB_555); icp[76] = AGIDL_RGB(16,31,16,AGIDL_RGB_555); icp[77] = AGIDL_RGB(4,9,5,AGIDL_RGB_555); icp[78] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[79] = AGIDL_RGB(13,27,20,AGIDL_RGB_555); icp[80] = AGIDL_RGB(16,31,29,AGIDL_RGB_555); icp[81] = AGIDL_RGB(16,31,31,AGIDL_RGB_555); icp[82] = AGIDL_RGB(16,27,31,AGIDL_RGB_555); icp[83] = AGIDL_RGB(16,23,31,AGIDL_RGB_555); icp[84] = AGIDL_RGB(16,19,31,AGIDL_RGB_555); 
	icp[85] = AGIDL_RGB(23,23,31,AGIDL_RGB_555); icp[86] = AGIDL_RGB(25,23,31,AGIDL_RGB_555); icp[87] = AGIDL_RGB(27,23,31,AGIDL_RGB_555); icp[88] = AGIDL_RGB(29,23,31,AGIDL_RGB_555); icp[89] = AGIDL_RGB(31,23,31,AGIDL_RGB_555); icp[90] = AGIDL_RGB(31,23,29,AGIDL_RGB_555); icp[91] = AGIDL_RGB(31,23,27,AGIDL_RGB_555); icp[92] = AGIDL_RGB(31,23,25,AGIDL_RGB_555); icp[93] = AGIDL_RGB(31,23,23,AGIDL_RGB_555); icp[94] = AGIDL_RGB(9,7,6,AGIDL_RGB_555); icp[95] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[96] = AGIDL_RGB(27,23,19,AGIDL_RGB_555); icp[97] = AGIDL_RGB(31,31,24,AGIDL_RGB_555); icp[98] = AGIDL_RGB(31,31,23,AGIDL_RGB_555); icp[99] = AGIDL_RGB(29,31,23,AGIDL_RGB_555); icp[100] = AGIDL_RGB(27,31,23,AGIDL_RGB_555); icp[101] = AGIDL_RGB(25,31,23,AGIDL_RGB_555); 
	icp[102] = AGIDL_RGB(23,31,23,AGIDL_RGB_555); icp[103] = AGIDL_RGB(23,31,25,AGIDL_RGB_555); icp[104] = AGIDL_RGB(23,31,28,AGIDL_RGB_555); icp[105] = AGIDL_RGB(23,31,30,AGIDL_RGB_555); icp[106] = AGIDL_RGB(23,31,31,AGIDL_RGB_555); icp[107] = AGIDL_RGB(23,30,31,AGIDL_RGB_555); icp[108] = AGIDL_RGB(23,28,31,AGIDL_RGB_555); icp[109] = AGIDL_RGB(23,25,31,AGIDL_RGB_555); icp[110] = AGIDL_RGB(0,0,14,AGIDL_RGB_555); icp[111] = AGIDL_RGB(1,0,4,AGIDL_RGB_555); icp[112] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[113] = AGIDL_RGB(6,0,12,AGIDL_RGB_555); icp[114] = AGIDL_RGB(11,0,14,AGIDL_RGB_555); icp[115] = AGIDL_RGB(14,0,14,AGIDL_RGB_555); icp[116] = AGIDL_RGB(14,0,10,AGIDL_RGB_555); icp[117] = AGIDL_RGB(14,0,7,AGIDL_RGB_555); icp[118] = AGIDL_RGB(14,0,3,AGIDL_RGB_555); 
	icp[119] = AGIDL_RGB(14,0,0,AGIDL_RGB_555); icp[120] = AGIDL_RGB(14,3,0,AGIDL_RGB_555); icp[121] = AGIDL_RGB(14,7,0,AGIDL_RGB_555); icp[122] = AGIDL_RGB(14,10,0,AGIDL_RGB_555); icp[123] = AGIDL_RGB(14,14,0,AGIDL_RGB_555); icp[124] = AGIDL_RGB(10,14,0,AGIDL_RGB_555); icp[125] = AGIDL_RGB(7,14,0,AGIDL_RGB_555); icp[126] = AGIDL_RGB(3,14,0,AGIDL_RGB_555); icp[127] = AGIDL_RGB(0,14,0,AGIDL_RGB_555); icp[128] = AGIDL_RGB(0,4,1,AGIDL_RGB_555); icp[129] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[130] = AGIDL_RGB(0,12,6,AGIDL_RGB_555); icp[131] = AGIDL_RGB(0,14,11,AGIDL_RGB_555); icp[132] = AGIDL_RGB(0,14,14,AGIDL_RGB_555); icp[133] = AGIDL_RGB(0,10,14,AGIDL_RGB_555); icp[134] = AGIDL_RGB(0,7,14,AGIDL_RGB_555); icp[135] = AGIDL_RGB(0,3,14,AGIDL_RGB_555); 
	icp[136] = AGIDL_RGB(0,0,1,AGIDL_RGB_555); icp[137] = AGIDL_RGB(0,0,1,AGIDL_RGB_555); icp[138] = AGIDL_RGB(1,0,1,AGIDL_RGB_555); icp[139] = AGIDL_RGB(1,0,1,AGIDL_RGB_555); icp[140] = AGIDL_RGB(1,0,1,AGIDL_RGB_555); icp[141] = AGIDL_RGB(1,0,1,AGIDL_RGB_555); icp[142] = AGIDL_RGB(1,0,0,AGIDL_RGB_555); icp[143] = AGIDL_RGB(1,0,0,AGIDL_RGB_555); icp[144] = AGIDL_RGB(1,0,0,AGIDL_RGB_555); icp[145] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[146] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[147] = AGIDL_RGB(1,0,0,AGIDL_RGB_555); icp[148] = AGIDL_RGB(1,1,0,AGIDL_RGB_555); icp[149] = AGIDL_RGB(1,1,0,AGIDL_RGB_555); icp[150] = AGIDL_RGB(1,1,0,AGIDL_RGB_555); icp[151] = AGIDL_RGB(0,1,0,AGIDL_RGB_555); icp[152] = AGIDL_RGB(0,1,0,AGIDL_RGB_555); 
	icp[153] = AGIDL_RGB(5,3,3,AGIDL_RGB_555); icp[154] = AGIDL_RGB(5,4,3,AGIDL_RGB_555); icp[155] = AGIDL_RGB(5,4,3,AGIDL_RGB_555); icp[156] = AGIDL_RGB(5,5,3,AGIDL_RGB_555); icp[157] = AGIDL_RGB(5,5,3,AGIDL_RGB_555); icp[158] = AGIDL_RGB(5,5,3,AGIDL_RGB_555); icp[159] = AGIDL_RGB(4,5,3,AGIDL_RGB_555); icp[160] = AGIDL_RGB(4,5,3,AGIDL_RGB_555); icp[161] = AGIDL_RGB(3,5,3,AGIDL_RGB_555); icp[162] = AGIDL_RGB(1,1,1,AGIDL_RGB_555); icp[163] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[164] = AGIDL_RGB(3,4,3,AGIDL_RGB_555); icp[165] = AGIDL_RGB(4,5,5,AGIDL_RGB_555); icp[166] = AGIDL_RGB(3,5,5,AGIDL_RGB_555); icp[167] = AGIDL_RGB(3,5,5,AGIDL_RGB_555); icp[168] = AGIDL_RGB(3,4,5,AGIDL_RGB_555); icp[169] = AGIDL_RGB(3,4,5,AGIDL_RGB_555); 
	icp[170] = AGIDL_RGB(0,0,8,AGIDL_RGB_555); icp[171] = AGIDL_RGB(2,0,8,AGIDL_RGB_555); icp[172] = AGIDL_RGB(4,0,8,AGIDL_RGB_555); icp[173] = AGIDL_RGB(6,0,8,AGIDL_RGB_555); icp[174] = AGIDL_RGB(8,0,8,AGIDL_RGB_555); icp[175] = AGIDL_RGB(8,0,6,AGIDL_RGB_555); icp[176] = AGIDL_RGB(8,0,4,AGIDL_RGB_555); icp[177] = AGIDL_RGB(8,0,2,AGIDL_RGB_555); icp[178] = AGIDL_RGB(8,0,0,AGIDL_RGB_555); icp[179] = AGIDL_RGB(2,0,0,AGIDL_RGB_555); icp[180] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[181] = AGIDL_RGB(7,3,0,AGIDL_RGB_555); icp[182] = AGIDL_RGB(9,6,0,AGIDL_RGB_555); icp[183] = AGIDL_RGB(8,8,0,AGIDL_RGB_555); icp[184] = AGIDL_RGB(6,8,0,AGIDL_RGB_555); icp[185] = AGIDL_RGB(4,8,0,AGIDL_RGB_555); icp[186] = AGIDL_RGB(2,8,0,AGIDL_RGB_555); 
	icp[187] = AGIDL_RGB(0,8,0,AGIDL_RGB_555); icp[188] = AGIDL_RGB(0,8,2,AGIDL_RGB_555); icp[189] = AGIDL_RGB(0,8,4,AGIDL_RGB_555); icp[190] = AGIDL_RGB(0,8,6,AGIDL_RGB_555); icp[191] = AGIDL_RGB(0,8,8,AGIDL_RGB_555); icp[192] = AGIDL_RGB(0,6,8,AGIDL_RGB_555); icp[193] = AGIDL_RGB(0,4,8,AGIDL_RGB_555); icp[194] = AGIDL_RGB(0,2,8,AGIDL_RGB_555); icp[195] = AGIDL_RGB(4,4,8,AGIDL_RGB_555); icp[196] = AGIDL_RGB(1,1,2,AGIDL_RGB_555); icp[197] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[198] = AGIDL_RGB(5,3,7,AGIDL_RGB_555); icp[199] = AGIDL_RGB(7,4,8,AGIDL_RGB_555); icp[200] = AGIDL_RGB(8,4,8,AGIDL_RGB_555); icp[201] = AGIDL_RGB(8,4,7,AGIDL_RGB_555); icp[202] = AGIDL_RGB(8,4,6,AGIDL_RGB_555); icp[203] = AGIDL_RGB(8,4,5,AGIDL_RGB_555); 
	icp[204] = AGIDL_RGB(8,4,4,AGIDL_RGB_555); icp[205] = AGIDL_RGB(8,5,4,AGIDL_RGB_555); icp[206] = AGIDL_RGB(8,6,4,AGIDL_RGB_555); icp[207] = AGIDL_RGB(8,7,4,AGIDL_RGB_555); icp[208] = AGIDL_RGB(8,8,4,AGIDL_RGB_555); icp[209] = AGIDL_RGB(7,8,4,AGIDL_RGB_555); icp[210] = AGIDL_RGB(6,8,4,AGIDL_RGB_555); icp[211] = AGIDL_RGB(5,8,4,AGIDL_RGB_555); icp[212] = AGIDL_RGB(4,8,4,AGIDL_RGB_555); icp[213] = AGIDL_RGB(1,2,1,AGIDL_RGB_555); icp[214] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[215] = AGIDL_RGB(3,7,5,AGIDL_RGB_555); icp[216] = AGIDL_RGB(4,8,7,AGIDL_RGB_555); icp[217] = AGIDL_RGB(4,8,8,AGIDL_RGB_555); icp[218] = AGIDL_RGB(4,7,8,AGIDL_RGB_555); icp[219] = AGIDL_RGB(4,6,8,AGIDL_RGB_555); icp[220] = AGIDL_RGB(4,5,8,AGIDL_RGB_555); 
	icp[221] = AGIDL_RGB(5,5,8,AGIDL_RGB_555); icp[222] = AGIDL_RGB(6,5,8,AGIDL_RGB_555); icp[223] = AGIDL_RGB(6,5,8,AGIDL_RGB_555); icp[224] = AGIDL_RGB(7,5,8,AGIDL_RGB_555); icp[225] = AGIDL_RGB(8,5,8,AGIDL_RGB_555); icp[226] = AGIDL_RGB(8,5,7,AGIDL_RGB_555); icp[227] = AGIDL_RGB(8,5,6,AGIDL_RGB_555); icp[228] = AGIDL_RGB(8,5,6,AGIDL_RGB_555); icp[229] = AGIDL_RGB(8,5,5,AGIDL_RGB_555); icp[230] = AGIDL_RGB(2,1,1,AGIDL_RGB_555); icp[231] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[232] = AGIDL_RGB(7,5,4,AGIDL_RGB_555); icp[233] = AGIDL_RGB(8,7,5,AGIDL_RGB_555); icp[234] = AGIDL_RGB(8,8,5,AGIDL_RGB_555); icp[235] = AGIDL_RGB(7,8,5,AGIDL_RGB_555); icp[236] = AGIDL_RGB(6,8,5,AGIDL_RGB_555); icp[237] = AGIDL_RGB(6,8,5,AGIDL_RGB_555); 
	icp[238] = AGIDL_RGB(5,8,5,AGIDL_RGB_555); icp[239] = AGIDL_RGB(5,8,6,AGIDL_RGB_555); icp[240] = AGIDL_RGB(5,8,6,AGIDL_RGB_555); icp[241] = AGIDL_RGB(5,8,7,AGIDL_RGB_555); icp[242] = AGIDL_RGB(5,8,8,AGIDL_RGB_555); icp[243] = AGIDL_RGB(5,7,8,AGIDL_RGB_555); icp[244] = AGIDL_RGB(5,6,8,AGIDL_RGB_555); icp[245] = AGIDL_RGB(5,6,8,AGIDL_RGB_555); icp[246] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[247] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[248] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[249] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[250] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[251] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[252] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[253] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); icp[254] = AGIDL_RGB(0,0,0,AGIDL_RGB_555); 

	AGIDL_ICP pal;
	pal.mode = AGIDL_ICP_16b_256;
	pal.fmt = AGIDL_RGB_555;

	for(int i = 0; i < 256; i++){
		pal.icp.palette_16b_256[i] = icp[i];
	}
	
	return pal;
}

void AGIDL_SetY(AGIDL_YCbCr *ycbcr, const u8 y){
	ycbcr->y = y;
}

void AGIDL_SetCb(AGIDL_YCbCr *ycbcr, const u8 cb){
	ycbcr->cb = cb;
}

void AGIDL_SetCr(AGIDL_YCbCr *ycbcr, const u8 cr){
	ycbcr->cr = cr;
}

void AGIDL_SetYCbCr(AGIDL_YCbCr *ycbcr, const u8 y, const u8 cb, const u8 cr){
	AGIDL_SetY(ycbcr,y);
	AGIDL_SetCb(ycbcr,cb);
	AGIDL_SetCr(ycbcr,cr);
}
