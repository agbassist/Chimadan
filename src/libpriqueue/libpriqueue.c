/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libpriqueue.h"


/**
  Initializes the priqueue_t data structure.

  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements.
  See also @ref comparer-page
 */
void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *))
{
        /* constructer which sets the parametes given */
        q->size = 0;
        q->head = NULL;
        q->comparer = comparer;
}


/**
  Inserts the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{
        /* Checks if Head is empty */
        if(q->head == NULL)
        {
                /* Create a Head */
                q->head = malloc(sizeof(task_t));
                q->head->ptr = ptr;
                q->head->next = NULL;
        }
        /* The Queue is not empty */
        else
        {
                /* Create a temp variable for the old head */
                task_t * temp = q->head;
                task_t * placeholder = q->head;

                /* Loop through Queue */
                while(temp != NULL)
                {
                        /* Compare values */
                        if(q->comparer(ptr,temp->ptr) < 0)
                        {
                                /* Check if the result is the head */
                                if(temp == q->head)
                                {
                                        /* Create task_t object */
                                        q->head = malloc(sizeof(task_t));
                                        q->head->ptr = ptr;
                                        q->head->next = temp;
                                }
                                else
                                {
                                        /* Create task_t object */
                                        placeholder->next = malloc(sizeof(task_t));
                                        placeholder = placeholder->next;
                                        placeholder->ptr = ptr;
                                        placeholder->next = temp;
                                }
                                break; //if the right place is found, no need to continue while loop
                        }
                        /* Increment the place in the loop and have a rider that sits one back */
                        placeholder = temp;
                        temp = temp->next;
                }
                /* If it gets to the end of the queue */
                if(temp == NULL)
                {
                        /* Create task_t object */
                        temp = malloc(sizeof(task_t));
                        temp->ptr = ptr;
                        temp->next = NULL;
                        placeholder->next = temp;
                }

        }
        q->size++;
        return q->size;
}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.

  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q)
{
        return q->head;
}


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.

  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */
void *priqueue_poll(priqueue_t *q)
{
        /* Create temp of the Head */
        task_t * temp = q->head;
        /* If the Queue is empty */
        if(temp == NULL)
        {
                return NULL;
        }
        /* If the Queue is not empty */
        else
        {
                /* Create temp of the Head */
                task_t * temp = q->head;
                /* If Head the only object in the Queue */
                if(temp->next != NULL)
                {
                        q->head = temp->next;
                }
                else
                /* If the Size of the Queue is 1 */
                {
                        q->head = NULL;
                }
                void* data = temp->ptr;
                free(temp);
                q->size--;

                return(data);
        }
}

/**
  Returns the element at the specified position in this list, or NULL if
  the queue does not contain an index'th element.

  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of retrieved element
  @return the index'th element in the queue
  @return NULL if the queue does not contain the index'th element
 */
void *priqueue_at(priqueue_t *q, int index)
{

        /* if the index is at the head */
        if(index==0)
        {
                return q->head->ptr;
        }

        /* Create a temp task of the head */
        task_t * temp = q->head;
        int count = 0;
        /* Loop until the right value */
        while(temp != NULL && count < index)
        {
                temp = temp->next;
                count = count + 1;
        }

        return temp->ptr;
}


/**
  Removes all instances of ptr from the queue.

  This function should not use the comparer function, but check if the data contained in each element of the queue is equal (==) to ptr.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr address of element to be removed
  @return the number of entries removed
 */
int priqueue_remove(priqueue_t *q, void *ptr)
{
        /* If the Queue is empty */
        if(q->head == NULL)
        {
                return 0;
        }
        /* If the Queue is not empty */
        else
        {
                /* Create two temps for the Head */
                task_t * temp = q->head; //used to traverse queue
                task_t * prev = q->head; //stores the previous temp value, used for reassigning next values
                int count = 0; //used to count how many have been removed
                /* while temp is not at the end */
                while(temp != NULL)
                {
                        /* If we have a match */
                        if(temp->ptr == ptr)
                        {
                                 /* if the head is a match */
                                if(temp == q->head)
                                {
                                        /* reset the head */
                                        q->head = q->head->next;
                                }
                                else
                                {
                                        /* Continue Looping */
                                        prev->next = temp->next;
                                }
                                /* Free the temp task */
                                free(temp);
                                /* Change the head */
                                temp = q->head;
                                prev = q->head;
                                q->size--;
                                count++;
                        }
                        else
                        {
                        /* Increment the Loop */
                        prev = temp;
                        temp = temp->next;
                        }


                }

                return count;
        }
}


/**
  Removes the specified index from the queue, moving later elements up
  a spot in the queue to fill the gap.

  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of element to be removed
  @return the element removed from the queue
  @return NULL if the specified index does not exist
 */
void *priqueue_remove_at(priqueue_t *q, int index)
{
        /* Create two temps for the head of the Queue */
        task_t * temp = q->head;
        task_t * prev = q->head;
        int count = 0;

        /* Loop until the right index */
        while(temp != NULL && count <= index)
        {
                /* Increment the values for the loop */
                prev = temp;
                temp = temp->next;
                count = count + 1;
        }

        /* If the node to remove is the head of the Queue */
        if(temp == q-> head)
        {
                q->head = temp->next;
        }
        else
        {
                prev->next = temp->next;
        }

        q->size--;
        return temp;
}


/**
  Returns the number of elements in the queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
        return q->size;
}


/**
  Destroys and frees all the memory associated with q.

  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{
        /* Loop through the Queue Deleting each one */
        while(q->head != NULL)
        {
                priqueue_poll(q);
        }
}

/**
  Prints what is in the Queue

  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_print(priqueue_t *q)
{
        /* Create a temp for the head of the */
        task_t* temp = q->head;

        /* If the Queue is not empty */
        if(temp != NULL)
        {
                printf("[");
                /* Loop inside of the head to print out the contents of the Queue */
                while(temp != NULL)
                 {
                        if(temp->next != NULL)
                        {
                                /* Print the contents of the Queue */
                                printf("%i, ", *(int *)temp->ptr);
                        }
                        else
                        {
                                /* Print the contents of the Queue */
                                printf("%i]\n", *(int *)temp->ptr);
                                temp = temp->next;
                        }
                }
        }
        else
        {
                /* Print an empty Queue */ 
                printf("[]");
        }
}
