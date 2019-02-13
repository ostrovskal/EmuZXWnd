
#pragma once

#define DIRECTINPUT_VERSION 0x0800
#pragma comment (lib, "comctl32.lib")

// мои типы данных
typedef long long						ssh_l;
typedef unsigned long long				ssh_u;
typedef unsigned char					ssh_b;
typedef unsigned short					ssh_w;
typedef unsigned long					ssh_d;
typedef unsigned int					ssh_i;
typedef const char*						ssh_ccs;
typedef const wchar_t*					ssh_cws;
typedef char							ssh_cs;
typedef char*							ssh_pcs;
typedef wchar_t							ssh_ws;
typedef wchar_t*						ssh_pws;

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#define _CRT_NON_CONFORMING_SWPRINTFS

#include <windows.h>
#include <WinUser.h>

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <timeapi.h>
#include <shlobj.h>
#include <sys/stat.h>
#include <commdlg.h>
#include "resource.h"
#include <dinput.h>
#include <dinputd.h>

#include "zxString.h"

#include "zxWnd.h"
#include "zxMsgMap.h"
#include "zxFrame.h"

enum class Radix { _dec, _bin, _oct, _hex, _dbl, _flt, _bool };

enum ZX_TSTATES {
	ZX_INT			= 1,
	ZX_DEBUG		= 4,
	ZX_EXEC			= 8,
	ZX_TERMINATE	= 16,
	ZX_WRITE_ROM	= 32,
	ZX_TRAP			= 64,
	ZX_RESET		= 128,
	ZX_NMI			= 256,
	ZX_BUFFER_GPU	= 512,
};

#pragma pack(push, 1)

struct HEADER_TGA {
	ssh_b	bIdLength;		// 
	ssh_b	bColorMapType;	// тип цветовой карты ()
	ssh_b	bType;			// тип файла ()
	ssh_w	wCmIndex;		// тип индексов в палитре
	ssh_w	wCmLength;		// длина палитры
	ssh_b	bCmEntrySize;	// число бит на элемент палитры
	ssh_w	wXorg;			// 
	ssh_w	wYorg;			// 
	ssh_w	wWidth;			// ширина
	ssh_w	wHeight;		// высота
	ssh_b	bBitesPerPixel;	// бит на пиксель
	ssh_b	bFlags;			// 
};

#pragma pack(pop)

struct ZX_BREAK_POINT {
	ZX_BREAK_POINT() : address1(0), address2(0), flags(0), value(0), mask(0) {}
	int address1;
	int address2;
	int value;
	int flags;
	ssh_b mask;
};

void* ssh_memset(void* ptr, int set, ssh_u count);
void* ssh_memzero(void* ptr, ssh_u count);
void* ssh_memcpy(void* dst, const void* src, ssh_u count);
ssh_u ssh_pow2(ssh_u val, bool nearest);

#define SAFE_RELEASE(p)		{ if(p) { (p)->Release(); (p)= nullptr; } }
#define SAFE_DELETE(ptr)	delete ptr; ptr = nullptr;
#define SAFE_CLOSE1(handle)	if(handle != -1) _close(handle);
#define SAFE_CLOSE2(handle)	if(handle) fclose(handle);
#define WM_IDLE				(WM_USER + 100)

#define FBP_PC			1
#define FBP_MEM			2

#define FBP_EQ			2	// 0 ==
#define FBP_NEQ			4	// 1 !=
#define FBP_LS			8	// 2 <
#define FBP_GT			12	// 3 >
#define FBP_QLS			16	// 4 <=
#define FBP_QGT			20	// 5 >=

#define FBP_ADDR		32
#define FBP_VAL			64
#define FBP_MASK		128

#define FBP_ACCESS		3
#define FBP_COND		30

#define COUNT_BP		10
#define COUNT_STORY_PC	64

extern "C" {
	void	asm_ssh_zx_init(void* regs, void* mem, void* port, void* checkBPS);
	void	asm_ssh_zx_step();
	ssh_b	asm_ssh_rotate(ssh_b v, ssh_u ops, ssh_b* fl);
	ssh_b	asm_ssh_accum(ssh_u op1, ssh_u op2, ssh_u ops, ssh_b* fl);
	ssh_w	asm_ssh_accum2(ssh_u op1, ssh_u op2, ssh_u ops, ssh_b* fl);
	ssh_d	asm_ssh_bilinear(ssh_d x, ssh_d y, ssh_d pitch, void* pix);
	ssh_d	asm_ssh_mixed(void* pix1, void* pix2);
	ssh_u	asm_ssh_capability();
	ssh_cws asm_ssh_to_base64(ssh_b* ptr, ssh_u count);
	ssh_b*	asm_ssh_from_base64(ssh_ws* str, ssh_u count, ssh_u* len_buf);
	void*	asm_ssh_wton(ssh_cws str, ssh_u radix, ssh_ws* end = nullptr);
	ssh_u	asm_ssh_length_last_number();
	ssh_cws asm_ssh_ntow(const void* ptr, ssh_u radix, ssh_ws* end = nullptr);
	ssh_ws* asm_ssh_parse_spec(void* arg, ssh_cws* str);
	ssh_l	asm_ssh_parse_xml(ssh_ws* src, ssh_w* vec);
	ssh_w	modifyTSTATE(int a, int r);
	void	writePort(ssh_b lport, ssh_b hport, ssh_b val);
	ssh_b	readPort(ssh_b lport, ssh_b hport);
}

ssh_cws fromNum(int num, ssh_cws fmt);
bool SshMsgPump(MSG* msg, bool isDlg);


extern zxFrame* theApp;
extern FILE* hf;
extern int _hf;

extern HMENU hMenu;
extern HINSTANCE hInst;

extern thread_local ssh_ws tmpBuf[MAX_PATH];
extern int lengthTmpBuf;
extern ssh_cws radix[];
extern ssh_cws cond_bp[];
extern ssh_cws nameROMs[];
