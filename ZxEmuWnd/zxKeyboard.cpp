
#include "stdafx.h"
#include "zxKeyboard.h"

static ZX_KEY keys[] = {
	{IDC_BUTTON_K1, L"1", L"1", L"1", L"blue", L"DEF FN", L"!", L"edit", 0xfe, 0x1},
	{IDC_BUTTON_K2, L"2", L"2", L"2", L"red", L"FN", L"@", L"caps lock", 0xfe, 0x2},
	{IDC_BUTTON_K3, L"3", L"3", L"3", L"magenta", L"LINE", L"#", L"tr video", 0xfe, 0x4},
	{IDC_BUTTON_K4, L"4", L"4", L"4", L"green", L"OPEN", L"$", L"inv video", 0xfe, 0x8},
	{IDC_BUTTON_K5, L"5", L"5", L"5", L"cyan", L"CLOSE", L"%", L"left", 0xfe, 0x10},
	{IDC_BUTTON_K6, L"6", L"6", L"6", L"yellow", L"MOVE", L"'", L"down", 0xef, 0x10},
	{IDC_BUTTON_K7, L"7", L"7", L"7", L"white", L"ERASE", L"&&", L"right", 0xef, 0x8},
	{IDC_BUTTON_K8, L"8", L"8", L"8", L"", L"POINT", L"*", L"up", 0xef, 0x4},
	{IDC_BUTTON_K9, L"9", L"9", L"9", L"bright", L"CAT", L"(", L"graphics", 0xef, 0x2},
	{IDC_BUTTON_K0, L"0", L"0", L"0", L"black", L"FORMAT", L")", L"delete", 0xef, 0x1},
	{IDC_BUTTON_KQ, L"PLOT", L"q", L"Q", L"SIN", L"ASN", L"<=", L"Q", 0xfb, 0x1},
	{IDC_BUTTON_KW, L"DRAW", L"w", L"W", L"COS", L"ACS", L"<>", L"W", 0xfb, 0x2},
	{IDC_BUTTON_KE, L"REM", L"e", L"E", L"TAN", L"ATN", L">=", L"E", 0xfb, 0x4},
	{IDC_BUTTON_KR, L"RUN", L"r", L"R", L"INT", L"VERIFY", L"<", L"R", 0xfb, 0x8},
	{IDC_BUTTON_KT, L"RAND", L"t", L"T", L"RND", L"MERGE", L">", L"T", 0xfb, 0x10},
	{IDC_BUTTON_KY, L"RETURN", L"y", L"Y", L"STR$", L"[", L"AND", L"Y", 0xdf, 0x10},
	{IDC_BUTTON_KU, L"IF", L"u", L"U", L"CHR$", L"]", L"OR", L"U", 0xdf, 0x8},
	{IDC_BUTTON_KI, L"INPUT", L"i", L"I", L"CODE", L"IN", L"AT", L"I", 0xdf, 0x4},
	{IDC_BUTTON_KO, L"POKE", L"o", L"O", L"PEEK", L"OUT", L";", L"O", 0xdf, 0x2},
	{IDC_BUTTON_KP, L"PRINT", L"p", L"P", L"TAB", L"@", L"\"", L"P", 0xdf, 0x1},
	{IDC_BUTTON_KA, L"NEW", L"a", L"A", L"READ", L"~", L"STOP", L"A", 0xfd, 0x1},
	{IDC_BUTTON_KS, L"SAVE", L"s", L"S", L"RESTORE", L"|", L"NOT", L"S", 0xfd, 0x2},
	{IDC_BUTTON_KD, L"DIM", L"d", L"D", L"DATE", L"\\", L"STEP", L"D", 0xfd, 0x4},
	{IDC_BUTTON_KF, L"FOR", L"f", L"F", L"SGN", L"{", L"TO", L"F", 0xfd, 0x8},
	{IDC_BUTTON_KG, L"GOTO", L"g", L"G", L"ABS", L"}", L"THEN", L"G", 0xfd, 0x10},
	{IDC_BUTTON_KH, L"GOSUB", L"h", L"H", L"SQR", L"CIRCLE", L"!", L"H", 0xbf, 0x10},
	{IDC_BUTTON_KJ, L"LOAD", L"j", L"J", L"VAL", L"VAL$", L"-", L"J", 0xbf, 0x8},
	{IDC_BUTTON_KK, L"LIST", L"k", L"K", L"LEN", L"SCRN$", L"+", L"K", 0xbf, 0x4},
	{IDC_BUTTON_KL, L"LET", L"l", L"L", L"USR", L"ATTR", L"=", L"L", 0xbf, 0x2},
	{IDC_BUTTON_ENTER, L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER", 0xbf, 0x1},
	{IDC_BUTTON_ENTER1, L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER", 0xbf, 0x1},
	{IDC_BUTTON_CAPS_SHIFT, L"CAPS SHIFT", L"CAPS SHIFT", L"CAPS SHIFT", L"CAPS SHIFT", L"CAPS SHIFT", L"CAPS SHIFT", L"CAPS SHIFT", 0xbf, 0x1},
	{IDC_BUTTON_KZ, L"COPY", L"z", L"Z", L"LN", L"BEEP", L":", L"Z", 0xfe, 0x2},
	{IDC_BUTTON_KX, L"CLEAR", L"x", L"X", L"EXP", L"INK", L"FUNT", L"X", 0xfe, 0x4},
	{IDC_BUTTON_KC, L"CONT", L"c", L"C", L"LPRINT", L"PAPER", L"?", L"C", 0xfe, 0x8},
	{IDC_BUTTON_KV, L"CLS", L"v", L"V", L"LLIST", L"FLUSH", L"/", L"V", 0xfe, 0x10},
	{IDC_BUTTON_KB, L"BORDER", L"b", L"B", L"BIN", L"BRIGHT", L"*", L"B", 0x7f, 0x10},
	{IDC_BUTTON_KN, L"NEXT", L"n", L"N", L"INKEYS$", L"OVER", L",", L"N", 0x7f, 0x8},
	{IDC_BUTTON_KM, L"PAUSE", L"m", L"M", L"PI", L"INVERSE", L".", L"M", 0x7f, 0x4},
	{IDC_BUTTON_SYMBOL_SHIFT, L"SYMBOL SHIFT", L"SYMBOL SHIFT", L"SYMBOL SHIFT", L"SYMBOL SHIFT", L"SYMBOL SHIFT", L"SYMBOL SHIFT", L"SYMBOL SHIFT", 0x7f, 0x2},
	{IDC_BUTTON_SPACE, L"", L"", L"", L"", L"", L"", L"", 0x7f, 0x1}
};

static ssh_d WINAPI KeyProc(void* params) {
	return theApp.keyboard->procKEY();
}

DWORD zxKeyboard::procKEY() {
	while(true) {
		int nmode = KM_SH_E;
		if(mode != nmode) {
			mode = nmode;
			for(auto& k : keys) {
				ssh_cws name;
				switch(mode) {
					case KM_K: name = k.name_k; break;
					case KM_L: name = k.name_l; break;
					case KM_C: name = k.name_c; break;
					case KM_E: name = k.name_e; break;
					case KM_G: name = k.name_l; break;
					case KM_SH_E: name = k.name_sh_e; break;
					case KM_SH_KL: name = k.name_sh_k; break;
					case KM_CH: name = k.name_l; break;
				}
				SetWindowText(k.hWndKey, name);
			}
		}
	}
	return 0;
}

void zxKeyboard::onInitDialog(HWND hWnd, LPARAM lParam) {
	for(auto& k : keys) {
		k.hWndKey = GetDlgItem(hWnd, k.butID);
		SendMessage(k.hWndKey, WM_SETFONT, (WPARAM)hFont, true);
	}
}

bool zxKeyboard::preCreate() {
	DWORD keyID;
	hKeyThread = CreateThread(nullptr, 0, KeyProc, nullptr, CREATE_SUSPENDED, &keyID);
	hFont = CreateFont(-10, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS,
					   CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_ROMAN, L"Courier New");

	return true;
}

void zxKeyboard::show(bool visible) {
	if(visible) {
		showWindow(true);
		updateWindow();
		ResumeThread(hKeyThread);
	} else {
		if(isWindowVisible()) {
			showWindow(false);
			Wow64SuspendThread(hKeyThread);
		}
	}

}