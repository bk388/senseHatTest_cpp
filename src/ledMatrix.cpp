#include "ledMatrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <time.h>
#include <poll.h>
#include <dirent.h>
#include <string.h>

#include <linux/input.h>
#include <linux/fb.h>

struct fb_t {
	uint16_t pixel[8][8];
};

LEDMatrix::LEDMatrix() {
	LEDMatrix::fbfd = 0;
	LEDMatrix::fbfd = open_fbdev("RPi-Sense FB");
	if (fbfd <= 0) {
		return;
	}

	LEDMatrix::fb = (fb_t*)mmap(0, 128, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if (!LEDMatrix::fb) {
		printf("Failed to mmap.\n");
		return;
	}
}

void LEDMatrix::setScreen(uint16_t img[8][8]) {
	for(int ii=0;ii<8;ii++) {
		for(int jj=0;jj<8;jj++) {
			LEDMatrix::fb->pixel[ii][jj] = img[ii][jj];
		}
	}
}

void LEDMatrix::showImage(uint16_t img[8][8][3]) {
	uint16_t img16bit[8][8];
	uint32_t red5bits, green6bits, blue5bits;
	for(int ii=0;ii<8;ii++) {
		for(int jj=0;jj<8;jj++) {
			red5bits = (((uint32_t)img[8][8][0] * 31) / 255) & 0x1f;
			green6bits = (((uint32_t)img[8][8][1] * 63) / 255) & 0x3f;
			blue5bits = (((uint32_t)img[8][8][2] * 31) / 255) & 0x1f;
			img16bit[ii][jj] = ((uint16_t)red5bits<<11)|((uint16_t)green6bits<<5)|((uint16_t)blue5bits);
		}
	}
	LEDMatrix::setScreen(img16bit);
}

/*void LEDMatrix::

void LEDMatrix::*/

void LEDMatrix::closeScreen() {
	close(LEDMatrix::fb);
}
