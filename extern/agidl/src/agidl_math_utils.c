/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_math_utils.c
*   Date: 10/22/2023
*   Version: 0.1b
*   Updated: 4/4/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_math_utils.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <agidl_math_trig_tables.h>

int AGIDL_Min(const int a, const int b){
	if(a < b){
		return a;
	}
	return b;
}

int AGIDL_Max(const int a, const int b){
	if(a > b){
		return a;
	}
	return b;
}

int AGIDL_Abs(const int a){
	if(a < 0){
		return -a;
	}
	return a;
}

int AGIDL_Clamp(const int minVal, const int targetVal, const int maxVal){
	return AGIDL_Max(AGIDL_Min(maxVal,targetVal),minVal);
}

int AGIDL_Lerp(const int a, const int b, const float interp){
	return a + interp * (a - b);
}

float AGIDL_Minf(const float a, const float b){
	if(a < b){
		return a;
	}
	return b;
}

float AGIDL_Maxf(const float a, const float b){
	if(a > b){
		return a;
	}
	return b;
}

float AGIDL_Absf(const float a){
	if(a < 0){
		return -a;
	}
	return a;
}

float AGIDL_Clampf(const float minVal, const float targetVal, const float maxVal){
	return AGIDL_Maxf(AGIDL_Minf(maxVal,targetVal),minVal);
}

int AGIDL_Floor(const int a){
	return a - (a % 1);
}

float AGIDL_FModf(const float a, const float b){
	const int result = AGIDL_Floor( a / b);
	return a - result * b;
}

float AGIDL_FixAng(float ang){
	ang = AGIDL_FModf(ang,360);
	if(ang < 0){
		ang += 360;
	}
	return ang;
}

float AGIDL_Sinf(const float ang){
	return sin_lut[(int)AGIDL_FixAng(ang)];
}
float AGIDL_Cosf(const float ang){
	return cos_lut[(int)AGIDL_FixAng(ang)];
}

float AGIDL_Tanf(const float ang){
	return tan_lut[(int)AGIDL_FixAng(ang)];
}

u32 AGIDL_Pow2(const u32 num){
	return 1 << num;
}

char* AGIDL_Dec2Bin(const u32 num){
	u8 num_of_bits = 0;
	
	if(num <= 1){
		num_of_bits = 1;
	}
	else if(num <= 3){
		num_of_bits = 2;
	}
	else if(num <= 7){
		num_of_bits = 3;
	}
	else if(num <= 15){
		num_of_bits = 4;
	}
	else if(num <= 31){
		num_of_bits = 5;
	}
	else if(num <= 63){
		num_of_bits = 6;
	}
	else if(num <= 127){
		num_of_bits = 7;
	}
	else if(num <= 255){
		num_of_bits = 8;
	}
	else if(num <= 511){
		num_of_bits = 9;
	}
	else if(num <= 1023){
		num_of_bits = 10;
	}
	else if(num <= 2047){
		num_of_bits = 11;
	}
	else if(num <= 4095){
		num_of_bits = 12;
	}
	else if(num <= 8191){
		num_of_bits = 13;
	}
	else if(num <= 16383){
		num_of_bits = 14;
	}
	else if(num <= 32767){
		num_of_bits = 15;
	}
	else if(num <= 65535){
		num_of_bits = 16;
	}
	else if(num <= 131071){
		num_of_bits = 17;
	}
	
	char* bin = malloc((sizeof(char)*num_of_bits)+1);

	for(int i = num_of_bits - 1; i >= 0; i--){
		const int k = num >> i;
		if(k & 1){
			bin[(num_of_bits-1)-i] = '1';
		}
		else{
			bin[(num_of_bits-1)-i] = '0';
		}
	}
	
	bin[num_of_bits] = '\0';
	
	return bin;
}

void AGIDL_PrintBin(const u32 num){
	for(int i = 8; i >= 0; i--){
		const int k = num >> i;
		if(k & 1){
			printf("1");
		}
		else{
			printf("0");
		}
	}
	
	printf("\n");
}


u32 AGIDL_Bin2Dec(const char* bin){
	u32 accumulation = 0;
	for(int i = strlen(bin) - 1; i >= 0; i--){
		u8 binmul = 0;
		if(bin[i] == '0'){
			binmul = 0;
		}
		if(bin[i] == '1'){
			binmul = 1;
		}
		accumulation += binmul * AGIDL_Pow2(strlen(bin)-(i+1));
	}
	return accumulation;
}

u32 AGIDL_BitReverse(const char* bin){
	char* reverse = malloc(strlen(bin)+1);
	reverse[strlen(bin)] = '\0';

	for(int i = 0; i < strlen(bin); i++){
		reverse[i] = bin[strlen(bin)-(i+1)];
	}

	const u32 bitreverse = AGIDL_Bin2Dec(reverse);

	free(reverse);
	
	return bitreverse;
}

u32 AGIDL_BitXor(const u32 i, const u32 j){
	return i ^ j;
}

u32 AGIDL_BitInterleave(const u32 a, const u32 b){
	char* binA = AGIDL_Dec2Bin(a);
	char* binB = AGIDL_Dec2Bin(b);

	const int lenA = strlen(binA);
	const int lenB = strlen(binB);
	
	char* binC = malloc(lenA+lenB+1);

	for(int i = 0; i < lenA+lenB; i += 2){
		const int index = i / 2;
		
		binC[i] = binA[index];
		binC[i+1] = binB[index];
	}
	
	binC[lenA+lenB] = '\0';

	const u32 bitinterleave = AGIDL_Bin2Dec(binC);
	
	free(binA);
	free(binB);
	free(binC);
	
	return bitinterleave;
}

u32 AGIDL_NearestPow2(const int num){
	const int pow2[12] = {1,2,4,8,16,32,64,128,256,512,1024,2048};
	
	u8 min_index = 0;
	
	u32 min_diff = 40000;

	for(int i = 0; i < 12; i++){
		const int diff = AGIDL_Abs(pow2[i]-num);
		if(diff < min_diff){
			min_diff = diff;
			min_index = i;
		}
	}
	
	return pow2[min_index];
}

u8 AGIDL_GetPow2Index(const u32 num){
	const int pow2[12] = {1,2,4,8,16,32,64,128,256,512,1024,2048};

	for(int i = 0; i < 12; i++){
		if(num == pow2[i]){
			if(i > 1){
				return i - 1;
			}
			return i;
		}
	}
	
	return 0;
}

f32 AGIDL_Deg2Rad(const f32 degrees){
	return degrees * (AGIDL_PI/180.00f);
}

f32 AGIDL_Rad2Deg(const f32 rad){
	return rad * (180.00f/AGIDL_PI);
}
