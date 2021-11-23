/*
 * Nes.h
 *
 *  Created on: 12/05/2010
 *      Author: pcslara
 */

#ifndef NES_H_
#define NES_H_

#include "nestypes.h"

class Cpu;
class PPU;
class Cartridge;
class Mapper;


class Nes {
private:
	Cpu * cpu;
	PPU * ppu;
	Cartridge * cart;
	Mapper * mapper;
	byte RAM[4][0x800];
public:

	Nes( char filename[256] );

	void WriteMem8( ushort addr, byte value );
	byte ReadMem8 ( ushort addr );

	Cpu * getCpu();
	PPU * getPPU();
	Cartridge * getCart();
	Mapper * getMapper();

	void Run();
};

#endif /* NES_H_ */
