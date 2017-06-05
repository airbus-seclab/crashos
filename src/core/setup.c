// This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
// Copyright Airbus Group
#include <core/setup.h>
#include <core/video.h>
#include <core/print.h>
#include <core/serial_driver.h>

void setup()
{
   video_clear();
   printf("-= o00oO =- Starting....\n\n");
   init_serial_uart(PORT_COM1);   printf("serial1 ready\n");
//   init_serial_uart(PORT_COM2);   printf("serial2 ready\n");
//   init_serial_uart(PORT_COM3);   printf("serial3 ready\n");
//   init_serial_uart(PORT_COM4);   printf("serial4 ready\n");
   printf("-= o00oO =- Crash OS ready.\n\n");
}
