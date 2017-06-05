#include <core/print.h>
#include <core/video.h>
#include <core/serial_driver.h>

size_t printf_log(char *format, ...) {
	va_list  params;
	size_t   retval;

	va_start( params, format );
	retval = sprintf( format, params ); // serial printf
	retval = vprintf( format, params ); // video  printf
	va_end( params );
	return retval; // warning value...
}

size_t log(char *format, ...)
{
   va_list  params;
   size_t   retval;

   va_start( params, format );
   retval = sprintf( format, params );
   va_end( params );

   return retval;
}

size_t printf(char *format, ...)
{
   va_list  params;
   size_t   retval;

   va_start( params, format );
   retval = vprintf( format, params );
   va_end( params );

   return retval;
}

size_t sprintf(char *format, va_list params)
{
   size_t   retval;
   char     buf[1024];

   retval = vsnprintf( buf, sizeof(buf), format, params );
   serial_write( buf, PORT_COM1);
   return retval;
}


size_t vprintf(char *format, va_list params)
{
   size_t   retval;
   char     buffer[1024];

   retval = vsnprintf( buffer, sizeof(buffer), format, params );
   video_write( buffer );
   return retval;
}

/*
** Poor-man printf
*/
size_t vsnprintf(char *buffer, size_t len, char *format, va_list params)
{
   size_t    count = 0, maxlen = len;
   bool      in_format = FALSE;
   uint8_t   precision=0;

#define ADDCHAR(c) \
   if( count < maxlen-1 ) { \
      count ++; \
      *buffer++ = c; \
   }

   while( *format )
   {
      if( in_format )
      {
         switch( *format )
	 {
	 case '%' :
	    ADDCHAR( *format );
	    break;
	 case '.' :   /* one digit precision */
	    format++;
	    precision = *format - '0';
	    break;
	 case 'b':
	 {
	    uint32_t i;
	    int      value = va_arg( params, int );

	    for( i=0 ; i<8 ; i++ )
	       ADDCHAR( (value&(1<<(7-i)))?'1':'0' );
	    break;
	 }
	 case 'd' :
	 {
	    char tmpbuf[32];
	    int tmpcount=0;
	    int value = va_arg( params, int );
           
	    if( value < 0 )
	    {
	       ADDCHAR( '-' );
	       value = -value;
	    }
	    else if( ! value )
	       ADDCHAR( '0' );
           
	    if( precision )
	    {
	       if( value < 10 )
		  ADDCHAR( '0' );
	       precision = 0;
	    }

	    while( value )
	    {
	       tmpbuf[tmpcount++] = (value%10) + '0';
	       value /= 10;
	    }

	    while( tmpcount-- )
	    {
	       ADDCHAR( tmpbuf[tmpcount] );
	    }
           
	    break;
	 }
	 case 's' :
	 {
	    char   *src = va_arg( params, char* );
           
	    while( *src )
	    {
	       ADDCHAR(*src);
	       src++;
	    }
           
	    break;
	 }
	 case 'c' :
	 {
	    int value = va_arg( params, int );
	    ADDCHAR( value );
           
	    break;
	 }
	 case 'x' :
	 {
	    char tmpbuf[32];
	    char hextable[] = {   '0', '1', '2', '3',
				  '4', '5', '6', '7',
				  '8', '9', 'a', 'b',
				  'c', 'd', 'e', 'f' };
	    int    tmpcount=0;
	    uint_t value = va_arg( params, uint_t );
           
	    if( ! value )
	    {
	       ADDCHAR( '0' );
	    }

	    if( precision )
	    {
	       if( value < 0x10 )
		  ADDCHAR( '0' );

	       precision = 0;
	    }

	    while( value )
	    {
	       tmpbuf[tmpcount++] = hextable[value%0x10];
	       value /= 0x10;
	    }
           
	    while( tmpcount )
	    {
	       ADDCHAR( tmpbuf[--tmpcount] );
	    }
	    break;
	 }
	 case 'X' :
	 {
	    char tmpbuf[32];
	    char hextable[] = {   '0', '1', '2', '3',
				  '4', '5', '6', '7',
				  '8', '9', 'a', 'b',
				  'c', 'd', 'e', 'f' };
	    int      tmpcount=0;
	    uint64_t value = va_arg( params, uint64_t );
           
	    if( ! value )
	    {
	       ADDCHAR( '0' );
	    }

	    if( precision )
	    {
	       if( value < 0x10 )
		  ADDCHAR( '0' );

	       precision = 0;
	    }

	    while( value )
	    {
	       tmpbuf[tmpcount++] = hextable[value%0x10];
	       value /= 0x10;
	    }
           
	    while( tmpcount )
	    {
	       ADDCHAR( tmpbuf[--tmpcount] );
	    }
	    break;
	 }


	 }//switch
	 
         if( ! precision )
            in_format = FALSE;
        
      }//if( in_format )
      else if( *format == '%' )
      {
         in_format = TRUE;
      }
      else
      {
         ADDCHAR( *format );
      }
      format ++;
   }
   *buffer = 0;

   return count+1;
}
