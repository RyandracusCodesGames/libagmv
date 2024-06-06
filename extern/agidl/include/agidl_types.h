#ifndef AGIDL_TYPES_H
#define AGIDL_TYPES_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_types.h
*   Date: 9/8/2023
*   Version: 0.1b
*   Updated: 1/19/2024
*   Author: Ryandracus Chapman
*
********************************************/

typedef int AGIDL_Bool;

#define TRUE 1
#define FALSE 0

/*Unsigned Integer Types*/
typedef unsigned char u8, uchar;
typedef unsigned short u16, ushort;
typedef unsigned long u32, ulong;
typedef unsigned int uint;
typedef unsigned long long u64;

/*Signed Integer Types*/
typedef signed char s8, schar;
typedef signed short s16;
typedef signed long s32;
typedef signed long long s64;

/*Float/Fixed Point Types*/
typedef float f32;
typedef double f64;

#define FIX_SHIFT 8
#define FIX_SCALE (1<<FIX_SHIFT)
#define FIX_MASK (FIX_SCALE-1)
#define FIX_SCALEF ((float)FIX_SCALE)
#define FIX_SCALEF_INV (1.0f/FIX_SCALEF)


#endif
