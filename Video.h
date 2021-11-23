/*
 * Video.h
 *
 *  Created on: 16/05/2010
 *      Author: pcslara
 */

#ifndef VIDEO_H_
#define VIDEO_H_

#include <allegro.h>
#include <stdio.h>

class Video {
private:
	int width;
	int height;
	BITMAP * bitmap;

public:

	Video( int w, int h ) {
		width = w;
		height = h;
		allegro_init();
		set_window_title("EmuNES, the NES emulator");
		install_keyboard();
		bitmap = create_bitmap( 256, 240 );
		set_gfx_mode(GFX_SAFE, 256, 240, 0, 0);      // inicia o vídeo
	}

	void PutPixelsOnFrame( uint ScreenBuff[256 * 240] ) {
		int i, j;
		static int n = 0;
		clear_bitmap( bitmap );
		for( i = 0; i < 240; i++ ) {
			for( j = 0; j < 256; j++ ) {
				putpixel( bitmap, j, i, ScreenBuff[i*256+j] );
				// printf("color = %d\n", makecol(0xFF & (ScreenBuff[i*256+j] >> 16), 0xFF & (ScreenBuff[i*256+j] >> 8), 0xFF & (ScreenBuff[i*256+j] )) );
				// putpixel( bitmap, j, i, makecol(0xFF & (ScreenBuff[i*256+j] >> 16), 0xFF & (ScreenBuff[i*256+j] >> 8), 0xFF & (ScreenBuff[i*256+j] )) );

			}
		}
		n++;
		blit( bitmap, screen, 0, 0, 0, 0, 256, 240 );
	}

};


#endif /* VIDEO_H_ */
