#pragma once

#define SIZE_BORDER			32
#define WIDTH_SCREEN		32 * 8
#define HEIGHT_SCREEN		192

class zxGPU {
public:
	zxGPU();
	virtual ~zxGPU();
	void execute();
	bool saveScreen(ssh_cws path);
	void showScreen();
	void updateData();
protected:
	void decodeColor(ssh_b color);
	void makeCanvas();
	ssh_d* memoryPrimary;
	ssh_d blink, blinkMsk, blinkShift;
	ssh_d ink;
	ssh_d paper;
	HBITMAP hbmpMemPrimary;
	HDC hdcMemPrimary;
	ssh_d colours[16];
};

