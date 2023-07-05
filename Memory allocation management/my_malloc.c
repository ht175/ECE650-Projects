#include "my_malloc.h"
unsigned long full_size=0;
unsigned long free_size=0;
meta_t * head;
meta_t * allocateNew(size_t size){
    meta_t * p=sbrk(size+BLOCK_SIZE); 
    full_size +=size+sizeof(meta_t);
    if(p==(void *)-1){
        return NULL; //allocate failure
    }
    else{
        p->next=NULL;
        p->prev=NULL;
        p->size=size;    
    }
    return p;
}
meta_t * findFreeSpace( size_t size){
    meta_t * cur = head;
    while(cur!=NULL){
      if(cur->size>=size){
            return cur;
        }
        cur=cur->next;
    }
    return NULL;
}
void addFree(meta_t * p){
   //add to HEAD?
   if(p<head || head==NULL){
     p->next=head;
     if(head){
        head->prev=p;
     }
     head=p;
   }
   // need to search where to put p 
   else{
    meta_t * cur=head;
    while(cur->next !=NULL && p>=cur->next){ //break condition p<cur->next //cur->next ==NULL
          cur=cur->next;
    }
    //now p<cur->next
     // cur<p<next
    //add to TAIL?
    if(cur->next==NULL){
        p->next=NULL;
        p->prev=cur;
        cur->next=p;
    }
    else{     //add to MID
        
        p->prev=cur;
        p->next=cur->next;
        cur->next->prev=p;
        cur->next=p;
    }
    
   }


}
void removeFree(meta_t * cur){
    meta_t * prev=cur->prev;
    meta_t * next=cur->next;
    //remove cur
    cur->next=NULL;
    cur->prev=NULL;
   // change cur's next and pre to relink to link list
    if(prev ==NULL && next==NULL){ //only one list
        head=NULL;
    }else if(next==NULL){//TAIL
       prev->next=NULL;
    }else if(prev==NULL){//HEAD
        head=next;
        head->prev=NULL;
    }else{
        prev->next=next;
        next->prev=prev;
    }
}
void seperateSpace(meta_t * cur,size_t size){
    //jump to spilt point
    meta_t * spilt=(void *)cur+BLOCK_SIZE+size;
    //change spilt size
    spilt->size=cur->size-size-BLOCK_SIZE;
    //add spilt into free list
    addFree(spilt);
    //change cur size to size
    cur->size=size;
    //remove cur from free list
    removeFree(cur);
    //add spilt into free list
}

void * ff_malloc(size_t size){
    meta_t * re_pointer; //point at the appropriate meta info space

        //need to search if available block exists
    meta_t * availabe = findFreeSpace(size);
    if(availabe==NULL){ 
            re_pointer=allocateNew(size);
    }else{
        re_pointer=availabe;
     if(availabe->size > size + sizeof(struct meta_tag)){
                seperateSpace(availabe,size);
        }else{
        
                // availabe->size=size;
                removeFree(availabe);
            }
           
     }
    
    return (void *) re_pointer + sizeof(meta_t);
}

void ff_free(void * p){
    //get pointer point to struct
    meta_t * meta= p-BLOCK_SIZE;
    //add meta to free list
    addFree(meta);
    //check pre and next to coalse
    if(meta->next!=NULL &&(void *) meta->next==(void *)meta+BLOCK_SIZE+meta->size){
        coalesce(meta,meta->next);
    }
    if(meta->prev!=NULL && (void *)meta==(void *)meta->prev+BLOCK_SIZE+meta->prev->size){
        coalesce(meta->prev,meta);

    }
}

void bf_free(void *ptr){
    ff_free(ptr);
}

meta_t * findbestFree(size_t size){
    //assert(free_head);s
     meta_t * cur=head;
     meta_t * best=NULL;
     while(cur!=NULL){
        if(cur->size==size){ //first best fit
            return cur;
        }   
        if(cur->size>size){ //may not be the best, update best
            if(best==NULL || best->size>cur->size){
                best=cur;
            }
        }         
        cur=cur->next;
    }
    return best;
}

void *bf_malloc(size_t size){
       meta_t * re_pointer; //point at the appropriate meta info space
        //need to search if available block exists
        meta_t * availabe = findbestFree(size);
        if(availabe==NULL){ 
            re_pointer=allocateNew(size);
        }else{
            re_pointer=availabe;
            if(availabe->size > size + sizeof(struct meta_tag)){
                seperateSpace(availabe,size);
            }else{
                removeFree(availabe);
            }
           
        }
    
    return (void *) re_pointer + sizeof(meta_t);

}

void coalesce(meta_t * left, meta_t * right){
    left->size += right->size + sizeof(struct meta_tag);
    left->next = right->next;
    if(right->next != NULL){
        right->next->prev = left;
    }
    removeFree(right);
}

unsigned long get_data_segment_size(){
    return full_size;
}

unsigned long get_data_segment_free_space_size(){
    // return free_size;
    meta_t * cur=head;
    unsigned long free_space=0;

    while (cur!=NULL)
    {
     free_space+=cur->size+BLOCK_SIZE;
     cur = cur->next;
    }
    return free_space;
}

void printFreeList(){
    meta_t * cur=head;
    printf("print free list begin \n");
    while (cur!=NULL)
    {
        printf("free list cur %p and size %ld  \n",cur,cur->size);
        cur=cur->next;
    }
}