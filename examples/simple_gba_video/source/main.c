#include "basics.h"
#include "GBA_GEN_AGMV.h"    
#include <stdlib.h>
#include <gba.h>

int IWRAM main(){
	
	SetVideoMode(MODE_3);
	EnableTimer2();  	
	
	int lastFr=0,FPS=0;  
	
	REG_BG2PA = 128;
	REG_BG2PD = 128;
	 
	u16* vram = (u16*)VRAM_F;
	
	File* file = (File*)malloc(sizeof(File));
	
	Open(file,GBA_AGMV_FILE,260390);
	
	AGMV* agmv = AGMV_AllocResources(file);
	
	*interrupt_enable = 0;
    *interrupt_selection |= INTERRUPT_VBLANK;
    *display_interrupts |= 0x08;
    *interrupt_enable = 1;

    /* clear the sound control initially */
    *sound_control = 0;
	
	irqInit();
	irqEnable(IRQ_VBLANK);

	while(1){
		
		if((REG_TM2D/6100)!=lastFr){

			if(button_pressed(BUTTON_RIGHT)){
				AGMV_SkipForwards(file,agmv,10);
			}
			
			if(button_pressed(BUTTON_LEFT)){
				AGMV_SkipBackwards(file,agmv,10);
			}
			
			if(button_pressed(BUTTON_A)){
				AGMV_ResetVideo(file,agmv);
			}
			
			if(button_pressed(BUTTON_B)){
				if(agmv->audio_chunk->enable_audio == TRUE){
					agmv->audio_chunk->enable_audio = FALSE;
				}
				else{
					agmv->audio_chunk->enable_audio = TRUE;
				}
			}

			AGMV_PlayAGMV(file,agmv); 	
			AGMV_DisplayFrame(vram,240,80,agmv);
		   
		   if(AGMV_IsVideoDone(agmv)){ 
			   AGMV_ResetVideo(file,agmv);
		   }
		   
		   FPS+=1; if(lastFr>(REG_TM2D/6100)){ FPS=0;}
		   lastFr=(REG_TM2D/6100);  

			VBlankIntrWait();		   
		}		
	}	
	
	free(file);
	DestroyAGMV(agmv);
}
