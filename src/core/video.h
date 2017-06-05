// This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
// Copyright Airbus Group
#ifndef __VIDEO_H__
#define __VIDEO_H__

#include <core/types.h>

#define BLACK       0
#define BLUE        1
#define GREEN       2
#define CYAN        3
#define RED         4
#define MAGENTA     5
#define BROWN       6
#define LGRAY       7
#define DGRAY       8
#define LBLUE       9
#define LGREEN     10
#define LCYAN      11
#define LRED       12
#define LMAGENTA   13
#define YELLOW     14
#define WHITE      15

#define VIDEO_FRAME_BUFFER            0xb8000
#define VIDEO_DEFAULT_NB_COLUMNS      80
#define VIDEO_DEFAULT_NB_LINES        25

typedef struct video_character
{
   uint8_t txt;
   uint8_t col;

} video_char_t;

typedef struct video_line
{
   struct video_character column[VIDEO_DEFAULT_NB_COLUMNS];

} video_line_t;

typedef struct video_screen
{
   struct video_line line[VIDEO_DEFAULT_NB_LINES];

} video_screen_t;

typedef struct video_information
{
   uint8_t           fg;
   uint8_t           bg;
   uint32_t          cl;
   uint32_t          ln;
   video_screen_t    *fb;

} video_info_t;

/**
 * Init the video memory
 */
void video_init();
/**
 * Write on the video memory
 * @param char* the character to write
 */
void video_write(char*);
/**
 * Clear the screen
 */
void video_clear();
/**
 * Scroll the content when the screen is full
 */
void video_scroll();

#endif
