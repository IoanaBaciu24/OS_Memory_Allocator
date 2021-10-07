#ifndef   	_MEM_ALLOC_TYPES_H_
#define   	_MEM_ALLOC_TYPES_H_
#include <stdint.h>

#define MAGIC_USED 54321
#define MAGIC_FREE 12345


/* Structure declaration for a free block */
struct mem_free_block{
    int magic;
    size_t size;
    size_t check_size;
    /* TODO: DEFINE */
    struct mem_free_block *next;
};
typedef struct mem_free_block mem_free_block_t;

/* Specific metadata for used blocks */
struct mem_used_block{
    int magic;
    size_t size;
    size_t check_size ;
    /* TODO: DEFINE */
};
typedef struct mem_used_block mem_used_block_t;


#endif
