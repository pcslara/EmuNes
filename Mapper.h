/*
 * Mapper.h
 *
 *  Created on: 07/05/2010
 *      Author: pcslara
 */

#ifndef MAPPER_H_
#define MAPPER_H_

/**
 * Base Class to NES Mapper (MMU)
 */
#include "nestypes.h"

class Nes;

class Mapper {
protected:


	Nes * nes;

	byte curr_prg_rom[8];
	byte curr_chr_rom[8];



public:
	Mapper(Nes * nes);

	virtual void WriteChrRom( ushort addr, byte value );
	virtual void WritePrgRom( ushort addr, byte value );

	byte ReadChrRom( ushort addr );
	byte ReadPrgRom( ushort addr );


	void SelectPrgRom16( ushort bank );
	void SelectChrRom( byte area, byte start, byte kbytes );
	void SelectPrgRom( byte area, byte start, byte kbytes );
	void Switch16kPrgRom(int start, int area);
	void Switch8kPrgRom(int start, int area);
};


#endif /* MAPPER_H_ */
