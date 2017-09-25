#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif //_GNU_SOURCE
#define DEV_FB "/dev"
#define FB_DEV_NAME "fb"

#include "RTIMULib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <string.h>
#include "ledMatrix.h"

/* Constructor of LEDMatrix */
LEDMatrix::LEDMatrix() {
	LEDMatrix::fbfd = 0;
	LEDMatrix::fbfd = open_fbdev("RPi-Sense FB");
	if (LEDMatrix::fbfd <= 0) {
		printf("Error: cannot open framebuffer device.\n");
		return;
	}

	LEDMatrix::fb = (fb_t*)mmap(0, 128, PROT_READ | PROT_WRITE, MAP_SHARED, LEDMatrix::fbfd, 0);
	if (!LEDMatrix::fb) {
		printf("Failed to mmap.\n");
		close(LEDMatrix::fbfd);
		return;
	}
	memset(LEDMatrix::fb, 0, 128);
}

/* Destructor of LEDMatrix */
LEDMatrix::~LEDMatrix() {
	for(int ii=0;ii<8;ii++) {
		for(int jj=0;jj<8;jj++) {
			LEDMatrix::fb->pixel[ii][jj] = 0;
		}
	}
	close(LEDMatrix::fbfd);
}

/* Sets one pixel to a given 16bit colour */
void LEDMatrix::setPixel(int xCoord, int yCoord, uint16_t value) {
	LEDMatrix::fb->pixel[xCoord][yCoord] = value;
}

/* Sets one pixel to the 16bit colour that is closer to te specified 24bit RGB
 * The colours are represented on 16 bits: 5 for red, 6 for green, 5 for blue */
void LEDMatrix::setRGBPixel(int xCoord, int yCoord, uint8_t red, uint8_t green, uint8_t blue) {
	uint16_t red16bits, green16bits, blue16bits;
	/* Will not overflow, since an 8bit number multiplied by 31 is at most 13 bits long */
	red16bits = ((red*31)/255);		//31:max value of a 5bit number; 255:max value for an 8bit number; note that the multiplication is first
	green16bits = ((green*63)/255);	//63:max value of a 6bit number; 255:max value for an 8bit number; note that the multiplication is first
	blue16bits = ((blue*31)/255);	//31:max value of a 5bit number; 255:max value for an 8bit number; note that the multiplication is first
	LEDMatrix::fb->pixel[xCoord][yCoord] = ((red16bits<<11)|(green16bits<<5)|blue16bits);
}

/* Sets all the pixels using an 8x8 array of 16bit numbers */
void LEDMatrix::setScreen(uint16_t img[8][8]) {
	for(int ii=0;ii<8;ii++) {
		for(int jj=0;jj<8;jj++) {
			LEDMatrix::fb->pixel[ii][jj] = img[ii][jj];
		}
	}
}

/* Shows an RGB image with 24bit colour depth (first converts it into 16bit deep)
 * The input array should be an 8x8 array of 3 long uint8_t arrays, encoding the value of each pixel */
void LEDMatrix::showImage(uint8_t img[8][8][3]) {
	uint16_t red16bits, green16bits, blue16bits;
	for(int ii=0;ii<8;ii++) {
		for(int jj=0;jj<8;jj++) {
			/* Will not overflow, since an 8bit number multiplied by 31 is at most 13 bits long */
			red16bits = ((img[ii][jj][0]*31)/255);		//31:max value of a 5bit number; 255:max value for an 8bit number; note that the multiplication is first
			green16bits = ((img[ii][jj][1]*63)/255);	//63:max value of a 6bit number; 255:max value for an 8bit number; note that the multiplication is first
			blue16bits = ((img[ii][jj][2]*31)/255);		//31:max value of a 5bit number; 255:max value for an 8bit number; note that the multiplication is first
			LEDMatrix::fb->pixel[ii][jj] = ((red16bits<<11)|(green16bits<<5)|blue16bits);
		}
	}
}

/* The following two functons were copied from the example code snake.c that comes with RTIMULib: */

int is_framebuffer_device(const struct dirent *dir) {
	return strncmp(FB_DEV_NAME, dir->d_name, strlen(FB_DEV_NAME)-1) == 0;
}

int open_fbdev(const char *dev_name) {
	struct dirent **namelist;
	int i, ndev;
	int fd = -1;
	struct fb_fix_screeninfo fix_info;

	ndev = scandir(DEV_FB, &namelist, is_framebuffer_device, versionsort);
	if (ndev <= 0)
		return ndev;

	for(i = 0; i < ndev; i++) {
		char fname[64];
		char name[256];

		snprintf(fname, sizeof(fname),
			 "%s/%s", DEV_FB, namelist[i]->d_name);
		fd = open(fname, O_RDWR);
		if(fd < 0) {
			continue;
		}
		ioctl(fd, FBIOGET_FSCREENINFO, &fix_info);
		if(strcmp(dev_name, fix_info.id) == 0) {
			break;
		}
		close(fd);
		fd = -1;
	}
	for(i = 0; i < ndev; i++) {
		free(namelist[i]);
	}

	return fd;
}

