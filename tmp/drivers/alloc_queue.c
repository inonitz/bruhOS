#include "alloc_queue.h"
#include "../../std/atomic.h"
#include "../../std/io.h"



#define QUEUE_SIZE      512
#define QUEUE_EMPTY_IDX 0xFFFF



void allocator_queue_init(alloc_queue_t* man, alloc_list_t* head, uint16_t allocCount)
{
    alloc_list_t* tmp = head;
    man->rear        = 0xFFFF;
    man->front       = 0;
    man->active      = 0;
    man->lock.__lock = 0;
    memset(man->data, 0x00, sizeof(man->data));
    for(uint16_t i = 0; i < allocCount; ++i)
    {
        alloc_queue_push(man, *(qnode_t*)&tmp->data);
        tmp = tmp->next;
    }
}


void alloc_queue_push(alloc_queue_t* man, qnode_t item)
{
    if(unlikely(isFull(man))) return;
    lock(&man->lock);

    man->rear            = (man->rear + 1) % QUEUE_SIZE; // mod will become shift right
    man->data[man->rear] = item;
    ++man->active;

    unlock(&man->lock);
    return;
}


qnode_t alloc_queue_pop(alloc_queue_t* man)
{
    if(unlikely(isEmpty(man))) return (qnode_t){ nullptr };
    lock(&man->lock);

    qnode_t* val = &man->data[man->front];
    
    man->front = (man->front + 1) % QUEUE_SIZE;
    --man->active;
    
    unlock(&man->lock);
    return *val;
}




void alloc_queue_info(alloc_queue_t* man)
{
    printk("Circular Queue");
}



