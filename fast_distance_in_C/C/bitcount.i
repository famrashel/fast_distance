/* File : example.i */
%module bitcount
%{
#include "bitcount.h"
extern int wegner_bitcount(char *wp, int nwords) ;
extern int bitcount(char *wp, int nwords) ;
%}
extern int wegner_bitcount(char *wp, int nwords) ;
extern int bitcount(char *wp, int nwords) ;


