#ifndef PPM_SPLT
#define  PPM_SPLT

typedef struct ppm{
  gray* graymap;
  int ich1 , ich2 ;
  int rows , col ;
  int maxval ;
} ppm ;

void print_rgb_int(FILE *f1 , FILE *f2 , FILE *f3 , int e ) ;

void print_rgb_char(FILE *f1 , FILE *f2 , FILE *f3 , int c ) ;

void print_rgb_line(FILE *f1 , FILE *f2 , FILE *f3  ) ;

void print_rgb_space(FILE *f1 , FILE *f2 , FILE *f3  ) ;

void print_rgb_color(FILE *f1 , FILE *f2 , FILE *f3 , int i , char d ) ;

void ppm_split(char * str) ;




#endif
