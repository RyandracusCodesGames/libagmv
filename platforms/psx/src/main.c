#include <stdio.h>
#include <agmv_psx_lib.h>

#define RGB15(r,g,b) ((b&0x1f) << 10 | (g&0x1f) << 5 | (r&0x1f))

#define SCREEN_W 256
#define SCREEN_H 240

#define OTLEN 8
#define MALLOC_MAX 3

u_long ot[2][OTLEN];
char primbuff[2][32768];
char *nextpri = primbuff[0]; 

DISPENV disp[2];
DRAWENV draw[2];

int db, pad, x, y;

RECT img_area, screen;

char spu_malloc_rec[SPU_MALLOC_RECSIZ * (MALLOC_MAX+1)]; 

void Display(){
	DrawSync(0);
	VSync(0);
	PutDispEnv(&disp[db]);
	PutDrawEnv(&draw[db]);
	DrawOTag(&ot[db][OTLEN - 1]);
	db = !db;
	nextpri = primbuff[db];
}

void InitializeGraphicsEnvironment(){
	/*INIT/RESET GPU AND ENABLE GPU-CENTRIC HARDWARE INTERRUPTS*/
	ResetGraph(0);
	InitGeom();
    SetGeomOffset(0,0);
    SetGeomScreen(128);
	/*INITIALIZE THE DISPLAY ENVIRONMENT DATA MEMBERS*/
	SetDefDispEnv(&disp[0],0,0,SCREEN_W,SCREEN_H);
	SetDefDispEnv(&disp[1],0,SCREEN_H,SCREEN_W,SCREEN_H);
	/*INITIALIZE THE DRAWING ENVIRONMENT DATA MEMBERS*/
	SetDefDrawEnv(&draw[0],0,SCREEN_H,SCREEN_W,SCREEN_H);
	SetDefDrawEnv(&draw[1],0,0,SCREEN_W,SCREEN_H);
	
	SetDispMask(1);
	
	db = 0;
	
	FntLoad(960, 256);
	FntOpen(5, 30, SCREEN_W, SCREEN_H, 0, 512);
}

void InitializeScreen(){
	screen.x = screen.y = 0;
	screen.w = SCREEN_W;
	screen.h = SCREEN_H;
	
	x = 0;
	y = 0;
	pad = 0;
	
	PadInit(0);
}

int main(){

	AGMV* agmv;

	InitializeGraphicsEnvironment();
	AGMV_InitializeMemory();
	InitializeScreen();
	CdInit();
	SpuInit();
	InitGeom();
	
	SpuInitMalloc(MALLOC_MAX, spu_malloc_rec);                                   
	
	agmv = AGMV_Open("\\INTRO.DAT;1");
	AGMV_SetNextPrim(agmv,nextpri);
	AGMV_InitScreen(agmv,0,0,SCREEN_W,SCREEN_H);
	AGMV_SetBufferFlip(agmv,AGMV_FALSE);
	
	while(1){
		ClearOTagR(ot[db], OTLEN);
		
		pad = PadRead(1);
			
		if(pad & PADLright){
			AGMV_SkipForwards(agmv->file,agmv,15);
		}
		
		if(pad & PADLleft){
			AGMV_SkipBackwards(agmv->file,agmv,3);
		}
		
		if(pad & PADstart){
			if(agmv->video_state == AGMV_VIDEO_PLAY){
				SpuSetReverbModeDepth(0,0);
				AGMV_SetVideoState(agmv,AGMV_VIDEO_PAUSE);
			}
			else{
				SpuSetReverbModeDepth(0x3FFF,0x3FFF);
				AGMV_SetVideoState(agmv,AGMV_VIDEO_PLAY);
			}
			
			AGMV_WaitUntilButtonReleased(PADstart);
		}
		
		if(pad & PADRdown){
			AGMV_ResetVideo(agmv->file,agmv);
		}
		
		AGMV_StreamMovie(agmv);
		FntPrint("File  Pos: %d\n",AGMV_Ftell(agmv->file));
		FntPrint("Video State: %d\n",agmv->video_state);
		FntPrint("File Size: %d\n",agmv->file->file->size);
		FntPrint("Frame: %d\n",agmv->frame_count);
		FntPrint("Frame Size: %d\n",agmv->frame_chunk->usize);
		FntFlush(-1);
		addPrim(ot[db], agmv->poly);                         
		nextpri += sizeof(POLY_FT4);   
		Display();

		if(AGMV_IsVideoDone(agmv) || AGMV_EOF(agmv->file)){
			AGMV_ResetVideo(agmv->file,agmv);
		}
	}
	
	AGMV_Close(agmv);
	CdControl(CdlStop, 0, 0);
	CdFlush();
	ResetGraph(1);
	PadStop();
	
	return 0;
}