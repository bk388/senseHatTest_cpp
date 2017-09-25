//============================================================================
// Name        : main.cpp
// Author      : Bence
// Version     :
// Copyright   : 
// Description : Main function for the project senseHatTest_cpp
//============================================================================

#include "RTIMULib.h"
#include "ledMatrix.h"

int main(int argc, char* args[]) {
	LEDMatrix ledMat;
	uint8_t image[8][8][3];
	for(int ii=0;ii<8;ii++) {
		image[ii][0][0] = 0xff;
		image[ii][0][1] = 0xff;
		image[ii][0][2] = 0xff;

		image[ii][7][0] = 0xff;
		image[ii][7][1] = 0xff;
		image[ii][7][2] = 0xff;

		image[0][ii][0] = 0xff;
		image[0][ii][1] = 0xff;
		image[0][ii][2] = 0xff;

		image[7][ii][0] = 0xff;
		image[7][ii][1] = 0xff;
		image[7][ii][2] = 0xff;
	}
	ledMat.showImage(image);
	while(1);
}
