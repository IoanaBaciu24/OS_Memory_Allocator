/* Headers for PBM+ */
/* E.B.             */

typedef unsigned char bit;
typedef unsigned char gray;

char pm_getc(FILE* file);
bit pm_getbit(FILE* file);
unsigned char pm_getrawbyte(FILE* file);
int pm_getint( FILE* file);

void pm_erreur(char *);

void copy_arr(gray * output,gray * graymap, int i , int j );

void output_util(int i, int j, int k , int l , int maxbinomial,int * s,int * t , int rows , int cols);

void median_util(int i , int j , gray * graymap , gray * values , int k , int l , int maxbinomial , int rows , int cols) ;


void sort(gray * arr,int length) ;


gray getmedian(gray * arr,int length) ;
