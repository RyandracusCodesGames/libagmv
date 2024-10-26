/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_psx_decode.c
*   Date: 10/4/2024
*   Version: 2.0
*   Updated: 10/26/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agmv_psx_decode.h>

int AGMV_DecodeHeader(File* file, AGMV* agmv){
	
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
	
	if(!AGMV_IsCorrectFourCC(agmv->header.fourcc,'A','G','M','V') || !(v == 35 || v == 36 || v == 20|| v == 37 || v == 38 || v == 102) 
	|| !(agmv->header.bits_per_sample == 16 || agmv->header.bits_per_sample == 0)){
		agmv->error = INVALID_HEADER_FORMATTING_ERR;
		return INVALID_HEADER_FORMATTING_ERR;
	}
	
	if(v == 20 || v == 102){
		return NO_ERR;
	}
	
	if(v == 35 || v == 37){	
		for(i = 0; i < 256; i++){
			r = AGMV_ReadByte(file);
			g = AGMV_ReadByte(file);
			b = AGMV_ReadByte(file);
			
			r >>= 3;
			g >>= 3;
			b >>= 3;
			
			agmv->header.palette0[i] = 1 << 15 | b << 10 | g << 5 | r;
		}
		
		for(i = 0; i < 256; i++){
			r = AGMV_ReadByte(file);
			g = AGMV_ReadByte(file);
			b = AGMV_ReadByte(file);
			
			r >>= 3;
			g >>= 3;
			b >>= 3;
			
			agmv->header.palette1[i] = 1 << 15 | b << 10 | g << 5 | r;
		}
	}
	else{
		for(i = 0; i < 256; i++){
			r = AGMV_ReadByte(file);
			g = AGMV_ReadByte(file);
			b = AGMV_ReadByte(file);
			
			r >>= 3;
			g >>= 3;
			b >>= 3;
			
			agmv->header.palette0[i] = 1 << 15 | b << 10 | g << 5 | r;
		}
	}
	
	return NO_ERR;
}

int AGMV_DecodeFrameChunk(File* file, AGMV* agmv){
	u32 i, size = agmv->header.width * agmv->header.height, width, height, offset, usize, csize, yoffset, pframe_count;	
	u16* img_data = agmv->frame->img_data, *iframe_data = agmv->iframe->img_data, *pframe_data = agmv->prev_frame->img_data, color;
	u16 *palette0, *palette1, *img_ptr, *block_mem, *color_array, prev_color, vq2color1, vq2color2;
	u8 byte, bits, block_size, block_w, block_h, index, fbit, *bitstream_data = agmv->bitstream->data, version;
	s8 n;
	Bool v1, v4;
	RECT irect, prect, rect, block;
	int x, y;
	
	v1 = AGMV_FALSE;
	v4 = AGMV_FALSE;
	
	AGMV_SeekForwards(file,8);
	agmv->frame_chunk->frame_type = AGMV_ReadShort(file);
	agmv->frame_chunk->usize = AGMV_ReadLong(file);
	agmv->frame_chunk->csize = AGMV_ReadLong(file);
	
	color_array = (u16*)ScratchMem;
	
	width  = agmv->frame->width;
	height = agmv->frame->height;
	version = agmv->header.version;
	
	palette0 = agmv->header.palette0;
	palette1 = agmv->header.palette1;
	prev_color = agmv->prev_fill_color;
	vq2color1 = agmv->vq2color1;
	vq2color2 = agmv->vq2color2;
	
	irect = agmv->irect;
	prect = agmv->prect;
	rect  = agmv->rect;
		
	StoreImage2(&irect,(u_long*)iframe_data);
	StoreImage2(&prect,(u_long*)pframe_data);
	
	if(version == 35 || version == 36 || version == 20){
		AGMV_DecompressLZSS(agmv,file,bitstream_data,agmv->frame_chunk->usize,agmv->frame_chunk->csize);
	}
	else{
		AGMV_ReadNBytes(file,bitstream_data,agmv->frame_chunk->usize);
	}

	if(version == 35 || version == 37){
		v1 = AGMV_TRUE;
	}
	
	if(version == 20 || version == 102){
		v4 = AGMV_TRUE;
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
						
							color_array[i] = bits << 8 | byte;
							
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
						
						color = bits << 8 | byte;
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
								
								color_array[i] = bits << 8 | byte;
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
						
							color_array[i] = bits << 8 | byte;
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
		LoadImage2(&irect,(u_long*)img_data);
		
		if(agmv->flip != AGMV_TRUE){
			MoveImage2(&irect,agmv->rect.x,agmv->rect.y);
		}
	}
	else{
		LoadImage2(&prect,(u_long*)img_data);
		
		if(agmv->flip != AGMV_TRUE){
			MoveImage2(&prect,agmv->rect.x,agmv->rect.y);
		}
	}
	
	if(agmv->flip){
		offset  = 0;
		yoffset = (height - 1) * width;
		
		for(y = 0; y < height; y += 2){
			AGMV_Memcpy16(iframe_data+yoffset,img_data+offset,width);
			
			offset += width;
			yoffset -= width;
			
			AGMV_Memcpy16(iframe_data+yoffset,img_data+offset,width);
			
			offset += width;
			yoffset -= width;
		}
		
		LoadImage2(&rect,(u_long*)iframe_data);
	}
	
	agmv->frame_count++;
	agmv->prev_fill_color = prev_color;
	agmv->vq2color1 = vq2color1;
	agmv->vq2color2 = vq2color2;

	return NO_ERR;
}

int AGMV_DecodeAudioChunk(File* file, AGMV* agmv){
	int i;
	u32 size;
	u16 sample, resample, *pcm = agmv->audio_track->pcm, bits_per_sample = agmv->header.bits_per_sample;
	u8 *bitstream_data = agmv->bitstream->data;
	
	AGMV_ReadFourCC(file,agmv->audio_chunk->fourcc);
	agmv->audio_chunk->size = AGMV_ReadLong(file);
	agmv->audio_chunk->csize = AGMV_ReadLong(file);

	size   = agmv->audio_chunk->size;
	
	AGMV_DecompressLZSSAudio(agmv,file,bitstream_data,size,agmv->audio_chunk->csize);

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
	
	return NO_ERR;
}
