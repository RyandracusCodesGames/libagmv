#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <agidl.h>
#include <time.h>
#include <math.h>

int main(){

	float startTimeInterval = (float)clock() / CLOCKS_PER_SEC;

	/*PERFORM WORK*/

	float endTimeInterval = (float)clock() / CLOCKS_PER_SEC;
	
	printf("deltaTime = %.4f\n",endTimeInterval-startTimeInterval);
	
	return 0;
}