/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: simple_error_handler.c
*   Date: 7/31/2024
*   Version: 2.0
*   Updated: 7/31/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <stdio.h>
#include <agmv.h>

int main(){
	
	int error = AGMV_DecodeAudio("gba_commercial.agmv",AGMV_AUDIO_WAV);
	
	if(error != NO_ERR){
		printf("%s\n",AGMV_Error2Str(error));
	}

	return 0;
}