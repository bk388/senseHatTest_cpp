//============================================================================
// Name        : main.cpp
// Author      : Bence
// Version     :
// Copyright   : 
// Description : Main function for the project senseHatTest_cpp
//============================================================================

#include "RTIMULib.h"
#include "ledMatrix.h"
#include <math.h>

#define EULER_NUM (float)2.71828
#define CONST_PI (float)3.141592654

LEDMatrix ledMat;

static void drawLine(float origin[2], double angle, float length, uint8_t colour[3], double smoothness);

int main(int argc, char* args[]) {
	float origin[2] = {3.5,3.5};
	uint8_t colour[3] = {255, 255, 255};
	while(1) {
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
		drawLine(origin, CONST_PI/4.0, 5, colour, 1000);
	}
}

static void drawLine(float origin[2], double angle, float length, uint8_t colour[3], double sharpness) {
	float distance;
	uint8_t pxColour[3];
	uint16_t red16bits, green16bits, blue16bits;
	float direction[2];
	float relPxPos[2];
	direction[0] = ((float)cos(angle));
	direction[1] = ((float)sin(angle));
	for(int ii=0;ii<8;ii++) {
		for(int jj=0;jj<8;jj++) {
			relPxPos[0] = (float)ii - origin[0];
			relPxPos[1] = (float)jj - origin[1];
			if(relPxPos[0]*direction[0] + relPxPos[1]*direction[1] <= 0) {
				/* The angle between the vector and the position vector of the pixel is more than 90° */
				distance = (float)sqrt(pow(relPxPos[0], 2) + pow(relPxPos[1], 2));
			} else if( (float)sqrt(pow(relPxPos[0], 2) + pow(relPxPos[1], 2)) > length ) {
				/* The pixel is further from the origin than the endpoint of the vector */
				distance = (float)sqrt(pow(relPxPos[0]-(direction[0]*length), 2) + pow(relPxPos[1]-(direction[1]*length), 2));
			} else {
				distance = abs( (relPxPos[0]*direction[1])-(relPxPos[1]*direction[0]) );
			}
			pxColour[0] = (uint8_t)(pow(EULER_NUM, (float)-1.0*((float)(distance*sharpness))) * colour[0]);
			pxColour[1] = (uint8_t)(pow(EULER_NUM, (float)-1.0*((float)(distance*sharpness))) * colour[1]);
			pxColour[2] = (uint8_t)(pow(EULER_NUM, (float)-1.0*((float)(distance*sharpness))) * colour[2]);
			ledMat.setRGBPixel(ii, jj, pxColour[0], pxColour[1], pxColour[2]);
		}
	}
}
