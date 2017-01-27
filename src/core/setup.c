#include <core/setup.h>
#include <core/video.h>
#include <core/print.h>
#include <core/serial_driver.h>

void setup()
{
   video_clear();
   printf("-= o00oO =- Starting....\n\n");
   init_serial_uart(PORT_COM1);
   init_serial_uart(PORT_COM2);
   init_serial_uart(PORT_COM3);
   init_serial_uart(PORT_COM4);
   printf("-= o00oO =- Crash OS ready.\n\n");
}
