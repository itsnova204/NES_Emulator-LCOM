#pragma once

#include <lcom/lcf.h>

typedef struct byte_node {
    uint8_t byte;
   struct byte_node* next;
} byte_node;

typedef struct Queue {
    struct byte_node* head;
    struct byte_node* tail;
}Queue;

uint8_t queue_front(Queue* queue);

void queue_pop(Queue* queue);

Queue * newQueue();

void push_byte(Queue* queue, uint8_t byte);


uint8_t pop_byte(Queue* queue);

bool isQueue_empty(Queue* queue);

void queue_delete(Queue* queue);

void queue_clean(Queue* queue);
