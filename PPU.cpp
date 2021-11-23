/*
 * PPU.cpp
 *
 *  Created on: 08/05/2010
 *      Author: pcslara
 */
#include <stdio.h>
#include <sys/time.h>
//#include <stdlib.h>

#include "PPU.h"
#include "Mapper.h"
#include "Cartridge.h"
#include "Nes.h"
#include "Cpu.h"

/** GetClock **/
uint32_t clk() {
	struct timeval t;
	gettimeofday(&t, NULL );
	return 1000000*t.tv_sec + t.tv_usec;
}


PPU::PPU( Nes * nes ) : video( 256, 240 ), nes( nes ){

	//printf("Construct\n");
	currScanLine = 0;
	frames = 0;
	nTblAddress = 0x2000;
	vramHiLoToggle = 1;
	sprAddr = 0x0;
	scrollV = 0;
	scrollH = 0;

 uint  colors [] = { 0x7C7C7C, 0x0000FC, 0x0000BC, 0x4428BC, 0x940084, 0xA80020, 0xA81000, 0x881400, 0x503000, 0x007800, 0x006800, 0x005800, 0x004058, 0x000000, 0x000000, 0x000000, 0xBCBCBC, 0x0078F8, 0x0058F8, 0x6844FC, 0xD800CC, 0xE40058, 0xF83800, 0xE45C10, 0xAC7C00, 0x00B800, 0x00A800, 0x00A844, 0x008888, 0x000000, 0x000000, 0x000000, 0xF8F8F8, 0x3CBCFC, 0x6888FC, 0x9878F8, 0xF878F8, 0xF85898, 0xF87858, 0xFCA044, 0xF8B800, 0xB8F818, 0x58D854, 0x58F898, 0x00E8D8, 0x787878, 0x000000, 0x000000, 0xFCFCFC, 0xA4E4FC, 0xB8B8F8, 0xD8B8F8, 0xF8B8F8, 0xF8A4C0, 0xF0D0B0, 0xFCE0A8, 0xF8D878, 0xD8F878, 0xB8F8B8, 0xB8F8D8, 0x00FCFC, 0xF8D8F8, 0x000000, 0x000000};
	// uint  colors [] = { 0x525252, 0x000080, 0x08008A, 0x2C007E, 0x4A004E, 0x500006, 0x440000, 0x260800, 0x0A2000, 0x002E00, 0x003200, 0x00260A, 0x001C48, 0x000000, 0x000000, 0x000000, 0xA4A4A4, 0x0038CE, 0x3416EC, 0x5E04DC, 0x8C00B0, 0x9A004C, 0x901800, 0x703600, 0x4C5400, 0x0E6C00, 0x007400, 0x006C2C, 0x005E84, 0x000000, 0x000000, 0x000000, 0xFFFFFF, 0x4C9CFF, 0x7C78FF, 0xA664FF, 0xDA5AFF, 0xF054C0, 0xF06A56, 0xD68610, 0xBAA400, 0x76C000, 0x46CC1A, 0x2EC866, 0x34C2BE, 0x3A3A3A, 0x000000, 0x000000, 0xFFFFFF, 0xB6DAFF, 0xC8CAFF, 0xDAC2FF, 0xF0BEFF, 0xFCBCEE, 0xFAC2C0, 0xF2CCA2, 0xE6DA92, 0xCCE68E, 0xB8EEA2, 0xAEEABE, 0xAEE8E2, 0xB0B0B0, 0x000000, 0x000000 };
	for(int i = 0; i < 64; i++ )
		NesPalette[i] = makecol( 0xFF & (colors[i] >> 16), 0xFF & (colors[i] >> 8), 0xFF & colors[i] );
}

byte PPU::ReadControlReg1() {
	/**
	+---------+----------------------------------------------------------+
    | Address | Description                                              |
    +---------+----------------------------------------------------------+
    |  $2000  | PPU Control Register #1 (W)                              |
    |         |   %vMsbpiNN                                              |
    |         |               v = Execute NMI on VBlank                  |
    |         |                      1 = Enabled                         |
    |         |               M = PPU Selection (unused)                 |
    |         |                      0 = Master                          |
    |         |                      1 = Slave                           |
    |         |               s = Sprite Size                            |
    |         |                      0 = 8x8                             |
    |         |                      1 = 8x16                            |
    |         |               b = Background Pattern Table Address       |
    |         |                      0 = $0000 (VRAM)                    |
    |         |                      1 = $1000 (VRAM)                    |
    |         |               p = Sprite Pattern Table Address           |
    |         |                      0 = $0000 (VRAM)                    |
    |         |                      1 = $1000 (VRAM)                    |
    |         |               i = PPU Address Increment                  |
    |         |                      0 = Increment by 1                  |
    |         |                      1 = Increment by 32                 |
    |         |              NN = Name Table Address                     |
    |         |                     00 = $2000 (VRAM)                    |
    |         |                     01 = $2400 (VRAM)                    |
    |         |                     10 = $2800 (VRAM)                    |
    |         |                     11 = $2C00 (VRAM)                    |
    |         |                                                          |
    |         | NOTE: Bit #6 (M) has no use, as there is only one (1)    |
    |         |       PPU installed in all forms of the NES and Famicom. |
    +---------+----------------------------------------------------------+
	*/

	byte NN, i, p, b, s, M, v;

	switch( nTblAddress ) {
		case 0x2000: NN = 0x00; break;
		case 0x2400: NN = 0x01; break;
		case 0x2800: NN = 0x02; break;
		case 0x2C00: NN = 0x03; break;
		default: printf("Panic Error on ReadControlReg1\n");
				 exit( 1 );
	}

	i = (addrInc == 32) ?             1 : 0;
	p = (sprPatternTable == 0x1000) ? 1 : 0;
	b = (bgPatternTable  == 0x1000) ? 1 : 0;
	s = (sprSize == 16) ?             1 : 0;
	M = ppuSelection & 0x01;
	v = nmiOnVblank  & 0x01;

	return (byte)(( NN & 0x3) |  (i << 2) | (p << 3) | (b << 4) | (s << 5) | (M << 6) | (v << 7));
}

void PPU::WriteControlReg1( byte value ) {
	// printf("Write control\n");
	switch( value & 0x03 ) {
		case 0x0: nTblAddress = 0x2000; break;
		case 0x1: nTblAddress = 0x2400; break;
		case 0x2: nTblAddress = 0x2800; break;
		case 0x3: nTblAddress = 0x2C00; break;
		default:
			printf("Error\n"); break;

	}


	addrInc           = ((value >> 2) & 0x01) ? 32 : 1;
	sprPatternTable	  = ((value >> 3) & 0x01) ? 0x1000 : 0x0000;
	bgPatternTable    = ((value >> 4) & 0x01) ? 0x1000 : 0x0000;
	sprSize           = ((value >> 5) & 0x01) ? 16 : 8;
	ppuSelection      =  (value >> 6) & 0x01;
	nmiOnVblank       =  (value >> 7) & 0x01;
}

byte PPU::ReadControlReg2() {
	/*
	+---------+----------------------------------------------------------+
	|  $2001  | PPU Control Register #2 (W)                              |
	|         |   %fffpcsit                                              |
	|         |             fff = Full Background Colour                 |
	|         |                    000 = Black                           |
	|         |                    001 = Red                             |
	|         |                    010 = Blue                            |
	|         |                    100 = Green                           |
	|         |               p = Sprite Visibility                      |
	|         |                      1 = Display                         |
	|         |               c = Background Visibility                  |
	|         |                      1 = Display                         |
	|         |               s = Sprite Clipping                        |
	|         |                      0 = Sprites not displayed in left   |
	|         |                          8-pixel column                  |
	|         |                      1 = No clipping                     |
	|         |               i = Background Clipping                    |
	|         |                      0 = Background not displayed in     |
	|         |                          left 8-pixel column             |
	|         |                      1 = No clipping                     |
	|         |               t = Display Type                           |
	|         |                      0 = Colour display                  |
	|         |                      1 = Mono-type (B&W) display         |
	+---------+----------------------------------------------------------+
	*/
	return (byte)((dispType & 0x01) | ( noBgClipping << 1 ) | ( noSprClipping << 2) | (bgVisibility << 3) |
			(sprVisibility << 4) | (fullBgColor << 5) );
}
void PPU::WriteControlReg2( byte value ) {
	dispType       =  value & 0x01;
	noBgClipping   = (value >> 1) & 0x01;
	noSprClipping  = (value >> 2) & 0x01;
	bgVisibility   = (value >> 3) & 0x01;
	sprVisibility  = (value >> 4) & 0x01;
	fullBgColor    = (value >> 5) & 0x07;
}

byte PPU::ReadStatReg() {
	/*
	+---------+----------------------------------------------------------+
    |  $2002  | PPU Status Register (R)                                  |
    |         |   %vhsw----                                              |
    |         |               v = VBlank Occurance                       |
    |         |                      1 = In VBlank                       |
    |         |               h = Sprite #0 Occurance                    |
    |         |                      1 = VBlank has hit Sprite #0        |
    |         |               s = Scanline Sprite Count                  |
    |         |                      0 = Less than 8 sprites on the      |
    |         |                          current scanline                |
    |         |                      1 = More than 8 sprites on the      |
    |         |                          current scanline                |
    |         |               w = VRAM Write Flag                        |
    |         |                      1 = Writes to VRAM are ignored      |
    |         |                                                          |
    |         | NOTE: If read during HBlank and Bit #7 of $2000 is set   |
    |         |       to 0, then switch to Name Table #0.                |
    |         | NOTE: After a read occurs, $2005 is reset, hence the     |
    |         |       next write to $2005 will be Horizontal.            |
    |         | NOTE: Bit #6 (h) is set to 0 after each VBlank.          |
    |         | NOTE: Bit #6 (h) is not set until the first actual non-  |
    |         |       transparent pixel is drawn. Hence, if you have a   |
    |         |       8x8 sprite which has all pixels transparent except |
    |         |       for pixel #4 (horizontally), Bit #6 will be set    |
    |         |       after the 4th pixel is found & drawn.              |
    |         | NOTE: Bit #7 (v) is set to 0 after read occurs.          |
    +---------+----------------------------------------------------------+
	*/
	if( currScanLine >= 240 )
		vBlankOcc = 1;
	else
		vBlankOcc = 0;

	vramHiLoToggle = 1;
	return (byte) ((vRamWrites << 4) | (sprScanlineCount << 5) | (spr0Hit << 6) | (vBlankOcc << 7));
}

void PPU::WriteScrollReg( byte value ) {
	if( vramHiLoToggle == 1 ) {
		scrollV = value;
		vramHiLoToggle = 0;
	} else {
		scrollH = value;
		vramHiLoToggle = 1;
	}
}

void PPU::WriteVramAddr( byte value ) {
	if( vramHiLoToggle == 1 ) {
		prev_VRAMaddr = curr_VRAMaddr;
		curr_VRAMaddr = (value << 8);

		vramHiLoToggle = 0;
	} else {
		curr_VRAMaddr = curr_VRAMaddr | value;
		vramHiLoToggle = 1;
	}
}

byte PPU::ReadVramIO() {
	byte ret = 0;

	if (curr_VRAMaddr < 0x3f00) {
		ret = vramBuff;
		if (curr_VRAMaddr >= 0x2000)
			vramBuff = nameTables[curr_VRAMaddr - 0x2000];
		else if (curr_VRAMaddr < 0x2000)
			vramBuff = nes->getMapper()->ReadChrRom( (ushort)curr_VRAMaddr );
	} else {
		ret = nameTables[curr_VRAMaddr - 0x2000];
	}

	curr_VRAMaddr = curr_VRAMaddr + addrInc;

	return ret;
}

void PPU::WriteVramIO( byte value ) {

	if (curr_VRAMaddr < 0x2000 )
		nes->getMapper()->WriteChrRom( curr_VRAMaddr, value );
	else if ((curr_VRAMaddr >= 0x2000) && (curr_VRAMaddr < 0x3f00))	 {

		if (nes->getCart()->GetMirroring() == HORIZONTAL_MIRRORING )	{

			switch (curr_VRAMaddr & 0x2C00)
			{
				case 0x2000:
					nameTables[curr_VRAMaddr - 0x2000] = value;
					break;
				case 0x2400:
					nameTables[(curr_VRAMaddr - 0x400) - 0x2000] = value;
					break;
				case 0x2800:
					nameTables[curr_VRAMaddr - 0x400 - 0x2000] = value;
					break;
				case 0x2C00:
					nameTables[(curr_VRAMaddr - 0x800) - 0x2000] = value;
					break;
			}


		} else if (nes->getCart()->GetMirroring() == VERTICAL_MIRRORING ) {
			switch (curr_VRAMaddr & 0x2C00) {
				case 0x2000:
					nameTables[curr_VRAMaddr - 0x2000] = value;
					break;
				case 0x2400:
					nameTables[curr_VRAMaddr - 0x2000] = value;
					break;
				case 0x2800:
					nameTables[curr_VRAMaddr - 0x800 - 0x2000] = value;
					break;
				case 0x2C00:
					nameTables[(curr_VRAMaddr - 0x800) - 0x2000] = value;
					break;
			}

		} else {
			nameTables[curr_VRAMaddr - 0x2000] = value;
		}
	}
	else if ((curr_VRAMaddr >= 0x3f00) && (curr_VRAMaddr < 0x3f20)) {
		nameTables[curr_VRAMaddr - 0x2000] = value;
		if ((curr_VRAMaddr & 0x7) == 0)
		{
			nameTables[(curr_VRAMaddr - 0x2000) ^ 0x10] = value;
		}
	}
	curr_VRAMaddr += addrInc;
}

void PPU::WriteSprAddr( byte value ) {
	sprAddr = value;
}

void PPU::WriteSprMem( byte value ) {
	sprRam[sprAddr] = value;
	sprAddr = (sprAddr + 1) & 0xff;
}

byte PPU::ReadSprMem() {
	return sprRam[sprAddr];
}

void PPU::ScanBackground() {


	byte   frameSide;
	ushort virtualScanLine;
	ushort baseTableAddr;
	ushort startColumn;
	ushort endColumn;
	ushort startPixel;
	ushort endPixel;
	ushort tilenum;
	ushort currTileCol;
	ushort tileOffset;

	byte tile1;
	byte tile2;
	byte pixelColor;
	byte paletteHighBits;

	/**
	 * Frame Size divides the frame in two parts
	 *
	 * FramSide = 0 (current name table)
	 * FramSide = 1 (next name table)
	 */


	for( frameSide = 0; frameSide < 2; frameSide++ ) {
		virtualScanLine = currScanLine + scrollH;
		baseTableAddr = nTblAddress;

		if( frameSide == 0 ) {
			if (virtualScanLine >= 240) {
				switch (nTblAddress) {
					case 0x2000:
						baseTableAddr = 0x2800;
						break;
					case 0x2400:
						baseTableAddr = 0x2C00;
						break;
					case 0x2800:
						baseTableAddr = 0x2000;
						break;
					case 0x2C00:
						baseTableAddr = 0x2400;
						break;
				}

				virtualScanLine = virtualScanLine - 240;
			}

			startColumn = scrollV / 8;
			endColumn  = 32;

		} else {

			if( virtualScanLine >= 240 ) {
				switch (nTblAddress) {
					case 0x2000:
						baseTableAddr = 0x2C00;
						break;
					case 0x2400:
						baseTableAddr = 0x2800;
						break;
					case 0x2800:
						baseTableAddr = 0x2400;
						break;
					case 0x2C00:
						baseTableAddr = 0x2000;
						break;
				}
				virtualScanLine = virtualScanLine - 240;
			} else {
				switch (nTblAddress) {
					case 0x2000:
						baseTableAddr = 0x2400;
						break;
					case 0x2400:
						baseTableAddr = 0x2000;
						break;
					case 0x2800:
						baseTableAddr = 0x2C00;
						break;
					case 0x2C00:
						baseTableAddr = 0x2800;
						break;
				}
			}

			startColumn = 0;
			endColumn = (scrollV/8) + 1;
		}

		if ( nes->getCart()->GetMirroring() == HORIZONTAL_MIRRORING ) {
			switch ( baseTableAddr ) {
				case (0x2400): baseTableAddr = 0x2000; break;
				case (0x2800): baseTableAddr = 0x2400; break;
				case (0x2C00): baseTableAddr = 0x2400; break;
			}
		} else if ( nes->getCart()->GetMirroring() == VERTICAL_MIRRORING ) {
			switch (baseTableAddr) {
				case (0x2800): baseTableAddr = 0x2000; break;
				case (0x2C00): baseTableAddr = 0x2400; break;
			}
		}
		/*
		 *
		 else if ( nes->getCart().GetMirroring() == ONESCREAM_MIRRORING )
		{
			baseTableAddr = nTblAddress;
		}
		*/

		for( currTileCol = startColumn; currTileCol < endColumn; currTileCol++ ) {

			// find tile num
			tilenum = nameTables[ currTileCol + (virtualScanLine/8)*32 + (baseTableAddr - 0x2000) ];

			// go to pattern table
			tileOffset = tilenum * 16 + bgPatternTable;

			// get in pattern table
			tile1 = nes->getMapper()->ReadChrRom((ushort)(tileOffset + ( virtualScanLine % 8) ) );
			tile2 = nes->getMapper()->ReadChrRom((ushort)(tileOffset + ( virtualScanLine % 8) + 8 ));

			//Step #4, get the attribute byte for the block of tiles we're in
			//this will put us in the correct section in the palette table

			paletteHighBits = nameTables[((baseTableAddr - 0x2000 +
				0x3c0 + (((virtualScanLine / 8) / 4) * 8) +  (currTileCol / 4)))];

			paletteHighBits = (byte)(paletteHighBits >> ((4 * (((virtualScanLine / 8 ) % 4) / 2)) +
				(2 * ((currTileCol % 4) / 2))));

			paletteHighBits = (byte)((paletteHighBits & 0x3) << 2);

			if( frameSide == 0 ) {
				if( currTileCol == startColumn ) {
					startPixel = scrollV % 8;
					endPixel = 8;
				} else {
					startPixel = 0;
					endPixel = 8;
				}
			} else {
				if (currTileCol == endColumn) {
					startPixel = 0;
					endPixel = scrollV % 8;
				} else {
					startPixel = 0;
					endPixel = 8;
				}
			}
			byte i = 0;

			for (i = startPixel; i < endPixel; i++) {
				pixelColor = paletteHighBits + (((tile2 & (1 << (7 - i))) >> (7 - i)) << 1) +
					((tile1 & (1 << (7 - i))) >> (7 - i));
					if (frameSide == 0) {
						ScreenBuffer[((currScanLine * 256) + (8 * currTileCol) - scrollV + i) % (256 * 256) ] =
							(uint) NesPalette[(0x3f & nameTables[0x1f00 + pixelColor])];
					} else {
						if ( ((8 * currTileCol) + (256 - scrollV) + i) < 256) {
							ScreenBuffer[(currScanLine * 256) + (8 * currTileCol) + (256 - scrollV) + i] =
								(uint) NesPalette[(0x3f & nameTables[0x1f00 + pixelColor])];
						}
					}
				}
			}
		}
}


void PPU::RenderScanLine() {
	sprCrossed = 0;


	if( currScanLine == 240 ) {
		if( nmiOnVblank != 0 ) {
			nes->getCpu()->Push16( nes->getCpu()->pc_reg );
			nes->getCpu()->PushStat();
			nes->getCpu()->pc_reg = nes->getCpu()->ReadMem16( 0xFFFA );
		}

		for(int i = 0; i < 256; i++ )
			ScreenBuffer[currScanLine*256+i] = NesPalette[(uint)nameTables[0x1f00]];

		if( currScanLine == 240 ) {

			video.PutPixelsOnFrame( ScreenBuffer );

			t1 = clk();
			uint32_t slp = (1000000/70 - (t1 - t0));
			if( slp > 0 && slp <  1000000/70 )
				usleep( slp );
			t0 = clk();


		}
	}
	if( currScanLine < 234 ) {
		for(int i = 0; i < 256; i++ )
			ScreenBuffer[currScanLine*256+i] = 0;

		if( sprVisibility)
			ScanSprites(0x20);
		if( bgVisibility )
			ScanBackground();
	    if( sprVisibility)
			ScanSprites(0x0);
	}
	if( currScanLine >= 256 ) {
		currScanLine = 0;
	}
	currScanLine = currScanLine + 1;
}

void PPU::Sprit0Hit() {
	 byte sprite_x, sprite_y, sprite_id, sprite_attributes;

		sprite_y = sprRam[0];
		sprite_id = sprRam[1];
		sprite_attributes = sprRam[2];
		sprite_x = sprRam[3];

		if (currScanLine >= (sprite_y + sprSize + 1))
			spr0Hit = 1;
}
void PPU::SprRamDMABegin(byte data) {
		int i;
		for (i = 0; i < 0x100; i++)
			sprRam[i] = nes->ReadMem8( ((data * 0x100) + i) & 0xFFFF );
}

void PPU::ScanSprites( byte mode ) {
	int i, j;
	int spriteLineToDraw;
	byte tiledata1, tiledata2;
	int offsetToSprite;
	byte paletteHighBits;
	int pixelColor;
	byte actualY;

	byte spriteId;

	//Step #1 loop through each sprite in sprite RAM
	//Back to front, early numbered sprites get drawing priority

	for (i = 252; i >= 0; i = i - 4)
	{
		actualY = (byte)(sprRam[i] + 1);
		//Step #2: if the sprite falls on the current scanline, draw it
		if (((sprRam[i+2] & 0x20) == mode)&&
			 (actualY <= currScanLine) &&
			((actualY + sprSize) > currScanLine))
		{
			sprCrossed++;
			//Step #3: Draw the sprites differently if they are 8x8 or 8x16
			if (sprSize == 8)
			{
				//Step #4: calculate which line of the sprite is currently being drawn
				//Line to draw is: currentScanline - Y coord + 1

				if ((sprRam[i+2] & 0x80) != 0x80)
					spriteLineToDraw = currScanLine - actualY;
				else
					spriteLineToDraw = actualY + 7 - currScanLine;

				//Step #5: calculate the offset to the sprite's data in
				//our chr rom data
				offsetToSprite = sprAddr + sprRam[i+1] * 16;

				//Step #6: extract our tile data
				tiledata1 = nes->getMapper()->ReadChrRom( (ushort)(offsetToSprite + spriteLineToDraw));
				tiledata2 = nes->getMapper()->ReadChrRom((ushort)(offsetToSprite + spriteLineToDraw + 8));

				//Step #7: get the palette attribute data
				paletteHighBits = (byte)((sprRam[i+2] & 0x3) << 2);

				//Step #8, render the line inside the tile to the offscreen buffer
				for (j = 0; j < 8; j++)
				{
					if ((sprRam[i+2] & 0x40) == 0x40)
					{
						pixelColor = paletteHighBits + (((tiledata2 & (1 << (j))) >> (j)) << 1) +
							((tiledata1 & (1 << (j))) >> (j));
					}
					else
					{
						pixelColor = paletteHighBits + (((tiledata2 & (1 << (7 - j))) >> (7 - j)) << 1) +
							((tiledata1 & (1 << (7 - j))) >> (7 - j));
					}
					if ((pixelColor % 4) != 0)
					{

						if ((sprRam[i+3] + j) < 256 && currScanLine < 256)
						{
							ScreenBuffer[(currScanLine * 256) + (sprRam[i+3]) + j] =
									NesPalette[(0x3f & nameTables[0x1f10 + pixelColor])];
						}

					}
				}
			}

			else
			{
				//The sprites are 8x16, to do so we draw two tiles with slightly
				//different rules than we had before

				//Step #4: Get the sprite ID and the offset in that 8x16 sprite
				//Note, for vertical flip'd sprites, we start at 15, instead of
				//8 like above to force the tiles in opposite order
				spriteId = sprRam[i+1];
				if ((sprRam[i+2] & 0x80) != 0x80)
				{
					spriteLineToDraw = currScanLine - actualY;
				}
				else
				{
					spriteLineToDraw = actualY + 15 - currScanLine;
				}
				//Step #5: We draw the sprite like two halves, so getting past the
				//first 8 puts us into the next tile
				//If the ID is even, the tile is in 0x0000, odd 0x1000
				if (spriteLineToDraw < 8)
				{
					//Draw the top tile
					{
						if ((spriteId % 2) == 0)
							offsetToSprite = 0x0000 + (spriteId) * 16;
						else
							offsetToSprite = 0x1000 + (spriteId - 1) * 16;

					}
				}
				else
				{
					//Draw the bottom tile
					spriteLineToDraw = spriteLineToDraw - 8;

					if ((spriteId % 2) == 0)
						offsetToSprite = 0x0000 + (spriteId + 1) * 16;
					else
						offsetToSprite = 0x1000 + (spriteId) * 16;
				}

				//Step #6: extract our tile data
				tiledata1 = nes->getMapper()->ReadChrRom((ushort)(offsetToSprite + spriteLineToDraw));
				tiledata2 = nes->getMapper()->ReadChrRom((ushort)(offsetToSprite + spriteLineToDraw + 8));

				//Step #7: get the palette attribute data
				paletteHighBits = (byte)((sprRam[i+2] & 0x3) << 2);

				//Step #8, render the line inside the tile to the offscreen buffer
				for (j = 0; j < 8; j++)
				{
					if ((sprRam[i+2] & 0x40) == 0x40)
					{
						pixelColor = paletteHighBits + (((tiledata2 & (1 << (j))) >> (j)) << 1) +
							((tiledata1 & (1 << (j))) >> (j));
					}
					else
					{
						pixelColor = paletteHighBits + (((tiledata2 & (1 << (7 - j))) >> (7 - j)) << 1) +
							((tiledata1 & (1 << (7 - j))) >> (7 - j));
					}
					if ((pixelColor % 4) != 0)
					{
						if ((sprRam[i+3] + j) < 256 && currScanLine < 256)
						{
							ScreenBuffer[(currScanLine * 256) + (sprRam[i+3]) + j] =
								NesPalette[(0x3f & nameTables[0x1f10 + pixelColor])];
						}
					}
				}

			}
		}
	}

}





