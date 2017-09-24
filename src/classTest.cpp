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

void LEDMatrix::close() {
	close(fbfd);
}
