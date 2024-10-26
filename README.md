# libagmv - Adaptive Graphics Motion Video
An open-source full-motion video codec optimized for real-time playback on low-end graphical devices.
![agmv](https://github.com/RyandracusCodesGames/libagmv/assets/108719757/9cbb12b4-4a41-40e5-9fb7-9ba0fb9427cd)

## About
At its core, libagmv is an indexed-colored, block-based full-motion video codec styled and flavored by a bygone era
of video codecs such as Interplay MVE, Smacker Video, Caimans Video Codec for the GBA, and the Quake 2 Cinematics video format.
libagmv employs classic retro-style video compression techniques of the mid-90s such as classic color quantization, block-based motion compensation, 
variable block-sizing and block subdivision, intraframe prediction, block copying, minor vector quantization elements, and a simple DPCM based 2:1 audio compression
routine, all in an effort to produce high-quality video that can be efficiently decoded in real-time by resource-limited systems, both in the graphical and processing power sense,
such as the 16.78MHz clock speed of the Nintendo Game Boy Advance.

Why use AGMV? AGMV serves four purposes:

* It maintains a unique blend of high-quality video and fast decoding for real-time playback on low-end graphical devices.
* It's open-source and has been ported to other platforms such as the Nintendo Game Boy Advance, DS, and Sony PlayStation.
* Its' compression and container formats are flexible and can be extended while maintaining compatibility with older versions.
* It utilizes the RGB color space, the native color space of most graphical devices, to avoid any intermediary color conversion step so that a decoded frame can be immediately blitted to the screen.
	
## GBA Video Demo

https://github.com/user-attachments/assets/f62adc24-0322-4a99-823c-a3d1e6242d36

## PSX Video Demo

https://github.com/user-attachments/assets/ec8ed678-616b-4bea-9782-fc743a00036e

## Video Player Demo

https://github.com/RyandracusCodesGames/libagmv/assets/108719757/f77fd24f-c94d-40b2-ba30-884fdaa9b481

# Example Programs

## Simple Video Encoding
```c
#include <stdio.h>
#include <agmv.h>
#include <agidl.h>

int main(){
	AGMV* agmv = CreateAGMV(212,320,240,24); /* (Number of Frames, Width, Height, Frames per Second) */
	AGMV_WavToAudioTrack("example.wav",agmv);
	AGMV_EncodeAGMV(agmv,"example.agmv","example_directory","agmv_base_name",AGMV_IMG_BMP,1,212,320,240,24,AGMV_OPT_III,AGMV_HIGH_QUALITY,AGMV_LZSS_COMPRESSION);
	
	return 0;
}

```
## Simple GBA Video Encoding
```c
#include <stdio.h>
#include <agmv.h>

int main(){
	u32 total_num_frames = 210;
 	
 	/* GENERATES A C HEADER FILE CONTAINING AGMV VIDEO*/
	AGMV* agmv = CreateAGMV(total_num_frames-1,320,240,24);
	AGMV_RawSignedPCMToAudioTrack("example.raw",agmv,1,16000);
	AGMV_EncodeAGMV(agmv,"example.agmv","example_directory","agmv_base_name",AGMV_IMG_BMP,1,total_num_frames,320,240,24,AGMV_OPT_GBA_I,AGMV_MID_QUALITY,AGMV_LZSS_COMPRESSION);
	
	return 0;
}
```
## Simple Video Decoding
```c
#include <stdio.h>
#include <agmv.h>
#include <agidl.h>

int main(){

	AGMV_DecodeAGMV("example.agmv",AGMV_IMG_BMP,AGMV_AUDIO_WAV);
	
	return 0;
}

```
## Simple Audio Decoding
```c
#include <stdio.h>
#include <agmv.h>
#include <agidl.h>

int main(){

	AGMV_DecodeAudio("example.agmv",AGMV_AUDIO_WAV);
	
	return 0;
}

```
## Simple AVI Video Conversion
```c
#include <stdio.h>
#include <agmv.h>

int main(){

	AGMV_ConvertAVItoAGMV("example.avi",AGMV_OPT_I,AGMV_HIGH_QUALITY,AGMV_NONE_COMPRESSION);
	
	return 0;
}

```
## Commands in FFMPEG to Extract Frames for GBA(v1.1)
1. Convert frame rate of video to 24 frames per second(use 16 if size is really important) ```ffmpeg -i example.mp4 -filter:v fps=fps=24 new_example.mp4```
2. Extract audio from new video and convert to GBA compatible format ```ffmpeg -i new_example.mp4 -ac 1 -ar 16000 -c:a pcm_u8 example.wav```
3. Extract image frames from video ```ffmpeg -i new_example.mp4 agmv_frame_%d.bmp```

## Commands in FFMPEG to Extract Frames for GBA(v1.0)
1. Convert frame rate of video to 24 frames per second(use 16 if size is really important) ```ffmpeg -i example.mp4 -filter:v fps=fps=24 new_example.mp4```
2. Extract audio from new video ```ffmpeg -i new_example.mp4 example.wav```
3. Extract image frames from video ```ffmpeg -i new_example.mp4 agmv_frame_%d.bmp```
4. In audacity, drag and drop ```example.wav```
5. Click on ```Tracks``` on the top bar and select ```Mix Stereo Down to Mono``` if the audio is not already mono.
6. Now, still on ```Tracks```, select resample, and choose ```16,000```.
7. Next, click on ```File``` at the top, ```Export```, then ```Export Audio```.
8. In ```Save Type```, select other compressed files
9. In ```Header```, select ```Raw(header-less)```
10. In ```encoding```, select ```Signed 8-bit PCM```
11. Now, click save

