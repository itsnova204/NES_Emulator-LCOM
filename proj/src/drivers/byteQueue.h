#pragma once

/**
 * @file byteQueue.h
 * @brief This file contains the implementation of a Queue data structure that stores 1 byte per node.
 * 
*/

#include <lcom/lcf.h>

/**
 * @brief Struct that represents a node of the queue
*/
typedef struct byte_node {
    uint8_t byte;
   struct byte_node* next;
} byte_node;

/**
 * @brief Struct that represents a Queue
*/
typedef struct Queue {
    struct byte_node* head;
    struct byte_node* tail;
}Queue;

/**
 * @brief Gets the head of the queue
 * @param queue Queue to get the head from
 * @return the value of the head of the queue
*/
uint8_t queue_head(Queue* queue);

/**
 * @brief Pops the head of the queue
 * @param queue Queue to pop the head from
*/
void queue_pop(Queue* queue);

/**
 * @brief Creates a new Queue
 * @return the new Queue
*/
Queue* newQueue();

/**
 * @brief Pushes a byte to the queue
 * @param queue Queue to push the byte to
 * @param byte Byte to push to the queue
*/
void push_byte(Queue* queue, uint8_t byte);

/**
 * @brief Pops a byte from the queue
 * @param queue Queue to pop the byte from
 * @return the byte that was popped
*/
uint8_t pop_byte(Queue* queue);

/**
 * @brief Checks if the queue is empty
 * @param queue Queue to check if it is empty
 * @return true if the queue is empty, false otherwise
*/
bool isQueue_empty(Queue* queue);

/**
 * @brief Deletes the queue and frees the memory
 * @param queue Queue to delete
*/
void queue_delete(Queue* queue);

/**
 * @brief Cleans the queue resetting the head and tail to NULL and freeing the memory
 * @param queue Queue to clean
*/
void queue_clean(Queue* queue);
