/* File: fast_distance.h */

int fast_distance(char *word1, char *word2) ;
int fast_similitude(char *word1, char *word2) ; /* dist(word1,word2) = len(word1) + len(word2) - 2 x sim(word1,word2) */

void init_memo_fast_distance(char * word1) ;    /* compile word1 for subsequent computations using memo_fast_distance or memo_fast_similitude */
int memo_fast_distance(char *word2) ;           /* compute edit distance between word2 and word1 (word1 should have been compiled using init_memo_fast_distance) */
int memo_fast_similitude(char *word2) ;         /* same but for similitude */
