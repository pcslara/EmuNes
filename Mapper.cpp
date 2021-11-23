/*
 * Mapper.cpp
 *
 *  Created on: 12/05/2010
 *      Author: pcslara
 */
#include <stdio.h>
#include "Mapper.h"
#include "Cartridge.h"
#include "Nes.h"
#include "nestypes.h"

Mapper::Mapper( Nes * nes ) : nes( nes ) {}

byte Mapper::ReadPrgRom( ushort addr ) {
	if( addr == 0xb7ca ) {
		// printf("cap %d\n", curr_prg_rom[0] );
	}

	byte ret = 0;

	if (addr < 0x9000)
		ret = nes->getCart()->prg_rom[curr_prg_rom[0]].v[addr - 0x8000];
	else if (addr < 0xA000)
		ret = nes->getCart()->prg_rom[curr_prg_rom[1]].v[addr - 0x9000];
	else if (addr < 0xB000)
		ret = nes->getCart()->prg_rom[curr_prg_rom[2]].v[addr - 0xA000];
	else if (addr < 0xC000)
		ret = nes->getCart()->prg_rom[curr_prg_rom[3]].v[addr - 0xB000];
	else if (addr < 0xD000)
		ret = nes->getCart()->prg_rom[curr_prg_rom[4]].v[addr - 0xC000];
	else if (addr < 0xE000)
		ret = nes->getCart()->prg_rom[curr_prg_rom[5]].v[addr - 0xD000];
	else if (addr < 0xF000)
		ret = nes->getCart()->prg_rom[curr_prg_rom[6]].v[addr - 0xE000];
	else
		ret = nes->getCart()->prg_rom[curr_prg_rom[7]].v[addr - 0xF000];

	return ret;
}


byte Mapper::ReadChrRom( ushort addr ) {
	if( addr < 0x400 ) {
		return nes->getCart()->chr_rom[curr_chr_rom[0]].v[addr];
	} else if( addr < 0x800 ) {
		return nes->getCart()->chr_rom[curr_chr_rom[1]].v[addr - 0x400];
	} else if( addr < 0xC00 ) {
		return nes->getCart()->chr_rom[curr_chr_rom[2]].v[addr - 0x800];
	} else if( addr < 0x1000 ) {
		return nes->getCart()->chr_rom[curr_chr_rom[3]].v[addr - 0xC00];
	} else if( addr < 0x1400 ) {
		return nes->getCart()->chr_rom[curr_chr_rom[4]].v[addr - 0x1000];
	} else if( addr < 0x1800 ) {
		return nes->getCart()->chr_rom[curr_chr_rom[5]].v[addr - 0x1400];
	} else if( addr < 0x1C00 ) {
		return nes->getCart()->chr_rom[curr_chr_rom[6]].v[addr - 0x1800];
	} else {
		return nes->getCart()->chr_rom[7].v[addr - 0x1C00];
	}
}

void Mapper::WritePrgRom( ushort addr, byte value ) { printf("Chamou!\n"); }

void Mapper::WriteChrRom(  ushort addr, byte value ) {

	if( nes->getCart()->isVRAM() ) {
		if( addr < 0x400 ) {
			nes->getCart()->chr_rom[0].v[addr]          = value;
		} else if( addr < 0x800 ) {
			nes->getCart()->chr_rom[1].v[addr - 0x400]  = value;
		} else if( addr < 0xC00 ) {
			nes->getCart()->chr_rom[2].v[addr - 0x800]  = value;
		} else if( addr < 0x1000 ) {
			nes->getCart()->chr_rom[3].v[addr - 0xC00]  = value;
		} else if( addr < 0x1400 ) {
			nes->getCart()->chr_rom[4].v[addr - 0x1000] = value;
		} else if( addr < 0x1800 ) {
			nes->getCart()->chr_rom[5].v[addr - 0x1400] = value;
		} else if( addr < 0x1C00 ) {
			nes->getCart()->chr_rom[6].v[addr - 0x1800] = value;
		} else {
			nes->getCart()->chr_rom[7].v[addr - 0x1C00] = value;
		}
	}
}
void Mapper::SelectPrgRom16( ushort bank ) {
	for( int i = 0; i < 4; i++ ) {
		curr_prg_rom[i] = 4*bank  + i;
	}
	//printf("bank : %d\n", bank );
}

void Mapper::SelectChrRom(byte start, byte area, byte kbytes ) {
	// area  = area -  (area % 2); // not be odd
	switch( nes->getCart()->GetChrRomCount() ) {
		case (2): start = (start & 0xf); break;
		case (4): start = (start & 0x1f); break;
		case (8): start = (start & 0x3f); break;
		case (16): start = (start & 0x7f); break;
		case (32): start = (start & 0xff); break;
	}
	for(int i = 0; i < kbytes && i < 8; i++ ) {
		curr_chr_rom[kbytes*area+i] = start + i;
	}
}

void Mapper::SelectPrgRom(byte start, byte area, byte kbytes ) {
	// area  = area -  (area % 2); // not be odd
	switch( nes->getCart()->GetChrRomCount() ) {
		case (2): start = (start & 0xf); break;
		case (4): start = (start & 0x1f); break;
		case (8): start = (start & 0x3f); break;
		case (16): start = (start & 0x7f); break;
		case (32): start = (start & 0xff); break;
	}
	for(int i = 0; i < kbytes && i < 8; i++ ) {
		curr_chr_rom[kbytes*area+i] = start + i;
	}
}


void Mapper::Switch16kPrgRom(int start, int area) {
		int i;
		switch (nes->getCart()->GetPrgRomCount())
		{
			case (2): start = (start & 0x7); break;
			case (4): start = (start & 0xf); break;
			case (8): start = (start & 0x1f); break;
			case (16): start = (start & 0x3f); break;
			case (32): start = (start & 0x7f); break;
		}

		for (i = 0; i < 4; i++)
		{
			curr_prg_rom[4*area + i] = (uint)(start + i);
		}

}

void Mapper::Switch8kPrgRom(int start, int area)
	{
		int i;
		switch (nes->getCart()->GetPrgRomCount()) {
			case (2): start = (start & 0x7); break;
			case (4): start = (start & 0xf); break;
			case (8): start = (start & 0x1f); break;
			case (16): start = (start & 0x3f); break;
			case (32): start = (start & 0x7f); break;
		}
		for (i = 0; i < 2; i++)
		{
			curr_prg_rom[2*area + i] = (uint)(start + i);
		}
}


