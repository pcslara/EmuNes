/*
 * Cartridge.h
 *
 *  Created on: 06/05/2010
 *      Author: pcslara
 */

#ifndef CARTRIDGE_H_
#define CARTRIDGE_H_

#include <string>
#include "nestypes.h"

class Nes;

#define VERTICAL_MIRRORING       0
#define HORIZONTAL_MIRRORING     1
#define FOURSCREEN_MIRRORING     2
#define SINGLESCREEN_MIRRORING   3
#define SINGLESCREEN_MIRRORING2  4
#define SINGLESCREEN_MIRRORING3  5
#define SINGLESCREEN_MIRRORING4  6
#define CHRROM_MIRRORING         7

struct PRG_Rom {
	byte v[0x1000]; // 4KB
};

struct CHR_Rom {
	byte v[0x400]; // 1KB
};


class Cartridge {
private:

	byte prg_rom_count;
	byte chr_rom_count;
	byte mapper;

	byte mirroring;

	bool vram;
	bool trainer;
	bool save_ram;
	bool four_screan;

public:
	PRG_Rom * prg_rom;
	CHR_Rom * chr_rom;


	Cartridge( char file_name[256] );
	ushort GetPrgRomCount();
	ushort GetChrRomCount();
	byte   GetMirroring();
	byte   GetMapperNumber();
	bool   isVRAM();
};



#endif /* CARTRIDGE_H_ */
