/* File : fast_distance.c */
/* (c) Yves Lepage */

#define MODULE "bitcount.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitcount.h"

#define WLEN		32	/* length of machine word in bits */
#define LOGWLEN	5	/* log word length -- round(LOG2(WLEN) */

/*
 * Wegner's method also called Kernighan's method, linear in the number of bit sets.
 */
int wegner_bitcount( char *wp, int nwords)
{
   register  char w ;
   register unsigned int count ;
   register unsigned int i, c, v ;

   count = 0 ;
   for ( i = 0 ; i < nwords ; ++i )
   {
      w = *(wp++) ;
      for ( c = 0 ; w ; c++ )
      {
         w &= w - 1 ;
      } ;
      count += c ;
   } ;
   return count ;
}

/* 
 * Other method supposed to be O(log)
 */
int bitcount( char *wp, int nwords)
{
   register  char w ;
   register unsigned int count ;
   register int j, rshift, i ;
   unsigned int bitmask[5] =
   {
      0x55555555, 0x33333333, 0x0f0f0f0f, 0x00ff00ff, 0x0000ffff
   } ;

   count = 0 ;
   for ( i = 0 ; i < nwords ; ++i )
   {
      w = *(wp++) ;
      rshift = 1 ;
      for ( j = 0 ; j < LOGWLEN ; ++j )
      {
         w = (w & bitmask[j]) + ((w & ~bitmask[j]) >> rshift) ;
         rshift <<= 1 ;
      } ;
      count += w ;
   } ;
   return count ;
}
