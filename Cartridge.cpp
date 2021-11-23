/*
 * Cartridge.cpp
 *
 *  Created on: 06/05/2010
 *      Author: pcslara
 */

#include <string>
#include <stdlib.h>
#include <stdio.h>

#include "Cartridge.h"
#include "nestypes.h"
using std::string;

Cartridge::Cartridge( char file_name[256] ) {
/*                        Nes Header
        Extract from http://nesdev.parodius.com/NinTech.txt
  	+--------+------+------------------------------------------+
	|   0    |  3   | 'NES'                                    |
	|   3    |  1   | $1A                                      |
	|   4    |  1   | 16K PRG-ROM page count                   |
	|   5    |  1   | 8K  CHR-ROM page count                   |
	|   6    |  1   | ROM Control Byte #1                      |
	|        |      |   %####vTsM                              |
	|        |      |    |  ||||+- 0 = Horizontal Mirroring    |
	|        |      |    |  ||||   1 = Vertical Mirroring      |
	|        |      |    |  |||+-- 1 = SRAM enabled            |
	|        |      |    |  ||+--- 1 = Trainer *1              |
	|        |      |    |  |+---- 1 = Four-screen VRAM layout |
	|        |      |    |  |                                  |
	|        |      |    +--+----- Mapper # (lower 4-bits)     |
	|        |      |                                          |
	|   7    |  1   | ROM Control Byte #2                      |
	|        |      |   %####0000                              |
	|        |      |    |  |                                  |
	|        |      |    +--+----- Mapper # (upper 4-bits)     |
	+--------+------+------------------------------------------+
	|   8    |  8   | Reserved, All filled with 0's.           |
	+--------+------+------------------------------------------+
*/
	byte header[16];
	int i;
	FILE * file = fopen( file_name,"rb" );

	if( file == NULL ) {
		fprintf( stderr, "Unable to open file ROM.\n");
		exit( 1 );
	}

	fread( header, sizeof( byte ), 16, file );

	if( header[0] != 'N' || header[1] != 'E' || header[2] != 'S' || header[3] != 0x1a ) {
		fprintf( stderr, "Unknow file ROM.\n");
		exit( 1 );
	}

	prg_rom_count = header[4];     // 16 kB Pages
	chr_rom_count = header[5];     // 8  kB Pages

	printf("PRG_ROM_COUNT : %d pages\n", prg_rom_count );
	printf("CHR_ROM_COUNT : %d pages\n", chr_rom_count );

	mirroring   = ( header[6] &  1 ) != 0;
	save_ram    = ( header[6] &  2 ) != 0;
	trainer     = ( header[6] &  4 ) != 0;
	four_screan = ( header[6] &  8 ) != 0;
	mapper      = ( header[6] >> 4 ) | ( header[7] & 0xF0 );

	// printf("Mapper : %X\n", mapper );


	prg_rom = new PRG_Rom[prg_rom_count*4];

	// Read Program ROM
	for( i = 0; i < prg_rom_count * 4; i++ )               // NO Using 4kB instead 16kB
		fread( prg_rom[i].v, sizeof( byte ), 0x1000, file );

	if( chr_rom_count != 0 ) {
		// Read Character ROM
		chr_rom = new CHR_Rom[chr_rom_count * 8];
		for( i = 0; i < chr_rom_count * 8; i++ )           // NO Using 1kB instead 8kB
			fread( chr_rom[i].v, sizeof( byte ), 0x400, file );
	} else {
		// Using VRAM
		chr_rom = new CHR_Rom[8];
		vram = true;
	}
}

ushort Cartridge::GetPrgRomCount() {
	return prg_rom_count;
}

ushort Cartridge::GetChrRomCount() {
	return chr_rom_count;
}

bool Cartridge::isVRAM() {
	return vram;
}

byte Cartridge::GetMirroring() {
	if( four_screan )
		return FOURSCREEN_MIRRORING;
	else if( mirroring )
		return VERTICAL_MIRRORING;
	else
		return HORIZONTAL_MIRRORING;
}

byte Cartridge::GetMapperNumber() {
	return mapper;
}
