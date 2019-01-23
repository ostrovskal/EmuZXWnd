
#include "stdafx.h"
#include "GpuZX.h"
#include "CpuZX.h"
#include "BorderZX.h"
#include "SoundZX.h"

ssh_d colours[] = {	0xff000000, 0xff0000c0, 0xffc00000, 0xffc000c0, 0xff00c000, 0xff00c0c0, 0xffc0c000, 0xffc0c0c0,
					0xff000000, 0xff0000e8, 0xffe80000, 0xffe800e8, 0xff00e800, 0xff00e8e8, 0xffe8e800, 0xffe8e8e8};

extern HWND hWnd;
extern RECT rectWnd;

GpuZX::GpuZX() {
	invert = 0;
	hbmpMem = nullptr;
	hdcMem = nullptr;
	memory = nullptr;
}

GpuZX::~GpuZX() {
	::DeleteObject(hbmpMem);
	::DeleteObject(hdcMem);
}

void GpuZX::makeCanvas() {
	BITMAPINFO bmi;

	memset(&bmi.bmiHeader, 0, sizeof(bmi.bmiHeader));
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = 320;
	bmi.bmiHeader.biHeight = 256;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = USHORT(32);
	bmi.bmiHeader.biCompression = BI_RGB;

	hbmpMem = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&memory, NULL, 0);
	hdcMem = ::CreateCompatibleDC(NULL);
}

wchar_t* str = new wchar_t[256];

void GpuZX::showScreen() {
	HDC hdc;
	if((hdc = ::GetDC(hWnd))) {
		HGDIOBJ h = SelectObject(hdcMem, hbmpMem);
		StretchBlt(hdc, 0, 0, rectWnd.right - rectWnd.left, rectWnd.bottom - rectWnd.top, hdcMem, 0, 0, 320, 256, SRCCOPY);
		SelectObject(hdcMem, h);
		::DeleteObject(hdc);
		wsprintf(str, L"PC: %d", CpuZX::PC);
		SetWindowText(hWnd, (LPCWSTR)str);
	}
}

void GpuZX::execute() {

	if(!hbmpMem) makeCanvas();
	
	int graph = 16384;
	int colors = 22528;
	
	ssh_d* dest = &memory[223 * 320 + 32];
	for(int bank = 0; bank < 3; bank++) {
		int graphN = graph;
		for(int znak = 0; znak < 8; znak++) {
			int graphZ = graphN;
			for(int line = 0; line < 8; line++) {
				ssh_d* graphics = dest;
				for(int x = 0; x < 32; x++) {
					decodeColor(CpuZX::memory[colors + x]);
					drawLine(graphics, CpuZX::memory[graphZ + x]);
					graphics += 8;
				}
				dest -= 320;
				graphZ += 256;
			}
			colors += 32;
			graphN += 32;
		}
		graph += 2048;
	}

	showScreen();

	invert++;

	CpuZX::trap = 1;
}

void GpuZX::decodeColor(ssh_b color) {
	bool inv = ((color & 128) ? ((invert & 15) >> 3) == 0 : false);
	ssh_b bright = ((bool)(color & 64) << 3);
	ssh_d ink1 = colours[(color & 7) + bright];
	ssh_d pap1 = colours[((color & 56) >> 3) + bright];
	ink = (inv ? pap1 : ink1);
	paper = (inv ? ink1 : pap1);
}

void GpuZX::write(ssh_b* address, ssh_b val) {
	return;
	ssh_w offs = (ssh_w)(address - &CpuZX::memory[16384]);
	int x, y;
	ssh_d* addr = &memory[32 * 320 + 32];
	if(offs >= 6144) {
		// ��������
		offs -= 6144;
		x = offs & 31;
		y = (offs / 32);
		decodeColor(val);
		ssh_b* screen = &CpuZX::memory[16384 + ((y / 8) * 2048 + ((y & 7) * 32)) + x];
		addr += ((24 - y) * 8 * 320 + x * 8 - 320);
		for(int n = 0; n < 8; n++) {
			drawLine(addr, *screen);
			screen += 256;
			addr -= 320;
		}
	} else {
		// �������
		int bank = (offs / 2048) * 64;
		offs &= 2047;

		int h = offs / 256;
		int l = offs & 255;

		x = offs & 31;
		y = 191 - (bank + (h + (l / 32) * 8));
		addr += (y * 320 + x * 8);
		decodeColor(CpuZX::memory[22528 + (y / 8) * 32 + x]);
		drawLine(addr, val);
	}
}

void GpuZX::drawLine(ssh_d* addr, ssh_b val) {
	for(int bit = 128; bit > 0; bit >>= 1) {
		*addr++ = ((val & bit) ? ink : paper);
	}
}

bool GpuZX::saveScreen(ssh_cws path) {
	int h = 0;
	bool result = true;
	bool exec = CpuZX::isExec;

	CpuZX::isExec = false;

	try {
		_wsopen_s(&h, path, _O_CREAT | _O_TRUNC | _O_WRONLY | _O_BINARY, _SH_DENYWR, _S_IWRITE);
		if(h) {
			HEADER_TGA head;

			head.bIdLength = 0;
			head.bColorMapType = 0;
			head.bType = 2;

			head.wCmIndex = 0;
			head.wCmLength = 0;
			head.bCmEntrySize = 0;

			head.wXorg = 0;
			head.wYorg = 0;
			head.wWidth = 320;
			head.wHeight = 256;

			head.bBitesPerPixel = 32;
			head.bFlags = 0x28;

			if(_write(h, &head, sizeof(HEADER_TGA)) != sizeof(HEADER_TGA)) throw(0);
			ssh_d* ptr = (memory + 320 * 255);
			for(int i = 0; i < 256; i++) {
				if(_write(h, ptr, 1280) != 1280) throw(0);
				ptr -= 320;
			}
		}
	} catch(...) {
		result = false;
	}
	if(h) _close(h);

	CpuZX::isExec = exec;

	return result;
}