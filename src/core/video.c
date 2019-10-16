// This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
// Copyright Airbus Group
#include <core/video.h>
#include <core/string.h>
#include <core/io.h>


video_info_t video = {
   .fb = (video_screen_t*)VIDEO_FRAME_BUFFER,
   .fg = WHITE,
   .bg = BLACK,
   .cl = 0,
   .ln = 0,
};

void video_clear()
{
    int x,y;
    for (x=0; x < VIDEO_DEFAULT_NB_COLUMNS; x++) {
	for (y=0; y < VIDEO_DEFAULT_NB_LINES; y++) {
	    video.fb->line[y].column[x].txt = 0;
	    video.fb->line[y].column[x].col = (video.bg<<4) | video.fg;
	}
    }
   video.cl = 0;
   video.ln = 0;
}


void move_cursor(int row, int col)
{
    unsigned short ofs = row*80+col;

    outb(0xf, 0x3d4);        // cursor location low register
    outb(ofs & 0xff, 0x3d5);
    outb(0xe, 0x3d4);       // cursor location high register
    outb((ofs >> 8)&0xff, 0x3d5);
}

void video_write(char *buffer)
{
   uint8_t color = (video.bg<<4) | video.fg;
   while( *buffer )
   {
      switch( *buffer )
      {
         case '\n':
	        video.ln++;
	        video.cl = 0;
                break;
         case '\r':
	        video.cl = 0;
	        break;
         default:
	        video.fb->line[video.ln].column[video.cl].txt = *buffer;
	        video.fb->line[video.ln].column[video.cl].col = color;
	        video.cl++;
	        if( video.cl >= VIDEO_DEFAULT_NB_COLUMNS )
	        {
	           video.cl = 0;
	           video.ln++;
	        }
	        break;
      }

      if( video.ln >= VIDEO_DEFAULT_NB_LINES )
      {
	     video_scroll();
        //video_scroll_bis();
	     video.ln--;
      }

      buffer++;
   }

   move_cursor(video.ln, video.cl);
}

void video_scroll()
{
   /* DONE */
   int i, j;
   uint8_t color = (video.bg<<4) | video.fg;
   for(i = 0; i < VIDEO_DEFAULT_NB_LINES; i++) {

      if(i < VIDEO_DEFAULT_NB_LINES - 1) {
    	  for (j = 0; j < VIDEO_DEFAULT_NB_COLUMNS; j++) {
    	       video.fb->line[i].column[j].txt = video.fb->line[i+1].column[j].txt;
    	       video.fb->line[i].column[j].col = video.fb->line[i+1].column[j].col;
    	  }


      } else {
         for (j = 0; j < VIDEO_DEFAULT_NB_COLUMNS; j++) {
            video.fb->line[i].column[j].txt = ' ';
            video.fb->line[i].column[j].col = color;
         }
      }
      
   }
}

void video_scroll_bis() {
  video.fb = video.fb + VIDEO_DEFAULT_NB_COLUMNS * 8; //TODO (optional)
  //...
}
