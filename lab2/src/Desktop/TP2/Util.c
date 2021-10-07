/* Some useful functions to read, write PBM, PGM, PPM files
**
**
** E.B.
*/

#include <stdio.h>
#include <stdlib.h>
#include "Util.h"

char pm_getc(FILE* file)
    {
    int ich;
    char ch;

    ich = getc( file );
    if ( ich == EOF )
        pm_erreur("EOF / read error" );
    ch = (char) ich;

    if ( ch == '#' )
        {
        do
            {
            ich = getc( file );
            if ( ich == EOF )
                pm_erreur("EOF / read error" );
            ch = (char) ich;
            }
        while ( ch != '\n' && ch != '\r' );
        }

    return ch;
    }


bit pm_getbit(FILE* file)
    {
    char ch;

    do
        {
        ch = pm_getc( file );
        }
    while ( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' );

    if ( ch != '0' && ch != '1' )
        pm_erreur("read error : != [0|1]" );

    return ( ch == '1' ) ? 1 : 0;
    }


unsigned char pm_getrawbyte(FILE* file)
    {
    int iby;

    iby = getc( file );
    if ( iby == EOF )
        pm_erreur("EOF / read error " );
    return (unsigned char) iby;
    }

int pm_getint( FILE* file)
    {
    char ch;
    int i;

    do
        {
        ch = pm_getc( file );
        }
    while ( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' );

    if ( ch < '0' || ch > '9' )
        pm_erreur( "read error : != [0-9]" );

    i = 0;
    do
        {
        i = i * 10 + ch - '0';
        ch = pm_getc( file );
        }
    while ( ch >= '0' && ch <= '9' );

    return i;
    }

void pm_erreur(char *texte)
{
  fprintf(stderr, "\n%s \n\n", texte);
  exit(1);
}

void copy_arr(gray* output,gray* graymap, int i , int j ){
  for ( int l = 0 ; l < i ; l++ ){
    for( int c = 0 ; c < j ; c++  ){
      graymap[ l * j + c] = output[ l * j + c] ;
    }
  }
}






void output_util(int i, int j, int k , int l , int maxbinomial,int * s,int * t , int rows , int cols){
  if ( maxbinomial == 3 ){
    switch (k) {
      case 0 :
      *s = i - 1 ;
      break ;
      case 1 :
      *s = i  ;
      break ;
      case 2 :
      *s = i + 1 ;
      break ;
    }

    switch (l) {
      case 0 :
      *t = j - 1 ;

      break ;
      case 1 :
      *t = j ;

      break ;
      case 2 :
      *t = j+1 ;
      break ;
    }

    if ( *s == -1 ) {
      *s = 0 ;
    }
    if ( *s == rows ){
      *s = rows -1 ;
    }

    if ( *t == -1 ) {
      *t = 0 ;
    }
    if ( *t == cols ){
      *t = cols -1 ;
    }
  }



  else {

    switch (k) {
      case 0 :
      *s = i - 2 ;
      break ;
      case 1 :
      *s = i - 1   ;
      break ;
      case 2 :
      *s = i ;
      break ;
      case 3 :
      *s = i + 1 ;
      break ;
      case 4 :
      *s = i + 2 ;
      break ;

    }

    switch (l) {
      case 0 :
      *t = j - 2 ;

      break ;
      case 1 :
      *t = j-1  ;

      break ;
      case 2 :
      *t = j ;
      break ;
      case 3 :
      *s = j + 1  ;
      break ;
      case 4 :
      *s = j + 2 ;
      break ;
    }

    if ( *s < 0 ) {
      *s = 0 ;
    }
    if ( *s >= rows ){
      *s = rows -1 ;
    }

    if ( *t < 0 ) {
      *t = 0 ;
    }
    if ( *t >= cols ){
      *t = cols -1 ;
    }

  }
}


void median_util(int i , int j , gray * graymap , gray * values , int k , int l , int maxbinomial , int rows , int cols) {
  if ( maxbinomial == 3 ){
    switch (k) {
      case 0 :
      s = i - 1 ;
      break ;
      case 1 :
      s = i  ;
      break ;
      case 2 :
      s = i + 1 ;
      break ;
    }

    switch (l) {
      case 0 :
      t = j - 1 ;

      break ;
      case 1 :
      t = j ;

      break ;
      case 2 :
      t = j+1 ;
      break ;
    }

    if ( s == -1 ) {
      s = 0 ;
    }
    if ( s == rows ){
      s = rows -1 ;
    }

    if ( t == -1 ) {
      t = 0 ;
    }
    if ( t == cols ){
      t = cols -1 ;
    }
    values[k * maxbinomial + s] = graymap[s * cols + t] ;
  }



  else {

    switch (k) {
      case 0 :
      s = i - 2 ;
      break ;
      case 1 :
      s = i - 1   ;
      break ;
      case 2 :
      s = i ;
      break ;
      case 3 :
      s = i + 1 ;
      break ;
      case 4 :
      s = i + 2 ;
      break ;

    }

    switch (l) {
      case 0 :
      t = j - 2 ;

      break ;
      case 1 :
      t = j-1  ;

      break ;
      case 2 :
      t = j ;
      break ;
      case 3 :
      s = j + 1  ;
      break ;
      case 4 :
      s = j + 2 ;
      break ;
    }

    if ( s < 0 ) {
      s = 0 ;
    }
    if ( s >= rows ){
      s = rows -1 ;
    }

    if ( t < 0 ) {
      t = 0 ;
    }
    if ( t >= cols ){
      t = cols -1 ;
    }
    values[k * maxbinomial + s] = graymap[s * cols + t] ;
  }
}



void sort(gray * arr,int length){
    for (int i = 0; i < length; i++) {
            for (int j = i+1; j < length; j++) {
               if(arr[i] > arr[j]) {
                   gray temp = arr[i];
                   arr[i] = arr[j];
                   arr[j] = temp;
           }
        }
    }
}

gray getmedian(gray * arr,int length){
    gray median=0;
    if (length%2 == 0)
        median = (arr[(length-1)/2] + arr[length/2])/2;
    else
        median = arr[length/2];

}
