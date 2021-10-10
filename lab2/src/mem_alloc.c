#include "mem_alloc.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#include "mem_alloc_types.h"
#include "my_mmap.h"

/* pointer to the beginning of the memory region to manage */
void *heap_start;

/* Pointer to the first free block in the heap */
mem_free_block_t *first_free;


mem_free_block_t * next_fit = NULL ;




#define ULONG(x)((long unsigned int)(x))

#if defined(FIRST_FIT)

/* TODO: code specific to the FIRST FIT allocation policy can be
 * inserted here */

/* You can define here functions that will be compiled only if the
 * symbol FIRST_FIT is defined, that is, only if the selected policy
 * is FF */
 void *allocate_memory(size_t size){
    mem_free_block_t *cur = first_free  ;


    while(cur!=NULL &&  (cur->size + sizeof(mem_free_block_t) < size  + sizeof( mem_used_block_t)  ) )
    {

        cur = cur->next;
    }
    return cur;
 }

#elif defined(BEST_FIT)

/* TODO: code specific to the BEST FIT allocation policy can be
 * inserted here */
 void *allocate_memory(size_t size){
    mem_free_block_t *cur = first_free  ;
    mem_free_block_t *result = NULL  ;


    while(cur!=NULL )
    {
        if ( (cur->size + sizeof(mem_free_block_t) >= size  + sizeof( mem_used_block_t)  )){
          if (result == NULL){
            result = cur ;
          }
          else {
            if ( result -> size > cur->size ){
              result = cur ;
            }
          }
        }
        cur = cur->next;
    }
    return result;
 }

#elif defined(NEXT_FIT)

/* TODO: code specific to the NEXT FIT allocation policy can be
 * inserted here */
 void *allocate_memory(size_t size){
    mem_free_block_t *cur = next_fit  ;


    while(cur!=NULL &&  (cur->size + sizeof(mem_free_block_t) < size  + sizeof( mem_used_block_t)  ) )
    {

        cur = cur->next;
    }
    if ( cur == NULL && next_fit != first_free ){

      cur = first_free ;

      while ( cur < next_fit ) {
        if(   (cur->size + sizeof(mem_free_block_t) >= size  + sizeof( mem_used_block_t)  ) ){
          break ;
        }
        cur = cur->next ;
      }

      if ( cur  == next_fit ) cur = NULL ;
    }

    if ( cur ){
      if(cur->next == NULL){
        next_fit = first_free;
      }
      else {
        next_fit = cur -> next ;
      }
    }



    return cur;
 }
#endif


void run_at_exit(void)
{
    if ( first_free->size == MEMORY_SIZE - sizeof(mem_free_block_t) ) {
      fprintf(stderr,"YEAH B-)\n");
    }
    else {
      fprintf(stderr,"LEAAAAAAAAAAAAAAAAKS \n");
    }
    /* TODO: insert your code here */
}


mem_free_block_t * find_previous( mem_free_block_t * addr ){
  mem_free_block_t * cur = first_free , * prev = NULL;
  while (cur) {
    if ( cur == addr ) return prev ;
    prev = cur ;
    cur = cur-> next ;
  }
  return prev ;

}

void memory_init(void)
{
    /* register the function that will be called when the programs exits */
    atexit(run_at_exit);

    /* TODO: insert your code here */
    heap_start = my_mmap(MEMORY_SIZE);
    size_t s = MEMORY_SIZE - sizeof(mem_free_block_t);
    // int magic = MAGIC;
    first_free = (mem_free_block_t*)heap_start;

    first_free->size = s;
    first_free->check_size = s;
    first_free->magic = MAGIC_FREE;

    first_free->next = NULL;
    next_fit = first_free ;
    /* TODO: start by using the provided my_mmap function to allocate
     * the memory region you are going to manage */

}


int check_mem_used_block ( mem_used_block_t * addr ){
  if(addr == NULL)
  return 1;
  return (addr->size == addr ->check_size) && (addr->magic == MAGIC_USED) ;
}

int check_mem_free_block ( mem_free_block_t * addr ){
  if(addr == NULL)
    return 1;
  if(addr->next != NULL)
  {
    if(addr->next->magic != MAGIC_FREE)
      return 0;
  }
  return  (addr->size == addr->check_size) && (addr->magic == MAGIC_FREE) ;  ;
}

void *memory_alloc(size_t size)
{

    /* TODO: insert your code here */

    /* TODO : don't forget to call the function print_alloc_info()
     * appropriately */

    mem_free_block_t *addr, *prev = NULL;

    if ( sizeof( mem_used_block_t  ) + size < sizeof(mem_free_block_t) ){
      addr = (mem_free_block_t*)allocate_memory(sizeof(mem_free_block_t) - sizeof(mem_used_block_t));
    }
    else {
      addr = (mem_free_block_t*)allocate_memory( size );
    }
    // addr = prev-> next ;
    if ( addr == NULL  ){
      print_alloc_error(size) ;
      exit(1) ;
    }
    if ( check_mem_free_block(addr) == 0  ){
      printf("metadata corruptioooooooooooooooooon !\n" );
      exit(1);
    }
    mem_used_block_t *new_addr;
    prev = find_previous( addr ) ;
    if ( prev && check_mem_free_block(prev) == 0  ){
      printf("metadata corruptioooooooooooooooooon !\n" );
      exit(1);
    }
    if(addr->size >= ( sizeof(mem_used_block_t) + size)) //case of
    {

      char *new_freeaux ;
      size_t new_block_size ;

      if ( sizeof( mem_used_block_t  ) + size < sizeof(mem_free_block_t) ) {
        new_freeaux = (char *)addr + sizeof(mem_free_block_t) ;
        new_block_size = addr->size - sizeof(mem_free_block_t) ;
      }
      else {
        new_freeaux = (char *)addr + sizeof(mem_used_block_t)+ size;
        new_block_size = addr->size - sizeof(mem_used_block_t) - size;
      }


      mem_free_block_t *new_free = (mem_free_block_t *)new_freeaux;
      new_free->size = new_block_size;
      new_free->check_size = new_block_size;
      new_free->magic = MAGIC_FREE;

      if (prev == NULL ){
        new_free->next=first_free->next;
        first_free = new_free;
      }
      else{
        prev->next = new_free;
        new_free->next = addr->next;
      }
      next_fit = new_free ;

      new_addr = (mem_used_block_t*)addr;
      new_addr->size = size;
      new_addr->check_size = size;
      new_addr ->magic = MAGIC_USED ;

      new_addr +=  1 ; //sizeof(mem_used_block_t);

    }
    else{
        // to do: when free, handle first node free
        if ( prev == NULL ){
          first_free = first_free->next ;
          next_fit = first_free ;

        }
        else {
          prev->next = addr->next;
          next_fit = addr -> next ;
        }


        size_t s = addr->size + sizeof(mem_free_block_t) - sizeof(mem_used_block_t);

        new_addr = (mem_used_block_t*)addr;
        new_addr->size = s;
        new_addr->check_size = s;
        new_addr ->magic = MAGIC_USED ;
        new_addr += 1;
    }

    //print_mem_state();
    print_alloc_info(new_addr, size ) ;
    return new_addr ;
}


mem_free_block_t * coalesce(mem_free_block_t *addr1, mem_free_block_t *addr2){

  if((char*)addr2 == ((char*)addr1+sizeof(mem_free_block_t)+addr1->size))
  {
      addr1->next = addr2->next;
      addr1->size += addr2->size + sizeof(mem_free_block_t);
      addr1->check_size = addr1->size;
      if ( next_fit == addr2) {
        next_fit = addr1 ;
      }
      return addr1;
  }
  else return NULL;

}

mem_free_block_t* insert_in_free_list(mem_free_block_t *addr)
{

    mem_free_block_t *cur = first_free, *prev = NULL;

    while(cur!=NULL)

    {
        if(cur>addr)
        {
          if (check_mem_free_block(addr->next) == 0){
            printf("corruptioooooooooooooooooon in mem_free_block \n");
            exit(1);
          }
            if(prev == NULL)
            {
              addr->next = first_free;
              first_free = addr;
              //coalesce
              mem_free_block_t *ret = coalesce(first_free, first_free->next);
              if(ret == NULL) return first_free;
              return ret;
            }
            else{
              addr->next = cur;
              prev->next = addr;
              //coalesce
              mem_free_block_t *result = coalesce(prev, addr);
              if(result)
              {
                coalesce(result, cur);
                return result;
              }
              else{
                coalesce(addr, cur);
                return addr;
              }

            }
            break;
        }
        else{
          prev = cur;
          cur = cur->next;
        }
    }
    return NULL;

}


void memory_free(void *p)
{

     mem_used_block_t *addr  = ( mem_used_block_t * ) (( char * ) p - sizeof(mem_used_block_t) ) ;
    // if ( addr-> magic != MAGIC ){
    //   printf("memory free error ! \n" );
    //   exit(1);
     //}
     //printf("%d :::: %d :::: %d ::: \n",addr->size , addr->check_size , addr->magic );
     if(check_mem_used_block(addr) == 0)
     {
       printf("free error : metadata is corrupted or you tried a double free !!!\n" );
       exit(1);
     }

     size_t size = addr->size;
     addr-> magic = 0 ;
     size_t free_size  ;
     //free_size = sizeof(mem_used_block_t) + size - sizeof(mem_free_block_t);
     if ( sizeof(mem_used_block_t) + size > sizeof(mem_free_block_t)  )
      free_size = sizeof(mem_used_block_t) + size - sizeof(mem_free_block_t);
     else free_size = 0 ;

     mem_free_block_t *new_free = (mem_free_block_t * ) addr;
     new_free->size = free_size;
     new_free -> check_size = free_size ;
     new_free -> magic = MAGIC_FREE ;
     new_free->next = NULL;

     //new_free = (mem_used_block_t*)addr;
     //new_free ++;
     mem_used_block_t * res =(mem_used_block_t*) insert_in_free_list(new_free);
     //print_mem_state();
     print_free_info(addr+1);



}

size_t memory_get_allocated_block_size(void *addr)
{

    mem_used_block_t * p =  ( mem_used_block_t * ) addr ;
    p-- ;
    return p->size ;

}

int isFree(char *addr)
{
  mem_free_block_t *cur = first_free;

  while (cur!=NULL) {

    if(addr>= (char * ) cur+sizeof(mem_free_block_t) && addr < (char*)cur + cur->size + sizeof(mem_free_block_t))
      return 1;
    cur = cur ->next;
    /* code */
  }
  return 0;
}


char * print_util( mem_free_block_t * cur , char * addr  ) {

    if ( (char*) (cur) == addr ){
      char * p ;
      // print * for bytes used for mem_free_block
      for ( p = addr ; p < addr + sizeof(mem_free_block_t) ; p++ ){
        printf("@");
      }
      for (  ; p < (addr + sizeof(mem_free_block_t) + (cur)-> size ); p++ ){
        printf(".");
      }
      return p  ;
    }
      printf("X");
      return  NULL ;
  }


void print_mem_state(void)
{
    /* TODO: insert your code here */
    char * addr = ( char *)heap_start ;
    char * final = ( char *)heap_start + MEMORY_SIZE ;
    mem_free_block_t * curfree =  first_free;

    while ( addr < final ) {
      char * x  = print_util( curfree , addr  ) ;
      if ( x )  {
        addr = x ;
        curfree = curfree->next ;
      }
      else addr++ ;
    }
    printf("\n");
}


void print_info(void) {
    fprintf(stderr, "Memory : [%lu %lu] (%lu bytes)\n", (long unsigned int) heap_start, (long unsigned int) ((char*)heap_start+MEMORY_SIZE), (long unsigned int) (MEMORY_SIZE));
}

void print_free_info(void *addr){
    if(addr){
        fprintf(stderr, "FREE  at : %lu \n", ULONG((char*)addr - (char*)heap_start));
    }
    else{
        fprintf(stderr, "FREE  at : %lu \n", ULONG(0));
    }

}

void print_alloc_info(void *addr, int size){
  if(addr){
    fprintf(stderr, "ALLOC at : %lu (%d byte(s))\n",
	    ULONG((char*)addr - (char*)heap_start), size);
  }
  else{
    fprintf(stderr, "Warning, system is out of memory\n");
  }
}

void print_alloc_error(int size)
{
    fprintf(stderr, "ALLOC error : can't allocate %d bytes\n", size);
}
void test_illegal_free(){

  /* The main can be changed, it is *not* involved in tests */
  memory_init();
  //print_info();
  int i ;
  for( i = 0; i < 10; i++){
    char *b = memory_alloc(rand()%8);
    memory_free(b);
  }

  char * a = memory_alloc(100);
  memory_free(a);



  a = memory_alloc(10);

  memory_free(a);
  memory_free(a);

  fprintf(stderr,"%lu\n",(long unsigned int) (memory_alloc(9)));

}

void test_corruption(){

  /* The main can be changed, it is *not* involved in tests */
  memory_init();
  //print_info();

  char * a = memory_alloc(100);
  mem_used_block_t * add =  ((mem_used_block_t *)a - 1 ) ;
  add -> size = add-> size - 66 ;
  memory_free(a);
  fprintf(stderr,"%lu\n",(long unsigned int) (memory_alloc(9)));

}

#ifdef MAIN
int main(int argc, char **argv){

  /* The main can be changed, it is *not* involved in tests */
  memory_init();
  print_info();
  int i ;
  for( i = 0; i < 10; i++){
    char *b = memory_alloc(rand()%8);
    memory_free(b);
  }

  char * a = memory_alloc(15);
  memory_free(a);


  a = memory_alloc(10);

  memory_free(a);

  fprintf(stderr,"%lu\n",(long unsigned int) (memory_alloc(9)));
  return EXIT_SUCCESS;
}
#endif
