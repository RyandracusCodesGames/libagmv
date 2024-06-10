#ifndef AGIDL_CC_TYPES
#define AGIDL_CC_TYPES

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_cc_types.h
*   Date: 9/8/2023
*   Version: 0.4b
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_types.h>

/********************************************
*		 		COLOR FORMATS		        *       
*********************************************
* Color formatting in computer graphics
* refers to the way in which human perceivable
* colors can be represented in the memory layout
* of a computer. AGIDL supports the RGB color space,
* which is a triplet value that states that any color
* can be represented as having some amount of red, green,
* and blue, and it supports four different formatting of
* the RGB color space. It supports 15-bit high color RGB 
* where each color components is allocated 5-bits and the
* equivalent BGR 15-bit as well. It supports the 24-bit and
* 32-bit variants as well. The next two color spaces are
* similarly related. The AGIDL supports the YCbCr and YIQ
* formatting which are the color spaces used in JPEG images
* and the NTSC video encoding standard in the image filter
* section of the library.
*****************************************************/

typedef enum AGIDL_CLR_FMT{
	AGIDL_RGB_888 = 0x1, /*24-Bit RGB*/
	AGIDL_BGR_888 = 0x2, /*24-Bit BGR*/
	AGIDL_RGBA_8888 = 0x3, /*32-Bit RGBA*/
	AGIDL_ARGB_8888 = 0x4, /*32-Bit ARGB*/
	AGIDL_RGB_555 = 0x5, /*15-Bit RGB*/
	AGIDL_BGR_555 = 0x6, /*15-Bit BGR*/
	AGIDL_RGB_565 = 0x7, /*16-Bit RGB*/
	AGIDL_BGR_565 = 0x8, /*16-Bit BGR*/
}AGIDL_CLR_FMT;

typedef enum AGIDL_CLR_MDL{
	AGIDL_CLR_RGB = 0x1,
	AGIDL_CLR_BGR = 0x2,
	AGIDL_CLR_RGBA = 0x3,
	AGIDL_CLR_ARGB = 0x4,
}AGIDL_CLR_MDL;

typedef enum AGIDL_BITS{
	AGIDL_BITS_888 = 24,
	AGIDL_BITS_555 = 15,
	AGIDL_BITS_565 = 16,
	AGIDL_BITS_8888 = 32,
	AGIDL_BITS_ICP = 8,
}AGIDL_BITS;

typedef enum AGIDL_CLR_COMPONENT{
	CC_R = 0x1,
	CC_G = 0x2,
	CC_B = 0x3,
}AGIDL_CLR_COMPONENT;

/*Enumeration that holds the most fundamental, named colors that when 
  called through the AGIDL_GetColor() function, it will provide a way to call 
  upon colors regardless of formatting or bit-depth*/

typedef enum AGIDL_CLR{
	BLACK = 0x1,
	WHITE = 0x2,
	GRAY = 0x3,
	RED = 0x4,
	GREEN = 0x5,
	BLUE = 0x6,
	YELLOW = 0x7,
	ORANGE = 0x8,
	PURPLE = 0x9,
	BROWN = 0x10,
	PINK = 0x11,
}AGIDL_CLR;

/*Fundamental 24-Bit RGB Color Macros*/
#define BLACK_RGB_888 0x000000
#define WHITE_RGB_888 0xFFFFFF
#define GRAY_RGB_888 0x808080
#define RED_RGB_888 0xFF0000
#define GREEN_RGB_888 0x00FF00
#define BLUE_RGB_888 0x0000FF
#define YELLOW_RGB_888 0xFFFF00
#define ORANGE_RGB_888 0xFFA500
#define PURPLE_RGB_888 0x4B0082
#define BROWN_RGB_888 0x964B00
#define PINK_RGB_888 0xFFC0CB
/*Fundamental 24-Bit BGR Color Macros*/
#define BLACK_BGR_888 0x000000
#define WHITE_BGR_888 0xFFFFFF
#define GRAY_BGR_888 0x808080
#define RED_BGR_888 0x0000FF
#define GREEN_BGR_888 0x00FF00
#define BLUE_BGR_888 0xFF0000
#define YELLOW_BGR_888 0x00FFFF
#define ORANGE_BGR_888 0x00A5FF
#define PURPLE_BGR_888 0x82004B
#define BROWN_BGR_888 0x004B96
#define PINK_BGR_888 0xCBC0FF
/*Fundamental 32-Bit RGBA Color Macros*/
#define BLACK_RGBA_8888 0x000000FF
#define WHITE_RGBA_8888 0xFFFFFFFF
#define GRAY_RGBA_8888 0x808080FF
#define RED_RGBA_8888 0xFF0000FF
#define GREEN_RGBA_8888 0x00FF00FF
#define BLUE_RGBA_8888 0x0000FFFF
#define YELLOW_RGBA_8888 0xFFFF00FF
#define ORANGE_RGBA_8888 0xFFA500FF
#define PURPLE_RGBA_8888 0x4B0082FF
#define BROWN_RGBA_8888 0x964B00FF
#define PINK_RGBA_8888 0xFFC0CBFF
/*Fundamental 32-Bit ARGB Color Macros*/
#define BLACK_ARGB_8888 0xFF000000
#define WHITE_ARGB_8888 0xFFFFFFFF
#define GRAY_ARGB_8888 0xFF808080
#define RED_ARGB_8888 0xFFFF0000
#define GREEN_ARGB_8888 0xFF00FF00
#define BLUE_ARGB_8888 0xFF0000FF
#define YELLOW_ARGB_8888 0xFFFFFF00
#define ORANGE_ARGB_8888 0xFFFFA500
#define PURPLE_ARGB_8888 0xFF4B0082
#define BROWN_ARGB_8888 0xFF964B00
#define PINK_ARGB_8888 0xFFFFC0CB
/*Fundamental 15-Bit BGR Color Macros*/
#define BLACK_BGR_555 0x0000
#define WHITE_BGR_555 0xFFFF
#define RED_BGR_555 0x001F
#define GREEN_BGR_555 0x03E0
#define BLUE_BGR_555 0x7C00
#define GRAY_BGR_555 0x294A
#define YELLOW_BGR_555 0x03FF
#define ORANGE_BGR_555 0x029F
#define PURPLE_BGR_555 0x4009
#define BROWN_BGR_555 0x0132
#define PINK_BGR_555 0x671F
/*Fundamental 15-Bit RGB Color Macros*/
#define BLACK_RGB_555 0x0000
#define WHITE_RGB_555 0xFFFF
#define RED_RGB_555 0x7C00
#define GREEN_RGB_555 0x03E0
#define BLUE_RGB_555 0x001F
#define GRAY_RGB_555 0x294A
#define YELLOW_RGB_555 0x7FE0
#define ORANGE_RGB_555 0x7E80
#define PURPLE_RGB_555 0x2410
#define BROWN_RGB_555 0x4920
#define PINK_RGB_555 0x7F19
/*Fundamental 16-Bit RGB Color Macros*/
#define BLACK_RGB_565 0x0000
#define WHITE_RGB_565 0xFFFF
#define RED_RGB_565 0xF800
#define GREEN_RGB_565 0x7E0
#define BLUE_RGB_565 0x001F
#define GRAY_RGB_565 0x514A
#define YELLOW_RGB_565 0xFFE0
#define ORANGE_RGB_565 0xFD00
#define PURPLE_RGB_565 0x4810
#define BROWN_RGB_565 0x9240
#define PINK_RGB_565 0xFE39
/*Fundamental 16-Bit BGR Color Macros*/
#define BLACK_BGR_565 0x0000
#define WHITE_BGR_565 0xFFFF
#define RED_BGR_565 0x001F
#define GREEN_BGR_565 0x7E0
#define BLUE_BGR_565 0xF800
#define GRAY_BGR_565 0x514A
#define YELLOW_BGR_565 0x7FF
#define ORANGE_BGR_565 0x53F
#define PURPLE_BGR_565 0x8009
#define BROWN_BGR_565 0x252
#define PINK_BGR_565 0xCE3F

typedef u32 COLOR;
typedef u16 COLOR16;

#define AGIDL_ICP_16       0x1
#define AGIDL_ICP_256      0x2
#define AGIDL_ICP_16b_16   0x3
#define AGIDL_ICP_16b_256  0x4

/*The ICP struct stands for Indexed Color Palette which can be used
  for images or general purpose use in a program. It holds two main 
  types of color palettes, those that are comprised of 32-bit colors
  and those of 16-bit colors. The underscore followed by a number 
  refers to how many numbers of colors the palette contains*/

typedef union ICP{
	COLOR palette_16[16];			/*16 element length 32-bit colors*/
	COLOR palette_256[256];			/*256 element length 32-bit colors*/
	COLOR16 palette_16b_16[16];		/*16 element length 16-bit colors*/
	COLOR16 palette_16b_256[256];	/*256 element length 16-bit colors*/
}ICP;

typedef struct AGIDL_ICP{
	int mode; /*The bit-depth of the color and num. of colors to use in the ICP*/
	AGIDL_CLR_FMT fmt;
	ICP icp;
}AGIDL_ICP;

typedef struct AGIDL_RGBf{
	f32 r,g,b;
}AGIDL_RGBf;

typedef struct AGIDL_RGBA{
	u8 r,g,b,a;
}AGDIDL_RGBA;

#endif