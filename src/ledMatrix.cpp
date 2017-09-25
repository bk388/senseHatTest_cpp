#include "classTest.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <string.h>

struct fb_t {
	uint16_t pixel[8][8];
};

int LEDMatrix::LEDMatrix() {
	int ret = 0;

	fbfd = 0;
	fbfd = open_fbdev("RPi-Sense FB");
	if (fbfd <= 0) {
		ret = fbfd;
		goto label_end;
	}

	fb = (fb_t*)mmap(0, 128, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if (!fb) {
		ret = EXIT_FAILURE;
		printf("Failed to mmap.\n");
		goto err_fb;
	}

	err_fb:
		close(fbfd);
	label_end:
		return ret;
}

void LEDMatrix::setScreen(uint16_t img[8][8]) {
	for(int ii=0;ii<8;ii++) {
		for(int jj=0;jj<8;jj++) {
			fb->pixel[ii][jj] = img[ii][jj];
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
	setScreen(img16bit);
}

/*void LEDMatrix::

void LEDMatrix::*/

void LEDMatrix::close() {
	close(fbfd);
}
