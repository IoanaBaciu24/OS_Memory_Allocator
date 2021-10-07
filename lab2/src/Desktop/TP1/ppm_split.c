#include <stdlib.h>
#include <stdio.h>
#include "Util.h"
#include "ppm_split.h"


void print_rgb_int(FILE *f1 , FILE *f2 , FILE *f3 , int e ){
  fprintf( f1, "%d", e );
  fprintf( f2, "%d", e );
  fprintf( f3, "%d", e );
}

void print_rgb_char(FILE *f1 , FILE *f2 , FILE *f3 , int c ){
  fprintf( f1, "%c", c );
  fprintf( f2, "%c", c );
  fprintf( f3, "%c", c );
}

void print_rgb_line(FILE *f1 , FILE *f2 , FILE *f3  ){
  fprintf( f1, "\n" );
  fprintf( f2, "\n");
  fprintf( f3, "\n" );
}

void print_rgb_space(FILE *f1 , FILE *f2 , FILE *f3  ){
  fprintf( f1, " " );
  fprintf( f2, " ");
  fprintf( f3, " " );
}

void print_rgb_color(FILE *f1 , FILE *f2 , FILE *f3 , int i , char d ){
  char p = 0 ;
  switch (i) {
    case 0 :
      fwrite( &d ,1 , sizeof(char) , f1  );
      fwrite( &p ,1 , sizeof(char) ,  f2 );
      fwrite( &p ,1 , sizeof(char) ,   f3);
    break ;
    case 1 :
      fwrite( &p ,1 , sizeof(char) , f1 );
      fwrite( &d ,1 , sizeof(char) , f2  );
      fwrite( &p ,1 , sizeof(char) , f3  );
    break ;
    case 2 :
      fwrite( &p ,1 , sizeof(char) ,f1  );
      fwrite( &p ,1 , sizeof(char) ,  f2 );
      fwrite( &d ,1 , sizeof(char) ,  f3 );
    break ;
  }
}



void ppm_split(char * str){
  FILE *f1 = fopen(str,"r");
  if (f1 == NULL) {
    printf("error in opening file %s\n",str);
    exit(1);
  }

  int i, j;
  int pgmraw ;

  FILE *red  = fopen("t1.ppm","w");
  FILE *green = fopen("t2.ppm","w");
  FILE *blue = fopen("t3.ppm","w");

  char c = getc( f1 );
  if ( c == EOF )
      pm_erreur( "EOF / read error / magic number" );


  print_rgb_char(red, green , blue , c ) ;

  c = getc( f1 );

  if ( c == EOF )
      pm_erreur( "EOF / read error / magic number" );

    if( c == '3' || c == '1' || c == '2' )
  pgmraw = 0;
    else pgmraw = 1;
    print_rgb_char(red, green , blue , c ) ;

    print_rgb_line(red, green, blue ) ;

    /* Reading image dimensions */
    int cols = pm_getint( f1 );
    print_rgb_int(red , green , blue , cols ) ;

    print_rgb_space(red, green, blue ) ;

    int rows = pm_getint( f1 );
    print_rgb_int(red , green , blue , rows ) ;

    print_rgb_line(red, green, blue ) ;

    int maxval = pm_getint( f1 );
    print_rgb_int(red , green , blue , maxval ) ;

    print_rgb_line(red, green, blue ) ;

    for(i=0; i < rows; i++){

      for(j=0; j < cols ; j++){
        char d  ;

        //if(pgmraw)
          d = pm_getrawbyte(f1) ;
        //else
          //d = pm_getint(f1);

        //d = pm_getc(f1);
        print_rgb_color(red,green,blue , j%3 , d) ;
        //print_rgb_space(red, green, blue ) ;

        }
        //print_rgb_line(red, green, blue ) ;

        }

 fclose(f1);
 fclose(red);
 fclose(green);
 fclose(blue);


}




int main(int argc, char const *argv[]) {
  if ( argc != 2 ){
    printf("\nUsage: %s file \n\n", argv[0]);
    exit(0);
  }
  ppm_split(argv[1]);

  return 0;
}
