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
*   Version: 2.0
*   Updated: 10/26/2024
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
Bool IsAGMVConv(char* conversion);

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
	".AGVS SCRIPTS HAVE THREE DISTINCT MODES THAT ARE FORMATTED DIFFERENTLY: ENCODING, DECODING, AND CONVERSION MODES\n\n"
	"ENC FMT: $video ENC $(OUTPUT_FILE_NAME) $(DIRECTORY) $(BASENAME) $(IMG_TYPE) $(START_FRAME) $(ENDFRAME) $(WIDTH)\n"
	"$(HEIGHT) $(FPS) $(OPTIMZATION_FLAG) $(QUALITY_FLAG) $(COMPRESSION)\n\n"
	"ENC EXAMPLE: $video ENC INTRO.AGMV movies/input frame_ BMP 1 247 320 240 30 OPT_II HIGH_Q LZSS\n\n"
	"$video - identifier token\nENC - encoding mode\n\n"
	"BMP - Supported Image File Types -> BMP, TGA, TIM, PCX, PVR, GXT, 3DF, PPM, LBM, LMP, BTI\n"
	"OPT_II - Optimization Flag -> OPT_I = 512 COLORS, HEAVY PDIFS, OPT_II = 256 COLORS, LIGHT PDFIS, OPT_III = 512 COLORS, LIGHT PDIFS, OPT_IV = 32768 COLORS, LIGHT PDIFS\n"
	"OPT_GBA_I = 512 COLORS, HEAVY PDFIS, GBA RESOLUTION, OPT_NDS_I = 512 COLORS, HEAVY PDFIS, NDS RESOLUTION\n"
	"HIGH_Q = Quality Flag -> CUSTOM = VARIABLE, HIGH_Q = 17BPP COLOR DEPTH, LOW COLOR BANDING, MID_Q = 16BPP COLOR DEPTH, MODERATE BLOCK ARTIFACTING AND COLOR BANDING, LOW_Q = 15BPP, HEAVY BLOCK ARTIFACTING, LOTS OF COLOR BANDING\n"
	"LZSS = Compression Flag -> LZSS or NONE Data Compression Algorithms"
	"\n"
	"AGMVCLI can parse the contents of a .WAV, .AIFF, or .AIFC audio files to apply unique AGMV audio compression and sync it to the video via an extension as such:\n\n"
	"AUDIO ENC: $video ENC INTRO.AGMV movies/input frame_ BMP 1 247 320 240 30 OPT_II LOW_Q LZ77 -v intro.wav\n"
	"AUDIO GBA 1: $video ENC INTRO.AGMV movies/input frame_ BMP 1 247 320 240 30 OPT_GBA_I LOW_Q LZ77 -v intro.wav\n"
	"AUDIO GBA 2: $video ENC INTRO.AGMV movies/input frame_ BMP 1 247 320 240 30 OPT_GBA_I LOW_Q NONE -v intro.raw 16000\n"
	"DEC FMT: $video DEC $(DIRECTORY) $(FILENAME) $(IMG_TYPE) $(AUDIO_TYPE)\n\n"
	"DEC EXAMPLE: $video DEC input/shinobi SHINOBI.agmv BMP WAV\n"
	"DEC EXAMPLE 2: $video DEC cur SHINOBI.agmv BMP AIFF\n"
	"\n"
	"CONV EXAMPLE: $video CONV AGMV AVI example.agmv\n"
	"CONV EXAMPLE: $video CONV AVI AGMV example.avi OPT_I HIGH_Q LZSS\n"
	"DITHERED CONV EXAMPLE: $video CONV AVI AGMV example.avi OPT_I HIGH_Q LZSS D";
	
	
	if(argc == 1){
		printf("%s",usage);
		return 1;
	}
	
	FILE* file = fopen(argv[1],"r");
	
	char vidtok[7], mode[10];
	fread(vidtok,1,6,file);
	fseek(file,1,SEEK_CUR);
	fread(mode,1,3,file);
	
	vidtok[6] = '\0';
	mode[3]   = '\0';
	
	if(mode[0] == 'C'){
		fseek(file,7,SEEK_SET);
		fread(mode,1,4,file);
		mode[4] = '\0';
	}

	if(vidtok[0] != '$' || vidtok[1] != 'v' || vidtok[2] != 'i' || vidtok[3] != 'd' || vidtok[4] != 'e' || vidtok[5] != 'o'){
		printf("Error: .agvs identifier token is not properly formatted! Must to be set to $video !!!\n");
		fclose(file);
		return 1;
	}
	else{	
		if(mode[0] == 'E' && mode[1] == 'N' && mode[2] == 'C'){
			AGMV_CUSTOMIZATION cq;
			char filename[100], directory[100], basename[100], type[10], opt[11], qopt[11], compression[11], at[11], track[100];
			u32 start_frame, end_frame, width, height, fps, sample_rate;
			fscanf(file,"%s %s %s %s %ld %ld %ld %ld %ld %s %s %s %s %s %ld",filename,directory,basename,type,&start_frame,&end_frame,&width,&height,&fps,opt,qopt,compression,at,track,&sample_rate);

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
			
			if(GetQuality(qopt) == AGMV_CUSTOM_QUALITY){
				FILE* customq = fopen("agmvcli.quality","rb");
				
				if(customq == NULL){
					printf("Error: File agmvcli.quality not found in local directory! Must be present to set custom quality parameters!!!\n");
					fclose(customq);
					fclose(file);
					return 1;
				}
				
				fscanf(customq,"%d %d %d %d %d %d %d %d",&cq.b4x4.fill,&cq.b4x4.icopy,&cq.b4x4.copy,&cq.b4x4.pcopy,&cq.b4x4.pmv,&cq.b4x4.mv,&cq.b4x4.vq2,&cq.b4x4.vq4);
				fscanf(customq,"%d %d %d %d %d %d %d %d",&cq.b8x4.fill,&cq.b8x4.icopy,&cq.b8x4.copy,&cq.b8x4.pcopy,&cq.b8x4.pmv,&cq.b8x4.mv,&cq.b8x4.vq2,&cq.b8x4.vq4);
				fscanf(customq,"%d %d %d %d %d %d %d %d",&cq.b2x4.fill,&cq.b2x4.icopy,&cq.b2x4.copy,&cq.b2x4.pcopy,&cq.b2x4.pmv,&cq.b2x4.mv,&cq.b2x4.vq2,&cq.b2x4.vq4);
				fscanf(customq,"%d %d %d %d %d %d %d %d",&cq.b8x8.fill,&cq.b8x8.icopy,&cq.b8x8.copy,&cq.b8x8.pcopy,&cq.b8x8.pmv,&cq.b8x8.mv,&cq.b8x8.vq2,&cq.b8x8.vq4);
				
				fclose(customq);
			}

			if(at[0] == '-' && at[1] == 'v'){
				AGMV_AUDIO_TYPE audio_type = GetAudioType(track);
				AGMV* agmv = CreateAGMV(end_frame-start_frame,width,height,fps);
				AGMV_OPT aopt = GetAGMVOpt(opt);
				
				agmv->custom.b4x4.fill  = cq.b4x4.fill;
				agmv->custom.b4x4.icopy = cq.b4x4.icopy;
				agmv->custom.b4x4.copy  = cq.b4x4.copy;
				agmv->custom.b4x4.pcopy = cq.b4x4.pcopy;
				agmv->custom.b4x4.mv    = cq.b4x4.mv;
				agmv->custom.b4x4.pmv   = cq.b4x4.pmv;
				agmv->custom.b4x4.vq2   = cq.b4x4.vq2;
				agmv->custom.b4x4.vq4   = cq.b4x4.vq4;
				
				agmv->custom.b8x4.fill  = cq.b8x4.fill;
				agmv->custom.b8x4.icopy = cq.b8x4.icopy;
				agmv->custom.b8x4.copy  = cq.b8x4.copy;
				agmv->custom.b8x4.pcopy = cq.b8x4.pcopy;
				agmv->custom.b8x4.mv    = cq.b8x4.mv;
				agmv->custom.b8x4.pmv   = cq.b8x4.pmv;
				agmv->custom.b8x4.vq2   = cq.b8x4.vq2;
				agmv->custom.b8x4.vq4   = cq.b8x4.vq4;
				
				agmv->custom.b2x4.fill  = cq.b2x4.fill;
				agmv->custom.b2x4.icopy = cq.b2x4.icopy;
				agmv->custom.b2x4.copy  = cq.b2x4.copy;
				agmv->custom.b2x4.pcopy = cq.b2x4.pcopy;
				agmv->custom.b2x4.mv    = cq.b2x4.mv;
				agmv->custom.b2x4.pmv   = cq.b2x4.pmv;
				agmv->custom.b2x4.vq2   = cq.b2x4.vq2;
				agmv->custom.b2x4.vq4   = cq.b2x4.vq4;
				
				agmv->custom.b8x8.fill  = cq.b8x8.fill;
				agmv->custom.b8x8.icopy = cq.b8x8.icopy;
				agmv->custom.b8x8.copy  = cq.b8x8.copy;
				agmv->custom.b8x8.pcopy = cq.b8x8.pcopy;
				agmv->custom.b8x8.mv    = cq.b8x8.mv;
				agmv->custom.b8x8.pmv   = cq.b8x8.pmv;
				agmv->custom.b8x8.vq2   = cq.b8x8.vq2;
				agmv->custom.b8x8.vq4   = cq.b8x8.vq4;
				
				if(aopt != AGMV_OPT_GBA_I && aopt != AGMV_OPT_GBA_II && aopt != AGMV_OPT_GBA_III && aopt != AGMV_OPT_GBA_IV){
					
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
					if(audio_type == AGMV_AUDIO_WAV){
						AGMV_WavToAudioTrack(track,agmv);
					}
					else if(audio_type == AGMV_AUDIO_AIFF){
						AGMV_AIFFToAudioTrack(track,agmv);
					}
					else if(audio_type == AGMV_AUDIO_AIFC){
						AGMV_AIFCToAudioTrack(track,agmv);
					}
					else{
						if(sample_rate < 8000 || sample_rate > 32000){
							printf("Error: Sample rate must be between 8000 and 32000 for the GBA!\n");
							DestroyAGMV(agmv);
							fclose(file);
							return 1;
						}
						
						AGMV_RawSignedPCMToAudioTrack(track,agmv,1,sample_rate);
					}
					
					if(AGMV_GetBitsPerSample(agmv) != 8){
						printf("Error: Bits per sample must be 8 for the GBA!\n");
						DestroyAGMV(agmv);
						fclose(file);
						return 1;
					}
					
					if(AGMV_GetNumberOfChannels(agmv) != 1){
						printf("Error: Number of channels must be 1 for the GBA!\n");
						DestroyAGMV(agmv);
						fclose(file);
						return 1;
					}
					
					AGMV_EncodeAGMV(agmv,filename,directory,basename,GetImageType(type),start_frame,end_frame,width,height,fps,GetAGMVOpt(opt),GetQuality(qopt),GetCompression(compression));
				}
			}
			else{
				if(GetQuality(qopt) == AGMV_CUSTOM_QUALITY){
					AGMV_EncodeVideo(filename,directory,basename,GetImageType(type),start_frame,end_frame,width,height,fps,GetAGMVOpt(opt),GetQuality(qopt),GetCompression(compression),&cq);
				}
				else{
					AGMV_EncodeVideo(filename,directory,basename,GetImageType(type),start_frame,end_frame,width,height,fps,GetAGMVOpt(opt),GetQuality(qopt),GetCompression(compression),NULL);
				}
			}
			
			fclose(file);
			
			return 0;
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
			
			return 0;
		}
		else if(mode[0] == 'C' && mode[1] == 'O' && mode[2] == 'N' && mode[3] == 'V'){
			char c1[10], c2[10], filename[50], opt[11], qopt[11], compression[11], dither[11];
			fscanf(file,"%s %s %s %s %s %s %s",c1,c2,filename,opt,qopt,compression,dither);
			
			if(!IsAGMVConv(c1) || !IsAGMVConv(c2)){
				printf("Error: .agvs conversion token is not properly formatted! Must be set to AVI or AGMV!!!\n");
				fclose(file);
				return 1;
			}
			
			if(c1[1] == 'G'){
				AGMV_ConvertAGMVtoAVI(filename);
			}
			else{
				if(!IsAGMVOpt(opt)){
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
				
				if(dither[0] == 'D'){
					AGMV_EnableDither();
				}
				
				AGMV_ConvertAVItoAGMV(filename,GetAGMVOpt(opt),GetQuality(qopt),GetCompression(compression));
			}
			
			fclose(file);
		}
		else{
			printf("Error: .agvs encoding/decoding token is not properly formatted! Must to be set to either ENC, DEC, or CONV !!!\n");
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
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'I' && opt[5] == 'V'){
		return TRUE;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'G' && opt[5] == 'B' && opt[6] == 'A' && opt[7] == '_' && opt[8] == 'I' && opt[9] != 'I'){
		return TRUE;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'G' && opt[5] == 'B' && opt[6] == 'A' && opt[7] == '_' && opt[8] == 'I' && opt[9] == 'I' && opt[10] != 'I'){
		return TRUE;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'G' && opt[5] == 'B' && opt[6] == 'A' && opt[7] == '_' && opt[8] == 'I' && opt[9] == 'I' && opt[10] == 'I'){
		return TRUE;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'G' && opt[5] == 'B' && opt[6] == 'A' && opt[7] == '_' && opt[8] == 'I' && opt[9] == 'V'){
		return TRUE;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'P' && opt[5] == 'S' && opt[6] == 'X' && opt[7] == '_' && opt[8] == 'I' && opt[9] != 'I'){
		return TRUE;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'P' && opt[5] == 'S' && opt[6] == 'X' && opt[7] == '_' && opt[8] == 'I' && opt[9] == 'I' && opt[10] != 'I'){
		return TRUE;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'P' && opt[5] == 'S' && opt[6] == 'X' && opt[7] == '_' && opt[8] == 'I' && opt[9] == 'I' && opt[10] == 'I'){
		return TRUE;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'P' && opt[5] == 'S' && opt[6] == 'X' && opt[7] == '_' && opt[8] == 'I' && opt[9] == 'V'){
		return TRUE;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'N' && opt[5] == 'D' && opt[6] == 'S' && opt[7] == '_' && opt[8] == 'I' && opt[9] != 'I'){
		return TRUE;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'N' && opt[5] == 'D' && opt[6] == 'S' && opt[7] == '_' && opt[8] == 'I' && opt[9] == 'I' && opt[10] != 'I'){
		return TRUE;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'N' && opt[5] == 'D' && opt[6] == 'S' && opt[7] == '_' && opt[8] == 'I' && opt[9] == 'I' && opt[10] == 'I'){
		return TRUE;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'N' && opt[5] == 'D' && opt[6] == 'S' && opt[7] == '_' && opt[8] == 'I' && opt[9] == 'V'){
		return TRUE;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'A' && opt[5] == 'N' && opt[6] == 'I' && opt[7] == 'M'){
		return TRUE;
	}
	else return FALSE;
}

AGMV_OPT GetAGMVOpt(char* opt){
	if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'I' && opt[5] != 'I' && opt[5] != 'V'){
		return AGMV_OPT_I;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'I' && opt[5] == 'I' && opt[6] != 'I'){
		return AGMV_OPT_II;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'I' && opt[5] == 'I' && opt[6] == 'I'){
		return AGMV_OPT_III;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'I' && opt[5] == 'V'){
		return AGMV_OPT_IV;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'G' && opt[5] == 'B' && opt[6] == 'A' && opt[7] == '_' && opt[8] == 'I' && opt[9] != 'I' && opt[9] != 'V'){
		return AGMV_OPT_GBA_I;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'G' && opt[5] == 'B' && opt[6] == 'A' && opt[7] == '_' && opt[8] == 'I' && opt[9] == 'I' && opt[10] != 'I'){
		return AGMV_OPT_GBA_II;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'G' && opt[5] == 'B' && opt[6] == 'A' && opt[7] == '_' && opt[8] == 'I' && opt[9] == 'I' && opt[10] == 'I'){
		return AGMV_OPT_GBA_III;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'G' && opt[5] == 'B' && opt[6] == 'A' && opt[7] == '_' && opt[8] == 'I' && opt[9] == 'V'){
		return AGMV_OPT_GBA_IV;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'P' && opt[5] == 'S' && opt[6] == 'X' && opt[7] == '_' && opt[8] == 'I' && opt[9] != 'I' && opt[9] != 'V'){
		return AGMV_OPT_PSX_I;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'P' && opt[5] == 'S' && opt[6] == 'X' && opt[7] == '_' && opt[8] == 'I' && opt[9] == 'I' && opt[10] != 'I'){
		return AGMV_OPT_PSX_II;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'P' && opt[5] == 'S' && opt[6] == 'X' && opt[7] == '_' && opt[8] == 'I' && opt[9] == 'I' && opt[10] == 'I'){
		return AGMV_OPT_PSX_III;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'P' && opt[5] == 'S' && opt[6] == 'X' && opt[7] == '_' && opt[8] == 'I' && opt[9] == 'V'){
		return AGMV_OPT_PSX_IV;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'N' && opt[5] == 'D' && opt[6] == 'S' && opt[7] == '_' && opt[8] == 'I' && opt[9] != 'I' && opt[9] != 'V'){
		return AGMV_OPT_NDS_I;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'N' && opt[5] == 'D' && opt[6] == 'S' && opt[7] == '_' && opt[8] == 'I' && opt[9] == 'I' && opt[10] != 'I'){
		return AGMV_OPT_NDS_II;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'N' && opt[5] == 'D' && opt[6] == 'S' && opt[7] == '_' && opt[8] == 'I' && opt[9] == 'I' && opt[10] == 'I'){
		return AGMV_OPT_NDS_III;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'N' && opt[5] == 'D' && opt[6] == 'S' && opt[7] == '_' && opt[8] == 'I' && opt[9] == 'V'){
		return AGMV_OPT_NDS_IV;
	}
	else if(opt[0] == 'O' &&  opt[1] == 'P' && opt[2] == 'T' && opt[3] == '_' && opt[4] == 'A' && opt[5] == 'N' && opt[6] == 'I' && opt[7] == 'M'){
		return AGMV_OPT_ANIM;
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
			if(ext[i+1] == 'r' && ext[i+2] == 'a' && ext[i+3] == 'w'){
				return AGMV_AUDIO_RAW;
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
	else if(opt[0] == 'C' && opt[1] == 'U' && opt[2] == 'S' && opt[3] == 'T' && opt[4] == 'O' && opt[5] == 'M'){
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
	else if(opt[0] == 'C' && opt[1] == 'U' && opt[2] == 'S' && opt[3] == 'T' && opt[4] == 'O' && opt[5] == 'M'){
		return AGMV_CUSTOM_QUALITY;
	}
	else return AGMV_MID_QUALITY;
}

Bool IsAGMVCompression(char* compression){
	if(compression[0] == 'L' && compression[1] == 'Z' && compression[2] == 'S' && compression[3] == 'S'){
		return TRUE;
	}
	else if(compression[0] == 'N' && compression[1] == 'O' && compression[2] == 'N' && compression[3] == 'E'){
		return TRUE;
	}
	else return FALSE;
}

AGMV_COMPRESSION GetCompression(char* compression){
	if(compression[0] == 'L' && compression[1] == 'Z' && compression[2] == 'S' && compression[3] == 'S'){
		return AGMV_LZSS_COMPRESSION;
	}
	else if(compression[0] == 'N' && compression[1] == 'O' && compression[2] == 'N' && compression[3] == 'E'){
		return AGMV_NONE_COMPRESSION;
	}
	else return AGMV_LZSS_COMPRESSION;
}

Bool IsAGMVConv(char* conversion){
	if(conversion[0] == 'A' && conversion[1] == 'G' && conversion[2] == 'M' && conversion[3] == 'V'){
		return TRUE;
	}
	else if(conversion[0] == 'A' && conversion[1] == 'V' && conversion[2] == 'I'){
		return TRUE;
	}
	else return FALSE;
}