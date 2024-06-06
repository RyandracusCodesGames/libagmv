#ifndef BASICS_H
#define BASICS_H

#include "agmv_gba.h"

#define VRAM_F         0x6000000 
#define VRAM_B	0x600A000
#define BACK_VRAM_M5	0x600A000
#define DISPCNT        *(u32*)0x4000000
#define BACKB   	   0x10 
#define	VBLANK_CNT	*(volatile unsigned short*)0x4000006
#define REG_BG2PA *(volatile unsigned short *)0x4000020
#define REG_BG2PD *(volatile unsigned short *)0x4000026
#define REG_TM2D       *(volatile u16*)0x4000108
#define REG_TM3D	   *(volatile u16*)0x400010c

void SwapBuffer(u16* VideoBuffer)	//this swaps the locations of the current display and current writing location
{
	if (DISPCNT&BACKB)		//backbuffer is being displayed so swap
	{
		DISPCNT &= ~BACKB;	//make display show the frontbuffer (remove backbuffer using it's bitwise compliement)
		VideoBuffer=(unsigned short*)BACK_VRAM_M5;	//make backbuffer the one we draw to
	}
	else		//frontbuffer is being displayed so swap
	{
		DISPCNT|=BACKB;	//make display show the backbuffer (add backbuffer)
		VideoBuffer=(unsigned short*)VRAM_F;	//make frontbuffer the one we draw to
	}
}

void WaitVBlank(void)	//waits for vblank to complete
{
	while (VBLANK_CNT<160){}	//do nothing untill vblank has completed
}

#endif