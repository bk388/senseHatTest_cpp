#ifndef LEDMATRIX_H
#define LEDMATRIX_H
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif //_GNU_SOURCE
#define DEV_FB "/dev"
#define FB_DEV_NAME "fb"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <string.h>
#endif //LEDMATRIX_H

extern int is_framebuffer_device(const struct dirent *dir);
extern int open_fbdev(const char *dev_name);
//...

struct fb_t {
	uint16_t pixel[8][8];
};

class LEDMatrix {
public:
	LEDMatrix();
	~LEDMatrix();
	void setScreen(uint16_t img[8][8]);
	void showImage(uint8_t img[8][8][3]);
	void setPixel(int xCoord, int yCoord, uint16_t value);
	void setRGBPixel(int xCoord, int yCoord, uint8_t red, uint8_t green, uint8_t blue);
private:
	struct fb_t *fb;
	int fbfd;
};

