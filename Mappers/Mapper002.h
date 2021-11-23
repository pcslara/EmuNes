/*
 * Mapper002.h
 *
 *  Created on: 12/05/2010
 *      Author: pcslara
 */

#ifndef MAPPER002_H_
#define MAPPER002_H_

#include "../Mapper.h"
#include "../nestypes.h"

class Mapper002 : public Mapper {

public:
	Mapper002(Nes * nes) : Mapper( nes ){
		//

		Switch16kPrgRom ((nes->getCart()->GetPrgRomCount() - 1) * 4, 1);
		for(int i = 0; i < 8; i++ ) {
			curr_chr_rom[i] = i;
			//curr_prg_rom[i] = i;
		}
	}

	void WritePrgRom( ushort addr, byte value ) {
		// printf("WPR ADDR %x val %x\n", addr, value );
		if( addr >= 0x8000 ) {
			Switch16kPrgRom( 4*value, 0 );
		} else {
			Mapper::WritePrgRom( addr, value );
		}
	}

};


#endif /* MAPPER002_H_ */
