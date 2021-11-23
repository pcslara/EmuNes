/*
 * Cpu.h
 *
 *  Created on: 25/04/2010
 *      Author: Pedro Carlos da Silva Lara
 */

#ifndef CPU_H_
#define CPU_H_

#include "nestypes.h"
#include "Memory.h"

class Nes;

/**
 * Emulate the Motorola 6502 CPU
 */

class Cpu {
public:

	// Registers
	byte a_reg;
	byte x_reg;
	byte y_reg;

	byte   sp_reg;
	ushort pc_reg;

	// Flags
	byte carry_flag;
	byte zero_flag;
	byte interrupt_flag;
	byte decimal_flag;
	byte brk_flag;
	byte overflow_flag;
	byte sign_flag;


	byte opcode;
	uint tick_count;
	uint tick_total;
	byte is_quit;

	Nes * nes;

public:

	Cpu( Nes * nes );

	// Helped Functions

	ushort MakeAddress( byte a, byte b ); // return (a << 8 || b)

	byte   ReadMem  ( ushort address );
	ushort ReadMem16( ushort address );
	byte   ReadMemX  ( ushort address, byte check );
	byte   ReadMemY  ( ushort address, byte check );
	byte   ReadMemIndirectX ( ushort address );
	byte   ReadMemIndirectY  ( ushort address, byte check );

	void WriteMem( ushort address, byte data );
	void WriteMemX( ushort address, byte data );
	void WriteMemY( ushort address, byte data );
	void WriteMemIndirectX( ushort address, byte data );
	void WriteMemIndirectY( ushort address, byte data );

	void Push  ( byte   data );
	void Push16( ushort data );
	void PushStat();

	byte   Pull();
	ushort Pull16();
	void   PullStat();

	void DumpProcessor( int  i );
	// Main

	byte GetStatus();

	void Run();

	Memory getMem();

	// Operations (ISA)

	void OpcodeADC();
	void OpcodeAND();
	void OpcodeASL();
	void OpcodeBCC();
	void OpcodeBCS();
	void OpcodeBEQ();
	void OpcodeBIT();
	void OpcodeBMI();
	void OpcodeBNE();
	void OpcodeBPL();
	void OpcodeBRK();
	void OpcodeBVC();
	void OpcodeBVS();
	void OpcodeCLC();
	void OpcodeCLD();
	void OpcodeCLI();
	void OpcodeCLV();
	void OpcodeCMP();
	void OpcodeCPX();
	void OpcodeCPY();
	void OpcodeDEC();
	void OpcodeDEX();
	void OpcodeDEY();
	void OpcodeEOR();
	void OpcodeINC();
	void OpcodeINX();
	void OpcodeINY();
	void OpcodeJMP();
	void OpcodeJSR();
	void OpcodeLDA();
	void OpcodeLDX();
	void OpcodeLDY();
	void OpcodeLSR();
	void OpcodeNOP();
	void OpcodeORA();
	void OpcodePHA();
	void OpcodePHP();
	void OpcodePLA();
	void OpcodePLP();
	void OpcodeROL();
	void OpcodeROR();
	void OpcodeRTI();
	void OpcodeRTS();
	void OpcodeSBC();
	void OpcodeSEC();
	void OpcodeSED();
	void OpcodeSEI();
	void OpcodeSTA();
	void OpcodeSTX();
	void OpcodeSTY();
	void OpcodeTAX();
	void OpcodeTAY();
	void OpcodeTSX();
	void OpcodeTXA();
	void OpcodeTXS();
	void OpcodeTYA();

};


#endif /* CPU_H_ */
