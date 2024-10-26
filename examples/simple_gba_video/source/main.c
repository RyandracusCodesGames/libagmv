#include <gba.h>
#include "agmv_gba.h"
#include "open06_agmv.h"

#define VRAM_F  0x6000000 
#define VRAM_B	0x600A000

int IWRAM main(){
	
	SetVideoMode(AGMV_MODE_3);
	EnableTimer2();
	
	u32 frame_rate = 2400;
	
	int lastFr=0,FPS=0;  
	
	REG_BG2PA = 1 << 7 | 76;
	REG_BG2PD = 1 << 7 | 76;
	
	//REG_BG2PA = 128;
	//REG_BG2PD = 128;
	 
	u16* vram = (u16*)VRAM_F;
	
	AGMV* agmv = AGMV_Open(open06_agmv,open06_agmv_size);
	AGMV_InitSound(agmv);
	
	//AGMV_DisableAudio(agmv);
	
	irqInit();
	irqEnable(IRQ_VBLANK);

	while(1){

		//VBlankIntrWait();
		
		if((REG_TM2CNT/frame_rate)!=lastFr){
		
			if(button_pressed(BUTTON_RIGHT)){
				AGMV_SkipForwards(agmv->file,agmv,10);
			}
			
			if(button_pressed(BUTTON_LEFT)){
				AGMV_SkipBackwards(agmv->file,agmv,4);
			}
			
			if(button_pressed(BUTTON_A)){
				AGMV_ResetVideo(agmv->file,agmv);
			}
			 
			if(button_pressed(BUTTON_B)){
				if(agmv->audio_state == AGMV_AUDIO_PLAY){
					agmv->audio_state = AGMV_AUDIO_PAUSE;
					AGMV_DisableMasterSound();
				}
				else{
					agmv->audio_state = AGMV_AUDIO_PLAY;
					AGMV_EnableMasterSound();
				}
			}
			
			if(button_pressed(BUTTON_UP)){
				AGMV_SetVolume(agmv,1);
			}
			
			if(button_pressed(BUTTON_DOWN)){
				AGMV_SetVolume(agmv,0);
			}
			
			if(button_pressed(BUTTON_START)){
				if(agmv->video_state == AGMV_VIDEO_PLAY){
					agmv->video_state = AGMV_VIDEO_PAUSE;
					AGMV_DisableMasterSound();
				}
				else{
					agmv->video_state = AGMV_VIDEO_PLAY;
					AGMV_EnableMasterSound();
				}
				
				while(button_pressed(BUTTON_START));
			}
			
			AGMV_StreamMovie(agmv);	
			AGMV_DisplayFrame(vram,240,80,agmv);
			
		   if(AGMV_IsVideoDone(agmv)){ 
			   AGMV_ResetVideo(agmv->file,agmv);
		   }

		   FPS+=1; if(lastFr>(REG_TM2CNT/frame_rate)){ FPS=0;}
		   lastFr=(REG_TM2CNT/frame_rate); 
		} 
		  	
	}	
	
	AGMV_Close(agmv);
}