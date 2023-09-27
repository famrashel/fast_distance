/* File : fast_distance.c */
/* (c) Yves Lepage, except for the Allison and Dix part */

#define MODULE "fast_distance"
#define TRACE 0
#define ERR_MALLOC "allocation memory error"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utf8.h"
#include "fast_distance.h"

#define ytrace(x) { printf x ; fflush(stderr) ; fflush(stdout) ; }
#define ntrace(x)

#if TRACE
/* #define trace(x) { ytrace(x) ; if ( iscgiuse() ) printf("<br>\n") ; fflush(stdout) ; } */
#define trace(x) { ytrace(x) ; }
#else
#define trace(x)
#endif

/*
 * Type definitions
 */

typedef unsigned int BYTE ;
#define mychar	unsigned char		/* type for the symbols */

static int tracet		= 0 ;
static int timetrace	= 0 ;
static int debug		= 0 ;
static int inverse		= 0 ;
static int LENGTH		= 1 ;

#define inv(a) (( inverse ) ? (a == 0) : (a != 0) )

/*
 * Various variables
 */

#define ALPHABETSIZEMAX (256*256)		/* maximal size of the alphabet */
static int presence[ALPHABETSIZEMAX] = { 0 } ;

#define WLEN		32	/* length of machine word in bits */
#define LOGWLEN		5	/* log word length -- round(LOG2(WLEN) */

#define BITMAX		16	/* maximum byte string length */
						/* sentence length ought to be < 512 = BITMAX * WLEN bytes */
		      

/*
 * Variables used in newalphastring, freealphastring, bitops and xbitlcs
 */

static char *memo_word = NULL ;
static int memo_word_length = 0 ;
static BYTE a_strings[BITMAX*WLEN][BITMAX] = { 0 } ;
static BYTE *pa_strings[BITMAX*WLEN] = { NULL } ;

/*
 * Min
 */

int min(int i, int j)
{
	if ( i < j )
		return i ;
	else
		return j ;
}

/*
 * Memoizing the parameters for the first string
 */

static void newalphastring(char *s, int len)
{
   register mychar *p ;
   register int i = 0, j = 0 ;
   register int n = 0 ;

trace(("in  newalphastring(%s, %d)\n",s,len))

   p = (mychar *) s ;
   j = len ;
   for ( i = 0 ; i < j ; ++i, ++p )
   {
      register int *pp = &(presence[*p]) ;

      if ( ! *pp )
      {
         *pp = ++n ;
         if ( n > BITMAX*WLEN ) /* a_strings and pa_strings are limited to BITMAX*WLEN */
				printf(MODULE "newalphastring" ERR_MALLOC) ;
         pa_strings[n] = a_strings[n] ;
      } ;
      a_strings[*pp][i/WLEN] |=  1 << (i % WLEN) ;
   } ;
   if ( debug )
   {
      int done[BITMAX*WLEN] = { 0 } ;
      mychar *ss = (mychar *) s ;

      for ( i = 0 ; i < len ; ++i )
      {
         if ( ! done[presence[ss[i]]] )
         {
				for ( j = 0 ; j < LENGTH ; ++j )
					printf("%c", s[i*LENGTH+j]) ;
				printf(":  ") ;
				for ( j = len - 1 ; j >= 0 ; --j )
					printf("%1x%s", (pa_strings[presence[ss[i]]][j/WLEN] >> (j % WLEN)) & 1, (j%WLEN == 0)?" ":"") ;
				printf("\n") ;
            done[presence[ss[i]]] = 1 ;
         } ;
      } ;
   } ;
   
trace(("out newalphastring(%s, %d)\n",s,len))
}


static void freealphastring(char *s, int len)
{
   register mychar *p ;
   register int i = 0, j = 0 ;

trace(("in  freealphastring(%s, %d)\n",s,len))

   p = (mychar *) s ;
   j = len ;
   for ( i = 0 ; i < j ; ++i, ++p )
   {
      int pp = presence[*p] ;

      if ( pp )
         a_strings[pp][i/WLEN] = 0 ;
/* we do not clean pa_strings because it is time consuming and useless */
/*       pa_strings[pp] = NULL ; */
   } ;
   p = (mychar *) s ;
   for ( i = 0 ; i < j ; ++i, ++p )
      presence[*p] = 0 ;

trace(("out freealphastring(%s, %d)\n",s,len))
}

/*
 * Wegner's method or Kernighan's method, linear in the number of bit sets.
 */
static int wegner_bitcount(BYTE *wp, int nwords)
{
   register BYTE w, count ;
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

/* The bit-string LCS program, i.e the *.c and *.h files, in this subdirectory,
   is released under the "GNU General Public License" (GPL) Version 2,
   June 1991, [ http://www.gnu.org/copyleft/gpl.html ]  provided that
   any resulting publications refer to the following paper: 
         L. Allison and T.I. Dix (1986).
         A Bit-String Longest-Common-Subsequence Algorithm.
         Inf. Proc.  Lett., V23, Dec' 1986, pp305-310. 
   - L. Allison & T. I. Dix, 5/2001
*/
 
/*
 * The functions bitops and xbitlcs contain the basic computation
 * of the Dix and Allison algorithm
 */
static int bitcount(BYTE *wp, int nwords)
{
   register BYTE w, count ;
   register int j, rshift, i ;
   BYTE bitmask[5] =
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

static void bitops(BYTE *last, BYTE *cur, int pp, int nwords)
{
   register BYTE x, y ;
   register int j ;
   register BYTE *a_s ;
   register BYTE top_borrow, bottombit ;

   a_s = &pa_strings[pp][0] ;
   bottombit = 1 ;
   for ( j = 0 ; j < nwords ; ++j )
   {
      y = *(last++) ;
      x =  y | *(a_s++) ;
      top_borrow = (y >> (WLEN - 1)) & 0x1 ;
      y = ((y << 1) | bottombit) ;
      if ( x < y )
         top_borrow = 1 ;
      *(cur++) = x & ((x - y) ^ x) ;
      bottombit = top_borrow ;
   } ;
}

static int xbitlcs(char *word1, int l1, char *word2, int l2)
{
   int nwords = (l1 + WLEN - 1) / WLEN ; /* no. of words for bits */
   BYTE bit1[BITMAX] = { 0 } ;
   BYTE bit2[BITMAX] ;
   BYTE *pb1, *pb2, *t1 ;
   register int i ;
   register mychar *pbstring ;
   register int result = 0 ;

trace(("in  xbitlcs(%s, %d, %s, %d)\n",word1,l1,word2,l2))

/*
   pb1 = &bit1[0];
   for ( i = 0 ; i < nwords ; ++i )
      *pb1++ = 0;		/*** bit1[i] = 0; ***/
   pb1 = &bit1[0] ;
   pb2 = &bit2[0] ;
   pbstring = (mychar *) word2 ;

   if ( debug )
      printf("WLEN=%d, LOGWLEN=%d, BITMAX=%d\n", WLEN,LOGWLEN, BITMAX) ;
   if ( tracet )
   {
      int k = 0 ;
      
      for ( i = l1-1 ; i >= 0 ; --i )
         for ( k = 0 ; k < LENGTH ; ++k )
	   printf("%c",word1[i*LENGTH+k]) ;
      printf("\n") ;
   } ;
   for ( i = 1 ; i <= l2 ; ++i )
   {
      int pp = presence[*pbstring] ;

      if ( inv(pp) )
      {
			bitops(pb1, pb2, pp, nwords);
			if ( tracet )
			{
				int j ;
            int k ;
   
				for ( j = l1 - 1 ; j >= 0 ; --j )
					printf("%1x%s", ((pb2[j/WLEN] >> (j % WLEN)) & 1), (j%WLEN == 0)?" ":"");
            for ( k = 0 ; k < LENGTH ; ++k )
					printf("%c", word2[(i-1)*LENGTH+k]);
				printf("\n");
			} ;
			t1 = pb1 ;
			pb1 = pb2 ;
			pb2 = t1 ;
      } ;
      pbstring++ ;
   } ;
   result = bitcount(pb1,nwords) ;

trace(("out xbitlcs(%s, %d, %s, %d) = %d\n",word1,l1,word2,l2,result))

   return result ;
}

static int bitlcs(char *word1, int l1, char *word2, int l2)
{
   register int result = 0 ;

trace(("in  bitlcs(%s, %d, %s, %d)\n",word1,l1,word2,l2))

   newalphastring(word1, l1) ;
   result = xbitlcs(word1,l1,word2,l2) ;
   freealphastring(word1,l1) ;

trace(("out bitlcs(%s, %d, %s, %d) = %d\n",word1,l1,word2,l2,result))

   return result ;
}

/*
 * Slow computation with dynamic programming method
 */
  
static int **xslow_distance(mychar *word1, int l1, mychar *word2, int l2)
{
   register int **matrix = NULL ;
   register int i1 = 0, i2 = 0 ;

trace(("in  xslow_distance(%s, %d, %s, %d)\n",word1,l1,word2,l2))

   matrix = (int **) calloc(l1+1,sizeof(int *)) ;
   for ( i1 = 0 ; i1 <= l1 ; ++i1 )
      matrix[i1] = (int *) calloc(l2+1,sizeof(int)) ;
   for ( i1 = 0 ; i1 <= l1 ; ++i1 )
      matrix[i1][0] = i1 ;
   for ( i2 = 0 ; i2 <= l2 ; ++i2 )
      matrix[0][i2] = i2 ;
   for ( i1 = 1 ; i1 <= l1 ; ++i1 )
      for ( i2 = 1 ; i2 <= l2 ; ++i2 )
         matrix[i1][i2] = min(min(matrix[i1-1][i2] + 1,
                                  matrix[i1][i2-1] + 1),
                                  matrix[i1-1][i2-1] + 2 * (word1[i1-1] != word2[i2-1])) ;

trace(("out xslow_distance(%s, %d, %s, %d)\n",word1,l1,word2,l2))

   return matrix ;
}

/*
 * Main functions in the module for ascii string.
 * Wrapping with fast_ functions necessary for utf8 strings
 * are calledin the interface functions of the module.
 */

void ascii_init_memo_fast_distance(char *word1)
{
	freealphastring(memo_word,memo_word_length) ;
	if ( memo_word )
		free(memo_word) ;
	memo_word = (char *) strdup(word1) ;
	memo_word_length = strlen(memo_word) ;
	newalphastring(memo_word,memo_word_length) ;
}

int ascii_memo_fast_distance(char *word2)
{
	int result = 0 ;
	int l2 = strlen(word2) ;

trace(("in  memo_fast_distance(%s,%d)\n",word2,l2))

	result = (memo_word_length + l2 - 2 * xbitlcs(memo_word,memo_word_length,word2,l2)) ;

trace(("out memo_fast_distance(%s,%d) = %d\n",word2,l2,result))

	return result ;
}

int ascii_memo_fast_similitude(char *word2)
{
	int result = 0 ;
	int l2 = strlen(word2) ;

trace(("in  memo_fast_similitude(%s,%d)\n",word2,l2))

	result = xbitlcs(memo_word,memo_word_length,word2,l2) ;

trace(("out memo_fast_similitude(%s,%d) = %d\n",word2,l2,result))

	return result ;
}

int ascii_fast_distance(char *word1, char *word2)
{
	int result = 0 ;
	int l1 = strlen(word1) ;
	int l2 = strlen(word2) ;

trace(("in  fast_distance(%s,%d, %s,%d)\n",word1,l1,word2,l2))

	result = (l1 + l2 - 2 * bitlcs(word1,l1,word2,l2)) ;

trace(("out fast_distance(%s,%d, %s,%d) = %d\n",word1,l1,word2,l2,result))

	return result ;
}

int ascii_fast_similitude(char *word1, char *word2)
{
	int result = 0 ;
	int l1 = strlen(word1) ;
	int l2 = strlen(word2) ;

trace(("in  fast_similitude(%s,%d, %s,%d)\n",word1,l1,word2,l2))

	result = bitlcs(word1,l1,word2,l2) ;

trace(("out fast_similitude(%s,%d, %s,%d) = %d\n",word1,l1,word2,l2,result))

	return result ;
}

/*
 * Computation of function distance or similitude
 * according to whether arguments are ascii or not.
 */

static int fast_fct(char *uword1, char *uword2, int *fct(char *, char *))
{
	int result = 0 ;
	char *word1 = NULL,
	     *word2 = NULL ;

trace(("in  fct(%s,%s)\n",uword1,uword2))

	if ( isasciistring(uword1) )
	{
		word2 = maskunicodechar(uword2,uword1) ;
		result = fct(uword1,word2) ;
		free(word2) ;
	}
	else if ( isasciistring(uword2) )
	{
		word1 = maskunicodechar(uword1,uword2) ;
		result = fct(uword2,word1) ;
		free(word1) ;
	}
	else
	{
		initializecodedict() ;
		word1 = encode(uword1) ;
		word2 = encode(uword2) ;
		result = fct(word1,word2) ;
		free(word1) ;
		free(word2) ;
	} ;

trace(("out fct(%s,%s) = %d\n",uword1,uword2,result))

   return result ;
}

/* UTF-8 wrapper for unary functions ("memo_" functions) */
/* We need to encode in all cases as the first string may be ascii and the second non-ascii... */

static int fast_unary_fct(char *uword, int *fct(char *))
{
	int result = 0 ;
	char *word = NULL ;

trace(("in  unary_fct(%s)\n",uword))

	word = encode(uword) ;
	result = fct(word) ;
	free(word) ;

trace(("out unary_fct(%s) = %d\n",uword,result))

	return result ;
}

/*
 * Interface of the module:
 * Definition of the five functions of this file callable from outside.
 * All previous functions were static.
 */

int fast_distance(char *word1, char *word2)
{
/*	return fast_fct(word1,word2,ascii_fast_distance) ;*/
	init_memo_fast_distance(word1) ;
	return memo_fast_distance(word2) ;
}

int fast_similitude(char *word1, char *word2)
{
/*	return fast_fct(word1,word2,ascii_fast_similitude) ;*/
	init_memo_fast_distance(word1) ;
	return memo_fast_similitude(word2) ;
}

void init_memo_fast_distance(char *word1)
{
	initializecodedict() ;
	fast_unary_fct(word1, ascii_init_memo_fast_distance) ;
}

int memo_fast_distance(char *word2)
{
   return fast_unary_fct(word2, ascii_memo_fast_distance) ;
}

int memo_fast_similitude(char *word2)
{
   return fast_unary_fct(word2, ascii_memo_fast_similitude) ;
}

