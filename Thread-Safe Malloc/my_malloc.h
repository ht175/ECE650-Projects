#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
typedef struct meta_tag meta_t;
struct meta_tag
{
    meta_t * prev; 
    meta_t * next;
    size_t size;
};
// #define BLOCK_SIZE sizeof(meta_t)
meta_t * allocateNew(size_t size,int sbrk_control);
meta_t * findFreeSpace( size_t size,meta_t **  head);
void coalesce(meta_t * left,meta_t * right,meta_t **  head);
void seperateSpace(meta_t * cur,size_t size,meta_t **  head);
unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();
void removeFree(meta_t * cur, meta_t **  head);
void addFree(meta_t * cur, meta_t **  head);
void *bf_malloc(size_t size, int sbrk_control,meta_t **  head);
void bf_free(void *ptr,meta_t **  head);
meta_t * findbestFree(size_t size,meta_t **  head);
void printFreeList();
void *ts_malloc_lock(size_t size);
void ts_free_lock(void *ptr);
void *ts_malloc_nolock(size_t size);
void ts_free_nolock(void *ptr);
