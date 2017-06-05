// This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
// Copyright Airbus Group
#include <core/string.h>

void* memset(void *dst, int c, size_t size)
{
   uint8_t *tmp = (uint8_t*)dst;

   while( size )
   {
      *tmp = (uint8_t)c;
      size--;
      tmp++;
   }

   return dst;
}

void* memcpy(void *dst0, const void *src0, unsigned int size)
{
  char *dst;
  const char *src;
  for (dst = (char*)dst0, src = (const char*)src0 ;
       size > 0 ;
       dst++, src++, size--)
    *dst = *src;
  return dst0;
}

int strcmp(void *s1, void *s2, unsigned int sz)
{
   unsigned int i;
   for(i=0 ; i<sz && ((char*)s1)[i] == ((char*)s2)[i] ; i++);
   return (i<sz)?0:1;
}

