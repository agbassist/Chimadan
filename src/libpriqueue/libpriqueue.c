/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>

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
    if(q->head == NULL){
        q->head = malloc(sizeof(task_t));
        q->head->ptr = ptr;
        q->head->next = NULL;
    }
    else{
        task_t * temp = q->head;
        task_t * placeholder = q->head;

        while(temp != NULL){
            if(q->comparer(ptr,temp->ptr) < 0){

                if(temp == q->head){
                    q->head = malloc(sizeof(task_t));
                    q->head->ptr = ptr;
                    q->head->next = temp;
                }
                else{
                    placeholder->next = malloc(sizeof(task_t));
                    placeholder = placeholder->next;
                    placeholder->ptr = ptr;
                    placeholder->next = temp;
                }

                break; //if the right place is found, no need to continue while loop
            }

            placeholder = temp;
            temp = temp->next;
        }

        if(temp == NULL){
            //if it gets to the end of the queue
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
	task_t * temp = q->head;
    if(temp == NULL){
        return NULL;
    }
    else{
        if(temp->next != NULL){
            q->head = temp->next;
        }
        else{ //size of queue is 1
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

    if(index==0){
        return q->head->ptr;
    }
    
    task_t * temp = q->head;
    int count = 0;

    while(temp != NULL && count < index){
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
    if(q->head == NULL){
        return 0;
    }
    else{
        task_t * temp = q->head; //used to traverse queue
        task_t * prev = q->head; //stores the previous temp value, used for reassigning next values
        int count = 0; //used to count how many have been removed

        while(temp != NULL){ //while temp is not at the end
            //printf("The values being compared are: ");
            //printf("%i", *(int *)ptr);
            //printf(" , ");
            //printf("%i\n", *(int *)(temp->ptr));

            //priqueue_print(q);

            if(temp->ptr == ptr){ //if we have a match

                if(temp == q->head){ //if the head is a match
                    q->head = q->head->next; //reset the head
                }
                else{
                    prev->next = temp->next;
                }
                free(temp);
                temp = q->head;
                prev = q->head;
                q->size--;
                count++;
            }
            else{
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
    task_t * temp = q->head;
    task_t * prev = q->head;
    int count = 0;

    while(temp != NULL && count <= index){
        prev = temp;
        temp = temp->next;
        count = count + 1;
    }

    if(temp == q-> head){
        q->head = temp->next;
    }
    else{
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
    while(q->head != NULL){
         priqueue_poll(q);
    }
}

void priqueue_print(priqueue_t *q)
{
  task_t* temp = q->head;

  if(temp != NULL) {
    printf("[");

    while(temp != NULL) {
      if(temp->next != NULL) {
        printf("%i, ", *(int *)temp->ptr);
      }
      else
        printf("%i]\n", *(int *)temp->ptr);
      temp = temp->next;
    }
  }
  else {
    printf("[]");
  }
}
