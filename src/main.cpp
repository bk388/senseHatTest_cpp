//============================================================================
// Name        : main.cpp
// Author      : Bence
// Version     :
// Copyright   : 
// Description : Main function for the project senseHatTest_cpp
//============================================================================

#include "RTIMULib.h"
#include "ledMatrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <math.h>

#define EULER_NUM (float)2.71828
#define CONST_PI (float)3.141592654

LEDMatrix ledMat;

static void drawLine(float origin[2], double angle, float length, uint8_t colour[3], double smoothness);
static float stringToNum(char* inptString);
static double doubleAbsVal(double num);

int main(int argc, char* args[]) {
	/*float origin[2];
	uint8_t colour[3] = {255, 255, 255};
	origin[0] = stringToNum(args[1]);
	origin[1] = stringToNum(args[2]);
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
		drawLine(origin, stringToNum(args[3])*CONST_PI/180.0, 5, colour, stringToNum(args[4]));
	}*/
	inquiry_info *ii = NULL;
	int max_rsp, num_rsp;
	int dev_id, sock, len, flags;
	int i;
	char addr[19] = { 0 };
	char name[248] = { 0 };

	struct sockaddr_l2 loc_addr = { 0 }, rem_addr = { 0 };
	char buf[1024] = { 0 };
	int s, client, bytes_read;
	int opt = sizeof(rem_addr);

	bool nameFound = true;
	struct bdaddr_t rem_bdaddr;

	dev_id = hci_get_route(NULL);
	sock = hci_open_dev( dev_id );
	if (dev_id < 0 || sock < 0) {
		perror("opening socket");
		exit(1);
	}
	len  = 8;
	max_rsp = 255;
	flags = IREQ_CACHE_FLUSH;
	ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

	num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
	if( num_rsp < 0 ) {
		perror("hci_inquiry");
	}

	for (i = 0; i < num_rsp; i++) {
		ba2str(&(ii+i)->bdaddr, addr);
		memset(name, 0, sizeof(name));
		if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), name, 0) < 0) {
			strcpy(name, "[unknown]");
		}
		if(argc>1) {
			nameFound = true;
			int jj = 0;
			while((uint8_t)args[1][jj] != (uint8_t)0) {
				if(jj>=248) {
					nameFound = false;
					break;
				}
				if(jj<247) {
					if((uint8_t)args[1][jj+1] == (uint8_t)0 && (uint8_t)name[jj+1] != (uint8_t)0) {
						nameFound = false;
						break;
					}
				}
				if(args[1][jj] != name[jj]) {
					nameFound = false;
					break;
				}
				jj ++;
			}
		}
		printf("%s  %s  %d\n", addr, name, nameFound);
		if(nameFound) {
			rem_bdaddr = &(ii+i)->bdaddr;
			break;
		}
	}
	ba2str(&rem_bdaddr, addr);
	printf("%s\n", addr)
	free( ii );
	close( sock );
	return 0;

}

static void drawLine(float origin[2], double angle, float length, uint8_t colour[3], double sharpness) {
	double distance;
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
				distance = sqrt(pow(relPxPos[0]-(direction[0]*length), 2) + pow(relPxPos[1]-(direction[1]*length), 2));
			} else {
				distance = doubleAbsVal( ((double)relPxPos[0]*(double)direction[1])-((double)relPxPos[1]*(double)direction[0]) );
			}
			pxColour[0] = (uint8_t)(pow(EULER_NUM, (float)-1.0*((float)(distance*sharpness))) * colour[0]);
			pxColour[1] = (uint8_t)(pow(EULER_NUM, (float)-1.0*((float)(distance*sharpness))) * colour[1]);
			pxColour[2] = (uint8_t)(pow(EULER_NUM, (float)-1.0*((float)(distance*sharpness))) * colour[2]);
			ledMat.setRGBPixel(ii, jj, pxColour[0], pxColour[1], pxColour[2]);
		}
	}
}

static float stringToNum(char* inptString) {
	float outptNum = 0;
	signed int dotPosition = -1;
	for(int ii=0;((uint8_t)inptString[ii])!=0;ii++){
		if( (((uint8_t)inptString[ii]) >= 48) &&
			(((uint8_t)inptString[ii]) <= 57)	) {
			if(dotPosition<0) {
				outptNum = outptNum*10 + ((uint8_t)inptString[ii]) - 48;
			} else {
				outptNum += ((uint8_t)inptString[ii] - 48)/(pow(10.0, ii-dotPosition));
			}
		} else if( ((uint8_t)inptString[ii]) == 46 ) {
			dotPosition = ii;
		}
	}
	return outptNum;
}

static double doubleAbsVal(double num) {
	if(num < 0) {
		return (double)-1.0*num;
	}
	return num;
}
