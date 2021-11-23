/*
 * PPU.h
 *
 *  Created on: 07/05/2010
 *      Author: pcslara
 */

#ifndef PPU_H_
#define PPU_H_

#include "Video.h"
#include "nestypes.h"
class Nes;

class PPU {
private:

	// Control Flags Register 1:
	ushort nmiOnVblank;      // NMI on VBlank. 0=disable, 1=enable
	ushort sprSize;          // Sprite size. 0=8x8, 1=8x16
	ushort bgPatternTable;   // Background Pattern Table address. 0=0x0000,1=0x1000
	ushort sprPatternTable;  // Sprite Pattern Table address. 0=0x0000,1=0x1000
	ushort addrInc;          // PPU Address Increment. 0=1,1=32
	ushort nTblAddress;      // Name Table Address. 0=0x2000,1=0x2400,2=0x2800,3=0x2C00
	ushort ppuSelection;     // 1=Slave 0=Master

	// Control Flags Register 2:
	byte fullBgColor;       // Background color. 0=black, 1=blue, 2=green, 4=red
	byte sprVisibility;     // Sprite visibility. 0=not displayed,1=displayed
	byte bgVisibility;      // Background visibility. 0=Not Displayed,1=displayed
	byte noSprClipping;     // Sprite clipping. 0=Sprites invisible in left 8-pixel column,1=No clipping
	byte noBgClipping;      // Background clipping. 0=BG invisible in left 8-pixel column, 1=No clipping
	byte dispType;          // Display type. 0=color, 1=monochrome

	// Status flags:
	byte vBlankOcc;         // VBLANK Occurrence true=1, false=0
	byte spr0Hit;
	byte sprScanlineCount;
	byte vRamWrites;

	// VRAM I/O:
	ushort vramAddress;
	ushort vramTmpAddress;
	ushort vramBufferedReadValue;
	byte   vramHiLoToggle;      // VRAM/Scroll Hi/Lo latch

	// Memory to Render
	byte sprRam[0x100];
	byte nameTables[0x2000];
	int frames;


	byte scrollV;
	byte scrollH;

	uint ScreenBuffer[256 * 256];
	uint NesPalette[64];
	ushort curr_VRAMaddr;
	ushort prev_VRAMaddr;


	byte sprAddr;
	byte vramBuff;
	int  sprCrossed;

	Video video;
	PPU * ppu;
	Nes * nes;

	uint32_t t0;
	uint32_t t1;
protected:



public:

	ushort currScanLine;
	PPU( Nes * nes );

	void WriteControlReg1(byte value);   // $2000
	void WriteControlReg2(byte value);   // $2001
	void WriteStatReg (byte value);      // $2002
	void WriteSprAddr( byte value );     // $2003
	void WriteSprMem ( byte value );     // $2004

	void WriteScrollReg(byte value);     // VRAM I/O Addr1 $2005
	void WriteVramAddr(byte value);      // VRAM I/O Addr2 $2006
	void WriteVramIO  (byte value);      // VRAM I/O       $2007


	byte ReadControlReg1();              // $2000
	byte ReadControlReg2();              // $2001
	byte ReadStatReg();                  // $2002
	byte ReadSprMem();                   // $2003
	byte ReadScrollReg(); 				 // VRAM I/O Addr $2005
	byte ReadVramIO();    				 // VRAM I/O   $2005

	void RenderScanLine();
	void ScanBackground();
	void ScanSprites( byte mode );
	void SprRamDMABegin(byte data);

	void Sprit0Hit();
};


#endif /* PPU_H_ */
