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
*   Version: 0.1b
*   Updated: 2/21/2024
*   Author: Ryandracus Chapman
*
********************************************/

COLOR AGIDL_CLR16_TO_CLR(const COLOR16 clr, const AGIDL_CLR_FMT srcfmt, const AGIDL_CLR_FMT destfmt){
	const u8 r = AGIDL_GetR(clr,srcfmt);
	const u8 g = AGIDL_GetG(clr,srcfmt);
	const u8 b = AGIDL_GetB(clr,srcfmt);
	
	u8 newR, newG, newB;
	
	if(srcfmt == AGIDL_RGB_565 || srcfmt == AGIDL_BGR_565){
		newR = r << 3;
		newG = g << 2;
		newB = b << 3;
		
		newR |= newR >> 5;
		newG |= newG >> 6;
		newB |= newB >> 5;
		
		return AGIDL_RGB(newR,newG,newB,destfmt);
	}
	if(AGIDL_GetBitCount(destfmt) != 32){
		newR = r << 3;
		newG = g << 3;
		newB = b << 3;
		
		newR |= newR >> 5;
		newG |= newG >> 5;
		newB |= newB >> 5;
		
		return AGIDL_RGB(newR,newG,newB,destfmt);
	}
	newR = r << 3;
	newG = g << 3;
	newB = b << 3;
		
	newR |= newR >> 5;
	newG |= newG >> 5;
	newB |= newB >> 5;
		
	return AGIDL_RGBA(newR,newG,newB,0x55,destfmt);
}

COLOR16 AGIDL_CLR_TO_CLR16(const COLOR clr, const AGIDL_CLR_FMT srcfmt, const AGIDL_CLR_FMT destfmt){
	const u8 r = AGIDL_GetR(clr,srcfmt);
	const u8 g = AGIDL_GetG(clr,srcfmt);
	const u8 b = AGIDL_GetB(clr,srcfmt);
	
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

COLOR AGIDL_RGB_TO_BGR(const COLOR rgb, const AGIDL_CLR_FMT fmt){
	const u8 r = AGIDL_GetR(rgb,fmt);
	const u8 g = AGIDL_GetG(rgb,fmt);
	const u8 b = AGIDL_GetB(rgb,fmt);
	const u8 a = (rgb >> 15) & 0x1;
	
	if(fmt == AGIDL_RGB_888){
		return AGIDL_RGB(r,g,b,AGIDL_BGR_888);
	}
	if(fmt == AGIDL_RGB_555){
		return a << 15 | AGIDL_RGB16(r,g,b,AGIDL_BGR_555);
	}
	if(fmt == AGIDL_RGB_565){
		return AGIDL_RGB16(r,g,b,AGIDL_BGR_565);
	}
	return 0;
}

COLOR AGIDL_BGR_TO_RGB(const COLOR bgr, const AGIDL_CLR_FMT fmt){
	const u8 r = AGIDL_GetR(bgr,fmt);
	const u8 g = AGIDL_GetG(bgr,fmt);
	const u8 b = AGIDL_GetB(bgr,fmt);
	const u8 a = (bgr >> 15) & 0x1;
	
	if(fmt == AGIDL_BGR_888){
		return AGIDL_RGB(r,g,b,AGIDL_RGB_888);
	}
	if(fmt == AGIDL_BGR_555){
		return a << 15 | AGIDL_RGB16(r,g,b,AGIDL_RGB_555);
	}
	if(fmt == AGIDL_BGR_565){
		return AGIDL_RGB16(r,g,b,AGIDL_RGB_565);
	}
	return 0;
}

COLOR16 AGIDL_555_TO_565(const COLOR16 clr, const AGIDL_CLR_FMT fmt){
	const u8 r = AGIDL_GetR(clr,fmt);
	u8 g = AGIDL_GetG(clr,fmt);
	const u8 b = AGIDL_GetB(clr,fmt);
	
	g = g << 1;
	g |= g >> 5;
	
	if(fmt == AGIDL_RGB_555){
		return AGIDL_RGB16(r,g,b,AGIDL_RGB_565);
	}
	return AGIDL_RGB16(r,g,b,AGIDL_BGR_565);
}

COLOR16 AGIDL_565_TO_555(const COLOR16 clr, const AGIDL_CLR_FMT fmt){
	const u8 r = AGIDL_GetR(clr,fmt);
	u8 g = AGIDL_GetG(clr,fmt);
	const u8 b = AGIDL_GetB(clr,fmt);
	
	g = g >> 1;
	g |= g >> 5;
	
	if(fmt == AGIDL_RGB_565){
		return AGIDL_RGB16(r,g,b,AGIDL_RGB_555);
	}
	return AGIDL_RGB16(r,g,b,AGIDL_BGR_555);
}

AGIDL_YCbCr AGIDL_CLR_TO_YCbCr(const COLOR clr, const AGIDL_CLR_FMT fmt){
	const u8 r = AGIDL_GetR(clr,fmt);
	const u8 g = AGIDL_GetG(clr,fmt);
	const u8 b = AGIDL_GetB(clr,fmt);
	
	return AGIDL_RGB_TO_YCbCr(r,g,b);
}

COLOR AGIDL_RGBA_TO_RGB(const COLOR rgba, const AGIDL_CLR_FMT src, const AGIDL_CLR_FMT dest){
	const u8 r = AGIDL_GetR(rgba,src);
	const u8 g = AGIDL_GetG(rgba,src);
	const u8 b = AGIDL_GetB(rgba,src);
	
	if(AGIDL_GetBitCount(dest) == 24){
		return AGIDL_RGB(r,g,b,dest);
	}
	return AGIDL_CLR_TO_CLR16(rgba,src,dest);
}

COLOR AGIDL_RGB_TO_RGBA(const COLOR rgb, const AGIDL_CLR_FMT src, const AGIDL_CLR_FMT dest){
	u8 r = AGIDL_GetR(rgb,src);
	u8 g = AGIDL_GetG(rgb,src);
	u8 b = AGIDL_GetB(rgb,src);
	
	if(AGIDL_GetBitCount(src) == 16){
		const COLOR clr = AGIDL_CLR16_TO_CLR(rgb,src,AGIDL_RGB_888);
		r = AGIDL_GetR(clr,AGIDL_RGB_888);
		g = AGIDL_GetG(clr,AGIDL_RGB_888);
		b = AGIDL_GetB(clr,AGIDL_RGB_888);
		return AGIDL_RGBA(r,g,b,0xff,dest);
	}
	return AGIDL_RGBA(r,g,b,0xff,dest);
}

COLOR AGIDL_ColorConvert(const COLOR src, const AGIDL_CLR_FMT sfmt, const AGIDL_CLR_FMT dfmt){
	if(sfmt == dfmt){
		return src;
	}

	const u8 r = AGIDL_GetR(src,sfmt);
	const u8 g = AGIDL_GetG(src,sfmt);
	const u8 b = AGIDL_GetB(src,sfmt);
	const u8 a = AGIDL_GetA(src,sfmt);

	const u8 sbits = AGIDL_GetBitCount(sfmt), dbits = AGIDL_GetBitCount(dfmt);

	if(sbits == dbits){
		
		if(dbits == 24 || (dbits == 16 && sfmt == AGIDL_RGB_555 && dfmt == AGIDL_BGR_555) || (dbits == 16 && sfmt == AGIDL_RGB_565 && dfmt == AGIDL_BGR_565)){
			return AGIDL_RGB(r,g,b,dfmt);
		}
		if(dbits == 32){
			return AGIDL_RGBA(r,g,b,a,dfmt);
		}
		if((sfmt == AGIDL_RGB_555 || sfmt == AGIDL_BGR_555) && (dfmt == AGIDL_BGR_565 || dfmt == AGIDL_RGB_565)){
			const COLOR16 dest = AGIDL_555_TO_565(src,sfmt);
				
			if(sfmt == AGIDL_RGB_555 && dfmt == AGIDL_BGR_565){
				return AGIDL_RGB_TO_BGR(dest,AGIDL_RGB_565);
			}
			if(sfmt == AGIDL_BGR_555 && dfmt == AGIDL_RGB_565){
				return AGIDL_BGR_TO_RGB(dest,AGIDL_BGR_565);
			}
			return dest;
		}
		const COLOR16 dest = AGIDL_565_TO_555(src,sfmt);
				
		if(sfmt == AGIDL_RGB_565 && dfmt == AGIDL_BGR_555){
			return AGIDL_RGB_TO_BGR(dest,AGIDL_RGB_555);
		}
		if(sfmt == AGIDL_BGR_565 && dfmt == AGIDL_RGB_555){
			return AGIDL_BGR_TO_RGB(dest,AGIDL_BGR_555);
		}
		return dest;
	}
	if(sbits == 24 && dbits == 32){
		return AGIDL_RGBA(r,g,b,0xff,dfmt);
	}
	if(sbits == 32 && dbits == 24){
		return AGIDL_RGB(r,g,b,dfmt);
	}
	if((sbits == 24 || sbits == 32) && dbits == 16){
		return AGIDL_CLR_TO_CLR16(src,sfmt,dfmt);
	}
	if(sbits == 16 && (dbits == 24 || dbits == 32)){
		return AGIDL_CLR16_TO_CLR(src,sfmt,dfmt);
	}
	return src;
}

AGIDL_YCbCr AGIDL_RGB_TO_YCbCr(const u8 r, const u8 g, const u8 b){
	AGIDL_YCbCr ycbcr;
	ycbcr.y = 0.299f*r + 0.587f*g + 0.114f*b;
	ycbcr.cb = -0.1687*r - 0.3313f*g + 0.5f*b + 128;
	ycbcr.cr = 0.5f*r - 0.4187f*g - 0.0813f*b + 128;
	return ycbcr;
}

COLOR AGIDL_YCbCr_TO_CLR(const AGIDL_YCbCr ycbcr, const AGIDL_CLR_FMT fmt){
	const u8 r = ycbcr.y + 1.402f * (ycbcr.cr - 128);
	const u8 g = ycbcr.y - 0.34414f * (ycbcr.cb - 128) - 0.71414 * (ycbcr.cr - 128);
	const u8 b = ycbcr.y + 0.1772f * (ycbcr.cb - 128);
	
	return AGIDL_RGB(r,g,b,fmt);
}

AGIDL_CMYK AGIDL_RGB_TO_CMYK(const u8 r, const u8 g, const u8 b){
	const float newR = r / 255.0f;
	const float newG = g / 255.0f;
	const float newB = b / 255.0f;
	
	float max;
	
	if(newR > newG){
		if(newR > newB){
			max = newR;
		}
		else{
			max = newB;
		}
	}
	else{
		if(newG > newB){
			max = newG;
		}
		else{
			max = newB;
		}
	}
	
	AGIDL_CMYK cmyk;
	cmyk.k = 1 - max;
	cmyk.c = (1-newR-cmyk.k)/(1.0f-cmyk.k);
	cmyk.m = (1-newG-cmyk.k)/(1.0f-cmyk.k);
	cmyk.y = (1-newB-cmyk.k)/(1.0f-cmyk.k);
	
	return cmyk;
}

AGIDL_CMYK AGIDL_CLR_TO_CMYK(const COLOR clr, const AGIDL_CLR_FMT fmt){
	const u8 r = AGIDL_GetR(clr,fmt);
	const u8 g = AGIDL_GetG(clr,fmt);
	const u8 b = AGIDL_GetB(clr,fmt);
	
	return AGIDL_RGB_TO_CMYK(r,g,b);
}

COLOR AGIDL_CMYK_TO_CLR(const AGIDL_CMYK cmyk, const AGIDL_CLR_FMT fmt){
	const u8 r = 255 * (1 - cmyk.c) * (1 - cmyk.k);
	const u8 g = 255 * (1 - cmyk.m) * (1 - cmyk.k);
	const u8 b = 255 * (1 - cmyk.k) * (1 - cmyk.k);
	return AGIDL_RGB(r,g,b,fmt);
}
