#include "Nes.h"
#include "Cpu.h"
#include "PPU.h"
#include "Cartridge.h"
#include "Mapper.h"
#include "Mappers/Mapper002.h"
#include "Mappers/Mapper004.h"

Nes::Nes( char filename[256] )  {

	cart = new Cartridge( filename );
	if( cart->GetMapperNumber() == 2 )
		mapper = new Mapper002( this );
	else if(cart->GetMapperNumber() == 4 ) {
		mapper = new Mapper004( this );
	} else {
		printf("Mapper %d not implemented\n", cart->GetMapperNumber() );
		exit( 1 );
	}
	ppu  = new PPU( this );
	cpu  = new Cpu( this );
}

Cpu * Nes::getCpu() {
	return cpu;
}

PPU * Nes::getPPU() {
	return ppu;
}

Cartridge * Nes::getCart() {
	return cart;
}

Mapper * Nes::getMapper() {
	return mapper;
}

void Nes::WriteMem8( unsigned short addr, byte value ) {
	if( addr < 0x2000 ) {
		if( addr < 0x800 )
			RAM[0][addr] = value;
		else if( addr < 0x1000 )
			RAM[1][addr - 0x800] = value;
		else if( addr < 0x1800 )
			RAM[2][addr - 0x1000] = value;
		else
			RAM[3][addr - 0x1800] = value ;
	} else if (addr < 0x4000) {

		switch ( addr ) {
			case 0x2000:
				ppu->WriteControlReg1( value );
				break;
			case 0x2001:
				ppu->WriteControlReg2( value );
				break;
			case 0x2003:
				ppu->WriteSprAddr( value );
				break;
			case 0x2004:
				ppu->WriteSprMem( value );
				break;
			case 0x2005:
				ppu->WriteScrollReg( value );
				break;
			case 0x2006:
				ppu->WriteVramAddr( value );
				break;
			case 0x2007:
				ppu->WriteVramIO( value );
				break;
			default:
				break;
		}
	} else if( addr < 0x6000 ) {
		switch (addr) {
			case (0x4014): ppu->SprRamDMABegin(value); break;

		}

	} else if( addr < 0x8000 ) {

	} else {
		mapper->WritePrgRom( addr, value );

	}

}

byte Nes::ReadMem8( ushort addr ) {
	byte ret = 0;

	if( addr < 0x2000 ) {
		if( addr < 0x800 )
			ret = RAM[0][addr];
		else if( addr < 0x1000 )
			ret = RAM[1][addr - 0x800];
		else if( addr < 0x1800 )
			ret = RAM[2][addr - 0x1000];
		else
			ret = RAM[3][addr - 0x1800];
	} else if( addr < 0x6000 ) {

		switch ( addr ) {
			case 0x2002:
				ret = ppu->ReadStatReg();
				break;
			case 0x2004:
				ret = ppu->ReadSprMem();
				break;
			case 0x2007:
				ret = ppu->ReadVramIO();
				break;
			case 0x4016:
				// returnvalue = myJoypad.Joypad_1_Read();
				break;
			case 0x4017:
				// returnvalue = myJoypad.Joypad_2_Read();
				break;
		}
	} else {
		// printf("%x\n", addr );
		ret = mapper->ReadPrgRom( addr );
		// printf("ret %x\n", ret );
	}
	return ret;
}

void Nes::Run() {
	cpu->Run();
}
