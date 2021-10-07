#include <stdlib.h>
#include <stdio.h>
#include "Util.h"



int main(int argc, char* argv[])
    {
    FILE* ifp;
    gray* graymap;
    gray* output ;

    FILE * f1 , *f2;

    int ich1, ich2, rows, cols, maxval=255, pgmraw;
    int i, j;
    int factor3 = 16 ;
    int factor5 = 273 ;
    int filter3[3][3]  = { {1,2,1}, {2,4,2} , {1,2,1} } ;
    int filter5[5][5]  = { {1,4,7,4,1}, {4,16,26,16,4} , {7,26,41,26,7},{4,16,26,16,4} , {1,4,7,4,1} } ;



    /* Arguments */
    if ( argc != 6 ){
      printf("\nUsage: %s file_input  file_output_bin file_output_median integer_n binmax \n\n", argv[0]);
      exit(0);
    }


    int n = atoi( argv [4] ) ;


    int maxbinomial = atoi( argv [5] ) ;
    if (maxbinomial != 3 && maxbinomial != 5 ) {
      printf("maxbinomial error !!! \n ");
      exit(1);
    }



    /* Opening */
    ifp = fopen(argv[1],"r");
    if (ifp == NULL) {
      printf("error in opening file %s\n", argv[1]);
      exit(1);
    }

    f1 = fopen(argv[2],"w") ;

    if (f1 == NULL) {
      printf("error in writing file %s\n", argv[2]);
      exit(1);
    }

    f2 = fopen(argv[3],"w") ;

    if (f2 == NULL) {
      printf("error in writing file %s\n", argv[3]);
      exit(1);
    }

    /*  Magic number reading */
    ich1 = getc( ifp );
    if ( ich1 == EOF )
        pm_erreur( "EOF / read error / magic number" );
    ich2 = getc( ifp );
    if ( ich2 == EOF )
        pm_erreur( "EOF /read error / magic number" );
    if(ich2 != '2' && ich2 != '5')
      pm_erreur(" wrong file type ");
    else
      if(ich2 == '2')
	pgmraw = 0;
      else pgmraw = 1;

    /* Reading image dimensions */
    cols = pm_getint( ifp );
    rows = pm_getint( ifp );
    maxval = pm_getint( ifp );

    /* Memory allocation  */
    graymap = (gray *) malloc(cols * rows * sizeof(gray));

    /* Reading */
    for(i=0; i < rows; i++)
      for(j=0; j < cols ; j++)
        if(pgmraw)
          graymap[i * cols + j] = pm_getrawbyte(ifp) ;
        else
          graymap[i * cols + j] = pm_getint(ifp);

    output = (gray *) malloc(cols * rows * sizeof(gray));


    gray * medianfilter = (gray *) malloc(cols * rows * sizeof(gray));

    gray * values = (gray *) malloc( maxbinomial * sizeof(gray));

    for(i=0; i < rows; i++){
      for(j=0; j < cols ; j++){
        for ( int k = 0 ; k < maxbinomial ; k++ ){
          for (int l = 0;  l< maxbinomial; l++) {
            median_util(i , j , graymap , values , k , l , maxbinomial , rows , cols ) ;
          }
        }
        sort( values , maxbinomial * maxbinomial ) ;
        medianfilter[i * cols + j] =  getmedian (values , maxbinomial * maxbinomial ) ;
      }
    }
  }



    for ( int count =0 ; count < n ; count ++ ){



      for(i=0; i < rows; i++){
        for(j=0; j < cols ; j++){
          int result = 0 ;
          for ( int k = 0 ; k < maxbinomial ; k++ ){
            for (int l = 0;  l< maxbinomial; l++) {

              int s , t  ;
              output_util(i,j,k , l , maxbinomial,&s,&t, rows , cols);
              if ( maxbinomial == 3 ){
                result += filter3[k][l] * graymap[s * cols + t] ;
              }
              else{
                result += filter5[k][l] * graymap[s * cols + t] ;

              }
            }

          }
          if (maxbinomial == 3 ) result /=  factor3 ;
          else  result /=  factor5 ;

          output[i * cols + j] =  result ;

        }

      }
      copy_arr(output,graymap, i , j ) ;

    }


    /* Writing */
    if( ! pgmraw)
      fprintf(f1,"P2\n");
    else
      fprintf(f1,"P5\n");

    fprintf(f1,"%d %d \n", cols, rows);
    fprintf(f1,"%d\n",maxval);

    for(i=0; i < rows; i++){
      for(j=0; j < cols ; j++){
        if( pgmraw)
          fwrite( &output[i * cols + j] ,1 , sizeof(char) , f1  );
          fwrite( &medianfilter[i * cols + j] ,1 , sizeof(char) , f2  );
        else
          fprintf(f1,"%d",output[i * cols + j]);
          fprintf(f2,"%d",medianfilter[i * cols + j]);
        /*putc(graymap[i * cols + j],stdout);*/
      }
    }



      /* Closing */
      fclose(ifp);
      fclose(f1);
      fclose(f2);
      return 0;
}
