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


#define ULONG(x)((long unsigned int)(x))

#if defined(FIRST_FIT)

/* TODO: code specific to the FIRST FIT allocation policy can be
 * inserted here */

/* You can define here functions that will be compiled only if the
 * symbol FIRST_FIT is defined, that is, only if the selected policy
 * is FF */
 void *allocate_memory(size_t size){
    mem_free_block_t *cur = first_free , *prev ;


    while(cur!=NULL && cur->size < ( size +  sizeof( mem_used_block_t ) ) )
    {
        prev = cur;
        cur = cur->next;
    }
    return prev;
 }

#elif defined(BEST_FIT)

/* TODO: code specific to the BEST FIT allocation policy can be
 * inserted here */

#elif defined(NEXT_FIT)

/* TODO: code specific to the NEXT FIT allocation policy can be
 * inserted here */

#endif


void run_at_exit(void)
{
    fprintf(stderr,"YEAH B-)\n");

    /* TODO: insert your code here */
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
      prev = (mem_free_block_t*)allocate_memory(sizeof(mem_free_block_t) );
    }
    else {
      prev = (mem_free_block_t*)allocate_memory( size );
    }
    addr = prev-> next ;

    if ( addr == NULL ){
      print_alloc_error(size) ;
      exit(1) ;
    }
    mem_used_block_t *new_addr;

    if(addr->size > (sizeof(mem_free_block_t) + sizeof(mem_used_block_t) + size))
    {

      mem_free_block_t *new_free = addr + sizeof(mem_used_block_t) + size;
      size_t new_block_size = addr->size - sizeof(mem_used_block_t) - size;
      new_free->size = new_block_size;
      prev->next = new_free;
      new_free->next = addr->next;

      new_addr = (mem_used_block_t*)addr;
      new_addr->size = size;
      new_addr += sizeof(mem_used_block_t);

    }
    else{
        prev->next = addr->next;
        size_t s = addr->size + sizeof(mem_free_block_t) - sizeof(mem_used_block_t);

        new_addr = (mem_used_block_t*)addr;
        new_addr->size = s;
        new_addr += sizeof(mem_used_block_t);
    }


    print_alloc_info(new_addr , size ) ;
    return new_addr ;
}

void memory_free(void *p)
{

    /* TODO: insert your code here */

    /* TODO : don't forget to call the function print_free_info()
     * appropriately */

}

size_t memory_get_allocated_block_size(void *addr)
{

    /* TODO: insert your code here */

    return 0;
}


void print_mem_state(void)
{
    /* TODO: insert your code here */
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
