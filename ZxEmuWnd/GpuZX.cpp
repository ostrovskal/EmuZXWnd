
#include "stdafx.h"
#include "GpuZX.h"
#include "CpuZX.h"
#include "SettingsZX.h"
#include "zxKeyboard.h"

ssh_d colours[] = {	0xff000000, 0xff2030c0, 0xffc04010, 0xffc040c0, 0xff40b010, 0xff50c0b0, 0xffe0c010, 0xffc0c0c0,
					0xff000000, 0xff3040ff, 0xffff4030, 0xffff70f0, 0xff50e010, 0xff50e0ff, 0xffffe850, 0xffffffff};

ssh_d scan_offs[] = {
	0, 256, 512, 768, 1024, 1280, 1536, 1792,
	32, 288, 544, 800, 1056, 1312, 1568, 1824,
	64, 320, 576, 832, 1088, 1344, 1600, 1856,
	96, 352, 608, 864, 1120, 1376, 1632, 1888,
	128, 384, 640, 896, 1152, 1408, 1664, 1920,
	160, 416, 672, 928, 1184, 1440, 1696, 1952,
	192, 448, 704, 960, 1216, 1472, 1728, 1984,
	224, 480, 736, 992, 1248, 1504, 1760, 2016
};
GpuZX::GpuZX() {
	invert = 0;
	hbmpMemPrimary = nullptr;
	hdcMemPrimary = nullptr;
	hbmpMemBack = nullptr;
	hdcMemBack = nullptr;
	memoryPrimary = nullptr;
	memoryBack = nullptr;
}

GpuZX::~GpuZX() {
	::DeleteObject(hbmpMemPrimary);
	::DeleteObject(hdcMemPrimary);
	::DeleteObject(hbmpMemBack);
	::DeleteObject(hdcMemBack);
}

void GpuZX::makeCanvas() {
	BITMAPINFO bmi;

	memset(&bmi.bmiHeader, 0, sizeof(bmi.bmiHeader));
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = WIDTH_SCREEN + SIZE_BORDER * 2;
	bmi.bmiHeader.biHeight = HEIGHT_SCREEN + SIZE_BORDER * 2;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = USHORT(32);
	bmi.bmiHeader.biCompression = BI_RGB;

	hbmpMemPrimary = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&memoryPrimary, NULL, 0);
	hdcMemPrimary = ::CreateCompatibleDC(NULL);
	hbmpMemBack = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&memoryBack, NULL, 0);
	hdcMemBack = ::CreateCompatibleDC(NULL);
}

void GpuZX::showScreen() {
	HDC hdc;
	if((hdc = ::GetDC(theApp.getHWND()))) {
		HGDIOBJ h;
		HDC hMem;
		if(!(_TSTATE & ZX_BUFFER_GPU)) {
			hMem = hdcMemPrimary;
			h = SelectObject(hdcMemPrimary, hbmpMemPrimary);
		} else {
			hMem = hdcMemBack;
			h = SelectObject(hdcMemBack, hbmpMemBack);
		}
		LPRECT r = &theApp.wndRect;
		StretchBlt(hdc, r->left, r->top, r->right - r->left, r->bottom - r->top, hMem, 0, 0, WIDTH_SCREEN + SIZE_BORDER * 2, HEIGHT_SCREEN + SIZE_BORDER * 2, SRCCOPY);
		SelectObject(hMem, h);
		::DeleteObject(hdc);
		StringZX::fmt(L"SshZX (%s : %d) %s [%s]", ((_TSTATE & ZX_EXEC) ? L"execute" : L"pause"), (*_PC),
					  nameROMs[theApp.getOpt(OPT_MEM_MODEL)->dval], theApp.opts.nameLoadProg.str());
		SetWindowText(theApp.getHWND(), (LPCWSTR)tmpBuf);
	}
	invert++;
}

void GpuZX::execute() {

	if(!hbmpMemPrimary) makeCanvas();
	
	int y = *_SCAN;
	ssh_d* dest = memBuffer(true);
	dest += y * (WIDTH_SCREEN + SIZE_BORDER * 2);
	ssh_b col = (*_PORT_FE) & 7;
	ssh_d c = colours[col];

	for(int x = 0; x < (WIDTH_SCREEN + SIZE_BORDER * 2); x++) {
		if(y < SIZE_BORDER || y >= (HEIGHT_SCREEN + SIZE_BORDER)) *(dest + x) = c;
		else if(x < SIZE_BORDER || x >= (WIDTH_SCREEN + SIZE_BORDER)) *(dest + x) = c;
	}
	if(y >= SIZE_BORDER && y < (HEIGHT_SCREEN + SIZE_BORDER)) {
		// ���������� ���� ����� ������
		int yy = 191 - (y - SIZE_BORDER);
		int y_bank = (yy / 64) * 2048;
		int offs = scan_offs[yy & 63] + y_bank;

		dest += SIZE_BORDER;
		auto src_cols = ((yy >> 3) * 32) + (memScreen + 6144);
		auto src = memScreen + offs;

		for(int x = 0; x < 32; x++) {
			decodeColor(*(src_cols + x));
			drawLine(dest, *(src + x));
			dest += 8;
		}
	}
	y++;
	if(y >= (HEIGHT_SCREEN + SIZE_BORDER * 2)) y = 0;
	*_SCAN = y;
	if(!y) {
		auto filter = theApp.getOpt(OPT_PP)->dval;
		if(filter > 0) {
			dest = memBuffer(true);
			int x = 0, y = 1;
			while(y++ < ((HEIGHT_SCREEN - 1) + SIZE_BORDER * 2)) {
				while(x++ < ((WIDTH_SCREEN - 1) + SIZE_BORDER * 2)) {
					*dest++ = (filter == 1 ? asm_ssh_mixed(dest, dest + (WIDTH_SCREEN + SIZE_BORDER * 2)) :
							   asm_ssh_bilinear(x, y, (WIDTH_SCREEN + SIZE_BORDER * 2), dest));
				}
				x = 0;
			}
		}
		modifyTSTATE(_TSTATE ^ ZX_BUFFER_GPU, ZX_BUFFER_GPU);
	}
}

ssh_d* GpuZX::memBuffer(bool primary) {
	if(_TSTATE & ZX_BUFFER_GPU) {
		return primary ? memoryPrimary : memoryBack;
	} else {
		return primary ? memoryBack: memoryPrimary;
	}
}
void GpuZX::decodeColor(ssh_b color) {
	bool inv = ((color & 128) ? ((invert & 15) >> 3) == 0 : false);
	ssh_d ink1 = colours[(color & 7) | (color & 64) >> 3];
	ssh_d pap1 = colours[(color & 120) >> 3];
	ink = (inv ? pap1 : ink1);
	paper = (inv ? ink1 : pap1);
}

void GpuZX::drawLine(ssh_d* addr, ssh_b val) {
	for(int bit = 128; bit > 0; bit >>= 1) {
		*addr++ = ((val & bit) ? ink : paper);
	}
}

bool GpuZX::saveScreen(ssh_cws path) {
	bool result = false;

	theApp.pauseCPU(true);

	try {
		_wsopen_s(&_hf, path, _O_CREAT | _O_TRUNC | _O_WRONLY | _O_BINARY, _SH_DENYWR, _S_IWRITE);
		if(_hf != -1) {
			result = true;
			HEADER_TGA head;

			head.bIdLength = 0;
			head.bColorMapType = 0;
			head.bType = 2;

			head.wCmIndex = 0;
			head.wCmLength = 0;
			head.bCmEntrySize = 0;

			head.wXorg = 0;
			head.wYorg = 0;
			head.wWidth = 288;
			head.wHeight = 224;

			head.bBitesPerPixel = 32;
			head.bFlags = 0x28;

			if(_write(_hf, &head, sizeof(HEADER_TGA)) != sizeof(HEADER_TGA)) throw(0);
			ssh_d* ptr = (memoryPrimary + 288 * 223);
			for(int i = 0; i < 256; i++) {
				if(_write(_hf, ptr, 1152) != 1152) throw(0);
				ptr -= 288;
			}
		}
	} catch(...) { result = false; }

	SAFE_CLOSE1(_hf);

	theApp.pauseCPU(false);

	return result;
}
