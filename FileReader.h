/*
 * FileReader.h
 *
 *  Created on: 02/05/2010
 *      Author: pcslara
 */

#ifndef FILEREADER_H_
#define FILEREADER_H_

#include <stdio.h>
#include "Memory.h"
#include "nestypes.h"

class FileReader {
private:
	FILE * file;
public:

	FileReader( const char file_name[255] ) {
		file = fopen( file_name, "r" );
		if( file == NULL )
			fprintf(stderr, "Error on open file!\n");
	}

	Memory GetMemory() {
		byte mem[0x10000];
		int i = 0;
		while( fread( &mem[i + 0x1000], sizeof( byte ), 1, file ) ) {
			i++;
		}
		mem[i] = 0x00;
		Memory ret( mem );
		return ret;
	}
};

#endif /* FILEREADER_H_ */
