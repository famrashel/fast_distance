/* File : example.i */
%module fast_distance
%{
#include "fast_distance.h"
/* Some helper functions to make it easier to test */
extern int fast_distance(char *word1, char *word2) ;
extern int fast_similitude(char *word1, char *word2) ;

extern void init_memo_fast_distance(char * word1) ;
extern int memo_fast_distance(char *word2) ;
extern int memo_fast_similitude(char *word2) ;
%}
extern int fast_distance(char *word1, char *word2) ;
extern int fast_similitude(char *word1, char *word2) ;

extern void init_memo_fast_distance(char * word1) ;
extern int memo_fast_distance(char *word2) ;
extern int memo_fast_similitude(char *word2) ;


