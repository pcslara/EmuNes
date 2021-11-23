#include <stdio.h>
#include <stdlib.h>
#include "Cpu.h"
#include "Nes.h"
#include "PPU.h"

Cpu::Cpu( Nes * nes ) : nes( nes ){
	a_reg  = 0;
	x_reg  = 0;
	y_reg  = 0;
	sp_reg = 0xff;
	pc_reg = ReadMem16( 0xFFFC );

	is_quit = 0;

	carry_flag     = 0;
	zero_flag      = 0;
	interrupt_flag = 0;
	decimal_flag   = 0;
	brk_flag       = 0;
	overflow_flag  = 0;
	sign_flag      = 0;

	tick_count       = 0;
	tick_total       = 0;
}

byte Cpu::GetStatus() {
	byte statusdata = 0;

	if (sign_flag == 1)
		statusdata = (byte)(statusdata + 0x80);

	if (overflow_flag == 1)
		statusdata = (byte)(statusdata + 0x40);

	if (brk_flag == 1)
		statusdata = (byte)(statusdata + 0x10);

	if (decimal_flag == 1)
		statusdata = (byte)(statusdata + 0x8);

	if (interrupt_flag == 1)
		statusdata = (byte)(statusdata + 0x4);

	if (zero_flag == 1)
		statusdata = (byte)(statusdata + 0x2);

	if (carry_flag == 1)
		statusdata = (byte)(statusdata + 0x1);

	return statusdata;
}
ushort Cpu::MakeAddress( byte a, byte b ) {
	return (ushort)((b << 8) | a);
}

byte Cpu::ReadMem( ushort address ) {
	// return mem.Read( address );
	return nes->ReadMem8( address );
}

byte Cpu::ReadMemX( ushort address, byte check ) {
	if( check != 0 && ((address & 0xff00) != ((address + x_reg) & 0xff00)) ) {
		//printf("RX\n");
		tick_count += 1;
	}

	return nes->ReadMem8( (ushort)((x_reg + address) & 0xffff) );
}

byte Cpu::ReadMemY( ushort address, byte check ) {
	if( check != 0 && ((address & 0xff00) != ((address + y_reg) & 0xff00)) ) {
		//printf("RY\n");
		tick_count += 1;
	}

	return nes->ReadMem8(  (y_reg + address) & 0xffff );
}

ushort Cpu::ReadMem16( ushort address ) {
	byte x1 = nes->ReadMem8( address );
	byte x2 = nes->ReadMem8( address + 1 );

	return (x2 << 8) | x1;
}

byte Cpu::ReadMemIndirectX( ushort address ) {
	return nes->ReadMem8( (ushort) ReadMem16((ushort)(0xffff & (address + (ushort) x_reg) )) );
}

byte Cpu::ReadMemIndirectY( ushort address, byte check ) {
	//if( address <= 0xff )
	if( check != 0 && (((ReadMem16( address ) + y_reg ) & 0xff00) != ((ReadMem16( address ) & 0xff00) )) ) {
		//printf("RIY\n");
		tick_count += 1;
	}

	return nes->ReadMem8( (ushort) (ReadMem16( address ) + y_reg ) );
	//else
	//	return nes->ReadMem8( (ushort) ReadMem16((ushort)(0xffff & (address + (ushort) y_reg) )) );
}

void Cpu::WriteMem( ushort address, byte data ) {
	nes->WriteMem8( address, data );
}

void Cpu::WriteMemX( ushort address, byte data ) {
	//if( address <= 0xff )
	//	nes->WriteMem8((ushort)(0xff & (address + (ushort) x_reg)), data );
	//else
	nes->WriteMem8((ushort)(0xffff & (address + (ushort) x_reg)), data );
}

void Cpu::WriteMemY( ushort address, byte data ) {
	//if( address <= 0xff )
	//	nes->WriteMem8((ushort)(0xff & (address + (ushort) y_reg)), data );
	//else
	// printf("ADDR %x val %x\n", (ushort)(0xffff & (address + (ushort) y_reg)), data );
	nes->WriteMem8((ushort)(0xffff & (address + (ushort) y_reg)), data );
}

void Cpu::WriteMemIndirectX( ushort address, byte data ) {
	//if( address <= 0xff )
	//	nes->WriteMem8((ushort) ReadMem16((ushort)(0xff & (address + (ushort) x_reg) )), data );
	//else
	nes->WriteMem8((ushort) ReadMem16((ushort)(0xffff & (address + (ushort) x_reg) )), data );
}

void Cpu::WriteMemIndirectY( ushort address, byte data ) {
	//if( address <= 0xff )
	//	nes->WriteMem8((ushort) ReadMem16((ushort)(0xff & (address + (ushort) y_reg) )), data );
	//else
	nes->WriteMem8( (ushort) (ReadMem16(address) + y_reg), data );
}

void Cpu::Push ( byte   data ) {
	nes->WriteMem8((ushort)(0x100+sp_reg), (byte)(data & 0xff));
	sp_reg = (byte)(sp_reg - 1);
}

void Cpu::Push16( ushort data ) {
	Push( (byte)(data >> 8) );
	Push( (byte)(data & 0xff) );
}

void Cpu::PushStat() {
	byte statusdata = 0;

	if (sign_flag == 1)
		statusdata = (byte)(statusdata | 0x80);

	if (overflow_flag == 1)
		statusdata = (byte)(statusdata | 0x40);

	//statusdata = (byte)(statusdata + 0x20);

	if (brk_flag == 1)
		statusdata = (byte)(statusdata | 0x10);

	if (decimal_flag == 1)
		statusdata = (byte)(statusdata | 0x8);

	if (interrupt_flag == 1)
		statusdata = (byte)(statusdata | 0x4);

	if (zero_flag == 1)
		statusdata = (byte)(statusdata | 0x2);

	if (carry_flag == 1)
		statusdata = (byte)(statusdata + 0x1);

	Push( statusdata );
}

byte Cpu::Pull() {
	sp_reg = (byte)(sp_reg + 1);
	return nes->ReadMem8( (ushort)(0x100 + sp_reg) );
}

ushort Cpu::Pull16() {
	byte value1 = Pull();
	byte value2 = Pull();

	return MakeAddress( value1, value2 );

}

void Cpu::PullStat() {

	byte statusdata = Pull();
	if ((statusdata & 0x80) == 0x80)
		sign_flag = 1;
	else
		sign_flag = 0;

	if ((statusdata & 0x40) == 0x40)
		overflow_flag = 1;
	else
		overflow_flag = 0;

	if ((statusdata & 0x10) == 0x10)
		brk_flag = 1;
	else
		brk_flag = 0;

	if ((statusdata & 0x8) == 0x8)
		decimal_flag = 1;
	else
		decimal_flag = 0;

	if ((statusdata & 0x4) == 0x4)
		interrupt_flag = 1;
	else
		interrupt_flag = 0;

	if ((statusdata & 0x2) == 0x2)
		zero_flag = 1;
	else
		zero_flag = 0;

	if ((statusdata & 0x1) == 0x1)
		carry_flag = 1;
	else
		carry_flag = 0;
}

void Cpu::DumpProcessor(int i ) {
	printf("A = %x X = %x Y = %x PC = %x SP = %x i = %d OP = %x M1 = %x M2 = %x SL = %d S = %x\n", a_reg, x_reg, y_reg, pc_reg, sp_reg, i,opcode, nes->ReadMem8( pc_reg + 1), nes->ReadMem8( pc_reg + 2), nes->getPPU()->currScanLine, GetStatus() );
}

void Cpu::Run() {
	int i = 0;

	while( is_quit == 0 ) {

		opcode = ReadMem( pc_reg );
		//i++;
		//DumpProcessor( i++ );
		switch ( opcode ) {
			case 0x00: OpcodeBRK(); break;
			case 0x01: OpcodeORA(); break;
			case 0x05: OpcodeORA(); break;
			case 0x06: OpcodeASL(); break;
			case 0x08: OpcodePHP(); break;
			case 0x09: OpcodeORA(); break;
			case 0x0a: OpcodeASL(); break;
			case 0x0d: OpcodeORA(); break;
			case 0x0e: OpcodeASL(); break;
			case 0x10: OpcodeBPL(); break;
			case 0x11: OpcodeORA(); break;
			case 0x15: OpcodeORA(); break;
			case 0x16: OpcodeASL(); break;
			case 0x18: OpcodeCLC(); break;
			case 0x19: OpcodeORA(); break;
			case 0x1d: OpcodeORA(); break;
			case 0x1e: OpcodeASL(); break;
			case 0x20: OpcodeJSR(); break;
			case 0x21: OpcodeAND(); break;
			case 0x24: OpcodeBIT(); break;
			case 0x25: OpcodeAND(); break;
			case 0x26: OpcodeROL(); break;
			case 0x28: OpcodePLP(); break;
			case 0x29: OpcodeAND(); break;
			case 0x2a: OpcodeROL(); break;
			case 0x2c: OpcodeBIT(); break;
			case 0x2d: OpcodeAND(); break;
			case 0x2e: OpcodeROL(); break;
			case 0x30: OpcodeBMI(); break;
			case 0x31: OpcodeAND(); break;
			case 0x32: OpcodeNOP(); break;
			case 0x33: OpcodeNOP(); break;
			case 0x34: OpcodeNOP(); break;
			case 0x35: OpcodeAND(); break;
			case 0x36: OpcodeROL(); break;
			case 0x38: OpcodeSEC(); break;
			case 0x39: OpcodeAND(); break;
			case 0x3d: OpcodeAND(); break;
			case 0x3e: OpcodeROL(); break;
			case 0x40: OpcodeRTI(); break;
			case 0x41: OpcodeEOR(); break;
			case 0x45: OpcodeEOR(); break;
			case 0x46: OpcodeLSR(); break;
			case 0x48: OpcodePHA(); break;
			case 0x49: OpcodeEOR(); break;
			case 0x4a: OpcodeLSR(); break;
			case 0x4c: OpcodeJMP(); break;
			case 0x4d: OpcodeEOR(); break;
			case 0x4e: OpcodeLSR(); break;
			case 0x50: OpcodeBVC(); break;
			case 0x51: OpcodeEOR(); break;
			case 0x55: OpcodeEOR(); break;
			case 0x56: OpcodeLSR(); break;
			case 0x58: OpcodeCLI(); break;
			case 0x59: OpcodeEOR(); break;
			case 0x5d: OpcodeEOR(); break;
			case 0x5e: OpcodeLSR(); break;
			case 0x60: OpcodeRTS(); break;
			case 0x61: OpcodeADC(); break;
			case 0x65: OpcodeADC(); break;
			case 0x66: OpcodeROR(); break;
			case 0x68: OpcodePLA(); break;
			case 0x69: OpcodeADC(); break;
			case 0x6a: OpcodeROR(); break;
			case 0x6c: OpcodeJMP(); break;
			case 0x6d: OpcodeADC(); break;
			case 0x6e: OpcodeROR(); break;
			case 0x70: OpcodeBVS(); break;
			case 0x71: OpcodeADC(); break;
			case 0x75: OpcodeADC(); break;
			case 0x76: OpcodeROR(); break;
			case 0x78: OpcodeSEI(); break;
			case 0x79: OpcodeADC(); break;
			case 0x7d: OpcodeADC(); break;
			case 0x7e: OpcodeROR(); break;
			case 0x81: OpcodeSTA(); break;
			case 0x84: OpcodeSTY(); break;
			case 0x85: OpcodeSTA(); break;
			case 0x86: OpcodeSTX(); break;
			case 0x88: OpcodeDEY(); break;
			case 0x8a: OpcodeTXA(); break;
			case 0x8c: OpcodeSTY(); break;
			case 0x8d: OpcodeSTA(); break;
			case 0x8e: OpcodeSTX(); break;
			case 0x90: OpcodeBCC(); break;
			case 0x91: OpcodeSTA(); break;
			case 0x94: OpcodeSTY(); break;
			case 0x95: OpcodeSTA(); break;
			case 0x96: OpcodeSTX(); break;
			case 0x98: OpcodeTYA(); break;
			case 0x99: OpcodeSTA(); break;
			case 0x9a: OpcodeTXS(); break;
			case 0x9d: OpcodeSTA(); break;
			case 0xa0: OpcodeLDY(); break;
			case 0xa1: OpcodeLDA(); break;
			case 0xa2: OpcodeLDX(); break;
			case 0xa4: OpcodeLDY(); break;
			case 0xa5: OpcodeLDA(); break;
			case 0xa6: OpcodeLDX(); break;
			case 0xa8: OpcodeTAY(); break;
			case 0xa9: OpcodeLDA(); break;
			case 0xaa: OpcodeTAX(); break;
			case 0xac: OpcodeLDY(); break;
			case 0xad: OpcodeLDA(); break;
			case 0xae: OpcodeLDX(); break;
			case 0xb0: OpcodeBCS(); break;
			case 0xb1: OpcodeLDA(); break;
			case 0xb4: OpcodeLDY(); break;
			case 0xb5: OpcodeLDA(); break;
			case 0xb6: OpcodeLDX(); break;
			case 0xb8: OpcodeCLV(); break;
			case 0xb9: OpcodeLDA(); break;
			case 0xba: OpcodeTSX(); break;
			case 0xbc: OpcodeLDY(); break;
			case 0xbd: OpcodeLDA(); break;
			case 0xbe: OpcodeLDX(); break;
			case 0xc0: OpcodeCPY(); break;
			case 0xc1: OpcodeCMP(); break;
			case 0xc4: OpcodeCPY(); break;
			case 0xc5: OpcodeCMP(); break;
			case 0xc6: OpcodeDEC(); break;
			case 0xc8: OpcodeINY(); break;
			case 0xc9: OpcodeCMP(); break;
			case 0xca: OpcodeDEX(); break;
			case 0xcc: OpcodeCPY(); break;
			case 0xcd: OpcodeCMP(); break;
			case 0xce: OpcodeDEC(); break;
			case 0xd0: OpcodeBNE(); break;
			case 0xd1: OpcodeCMP(); break;
			case 0xd5: OpcodeCMP(); break;
			case 0xd6: OpcodeDEC(); break;
			case 0xd8: OpcodeCLD(); break;
			case 0xd9: OpcodeCMP(); break;
			case 0xdd: OpcodeCMP(); break;
			case 0xde: OpcodeDEC(); break;
			case 0xe0: OpcodeCPX(); break;
			case 0xe1: OpcodeSBC(); break;
			case 0xe4: OpcodeCPX(); break;
			case 0xe5: OpcodeSBC(); break;
			case 0xe6: OpcodeINC(); break;
			case 0xe8: OpcodeINX(); break;
			case 0xe9: OpcodeSBC(); break;
			case 0xec: OpcodeCPX(); break;
			case 0xed: OpcodeSBC(); break;
			case 0xee: OpcodeINC(); break;
			case 0xf0: OpcodeBEQ(); break;
			case 0xf1: OpcodeSBC(); break;
			case 0xf5: OpcodeSBC(); break;
			case 0xf6: OpcodeINC(); break;
			case 0xf8: OpcodeSED(); break;
			case 0xf9: OpcodeSBC(); break;
			case 0xfd: OpcodeSBC(); break;
			case 0xfe: OpcodeINC(); break;
			default:   OpcodeNOP(); break;
		}
		//nes->getPPU()->RenderScanLine();

		if( tick_count >= 113 ) {
			tick_count = tick_count - 113;
			nes->getPPU()->RenderScanLine();
		}

		// printf("2 scan on cpu : %d\n", nes->getPPU().currScanLine );
	}
}

void Cpu::OpcodeADC() {

	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );
	byte arg2 = ReadMem( (ushort) (pc_reg + 2) );

	byte value = 0x00;
	switch ( opcode ) {
		case 0x69:
			value = arg1;

			pc_reg     += 2;
			tick_count += 2;
			break;
		case 0x65:
			value = ReadMem( arg1 );

			pc_reg     += 2;
			tick_count += 3;
			break;

		case 0x75:
			value = ReadMemX( arg1, 0 );

			pc_reg     += 2;
			tick_count += 4;
			break;

		case 0x6D:
			value = ReadMem( MakeAddress( arg1, arg2 ) );

			pc_reg     += 3;
			tick_count += 4;
			break;

		case 0x7D:
			value = ReadMemX( MakeAddress( arg1, arg2 ), 1 );

			pc_reg     += 3;
			tick_count += 4;
			break;

		case 0x79:
			value = ReadMemY( MakeAddress( arg1, arg2 ), 1 );

			pc_reg     += 3;
			tick_count += 4;
			break;

		case 0x61:
			value = ReadMemIndirectX( arg1 );

			pc_reg     += 2;
			tick_count += 6;
			break;

		case 0x71:
			value = ReadMemIndirectY( arg1, 1 );

			pc_reg     += 2;
			tick_count += 5;
			break;

		default:
			break;
	}

	uint value32 = (uint)( a_reg + value + carry_flag );

	if( value32 > 0xFF ) {
		carry_flag    = 1;
		overflow_flag = 1;
	} else {
		carry_flag    = 0;
		overflow_flag = 0;
	}

	if( (value32 & 0xff) == 0x00)
		zero_flag = 1;
	else
		zero_flag = 0;

	if( (value32 & 0x80) == 0x80 )
		sign_flag = 1;
	else
		sign_flag = 0;

	a_reg = (byte)( value32 & 0xFF );
}

void Cpu::OpcodeAND() {
	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );
	byte arg2 = ReadMem( (ushort) (pc_reg + 2) );

	byte value = 0x00;
	switch ( opcode ) {
		case 0x29:
			value = arg1;

			pc_reg     += 2;
			tick_count += 2;
			break;
		case 0x25:
			value = ReadMem( arg1 );

			pc_reg     += 2;
			tick_count += 3;
			break;

		case 0x35:
			value = ReadMemX( arg1, 0 );

			pc_reg     += 2;
			tick_count += 4;
			break;

		case 0x2D:
			value = ReadMem( MakeAddress( arg1, arg2 ) );

			pc_reg     += 3;
			tick_count += 3;
			break;

		case 0x3D:
			value = ReadMemX( MakeAddress( arg1, arg2 ), 1 );

			pc_reg     += 3;
			tick_count += 4;
			break;

		case 0x39:
			value = ReadMemY( MakeAddress( arg1, arg2 ), 1 );

			pc_reg     += 3;
			tick_count += 4;
			break;

		case 0x21:
			value = ReadMemIndirectX( arg1 );

			pc_reg     += 2;
			tick_count += 6;
			break;

		case 0x31:
			value = ReadMemIndirectY( arg1, 0 );

			pc_reg     += 2;
			tick_count += 5;
			break;

		default:
			break;
	}

	a_reg = (byte) ( a_reg & value );

	if( (a_reg & 0xff) == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if( (a_reg & 0x80) == 0x80 )
		sign_flag = 1;
	else
		sign_flag = 0;

}

void Cpu::OpcodeASL() {
	// Aritmetic Shift Left
	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );
	byte arg2 = ReadMem( (ushort) (pc_reg + 2) );

	byte value = 0x00;
	switch ( opcode ) {
		case 0x0a:
			value = a_reg;

			break;
		case 0x06:
			value = ReadMem( arg1 );

			break;

		case 0x16:
			value = ReadMemX( arg1, 0 );

			break;

		case 0x0e:
			value = ReadMem( MakeAddress( arg1, arg2 ) );

			break;

		case 0x1e:
			value = ReadMemX( MakeAddress( arg1, arg2 ), 0 );

			break;

		default:
			break;
	}

	if( (value & 0x80) == 0x80 )
		carry_flag = 1;
	else
		carry_flag = 0;

	value = (byte)( value << 1 );

	if( value == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if( (value & 0x80) == 0x80 )
		sign_flag = 1;
	else
		sign_flag = 0;

	switch ( opcode ) {
		case 0x0a:
			a_reg = value;

			tick_count += 2;
			pc_reg     += 1;
			break;
		case 0x06:
			WriteMem( arg1, value );

			tick_count += 5;
			pc_reg     += 2;
			break;

		case 0x16:
			WriteMemX( arg1, value );

			tick_count += 6;
			pc_reg     += 2;
			break;

		case 0x0e:
			WriteMem( MakeAddress( arg1, arg2), value );

			tick_count += 6;
			pc_reg     += 3;
			break;

		case 0x1e:
			WriteMemX( MakeAddress( arg1, arg2), value );

			tick_count += 7;
			pc_reg     += 3;
			break;

		default:
			break;
	}
}

void Cpu::OpcodeBCC() {
	// Branch if carry clear
	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );


	pc_reg     += 2;
	tick_count += 2;

	if( carry_flag == 0 ) {

		if( (pc_reg & 0xff00) != ((pc_reg + 2 + (sbyte)arg1) & 0xff00)) {
			tick_count += 1;
		}
		pc_reg = (ushort)( pc_reg + (sbyte)arg1 );
		tick_count += 1;
	}
}

void Cpu::OpcodeBCS() {
	// Branch if carry set
	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );


	pc_reg     += 2;
	tick_count += 2;

	if( carry_flag == 1 ) {

		if( (pc_reg & 0xff00) != ((pc_reg + 2 + (sbyte)arg1) & 0xff00))
			tick_count += 1;

		pc_reg = (ushort)( pc_reg + (sbyte)arg1 );
		tick_count += 1;
	}
}

void Cpu::OpcodeBEQ() {

	// Branch if equal
	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );

	pc_reg     += 2;
	tick_count += 2;

	if( zero_flag == 1 ) {

		if( (pc_reg & 0xff00) != ((pc_reg + 2 + (sbyte)arg1) & 0xff00))
			tick_count += 1;

		pc_reg = (ushort)( pc_reg + (sbyte)arg1 );
		tick_count += 1;
	}

}

void Cpu::OpcodeBIT() {
	// Test memory bits with accumulator (logic and)

	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );
	byte arg2 = ReadMem( (ushort) (pc_reg + 2) );

	byte value = 0x00;

	switch( opcode ) {
		case 0x24:
			value = ReadMem( arg1 );

			pc_reg     += 2;
			tick_count += 3;
			break;
		case 0x2c:
			value = ReadMem( MakeAddress( arg1, arg2 ) );

			pc_reg     += 3;
			tick_count += 4;
			break;

		default:
			break;
	}

	if( (a_reg & value) == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if( (value & 0x80) == 0x80 )
		sign_flag = 1;
	else
		sign_flag = 0;

	if( (value & 0x40) == 0x40 )
		overflow_flag = 1;
	else
		overflow_flag = 0;
}


void Cpu::OpcodeBMI() {
		// Branch if minus
		byte arg1 = ReadMem( (ushort) (pc_reg + 1) );

		pc_reg     += 2;
		tick_count += 2;

		if( sign_flag == 1 ) {

			if( (pc_reg & 0xff00) != ((pc_reg + 2 + (sbyte)arg1) & 0xff00))
				tick_count += 1;

			pc_reg = (ushort)( pc_reg + (sbyte)arg1 );
			tick_count += 1;
		}
}

void Cpu::OpcodeBNE() {
	// Branch if not zero
	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );

	pc_reg     += 2;
	tick_count += 2;

	if( zero_flag == 0 ) {

		if( (pc_reg & 0xff00) != ((pc_reg + 2 + (sbyte)arg1) & 0xff00))
			tick_count += 1;

		pc_reg = (ushort)(pc_reg + (sbyte) arg1);
		tick_count += 1;
	}
}

void Cpu::OpcodeBPL() {
	// Branch if plus
	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );

	pc_reg     += 2;
	tick_count += 2;

	if( sign_flag == 0 ) {

		if( (pc_reg & 0xff00) != ((pc_reg + 2 + (sbyte)arg1) & 0xff00))
			tick_count += 1;

		pc_reg = (ushort)( pc_reg + (sbyte)arg1 );
		tick_count += 1;
	}
}

void Cpu::OpcodeBRK() {
	// Break
	brk_flag       = 1;
	pc_reg        += 2;
	tick_count    += 2;

	Push16( pc_reg );
	PushStat();
	interrupt_flag = 1;
	pc_reg = ReadMem16( 0xFFFE );

	//is_quit = 1;

}

void Cpu::OpcodeBVC() {
	// Branch if overflow clear
	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );

	pc_reg     += 2;
	tick_count += 2;

	if( overflow_flag == 0 ) {

		if( (pc_reg & 0xff00) != ((pc_reg + 2 + (sbyte)arg1) & 0xff00))
			tick_count += 1;

		pc_reg = (ushort)( pc_reg + (sbyte)arg1 );
		tick_count += 1;
	}
}

void Cpu::OpcodeBVS() {
	// Branch if overflow set
	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );

	pc_reg     += 2;
	tick_count += 2;

	if( overflow_flag == 1 ) {

		if( (pc_reg & 0xff00) != ((pc_reg + 2 + (sbyte)arg1) & 0xff00))
			tick_count += 1;

		pc_reg = (ushort)( pc_reg + (sbyte)arg1 );
		tick_count += 1;
	}
}

void Cpu::OpcodeCLC() {
	// clear carry
	carry_flag  = 0;
	pc_reg     += 1;
	tick_count += 2;
}

void Cpu::OpcodeCLD() {
	// clear decimal flag
	decimal_flag = 0;
	pc_reg      += 1;
	tick_count  += 2;
}

void Cpu::OpcodeCLI() {
	// clear interruption
	interrupt_flag = 0;
	pc_reg        += 1;
	tick_count    += 2;
}

void Cpu::OpcodeCLV() {
	// clear overflow
	overflow_flag  = 0;
	pc_reg        += 1;
	tick_count    += 2;
}

void Cpu::OpcodeCMP() {

	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );
	byte arg2 = ReadMem( (ushort) (pc_reg + 2) );

	byte value = 0x00;

	switch ( opcode ) {
		case 0xc9:
			value = arg1;

			pc_reg     += 2;
			tick_count += 2;
			break;
		case 0xc5:
			value = ReadMem( arg1 );

			pc_reg     += 2;
			tick_count += 3;
			break;

		case 0xd5:
			value = ReadMemX( arg1, 0 );

			pc_reg     += 2;
			tick_count += 4;
			break;

		case 0xcd:
			value = ReadMem( MakeAddress( arg1, arg2 ) );

			pc_reg     += 3;
			tick_count += 4;
			break;

		case 0xdd:
			value = ReadMemX( MakeAddress( arg1, arg2 ), 1 );

			pc_reg     += 3;
			tick_count += 4;
			break;

		case 0xd9:
			value = ReadMemY( MakeAddress( arg1, arg2 ), 1 );

			pc_reg     += 3;
			tick_count += 4;
			break;

		case 0xc1:
			value = ReadMemIndirectX( arg1 );

			pc_reg     += 2;
			tick_count += 6;
			break;

		case 0xd1:
			value = ReadMemIndirectY( arg1, 1 );

			pc_reg     += 2;
			tick_count += 5;
			break;

		default:
			break;
	}



	if( a_reg >= value )
		carry_flag = 1;
	else
		carry_flag = 0;

	value = (byte)( a_reg - value );

	if( (value & 0xff) == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if ((value & 0x80) == 0x80)
		sign_flag = 1;
	else
		sign_flag = 0;

}

void Cpu::OpcodeCPX() {

	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );
	byte arg2 = ReadMem( (ushort) (pc_reg + 2) );

	byte value = 0x00;

	switch ( opcode ) {
		case 0xe0:
			value = arg1;

			pc_reg     += 2;
			tick_count += 2;
			break;
		case 0xe4:
			value = ReadMem( arg1 );

			pc_reg     += 2;
			tick_count += 3;
			break;

		case 0xec:
			value = ReadMem( MakeAddress( arg1, arg2 ) );

			pc_reg     += 3;
			tick_count += 4;
			break;
		default:
			break;
	}

	if( x_reg >= value )
		carry_flag = 1;
	else
		carry_flag = 0;

	value = (byte)( x_reg - value );

	if( (value & 0xff) == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if ((value & 0x80) == 0x80)
		sign_flag = 1;
	else
		sign_flag = 0;
}

void Cpu::OpcodeCPY() {
	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );
	byte arg2 = ReadMem( (ushort) (pc_reg + 2) );

	byte value = 0x00;

	switch ( opcode ) {
		case 0xc0:
			value = arg1;

			pc_reg     += 2;
			tick_count += 2;
			break;
		case 0xc4:
			value = ReadMem( arg1 );

			pc_reg     += 2;
			tick_count += 3;
			break;

		case 0xcc:
			value = ReadMem( MakeAddress( arg1, arg2 ) );

			pc_reg     += 3;
			tick_count += 4;
			break;
		default:
			break;
	}

	if( y_reg >= value )
		carry_flag = 1;
	else
		carry_flag = 0;

	value = (byte)( y_reg - value );

	if( (value & 0xff) == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if ((value & 0x80) == 0x80)
		sign_flag = 1;
	else
		sign_flag = 0;
}

void Cpu::OpcodeDEC() {
	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );
	byte arg2 = ReadMem( (ushort) (pc_reg + 2) );

	byte value = 0x00;

	switch ( opcode ) {
		case 0xc6:
			value = ReadMem( arg1 );
 			break;

		case 0xd6:
			value = ReadMemX( arg1, 0 );
			break;
		case 0xce:
			value = ReadMem( MakeAddress( arg1, arg2 ) );
			break;

		case 0xde:
			value = ReadMemX( MakeAddress( arg1, arg2 ), 0 );
			break;
		default:
			break;
	}

	value = (value - 1) & 0xff;

	if( value == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if( (value & 0x80) == 0x80 )
		sign_flag = 1;
	else
		sign_flag = 0;

	switch ( opcode ) {
		case 0xc6:
			WriteMem( arg1, value );
			pc_reg     += 2;
			tick_count += 5;
			break;

		case 0xd6:
			WriteMemX( arg1, value );

			pc_reg     += 2;
			tick_count += 6;
			break;
		case 0xce:
			WriteMem( MakeAddress( arg1, arg2 ), value );

			pc_reg     += 3;
			tick_count += 6;
			break;

		case 0xde:
			WriteMemX( MakeAddress( arg1, arg2), value );

			pc_reg     += 3;
			tick_count += 7;
			break;
		default:
			break;
	}
}

void Cpu::OpcodeDEX() {
	// decrement X
	x_reg   = (x_reg - 1) & 0xff;

	pc_reg     += 1;
	tick_count += 2;

	if( x_reg == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if( (x_reg & 0x80) == 0x80 )
		sign_flag = 1;
	else
		sign_flag = 0;
}

void Cpu::OpcodeDEY() {
	// decrement Y
	y_reg   = (y_reg - 1) & 0xff;

	pc_reg     += 1;
	tick_count += 2;

	if( y_reg == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if( (y_reg & 0x80) == 0x80 )
		sign_flag = 1;
	else
		sign_flag = 0;
}

void Cpu::OpcodeEOR() {
	// Exclusive OR
	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );
	byte arg2 = ReadMem( (ushort) (pc_reg + 2) );

	byte value = 0x00;
	switch ( opcode ) {
		case 0x49:
			value = arg1;

			pc_reg     += 2;
			tick_count += 2;
			break;
		case 0x45:
			value = ReadMem( arg1 );

			pc_reg     += 2;
			tick_count += 3;
			break;

		case 0x55:
			value = ReadMemX( arg1, 0 );

			pc_reg     += 2;
			tick_count += 4;
			break;

		case 0x4D:
			value = ReadMem( MakeAddress( arg1, arg2 ) );

			pc_reg     += 3;
			tick_count += 3;
			break;

		case 0x5D:
			value = ReadMemX( MakeAddress( arg1, arg2 ), 1 );

			pc_reg     += 3;
			tick_count += 4;
			break;

		case 0x59:
			value = ReadMemY( MakeAddress( arg1, arg2 ), 1 );

			pc_reg     += 3;
			tick_count += 4;
			break;

		case 0x41:
			value = ReadMemIndirectX( arg1 );

			pc_reg     += 2;
			tick_count += 6;
			break;

		case 0x51:
			value = ReadMemIndirectY( arg1, 1 );

			pc_reg     += 2;
			tick_count += 5;
			break;

		default:
			break;
	}

	a_reg = (byte) ( a_reg ^ value );

	if( (a_reg & 0xff) == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if( (a_reg & 0x80) == 0x80 )
		sign_flag = 1;
	else
		sign_flag = 0;
}

void Cpu::OpcodeINC() {
	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );
	byte arg2 = ReadMem( (ushort) (pc_reg + 2) );

	byte value = 0x00;

	switch ( opcode ) {
		case 0xe6:
			value = ReadMem( arg1 );
 			break;

		case 0xf6:
			value = ReadMemX( arg1, 0 );
			break;
		case 0xee:
			value = ReadMem( MakeAddress( arg1, arg2 ) );
			break;

		case 0xfe:
			value = ReadMemX( MakeAddress( arg1, arg2 ), 0 );
			break;
		default:
			break;
	}

	value = (value + 1) & 0xff;

	if( (value & 0xff) == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if( (value & 0x80) == 0x80 )
		sign_flag = 1;
	else
		sign_flag = 0;

	switch ( opcode ) {
		case 0xe6:
			WriteMem( arg1, value );
			pc_reg     += 2;
			tick_count += 5;
			break;

		case 0xf6:
			WriteMemX( arg1, value );

			pc_reg     += 2;
			tick_count += 6;
			break;
		case 0xee:
			WriteMem( MakeAddress( arg1, arg2 ), value );

			pc_reg     += 3;
			tick_count += 6;
			break;

		case 0xfe:
			WriteMemX( MakeAddress( arg1, arg2), value );

			pc_reg     += 3;
			tick_count += 7;
			break;
		default:
			break;
	}

}

void Cpu::OpcodeINX() {
		// increment X
		x_reg   = (x_reg + 1) & 0xff;

		pc_reg     += 1;
		tick_count += 2;

		if( x_reg == 0 )
			zero_flag = 1;
		else
			zero_flag = 0;

		if( (x_reg & 0x80) == 0x80 )
			sign_flag = 1;
		else
			sign_flag = 0;

}
void Cpu::OpcodeINY() {
		// increment Y
		y_reg   = (y_reg + 1) & 0xff;

		pc_reg     += 1;
		tick_count += 2;

		if( y_reg == 0 )
			zero_flag = 1;
		else
			zero_flag = 0;

		if( (y_reg & 0x80) == 0x80 )
			sign_flag = 1;
		else
			sign_flag = 0;

}
void Cpu::OpcodeJMP() {
	ushort addr = ReadMem16((ushort) (pc_reg + 1 ));

	switch( opcode ) {
		case 0x4c:
			// printf("%.4X %.2X\n", addr, ReadMem( addr ) );
			pc_reg      = addr;
			tick_count += 3;
			break;
		case 0x6c:
			pc_reg      = ReadMem16( addr );
			tick_count += 5;
			break;
		default:
			break;
	}
}

void Cpu::OpcodeJSR() {
	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );
	byte arg2 = ReadMem( (ushort) (pc_reg + 2) );

	Push16( (ushort)(pc_reg + 2) );
	pc_reg      = MakeAddress( arg1, arg2 );
	tick_count += 6;
}

void Cpu::OpcodeLDA() {
	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );
	byte arg2 = ReadMem( (ushort) (pc_reg + 2) );

	switch( opcode ) {
		case 0xa9:
			a_reg = arg1;
			pc_reg     += 2;
			tick_count += 2;
			break;

		case 0xa5:
			a_reg = ReadMem( arg1 );
			pc_reg     += 2;
			tick_count += 3;
			break;

		case 0xb5:
			a_reg = ReadMemX( arg1, 0 );
			pc_reg     += 2;
			tick_count += 4;
			break;

		case 0xad:
			a_reg = ReadMem( MakeAddress(arg1, arg2) );
			pc_reg     += 3;
			tick_count += 4;
			break;

		case 0xbd:
			a_reg = ReadMemX( MakeAddress(arg1, arg2), 1 );
			pc_reg     += 3;
			tick_count += 4;
			break;

		case 0xb9:
			a_reg = ReadMemY( MakeAddress(arg1, arg2), 1 );
			pc_reg     += 3;
			tick_count += 4;
			break;
		case 0xa1:
			a_reg = ReadMemIndirectX( arg1 );
			pc_reg     += 2;
			tick_count += 6;
			break;

		case 0xb1:
			a_reg = ReadMemIndirectY( arg1, 1 );
			pc_reg     += 2;
			tick_count += 5;
			break;
		default:
			break;
	}

	if( (a_reg & 0xff) == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if( (a_reg & 0x80) == 0x80 )
		sign_flag = 1;
	else
		sign_flag = 0;
}

void Cpu::OpcodeLDX() {
	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );
	byte arg2 = ReadMem( (ushort) (pc_reg + 2) );


	switch( opcode ) {
		case 0xa2:
			x_reg = arg1;
			pc_reg     += 2;
			tick_count += 2;
			break;

		case 0xa6:
			x_reg = ReadMem( arg1 );
			pc_reg     += 2;
			tick_count += 3;
			break;

		case 0xb6:
			x_reg = ReadMemY( arg1, 0 );
			pc_reg     += 2;
			tick_count += 4;
			break;

		case 0xae:
			x_reg = ReadMem( MakeAddress(arg1, arg2) );
			pc_reg     += 3;
			tick_count += 4;
			break;

		case 0xbe:
			x_reg = ReadMemY( MakeAddress(arg1, arg2), 1 );
			pc_reg     += 3;
			tick_count += 4;
			break;

		default:
			break;
	}

	if( x_reg == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if( (x_reg & 0x80) == 0x80 )
		sign_flag = 1;
	else
		sign_flag = 0;
}

void Cpu::OpcodeLDY() {
	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );
	byte arg2 = ReadMem( (ushort) (pc_reg + 2) );

	switch( opcode ) {
		case 0xa0:
			y_reg = arg1;
			pc_reg     += 2;
			tick_count += 2;
			break;

		case 0xa4:
			y_reg = ReadMem( arg1 );
			pc_reg     += 2;
			tick_count += 3;
			break;

		case 0xb4:
			y_reg = ReadMemX( arg1, 0 );
			pc_reg     += 2;
			tick_count += 4;
			break;

		case 0xac:
			y_reg = ReadMem( MakeAddress(arg1, arg2) );
			pc_reg     += 3;
			tick_count += 4;
			break;

		case 0xbc:
			y_reg = ReadMemX( MakeAddress(arg1, arg2), 1 );
			pc_reg     += 3;
			tick_count += 4;
			break;

		default:
			break;
	}

	if( y_reg == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if( (y_reg & 0x80) == 0x80 )
		sign_flag = 1;
	else
		sign_flag = 0;

}

void Cpu::OpcodeLSR() {
	// Logical Shift Right

	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );
	byte arg2 = ReadMem( (ushort) (pc_reg + 2) );

	byte value = 0x00;
	switch ( opcode ) {
		case 0x4a:
			value = a_reg;
			break;

		case 0x46:
			value = ReadMem( arg1 );
			break;

		case 0x56:
			value = ReadMemX( arg1, 0 );
			break;

		case 0x4e:
			value = ReadMem( MakeAddress( arg1, arg2 ) );
			break;

		case 0x5e:
			value = ReadMemX( MakeAddress( arg1, arg2 ), 0 );
			break;

		default:
			break;
	}

	if( (value & 1 ) == 1 )
		carry_flag = 1;
	else
		carry_flag = 0;

	value = (byte)( value >> 1 );

	if( value == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if( (value & 0x80) == 0x80 )
		// never do here
		sign_flag = 1;
	else
		sign_flag = 0;

	switch ( opcode ) {
		case 0x4a:
			a_reg = value;

			tick_count += 2;
			pc_reg     += 1;
			break;
		case 0x46:
			WriteMem( arg1, value );

			tick_count += 5;
			pc_reg     += 2;
			break;

		case 0x56:
			WriteMemX( arg1, value );

			tick_count += 6;
			pc_reg     += 2;
			break;

		case 0x4e:
			WriteMem( MakeAddress( arg1, arg2), value );

			tick_count += 6;
			pc_reg     += 3;
			break;

		case 0x5e:
			WriteMemX( MakeAddress( arg1, arg2), value );

			tick_count += 7;
			pc_reg     += 3;
			break;

		default:
			break;
	}
}

void Cpu::OpcodeNOP() {
	pc_reg     += 1;
	tick_count += 2;
}

void Cpu::OpcodeORA() {
	// Logic OR with Accumulator
	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );
	byte arg2 = ReadMem( (ushort) (pc_reg + 2) );

	byte value = 0x00;
	switch ( opcode ) {
		case 0x09:
			value = arg1;

			pc_reg     += 2;
			tick_count += 2;
			break;
		case 0x05:
			value = ReadMem( arg1 );

			pc_reg     += 2;
			tick_count += 3;
			break;

		case 0x15:
			value = ReadMemX( arg1, 0 );

			pc_reg     += 2;
			tick_count += 4;
			break;

		case 0x0D:
			value = ReadMem( MakeAddress( arg1, arg2 ) );

			pc_reg     += 3;
			tick_count += 4;
			break;

		case 0x1D:
			value = ReadMemX( MakeAddress( arg1, arg2 ), 1 );

			pc_reg     += 3;
			tick_count += 4;
			break;

		case 0x19:
			value = ReadMemY( MakeAddress( arg1, arg2 ), 1 );

			pc_reg     += 3;
			tick_count += 4;
			break;

		case 0x01:
			value = ReadMemIndirectX( arg1 );

			pc_reg     += 2;
			tick_count += 6;
			break;

		case 0x11:
			value = ReadMemIndirectY( arg1, 0 );

			pc_reg     += 2;
			tick_count += 5;
			break;

		default:
			break;
	}

	a_reg = (byte) ( a_reg | value );

	if( a_reg == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if( (a_reg & 0x80) == 0x80 )
		sign_flag = 1;
	else
		sign_flag = 0;

}

void Cpu::OpcodePHA() {
	Push( a_reg );
	pc_reg     += 1;
	tick_count += 3;
}

void Cpu::OpcodePHP() {
	PushStat();
	pc_reg     += 1;
	tick_count += 3;
}

void Cpu::OpcodePLA() {
	a_reg = Pull() & 0xff;

	pc_reg     += 1;
	tick_count += 4;


	if( a_reg == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if ((a_reg & 0x80) == 0x80)
		sign_flag = 1;
	else
		sign_flag = 0;

}

void Cpu::OpcodePLP() {
		PullStat();
		pc_reg      += 1;
		tick_count  += 4;
}

void Cpu::OpcodeROL() {
	// Rotate Left

	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );
	byte arg2 = ReadMem( (ushort) (pc_reg + 2) );

	byte value = 0x00;
	switch ( opcode ) {
		case 0x2a:
			value = a_reg;
			break;

		case 0x26:
			value = ReadMem( arg1 );
			break;

		case 0x36:
			value = ReadMemX( arg1, 0 );
			break;

		case 0x2e:
			value = ReadMem( MakeAddress( arg1, arg2 ) );
			break;

		case 0x3e:
			value = ReadMemX( MakeAddress( arg1, arg2 ), 0 );
			break;

		default:
			break;
	}

	byte new_carry = (value >> 7) & 0x01;

	value = (byte)(( value << 1 ) | carry_flag);

	carry_flag = new_carry;

	if( value == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if( (value & 0x80) == 0x80 )
		sign_flag = 1;
	else
		sign_flag = 0;

	switch ( opcode ) {
		case 0x2a:
			a_reg = value;

			tick_count += 2;
			pc_reg     += 1;
			break;
		case 0x26:
			WriteMem( arg1, value );

			tick_count += 5;
			pc_reg     += 2;
			break;

		case 0x36:
			WriteMemX( arg1, value );

			tick_count += 6;
			pc_reg     += 2;
			break;

		case 0x2e:
			WriteMem( MakeAddress( arg1, arg2), value );

			tick_count += 6;
			pc_reg     += 3;
			break;

		case 0x3e:
			WriteMemX( MakeAddress( arg1, arg2), value );

			tick_count += 7;
			pc_reg     += 3;
			break;

		default:
			break;
	}

}

void Cpu::OpcodeROR() {
	// Rotate Right


	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );
	byte arg2 = ReadMem( (ushort) (pc_reg + 2) );

	byte value = 0x00;
	switch ( opcode ) {
		case 0x6a:
			value = a_reg;
			break;

		case 0x66:
			value = ReadMem( arg1 );
			break;

		case 0x76:
			value = ReadMemX( arg1, 0 );
			break;

		case 0x6e:
			value = ReadMem( MakeAddress( arg1, arg2 ) );
			break;

		case 0x7e:
			value = ReadMemX( MakeAddress( arg1, arg2 ), 0 );
			break;

		default:
			break;
	}

	byte new_carry = value  & 0x01;

	value = (byte)(( value >> 1 ) | ((carry_flag & 0x1) << 7));

	carry_flag = new_carry;

	if( value == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if( (value & 0x80) == 0x80 )
		sign_flag = 1;
	else
		sign_flag = 0;

	switch ( opcode ) {
		case 0x6a:
			a_reg = value;

			tick_count += 2;
			pc_reg     += 1;
			break;
		case 0x66:
			WriteMem( arg1, value );

			tick_count += 5;
			pc_reg     += 2;
			break;

		case 0x76:
			WriteMemX( arg1, value );

			tick_count += 6;
			pc_reg     += 2;
			break;

		case 0x6e:
			WriteMem( MakeAddress( arg1, arg2), value );

			tick_count += 6;
			pc_reg     += 3;
			break;

		case 0x7e:
			WriteMemX( MakeAddress( arg1, arg2), value );

			tick_count += 7;
			pc_reg     += 3;
			break;

		default:
			break;
	}

}

void Cpu::OpcodeRTI() {
	PullStat();
	pc_reg       = Pull16();
	tick_count  += 6;
}

void Cpu::OpcodeRTS() {
	pc_reg      = Pull16();
	tick_count += 6;
	pc_reg     += 1;
}

void Cpu::OpcodeSBC() {
	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );
	byte arg2 = ReadMem( (ushort) (pc_reg + 2) );

	byte value = 0x00;
	switch ( opcode ) {
		case 0xe9:
			value = arg1;

			pc_reg     += 2;
			tick_count += 2;
			break;
		case 0xe5:
			value = ReadMem( arg1 );

			pc_reg     += 2;
			tick_count += 3;
			break;

		case 0xf5:
			value = ReadMemX( arg1, 0 );

			pc_reg     += 2;
			tick_count += 4;
			break;

		case 0xeD:
			value = ReadMem( MakeAddress( arg1, arg2 ) );

			pc_reg     += 3;
			tick_count += 4;
			break;

		case 0xfD:
			value = ReadMemX( MakeAddress( arg1, arg2 ), 1 );

			pc_reg     += 3;
			tick_count += 4;
			break;

		case 0xf9:
			value = ReadMemY( MakeAddress( arg1, arg2 ), 1 );

			pc_reg     += 3;
			tick_count += 4;
			break;

		case 0xe1:
			value = ReadMemIndirectX( arg1 );

			pc_reg     += 2;
			tick_count += 6;
			break;

		case 0xf1:
			value = ReadMemIndirectY( arg1, 0 );

			pc_reg     += 2;
			tick_count += 5;
			break;

		default:
			break;
	}

	uint value32 = (uint)( a_reg - value );
	if( carry_flag == 0 )
		value32--;

	if( value32 > 0xFF ) {
		carry_flag    = 0;
		overflow_flag = 1;
	} else {
		carry_flag    = 1;
		overflow_flag = 0;
	}

	if( (value32 & 0xff ) == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;


	if( (value32 & 0x80) == 0x80 )
		sign_flag = 1;
	else
		sign_flag = 0;

	a_reg = (byte)( value32 & 0xFF );
}

void Cpu::OpcodeSEC() {
	// Set carry
	carry_flag   = 1;
	tick_count  += 2;
	pc_reg      += 1;
}

void Cpu::OpcodeSED() {
	// Set decimal
	decimal_flag  = 1;
	tick_count   += 2;
	pc_reg       += 1;
}

void Cpu::OpcodeSEI() {
	// Set decimal
	interrupt_flag  = 1;
	tick_count     += 2;
	pc_reg         += 1;
}

void Cpu::OpcodeSTA() {
	// Store a_reg

	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );
	byte arg2 = ReadMem( (ushort) (pc_reg + 2) );

	switch ( opcode ) {
		case 0x85:
			WriteMem( arg1, a_reg );

			pc_reg     += 2;
			tick_count += 3;
			break;

		case 0x95:
			WriteMemX( arg1, a_reg );

			pc_reg     += 2;
			tick_count += 4;
			break;

		case 0x8D:
			WriteMem( MakeAddress( arg1, arg2), a_reg );

			pc_reg     += 3;
			tick_count += 4;
			break;

		case 0x9D:
			WriteMemX( MakeAddress( arg1, arg2 ), a_reg );

			pc_reg     += 3;
			tick_count += 5;
			break;

		case 0x99:
			WriteMemY( MakeAddress( arg1, arg2 ), a_reg );

			pc_reg     += 3;
			tick_count += 5;
			break;

		case 0x81:
			WriteMemIndirectX( arg1, a_reg );

			pc_reg     += 2;
			tick_count += 6;
			break;

		case 0x91:
			WriteMemIndirectY( arg1, a_reg );

			pc_reg     += 2;
			tick_count += 6;
			break;

		default:
			break;
	}

}

void Cpu::OpcodeSTX() {
	// Store x_reg

	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );
	byte arg2 = ReadMem( (ushort) (pc_reg + 2) );

	switch ( opcode ) {
		case 0x86:
			WriteMem( arg1, x_reg );

			pc_reg     += 2;
			tick_count += 3;
			break;

		case 0x96:
			WriteMemY( arg1, x_reg );

			pc_reg     += 2;
			tick_count += 4;
			break;

		case 0x8e:
			WriteMem( MakeAddress( arg1, arg2), x_reg );

			pc_reg     += 3;
			tick_count += 4;
			break;
		default:
			break;
	}
}

void Cpu::OpcodeSTY() {
	// Store y_reg

	byte arg1 = ReadMem( (ushort) (pc_reg + 1) );
	byte arg2 = ReadMem( (ushort) (pc_reg + 2) );

	switch ( opcode ) {
		case 0x84:
			WriteMem( arg1, y_reg );

			pc_reg     += 2;
			tick_count += 3;
			break;

		case 0x94:
			WriteMemX( arg1, y_reg );

			pc_reg     += 2;
			tick_count += 4;
			break;

		case 0x8c:
			WriteMem( MakeAddress( arg1, arg2), y_reg );

			pc_reg     += 3;
			tick_count += 4;
			break;
		default:
			break;
	}
}

void Cpu::OpcodeTAX() {
	x_reg = a_reg;

	pc_reg      += 1;
	tick_count  += 2;

	if( x_reg == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if( (x_reg & 0x80) == 0x80 )
		sign_flag = 1;
	else
		sign_flag = 0;

}

void Cpu::OpcodeTAY() {
	y_reg = a_reg;

	pc_reg      += 1;
	tick_count  += 2;

	if( y_reg == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if( (y_reg & 0x80) == 0x80 )
		sign_flag = 1;
	else
		sign_flag = 0;
}

void Cpu::OpcodeTSX() {
	x_reg = sp_reg;

	pc_reg      += 1;
	tick_count  += 2;

	if( x_reg == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if( (x_reg & 0x80) == 0x80 )
		sign_flag = 1;
	else
		sign_flag = 0;
}

void Cpu::OpcodeTXA() {
	a_reg = x_reg;

	pc_reg      += 1;
	tick_count  += 2;

	if( a_reg == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if( (a_reg & 0x80) == 0x80 )
		sign_flag = 1;
	else
		sign_flag = 0;

}

void Cpu::OpcodeTXS() {
	sp_reg = x_reg;

	pc_reg      += 1;
	tick_count  += 2;
}

void Cpu::OpcodeTYA() {
	a_reg = y_reg;

	pc_reg      += 1;
	tick_count  += 2;

	if( a_reg == 0 )
		zero_flag = 1;
	else
		zero_flag = 0;

	if( (a_reg & 0x80) == 0x80 )
		sign_flag = 1;
	else
		sign_flag = 0;
}
