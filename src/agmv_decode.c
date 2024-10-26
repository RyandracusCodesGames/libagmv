
/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_decode.c
*   Date: 5/17/2024
*   Version: 2.0
*   Updated: 10/26/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <agidl.h>
#include <stdlib.h>
#include <string.h>
#include <agmv_decode.h>
#include <agmv_utils.h>

int AGMV_DecodeHeader(FILE* file, AGMV* agmv){
	
	int i, v, r, g, b;
	
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
	agmv->header.bits_per_sample = AGMV_ReadShort(file);
	
	v = agmv->header.version;
	
	if(!AGMV_IsCorrectFourCC(agmv->header.fourcc,'A','G','M','V') || !(v == 1 || v == 2 || v == 3 || v == 4 || v == 5 || v == 6 
	|| v == 7 || v == 8 || v == 35 || v == 36 || v == 37 || v == 38 || v == 25 || v == 26 || v == 27 || v == 28
	|| v == 0 || v == 10 || v == 20 || v == 30 || v == 100 || v == 101 || v == 102 || v == 103) || !(agmv->header.bits_per_sample == 16 || agmv->header.bits_per_sample == 8 || agmv->header.bits_per_sample == 0)){
		return INVALID_HEADER_FORMATTING_ERR;
	}
	
	if(v == 0 || v == 10 || v == 20 || v == 30 || v == 100 || v == 101 || v == 102 || v == 103){
		return NO_ERR;
	}
	
	if(v == 1 || v == 3 || v == 5 || v == 7 || v == 35 || v == 37 || v == 25 || v == 27){	
		for(i = 0; i < 256; i++){
			r = AGMV_ReadByte(file);
			g = AGMV_ReadByte(file);
			b = AGMV_ReadByte(file);
			
			agmv->header.palette0[i] = AGIDL_RGB(r,g,b,agmv->header.fmt);
		}
		
		for(i = 0; i < 256; i++){
			r = AGMV_ReadByte(file);
			g = AGMV_ReadByte(file);
			b = AGMV_ReadByte(file);
			
			agmv->header.palette1[i] = AGIDL_RGB(r,g,b,agmv->header.fmt);
		}
	}
	else{
		for(i = 0; i < 256; i++){
			r = AGMV_ReadByte(file);
			g = AGMV_ReadByte(file);
			b = AGMV_ReadByte(file);
			
			agmv->header.palette0[i] = AGIDL_RGB(r,g,b,agmv->header.fmt);
		}
	}
	
	return NO_ERR;
}

void AGMV_DecompressLZSS(FILE* file, u8* bitstream_data, u32 usize){
	u32 bpos = 0;
	u8 byte = 0, flag_num = 0;
	u16 word, offset;
	u8 len, *src, *dest;
	int window_start;

	while(bpos < usize){
		if(flag_num == 0){
			byte = AGMV_ReadByte(file);
			flag_num = 8;
		}

		if(byte & 0x80){
			bitstream_data[bpos++] = AGMV_ReadByte(file);
		}
		else{
			word = AGMV_ReadShort(file);
			
			offset = word >> 4;
			len = word & 0xF;

			window_start = bpos - offset;
			
			dest = &bitstream_data[bpos];
			src = &bitstream_data[window_start];
			
			AGMV_Memcpy8(dest,src,len);

			bpos += len;
		}

		byte <<= 1;
		flag_num--;
	}
}

void AGMV_BlockCopy(u32* dest, u32 *src, u32 doffset, u32 soffset, u8 block_w, u8 block_h, u32 width){
	u32* dest_ptr, *src_ptr, i;
	
	dest_ptr = dest + doffset;
	src_ptr  = src  + soffset;
	
	for(i = 0; i < block_h; i += 4){
		AGMV_Memcpy32(dest_ptr,src_ptr,block_w);
		dest_ptr += width;   src_ptr += width;
		AGMV_Memcpy32(dest_ptr,src_ptr,block_w);
		dest_ptr += width;   src_ptr += width;
		AGMV_Memcpy32(dest_ptr,src_ptr,block_w);
		dest_ptr += width;   src_ptr += width;
		AGMV_Memcpy32(dest_ptr,src_ptr,block_w);
		dest_ptr += width;   src_ptr += width;
	}
}

void AGMV_SBlockCopy(u32* dest, u32 *src, u32 doffset, u32 soffset, u8 block_w, u8 block_h, u32 width){
	u32* dest_ptr, *src_ptr, i;
	
	dest_ptr = dest + doffset;
	src_ptr  = src  + soffset;
	
	for(i = 0; i < block_h; i += 2){
		AGMV_Memcpy32(dest_ptr,src_ptr,block_w);
		dest_ptr += width;   src_ptr += width;
		AGMV_Memcpy32(dest_ptr,src_ptr,block_w);
		dest_ptr += width;   src_ptr += width;
	}
}

void AGMV_BlockFill(u32* dest, u32 color, u8 block_w, u8 block_h, u32 width){
	while(block_h--){
		AGMV_Memset32(dest,color,block_w);
		dest += width;
	}
}

int AGMV_DecodeFrameChunk(FILE* file, AGMV* agmv){
	u32 i, bits, size = agmv->header.width * agmv->header.height, width, height, usize, csize, yoffset;	
	u32* img_data = agmv->frame->img_data, *iframe_data = agmv->iframe->img_data, *pframe_data = agmv->prev_frame->img_data, color, offset;
	u32 *palette0, *palette1, *img_ptr, color_array[8], pframe_count, vq2color1, vq2color2, prev_color;
	u16 color16;
	u8 byte, block_size, block_w, block_h, index, fbit, *bitstream_data = agmv->bitstream->data, version;
	u8 r, g, b;
	s8 n;
	Bool v1, v4;
	int x, y;

	v1 = FALSE;
	v4 = FALSE;
	
	AGMV_ReadFourCC(file,agmv->frame_chunk->fourcc);
	agmv->frame_chunk->frame_num = AGMV_ReadLong(file);
	agmv->frame_chunk->frame_type = AGMV_ReadShort(file);
	agmv->frame_chunk->uncompressed_size = AGMV_ReadLong(file);
	agmv->frame_chunk->compressed_size = AGMV_ReadLong(file);
	
	width  = agmv->frame->width;
	height = agmv->frame->height;
	version = agmv->header.version;
	
	usize = agmv->frame_chunk->uncompressed_size;
	csize = agmv->frame_chunk->compressed_size;
	
	palette0 = agmv->header.palette0;
	palette1 = agmv->header.palette1;
	
	vq2color1 = agmv->vq2color1;
	vq2color2 = agmv->vq2color2;
	prev_color = agmv->prev_fill_color;
	
	if(!AGMV_IsCorrectFourCC(agmv->frame_chunk->fourcc,'A','G','F','C')){
		return INVALID_HEADER_FORMATTING_ERR;
	}
	
	if(version == 1 || version == 2 || version == 5 || version == 6 || version == 25 || version == 26 || version == 0 || version == 30 || version == 10 || version == 35 || version == 36 || version == 20){
		AGMV_DecompressLZSS(file,bitstream_data,usize);
	}
	else{
		AGMV_ReadNBytes(file,bitstream_data,csize);
	}

	if(version == 1 || version == 3 || version == 5 || version == 7 || version == 35 || version == 37 || version == 25 || version == 27){
		v1 = TRUE;
	}
	
	if(version == 0 || version == 10 || version == 20 || version == 30 || version == 100 || version == 101 || version == 102 || version == 103){
		v4 = TRUE;
	}
	
	if(agmv->frame_chunk->frame_type == AGMV_IFRAME){
		agmv->pframe_count = 0;
	}
	else{
		agmv->pframe_count++;
	}

	pframe_count = agmv->pframe_count;

	for(y = 0; y < height; y += 4){
		yoffset = y * width;
		for(x = 0; x < width; x += 4){
			
			bits       = *(bitstream_data++);
			block_size = (bits >> 6) & 3;
			byte       = (bits & 0x3f);
			block_w    = AGMV_GetBlockWidth(block_size);
			block_h    = AGMV_GetBlockHeight(block_size);

			offset = x + yoffset;
			
			img_ptr = &img_data[offset];
			
			switch(byte){
				case AGMV_NORMAL_FLAG:{
					if(v1){
						for(i = 0; i < 5; i++){
							index = *(bitstream_data++);
							fbit = (index >> 7);
							bits = (index & 0x7f);
							
							if(bits >= 127) bits = *(bitstream_data++);
							color_array[i] = fbit ? palette1[bits] : palette0[bits];

							if(i <= 1){
								img_ptr[i] = color_array[i];
							}
						}
					}
					else if(v4){
						for(i = 0; i < 5; i++){
							bits = *(bitstream_data++);
							byte = *(bitstream_data++);
							
							color16 = bits << 8 | byte;
					
							b = (color16 >> 10) & 0x1f;
							g = (color16 >> 5) & 0x1f;
							r = color16  & 0x1f;
							
							b <<= 3;
							g <<= 3;
							r <<= 3;
							
							color_array[i] = r << 16 | g << 8 | b;
							
							if(i <= 1){
								img_ptr[i] = color_array[i];
							}
						}
					}
					else{
						for(i = 0; i < 5; i++){
							color_array[i] = palette0[*(bitstream_data++)];
							
							if(i <= 1){
								img_ptr[i] = color_array[i];
							}
						}
					}
					
					img_ptr += width;

					img_ptr[0] = AGMV_InterpColor(color_array[0],color_array[2]);
					img_ptr[1] = AGMV_InterpColor(color_array[1],color_array[3]);
					
					img_ptr += width;
					
					img_ptr[0] = color_array[2];
					img_ptr[1] = color_array[3];
					
					img_ptr += width;
					
					img_ptr[0] = img_ptr[1] = color_array[4];
				
					x -= 2;
				}break;
				case AGMV_PCOPY_FLAG:{
					AGMV_BlockCopy(img_data,pframe_data,offset,offset,block_w,block_h,width);
					
					x += (block_size == 1 || block_size == 3) ? 4 : (block_size == 2) ? -2 : 0;
				}break;
				case AGMV_COPY_FLAG:{

					if(pframe_count > 1)
						AGMV_BlockCopy(img_data,iframe_data,offset,offset,block_w,block_h,width);

					x += (block_size == 1 || block_size == 3) ? 4 : (block_size == 2) ? -2 : 0;
				}break;
				case AGMV_PMV_FLAG:
				case AGMV_MV_FLAG:{
					u32 offset2;
					s8 mx, my;
					int mvx, mvy;
					
					bits = *(bitstream_data++);
					
					mx = ((bits >> 4) & 0x0f) - 8;
					my = (bits & 0x0f) - 8;
					
					mvx = mx + x;
					mvy = my + y;
					
					if(mvx + block_w >= width || mvy + block_h >= height){
						break;
					}

					offset2 = mvx + mvy * width;
					
					if(byte == AGMV_MV_FLAG){
						AGMV_BlockCopy(img_data,iframe_data,offset,offset2,block_w,block_h,width);
					}
					else{
						AGMV_BlockCopy(img_data,pframe_data,offset,offset2,block_w,block_h,width);
					}
					
					x += (block_size == 1 || block_size == 3) ? 4 : (block_size == 2) ? -2 : 0;
				}break;
				case AGMV_FILL_FLAG:{
					if(v4){
						bits = *(bitstream_data++);
						byte = *(bitstream_data++);
						
						color16 = bits << 8 | byte;
					
						b = (color16 >> 10) & 0x1f;
						g = (color16 >> 5) & 0x1f;
						r = color16  & 0x1f;
						
						b <<= 3;
						g <<= 3;
						r <<= 3;
						
						color = r << 16 | g << 8 | b;
					}
					else if(v1){
						index = *(bitstream_data++); 
						fbit = (index >> 7);
						bits = (index & 0x7f);
				
						if(bits >= 127) bits = *(bitstream_data++);
						color = fbit ? palette1[bits] : palette0[bits];
					}
					else{
						color = palette0[*(bitstream_data++)];
					}

					AGMV_BlockFill(img_ptr,color,block_w,block_h,width);
					
					prev_color = color;
					
					x += (block_size == 1 || block_size == 3) ? 4 : (block_size == 2) ? -2 : 0;
				}break;
				case AGMV_PFILL_FLAG:{

					AGMV_BlockFill(img_ptr,prev_color,block_w,block_h,width);
					
					x += (block_size == 1 || block_size == 3) ? 4 : (block_size == 2) ? -2 : 0;
				}break;
				case AGMV_SKIP_8:{
					x += 4;
				}break;
				case AGMV_ICOPY_FLAG:
				case AGMV_ICOPYR_FLAG:{
					u32 ix, iy, ioffset, sx, sy;
					
					bits = *(bitstream_data++);
					
					sx = (bits >> 4) & 15;
					sy = (bits & 15);
					
					if(byte == AGMV_ICOPY_FLAG){
						ix = x - sx; iy = y - sy;
					}
					else{
						ix = x + sx; iy = y - sy;
					}
					
					ioffset = ix + iy * width;
					
					AGMV_BlockCopy(img_data,img_data,offset,ioffset,block_w,block_h,width);
					
					x += (block_size == 1 || block_size == 3) ? 4 : (block_size == 2) ? -2 : 0;
				}break;
				case AGMV_SMV_FLAG:{
					u32 offset_arr[4], xoffset_arr[4], yoffset_arr[4], offset2;
					s8 mx, my;
					int mvx, mvy;
					
					if(block_size == AGMV_BLOCK_8x4){
						
						offset = x + yoffset;
						
						offset_arr[0] = offset;
						offset_arr[1] = offset + 4;
						
						xoffset_arr[0] = x; xoffset_arr[1] = x + 4;
						
						for(i = 0; i < 2; i++){
							
							byte = *(bitstream_data++);
							
							mx = (s8)(byte);
							
							byte = *(bitstream_data++);
							
							my = (s8)(byte);
							
							mvx = mx + xoffset_arr[i];
							mvy = my + y;
							
							if(mvx + 4 >= width || mvy + 4 >= height){
								break;
							}
							
							offset  = offset_arr[i];
							offset2 = mvx + mvy * width;
							
							if(pframe_count > 1){
								AGMV_BlockCopy(img_data,pframe_data,offset,offset2,4,4,width);
							}
							else{
								AGMV_BlockCopy(img_data,iframe_data,offset,offset2,4,4,width);
							}
						}
						
						x += 4;
					}
					else if(block_size == AGMV_BLOCK_4x4){
						
						offset_arr[0] = offset;
						offset_arr[1] = offset + 2;
						offset_arr[2] = offset + width + width;
						offset_arr[3] = offset_arr[2] + 2;
						
						xoffset_arr[0] = x = xoffset_arr[2] = x;
						xoffset_arr[1] = xoffset_arr[3] = x + 2;
						
						yoffset_arr[0] = yoffset_arr[1] = y;
						yoffset_arr[2] = yoffset_arr[3] = y + 2;
						
						for(i = 0; i < 4; i++){
							byte = *(bitstream_data++);
							
							mx = ((byte >> 4) & 0x0f) - 8;
							my = (byte & 0x0f) - 8;
							
							mvx = mx + xoffset_arr[i];
							mvy = my + yoffset_arr[i];
							
							if(mvx + 2 >= width || mvy + 2 >= height){
								break;
							}
							
							offset  = offset_arr[i];
							offset2 = mvx + mvy * width;
							
							if(pframe_count > 1){
								AGMV_BlockCopy(img_data,pframe_data,offset,offset2,2,2,width);
							}
							else{
								AGMV_BlockCopy(img_data,iframe_data,offset,offset2,2,2,width);
							}
						}
					}
					else{
						
						offset_arr[0] =  offset;
						offset_arr[1] =  offset + width + width;
						
						yoffset_arr[0] = y; yoffset_arr[1] = y + 2;
						
						for(i = 0; i < 2; i++){
							byte = *(bitstream_data++);
							
							mx = ((byte >> 4) & 0x0f) - 8;
							my = (byte & 0x0f) - 8;
							
							mvx = mx + x;
							mvy = my + yoffset_arr[i];

							if(mvx + 2 >= width || mvy + 2 >= height){
								break;
							}

							offset  = offset_arr[i];
							offset2 = mvx + mvy * width;
							
							if(pframe_count > 1){
								AGMV_BlockCopy(img_data,pframe_data,offset,offset2,2,2,width);
							}
							else{
								AGMV_BlockCopy(img_data,iframe_data,offset,offset2,2,2,width);
							}
						}
						
						x -= 2;
					}
				}break;
				case AGMV_VQ2_FLAG:
				case AGMV_PVQ2_FLAG:{
				
					if(byte == AGMV_VQ2_FLAG){
						if(v4){
							for(i = 0; i < 2; i++){
								bits = *(bitstream_data++);
								byte = *(bitstream_data++);
								
								color16 = bits << 8 | byte;
					
								b = (color16 >> 10) & 0x1f;
								g = (color16 >> 5) & 0x1f;
								r = color16  & 0x1f;
								
								b <<= 3;
								g <<= 3;
								r <<= 3;
								
								color_array[i] = r << 16 | g << 8 | b;
							}
						}
						else if(v1){
							for(i = 0; i < 2; i++){
								index = *(bitstream_data++);
								fbit = (index >> 7);
								bits = (index & 0x7f);
						
								if(bits >= 127) bits = *(bitstream_data++);
								color_array[i] = fbit ? palette1[bits] : palette0[bits];
							}
						}
						else{
							color_array[0] = palette0[*(bitstream_data++)];
							color_array[1] = palette0[*(bitstream_data++)];
						}
							
						vq2color1 = color_array[0];
						vq2color2 = color_array[1];
					}
					else{
						color_array[0] = vq2color1;
						color_array[1] = vq2color2;
					}
					
					if(block_size == AGMV_BLOCK_4x4){
						byte = *(bitstream_data++);
						
						*(img_ptr++) = color_array[byte >> 7];
						*(img_ptr++) = color_array[(byte >> 6) & 1];
						*(img_ptr++) = color_array[(byte >> 5) & 1];
						*(img_ptr++) = color_array[(byte >> 4) & 1];

						img_ptr += width - 4;

						*(img_ptr++) = color_array[(byte >> 3) & 1];
						*(img_ptr++) = color_array[(byte >> 2) & 1];
						*(img_ptr++) = color_array[(byte >> 1) & 1];
						*(img_ptr++) = color_array[byte & 1];

						img_ptr += width - 4;
						
						byte = *(bitstream_data++);
						
						*(img_ptr++) = color_array[byte >> 7];
						*(img_ptr++) = color_array[(byte >> 6) & 1];
						*(img_ptr++) = color_array[(byte >> 5) & 1];
						*(img_ptr++) = color_array[(byte >> 4) & 1];

						img_ptr += width - 4;

						*(img_ptr++) = color_array[(byte >> 3) & 1];
						*(img_ptr++) = color_array[(byte >> 2) & 1];
						*(img_ptr++) = color_array[(byte >> 1) & 1];
						*(img_ptr++) = color_array[byte & 1];
					}
					else if(block_size == AGMV_BLOCK_8x4 || block_size == AGMV_BLOCK_8x8){
						for(i = 0; i < block_h; i++){
							byte = *(bitstream_data++);
							
							for(n = 7; n >= 0; n--)
								*(img_ptr++) = color_array[(byte >> n) & 1];

							img_ptr += width - 8;
						}
						
						x += 4;
					}
					else{
						byte = *(bitstream_data++);
						
						*(img_ptr++) = color_array[byte >> 7];
						*(img_ptr++) = color_array[(byte >> 6) & 1];
						
						img_ptr += width - 2;
						
						*(img_ptr++) = color_array[(byte >> 5) & 1];
						*(img_ptr++) = color_array[(byte >> 4) & 1];

						img_ptr += width - 2;

						*(img_ptr++) = color_array[(byte >> 3) & 1];
						*(img_ptr++) = color_array[(byte >> 2) & 1];
						
						img_ptr += width - 2;
						
						*(img_ptr++) = color_array[(byte >> 1) & 1];
						*(img_ptr++) = color_array[byte & 1];

						x -= 2;
					}
				}break;
				case AGMV_VQ4_FLAG:{
					if(v4){
						for(i = 0; i < 4; i++){
							bits = *(bitstream_data++);
							byte = *(bitstream_data++);
						
							color16 = bits << 8 | byte;
					
							b = (color16 >> 10) & 0x1f;
							g = (color16 >> 5) & 0x1f;
							r = color16  & 0x1f;
							
							b <<= 3;
							g <<= 3;
							r <<= 3;
							
							color_array[i] = r << 16 | g << 8 | b;
						}
					}
					else if(v1){
						for(i = 0; i < 4; i++){
							index = *(bitstream_data++);
							fbit = (index >> 7);
							bits = (index & 0x7f);
							
							if(bits >= 127) bits = *(bitstream_data++);
							color_array[i] = fbit ? palette1[bits] : palette0[bits];
						}
					}
					else{
						for(i = 0; i < 4; i++){
							color_array[i] = palette0[*(bitstream_data++)];
						}
					}
					
					if(block_size == AGMV_BLOCK_4x4){
						for(i = 0; i < 4; i++){
							byte = *(bitstream_data++);
							*(img_ptr++) = color_array[(byte >> 6) & 3];
							*(img_ptr++) = color_array[(byte >> 4) & 3];
							*(img_ptr++) = color_array[(byte >> 2) & 3];
							*(img_ptr++) = color_array[byte & 3];
							img_ptr += width - 4;
						}
					}
					else{
						for(i = 0; i < block_h; i++){
							byte = *(bitstream_data++);
							
							*(img_ptr++) = color_array[(byte >> 6) & 3];
							*(img_ptr++) = color_array[(byte >> 4) & 3];
							*(img_ptr++) = color_array[(byte >> 2) & 3];
							*(img_ptr++) = color_array[byte & 3];
							
							byte = *(bitstream_data++);
							
							*(img_ptr++) = color_array[(byte >> 6) & 3];
							*(img_ptr++) = color_array[(byte >> 4) & 3];
							*(img_ptr++) = color_array[(byte >> 2) & 3];
							*(img_ptr++) = color_array[byte & 3];
							
							img_ptr += width - 8;
						}
						
						x += 4;
					}
				}break;
			}
		}
	}
	
	if(agmv->frame_chunk->frame_type == AGMV_IFRAME){
		AGMV_Memcpy32(iframe_data,img_data,size);
	}
	else{
		AGMV_Memcpy32(pframe_data,img_data,size);
	}
	
	agmv->vq2color1 = vq2color1;
	agmv->vq2color2 = vq2color2;
	agmv->prev_fill_color = prev_color;
	
	agmv->frame_count++;

	return NO_ERR;
}

int AGMV_DebugFrameChunk(FILE* file, AGMV* agmv){
	u32 i, bits, width, height, usize, csize, yoffset, offset;
	u32 fill_count, pfill_count, copy_count, pcopy_count, icopy_count, mv_count, pmv_count, smv_count;
	u32 vq2_count, vq4_count, norm_count;
	u8 byte, block_size, block_w, block_h, index, *bitstream_data = agmv->bitstream->data, version;
	Bool v1, v4;
	int x, y;

	v1 = FALSE;
	v4 = FALSE;
	
	AGMV_ReadFourCC(file,agmv->frame_chunk->fourcc);
	agmv->frame_chunk->frame_num = AGMV_ReadLong(file);
	agmv->frame_chunk->frame_type = AGMV_ReadShort(file);
	agmv->frame_chunk->uncompressed_size = AGMV_ReadLong(file);
	agmv->frame_chunk->compressed_size = AGMV_ReadLong(file);
	
	width  = agmv->header.width;
	height = agmv->header.height;
	version = agmv->header.version;
	
	usize = agmv->frame_chunk->uncompressed_size;
	csize = agmv->frame_chunk->compressed_size;
	
	fill_count = 0; pfill_count = 0; copy_count = 0; pcopy_count = 0; icopy_count = 0; mv_count = 0; pmv_count = 0;
	smv_count = 0; vq2_count = 0; vq4_count = 0; norm_count = 0;
	
	if(!AGMV_IsCorrectFourCC(agmv->frame_chunk->fourcc,'A','G','F','C')){
		return INVALID_HEADER_FORMATTING_ERR;
	}
	
	if(version == 1 || version == 2 || version == 5 || version == 6 || version == 25 || version == 26 || version == 0 || version == 30 || version == 10){
		AGMV_DecompressLZSS(file,bitstream_data,usize);
	}
	else{
		AGMV_ReadNBytes(file,bitstream_data,csize);
	}

	if(version == 1 || version == 3 || version == 5 || version == 7 || version == 35 || version == 37 || version == 25 || version == 27){
		v1 = TRUE;
	}
	
	if(version == 0 || version == 10 || version == 20 || version == 30 || version == 100 || version == 101 || version == 102 || version == 103){
		v4 = TRUE;
	}
	
	if(agmv->frame_chunk->frame_type == AGMV_IFRAME){
		agmv->pframe_count = 0;
	}
	else{
		agmv->pframe_count++;
	}

	for(y = 0; y < height; y += 4){
		yoffset = y * width;
		for(x = 0; x < width; x += 4){
			
			bits       = *(bitstream_data++);
			block_size = (bits >> 6) & 3;
			byte       = (bits & 0x3f);
			block_w    = AGMV_GetBlockWidth(block_size);
			block_h    = AGMV_GetBlockHeight(block_size);

			offset = x + yoffset;
			
			switch(byte){
				case AGMV_NORMAL_FLAG:{
					norm_count++;
					
					if(v1){
						for(i = 0; i < 5; i++){
							index = *(bitstream_data++);
							bits = (index & 0x7f);
							
							if(bits >= 127) (bitstream_data++);
						}
					}
					else if(v4){
						for(i = 0; i < 5; i++){
							bitstream_data++;
							bitstream_data++;
						}
					}
					else{
						for(i = 0; i < 5; i++){
							bitstream_data++;
						}
					}
				
					x -= 2;
				}break;
				case AGMV_PCOPY_FLAG:{
					pcopy_count++;
					x += (block_size == 1 || block_size == 3) ? 4 : (block_size == 2) ? -2 : 0;
				}break;
				case AGMV_COPY_FLAG:{
					copy_count++;
					x += (block_size == 1 || block_size == 3) ? 4 : (block_size == 2) ? -2 : 0;
				}break;
				case AGMV_PMV_FLAG:
				case AGMV_MV_FLAG:{
					s8 mx, my;
					int mvx, mvy;
					
					bits = *(bitstream_data++);
					
					mx = ((bits >> 4) & 0x0f) - 8;
					my = (bits & 0x0f) - 8;
					
					mvx = mx + x;
					mvy = my + y;
					
					if(mvx + block_w >= width || mvy + block_h >= height){
						break;
					}
					
					if(byte == AGMV_MV_FLAG){
						mv_count++;
					}
					else{
						pmv_count++;
					}
					
					x += (block_size == 1 || block_size == 3) ? 4 : (block_size == 2) ? -2 : 0;
				}break;
				case AGMV_FILL_FLAG:{
					fill_count++;
					
					if(v4){
						bitstream_data++;
						bitstream_data++;
					}
					else if(v1){
						index = *(bitstream_data++); 
						bits = (index & 0x7f);
				
						if(bits >= 127) bitstream_data++;
					}
					else{
						bitstream_data++;
					}
					x += (block_size == 1 || block_size == 3) ? 4 : (block_size == 2) ? -2 : 0;
				}break;
				case AGMV_PFILL_FLAG:{
					pfill_count++;
					x += (block_size == 1 || block_size == 3) ? 4 : (block_size == 2) ? -2 : 0;
				}break;
				case AGMV_SKIP_8:{
					x += 4;
				}break;
				case AGMV_ICOPY_FLAG:
				case AGMV_ICOPYR_FLAG:{
					icopy_count++;
					bitstream_data++;
					x += (block_size == 1 || block_size == 3) ? 4 : (block_size == 2) ? -2 : 0;
				}break;
				case AGMV_SMV_FLAG:{
					smv_count++;
					
					u32 offset_arr[4], xoffset_arr[4], yoffset_arr[4];
					s8 mx, my;
					int mvx, mvy;
					
					if(block_size == AGMV_BLOCK_8x4){
						
						offset = x + yoffset;
						
						offset_arr[0] = offset;
						offset_arr[1] = offset + 4;
						
						xoffset_arr[0] = x; xoffset_arr[1] = x + 4;
						
						for(i = 0; i < 2; i++){
							
							byte = *(bitstream_data++);
							
							mx = (s8)(byte);
							
							byte = *(bitstream_data++);
							
							my = (s8)(byte);
							
							mvx = mx + xoffset_arr[i];
							mvy = my + y;
							
							if(mvx + 4 >= width || mvy + 4 >= height){
								break;
							}
						}
						
						x += 4;
					}
					else if(block_size == AGMV_BLOCK_4x4){
						
						offset_arr[0] = offset;
						offset_arr[1] = offset + 2;
						offset_arr[2] = offset + width + width;
						offset_arr[3] = offset_arr[2] + 2;
						
						xoffset_arr[0] = x = xoffset_arr[2] = x;
						xoffset_arr[1] = xoffset_arr[3] = x + 2;
						
						yoffset_arr[0] = yoffset_arr[1] = y;
						yoffset_arr[2] = yoffset_arr[3] = y + 2;
						
						for(i = 0; i < 4; i++){
							byte = *(bitstream_data++);
							
							mx = ((byte >> 4) & 0x0f) - 8;
							my = (byte & 0x0f) - 8;
							
							mvx = mx + xoffset_arr[i];
							mvy = my + yoffset_arr[i];
							
							if(mvx + 2 >= width || mvy + 2 >= height){
								break;
							}
						}
					}
					else{
						
						offset_arr[0] =  offset;
						offset_arr[1] =  offset + width + width;
						
						yoffset_arr[0] = y; yoffset_arr[1] = y + 2;
						
						for(i = 0; i < 2; i++){
							byte = *(bitstream_data++);
							
							mx = ((byte >> 4) & 0x0f) - 8;
							my = (byte & 0x0f) - 8;
							
							mvx = mx + x;
							mvy = my + yoffset_arr[i];

							if(mvx + 2 >= width || mvy + 2 >= height){
								break;
							}
						}
						
						x -= 2;
					}
				}break;
				case AGMV_VQ2_FLAG:
				case AGMV_PVQ2_FLAG:{
					vq2_count++;
				
					if(byte == AGMV_VQ2_FLAG){
						if(v4){
							for(i = 0; i < 2; i++){
								bitstream_data++;
								bitstream_data++;
							}
						}
						else if(v1){
							for(i = 0; i < 2; i++){
								index = *(bitstream_data++);
								bits = (index & 0x7f);
						
								if(bits >= 127) bitstream_data++;
							}
						}
						else{
							bitstream_data++;
							bitstream_data++;
						}
					}
					
					if(block_size == AGMV_BLOCK_4x4){
						bitstream_data++;
						bitstream_data++;
					}
					else if(block_size == AGMV_BLOCK_8x4 || block_size == AGMV_BLOCK_8x8){
						for(i = 0; i < block_h; i++){
							bitstream_data++;
						}
						
						x += 4;
					}
					else{
						bitstream_data++;
						x -= 2;
					}
				}break;
				case AGMV_VQ4_FLAG:{
					vq4_count++;
					
					if(v4){
						for(i = 0; i < 4; i++){
							bitstream_data++;
							bitstream_data++;
						}
					}
					else if(v1){
						for(i = 0; i < 4; i++){
							index = *(bitstream_data++);
							bits = (index & 0x7f);
							
							if(bits >= 127) bitstream_data++;
						}
					}
					else{
						for(i = 0; i < 4; i++){
							bitstream_data++;
						}
					}
					
					if(block_size == AGMV_BLOCK_4x4){
						for(i = 0; i < 4; i++){
							bitstream_data++;
						}
					}
					else{
						for(i = 0; i < block_h; i++){
							bitstream_data++;
							bitstream_data++;
						}
						
						x += 4;
					}
				}break;
			}
		}
	}
	
	printf("Frame: %ld\n",agmv->frame_count+1);
	printf("Fill Count:   %ld\n",fill_count);
	printf("PFill Count:  %ld\n",pfill_count);
	printf("Copy Count:   %ld\n",copy_count);
	printf("PCopy Count:  %ld\n",pcopy_count);
	printf("ICopy Count:  %ld\n",icopy_count);
	printf("MV Count:     %ld\n",mv_count);
	printf("PMV Count:    %ld\n",pmv_count);
	printf("SMV Count:    %ld\n",smv_count);
	printf("VQ2 Count:    %ld\n",vq2_count);
	printf("VQ4 Count:    %ld\n",vq4_count);
	printf("Normal Count: %ld\n",norm_count);
	printf("Total Count:  %ld\n",fill_count+pfill_count+copy_count+pcopy_count+icopy_count+mv_count+pmv_count+smv_count+vq2_count+vq4_count+norm_count);
	printf("Frame Size:   %ld\n\n",agmv->frame_chunk->uncompressed_size);

	agmv->frame_count++;

	return NO_ERR;
}

int AGMV_DecodeAudioChunk(FILE* file, AGMV* agmv){
	int i;
	u32 size, start_point;
	u16 sample, resample, *pcm = agmv->audio_track->pcm, bits_per_sample = AGMV_GetBitsPerSample(agmv);
	u8* pcm8 = agmv->audio_track->pcm8, *bitstream_data = agmv->bitstream->data, previous_sample = 0, current_sample;
	s8 diff;
	
	AGMV_ReadFourCC(file,agmv->audio_chunk->fourcc);
	agmv->audio_chunk->size = AGMV_ReadLong(file);
	agmv->audio_chunk->csize = AGMV_ReadLong(file);
	
	if(!AGMV_IsCorrectFourCC(agmv->audio_chunk->fourcc,'A','G','A','C')){
		return INVALID_HEADER_FORMATTING_ERR;
	}
	
	size   = agmv->audio_chunk->size;
	start_point = agmv->audio_track->start_point;

	AGMV_DecompressLZSS(file,bitstream_data,size);

	if(bits_per_sample == 16){
		for(i = 0; i < size; i++){
			sample = bitstream_data[i];
			
			if(sample % 2 == 0){
				resample = sample * sample;
			}
			else{
				resample = sample << 8;
			}
			
			pcm[start_point++] = resample; 
		}
	}
	else{
		for(i = 0; i < size; i++){
			diff = (s8)bitstream_data[i];
			current_sample = previous_sample + diff;
			pcm8[start_point++] = current_sample << 1;
			previous_sample = current_sample;
		}
	}
	
	agmv->audio_track->start_point = start_point;
	
	return NO_ERR;
}

int AGMV_DecodeAudioSample(FILE* file, AGMV* agmv){
	int i;
	u32 size;
	u16 sample, resample, *pcm = agmv->audio_track->pcm, bits_per_sample = AGMV_GetBitsPerSample(agmv);
	u8* pcm8 = agmv->audio_track->pcm8, *bitstream_data = agmv->bitstream->data, previous_sample = 0, current_sample;
	s8 diff;
	
	AGMV_ReadFourCC(file,agmv->audio_chunk->fourcc);
	agmv->audio_chunk->size = AGMV_ReadLong(file);
	agmv->audio_chunk->csize = AGMV_ReadLong(file);
	
	if(!AGMV_IsCorrectFourCC(agmv->audio_chunk->fourcc,'A','G','A','C')){
		return INVALID_HEADER_FORMATTING_ERR;
	}
	
	size   = agmv->audio_chunk->size;
	
	AGMV_DecompressLZSS(file,bitstream_data,size);

	if(bits_per_sample == 16){
		for(i = 0; i < size; i++){
			sample = bitstream_data[i];
			
			if(sample % 2 == 0){
				resample = sample * sample;
			}
			else{
				resample = sample << 8;
			}
			
			pcm[i] = resample; 
		}
	}
	else{
		for(i = 0; i < size; i++){
			diff = (s8)bitstream_data[i];
			current_sample = previous_sample + diff;
			pcm8[i] = current_sample << 1;
			previous_sample = current_sample;
		}
	}
	
	return NO_ERR;
}

int AGMV_DecodeVideo(const char* filename, u8 img_type){
	
	int err, err1, i, num_of_frames;
	Bool has_audio = FALSE;
	
	AGMV* agmv = (AGMV*)malloc(sizeof(AGMV));
	agmv->frame_chunk = (AGMV_FRAME_CHUNK*)malloc(sizeof(AGMV_FRAME_CHUNK));
	agmv->audio_chunk = (AGMV_AUDIO_CHUNK*)malloc(sizeof(AGMV_AUDIO_CHUNK));
	agmv->bitstream = (AGMV_BITSTREAM*)malloc(sizeof(AGMV_BITSTREAM));
	agmv->bitstream->pos = 0;
	agmv->frame = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->iframe = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->prev_frame = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->audio_track = (AGMV_AUDIO_TRACK*)malloc(sizeof(AGMV_AUDIO_TRACK));
	
	agmv->iframe_entries = NULL;
	agmv->image_entries  = NULL;
	
	FILE* file = fopen(filename,"rb");
	
	if(file == NULL){

		free(agmv->frame_chunk);
		free(agmv->audio_chunk);
		free(agmv->bitstream);
		free(agmv->frame);
		free(agmv->iframe);
		free(agmv->prev_frame);
		free(agmv->audio_track);
		free(agmv);

		return FILE_NOT_FOUND_ERR;
	}
	
	err = AGMV_DecodeHeader(file,agmv);
	
	if(agmv->header.total_audio_duration != 0){
		has_audio = TRUE;
	}
	
	agmv->header.total_audio_duration = 0;
	
	agmv->frame->width = agmv->header.width;
	agmv->frame->height = agmv->header.height;
	agmv->frame->img_data = (u32*)malloc(sizeof(u32)*agmv->frame->width*agmv->frame->height);
	
	agmv->iframe->width = agmv->header.width;
	agmv->iframe->height = agmv->header.height;
	agmv->iframe->img_data = (u32*)malloc(sizeof(u32)*agmv->frame->width*agmv->frame->height);
	
	agmv->prev_frame->width = agmv->header.width;
	agmv->prev_frame->height = agmv->header.height;
	agmv->prev_frame->img_data = (u32*)malloc(sizeof(u32)*agmv->frame->width*agmv->frame->height);
	
	agmv->bitstream->len = agmv->header.width*agmv->header.height*2;
	agmv->bitstream->data = (u8*)malloc(sizeof(u8)*agmv->bitstream->len);
	
	num_of_frames = AGMV_GetNumberOfFrames(agmv);
	agmv->pframe_count = 0;
	
	if(err != NO_ERR){
		fclose(file);
		free(agmv->frame_chunk);
		free(agmv->audio_chunk);
		free(agmv->bitstream->data);
		free(agmv->bitstream);
		free(agmv->frame->img_data);
		free(agmv->frame);
		free(agmv->iframe->img_data);
		free(agmv->iframe);
		free(agmv->prev_frame->img_data);
		free(agmv->prev_frame);
		free(agmv->audio_track);
		free(agmv);
		return err;
	}
	else{
		for(i = 0; i < num_of_frames; i++){
			AGMV_FindNextFrameChunk(file);
			err1 = AGMV_DecodeFrameChunk(file,agmv);

			if(has_audio == TRUE){
				AGMV_FindNextAudioChunk(file);
				AGMV_SkipAudioChunk(file);
			}
				
			if(err1 != NO_ERR){
				fclose(file);
				free(agmv->frame_chunk);
				free(agmv->audio_chunk);
				free(agmv->bitstream->data);
				free(agmv->bitstream);
				free(agmv->frame->img_data);
				free(agmv->frame);
				free(agmv->iframe->img_data);
				free(agmv->iframe);
				free(agmv->prev_frame->img_data);
				free(agmv->prev_frame);
				free(agmv->audio_track);
				free(agmv);
				return err1;
			}
			
			AGIDL_QuickExport(agmv->frame->img_data,agmv->frame->width,agmv->frame->height,AGIDL_RGB_888,img_type);
		}
	}
	
	fclose(file);
	free(agmv->frame_chunk);
	free(agmv->audio_chunk);
	free(agmv->bitstream->data);
	free(agmv->bitstream);
	free(agmv->frame->img_data);
	free(agmv->iframe->img_data);
	free(agmv->prev_frame->img_data);
	free(agmv->frame);
	free(agmv->iframe);
	free(agmv->prev_frame);
	free(agmv->audio_track);
	free(agmv);
	
	return NO_ERR;
}

int AGMV_DebugVideo(const char* filename, u32 start_frame, u32 end_frame){
	
	int err, err1, i, num_of_frames;
	Bool has_audio = FALSE;
	
	AGMV* agmv = (AGMV*)malloc(sizeof(AGMV));
	agmv->frame_chunk = (AGMV_FRAME_CHUNK*)malloc(sizeof(AGMV_FRAME_CHUNK));
	agmv->bitstream = (AGMV_BITSTREAM*)malloc(sizeof(AGMV_BITSTREAM));
	agmv->bitstream->pos = 0;

	FILE* file = fopen(filename,"rb");
	
	if(file == NULL){
		
		free(agmv->frame_chunk);
		free(agmv->bitstream);
		free(agmv);

		return FILE_NOT_FOUND_ERR;
	}
	
	err = AGMV_DecodeHeader(file,agmv);
	
	if(agmv->header.total_audio_duration != 0){
		has_audio = TRUE;
	}
	
	agmv->header.total_audio_duration = 0;
	agmv->bitstream->len = agmv->header.width*agmv->header.height*2;
	agmv->bitstream->data = (u8*)malloc(sizeof(u8)*agmv->bitstream->len);
	
	num_of_frames = AGMV_GetNumberOfFrames(agmv);
	agmv->pframe_count = 0;
	
	if(start_frame > num_of_frames){
		start_frame = num_of_frames;
	}
	
	if(end_frame > num_of_frames){
		end_frame = num_of_frames;
	}
	
	if(err != NO_ERR){
		fclose(file);
		free(agmv->frame_chunk);
		free(agmv->bitstream->data);
		free(agmv->bitstream);
		free(agmv);
		return err;
	}
	else{
		for(i = start_frame; i < end_frame; i++){
			AGMV_FindNextFrameChunk(file);
			err1 = AGMV_DebugFrameChunk(file,agmv);

			if(has_audio == TRUE){
				AGMV_FindNextAudioChunk(file);
				AGMV_SkipAudioChunk(file);
			}
				
			if(err1 != NO_ERR){
				fclose(file);
				free(agmv->frame_chunk);
				free(agmv->bitstream->data);
				free(agmv->bitstream);
				free(agmv);
				return err1;
			}
		}
	}
	
	fclose(file);
	free(agmv->frame_chunk);
	free(agmv->bitstream->data);
	free(agmv->bitstream);
	free(agmv);
	
	return NO_ERR;
}

int AGMV_DecodeAGMV(const char* filename, u8 img_type, AGMV_AUDIO_TYPE audio_type){
	FILE* file, *audio;
	int err, err1, err2, i, num_of_frames;
	u16 bits_per_sample;
	
	AGMV* agmv = (AGMV*)malloc(sizeof(AGMV));
	agmv->frame_chunk = (AGMV_FRAME_CHUNK*)malloc(sizeof(AGMV_FRAME_CHUNK));
	agmv->audio_chunk = (AGMV_AUDIO_CHUNK*)malloc(sizeof(AGMV_AUDIO_CHUNK));
	agmv->bitstream = (AGMV_BITSTREAM*)malloc(sizeof(AGMV_BITSTREAM));
	agmv->bitstream->pos = 0;
	agmv->frame = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->iframe = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->prev_frame = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->audio_track = (AGMV_AUDIO_TRACK*)malloc(sizeof(AGMV_AUDIO_TRACK));
	agmv->iframe_entries = NULL;
	agmv->image_entries = NULL;
	agmv->audio_track->pcm = NULL;
	agmv->audio_track->pcm8 = NULL;
	agmv->audio_chunk->atsample = NULL;
	
	file = fopen(filename,"rb");
	
	if(file == NULL){
		
		free(agmv->frame_chunk);
		free(agmv->audio_chunk);
		free(agmv->bitstream);
		free(agmv->frame);
		free(agmv->iframe);
		free(agmv->prev_frame);
		free(agmv->audio_track);
		free(agmv);

		return FILE_NOT_FOUND_ERR;
	}
	
	err = AGMV_DecodeHeader(file,agmv);
	
	agmv->frame->width = agmv->header.width;
	agmv->frame->height = agmv->header.height;
	agmv->frame->img_data = (u32*)malloc(sizeof(u32)*agmv->frame->width*agmv->frame->height);
	
	agmv->iframe->width = agmv->header.width;
	agmv->iframe->height = agmv->header.height;
	agmv->iframe->img_data = (u32*)malloc(sizeof(u32)*agmv->frame->width*agmv->frame->height);
	
	agmv->prev_frame->width = agmv->header.width;
	agmv->prev_frame->height = agmv->header.height;
	agmv->prev_frame->img_data = (u32*)malloc(sizeof(u32)*agmv->frame->width*agmv->frame->height);
	
	agmv->bitstream->len = agmv->header.width*agmv->header.height*2;
	agmv->bitstream->data = (u8*)malloc(sizeof(u8)*agmv->bitstream->len);
	
	num_of_frames = AGMV_GetNumberOfFrames(agmv);
	bits_per_sample = AGMV_GetBitsPerSample(agmv);
	agmv->pframe_count = 0;
	
	if(err != NO_ERR){
		fclose(file);
		DestroyAGMV(agmv);
		return err;
	}
	else{	
		if(agmv->header.total_audio_duration != 0){
			
			switch(audio_type){
				case AGMV_AUDIO_WAV:{
					audio = fopen("quick_export.wav","wb");
					AGMV_ExportAudioType(audio,agmv,AGMV_AUDIO_WAV);
				}break;
				case AGMV_AUDIO_AIFC:{
					audio = fopen("quick_export.aiff","wb");
					AGMV_ExportAudioType(audio,agmv,AGMV_AUDIO_AIFC);
				}break;
				case AGMV_AUDIO_AIFF:{
					audio = fopen("quick_export.aiff","wb");
					AGMV_ExportAudioType(audio,agmv,AGMV_AUDIO_AIFF);
				}break;
				default:{
					audio = fopen("quick_export.wav","wb");
					AGMV_ExportAudioType(audio,agmv,AGMV_AUDIO_WAV);	
				}break;
			}
			
			agmv->audio_chunk->size = agmv->header.audio_size / (f32)agmv->header.num_of_frames;
			
			if(bits_per_sample == 16){
				agmv->audio_track->pcm = (u16*)malloc(sizeof(u16)*agmv->audio_chunk->size);
			}
			else{
				agmv->audio_track->pcm8 = (u8*)malloc(sizeof(u8)*agmv->audio_chunk->size);
			}

			for(i = 0; i < num_of_frames; i++){
				AGMV_FindNextFrameChunk(file);
				err1 = AGMV_DecodeFrameChunk(file,agmv);
				AGMV_FindNextAudioChunk(file);
				err2 = AGMV_DecodeAudioSample(file,agmv);
				
				if(err1 != NO_ERR){
					fclose(file);
					fclose(audio);
					DestroyAGMV(agmv);
					return err1;
				}
				
				if(err2 != NO_ERR){
					fclose(audio);
					fclose(file);
					DestroyAGMV(agmv);
					return err1;
				}
				
				AGIDL_QuickExport(agmv->frame->img_data,agmv->frame->width,agmv->frame->height,AGIDL_RGB_888,img_type);
				AGMV_ExportAudio(audio,agmv,audio_type);
			}

			fclose(audio);
		}
		else{
			for(i = 0; i < num_of_frames; i++){
				AGMV_FindNextFrameChunk(file);
				err1 = AGMV_DecodeFrameChunk(file,agmv);
					
				if(err1 != NO_ERR){
					fclose(file);
					DestroyAGMV(agmv);
					return err1;
				}
				
				AGIDL_QuickExport(agmv->frame->img_data,agmv->frame->width,agmv->frame->height,AGIDL_RGB_888,img_type);
			}
		}
	}
	
	fclose(file);
	DestroyAGMV(agmv);
	
	return NO_ERR;
}

void to_80bitfloat(u32 num, u8 bytes[10])
{	u32 mask = 0x40000000 ;
	int	count ;

	if (num <= 1)
	{	bytes [0] = 0x3F ;
		bytes [1] = 0xFF ;
		bytes [2] = 0x80 ;
		return ;
		} ;

	bytes [0] = 0x40 ;

	if (num >= mask)
	{	bytes [1] = 0x1D ;
		return ;
		} ;

	for (count = 0 ; count < 32 ; count ++)
	{	if (num & mask)
			break ;
		mask >>= 1 ;
		} ;

	num = count < 31 ? num << (count + 1) : 0 ;
	bytes [1] = 29 - count ;
	bytes [2] = (num >> 24) & 0xFF ;
	bytes [3] = (num >> 4) & 0xFF ;
	bytes [4] = (num >> 8) & 0xFF ;
	bytes [5] = num & 0xFF ;

} 

int AGMV_DecodeAudio(const char* filename, AGMV_AUDIO_TYPE audio_type){
	FILE* file, *audio;
	int err, err1, i, num_of_frames;
	u16 bits_per_sample;
	
	AGMV* agmv = (AGMV*)malloc(sizeof(AGMV));
	agmv->frame_chunk = (AGMV_FRAME_CHUNK*)malloc(sizeof(AGMV_FRAME_CHUNK));
	agmv->audio_chunk = (AGMV_AUDIO_CHUNK*)malloc(sizeof(AGMV_AUDIO_CHUNK));
	agmv->bitstream = (AGMV_BITSTREAM*)malloc(sizeof(AGMV_BITSTREAM));
	agmv->frame = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->iframe = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->prev_frame = (AGMV_FRAME*)malloc(sizeof(AGMV_FRAME));
	agmv->audio_track = (AGMV_AUDIO_TRACK*)malloc(sizeof(AGMV_AUDIO_TRACK));
	agmv->iframe_entries = NULL;
	agmv->image_entries = NULL;
	agmv->audio_track->pcm = NULL;
	agmv->audio_track->pcm8 = NULL;
	agmv->audio_chunk->atsample = NULL;
	
	file = fopen(filename,"rb");
	
	if(file == NULL){

		free(agmv->frame_chunk);
		free(agmv->audio_chunk);
		free(agmv->bitstream);
		free(agmv->frame);
		free(agmv->iframe);
		free(agmv->prev_frame);
		free(agmv->audio_track);
		free(agmv);

		return FILE_NOT_FOUND_ERR;
	}
	
	err = AGMV_DecodeHeader(file,agmv);
	num_of_frames = AGMV_GetNumberOfFrames(agmv);
	bits_per_sample = AGMV_GetBitsPerSample(agmv);
	
	agmv->bitstream->len = (AGMV_GetAudioSize(agmv)/(f32)AGMV_GetNumberOfFrames(agmv))*2;
	agmv->bitstream->data = (u8*)malloc(sizeof(u8)*agmv->bitstream->len);
	
	if(err != NO_ERR){
		fclose(file);
		DestroyAGMV(agmv);
		return err;
	}
	else{
		if(agmv->header.total_audio_duration != 0){
			switch(audio_type){
				case AGMV_AUDIO_WAV:{
					audio = fopen("quick_export.wav","wb");
					AGMV_ExportAudioType(audio,agmv,AGMV_AUDIO_WAV);
				}break;
				case AGMV_AUDIO_AIFC:{
					audio = fopen("quick_export.aiff","wb");
					AGMV_ExportAudioType(audio,agmv,AGMV_AUDIO_AIFC);
				}break;
				case AGMV_AUDIO_AIFF:{
					audio = fopen("quick_export.aiff","wb");
					AGMV_ExportAudioType(audio,agmv,AGMV_AUDIO_AIFF);
				}break;
				default:{
					audio = fopen("quick_export.wav","wb");
					AGMV_ExportAudioType(audio,agmv,AGMV_AUDIO_WAV);	
				}break;
			}
			
			if(bits_per_sample == 16){
				agmv->audio_track->pcm = (u16*)malloc(sizeof(u16)*agmv->audio_chunk->size);
			}
			else{
				agmv->audio_track->pcm8 = (u8*)malloc(sizeof(u8)*agmv->audio_chunk->size);
			}
			
			for(i = 0; i < num_of_frames; i++){
				AGMV_FindNextFrameChunk(file);
				AGMV_SkipFrameChunk(file);
				AGMV_FindNextAudioChunk(file);
				err1 = AGMV_DecodeAudioSample(file,agmv);
				
				AGMV_ExportAudio(audio,agmv,audio_type);

				if(err1 != NO_ERR){
					fclose(audio);
					fclose(file);
					DestroyAGMV(agmv);
					return err1;
				}
			}
			
			fclose(audio);
		}
	}
	
	fclose(file);
	DestroyAGMV(agmv);
	
	return NO_ERR;
}
