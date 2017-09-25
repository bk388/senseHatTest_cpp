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

struct fb_t {
	uint16_t pixel[8][8];
};

LEDMatrix::LEDMatrix() {
	return;
}

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

