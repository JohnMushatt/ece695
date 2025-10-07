#ifndef BUFFER_H
#define BUFFER_H
#include "lab2-api.h"


// Defines
#define BUFFER_SIZE 32

//Return values
#define BUFFER_SUCCESS 0
#define BUFFER_ERROR -1

typedef struct circular_buffer {
    char buffer[BUFFER_SIZE];
    int  head;
    int  tail;
    lock_t lock;   
    sem_t s_fullslots;
    sem_t s_emptyslots;
    cond_t c_fullslots;
    cond_t c_emptyslots;
} circular_buffer;

/**
 * Initialize the buffer
 * @param buffer: The buffer to initialize
 * @return: 0 if successful, -1 if error
 */
int buffer_init(circular_buffer *buffer);

/**
 * Put an item into the buffer
 * @param buffer: The buffer to put the item into
 * @param item: The item to put into the buffer
 * @return: 0 if successful, -1 if error
 */
int buffer_put(circular_buffer *buffer, char item);

/**
 * Get an item from the buffer
 * @param buffer: The buffer to get the item from
 * @param item: The item to get from the buffer
 * @return: 0 if successful, -1 if error
 */
int buffer_get(circular_buffer *buffer, char item);

#endif // BUFFER_H