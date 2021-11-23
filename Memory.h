/*
 * Memory
 *
 *  Created on: 02/05/2010
 *      Author: pcslara
 */

#ifndef MEMORY_
#define MEMORY_

#include <string.h>
#include "nestypes.h"

class Memory {
private:
	byte __mem[0x10000];

public:

	Memory() {}
	Memory( const byte mem[0x10000] ){ memcpy(__mem, mem, 0x10000); }


	byte Read( ushort addr ) {
		if( addr <= 0xffff )
			return __mem[addr];
	}

	void Write( ushort addr, byte value ) {
		if( addr <= 0xffff )
			__mem[addr] = value;
	}

};

#endif /* MEMORY_ */
