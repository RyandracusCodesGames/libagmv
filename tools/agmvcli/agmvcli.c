#include <stdio.h>
#include <string.h>
#include <agmv.h>

/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmvcli.c
*   Date: 4/13/2024
*   Version: 1.0
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/

Bool IsAGIDLImage(char* ext);
AGMV_IMG_TYPE GetImageType(char* ext);
Bool IsAGMVOpt(char* opt);
AGMV_OPT GetAGMVOpt(char* opt);
AGMV_AUDIO_TYPE GetAudioType(char* ext);
AGMV_AUDIO_TYPE GetAudioTypeToken(char* token);
Bool IsAGMVQOpt(char* opt);
AGMV_QUALITY GetQuality(char* opt);
Bool IsAGMVCompression(char* compression);
AGMV_COMPRESSION GetCompression(char* compression);

int main(int argc, char* argv[]){
	
	char* usage = 
	"\nAGMVCLI Copyright (c) Ryandracus Keith Chapman\n"
	"........................................................................\n"
	"Welcome to the Adaptive Graphics Motion Video Command Line Tool! - v1.0\n"
	"........................................................................\n"
	"The purpose of this command line tool is to faciliate an easy-to-use environment\n"
	"to autmote the process of encoding and decoding AGMV full-motion video files!\n\n"
	"This tool relies on specially formatted .agvs script files to be loaded in as\n"
	"arguments that will contain the tokens necessary to encode or decode AGMVs.\n\n"
	".AGVS SCRIPTS HAVE TWO DISTINCT MODES THAT ARE FORMATTED DIFFERENTLY: ENCODING AND DECODING MODES\n\n"
	"ENC FMT: $video ENC $(OUTPUT_FILE_NAME) $(DIRECTORY) $(BASENAME) $(IMG_TYPE) $(START_FRAME) $(ENDFRAME) $(WIDTH)\n"
	"$(HEIGHT) $(FPS) $(OPTIMZATION_FLAG) $(QUALITY_FLAG) $(COMPRESSION)\n\n"
	"ENC EXAMPLE: $video ENC INTRO.AGMV movies/input frame_ BMP 1 247 320 240 30 OPT_II HIGH_Q LZSS\n\n"
	"$video - identifier token\nENC - encoding mode\n\n"
	"BMP - Supported Image File Types -> BMP, TGA, TIM, PCX, PVR, GXT, 3DF, PPM, LBM, LMP, BTI\n"
	"OPT_II - Optimization Flag -> OPT_I = 512 COLORS, HEAVY PDIFS, OPT_II = 256 COLORS, LIGHT PDFIS, OPT_III = 512 COLORS, LIGHT PDIFS\n"
	"OPT_GBA_I = 512 COLORS, HEAVY PDFIS, GBA RESOLUTION, OPT_NDS = 512 COLORS, HEAVY PDFIS, NDS RESOLUTION\n"
	"HIGH_Q = Quality Flag -> HIGH_Q = 19BPP COLOR DEPTH, LOW COLOR BANDING, MID_Q = 17BPP COLOR DEPTH, MODERATE COLOR BANDING, LOW_Q = 16BPP, LOTS OF COLOR BANDING\n"
	"LZSS = Compression Flag -> LZSS or LZ77 Data Compression Algorithms"
	"\n"
	"AGMVCLI can parse the contents of a .WAV, .AIFF, or .AIFC audio files to apply unique AGMV audio compression and sync it to the video via an extension as such:\n\n"
	"AUDIO ENC: $video ENC INTRO.AGMV movies/input frame_ BMP 1 247 320 240 30 OPT_II LOW_Q LZ77 -v intro.wav\n"
	"DEC FMT: $video DEC $(DIRECTORY) $(FILENAME) $(IMG_TYPE) $(AUDIO_TYPE)\n\n"
	"DEC EXAMPLE: $video DEC input/shinobi SHINOBI.agmv BMP WAV\n"
	"DEC EXAMPLE 2: $video DEC cur SHINOBI.agmv BMP AIFF";
	
	
	if(argc == 1){
		printf("%s",usage);
		return 1;
	}
	
	FILE* file = fopen(argv[1],"r");
	
	char vidtok[7], mode[4];
	fread(vidtok,1,6,file);
	fseek(file,1,SEEK_CUR);
	fread(mode,1,3,file);
	
	vidtok[6] = '\0';
	mode[3]   = '\0';

	if(vidtok[0] != '$' || vidtok[1] != 'v' || vidtok[2] != 'i' || vidtok[3] != 'd' || vidtok[4] != 'e' || vidtok[5] != 'o'){
		printf("Error: .agvs identifier token is not properly formatted! Must to be set to $video !!!\n");
		fclose(file);
		return 1;
	}
	else{	
		if(mode[0] == 'E' && mode[1] == 'N' && mode[2] == 'C'){
			char filename[100], directory[100], basename[100], type[10], opt[11], qopt[11], compression[11], at[11], track[100];
			u32 start_frame, end_frame, width, height, fps;
			fscanf(file,"%s %s %s %s %ld %ld %ld %ld %ld %s %s %s %s %s",filename,directory,basename,type,&start_frame,&end_frame,&width,&height,&fps,opt,qopt,compression,at,track);

			if(!IsAGIDLImage(type)){
				printf("Image type must be AGIDL compliant!\n");
				fclose(file);
				return 1;
			}
			else if(!IsAGMVOpt(opt)){
				printf("Error: .agvs optimization flag token is not properly formatted! Must be set to OPT_I, OPT_II, OPT_III, OPT_GBA_I, OPT_GBA_II, or OPT_GBA_NDS !!!\n");
				fclose(file);
				return 1;
			}
			else if(!IsAGMVQOpt(qopt)){
				printf("Error: .agvs quality flag token is not properly formatted! Must be set to HIGH_Q, MID_Q, or LOW_Q !!!\n");
				fclose(file);
				return 1;
			}
			else if(!IsAGMVCompression(compression)){
				printf("Error: .agvs compression flag token is not properly formatted! Must be set to LZSS or LZ77!!!\n");
				fclose(file);
				return 1;
			}
			else if(start_frame > end_frame){
				printf("Error: Start frame is greater than end frame!\n");
				fclose(file);
				return 1;
			}
			
			if(at[0] == '-' && at[1] == 'v'){
				AGMV_AUDIO_TYPE audio_type = GetAudioType(track);
				AGMV* agmv = CreateAGMV(end_frame-start_frame,width,height,fps);
				AGMV_OPT aopt = GetAGMVOpt(opt);
				
				if(aopt != AGMV_OPT_GBA_I && aopt != AGMV_OPT_GBA_II && aopt != AGMV_OPT_GBA_III){
					
					if(audio_type == AGMV_AUDIO_WAV){
						AGMV_WavToAudioTrack(track,agmv);
					}
					else if(audio_type == AGMV_AUDIO_AIFF){
						AGMV_AIFFToAudioTrack(track,agmv);
					}
					else{
						AGMV_AIFCToAudioTrack(track,agmv);
					}
				
					AGMV_EncodeAGMV(agmv,filename,directory,basename,GetImageType(type),start_frame,end_frame,width,height,fps,GetAGMVOpt(opt),GetQuality(qopt),GetCompression(compression));
				}
				else{
					if(aopt == AGMV_OPT_GBA_I || aopt == AGMV_OPT_GBA_II){
						AGMV_ExportRaw8PCM(track,(end_frame-start_frame)/2.0f);
					}
					else{
						AGMV_ExportRaw8PCM(track,(end_frame-start_frame)*0.75f);
					}
					AGMV_EncodeVideo(filename,directory,basename,GetImageType(type),start_frame,end_frame,width,height,fps,GetAGMVOpt(opt),GetQuality(qopt),GetCompression(compression));
				}
			}
			else{
				AGMV_EncodeVideo(filename,directory,basename,GetImageType(type),start_frame,end_frame,width,height,fps,GetAGMVOpt(opt),GetQuality(qopt),GetCompression(compression));
			}
			
			fclose(file);
			
		}
		else if(mode[0] == 'D' && mode[1] == 'E' && mode[2] == 'C'){
			char directory[30], filename[30], img_type[10], audio_type[10];
			fscanf(file,"%s %s %s %s",directory,filename,img_type,audio_type);
			
			if(!IsAGIDLImage(img_type)){
				printf("Image type must be AGIDL compliant!\n");
				fclose(file);
				return 1;
			}
			
			if(directory[0] == 'c' && directory[1] == 'u' && directory[2] == 'r'){
				AGMV_DecodeAGMV(filename,GetImageType(img_type),GetAudioTypeToken(audio_type));
			}
			else{
				char concat[100];
				sprintf("%s%s",directory,filename);
				AGMV_DecodeAGMV(concat,GetImageType(img_type),GetAudioTypeToken(audio_type));
				
				fclose(file);
			}
		}
		else{
			printf("Error: .agvs encoding/decoding token is not properly formatted! Must to be set to either ENC or DEC !!!\n");
			fclose(file);
			return 1;
		}
	}

	return 0; 
}

Bool IsAGIDLImage(char* ext){
	if(ext[0] == 'B' && ext[1] == 'M' && ext[2] == 'P'){
		return TRUE;
	}
	else if(ext[0] == 'T' && ext[1] == 'G' && ext[2] == 'A'){
		return TRUE;
	}
	else if(ext[0] == 'T' && ext[1] == 'I' && ext[2] == 'M'){
		return TRUE;
	}
	else if(ext[0] == 'P' && ext[1] == 'C' && ext[2] == 'X'){
		return TRUE;
	}
	else if(ext[0] == 'P' && ext[1] == 'V' && ext[2] == 'R'){
		return TRUE;
	}
	else if(ext[0] == 'L' && ext[1] == 'M' && ext[2] == 'P'){
		return TRUE;
	}
	else if(ext[0] == 'L' && ext[1] == 'B' && ext[2] == 'M'){
		return TRUE;
	}
	else if(ext[0] == '3' && ext[1] == 'D' && ext[2] == 'F'){
		return TRUE;
	}
	else if(ext[0] == 'G' && ext[1] == 'T' && ext[2] == 'X'){
		return TRUE;
	}
	else if(ext[0] == 'P' && ext[1] == 'P' && ext[2] == 'M'){
		return TRUE;
	}
	else if(ext[0] == 'B' && ext[1] == 'T' && ext[2] == 'I'){
		return TRUE;
	}
	else return FALSE;
}

AGMV_IMG_TYPE GetImageType(char* ext){
	if(ext[0] == 'B' && ext[1] == 'M' && ext[2] == 'P'){
		return AGMV_IMG_BMP;
	}
	else if(ext[0] == 'T' && ext[1] == 'G' && ext[2] == 'A'){
		return AGMV_IMG_TGA;
	}
	else if(ext[0] == 'T' && ext[1] == 'I' && ext[2] == 'M'){
		return AGMV_IMG_TIM;
	}
	else if(ext[0] == 'P' && ext[1] == 'C' && ext[2] == 'X'){
		return AGMV_IMG_PCX;
	}
	else if(ext[0] == 'P' && ext[1] == 'V' && ext[2] == 'R'){
		return AGMV_IMG_PVR;
	}
	else if(ext[0] == 'L' && ext[1] == 'M' && ext[2] == 'P'){
		return AGMV_IMG_LMP;
	}
	else if(ext[0] == 'L' && ext[1] == 'B' && ext[2] == 'M'){
		return AGMV_IMG_LBM;
	}
	else if(ext[0] == '3' && ext[1] == 'D' && ext[2] == 'F'){
		return AGMV_IMG_3DF;
	}
	else if(ext[0] == 'G' && ext[1] == 'T' && ext[2] == 'X'){
		return AGMV_IMG_GXT;
	}
	else if(ext[0] == 'P' && ext[1] == 'P' && ext[2] == 'M'){
		return AGMV_IMG_PPM;
	}
	else if(ext[0] == 'B' && ext[1] == 'T' && ext[2] == 'I'){
		return AGMV_IMG_BTI;
	}
	else return AGMV_IMG_BMP;
}

Bool IsAGMVOpt(char* opt){
	if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'I' && opt[5] != 'I'){
		return TRUE;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'I' && opt[5] == 'I' && opt[6] != 'I'){
		return TRUE;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'I' && opt[5] == 'I' && opt[6] == 'I'){
		return TRUE;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'G' && opt[5] == 'B' && opt[6] == 'A' && opt[7] == '_' && opt[8] == 'I' && opt[9] != 'I'){
		return TRUE;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'G' && opt[5] == 'B' && opt[6] == 'A' && opt[7] == '_' && opt[8] == 'I' && opt[9] == 'I' && opt[10] == 'I'){
		return TRUE;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'A' && opt[5] == 'N' && opt[6] == 'I' && opt[7] == 'M'){
		return TRUE;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'N' && opt[5] == 'D' && opt[6] == 'S'){
		return TRUE;
	}
	else return FALSE;
}

AGMV_OPT GetAGMVOpt(char* opt){
	if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'I' && opt[5] != 'I'){
		return AGMV_OPT_I;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'I' && opt[5] == 'I' && opt[6] != 'I'){
		return AGMV_OPT_II;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'I' && opt[5] == 'I' && opt[6] == 'I'){
		return AGMV_OPT_III;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'G' && opt[5] == 'B' && opt[6] == 'A' && opt[7] == '_' && opt[8] == 'I' && opt[9] != 'I'){
		return AGMV_OPT_GBA_I;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'G' && opt[5] == 'B' && opt[6] == 'A' && opt[7] == '_' && opt[8] == 'I' && opt[9] == 'I' && opt[10] != 'I'){
		return AGMV_OPT_GBA_II;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'G' && opt[5] == 'B' && opt[6] == 'A' && opt[7] == '_' && opt[8] == 'I' && opt[9] == 'I' && opt[10] == 'I'){
		return AGMV_OPT_GBA_III;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'A' && opt[5] == 'N' && opt[6] == 'I' && opt[7] == 'M'){
		return AGMV_OPT_ANIM;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'N' && opt[5] == 'D' && opt[6] == 'S'){
		return AGMV_OPT_NDS;
	}
	else return AGMV_OPT_I;
}

AGMV_AUDIO_TYPE GetAudioType(char* ext){
	int i;
	for(i = 0; i < strlen(ext); i++){
		if(ext[i] == '.'){
			if(ext[i+1] == 'w' && ext[i+2] == 'a' && ext[i+3] == 'v'){
				return AGMV_AUDIO_WAV;
			}
			if(ext[i+1] == 'a' && ext[i+2] == 'i' && ext[i+3] == 'f' && ext[i+4] == 'f'){
				return AGMV_AUDIO_AIFF;
			}
			if(ext[i+1] == 'a' && ext[i+2] == 'i' && ext[i+3] == 'f' && ext[i+4] == 'c'){
				return AGMV_AUDIO_AIFC;
			}
		}
	}
	return AGMV_AUDIO_WAV;
}

AGMV_AUDIO_TYPE GetAudioTypeToken(char* token){
	if(token[0] == 'W' && token[1] == 'A' && token[2] == 'V'){
		return AGMV_AUDIO_WAV;
	}
	else if(token[0] == 'A' && token[1] == 'I' && token[2] == 'F' && token[3] == 'F'){
		return AGMV_AUDIO_AIFF;
	}
	else if(token[0] == 'A' && token[1] == 'I' && token[2] == 'F' && token[3] == 'C'){
		return AGMV_AUDIO_AIFC;
	}
	else return AGMV_AUDIO_WAV;
}

Bool IsAGMVQOpt(char* opt){
	if(opt[0] == 'H' && opt[1] == 'I' && opt[2] == 'G' && opt[3] == 'H' && opt[4] == '_' && opt[5] == 'Q'){
		return TRUE;
	}
	else if(opt[0] == 'M' && opt[1] == 'I' && opt[2] == 'D' && opt[3] == '_' && opt[4] == 'Q'){
		return TRUE;
	}
	else if(opt[0] == 'L' && opt[1] == 'O' && opt[2] == 'W' && opt[3] == '_' && opt[4] == 'Q'){
		return TRUE;
	}
	else return FALSE;
}

AGMV_QUALITY GetQuality(char* opt){
	if(opt[0] == 'H' && opt[1] == 'I' && opt[2] == 'G' && opt[3] == 'H' && opt[4] == '_' && opt[5] == 'Q'){
		return AGMV_HIGH_QUALITY;
	}
	else if(opt[0] == 'M' && opt[1] == 'I' && opt[2] == 'D' && opt[3] == '_' && opt[4] == 'Q'){
		return AGMV_MID_QUALITY;
	}
	else if(opt[0] == 'L' && opt[1] == 'O' && opt[2] == 'W' && opt[3] == '_' && opt[4] == 'Q'){
		return AGMV_LOW_QUALITY;
	}
	else return AGMV_HIGH_QUALITY;
}

Bool IsAGMVCompression(char* compression){
	if(compression[0] == 'L' && compression[1] == 'Z' && compression[2] == 'S' && compression[3] == 'S'){
		return TRUE;
	}
	else if(compression[0] == 'L' && compression[1] == 'Z' && compression[2] == '7' && compression[3] == '7'){
		return TRUE;
	}
	else return FALSE;
}

AGMV_COMPRESSION GetCompression(char* compression){
	if(compression[0] == 'L' && compression[1] == 'Z' && compression[2] == 'S' && compression[3] == 'S'){
		return AGMV_LZSS_COMPRESSION;
	}
	else if(compression[0] == 'L' && compression[1] == 'Z' && compression[2] == '7' && compression[3] == '7'){
		return AGMV_LZ77_COMPRESSION;
	}
	else return AGMV_LZSS_COMPRESSION;
}