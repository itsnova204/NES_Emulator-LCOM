#include "byteQueue.h"


Queue * newQueue() {
    Queue * newQueue = malloc(sizeof(Queue));

    newQueue->head = NULL;
    newQueue->tail = NULL;

    return newQueue;
}

inline void push_byte(Queue* queue, uint8_t byte) {
    byte_node* newNode = (byte_node*) malloc(sizeof(byte_node));

    newNode->byte = byte;
    newNode->next = NULL;

    if(queue->head == NULL) {
        queue->head = newNode;
    } else {
        queue->tail->next = newNode;
    }

    queue->tail = newNode;
}


inline uint8_t pop_byte(Queue* queue) {
    if(queue->head == NULL) return 0; //0 and NULL are the same thing in this case
    
    uint8_t pop = queue->head->byte;

    byte_node* item = queue->head;
    queue->head = item->next;
    free(item);
    return pop;
}

inline uint8_t queue_head(Queue* queue) {
    if(queue->head == NULL) return 0;
    return queue->head->byte;
}

inline void queue_pop(Queue* queue) {
    if(queue->head == NULL) return;

    byte_node * item = queue->head;
    queue->head = item->next;
    free(item);
}


inline bool isQueue_empty(Queue* queue) {
    return queue->head == NULL;
}

inline void queue_delete(Queue* queue) {
    while(!isQueue_empty(queue)) {
        pop_byte(queue);
    }

    free(queue);
}

inline void queue_clean(Queue* queue) {
    while(!isQueue_empty(queue)) {
        pop_byte(queue);
    }
}
