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
    fprintf(stderr,"YEAH B-)\n");

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
    first_free->next = NULL;
    next_fit = first_free ;
    /* TODO: start by using the provided my_mmap function to allocate
     * the memory region you are going to manage */

}




void *memory_alloc(size_t size)
{

    /* TODO: insert your code here */

    /* TODO : don't forget to call the function print_alloc_info()
     * appropriately */

    mem_free_block_t *addr, *prev = NULL;

    if ( sizeof( mem_used_block_t  ) + size < sizeof(mem_free_block_t) ){
      addr = (mem_free_block_t*)allocate_memory(sizeof(mem_free_block_t) );
    }
    else {
      addr = (mem_free_block_t*)allocate_memory( size );
    }
    // addr = prev-> next ;

    if ( addr == NULL ){
      print_alloc_error(size) ;
      exit(1) ;
    }
    mem_used_block_t *new_addr;
    prev = find_previous( addr ) ;

    if(addr->size >= ( sizeof(mem_used_block_t) + size)) //case of
    {

      char *new_freeaux = (char *)addr + sizeof(mem_used_block_t)+ size;

      size_t new_block_size = addr->size - sizeof(mem_used_block_t) - size;
      mem_free_block_t *new_free = (mem_free_block_t *)new_freeaux;
      new_free->size = new_block_size;
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

}


void memory_free(void *p)
{

    /* TODO: insert your code here */

    /* TODO : don't forget to call the function print_free_info()
     * appropriately */

     mem_used_block_t *addr = (mem_used_block_t *)p;
     addr--;
     size_t size = addr->size;
     size_t free_size = sizeof(mem_used_block_t) + size - sizeof(mem_free_block_t);

     mem_free_block_t *new_free = (mem_free_block_t * ) addr;
     new_free->size = free_size;
     new_free->next = NULL;

     //new_free = (mem_used_block_t*)addr;
     //new_free ++;
     mem_used_block_t * res =(mem_used_block_t*) insert_in_free_list(new_free);
     //print_mem_state();
     print_free_info(addr+1);



}

size_t memory_get_allocated_block_size(void *addr)
{

    /* TODO: insert your code here */

    return 0;
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
