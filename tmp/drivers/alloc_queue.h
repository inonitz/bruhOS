#pragma once
#ifndef __ALLOCATOR_QUEUE__
#define __ALLOCATOR_QUEUE__
#include "alloc_list.h"
#include "../../std/lock.h"


/* 
    NOTE: This is a circular Lock-based queue.
          Currently, this implementation can satisfy a max 512 CPU's Simultaneously.
          If you need more, increase the size of the node[N] structure.

*/
typedef struct __queue_node
{
    alloc_list_t* alloc;
} qnode_t;


typedef struct __allocator_finite_queue
{
    qnode_t    data[512];  
    uint16_t   front;
    uint16_t   rear;
    uint16_t   active;
    spinlock_t lock;
} alloc_queue_t;


/* 
    * Initializes the Queue with an already available allocator list.
    * alloc_queue_t* man  - the queue to initialize.
    * alloc_list*    head - the list which'll be used to init the Queue elements.
*/
void allocator_queue_init(alloc_queue_t* man, alloc_list_t* head, uint16_t allocCount);


/* 
    * Push a qnode_t to the end of the queue.
    * alloc_queue_t* man  - the queue to modify.
    * qnode_t*       item - the node that will be pushed.
*/
void push(alloc_queue_t* man, qnode_t item);


/* 
    * pop a qnode_t from the head of the queue.
    * alloc_queue_t* man  - the queue to modify.
    * qnode_t*       item - the node that will be popped.
*/
qnode_t pop(alloc_queue_t* man);


/* 
    * Self Explanatory.
*/
static __force_inline bool_t isFull(alloc_queue_t*  man)
{
    return man->active == (sizeof(man->data) / sizeof(qnode_t));
}


/* 
    * Self Explanatory.
*/
static __force_inline bool_t isEmpty(alloc_queue_t* man)
{
    return man->active == 0;
}


/* 
    * Print info about the given allocator queue.
    * alloc_queue_t* man - the queue whose info will be printed.
*/
void allocator_queue_info(alloc_queue_t* man);

#endif