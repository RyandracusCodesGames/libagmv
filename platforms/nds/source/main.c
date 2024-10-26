#include <nds.h>
#include <filesystem.h>
#include <fat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <agmv_nds_lib.h>

//---------------------------------------------------------------------------------
void dirlist(const char* path) {
//---------------------------------------------------------------------------------

	DIR* pdir = opendir(path);

	if (pdir != NULL) {

		while(true) {
			struct dirent* pent = readdir(pdir);
			if(pent == NULL) break;
			
			if(strcmp(".", pent->d_name) != 0 && strcmp("..", pent->d_name) != 0) {
				if(pent->d_type == DT_DIR) {
					printf("%s/%s <DIR>\n", (strcmp("/",path) == 0)?"":path, pent->d_name);
					char *dnbuf = (char *)malloc(strlen(pent->d_name)+strlen(path)+2);
					sprintf(dnbuf, "%s/%s", (strcmp("/",path) == 0)?"":path, pent->d_name);
					dirlist(dnbuf);
					free(dnbuf);
				} else {
					printf("%s/%s\n",(strcmp("/",path) == 0)?"":path, pent->d_name);
				}
			}
		}
		
		closedir(pdir);
	} else {
		printf("opendir() failure.\n");
	}
}

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------

	consoleDemoInit();

	videoSetMode(MODE_5_2D); 

	vramSetPrimaryBanks(	VRAM_A_MAIN_BG_0x06000000, VRAM_B_MAIN_BG_0x06020000, 
		VRAM_C_SUB_BG , VRAM_D_LCD); 

	int bg = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);

	u16* backBuffer = (u16*)bgGetGfxPtr(bg) + 256*256, scaleX = 215, scaleY = 190;
	
	bgSetScale(bg, scaleX, scaleY);

	nitroFSInit(NULL);

	dirlist("/");
	
	u32 frame_rate = 0;
	
	AGMV* agmv = AGMV_Open("intro.agmv");
	soundEnable();
	
	if(agmv->error != NO_ERR){
		AGMV_Close(agmv);
	}

	while(1) {
		swiWaitForVBlank();
		scanKeys();
		
		frame_rate++;
		
		if(keysDown()&KEY_RIGHT){
			AGMV_SkipForwards(agmv->file,agmv,25);
		}
		
		if(keysDown()&KEY_LEFT){
			AGMV_SkipBackwards(agmv->file,agmv,25);
		}
		
		if(keysDown()&KEY_B){
			AGMV_ResetVideo(agmv->file,agmv);
		}
		
		if(keysDown()&KEY_A){
			if(agmv->audio_state == AGMV_AUDIO_PLAY){
				agmv->audio_state = AGMV_AUDIO_PAUSE;
			}
			else{
				agmv->audio_state = AGMV_AUDIO_PLAY;
			}
		}
		
		if(keysDown()&KEY_UP){
			agmv->volume += 5;
			AGMV_SetVolume(agmv,agmv->volume);
		}
		
		if(keysDown()&KEY_DOWN){
			agmv->volume -= 5;
			AGMV_SetVolume(agmv,agmv->volume);
		}
		
		
		bgUpdate();

		if(frame_rate == 3){
			
			frame_rate = 0;
		
			AGMV_StreamMovie(agmv);
		
			backBuffer = (u16*)bgGetGfxPtr(bg);
			
			if(bgGetMapBase(bg) == 8)
			{
				bgSetMapBase(bg, 0);
			}
			else
			{
				bgSetMapBase(bg, 8);
			}
			
			AGMV_DisplayFrame(backBuffer,256,192,agmv);

			if(AGMV_IsVideoDone(agmv)){ 
				AGMV_ResetVideo(agmv->file,agmv);
			}
		}

	}
	
	soundDisable();
	AGMV_Close(agmv);

	return 0;
}
