# libagmv - Adaptive Graphics Motion Video
An open-source full-motion video codec optimized for real-time playback on low-end graphical devices.

## About
AGMV is an indexed colored, block-based full-motion video codec that utilizes classic
color quantization, progressive interpolated motion compensation, lz77/lzss data compression,
a routine 2:1 audio compression algorithm, and various other old-school video compression techniques
inspired from the classics such as Smacker video and Quake 2's Cinematics video formats to achieve real-time video playback on low-end graphical devices
favoring decoding speed over aggressive compression routines to ensure that even a CPU clocked at 16.78MHz like the Nintendo Gameboy Advance can decode individual frames in real time.

Why use AGMV? AGMV serves four purposes:

	* It maintains blazing fast decoding speed to even where low-end hardware such as the GBA can use it for real-time video playback.
	* It uses the RGB color space, the native color space of most graphical devices, to avoid any intermediary color conversion step so that a decoded frame can be immediately blitted to the screen. 
	* At the cost of some visual quality, it can even rival Smacker video, MJPEG, and Cinepak compression rates while being virtually 4x easier to decode.
	
## GBA Video Demo

https://github.com/RyandracusCodesGames/libagmv/assets/108719757/9c914d0a-4635-4dac-82ff-a18c1e7b5215

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
	AGMV_EncodeAGMV(agmv,"example.agmv","example_directory","agmv_base_name",1,212,320,240,24,AGMV_OPT_III,AGMV_LOW_QUALITY,AGMV_LZSS_COMPRESSION);
	
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

