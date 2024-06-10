/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_cc_mixer.c
*   Date: 9/10/2023
*   Version: 0.4b
*   Updated: 6/10/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <stdio.h>
#include <agidl_cc_mixer.h>
#include <agidl_img_types.h>
#include <agidl_math_utils.h>

float AGIDL_FindRBDivisor(AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_RGB_888:{
			return (255.0f);
		}break;
		case AGIDL_BGR_888:{
			return (255.0f);
		}break;
		case AGIDL_RGB_555:{
			return (31.0f);
		}break;
		case AGIDL_BGR_555:{
			return (31.0f);
		}break;
		case AGIDL_RGB_565:{
			return (31.0f);
		}break;
		case AGIDL_BGR_565:{
			return (31.0f);
		}break;
		case AGIDL_RGBA_8888:{
			return (255.0f);
		}break;
		case AGIDL_ARGB_8888:{
			return (255.0f);
		}break;
	}
	return 0;
}

float AGIDL_FindGDivisor(AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_RGB_888:{
			return (255.0f);
		}break;
		case AGIDL_BGR_888:{
			return (255.0f);
		}break;
		case AGIDL_RGB_555:{
			return (31.0f);
		}break;
		case AGIDL_BGR_555:{
			return (31.0f);
		}break;
		case AGIDL_RGB_565:{
			return (63.0f);
		}break;
		case AGIDL_BGR_565:{
			return (63.0f);
		}break;
		case AGIDL_RGBA_8888:{
			return (255.0f);
		}break;
		case AGIDL_ARGB_8888:{
			return (255.0f);
		}break;
	}
	return 0;
}

u8 AGIDL_MulCombine565(u8 cc1, u8 cc2){
	return (cc1*cc2) >> 6;
}

u8 AGIDL_MulCombine(u8 cc1, u8 cc2, AGIDL_CLR_FMT fmt){
	switch(fmt){
		case AGIDL_RGB_888:{
			return (cc1*cc2) >> 8;
		}break;
		case AGIDL_BGR_888:{
			return (cc1*cc2) >> 8;
		}break;
		case AGIDL_RGB_555:{
			return (cc1*cc2) >> 5;
		}break;
		case AGIDL_BGR_555:{
			return (cc1*cc2) >> 5;
		}break;
		case AGIDL_RGB_565:{
			return (cc1*cc2) >> 5;
		}break;
		case AGIDL_BGR_565:{
			return (cc1*cc2) >> 5;
		}break;
		case AGIDL_RGBA_8888:{
			return (cc1*cc2) >> 8;
		}break;
		case AGIDL_ARGB_8888:{
			return (cc1*cc2) >> 8;
		}break;
	}
	return 0;
}

COLOR AGIDL_AddColor(COLOR clr1, COLOR clr2, AGIDL_CLR_FMT fmt){
	u8 r1 = AGIDL_GetR(clr1,fmt);
	u8 g1 = AGIDL_GetG(clr1,fmt);
	u8 b1 = AGIDL_GetB(clr1,fmt);
	
	u8 r2 = AGIDL_GetR(clr2,fmt);
	u8 g2 = AGIDL_GetG(clr2,fmt);
	u8 b2 = AGIDL_GetB(clr2,fmt);
	
	u8 r = r1 + r2;
	u8 g = g1 + g2;
	u8 b = b1 + b2;
	
	if(AGIDL_GetBitCount(fmt) == 32){
		u8 a1 = AGIDL_GetA(clr1,fmt), a2 = AGIDL_GetA(clr2,fmt);
		u8 a = (a1+a2) >> 1;
		return AGIDL_RGBA(r,g,b,a,fmt);
	}
	
	return AGIDL_RGB(r,g,b,fmt);
}

COLOR AGIDL_SubColor(COLOR clr1, COLOR clr2, AGIDL_CLR_FMT fmt){
	u8 r1 = AGIDL_GetR(clr1,fmt);
	u8 g1 = AGIDL_GetG(clr1,fmt);
	u8 b1 = AGIDL_GetB(clr1,fmt);
	
	u8 r2 = AGIDL_GetR(clr2,fmt);
	u8 g2 = AGIDL_GetG(clr2,fmt);
	u8 b2 = AGIDL_GetB(clr2,fmt);
	
	u8 r = r1 - r2;
	u8 g = g1 - g2;
	u8 b = b1 - b2;
	
	if(AGIDL_GetBitCount(fmt) == 32){
		u8 a1 = AGIDL_GetA(clr1,fmt), a2 = AGIDL_GetA(clr2,fmt);
		u8 a = (a1+a2) >> 1;
		return AGIDL_RGBA(r,g,b,a,fmt);
	}
	
	return AGIDL_RGB(r,g,b,fmt);
}

COLOR AGIDL_MulColor(COLOR clr1, COLOR clr2, AGIDL_CLR_FMT fmt){
	u8 r1 = AGIDL_GetR(clr1,fmt);
	u8 g1 = AGIDL_GetG(clr1,fmt);
	u8 b1 = AGIDL_GetB(clr1,fmt);
	
	u8 r2 = AGIDL_GetR(clr2,fmt);
	u8 g2 = AGIDL_GetG(clr2,fmt);
	u8 b2 = AGIDL_GetB(clr2,fmt);
	
	u8 r = r1 * r2;
	u8 g = g1 * g2;
	u8 b = b1 * b2;
	
	if(AGIDL_GetBitCount(fmt) == 32){
		u8 a1 = AGIDL_GetA(clr1,fmt), a2 = AGIDL_GetA(clr2,fmt);
		u8 a = (a1*a2) >> 8;
		return AGIDL_RGBA(r,g,b,a,fmt);
	}
	
	return AGIDL_RGB(r,g,b,fmt);
}

COLOR AGIDL_DotColor(COLOR clr1, float factor, AGIDL_CLR_FMT fmt){
	
	factor = AGIDL_Clampf(0.0f,factor,1.0f);

	u8 r1 = AGIDL_GetR(clr1,fmt);
	u8 g1 = AGIDL_GetG(clr1,fmt);
	u8 b1 = AGIDL_GetB(clr1,fmt);
	
	u8 r = r1 * factor;
	u8 g = g1 * factor;
	u8 b = b1 * factor;
	
	if(AGIDL_GetBitCount(fmt) == 32){
		u8 a1 = AGIDL_GetA(clr1,fmt);
		return AGIDL_RGBA(r,g,b,a1,fmt);
	}
	
	return AGIDL_RGB(r,g,b,fmt);
}


COLOR AGIDL_DivColor(COLOR clr1, float factor, AGIDL_CLR_FMT fmt){
	
	factor = AGIDL_Clampf(0.0f,factor,1.0f);

	u8 r1 = AGIDL_GetR(clr1,fmt);
	u8 g1 = AGIDL_GetG(clr1,fmt);
	u8 b1 = AGIDL_GetB(clr1,fmt);
	
	u8 r = r1 / factor;
	u8 g = g1 / factor;
	u8 b = b1 / factor;
	
	if(AGIDL_GetBitCount(fmt) == 32){
		u8 a1 = AGIDL_GetA(clr1,fmt);
		return AGIDL_RGBA(r,g,b,a1,fmt);
	}
	
	return AGIDL_RGB(r,g,b,fmt);
}

COLOR AGIDL_InterpColor(COLOR clr1, COLOR clr2, float interp, AGIDL_CLR_FMT fmt){
	u8 r1 = AGIDL_GetR(clr1,fmt);
	u8 g1 = AGIDL_GetG(clr1,fmt);
	u8 b1 = AGIDL_GetB(clr1,fmt);
	
	u8 r2 = AGIDL_GetR(clr2,fmt);
	u8 g2 = AGIDL_GetG(clr2,fmt);
	u8 b2 = AGIDL_GetB(clr2,fmt);
	
	u8 r = r1 + interp * (r2 - r1);
	u8 g = g1 + interp * (g2 - g1);
	u8 b = b1 + interp * (b2 - b1);
	
	return AGIDL_RGB(r,g,b,fmt);
}

COLOR AGIDL_BilerpColor(COLOR clr1, COLOR clr2, COLOR clr3, COLOR clr4, float interp, float bilerp, AGIDL_CLR_FMT fmt){
	COLOR top = AGIDL_InterpColor(clr1,clr2,interp,fmt);
	COLOR bottom = AGIDL_InterpColor(clr3,clr4,interp,fmt);
	
	return AGIDL_InterpColor(top,bottom,bilerp,fmt);
}

COLOR AGIDL_GrayscaleColor(COLOR clr, AGIDL_CLR_FMT fmt){
	u8 r = AGIDL_GetR(clr,fmt);
	u8 g = AGIDL_GetG(clr,fmt);
	u8 b = AGIDL_GetB(clr,fmt);
	
	u8 rgb = (u8)((r+g+b)/3.0f);
	
	if(AGIDL_GetBitCount(fmt) == 32){
		u8 a = AGIDL_GetA(clr,fmt);
		return AGIDL_RGBA(rgb,rgb,rgb,a,fmt);
	}
	
	return AGIDL_RGB(rgb,rgb,rgb,fmt);
}

COLOR AGIDL_BlendColor(COLOR clr1, COLOR clr2, float blend_factor, int blend_mode, AGIDL_CLR_FMT fmt){
	if(blend_factor > 1.0f){
		blend_factor = 1.0f;
	}
	
	if(blend_factor < 0.0f){
		blend_factor = 0.0f;
	}
	
	float iblend_factor = 1.0f - blend_factor;
	
	u8 r1 = AGIDL_GetR(clr1,fmt);
	u8 g1 = AGIDL_GetG(clr1,fmt);
	u8 b1 = AGIDL_GetB(clr1,fmt);
	
	u8 r2 = AGIDL_GetR(clr2,fmt);
	u8 g2 = AGIDL_GetG(clr2,fmt);
	u8 b2 = AGIDL_GetB(clr2,fmt);
	
	if(fmt == AGIDL_RGBA_8888 || fmt == AGIDL_ARGB_8888){
		u8 a1 = AGIDL_GetA(clr1,fmt), a2 = AGIDL_GetA(clr2,fmt);
		
		switch(blend_mode){
			case CC_BLEND_CLR_SRCINV:{
				float blend_factor = (a1/255.0f);
				float iblend_factor = 1.0f - blend_factor;
				u8 r = r1*iblend_factor + r2*blend_factor;
				u8 g = g1*iblend_factor + g2*blend_factor;
				u8 b = b1*iblend_factor + b2*blend_factor;
				u8 a = a1*iblend_factor + a2*blend_factor;
				return AGIDL_RGBA(r,g,b,a,fmt);
			}break;
			case CC_BLEND_CLR_DESTINV:{
				float blend_factor = a2/255.0f;
				float iblend_factor = 1.0f - blend_factor;
				u8 r = r1*iblend_factor + r2*blend_factor;
				u8 g = g1*iblend_factor + g2*blend_factor;
				u8 b = b1*iblend_factor + b2*blend_factor;
				u8 a = a1*iblend_factor + a2*blend_factor;
				return AGIDL_RGBA(r,g,b,a,fmt);
			}break;
			case CC_BLEND_CLR_SRC:{
				float blend_factor = a1/255.0f;
				u8 r = r1*blend_factor + r2;
				u8 g = g1*blend_factor + g2;
				u8 b = b1*blend_factor + b2;
				u8 a = a1*blend_factor + a2;
				return AGIDL_RGBA(r,g,b,a,fmt);
			}break;
			
			case CC_BLEND_CLR_DEST:{
				float blend_factor = a2/255.0f;
				u8 r = r1*blend_factor + r2;
				u8 g = g1*blend_factor + g2;
				u8 b = b1*blend_factor + b2;
				u8 a = a1*blend_factor + a2;
				return AGIDL_RGBA(r,g,b,a,fmt);
			}break;
			case CC_BLEND_CLR_ISRC:{
				float blend_factor = 1.0f - (a1/255.0f);
				u8 r = r1*blend_factor + r2;
				u8 g = g1*blend_factor + g2;
				u8 b = b1*blend_factor + b2;
				u8 a = a1*blend_factor + a2;
				return AGIDL_RGBA(r,g,b,a,fmt);
			}break;
			
			case CC_BLEND_CLR_IDEST:{
				float blend_factor = 1.0f - (a2/255.0f);
				u8 r = r1*blend_factor + r2;
				u8 g = g1*blend_factor + g2;
				u8 b = b1*blend_factor + b2;
				u8 a = a1*blend_factor + a2;
				return AGIDL_RGBA(r,g,b,a,fmt);
			}break;
		}
	}
		
		switch(blend_mode){
			case CC_BLEND_CLR_SRCINV:{
				u8 r = r1*iblend_factor + r2*blend_factor;
				u8 g = g1*iblend_factor + g2*blend_factor;
				u8 b = b1*iblend_factor + b2*blend_factor;
				return AGIDL_RGB(r,g,b,fmt);
			}break;
			case CC_BLEND_CLR_DESTINV:{
				u8 r = r1*blend_factor + r2*iblend_factor;
				u8 g = g1*blend_factor + g2*iblend_factor;
				u8 b = b1*blend_factor + b2*iblend_factor;
				return AGIDL_RGB(r,g,b,fmt);
			}break;
			case CC_BLEND_CLR_SRC:{
				u8 r = r1*blend_factor + r2;
				u8 g = g1*blend_factor + g2;
				u8 b = b1*blend_factor + b2;
				return AGIDL_RGB(r,g,b,fmt);
			}break;
			
			case CC_BLEND_CLR_DEST:{
				u8 r = r1 + r2*blend_factor;
				u8 g = g1 + g2*blend_factor;
				u8 b = b1 + b2*blend_factor;
				return AGIDL_RGB(r,g,b,fmt);
			}break;
			case CC_BLEND_CLR_ISRC:{
				u8 r = r1*iblend_factor + r2;
				u8 g = g1*iblend_factor + g2;
				u8 b = b1*iblend_factor + b2;
				return AGIDL_RGB(r,g,b,fmt);
			}break;
			
			case CC_BLEND_CLR_IDEST:{
				u8 r = r1 + r2*iblend_factor;
				u8 g = g1 + g2*iblend_factor;
				u8 b = b1 + b2*iblend_factor;
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
			float interp = ((r3/rdiv)+(g3/gdiv)+(b3/bdiv))/(3.0f);
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
			float interp = 1.0f - ((r1/rdiv)+(g1/gdiv)+(b1/bdiv))/(3.0f);
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
			float interp = ((r1/rdiv)+(g1/gdiv)+(b1/bdiv))/(3.0f);
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
			float interp = 1.0f - ((r2/rdiv)+(g2/gdiv)+(b2/bdiv))/(3.0f);
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
			float interp = ((r2/rdiv)+(g2/gdiv)+(b2/bdiv))/(3.0f);
			return AGIDL_InterpColor(clr3,clr4,interp,fmt);
		}break;
		case CC_INTERP_CLR:{
			float rdiv = AGIDL_FindRBDivisor(fmt);
			float bdiv = AGIDL_FindRBDivisor(fmt);
			float gdiv = AGIDL_FindGDivisor(fmt);
			float interp1 = ((r1/rdiv)+(g1/gdiv)+(b1/bdiv))/(3.0f);
			float interp2 = ((r2/rdiv)+(g2/gdiv)+(b2/bdiv))/(3.0f);
			float interp = (interp1+interp2)/(2.0f);
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
			float interp1 = ((r1/rdiv)+(g1/gdiv)+(b1/bdiv))/(3.0f);
			float interp2 = ((r3/rdiv)+(g3/gdiv)+(b3/bdiv))/(3.0f);
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
			float interp1 = ((r1/rdiv)+(g1/gdiv)+(b1/bdiv))/(3.0f);
			float interp2 = ((r3/rdiv)+(g3/gdiv)+(b3/bdiv))/(3.0f);
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
			float interp1 = ((r1/rdiv)+(g1/gdiv)+(b1/bdiv))/(3.0f);
			float interp2 = ((r3/rdiv)+(g3/gdiv)+(b3/bdiv))/(3.0f);
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
			float interp1 = ((r2/rdiv)+(g2/gdiv)+(b2/bdiv))/(3.0f);
			float interp2 = ((r3/rdiv)+(g3/gdiv)+(b3/bdiv))/(3.0f);
			float interp = (interp1+interp2)/(2.0f);
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
			float interp1 = ((r2/rdiv)+(g2/gdiv)+(b2/bdiv))/(3.0f);
			float interp2 = ((r3/rdiv)+(g3/gdiv)+(b3/bdiv))/(3.0f);
			float interp = (interp1+interp2)/(2.0f);
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
			float interp1 = ((r2/rdiv)+(g2/gdiv)+(b2/bdiv))/(3.0f);
			float interp2 = ((r3/rdiv)+(g3/gdiv)+(b3/bdiv))/(3.0f);
			float interp = (interp1+interp2)/(2.0f);
			return AGIDL_InterpColor(clr3,clr2,interp,fmt);
		}break;
	}
	return 0;
}

