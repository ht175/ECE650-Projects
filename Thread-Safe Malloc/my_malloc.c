#include "my_malloc.h"
meta_t * lock_head = NULL;
__thread meta_t * thread_head = NULL;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *ts_malloc_lock(size_t size){
    //lock 
    pthread_mutex_lock(&lock);
    int sbrk_control = 0;
    void * ans=bf_malloc(size,sbrk_control,&lock_head);
    pthread_mutex_unlock(&lock);
    return ans;
}

void ts_free_lock(void *ptr){
    pthread_mutex_lock(&lock);
    bf_free(ptr,&lock_head);
    pthread_mutex_unlock(&lock);
}

void *ts_malloc_nolock(size_t size){
    int sbrk_control = 1;
    void * ans=bf_malloc(size,sbrk_control,&thread_head);
    return ans;
}

void ts_free_nolock(void *ptr){
    bf_free(ptr,&thread_head);
}

meta_t * allocateNew(size_t size,int sbrk_control){
    meta_t * p = NULL;
    if(sbrk_control==1){
        pthread_mutex_lock(&lock);
        p=sbrk(size+sizeof(meta_t)); 
        pthread_mutex_unlock(&lock);
    }else{
        p=sbrk(size+sizeof(meta_t));        
    }
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

void addFree(meta_t * p, meta_t **  head){
   //add to HEAD?
   if(p<*head || *head==NULL){
    //  p->prev =NULL;
     p->next= *head;
     if(*head){
        (*head)->prev=p;
     }
     *head=p;
   }
   // need to search where to put p 
   else{
    meta_t * cur=*head;
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

void removeFree(meta_t * cur,meta_t **  head){
    meta_t * prev=cur->prev;
    meta_t * next=cur->next;
    //remove cur
    cur->next=NULL;
    cur->prev=NULL;
   // change cur's next and pre to relink to link list
    if(prev ==NULL && next==NULL){ //only one list
        *head=NULL;
    }else if(next==NULL){//TAIL
       prev->next=NULL;
    }else if(prev==NULL){//HEAD
        *head=next;
        (*head)->prev=NULL;
    }else{
        prev->next=next;
        next->prev=prev;
    }
}

void seperateSpace(meta_t * cur,size_t size,meta_t **  head){
    //jump to spilt point
    meta_t * spilt=(void *)cur+sizeof(meta_t)+size;
    //change spilt size
    spilt->size=cur->size-size-sizeof(meta_t);
    //add spilt into free list
    addFree(spilt,head);
    //change cur size to size
    cur->size=size;
    //remove cur from free list
    removeFree(cur,head);
    //add spilt into free list
}



void bf_free(void * p, meta_t **  head){
    //get pointer point to struct
    meta_t * meta= p-sizeof(meta_t);
    //add meta to free list
    addFree(meta,head);
    //check pre and next to coalse
    if(meta->next!=NULL &&(void *) meta->next==(void *)meta+sizeof(meta_t)+meta->size){
        coalesce(meta,meta->next,head);
    }
    if(meta->prev!=NULL && (void *)meta==(void *)meta->prev+sizeof(meta_t)+meta->prev->size){
        coalesce(meta->prev,meta,head);

    }
}



meta_t * findbestFree(size_t size,meta_t **  head){
 
    //assert(free_head);s
     meta_t * cur=*head;
     meta_t * best=NULL;
     while(cur!=NULL){
         if(cur->size==size){ //first best fit
            return cur;
            break;
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

void *bf_malloc(size_t size, int sbrk_control, meta_t **  head){
       meta_t * re_pointer; //point at the appropriate meta info space
        //need to search if available block exists
        meta_t * availabe = findbestFree(size,head);
        if(availabe==NULL){ 
            re_pointer=allocateNew(size,sbrk_control);
        }else{
            re_pointer=availabe;
            if(availabe->size > size + sizeof(struct meta_tag)){
                seperateSpace(availabe,size,head);
            }else{
                removeFree(availabe,head);
            }
           
        }
    
    return (void *) re_pointer + sizeof(meta_t);

}

void coalesce(meta_t * left, meta_t * right,meta_t **  head){
    left->size += right->size + sizeof(struct meta_tag);
    left->next = right->next;
    if(right->next != NULL){
        right->next->prev = left;
    }
    removeFree(right,head);
}


