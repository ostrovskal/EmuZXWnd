
#include "stdafx.h"
#include "zxGamePad.h"

void zxGamepad::init() {
	// ���������� �����
	for(int i = 0; i < 2; i++) {
		zxString map = theApp->getOpt(OPT_JOY1_STATUS + i)->sval;
		// POV4, but12, xAxis2, yAxis2, zAxis2, xRot2, yRot2, zRot2
		int count = 0;
		auto maps = map.split(L",", count);
		memset(&mapOrig[i], 0, countButtons);

		if(count > countButtons) count = countButtons;
		if(count < 0) count = 0;
		for(int j = 0; j < count; j++) {
			mapOrig[i][j] = (ssh_b)*(ssh_u*)asm_ssh_wton(maps[j].str(), (ssh_u)Radix::_hex);
		}
		changeMode(i, theApp->getOpt(OPT_JOY1_MAPPING + i)->dval);
	}
}

void zxGamepad::update() {
	JOYINFOEX jie;
	jie.dwSize = sizeof(JOYINFOEX);
	for(int i = 0; i < 2; i++) {
		bool is = (joyGetPosEx(i, &jie) == JOYERR_NOERROR);
		bool bWasConnected(connected[i]);
		connected[i] = is;
		removed[i] = (bWasConnected && !connected[i]);
		inserted[i] = (!bWasConnected && connected[i]);
		if(is) {
			wButtons[i] = wNowButtons[i];
			axis[i][axisX] = (jie.dwXpos - 32767); axis[i][axisY] = jie.dwYpos - 32767; axis[i][axisZ] = jie.dwZpos - 32767;
			rot[i][axisX] = jie.dwRpos - 32767; rot[i][axisY] = jie.dwUpos - 32767; rot[i][axisZ] = jie.dwVpos - 32767;
			ssh_w v = 0;
			if(jie.dwFlags & JOY_RETURNPOV) {
				// POV
				auto pov = jie.dwPOV;
				if(pov != 0xffff) {
					static ssh_b cnvPOV[8] = {1, 3, 2, 6, 4, 12, 8, 9};
					v |= cnvPOV[pov / 4500];
				}
			}
			if(jie.dwFlags & JOY_RETURNBUTTONS) {
				// BUTTONS 12
				v |= (jie.dwButtons << 4) & 0xffff;
			}
			wNowButtons[i] = v;
		}
	}
}

ssh_b* zxGamepad::getPredefinedMode(int mode, int& count) {
	static ssh_b kempston[5] = {ZX_KEY_KEM_UP, ZX_KEY_KEM_RIGHT, ZX_KEY_KEM_DOWN, ZX_KEY_KEM_LEFT, ZX_KEY_KEM_FIRE};
	static ssh_b sinclair1[5] = {ZX_KEY_8, ZX_KEY_7, ZX_KEY_9, ZX_KEY_6, ZX_KEY_0};
	static ssh_b sinclair2[5] = {ZX_KEY_3, ZX_KEY_2, ZX_KEY_4, ZX_KEY_1, ZX_KEY_5};
	static ssh_b cursor[5] = {ZX_KEY_UP, ZX_KEY_RIGHT, ZX_KEY_DOWN, ZX_KEY_LEFT, ZX_KEY_0};
	static ssh_b keyboard[9] = {ZX_KEY_Q, ZX_KEY_P, ZX_KEY_A, ZX_KEY_O, ZX_KEY_SPACE, ZX_KEY_M, ZX_KEY_D, ZX_KEY_S, ZX_KEY_E};

	count = 5;
	
	switch(mode) {
		case JOY_KEMPSTON: return kempston;
		case JOY_INTERFACE_I: return sinclair1;
		case JOY_INTERFACE_II: return sinclair2;
		case JOY_CURSOR: return cursor;
		case JOY_KEYBOARD: count = 9; return keyboard;
	}
	return nullptr;
}

void zxGamepad::changeMode(bool second, int mode) {
	int count = 0;
	auto m = getPredefinedMode(mode, count);
	modes[second] = mode;
	memcpy(&mapCurrent[second], &mapOrig[second], countButtons);
	if(m) memcpy(&mapCurrent[second], m, count);
}

void zxGamepad::updateKey(bool pressed, int k) {
	ZX_KEY* key = &keys[k];
	if(key->bitEx) {
		ssh_b portEx = key->portEx;
		ssh_b bitEx = key->bitEx;
		if(pressed) _KEYS[portEx] &= ~bitEx; else _KEYS[portEx] |= bitEx;
	}
	ssh_b port = key->port;
	ssh_b bit = key->bit;
	if(port == 31) {
		if(pressed) (*_KEMPSTON) |= bit; else (*_KEMPSTON) &= ~bit;
	} else {
		if(pressed) _KEYS[port] &= ~bit; else _KEYS[port] |= bit;
	}
}

void zxGamepad::remap(bool second) {
	// ��������� ��� ������ ��������� � ����������/�������� ��������������� �� ������ �� �����
	for(int i = 0; i < countButtons; i++) {
		ssh_b k = mapCurrent[second][i];
		if(k == 0) continue;
		if(i > but12) {
			// ��������� ��� � ��������
			long val;
			if(i > zAxisM) {
				//xRotP, xRotM, yRotP, yRotM, zRotP, zRotM
				val = is_rotate(second, (Axis)(i - xRotP));
			} else {
				//xAxisP, xAxisM, yAxisP, yAxisM, zAxisP, zAxisM
				val = is_axis(second, (Axis)((i - xAxisP) / 2));
			}
			if(val > 0 && !(i & 1)) {
				updateKey(true, k);
			} else if(val < 0 && (i & 1)) {
				updateKey(true, k);
			} else {
				updateKey(false, k);
			}
		} else {
			updateKey(wButtons[second] & (1 << i), k);
		}
	}
}
