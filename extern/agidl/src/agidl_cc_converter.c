#include <agidl_cc_converter.h>
#include <agidl_cc_manager.h>
#include <agidl_img_types.h>

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_cc_converter.c
*   Date: 9/9/2023
*   Version: 0.4b
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/

COLOR AGIDL_CLR16_TO_CLR(COLOR16 clr, AGIDL_CLR_FMT srcfmt, AGIDL_CLR_FMT destfmt){
	u8 r, g, b, newR, newG, newB;
	
	r = AGIDL_GetR(clr,srcfmt);
	g = AGIDL_GetG(clr,srcfmt);
	b = AGIDL_GetB(clr,srcfmt);
	
	if(srcfmt == AGIDL_RGB_565 || srcfmt == AGIDL_BGR_565){
		newR = r << 3;
		newG = g << 2;
		newB = b << 3;
		
		newR |= newR >> 5;
		newG |= newG >> 6;
		newB |= newB >> 5;
		
		return AGIDL_RGB(newR,newG,newB,destfmt);
	}
	else if(AGIDL_GetBitCount(destfmt) != 32){
		newR = r << 3;
		newG = g << 3;
		newB = b << 3;
		
		newR |= newR >> 5;
		newG |= newG >> 5;
		newB |= newB >> 5;
		
		return AGIDL_RGB(newR,newG,newB,destfmt);
	}
	else{
		newR = r << 3;
		newG = g << 3;
		newB = b << 3;
		
		newR |= newR >> 5;
		newG |= newG >> 5;
		newB |= newB >> 5;
		
		return AGIDL_RGBA(newR,newG,newB,0xff,destfmt);
	}
	
	
}

COLOR16 AGIDL_CLR_TO_CLR16(COLOR clr, AGIDL_CLR_FMT srcfmt, AGIDL_CLR_FMT destfmt){
	u8 r = AGIDL_GetR(clr,srcfmt);
	u8 g = AGIDL_GetG(clr,srcfmt);
	u8 b = AGIDL_GetB(clr,srcfmt);
	
	u8 newR, newG, newB;
	
	if(destfmt == AGIDL_RGB_565 || srcfmt == AGIDL_BGR_565){
		newR = r >> 3;
		newG = g >> 2;
		newB = b >> 3;
		
		newR |= newR >> 5;
		newG |= newG >> 6;
		newB |= newB >> 5;
	}
	else{
		newR = r >> 3;
		newG = g >> 3;
		newB = b >> 3;
		
		newR |= newR >> 5;
		newG |= newG >> 5;
		newB |= newB >> 5;
	}
	
	return AGIDL_RGB16(newR,newG,newB,destfmt);
}

COLOR AGIDL_RGB_TO_BGR(COLOR rgb, AGIDL_CLR_FMT fmt){
	u8 r = AGIDL_GetR(rgb,fmt);
	u8 g = AGIDL_GetG(rgb,fmt);
	u8 b = AGIDL_GetB(rgb,fmt);
	u8 a = (rgb >> 15) & 0x1;
	
	if(fmt == AGIDL_RGB_888){
		return AGIDL_RGB(r,g,b,AGIDL_BGR_888);
	}
	else if(fmt == AGIDL_RGB_555){
		return a << 15 | AGIDL_RGB16(r,g,b,AGIDL_BGR_555);
	}
	else if(fmt == AGIDL_RGB_565){
		return AGIDL_RGB16(r,g,b,AGIDL_BGR_565);
	}
	else return 0;
}

COLOR AGIDL_BGR_TO_RGB(COLOR bgr, AGIDL_CLR_FMT fmt){
	u8 r = AGIDL_GetR(bgr,fmt);
	u8 g = AGIDL_GetG(bgr,fmt);
	u8 b = AGIDL_GetB(bgr,fmt);
	u8 a = (bgr >> 15) & 0x1;
	
	if(fmt == AGIDL_BGR_888){
		return AGIDL_RGB(r,g,b,AGIDL_RGB_888);
	}
	else if(fmt == AGIDL_BGR_555){
		return a << 15 | AGIDL_RGB16(r,g,b,AGIDL_RGB_555);
	}
	else if(fmt == AGIDL_BGR_565){
		return AGIDL_RGB16(r,g,b,AGIDL_RGB_565);
	}
	else return 0;
}

COLOR16 AGIDL_555_TO_565(COLOR16 clr, AGIDL_CLR_FMT fmt){
	u8 r = AGIDL_GetR(clr,fmt);
	u8 g = AGIDL_GetG(clr,fmt);
	u8 b = AGIDL_GetB(clr,fmt);
	
	g = g << 1;
	g |= g >> 5;
	
	if(fmt == AGIDL_RGB_555){
		return AGIDL_RGB16(r,g,b,AGIDL_RGB_565);
	}
	else return AGIDL_RGB16(r,g,b,AGIDL_BGR_565);
}

COLOR16 AGIDL_565_TO_555(COLOR16 clr, AGIDL_CLR_FMT fmt){
	u8 r = AGIDL_GetR(clr,fmt);
	u8 g = AGIDL_GetG(clr,fmt);
	u8 b = AGIDL_GetB(clr,fmt);
	
	g = g >> 1;
	g |= g >> 5;
	
	if(fmt == AGIDL_RGB_565){
		return AGIDL_RGB16(r,g,b,AGIDL_RGB_555);
	}
	else return AGIDL_RGB16(r,g,b,AGIDL_BGR_555);
}

COLOR AGIDL_RGBA_TO_RGB(COLOR rgba, AGIDL_CLR_FMT src, AGIDL_CLR_FMT dest){
	u8 r = AGIDL_GetR(rgba,src);
	u8 g = AGIDL_GetG(rgba,src);
	u8 b = AGIDL_GetB(rgba,src);
	
	if(AGIDL_GetBitCount(dest) == 24){
		return AGIDL_RGB(r,g,b,dest);
	}
	else{
		return AGIDL_CLR_TO_CLR16(rgba,src,dest);
	}
}

COLOR AGIDL_RGB_TO_RGBA(COLOR rgb, AGIDL_CLR_FMT src, AGIDL_CLR_FMT dest){
	u8 r = AGIDL_GetR(rgb,src);
	u8 g = AGIDL_GetG(rgb,src);
	u8 b = AGIDL_GetB(rgb,src);
	
	if(AGIDL_GetBitCount(src) == 16){
		COLOR clr = AGIDL_CLR16_TO_CLR(rgb,src,AGIDL_RGB_888);
		r = AGIDL_GetR(clr,AGIDL_RGB_888);
		g = AGIDL_GetG(clr,AGIDL_RGB_888);
		b = AGIDL_GetB(clr,AGIDL_RGB_888);
		return AGIDL_RGBA(r,g,b,0xff,dest);
	}
	else{
		return AGIDL_RGBA(r,g,b,0xff,dest);
	}
}

COLOR AGIDL_ColorConvert(COLOR src, AGIDL_CLR_FMT sfmt, AGIDL_CLR_FMT dfmt){
	if(sfmt == dfmt){
		return src;
	}
	
	u8 r = AGIDL_GetR(src,sfmt);
	u8 g = AGIDL_GetG(src,sfmt);
	u8 b = AGIDL_GetB(src,sfmt);
	u8 a = AGIDL_GetA(src,sfmt);
	
	u8 sbits = AGIDL_GetBitCount(sfmt), dbits = AGIDL_GetBitCount(dfmt);

	if(sbits == dbits){
		
		if(dbits == 24 || (dbits == 16 && sfmt == AGIDL_RGB_555 && dfmt == AGIDL_BGR_555) || (dbits == 16 && sfmt == AGIDL_RGB_565 && dfmt == AGIDL_BGR_565)){
			return AGIDL_RGB(r,g,b,dfmt);
		}
		else if(dbits == 32){
			return AGIDL_RGBA(r,g,b,a,dfmt);
		}
		else{
			if((sfmt == AGIDL_RGB_555 || sfmt == AGIDL_BGR_555) && (dfmt == AGIDL_BGR_565 || dfmt == AGIDL_RGB_565)){
				
				COLOR16 dest = AGIDL_555_TO_565(src,sfmt);
				
				if(sfmt == AGIDL_RGB_555 && dfmt == AGIDL_BGR_565){
					return AGIDL_RGB_TO_BGR(dest,AGIDL_RGB_565);
				}
				else if(sfmt == AGIDL_BGR_555 && dfmt == AGIDL_RGB_565){
					return AGIDL_BGR_TO_RGB(dest,AGIDL_BGR_565);
				}
				else return dest;
			}
			else{
				COLOR16 dest = AGIDL_565_TO_555(src,sfmt);
				
				if(sfmt == AGIDL_RGB_565 && dfmt == AGIDL_BGR_555){
					return AGIDL_RGB_TO_BGR(dest,AGIDL_RGB_555);
				}
				else if(sfmt == AGIDL_BGR_565 && dfmt == AGIDL_RGB_555){
					return AGIDL_BGR_TO_RGB(dest,AGIDL_BGR_555);
				}
				else return dest;
			}
		}
	}
	else if(sbits == 24 && dbits == 32){
		return AGIDL_RGBA(r,g,b,0xff,dfmt);
	}
	else if(sbits == 32 && dbits == 24){
		return AGIDL_RGB(r,g,b,dfmt);
	}
	else if((sbits == 24 || sbits == 32) && dbits == 16){
		return AGIDL_CLR_TO_CLR16(src,sfmt,dfmt);
	}
	else if(sbits == 16 && (dbits == 24 || dbits == 32)){
		return AGIDL_CLR16_TO_CLR(src,sfmt,dfmt);
	}
	else return src;
}