#include <agidl_cc_mixer.h>

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_cc_mixer.c
*   Date: 9/10/2023
*   Version: 0.1b
*   Updated: 2/11/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_img_types.h>
#include <agidl_math_utils.h>

float AGIDL_FindRBDivisor(const AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_RGB_888:{
			return 255.0f;
		}break;
		case AGIDL_BGR_888:{
			return 255.0f;
		}break;
		case AGIDL_RGB_555:{
			return 31.0f;
		}break;
		case AGIDL_BGR_555:{
			return 31.0f;
		}break;
		case AGIDL_RGB_565:{
			return 31.0f;
		}break;
		case AGIDL_BGR_565:{
			return 31.0f;
		}break;
		case AGIDL_RGBA_8888:{
			return 255.0f;
		}break;
		case AGIDL_ARGB_8888:{
			return 255.0f;
		}break;
	}
	return 0;
}

float AGIDL_FindGDivisor(const AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_RGB_888:{
			return 255.0f;
		}break;
		case AGIDL_BGR_888:{
			return 255.0f;
		}break;
		case AGIDL_RGB_555:{
			return 31.0f;
		}break;
		case AGIDL_BGR_555:{
			return 31.0f;
		}break;
		case AGIDL_RGB_565:{
			return 63.0f;
		}break;
		case AGIDL_BGR_565:{
			return 63.0f;
		}break;
		case AGIDL_RGBA_8888:{
			return 255.0f;
		}break;
		case AGIDL_ARGB_8888:{
			return 255.0f;
		}break;
	}
	return 0;
}

u8 AGIDL_MulCombine565(const u8 cc1, const u8 cc2){
	return (cc1*cc2)/63.0f;
}

u8 AGIDL_MulCombine(const u8 cc1, const u8 cc2, const AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_RGB_888:{
			return (cc1*cc2)/255.0f;
		}break;
		case AGIDL_BGR_888:{
			return (cc1*cc2)/255.0f;
		}break;
		case AGIDL_RGB_555:{
			return (cc1*cc2)/31.0f;
		}break;
		case AGIDL_BGR_555:{
			return (cc1*cc2)/31.0f;
		}break;
		case AGIDL_RGB_565:{
			return (cc1*cc2)/31.0f;
		}break;
		case AGIDL_BGR_565:{
			return (cc1*cc2)/31.0f;
		}break;
		case AGIDL_RGBA_8888:{
			return (cc1*cc2)/255.0f;
		}break;
		case AGIDL_ARGB_8888:{
			return (cc1*cc2)/255.0f;
		}break;
	}
	return 0;
}

COLOR AGIDL_AddColor(const COLOR clr1, const COLOR clr2, const AGIDL_CLR_FMT fmt){
	const u8 r1 = AGIDL_GetR(clr1,fmt);
	const u8 g1 = AGIDL_GetG(clr1,fmt);
	const u8 b1 = AGIDL_GetB(clr1,fmt);

	const u8 r2 = AGIDL_GetR(clr2,fmt);
	const u8 g2 = AGIDL_GetG(clr2,fmt);
	const u8 b2 = AGIDL_GetB(clr2,fmt);

	const u8 r = r1 + r2;
	const u8 g = g1 + g2;
	const u8 b = b1 + b2;

	if(AGIDL_GetBitCount(fmt) == 32){
		const u8 a1 = AGIDL_GetA(clr1,fmt), a2 = AGIDL_GetA(clr2,fmt);
		const u8 a = (a1+a2)/2.0f;
		return AGIDL_RGBA(r,g,b,a,fmt);
	}

	return AGIDL_RGB(r,g,b,fmt);
}

COLOR AGIDL_SubColor(const COLOR clr1, const COLOR clr2, const AGIDL_CLR_FMT fmt){
	const u8 r1 = AGIDL_GetR(clr1,fmt);
	const u8 g1 = AGIDL_GetG(clr1,fmt);
	const u8 b1 = AGIDL_GetB(clr1,fmt);

	const u8 r2 = AGIDL_GetR(clr2,fmt);
	const u8 g2 = AGIDL_GetG(clr2,fmt);
	const u8 b2 = AGIDL_GetB(clr2,fmt);

	const u8 r = r1 - r2;
	const u8 g = g1 - g2;
	const u8 b = b1 - b2;

	if(AGIDL_GetBitCount(fmt) == 32){
		const u8 a1 = AGIDL_GetA(clr1,fmt), a2 = AGIDL_GetA(clr2,fmt);
		const u8 a = (a1+a2)/2.0f;
		return AGIDL_RGBA(r,g,b,a,fmt);
	}

	return AGIDL_RGB(r,g,b,fmt);
}

COLOR AGIDL_MulColor(const COLOR clr1, const COLOR clr2, const AGIDL_CLR_FMT fmt){
	const u8 r1 = AGIDL_GetR(clr1,fmt);
	const u8 g1 = AGIDL_GetG(clr1,fmt);
	const u8 b1 = AGIDL_GetB(clr1,fmt);

	const u8 r2 = AGIDL_GetR(clr2,fmt);
	const u8 g2 = AGIDL_GetG(clr2,fmt);
	const u8 b2 = AGIDL_GetB(clr2,fmt);

	const u8 r = r1 * r2;
	const u8 g = g1 * g2;
	const u8 b = b1 * b2;

	if(AGIDL_GetBitCount(fmt) == 32){
		const u8 a1 = AGIDL_GetA(clr1,fmt), a2 = AGIDL_GetA(clr2,fmt);
		const u8 a = (a1*a2)/255.0f;
		return AGIDL_RGBA(r,g,b,a,fmt);
	}

	return AGIDL_RGB(r,g,b,fmt);
}

COLOR AGIDL_DotColor(const COLOR clr1, float factor, const AGIDL_CLR_FMT fmt){

	factor = AGIDL_Clampf(0.0f,factor,1.0f);

	const u8 r1 = AGIDL_GetR(clr1,fmt);
	const u8 g1 = AGIDL_GetG(clr1,fmt);
	const u8 b1 = AGIDL_GetB(clr1,fmt);

	const u8 r = r1 * factor;
	const u8 g = g1 * factor;
	const u8 b = b1 * factor;

	if(AGIDL_GetBitCount(fmt) == 32){
		const u8 a1 = AGIDL_GetA(clr1,fmt);
		return AGIDL_RGBA(r,g,b,a1,fmt);
	}

	return AGIDL_RGB(r,g,b,fmt);
}


COLOR AGIDL_DivColor(const COLOR clr1, float factor, const AGIDL_CLR_FMT fmt){

	factor = AGIDL_Clampf(0.0f,factor,1.0f);

	const u8 r1 = AGIDL_GetR(clr1,fmt);
	const u8 g1 = AGIDL_GetG(clr1,fmt);
	const u8 b1 = AGIDL_GetB(clr1,fmt);

	const u8 r = r1 / factor;
	const u8 g = g1 / factor;
	const u8 b = b1 / factor;

	if(AGIDL_GetBitCount(fmt) == 32){
		const u8 a1 = AGIDL_GetA(clr1,fmt);
		return AGIDL_RGBA(r,g,b,a1,fmt);
	}

	return AGIDL_RGB(r,g,b,fmt);
}

COLOR AGIDL_InterpColor(const COLOR clr1, const COLOR clr2, const float interp, const AGIDL_CLR_FMT fmt){
	const u8 r1 = AGIDL_GetR(clr1,fmt);
	const u8 g1 = AGIDL_GetG(clr1,fmt);
	const u8 b1 = AGIDL_GetB(clr1,fmt);

	const u8 r2 = AGIDL_GetR(clr2,fmt);
	const u8 g2 = AGIDL_GetG(clr2,fmt);
	const u8 b2 = AGIDL_GetB(clr2,fmt);

	const u8 r = r1 + interp * (r2 - r1);
	const u8 g = g1 + interp * (g2 - g1);
	const u8 b = b1 + interp * (b2 - b1);

	return AGIDL_RGB(r,g,b,fmt);
}

COLOR AGIDL_BilerpColor(const COLOR clr1, const COLOR clr2, const COLOR clr3, const COLOR clr4, const float interp, const float bilerp, const AGIDL_CLR_FMT fmt){
	const COLOR top = AGIDL_InterpColor(clr1,clr2,interp,fmt);
	const COLOR bottom = AGIDL_InterpColor(clr3,clr4,interp,fmt);

	return AGIDL_InterpColor(top,bottom,bilerp,fmt);
}

COLOR AGIDL_GrayscaleColor(const COLOR clr, const AGIDL_CLR_FMT fmt){
	const u8 r = AGIDL_GetR(clr,fmt);
	const u8 g = AGIDL_GetG(clr,fmt);
	const u8 b = AGIDL_GetB(clr,fmt);

	const u8 rgb = (u8)((r+g+b)/3.0f);

	if(AGIDL_GetBitCount(fmt) == 32){
		const u8 a = AGIDL_GetA(clr,fmt);
		return AGIDL_RGBA(rgb,rgb,rgb,a,fmt);
	}

	return AGIDL_RGB(rgb,rgb,rgb,fmt);
}

COLOR AGIDL_BlendColor(const COLOR clr1, const COLOR clr2, float blend_factor, const int blend_mode, const AGIDL_CLR_FMT fmt){
	if(blend_factor > 1.0f){
		blend_factor = 1.0f;
	}

	if(blend_factor < 0.0f){
		blend_factor = 0.0f;
	}

	float iblend_factor = 1.0f - blend_factor;

	const u8 r1 = AGIDL_GetR(clr1,fmt);
	const u8 g1 = AGIDL_GetG(clr1,fmt);
	const u8 b1 = AGIDL_GetB(clr1,fmt);

	const u8 r2 = AGIDL_GetR(clr2,fmt);
	const u8 g2 = AGIDL_GetG(clr2,fmt);
	const u8 b2 = AGIDL_GetB(clr2,fmt);

	if(fmt == AGIDL_RGBA_8888 || fmt == AGIDL_ARGB_8888){
		const u8 a1 = AGIDL_GetA(clr1,fmt), a2 = AGIDL_GetA(clr2,fmt);

		switch(blend_mode){
			case CC_BLEND_CLR_SRCINV:{
				blend_factor = a1/255.0f;
				iblend_factor = 1.0f - blend_factor;
				const u8 r = r1*iblend_factor + r2*blend_factor;
				const u8 g = g1*iblend_factor + g2*blend_factor;
				const u8 b = b1*iblend_factor + b2*blend_factor;
				const u8 a = a1*iblend_factor + a2*blend_factor;
				return AGIDL_RGBA(r,g,b,a,fmt);
			}break;
			case CC_BLEND_CLR_DESTINV:{
				blend_factor = a2/255.0f;
				iblend_factor = 1.0f - blend_factor;
				const u8 r = r1*iblend_factor + r2*blend_factor;
				const u8 g = g1*iblend_factor + g2*blend_factor;
				const u8 b = b1*iblend_factor + b2*blend_factor;
				const u8 a = a1*iblend_factor + a2*blend_factor;
				return AGIDL_RGBA(r,g,b,a,fmt);
			}break;
			case CC_BLEND_CLR_SRC:{
				blend_factor = a1/255.0f;
				const u8 r = r1*blend_factor + r2;
				const u8 g = g1*blend_factor + g2;
				const u8 b = b1*blend_factor + b2;
				const u8 a = a1*blend_factor + a2;
				return AGIDL_RGBA(r,g,b,a,fmt);
			}break;

			case CC_BLEND_CLR_DEST:{
				blend_factor = a2/255.0f;
				const u8 r = r1*blend_factor + r2;
				const u8 g = g1*blend_factor + g2;
				const u8 b = b1*blend_factor + b2;
				const u8 a = a1*blend_factor + a2;
				return AGIDL_RGBA(r,g,b,a,fmt);
			}break;
			case CC_BLEND_CLR_ISRC:{
				blend_factor = 1.0f - (a1/255.0f);
				const u8 r = r1*blend_factor + r2;
				const u8 g = g1*blend_factor + g2;
				const u8 b = b1*blend_factor + b2;
				const u8 a = a1*blend_factor + a2;
				return AGIDL_RGBA(r,g,b,a,fmt);
			}break;

			case CC_BLEND_CLR_IDEST:{
				blend_factor = 1.0f - (a2/255.0f);
				const u8 r = r1*blend_factor + r2;
				const u8 g = g1*blend_factor + g2;
				const u8 b = b1*blend_factor + b2;
				const u8 a = a1*blend_factor + a2;
				return AGIDL_RGBA(r,g,b,a,fmt);
			}break;
		}
	}

		switch(blend_mode){
			case CC_BLEND_CLR_SRCINV:{
				const u8 r = r1*iblend_factor + r2*blend_factor;
				const u8 g = g1*iblend_factor + g2*blend_factor;
				const u8 b = b1*iblend_factor + b2*blend_factor;
				return AGIDL_RGB(r,g,b,fmt);
			}break;
			case CC_BLEND_CLR_DESTINV:{
				const u8 r = r1*blend_factor + r2*iblend_factor;
				const u8 g = g1*blend_factor + g2*iblend_factor;
				const u8 b = b1*blend_factor + b2*iblend_factor;
				return AGIDL_RGB(r,g,b,fmt);
			}break;
			case CC_BLEND_CLR_SRC:{
				const u8 r = r1*blend_factor + r2;
				const u8 g = g1*blend_factor + g2;
				const u8 b = b1*blend_factor + b2;
				return AGIDL_RGB(r,g,b,fmt);
			}break;

			case CC_BLEND_CLR_DEST:{
				const u8 r = r1 + r2*blend_factor;
				const u8 g = g1 + g2*blend_factor;
				const u8 b = b1 + b2*blend_factor;
				return AGIDL_RGB(r,g,b,fmt);
			}break;
			case CC_BLEND_CLR_ISRC:{
				const u8 r = r1*iblend_factor + r2;
				const u8 g = g1*iblend_factor + g2;
				const u8 b = b1*iblend_factor + b2;
				return AGIDL_RGB(r,g,b,fmt);
			}break;

			case CC_BLEND_CLR_IDEST:{
				const u8 r = r1 + r2*iblend_factor;
				const u8 g = g1 + g2*iblend_factor;
				const u8 b = b1 + b2*iblend_factor;
				return AGIDL_RGB(r,g,b,fmt);
			}break;
	}	
	
	return 0;
}

COLOR AGIDL_ColorCombine(COLOR clr1, COLOR clr2, AGIDL_CC_COMBINE_MODE cc, AGIDL_CLR_FMT fmt){
	u8 r1 = AGIDL_GetR(clr1,fmt);
	u8 g1 = AGIDL_GetG(clr1,fmt);
	u8 b1 = AGIDL_GetB(clr1,fmt);
	
	u8 r2 = AGIDL_GetR(clr2,fmt);
	u8 g2 = AGIDL_GetG(clr2,fmt);
	u8 b2 = AGIDL_GetB(clr2,fmt);
	
	switch(cc){
		case CC_MUL_CLR:{
			u8 r3 = AGIDL_MulCombine(r1,r2,fmt);
			u8 g3 = AGIDL_MulCombine(g1,g2,fmt);
			u8 b3 = AGIDL_MulCombine(b1,b2,fmt);
			return AGIDL_RGB(r3,g3,b3,fmt);
		}break;
		case CC_ADD_CLR:{
			return AGIDL_AddColor(clr1,clr2,fmt);
		}break;
		case CC_SUB_CLR:{
			return AGIDL_SubColor(clr1,clr2,fmt);
		}break;
		case CC_MUL_AND_SUB_CLR:{
			u8 r3 = AGIDL_MulCombine(r1,r2,fmt);
			u8 g3 = AGIDL_MulCombine(g1,g2,fmt);
			u8 b3 = AGIDL_MulCombine(b1,b2,fmt);
			COLOR clr3 = AGIDL_RGB(r3,g3,b3,fmt);
			COLOR clr4 = AGIDL_SubColor(clr1,clr2,fmt);
			float rdiv = AGIDL_FindRBDivisor(fmt);
			float bdiv = AGIDL_FindRBDivisor(fmt);
			float gdiv = AGIDL_FindGDivisor(fmt);
			float interp = 1.0f - ((r3/rdiv)+(g3/gdiv)+(b3/bdiv))/(3.0f);
			return AGIDL_InterpColor(clr3,clr4,interp,fmt);
		}break;
		case CC_MUL_AND_ADD_CLR:{
			u8 r3 = AGIDL_MulCombine(r1,r2,fmt);
			u8 g3 = AGIDL_MulCombine(g1,g2,fmt);
			u8 b3 = AGIDL_MulCombine(b1,b2,fmt);
			COLOR clr3 = AGIDL_RGB(r3,g3,b3,fmt);
			COLOR clr4 = AGIDL_AddColor(clr1,clr2,fmt);
			float rdiv = AGIDL_FindRBDivisor(fmt);
			float bdiv = AGIDL_FindRBDivisor(fmt);
			float gdiv = AGIDL_FindGDivisor(fmt);
			float interp = ((r3/rdiv)+(g3/gdiv)+(b3/bdiv))/3.0f;
			return AGIDL_InterpColor(clr3,clr4,interp,fmt);
		}break;
		case CC_MUL_AND_SUB_CLR1:{
			u8 r3 = AGIDL_MulCombine(r1,r2,fmt);
			u8 g3 = AGIDL_MulCombine(g1,g2,fmt);
			u8 b3 = AGIDL_MulCombine(b1,b2,fmt);
			COLOR clr3 = AGIDL_RGB(r3,g3,b3,fmt);
			COLOR clr4 = AGIDL_SubColor(clr3,clr1,fmt);
			float rdiv = AGIDL_FindRBDivisor(fmt);
			float bdiv = AGIDL_FindRBDivisor(fmt);
			float gdiv = AGIDL_FindGDivisor(fmt);
			float interp = 1.0f - ((r1/rdiv)+(g1/gdiv)+(b1/bdiv))/3.0f;
			return AGIDL_InterpColor(clr3,clr4,interp,fmt);
		}break;
		case CC_MUL_AND_ADD_CLR1:{
			u8 r3 = AGIDL_MulCombine(r1,r2,fmt);
			u8 g3 = AGIDL_MulCombine(g1,g2,fmt);
			u8 b3 = AGIDL_MulCombine(b1,b2,fmt);
			COLOR clr3 = AGIDL_RGB(r3,g3,b3,fmt);
			COLOR clr4 = AGIDL_SubColor(clr3,clr1,fmt);
			float rdiv = AGIDL_FindRBDivisor(fmt);
			float bdiv = AGIDL_FindRBDivisor(fmt);
			float gdiv = AGIDL_FindGDivisor(fmt);
			float interp = ((r1/rdiv)+(g1/gdiv)+(b1/bdiv))/3.0f;
			return AGIDL_InterpColor(clr3,clr4,interp,fmt);
		}break;
		case CC_MUL_AND_SUB_CLR2:{
			u8 r3 = AGIDL_MulCombine(r1,r2,fmt);
			u8 g3 = AGIDL_MulCombine(g1,g2,fmt);
			u8 b3 = AGIDL_MulCombine(b1,b2,fmt);
			COLOR clr3 = AGIDL_RGB(r3,g3,b3,fmt);
			COLOR clr4 = AGIDL_SubColor(clr3,clr2,fmt);
			float rdiv = AGIDL_FindRBDivisor(fmt);
			float bdiv = AGIDL_FindRBDivisor(fmt);
			float gdiv = AGIDL_FindGDivisor(fmt);
			float interp = 1.0f - ((r2/rdiv)+(g2/gdiv)+(b2/bdiv))/3.0f;
			return AGIDL_InterpColor(clr3,clr4,interp,fmt);
		}break;
		case CC_MUL_AND_ADD_CLR2:{
			u8 r3 = AGIDL_MulCombine(r1,r2,fmt);
			u8 g3 = AGIDL_MulCombine(g1,g2,fmt);
			u8 b3 = AGIDL_MulCombine(b1,b2,fmt);
			COLOR clr3 = AGIDL_RGB(r3,g3,b3,fmt);
			COLOR clr4 = AGIDL_SubColor(clr3,clr2,fmt);
			float rdiv = AGIDL_FindRBDivisor(fmt);
			float bdiv = AGIDL_FindRBDivisor(fmt);
			float gdiv = AGIDL_FindGDivisor(fmt);
			float interp = ((r2/rdiv)+(g2/gdiv)+(b2/bdiv))/3.0f;
			return AGIDL_InterpColor(clr3,clr4,interp,fmt);
		}break;
		case CC_INTERP_CLR:{
			float rdiv = AGIDL_FindRBDivisor(fmt);
			float bdiv = AGIDL_FindRBDivisor(fmt);
			float gdiv = AGIDL_FindGDivisor(fmt);
			float interp1 = ((r1/rdiv)+(g1/gdiv)+(b1/bdiv))/3.0f;
			float interp2 = ((r2/rdiv)+(g2/gdiv)+(b2/bdiv))/3.0f;
			float interp = (interp1+interp2)/2.0f;
			return AGIDL_InterpColor(clr1,clr2,interp,fmt);
		}break;
		case CC_MUL_AND_INTERP_CLR1:{
			u8 r3 = AGIDL_MulCombine(r1,r2,fmt);
			u8 g3 = AGIDL_MulCombine(g1,g2,fmt);
			u8 b3 = AGIDL_MulCombine(b1,b2,fmt);
			COLOR clr3 = AGIDL_RGB(r3,g3,b3,fmt);
			float rdiv = AGIDL_FindRBDivisor(fmt);
			float bdiv = AGIDL_FindRBDivisor(fmt);
			float gdiv = AGIDL_FindGDivisor(fmt);
			float interp1 = ((r1/rdiv)+(g1/gdiv)+(b1/bdiv))/3.0f;
			float interp2 = ((r3/rdiv)+(g3/gdiv)+(b3/bdiv))/3.0f;
			float interp = (interp1+interp2)/(2.0f);
			return AGIDL_InterpColor(clr3,clr1,interp,fmt);
		}break;
		case CC_ADD_AND_INTERP_CLR1:{
			COLOR clr3 = AGIDL_AddColor(clr1,clr2,fmt);
			u8 r3 = AGIDL_GetR(clr3,fmt);
			u8 g3 = AGIDL_GetG(clr3,fmt);
			u8 b3 = AGIDL_GetB(clr3,fmt);
			float rdiv = AGIDL_FindRBDivisor(fmt);
			float bdiv = AGIDL_FindRBDivisor(fmt);
			float gdiv = AGIDL_FindGDivisor(fmt);
			float interp1 = ((r1/rdiv)+(g1/gdiv)+(b1/bdiv))/3.0f;
			float interp2 = ((r3/rdiv)+(g3/gdiv)+(b3/bdiv))/3.0f;
			float interp = (interp1+interp2)/(2.0f);
			return AGIDL_InterpColor(clr1,clr3,interp,fmt);
		}break;
		case CC_SUB_AND_INTERP_CLR1:{
			COLOR clr3 = AGIDL_SubColor(clr1,clr2,fmt);
			u8 r3 = AGIDL_GetR(clr3,fmt);
			u8 g3 = AGIDL_GetG(clr3,fmt);
			u8 b3 = AGIDL_GetB(clr3,fmt);
			float rdiv = AGIDL_FindRBDivisor(fmt);
			float bdiv = AGIDL_FindRBDivisor(fmt);
			float gdiv = AGIDL_FindGDivisor(fmt);
			float interp1 = ((r1/rdiv)+(g1/gdiv)+(b1/bdiv))/3.0f;
			float interp2 = ((r3/rdiv)+(g3/gdiv)+(b3/bdiv))/3.0f;
			float interp = (interp1+interp2)/(2.0f);
			return AGIDL_InterpColor(clr3,clr1,interp,fmt);
		}break;
		case CC_MUL_AND_INTERP_CLR2:{
			u8 r3 = AGIDL_MulCombine(r1,r2,fmt);
			u8 g3 = AGIDL_MulCombine(g1,g2,fmt);
			u8 b3 = AGIDL_MulCombine(b1,b2,fmt);
			COLOR clr3 = AGIDL_RGB(r3,g3,b3,fmt);
			float rdiv = AGIDL_FindRBDivisor(fmt);
			float bdiv = AGIDL_FindRBDivisor(fmt);
			float gdiv = AGIDL_FindGDivisor(fmt);
			float interp1 = ((r2/rdiv)+(g2/gdiv)+(b2/bdiv))/3.0f;
			float interp2 = ((r3/rdiv)+(g3/gdiv)+(b3/bdiv))/3.0f;
			float interp = (interp1+interp2)/2.0f;
			return AGIDL_InterpColor(clr3,clr2,interp,fmt);
		}break;
		case CC_ADD_AND_INTERP_CLR2:{
			COLOR clr3 = AGIDL_AddColor(clr1,clr2,fmt);
			u8 r3 = AGIDL_GetR(clr3,fmt);
			u8 g3 = AGIDL_GetG(clr3,fmt);
			u8 b3 = AGIDL_GetB(clr3,fmt);
			float rdiv = AGIDL_FindRBDivisor(fmt);
			float bdiv = AGIDL_FindRBDivisor(fmt);
			float gdiv = AGIDL_FindGDivisor(fmt);
			float interp1 = ((r2/rdiv)+(g2/gdiv)+(b2/bdiv))/3.0f;
			float interp2 = ((r3/rdiv)+(g3/gdiv)+(b3/bdiv))/3.0f;
			float interp = (interp1+interp2)/2.0f;
			return AGIDL_InterpColor(clr2,clr3,interp,fmt);
		}break;
		case CC_SUB_AND_INTERP_CLR2:{
			COLOR clr3 = AGIDL_SubColor(clr1,clr2,fmt);
			u8 r3 = AGIDL_GetR(clr3,fmt);
			u8 g3 = AGIDL_GetG(clr3,fmt);
			u8 b3 = AGIDL_GetB(clr3,fmt);
			float rdiv = AGIDL_FindRBDivisor(fmt);
			float bdiv = AGIDL_FindRBDivisor(fmt);
			float gdiv = AGIDL_FindGDivisor(fmt);
			float interp1 = ((r2/rdiv)+(g2/gdiv)+(b2/bdiv))/3.0f;
			float interp2 = ((r3/rdiv)+(g3/gdiv)+(b3/bdiv))/3.0f;
			float interp = (interp1+interp2)/2.0f;
			return AGIDL_InterpColor(clr3,clr2,interp,fmt);
		}break;
	}
	return 0;
}
