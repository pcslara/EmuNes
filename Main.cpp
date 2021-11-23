/*
 * Main.cpp
 *
 *  Created on: 02/05/2010
 *      Author: pcslara
 */
#include <string>
#include <allegro.h>
#include <stdio.h>
#include "Nes.h"
#include "PPU.h"
#include "FileReader.h"
#include "Memory.h"
#include "Cpu.h"
#include "nestypes.h"
#include "Cartridge.h"

#define MON 0

void CloseHandler() { exit(0); }

#if MON
int main( int argc, char ** argv ) {

	if( argc != 2 ) {
		printf("Error on arg len\n");
		return 1;
	}

	FileReader reader( argv[1] );
	Memory mem = reader.GetMemory();
	Cpu cpu6502 ( mem );

	cpu6502.Run();


	return 0;
}
#else

using std::string;

int main( int argc, char ** argv ) {
	if( argc != 2 ) {
		printf("Error on arg len\n");
		return 1;
	}

	Nes * nes = new Nes( argv[1] );
	LOCK_FUNCTION(CloseHandler);
	set_close_button_callback(CloseHandler);
	nes->Run();

	return 0;

}
END_OF_MAIN();
#endif
