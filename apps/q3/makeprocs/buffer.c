#include "buffer.h"
#include "usertraps.h"

/**
 * Initialize the buffer
 * @param buffer: The buffer to initialize
 * @return: BUFFER_SUCCESS if the buffer is initialized successfully, BUFFER_ERROR otherwise
 */
int buffer_init(circular_buffer *buffer) {
    // Set head = tail to indicate empty buffer
    buffer->head = 0;
    buffer->tail = 0;

    // Create a lock for the buffer
    buffer->lock = lock_create();

    // Check if lock was created successfully
    if (buffer->lock == SYNC_FAIL) {
        Printf("Failed to create lock for buffer\n");
        return BUFFER_ERROR;
    }

    if( (buffer->s_fullslots = sem_create(0)) == SYNC_FAIL) {
        Printf("Failed to create semaphore for full slots\n");
        return BUFFER_ERROR;
    }
    if( (buffer->s_emptyslots = sem_create(BUFFER_SIZE)) == SYNC_FAIL) {
        Printf("Failed to create semaphore for empty slots\n");
        return BUFFER_ERROR;
    }
    Printf("Buffer using semaphores and locks for synchronization\n");
    return BUFFER_SUCCESS;
}
/**
 * Put an item into the buffer
 * @param buffer: The buffer to put the item into
 * @param item: The item to put into the buffer
 * @return: BUFFER_SUCCESS if the item is put into the buffer successfully, BUFFER_ERROR otherwise
 */
int buffer_put(circular_buffer *buffer, char item) {

    if( buffer == NULL)
    {
        Printf("Buffer is NULL\n");
        return BUFFER_ERROR;
    }
    if( sem_wait(buffer->s_emptyslots) != SYNC_SUCCESS) {
        Printf("Failed to wait for empty slots\n");
        return BUFFER_ERROR;
    }
    if( lock_acquire(buffer->lock) != SYNC_SUCCESS) {
        Printf("Failed to acquire lock\n");
        return BUFFER_ERROR;
    }
    buffer->buffer[buffer->head] = item;
    buffer->head = (buffer->head+1) % BUFFER_SIZE;

    if( lock_release(buffer->lock) != SYNC_SUCCESS) {
        Printf("Failed to release lock\n");
        return BUFFER_ERROR;
    }
    if (sem_signal(buffer->s_fullslots) != SYNC_SUCCESS) {
        Printf("Failed to signal full slots\n");
        return BUFFER_ERROR;
    }

    return BUFFER_SUCCESS;
}
/**
 * Get an item from the buffer
 * @param buffer: The buffer to get the item from
 * @param item: The item to get from the buffer
 * @return: 0 if successful, -1 if error
 */
int buffer_get(circular_buffer *buffer, char *item)
{
    if( buffer == NULL)
    {
        Printf("Buffer is NULL\n");
        return BUFFER_ERROR;
    }
    if( sem_wait(buffer->s_fullslots) != SYNC_SUCCESS) {
        Printf("Failed to wait for full slots\n");
        return BUFFER_ERROR;
    }
    if( lock_acquire(buffer->lock) != SYNC_SUCCESS) {
        Printf("Failed to acquire lock\n");
        sem_signal(buffer->s_fullslots);
        return BUFFER_ERROR;
    }
    if( buffer->buffer[buffer->tail] == *item) {
        *item = buffer->buffer[buffer->tail];
        buffer->tail = (buffer->tail + 1) % BUFFER_SIZE;
        lock_release(buffer->lock);
        sem_signal(buffer->s_emptyslots);
        return BUFFER_SUCCESS;
    }
    else 
    {
        lock_release(buffer->lock);
        sem_signal(buffer->s_fullslots);
        return BUFFER_ERROR;
    }

}
