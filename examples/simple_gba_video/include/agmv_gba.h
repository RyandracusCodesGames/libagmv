#ifndef AGMV_GBA_H
#define AGMV_GBA_H

/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_gba.h
*   Date: 5/20/2024
*   Version: 1.0
*   Updated: 6/7/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <gba.h>

typedef unsigned char   u8;
typedef unsigned short u16;
typedef unsigned long  u32;

typedef int Bool;

#define TRUE  1
#define FALSE 0

typedef struct File{
	u32 len;
	u32 pos;
	const u8* data;
}File;

#define SEEK_SET 0
#define SEEK_CUR 1

/*--------------GBA FILE I/O ROUTINES----------------*/
Bool IWRAM AGMV_EOF(File* file){
	if(file->pos + 1 >= file->len){
		return TRUE;
	}
	else return FALSE;
}

static inline u8 IWRAM AGMV_ReadByte(File* file){
	return file->data[file->pos++];
}

static inline u16 IWRAM AGMV_ReadShort(File* file){
	u8 lsb = AGMV_ReadByte(file);
	u8 msb = AGMV_ReadByte(file);
	
	return msb << 8 | lsb;
}

static inline u32 IWRAM AGMV_ReadLong(File* file){
	u8 lsb = AGMV_ReadByte(file);
	u8 lsb2 = AGMV_ReadByte(file);
	u8 msb1 = AGMV_ReadByte(file);
	u8 msb2 = AGMV_ReadByte(file);
	
	return msb2 << 24 | msb1 << 16 | lsb2 << 8 | lsb;
}

u32 bitbuf = 0,
    bitsin = 0,
    masks[17] = {0,1,3,7,15,31,63,127,255,511,1023,2047,4095,8191,16383,32767,65535};
		
static inline u32 IWRAM AGMV_ReadBits(File* file, u32 num_of_bits){
	register u32 i;

	i = bitbuf >> (8 - bitsin);

	while (num_of_bits > bitsin)
	{
			bitbuf = AGMV_ReadByte(file);
			i |= (bitbuf << bitsin);
			bitsin += 8;
	}

	bitsin -= num_of_bits;

	return (i & masks[num_of_bits]);
}

static inline void AGMV_FlushReadBits() {
    bitbuf = 0;
    bitsin = 0;
}

void Open(File* file, const u8* data, u32 len){
	file->pos = 0;
	file->len = len;
	file->data = data;
}

static inline u32 IWRAM tell(File* file){
	return file->pos;
}

static inline u32 IWRAM seek(File* file, u32 offset, u8 mode){
	if(mode == SEEK_SET){
		file->pos = offset;
	}
	if(mode == SEEK_CUR){
		if(file->pos + offset < file->len){
			file->pos += offset;
		}
	}
}

void read(File* file, u8* buf, u32 count){
	if(file->pos + count <= file->len){
		int i;
		for(i = 0; i < count; i++){
			buf[i] = AGMV_ReadByte(file);
		}
	}
}

static inline void IWRAM AGMV_ReadFourCC(File* file, char fourcc[4]){
	fourcc[0] = AGMV_ReadByte(file);
	fourcc[1] = AGMV_ReadByte(file);
	fourcc[2] = AGMV_ReadByte(file);
	fourcc[3] = AGMV_ReadByte(file);
}

/*---------GBA AUDIO ROUTINES----------------------*/

volatile unsigned short* buttons = (volatile unsigned short*) 0x04000130;

/* the bit positions indicate each button - the first bit is for A, second for
 * B, and so on, each constant below can be ANDED into the register to get the
 * status of any one button */
#define BUTTON_A (1 << 0)
#define BUTTON_B (1 << 1)
#define BUTTON_SELECT (1 << 2)
#define BUTTON_START (1 << 3)
#define BUTTON_RIGHT (1 << 4)
#define BUTTON_LEFT (1 << 5)
#define BUTTON_UP (1 << 6)
#define BUTTON_DOWN (1 << 7)
#define BUTTON_R (1 << 8)
#define BUTTON_L (1 << 9)

/* define the timer control registers */
volatile unsigned short* timer0_data = (volatile unsigned short*) 0x4000100;
volatile unsigned short* timer0_control = (volatile unsigned short*) 0x4000102;

/* make defines for the bit positions of the control register */
#define TIMER_FREQ_1 0x0
#define TIMER_FREQ_64 0x2
#define TIMER_FREQ_256 0x3
#define TIMER_FREQ_1024 0x4
#define TIMER_ENABLE 0x80
#define TIMER_IRQ_ENABLE	0x40

/* the GBA clock speed is fixed at this rate */
#define CLOCK 16777216 
#define CYCLES_PER_BLANK 280806

/* turn DMA on for different sizes */
#define DMA_ENABLE 0x80000000
#define DMA_16 0x00000000
#define DMA_32 0x04000000

/* this causes the DMA destination to be the same each time rather than increment */
#define DMA_DEST_FIXED 0x400000

/* this causes the DMA to repeat the transfer automatically on some interval */
#define DMA_REPEAT 0x2000000

/* this causes the DMA repeat interval to be synced with timer 0 */
#define DMA_SYNC_TO_TIMER 0x30000000

/* pointers to the DMA source/dest locations and control registers */
volatile unsigned int* dma1_source = (volatile unsigned int*) 0x40000BC;
volatile unsigned int* dma1_destination = (volatile unsigned int*) 0x40000C0;
volatile unsigned int* dma1_control = (volatile unsigned int*) 0x40000C4;

volatile unsigned int* dma2_source = (volatile unsigned int*) 0x40000C8;
volatile unsigned int* dma2_destination = (volatile unsigned int*) 0x40000CC;
volatile unsigned int* dma2_control = (volatile unsigned int*) 0x40000D0;

volatile unsigned int* dma3_source = (volatile unsigned int*) 0x40000D4;
volatile unsigned int* dma3_destination = (volatile unsigned int*) 0x40000D8;
volatile unsigned int* dma3_control = (volatile unsigned int*) 0x40000DC;

/* copy data using DMA channel 3 (normal memory transfers) */
void memcpy16_dma(unsigned short* dest, unsigned short* source, int amount) {
    *dma3_source = (unsigned int) source;
    *dma3_destination = (unsigned int) dest;
    *dma3_control = DMA_ENABLE | DMA_16 | amount;
}

/* this function checks whether a particular button has been pressed */
unsigned char button_pressed(unsigned short button) {
    /* and the button register with the button constant we want */
    unsigned short pressed = *buttons & button;

    /* if this value is zero, then it's not pressed */
    if (pressed == 0) {
        return 1;
    } else {
        return 0;
    }
}

/* the global interrupt enable register */
volatile unsigned short* interrupt_enable = (unsigned short*) 0x4000208;

/* this register stores the individual interrupts we want */
volatile unsigned short* interrupt_selection = (unsigned short*) 0x4000200;

/* this registers stores which interrupts if any occured */
volatile unsigned short* interrupt_state = (unsigned short*) 0x4000202;

/* the address of the function to call when an interrupt occurs */
volatile unsigned int* interrupt_callback = (unsigned int*) 0x3007FFC;

/* this register needs a bit set to tell the hardware to send the vblank interrupt */
volatile unsigned short* display_interrupts = (unsigned short*) 0x4000004;

/* the interrupts are identified by number, we only care about this one */
#define INTERRUPT_VBLANK 0x1

/* allows turning on and off sound for the GBA altogether */
volatile unsigned short* master_sound = (volatile unsigned short*) 0x4000084;
#define SOUND_MASTER_ENABLE 0x80

/* has various bits for controlling the direct sound channels */
volatile unsigned short* sound_control = (volatile unsigned short*) 0x4000082;

/* bit patterns for the sound control register */
#define SOUND_A_RIGHT_CHANNEL 0x100
#define SOUND_A_LEFT_CHANNEL 0x200
#define SOUND_A_FIFO_RESET 0x800
#define SOUND_B_RIGHT_CHANNEL 0x1000
#define SOUND_B_LEFT_CHANNEL 0x2000
#define SOUND_B_FIFO_RESET 0x8000

/* the location of where sound samples are placed for each channel */
volatile unsigned char* fifo_buffer_a  = (volatile unsigned char*) 0x40000A0;
volatile unsigned char* fifo_buffer_b  = (volatile unsigned char*) 0x40000A4;

/* global variables to keep track of how much longer the sounds are to play */
unsigned int channel_a_vblanks_remaining = 0;
unsigned int channel_a_total_vblanks = 0;
unsigned int channel_b_vblanks_remaining = 0;

/* play a sound with a number of samples, and sample rate on one channel 'A' or 'B' */
void play_sound(const signed char* sound, int total_samples, int sample_rate, char channel) {
    /* start by disabling the timer and dma controller (to reset a previous sound) */
    *timer0_control = 0;
    if (channel == 'A') {
        *dma1_control = 0;
    } else if (channel == 'B') {
        *dma2_control = 0; 
    }

    /* output to both sides and reset the FIFO */
    if (channel == 'A') {
        *sound_control |= SOUND_A_RIGHT_CHANNEL | SOUND_A_LEFT_CHANNEL | SOUND_A_FIFO_RESET;
    } else if (channel == 'B') {
        *sound_control |= SOUND_B_RIGHT_CHANNEL | SOUND_B_LEFT_CHANNEL | SOUND_B_FIFO_RESET;
    }

    /* enable all sound */
    *master_sound = SOUND_MASTER_ENABLE;

    /* set the dma channel to transfer from the sound array to the sound buffer */
    if (channel == 'A') {
        *dma1_source = (unsigned int) sound;
        *dma1_destination = (unsigned int) fifo_buffer_a;
        *dma1_control = DMA_DEST_FIXED | DMA_REPEAT | DMA_32 | DMA_SYNC_TO_TIMER | DMA_ENABLE;
    } else if (channel == 'B') {
        *dma2_source = (unsigned int) sound;
        *dma2_destination = (unsigned int) fifo_buffer_b;
        *dma2_control = DMA_DEST_FIXED | DMA_REPEAT | DMA_32 | DMA_SYNC_TO_TIMER | DMA_ENABLE;
    }

    /* set the timer so that it increments once each time a sample is due
     * we divide the clock (ticks/second) by the sample rate (samples/second)
     * to get the number of ticks/samples */
    unsigned short ticks_per_sample = CLOCK / sample_rate;

    /* the timers all count up to 65536 and overflow at that point, so we count up to that
     * now the timer will trigger each time we need a sample, and cause DMA to give it one! */
    *timer0_data = 65536 - ticks_per_sample;

    /* determine length of playback in vblanks
     * this is the total number of samples, times the number of clock ticks per sample,
     * divided by the number of machine cycles per vblank (a constant) */
    if (channel == 'A') {
        channel_a_vblanks_remaining = total_samples * ticks_per_sample * (1.0 / CYCLES_PER_BLANK);
        channel_a_total_vblanks = channel_a_vblanks_remaining;
    } else if (channel == 'B') {
        channel_b_vblanks_remaining = total_samples * ticks_per_sample * (1.0 / CYCLES_PER_BLANK);
    }

    /* enable the timer */
    *timer0_control = TIMER_ENABLE | TIMER_FREQ_1;
}

/*-----------AGMV FILE UTILITY FUNCTIONS----------------*/

static inline Bool IWRAM AGMV_IsCorrectFourCC(char fourcc[4], char f, char o, char u, char r){
	if(f != fourcc[0] || o != fourcc[1] || u != fourcc[2] || r != fourcc[3]){
		return FALSE;
	}
	else return TRUE;
}

static inline void IWRAM AGMV_FindNextFrameChunk(File* file){
	u32 pos;
	Bool isFrame;
	char fourcc[4];

	AGMV_ReadFourCC(file,fourcc);
	isFrame = AGMV_IsCorrectFourCC(fourcc,'A','G','F','C');

	if(isFrame){
		pos = tell(file);
		seek(file,pos-4,SEEK_SET);
		return;
	}

	while(!isFrame && !AGMV_EOF(file)){
		AGMV_ReadFourCC(file,fourcc);
		isFrame = AGMV_IsCorrectFourCC(fourcc,'A','G','F','C');
		
		if(!isFrame){
			pos = tell(file);
			seek(file,pos-3,SEEK_SET);
		}
	}

	pos = tell(file);
	seek(file,pos-4,SEEK_SET);
}

static inline void IWRAM AGMV_FindNextAudioChunk(File* file){
	u32 pos;
	Bool isAudio;
	char fourcc[4];

	AGMV_ReadFourCC(file,fourcc);
	isAudio = AGMV_IsCorrectFourCC(fourcc,'A','G','A','C');

	if(isAudio){
		pos = tell(file);
		seek(file,pos-4,SEEK_SET);
		return;
	}

	while(!isAudio && !AGMV_EOF(file)){
		AGMV_ReadFourCC(file,fourcc);
		isAudio = AGMV_IsCorrectFourCC(fourcc,'A','G','A','C');
		
		if(!isAudio){
			pos = tell(file);
			seek(file,pos-3,SEEK_SET);
		}
	}

	pos = tell(file);
	seek(file,pos-4,SEEK_SET);
}

void AGMV_SkipFrameChunk(File* file){
	u32 fourcc, frame_num, usize, csize;

	AGMV_FindNextFrameChunk(file);

	fourcc    = AGMV_ReadLong(file);
	frame_num = AGMV_ReadLong(file);
	usize     = AGMV_ReadLong(file);
	csize     = AGMV_ReadLong(file);

	seek(file,csize,SEEK_CUR);
}

void AGMV_SkipAudioChunk(File* file){
	u32 fourcc, size;

	AGMV_FindNextAudioChunk(file);

	fourcc = AGMV_ReadLong(file);
	size   = AGMV_ReadLong(file);

	seek(file,size,SEEK_CUR);
}

/*-------------AGMV ERROR CODES--------------*/

typedef enum Error{
	NO_ERR = 0x0,
	INVALID_HEADER_FORMATTING_ERR = 0x1,
	FILE_NOT_FOUND_ERR = 0x2,
	MEMORY_CORRUPTION_ERR = 0x3,
}Error;

/*-----------AGMV DATA STRUCTURES-----------*/

#define AGMV_MAX_CLR     65535
#define MAX_OFFSET_TABLE 40000
#define AGMV_FILL_FLAG    0x1f
#define AGMV_NORMAL_FLAG  0x2f
#define AGMV_COPY_FLAG    0x5E

typedef struct AGMV_MAIN_HEADER{
	char fourcc[4]; /* AGMV IN PLAIN ASCII */
	u32 num_of_frames;
	u32 width;
	u32 height;
	u8  fmt;
	u8  version;
	u32 frames_per_second;
	u32 total_audio_duration;
	u32 sample_rate;
	u32 audio_size;
	u16 num_of_channels;
	u16 palette0[256];
	u16 palette1[256];
}AGMV_MAIN_HEADER;

typedef struct AGMV_FRAME_CHUNK{
	char fourcc[4]; /* AGFC IN PLAIN ASCII */
	u32 frame_num;
	u32 uncompressed_size;
	u32 compressed_size;
}AGMV_FRAME_CHUNK;

typedef struct AGMV_AUDIO_CHUNK{
	char fourcc[4]; /* AGAC IN PLAIN ASCII */
	u32 size;
	u32 total_size;
	const s8* pcm;
	s8* sample;
	s8* empty;
	u32 point;
	Bool enable_audio;
}AGMV_AUDIO_CHUNK;

typedef struct AGMV_FRAME{
	u32 width;
	u32 height;
	u32 dwidth;
	u32 dheight;
	u16* img_data;
}AGMV_FRAME;

typedef struct AGMV_AUDIO_TRACK{
	u32 total_audio_duration;
	u32 start_point;
	u8* pcm;
}AGMV_AUDIO_TRACK;

typedef struct AGMV_BITSTREAM{
	u8* data;
	u32 len;
	u32 pos;
}AGMV_BITSTREAM;

typedef struct AGMV{
	AGMV_MAIN_HEADER header;
	AGMV_FRAME_CHUNK* frame_chunk;
	AGMV_AUDIO_CHUNK* audio_chunk;
	AGMV_BITSTREAM* bitstream;
	AGMV_FRAME* frame;
	AGMV_FRAME* iframe;
	AGMV_AUDIO_TRACK* audio_track;
	u32 frame_count;
	u32 frame_index;
	u32 offset_table[MAX_OFFSET_TABLE];
	Bool disable_all_audio;
}AGMV;

/*-----------------AGMV DECODING------------------*/

void AGMV_DisableAllAudio(AGMV* agmv){
	agmv->disable_all_audio = TRUE;
}

void AGMV_EnableAllAudio(AGMV* agmv){
	agmv->disable_all_audio = FALSE;
}

int AGMV_NextIFrame(int n, int frame_count){
	int nexti = n;
	while((nexti + frame_count) % 4 != 0){
		nexti++;
	}
	return nexti;
}

int AGMV_PrevIFrame(int n, int frame_count){
	int nexti = n;
	while((nexti - frame_count) % 4 != 0){
		nexti--;
	}
	return nexti;
}

int AGMV_SkipToNearestIFrame(int n){
	int nexti = n;
	while(nexti % 4 != 0){
		nexti++;
	}
	return nexti;
}

AGMV* AGMV_AllocResources(File* file, const u8* agmv_sound, u32 total_audio_size, u32 sample_audio_size){
	int i;
	
	AGMV* agmv = (AGMV*)malloc(sizeof(AGMV));
	agmv->frame_chunk = (AGMV_FRAME_CHUNK*)malloc(sizeof(AGMV_FRAME_CHUNK));
	agmv->frame = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->iframe = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->audio_chunk = (AGMV_AUDIO_CHUNK*)malloc(sizeof(AGMV_AUDIO_CHUNK));
	agmv->bitstream = (AGMV_BITSTREAM*)malloc(sizeof(AGMV_BITSTREAM));
	
	AGMV_DecodeHeader(file,agmv);
	
	agmv->frame_count = 0;
	agmv->frame->width = agmv->header.width;
	agmv->frame->height = agmv->header.height;
	agmv->frame->dwidth = agmv->header.width << 1;
	agmv->frame->dheight = agmv->header.height << 1;
	agmv->frame->img_data = (u16*)malloc(sizeof(u16)*agmv->frame->width*agmv->frame->height);
	agmv->iframe->width = agmv->header.width;
	agmv->iframe->height = agmv->header.height;
	agmv->iframe->dwidth = agmv->header.width << 1;
	agmv->iframe->dheight = agmv->header.height << 1;
	agmv->iframe->img_data = (u16*)malloc(sizeof(u16)*agmv->frame->width*agmv->frame->height);
	agmv->bitstream->pos = 0;
	agmv->bitstream->len = agmv->frame->width*agmv->frame->height*2;
	agmv->bitstream->data = (u8*)malloc(sizeof(u8)*agmv->bitstream->len);
	agmv->audio_chunk->size = sample_audio_size*2;
	agmv->audio_chunk->total_size = total_audio_size;
	agmv->audio_chunk->pcm = agmv_sound;
	agmv->audio_chunk->sample = (s8*)malloc(sizeof(s8)*sample_audio_size*2);
	agmv->audio_chunk->empty = (s8*)malloc(sizeof(s8)*sample_audio_size*2);
	memset(agmv->audio_chunk->empty,0,sample_audio_size*2);
	agmv->audio_chunk->enable_audio = TRUE;
	agmv->audio_chunk->point = 0;
	
	AGMV_EnableAllAudio(agmv);
	
	return agmv;
}

void DestroyAGMV(AGMV* agmv){
	int i;
	if(agmv != NULL){
		free(agmv->audio_chunk->sample);
		free(agmv->audio_chunk->empty);
		free(agmv->frame->img_data);
		free(agmv->frame);
		free(agmv->iframe->img_data);
		free(agmv->iframe);
		free(agmv->frame_chunk);	
		free(agmv->bitstream->data);
		free(agmv->bitstream);
		free(agmv->audio_chunk); 
		free(agmv);
	}
}

int AGMV_DecodeHeader(File* file, AGMV* agmv){
	
	int i;
	
	AGMV_ReadFourCC(file,agmv->header.fourcc);
	agmv->header.num_of_frames = AGMV_ReadLong(file);
	agmv->header.width = AGMV_ReadLong(file);
	agmv->header.height = AGMV_ReadLong(file);
	
	agmv->header.fmt = AGMV_ReadByte(file);
	agmv->header.version = AGMV_ReadByte(file);
	agmv->header.frames_per_second = AGMV_ReadLong(file);
	
	agmv->header.total_audio_duration = AGMV_ReadLong(file);
	agmv->header.sample_rate = AGMV_ReadLong(file);
	agmv->header.audio_size = AGMV_ReadLong(file);
	agmv->header.num_of_channels = AGMV_ReadShort(file);
	
	if(!AGMV_IsCorrectFourCC(agmv->header.fourcc,'A','G','M','V') || !(agmv->header.version == 1 || agmv->header.version == 2 || agmv->header.version == 3 || agmv->header.version == 4) || agmv->header.frames_per_second >= 200){
		return INVALID_HEADER_FORMATTING_ERR;
	}
	
	if(agmv->header.version == 1 || agmv->header.version == 3){	
		for(i = 0; i < 256; i++){
			u8 r = AGMV_ReadByte(file);
			u8 g = AGMV_ReadByte(file);
			u8 b = AGMV_ReadByte(file);
			
			r >>= 3;
			g >>= 3;
			b >>= 3;
			
			agmv->header.palette0[i] = b << 10 | g << 5 | r;
		}
		
		for(i = 0; i < 256; i++){
			u8 r = AGMV_ReadByte(file);
			u8 g = AGMV_ReadByte(file);
			u8 b = AGMV_ReadByte(file);
			
			r >>= 3;
			g >>= 3;
			b >>= 3;
			
			agmv->header.palette1[i] = b << 10 | g << 5 | r;
		}
	}
	else{
		for(i = 0; i < 256; i++){
			u8 r = AGMV_ReadByte(file);
			u8 g = AGMV_ReadByte(file);
			u8 b = AGMV_ReadByte(file);
			
			r >>= 3;
			g >>= 3;
			b >>= 3;
			
			agmv->header.palette0[i] = b << 10 | g << 5 | r;
		}
	}
	
	return NO_ERR;
}

int IWRAM AGMV_DecodeFrameChunk(File* file, AGMV* agmv){
	u32 i, pos, bitpos = 0, bpos = 0, size = agmv->header.width * agmv->header.height, usize, csize, width, height, bits, num_of_bits;
	u16 offset, palette0[256], palette1[256], color;
	u16* img_data, *iframe_data, *palette;
	u8 byte, len, org, index, fbit, bot, *bitstream_data, *dest, *src;
	Bool escape = FALSE;
	
	agmv->bitstream->pos = 0;
	
	seek(file,8,SEEK_CUR);
	
	agmv->frame_chunk->uncompressed_size = AGMV_ReadLong(file);
	agmv->frame_chunk->compressed_size = AGMV_ReadLong(file);
	
	usize = agmv->frame_chunk->uncompressed_size;
	csize = agmv->frame_chunk->compressed_size;
	
	width  = agmv->frame->width;
	height = agmv->frame->height;
		
	bitstream_data = agmv->bitstream->data;
	
	img_data = agmv->frame->img_data;
	iframe_data = agmv->iframe->img_data;
	
	num_of_bits = csize * 8;
		
	for(i = 0; i < 256; i++){
		palette0[i] = agmv->header.palette0[i];
		palette1[i] = agmv->header.palette1[i];
	}

	if(agmv->header.version == 3 || agmv->header.version == 4){	
		for(i = 0; i < csize; i += 4){
			
			offset = AGMV_ReadShort(file);
			len = AGMV_ReadByte(file);	
			byte = AGMV_ReadByte(file);
			
			pos = bpos;
			
			int i;
			for(i = 0; i < len; i++){
				bitstream_data[bpos++] = bitstream_data[pos - offset + i];
			}
			
			bitstream_data[bpos++] = byte;
		}
	}
	else{
		for(bits = 0; bits <= num_of_bits;){

			byte = AGMV_ReadBits(file,1); bits++;
			
			if(byte & 1){
				bitstream_data[bpos++] = AGMV_ReadBits(file,8);
				bits += 8;
			}
			else{
				offset = AGMV_ReadBits(file,16);
				len = AGMV_ReadBits(file,4);
				
				org = len;
				
				bits += 20;
				
				dest = &bitstream_data[bpos];
				src = dest - offset;

				do {
					*dest++ = *src++;
				}while(len--);
				
				bpos += org;
			}
		}
	}
	
	AGMV_FlushReadBits();
	
	agmv->bitstream->pos = bpos;
	
	if(agmv->header.version == 1 || agmv->header.version == 3){
		int x,y;
		for(y = 0; y < height && escape != TRUE; y += 4){
			for(x = 0; x < width && escape != TRUE; x += 4){
				u8 byte = bitstream_data[bitpos++];
				
				if(bitpos >= bpos){
					escape = TRUE;
					break;
				}
				
				if(byte == AGMV_FILL_FLAG){
					index = bitstream_data[bitpos++];
					fbit = (index >> 7) & 1;
					bot = (index & 0x7f);
					
					if(bitpos >= bpos){
						escape = TRUE;
						break;
					}

					palette = fbit ? palette1 : palette0;
			
					if(bot < 127){
						color = palette[bot];
					}
					else{
						index = bitstream_data[bitpos++];
						color = palette[index];
					}
					
					int i,j;
					for(j = 0; j < 4; j++){
						u32 offset = (y+j)*width;
						for(i = 0; i < 4; i++){
							img_data[(x+i)+offset] = color;
						}
					}
				}
				else if(byte == AGMV_COPY_FLAG){
					if(bitpos >= bpos){
						escape = TRUE;
						break;
					}
					int i,j;
					for(j = 0; j < 4; j++){
						u32 offset = (y+j)*width;
						for(i = 0; i < 4; i++){
							u32 index = (x+i) + offset;
							img_data[index] = iframe_data[index];
						}
					}
				}
				else{
					int i,j;
					for(j = 0; j < 4; j++){
						u32 offset = (y+j) * width;
						for(i = 0; i < 4; i++){
							index = bitstream_data[bitpos++];
							fbit = (index >> 7) & 1;
							bot = (index & 0x7f);
							
							if(bitpos >= bpos){
								escape = TRUE;
								break;
							}
							palette = fbit ? palette1 : palette0;
							
							if(bot < 127){
								color = palette[bot];
							}
							else{
								index = bitstream_data[bitpos++];
								color = palette[index];
							}
							
							img_data[(x+i)+offset] = color;
						}
					}
				}
			}
		}
	}
	else{	
		int x,y;
		for(y = 0; y < height && escape != TRUE; y += 4){
			for(x = 0; x < width && escape != TRUE; x += 4){
				u8 byte = bitstream_data[bitpos++];
				
				if(bitpos >= bpos){
					escape = TRUE;
					break;
				}
				
				if(byte == AGMV_FILL_FLAG){
					index = bitstream_data[bitpos++];
					color = palette0[index];
					
					if(bitpos >= bpos){
						escape = TRUE;
						break;
					}
					
					int i,j;
					for(j = 0; j < 4; j++){
						u32 offset = (y+j)*width;
						for(i = 0; i < 4; i++){
							img_data[(x+i)+offset] = color;
						}
					}
				}
				else if(byte == AGMV_COPY_FLAG){
					int i,j;
					for(j = 0; j < 4; j++){
						u32 offset = (y+j)*width;
						for(i = 0; i < 4; i++){
							u32 index = (x+i)+offset;
							img_data[index] = iframe_data[index];
						}
					}
				}
				else{
					int i,j;
					for(j = 0; j < 4; j++){
						u32 offset = (y+j)*width;
						for(i = 0; i < 4; i++){
							u8 index = bitstream_data[bitpos++];
							if(bitpos >= bpos){
								escape = TRUE;
								break;
							}
							u16 color = palette0[index];
							img_data[(x+i)+offset] = color;
						}
					}
				}
			}
		}
	}
	
	if(agmv->frame_count % 4 == 0){
		for(i = 0; i < width*height; i++){
			iframe_data[i] = img_data[i];
		}
	}
	
	agmv->frame_count++;
	
	return NO_ERR;
}

int IWRAM AGMV_DecodeGBAAudioChunk(File* file, AGMV* agmv){
	int i;
	u8 sample;
	
	AGMV_ReadFourCC(file,agmv->audio_chunk->fourcc);
	agmv->audio_chunk->size = AGMV_ReadLong(file);
	
	if(!AGMV_IsCorrectFourCC(agmv->audio_chunk->fourcc,'A','G','A','C')){
		return INVALID_HEADER_FORMATTING_ERR;
	}

	for(i = 0; i < agmv->audio_chunk->size; i++){
		sample = AGMV_ReadByte(file);
		agmv->audio_chunk->sample[i] = sample; 
	}
	
	return NO_ERR;
}

#define MODE_3   0x3
#define MODE_5   0x5

void SetVideoMode(u8 mode){
	*(u16*)0x4000000 = 0x400 | mode; 
}

void EnableTimer2(){	
	*(u16*)0x400010A = 0x82;
	*(u16*)0x400010E = 0x84;
}

void AGMV_ResetVideo(File* file, AGMV* agmv){
	if(agmv->header.version == 1){
		seek(file,1572,SEEK_SET);
	}
	else{
		seek(file,804,SEEK_SET);
	}
	agmv->frame_count = 0;
	agmv->audio_chunk->point = 0;
}

Bool IWRAM AGMV_IsVideoDone(AGMV* agmv){
	if(agmv->frame_count >= agmv->header.num_of_frames){
		return TRUE;
	}
	else return FALSE;
}

void AGMV_SkipForwards(File* file, AGMV* agmv, int n){
	n = AGMV_NextIFrame(n,agmv->frame_count);
	
	int i;
	for(i = 0; i < n; i++){
		AGMV_FindNextFrameChunk(file);
		agmv->offset_table[agmv->frame_count++] = tell(file);
		AGMV_SkipFrameChunk(file);
	}	
	
	agmv->audio_chunk->point += n * agmv->audio_chunk->size/2;
	
	if(AGMV_IsVideoDone(agmv)){
		AGMV_ResetVideo(file,agmv);
	}
}

void AGMV_SkipBackwards(File* file, AGMV* agmv, int n){
	n = AGMV_PrevIFrame(n,agmv->frame_count);
	
	int frame_count = agmv->frame_count;
	frame_count -= n;
	
	if(frame_count < 0){
		agmv->frame_count = 0;
	}
	else{
		agmv->frame_count = frame_count;
	}
	
	seek(file,agmv->offset_table[agmv->frame_count],SEEK_SET);
	
	if(agmv->disable_all_audio != TRUE){
		int point = agmv->audio_chunk->point;
		point -= n * agmv->audio_chunk->size/2;
		if(point <= 0 || point >= agmv->audio_chunk->total_size/2){
			agmv->audio_chunk->point = 0;
		}
		else{
			agmv->audio_chunk->point = point;
		}
	}
}

/* ONLY CALL SKIP TO FUNCTION AFTER ALL FRAMES HAVE BEEN READ */

void AGMV_SkipTo(File* file, AGMV* agmv, int n){
	n = AGMV_SkipToNearestIFrame(n);
	if(n >= 0 && n < agmv->header.num_of_frames){
		seek(file,agmv->offset_table[n],SEEK_SET);
		agmv->frame_count = n;
		if(agmv->disable_all_audio != TRUE){
			agmv->audio_chunk->point = n * agmv->audio_chunk->size/2;
		}
	}
}

static inline void IWRAM AGMV_PlayAGMV(File* file, AGMV* agmv){
	AGMV_FindNextFrameChunk(file);
	agmv->offset_table[agmv->frame_count] = tell(file);
	AGMV_DecodeFrameChunk(file,agmv);
	
	if(agmv->disable_all_audio != TRUE){
		if(agmv->audio_chunk->enable_audio == TRUE){	
			s8* sample = agmv->audio_chunk->sample;
			s8* pcm = agmv->audio_chunk->pcm;
			u32 point = agmv->audio_chunk->point;
			u32 size = agmv->audio_chunk->size;
			int i;
			for(i = 0; i < size/2; i++){
				sample[i] = pcm[point++];
			}
			agmv->audio_chunk->point = point;
			play_sound(sample, size/2, 16000, 'A');	
		}
		else{
			s8* empty = agmv->audio_chunk->empty;
			u32 size = agmv->audio_chunk->size;
			play_sound(empty, size/2, 16000, 'A');
			agmv->audio_chunk->point += size/2;
		}
	}
}

void IWRAM AGMV_DisplayFrame(u16* vram, u16 width, u16 height, AGMV* agmv){
	u16 frame_width = agmv->frame->width;
	u16 frame_height = agmv->frame->height;
	u16 yoffset, y2w;
	
	u16* img_data = agmv->frame->img_data;
	u16* vrm = vram;
	
	int x,y;
	for(y = 0; y < frame_height; y++){
		yoffset = (frame_height-y) * width;
		y2w = y * frame_width;
		for(x = 0; x < frame_width; x++){
			vrm[x+yoffset] = img_data[x+y2w];
		}
	}
}

const u8 X2_TABLE[240] = {
	0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,
	11,12,12,13,13,14,14,15,15,16,16,17,17,18,18,19,19,
	20,20,21,21,22,22,23,23,24,24,25,25,26,26,27,27,28,
	28,29,29,30,30,31,31,32,32,33,33,34,34,35,35,36,36,
	37,37,38,38,39,39,40,40,41,41,42,42,43,43,44,44,45,
	45,46,46,47,47,48,48,49,49,50,50,51,51,52,52,53,53,
	54,54,55,55,56,56,57,57,58,58,59,59,60,60,61,61,62,
	62,63,63,64,64,65,65,66,66,67,67,68,68,69,69,70,70,
	71,71,72,72,73,73,74,74,75,75,76,76,77,77,78,78,79,
	79,80,80,81,81,82,82,83,83,84,84,85,85,86,86,87,87,
	88,88,89,89,90,90,91,91,92,92,93,93,94,94,95,95,96,
	96,97,97,98,98,99,99,100,100,101,101,102,102,103,103,
	104,104,105,105,106,106,107,107,108,108,109,109,110,
	110,111,111,112,112,113,113,114,114,115,115,116,116,
	117,117,118,118,119,119,
};

const u8 Y2_TABLE[160] = {
	0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15,16,16,17,17,18,18,
	19,19,20,20,21,21,22,22,23,23,24,24,25,25,26,26,27,27,28,28,29,29,30,30,31,31,32,32,33,33,34,
	34,35,35,36,36,37,37,38,38,39,39,40,40,41,41,42,42,43,43,44,44,45,45,46,46,47,47,48,48,49,49,
	50,50,51,51,52,52,53,53,54,54,55,55,56,56,57,57,58,58,59,59,60,60,61,61,62,62,63,63,64,64,65,
	65,66,66,67,67,68,68,69,69,70,70,71,71,72,72,73,73,74,74,75,75,76,76,77,77,78,78,79,
};

void IWRAM AGMV_DisplayScaledFrame(u16* vram, u16 width, u16 height, AGMV* agmv){
	u16 frame_width = agmv->frame->width;
	u16 frame_height = agmv->frame->height;
	u16 dwidth = agmv->frame->dwidth;
	u16 dheight = agmv->frame->dheight - 1;
	u16 x2, y2, yoffset, x2_0, x2_1,x2_2,x2_3, w = width, y2tw;
	
	u16* img_data = agmv->frame->img_data;
	u16* vrm = vram;
	
	u16 x,y;
	for(y = 0; y < dheight; y++){
		y2 = Y2_TABLE[y];
		yoffset = (dheight - y) * w;
		y2tw = y2 * frame_width;
		x = 0;
		for (; x < dwidth - 3; x += 4) {
			x2_0 = X2_TABLE[x];
			x2_1 = X2_TABLE[x + 1];
			x2_2 = X2_TABLE[x + 2];
			x2_3 = X2_TABLE[x + 3];
			
			vrm[x + yoffset] = img_data[x2_0 + y2tw];
			vrm[x + 1 + yoffset] = img_data[x2_1 + y2tw];
			vrm[x + 2 + yoffset] = img_data[x2_2 + y2tw];
			vrm[x + 3 + yoffset] = img_data[x2_3 + y2tw];
		}
		
		for (; x < dwidth; x++) {
			x2 = X2_TABLE[x];
			vrm[x + yoffset] = img_data[x2 + y2tw];
		}
	}
}

#endif