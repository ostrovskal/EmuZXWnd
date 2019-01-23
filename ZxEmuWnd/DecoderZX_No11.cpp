#include "stdafx.h"
#include "DecoderZX.h"

funcOps table_opsN11_XXX[] = {
	&DecoderZX::funcN11_000, &DecoderZX::funcN11_001, &DecoderZX::funcN11_000, &DecoderZX::funcN11_011, &DecoderZX::funcN11_000,
	&DecoderZX::funcN11_101, &DecoderZX::funcN11_000, &DecoderZX::funcN11_000
};

void DecoderZX::funcN11_000() {
	ssh_w nn = 0;
	bool flg = false;
	if(typeOps < 6) {
		if(typeOps > 0) nn = read_mem16PC();
		if(!isFlag(ops)) return;
	}
	switch(typeOps) {
		// RET CCC; ���� CCC �� PC <- [SP]; SP += 2
		case 0: 
			CpuZX::PC = read_mem16(*CpuZX::SP);
			(*CpuZX::SP) += 2;
			break;
		// JP CCC, nn; ���� CCC �� PC <- nn
		case 2:  CpuZX::PC = nn; break;
		// CALL ���, nn; ���� ��� �� SP -= 2; [SP] <-PC; PC <-nn
		// RST NNN; SP -= 2; [SP] <- PC;  PC <- NNN * 8
		case 4: case 7:
			(*CpuZX::SP) -= 2;
			write_mem16(*CpuZX::SP, CpuZX::PC);
			CpuZX::PC = (typeOps == 4 ? nn : ops * 8);
			break;
		// ARIFTH
		case 6:  opsAccum(read_mem8PC()); break;
	}
}


void DecoderZX::funcN11_001() {
	ssh_w reg;
	if(ops == 5 || ops == 7) {
		reg = *fromRP_AF(4);
	} else if(ops != 3) {
		reg = read_mem16(*CpuZX::SP);
		(*CpuZX::SP) += 2;
	}
	switch(ops) {
		// RET; PC <- [SP]; SP += 2
		// JP [HL/IX]; PC <- HL/IX;
		case 1:	case 5: CpuZX::PC = reg; break;
		// EXX
		case 3: swapReg(CpuZX::BC, (CpuZX::BC + 7)); swapReg(CpuZX::DE, (CpuZX::DE + 7)); swapReg(CpuZX::HL, (CpuZX::HL + 7)); break;
		// LD SP, HL; SP <- HL
		case 7: *CpuZX::SP = reg; break;
		// POP RP; RP <- [SP]; SP += 2
		default: *fromRP_AF(ops) = reg;
	}
}

void DecoderZX::funcN11_101() {
	switch(ops) {
		// CALL nn; SP -= 2; [SP] <- PC; PC <-nn
		case 1: execCALL(read_mem16PC()); break;
		// prefix DD/FD
		case 3: case 7: {
			ssh_b n = CpuZX::memory[CpuZX::PC];
			if(n == 0xDD || n == 0xFD || n == 0xED) noni();
			else execOps(((ops & 4) >> 2) + 1, (n == 0xCB ? PREFIX_CB : 0));
			break;
		}
		// prefix ED
		case 5: execOps(0, PREFIX_ED); break;
		// PUSH RP; SP -= 2; [SP] <-RP
		default: (*CpuZX::SP) -= 2; write_mem16(*CpuZX::SP, *fromRP_AF(ops));
	}
}

void DecoderZX::funcN11_011() {
	ssh_b a = *CpuZX::A;
	switch(ops) {
		// JP nn; PC <-nn
		case 0: CpuZX::PC = read_mem16PC(); break;
		// prefix CB
		case 1: execOps(0, PREFIX_CB); break;
		// OUT(d), A; [Ad] <- A
		case 2: writePort((a << 8 | read_mem8PC()), a); break;
		// IN A, (d); A <- [Ad]
		case 3: *CpuZX::A = readPort((a << 8) | (read_mem8PC())); break;
		// EX(SP), HL; [SP] <-> HL
		case 4: {
			ssh_w* reg = fromRP_AF(4);
			ssh_w nn = read_mem16(*CpuZX::SP);
			write_mem16(*CpuZX::SP, *reg);
			*reg = nn;
			break;
		}
		// EX HL, DE
		case 5: swapReg(CpuZX::DE, CpuZX::HL); break;
		default: CpuZX::IFF1 = CpuZX::IFF2 = (ops & 1); CpuZX::STATE &= CpuZX::INT; CpuZX::STATE |= (CpuZX::INT * !(ops & 1));
	}
}