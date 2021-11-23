/*
 * Mapper004.h
 *
 *  Created on: 10/05/2010
 *      Author: pcslara
 */

#ifndef MAPPER004_H_
#define MAPPER004_H_

#include "../Mapper.h"
/**
 *
 Comprehensive NES Mapper Document v0.80 by \Firebug\ (emulation@biosys.net)
 Best viewed under DOS EDIT
 Information provided by FanWen, Y0SHi, D, and Jim Geffre
 +----------------+
 ¦ Mapper 4: MMC3 ¦
 +----------------+

 +--------------------------------------------------------------------+
 ¦ A great majority of newer NES games (early 90's) use this mapper,  ¦
 ¦ both U.S. and Japanese. Among the better-known MMC3 titles are     ¦
 ¦ Super Mario Bros. 2 and 3, MegaMan 3, 4, 5, and 6, and Crystalis.  ¦
 +--------------------------------------------------------------------+

 +-------+   +------------------------------------------------------+
 ¦ $8000 +---¦ CPxxxNNN                                             ¦
 +-------+   ¦ ¦¦   +-+                                             ¦
             ¦ ¦¦    +--- Command Number                            ¦
             ¦ ¦¦          0 - Select 2 1K VROM pages at PPU $0000  ¦
             ¦ ¦¦          1 - Select 2 1K VROM pages at PPU $0800  ¦
             ¦ ¦¦          2 - Select 1K VROM page at PPU $1000     ¦
             ¦ ¦¦          3 - Select 1K VROM page at PPU $1400     ¦
             ¦ ¦¦          4 - Select 1K VROM page at PPU $1800     ¦
             ¦ ¦¦          5 - Select 1K VROM page at PPU $1C00     ¦
             ¦ ¦¦          6 - Select first switchable ROM page     ¦
             ¦ ¦¦          7 - Select second switchable ROM page    ¦
             ¦ ¦¦                                                   ¦
             ¦ ¦+-------- PRG Address Select                        ¦
             ¦ ¦           0 - Enable swapping for $8000 and $A000  ¦
             ¦ ¦           1 - Enable swapping for $A000 and $C000  ¦
             ¦ ¦                                                    ¦
             ¦ +--------- CHR Address Select                        ¦
             ¦             0 - Use normal address for commands 0-5  ¦
             ¦             1 - XOR command 0-5 address with $1000   ¦
             +------------------------------------------------------+

 +-------+   +----------------------------------------------+
 ¦ $8001 +---¦ PPPPPPPP                                     ¦
 +-------+   ¦ +------+                                     ¦
             ¦    ¦                                         ¦
             ¦    ¦                                         ¦
             ¦    +------- Page Number for Command          ¦
             ¦              Activates the command number    ¦
             ¦              written to bits 0-2 of $8000    ¦
             +----------------------------------------------+

 +-------+   +----------------------------------------------+
 ¦ $A000 +---¦ xxxxxxxM                                     ¦
 +-------+   ¦        ¦                                     ¦
             ¦        ¦                                     ¦
             ¦        ¦                                     ¦
             ¦        +--- Mirroring Select                 ¦
             ¦              0 - Horizontal mirroring        ¦
             ¦              1 - Vertical mirroring          ¦
             ¦ NOTE: I don't have any confidence in the     ¦
             ¦       accuracy of this information.          ¦
             +----------------------------------------------+

 +-------+   +----------------------------------------------+
 ¦ $A001 +---¦ Sxxxxxxx                                     ¦
 +-------+   ¦ ¦                                            ¦
             ¦ ¦                                            ¦
             ¦ ¦                                            ¦
             ¦ +---------- SaveRAM Toggle                   ¦
             ¦              0 - Disable $6000-$7FFF         ¦
             ¦              1 - Enable $6000-$7FFF          ¦
             +----------------------------------------------+

 +-------+   +----------------------------------------------+
 ¦ $C000 +---¦ IIIIIIII                                     ¦
 +-------+   ¦ +------+                                     ¦
             ¦    ¦                                         ¦
             ¦    ¦                                         ¦
             ¦    +------- IRQ Counter Register             ¦
             ¦              The IRQ countdown value is      ¦
             ¦              stored here.                    ¦
             +----------------------------------------------+

 +-------+   +----------------------------------------------+
 ¦ $C001 +---¦ IIIIIIII                                     ¦
 +-------+   ¦ +------+                                     ¦
             ¦    ¦                                         ¦
             ¦    ¦                                         ¦
             ¦    +------- IRQ Latch Register               ¦
             ¦              A temporary value is stored     ¦
             ¦              here.                           ¦
             +----------------------------------------------+

 +-------+   +----------------------------------------------+
 ¦ $E000 +---¦ xxxxxxxx                                     ¦
 +-------+   ¦ +------+                                     ¦
             ¦    ¦                                         ¦
             ¦    ¦                                         ¦
             ¦    +------- IRQ Control Register 0           ¦
             ¦              Any value written here will     ¦
             ¦              disable IRQ's and copy the      ¦
             ¦              latch register to the actual    ¦
             ¦              IRQ counter register.           ¦
             +----------------------------------------------+

 +-------+   +----------------------------------------------+
 ¦ $E001 +---¦ xxxxxxxx                                     ¦
 +-------+   ¦ +------+                                     ¦
             ¦    ¦                                         ¦
             ¦    ¦                                         ¦
             ¦    +------- IRQ Control Register 1           ¦
             ¦              Any value written here will     ¦
             ¦              enable IRQ's.                   ¦
             +----------------------------------------------+

 Notes: - Two of the 8K ROM banks in the PRG area are switchable.
           The other two are "hard-wired" to the last two banks in
           the cart. The default setting is switchable banks at
           $8000 and $A000, with banks 0 and 1 being swapped in
           at reset. Through bit 6 of $8000, the hard-wiring can
           be made to affect $8000 and $E000 instead of $C000 and
           $E000. The switchable banks, whatever their addresses,
           can be swapped through commands 6 and 7.
        - A cart will first write the command and base select number
           to $8000, then the value to be used to $8001.
        - On carts with VROM, the first 8K of VROM is swapped into
           PPU $0000 on reset. On carts without VROM, as always, there
           is 8K of VRAM at PPU $0000.
 *
 */
class Mapper004 : public Mapper {
private:

	byte commandNumber;  // 3 bits
	byte prgAddr;        // 1
	byte chrAddr;        //
public:
	Mapper004(Nes * nes) : Mapper( nes ){
		prgAddr = 0;
		chrAddr = 0;
		Switch16kPrgRom((nes->getCart()->GetChrRomCount() - 1) * 4, 1);
	}
	virtual void WritePrgRom( ushort addr, byte value ) {
		if( addr == 0x8000 ) {
			commandNumber = value & 0x7;
			prgAddr       = (value >> 6) & 0x1;
			chrAddr       = (value >> 7) & 0x1;
		} else if( addr == 0x8001 ) {
			printf("command = %d\n", commandNumber);
			if( commandNumber == 0 ) {
				value = (byte)(value - (value % 2));
				if( chrAddr == 0 )
					SelectChrRom(value, 0, 2);  // Select 2KB at 0x0000 in PPU
				else
					SelectChrRom(value, 2, 2);    // Select 2KB at 0x0800 in PPU
			} else if(commandNumber == 1  ) {
				value = (byte)(value - (value % 2));
				if( chrAddr == 0 )
					SelectChrRom(value, 1, 2);  // Select 2KB
				else
					SelectChrRom(value, 3, 2);    // Select 2KB
			} else if( commandNumber == 2 ) {
				value = (byte)(value & (nes->getCart()->GetChrRomCount()* 8 - 1));
				if( chrAddr == 0 )
					SelectChrRom(value, 4, 1);  // Select 2KB at 0x0000 in PPU
				else
					SelectChrRom(value, 0, 1);    // Select 2KB at 0x0800 in PPU
			} else if( commandNumber == 3 ) {
					if( chrAddr == 0 )
						SelectChrRom(value, 5, 1);  // Select 1KB at 0x0000 in PPU
					else
						SelectChrRom(value, 1, 1);    // Select 1KB at 0x0800 in PPU
			} else if( commandNumber == 4 ) {
				if( chrAddr == 0 )
					SelectChrRom(value, 6, 1);  // Select 2KB at 0x0000 in PPU
				else
					SelectChrRom(value, 2, 1);    // Select 2KB at 0x0800 in PPU
			} else if( commandNumber == 5 ) {
				if( chrAddr == 0 )
					SelectChrRom(value, 7, 1);  // Select 2KB at 0x0000 in PPU
				else
					SelectChrRom(value, 3, 1);    // Select 2KB at 0x0800 in PPU
			} else if( commandNumber == 6 ) {
				if (prgAddr == 0) {
					Switch8kPrgRom(value * 2, 0);
					Switch8kPrgRom((nes->getCart()->GetChrRomCount()* 4) - 4, 2);
				} else	{
					Switch8kPrgRom(value * 2, 2);
					Switch8kPrgRom((nes->getCart()->GetChrRomCount()*4) - 4, 0);
				}

			} else if( commandNumber == 7 ) {
				Switch8kPrgRom(value * 2, 1);
				if (prgAddr == 0)
					Switch8kPrgRom((nes->getCart()->GetChrRomCount() * 4) - 4, 2);
				else
					Switch8kPrgRom((nes->getCart()->GetChrRomCount() * 4) - 4, 0);
			}

		} else {
			printf("addr : %x value : %x\n", addr, value );
		}



	}


};

#endif /* MAPPER004_H_ */
