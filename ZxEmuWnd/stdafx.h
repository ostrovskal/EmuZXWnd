// stdafx.h: включаемый файл для стандартных системных включаемых файлов
// или включаемых файлов для конкретного проекта, которые часто используются, но
// не часто изменяются
//

#pragma once

#pragma comment (lib, "comctl32.lib")

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Исключите редко используемые компоненты из заголовков Windows
// Файлы заголовков Windows:
#include <windows.h>

// Файлы заголовков C RunTime
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

#include "StringZX.h"

enum class Radix { _dec, _bin, _oct, _hex, _dbl, _flt, _bool };

enum ZX_STATES {
	ZX_INT			= 1,
	ZX_BORDER		= 2,
	ZX_SOUND		= 4,
	ZX_EXEC			= 8,
	ZX_REGENERATE	= 16
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

void* ssh_memset(void* ptr, int set, ssh_u count);
void* ssh_memzero(void* ptr, ssh_u count);
void* ssh_memcpy(void* dst, const void* src, ssh_u count);
	
extern "C" {
	ssh_d	asm_ssh_bilinear(ssh_d x, ssh_d y, ssh_d pitch, void* pix);
	ssh_d	asm_ssh_mixed(void* pix1, void* pix2);
	ssh_u	asm_ssh_capability();
	ssh_cws asm_ssh_to_base64(ssh_b* ptr, ssh_u count);
	ssh_b*	asm_ssh_from_base64(ssh_ws* str, ssh_u count, ssh_u* len_buf);
	void*	asm_ssh_wton(ssh_cws str, ssh_u radix, ssh_ws* end = nullptr);
	ssh_cws asm_ssh_ntow(const void* ptr, ssh_u radix, ssh_ws* end = nullptr);
	ssh_ws* asm_ssh_parse_spec(void* arg, ssh_cws* str);
	ssh_l	asm_ssh_parse_xml(ssh_ws* src, ssh_w* vec);
}

enum CPU_REG {
	RC, // 0
	RB, // 1
	RE, // 2
	RD, // 3
	RL, // 4
	RH, // 5
	RA, // 6
	RF, // 7 (HL)
	RIXL, // 8
	RIXH, // 9
	RIYL, // 10
	RIYH, // 11
	RSPL, // 12
	RSPH, // 13
	RBC,// 14
	RDE,// 15
	RHL,// 16
	RAF,// 17
	RIX,// 18
	RIY,// 19
	RSP// 20
};

enum CPU_FLAGS {
	FC = 1,
	FN = 2,// SUB,DEC,CMP
	FPV =4,//P=1->четность бит,V=1->переполнение
	F3 = 8,// не используется
	FH = 16,// перенос между 3 и 4 битами
	F5 = 32,// не используется
	FZ = 64,
	FS = 128
};

// префиксы
#define PREFIX_NO		0//0
#define PREFIX_CB		1//203
#define PREFIX_ED		2//237
#define PREFIX_IX		1//221
#define PREFIX_IY		2//253

#define GET_FZ(val)				(val == 0) << 6
#define GET_FV(val1, val2)		calcFV(val1, val2)
#define GET_FP(val)				calcFP(val) << 2
#define GET_FN(val)				val << 1
#define GET_FC(val1, val2)		(val1 > val2)
#define DA_PUT(c)				path[pos++] = c

ssh_b calcFP(ssh_b val);
ssh_b calcFV(ssh_b val1, ssh_b val2);
ssh_b calcFH(ssh_b val, ssh_b offs, ssh_b fn);

extern ssh_w _PC;
extern ssh_b _I;
extern ssh_b _R;
extern ssh_b _IM;
extern ssh_b _IFF1;
extern ssh_b _IFF2;
extern ssh_b _STATE;
extern ssh_b _PORT_FE;
extern ssh_b _TRAP;
extern ssh_b* _A;
extern ssh_b* _B;
extern ssh_w* _BC;
extern ssh_w* _DE;
extern ssh_w* _HL;
extern ssh_w* _SP;

extern ssh_b* memZX;
extern ssh_b regsZX[22];
extern ssh_b portsZX[65536];

extern HWND hWnd;
extern RECT rectWnd;
extern DWORD delayCPU;
extern DWORD delayGPU;
