// #include <stdio.h>
// #include <unistd.h>
// #include <assert.h>
// typedef struct meta_tag meta_t;
// struct meta_tag
// {
//     int free;//if avaliable 0 for not avaliable 1 for avaliable
//     meta_t * prev; 
//     meta_t * next;
//     size_t size;
//     meta_t * next_free;
//     meta_t * pre_free;
// };
// meta_t * allocateNew(size_t size);
// meta_t * findFreeSpace( size_t size);
// void coalesce(meta_t * left,meta_t * right);
// void seperateSpace(meta_t * cur,size_t size);
// int checkVaildPointer(void * p);
// void ff_free(void * p);
// void * ff_malloc(size_t size);
// unsigned long get_data_segment_size();
// unsigned long get_data_segment_free_space_size();
// void removeFree(meta_t * cur);
// void addFreeToFront(meta_t * cur);
// void *bf_malloc(size_t size);
// void bf_free(void *ptr);
// meta_t * findbestFree(size_t size);
// void printFreeList();
// void printRealList();
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
typedef struct meta_tag meta_t;
struct meta_tag
{
    int free;//if avaliable 0 for not avaliable 1 for avaliable
    meta_t * prev; 
    meta_t * next;
    size_t size;
};
#define BLOCK_SIZE sizeof(meta_t)
meta_t * allocateNew(size_t size);
meta_t * findFreeSpace( size_t size);
void coalesce(meta_t * left,meta_t * right);
void seperateSpace(meta_t * cur,size_t size);
void ff_free(void * p);
void * ff_malloc(size_t size);
unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();
void removeFree(meta_t * cur);
void addFree(meta_t * cur);
void *bf_malloc(size_t size);
void bf_free(void *ptr);
meta_t * findbestFree(size_t size);
void printFreeList();
