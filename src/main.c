#include <stdio.h>
#include <agidl.h>
#include <time.h>
#include <agmv.h>

int main(){
	float startTimeInterval = (float)clock() / CLOCKS_PER_SEC;

	AGMV_DecodeAGMV("agmv_splash.agmv",AGMV_IMG_BMP,AGMV_AUDIO_WAV);
	
	float endTimeInterval = (float)clock() / CLOCKS_PER_SEC;
	
	printf("deltaTime = %.4f\n",endTimeInterval-startTimeInterval);

	return 0;
}