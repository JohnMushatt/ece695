#include "buffer.h"
#include "usertraps.h"

/**
 * Initialize the buffer
 * @param buffer: The buffer to initialize
 * @return: BUFFER_SUCCESS if the buffer is initialized successfully, BUFFER_ERROR otherwise
 */
int buffer_init(circular_buffer *buffer) {
    int status = BUFFER_SUCCESS;
    // Set head = tail to indicate empty buffer
    buffer->head = 0;
    buffer->tail = 0;

    // Create a lock for the buffer
    buffer->lock = lock_create();

    // Check if lock was created successfully
    if (buffer->lock == SYNC_FAIL) {
        Printf("Failed to create lock for buffer\n");
        status = BUFFER_ERROR;
    }

    if( (buffer->c_fullslots = cond_create(buffer->lock)) == SYNC_FAIL)
    {
        Printf("Failed to create condition variable for full slots\n");
        status = BUFFER_ERROR;
    }
    if( (buffer->c_emptyslots = cond_create(buffer->lock)) == SYNC_FAIL)
    {
        Printf("Failed to create condition variable for empty slots\n");
        status = BUFFER_ERROR;
    }
    else {
        Printf("Buffer.c using condition variables for synchronization\n");
    }
    return status;
}
/**
 * Put an item into the buffer
 * @param buffer: The buffer to put the item into
 * @param item: The item to put into the buffer
 * @return: BUFFER_SUCCESS if the item is put into the buffer successfully, BUFFER_ERROR otherwise
 */
int buffer_put(circular_buffer *buffer, char item) {
    int status = BUFFER_ERROR;
    // Attempt to acquire lock
    if (buffer == NULL)
    {
        Printf("Buffer is NULL\n");
        return BUFFER_ERROR;
    }
    if (lock_acquire(buffer->lock) != SYNC_SUCCESS)
    {
        Printf("Failed to acquire lock\n");
        return BUFFER_ERROR;
    }
    // Instead wait for an empty slot to be avilalbe
    while( ((buffer->head + 1) % BUFFER_SIZE) == buffer->tail)
    {
        if (cond_wait(buffer->c_emptyslots) != SYNC_SUCCESS)
        {
            Printf("Failed to wait for full slots\n");
            return BUFFER_ERROR;
        }
    }
    buffer->buffer[buffer->head] = item;
    buffer->head = (buffer->head + 1) % BUFFER_SIZE;
    status = BUFFER_SUCCESS;
    if (cond_signal(buffer->c_fullslots) != SYNC_SUCCESS)
    {
        Printf("Failed to signal condition variable for empty slots\n");
        return BUFFER_ERROR;
    }
    lock_release(buffer->lock);
    return status;
}
/**
 * Get an item from the buffer
 * @param buffer: The buffer to get the item from
 * @param item: The item to get from the buffer
 * @return: 0 if successful, -1 if error
 */
int buffer_get(circular_buffer *buffer, char item)
{
    int status = BUFFER_ERROR;
    // Check for bad buffer
    if(buffer == NULL)
    {
        Printf("buffer_get:Process %d Buffer is NULL\n", Getpid());
        return BUFFER_ERROR;
    }
    if (lock_acquire(buffer->lock) != SYNC_SUCCESS)
    {
        Printf("Failed to acquire lock\n");
        return BUFFER_ERROR;
    }
    while (buffer->head == buffer->tail)
    {
        if( cond_wait(buffer->c_fullslots) != SYNC_SUCCESS)
        {
            Printf("Failed to wait for full slots\n");
            return BUFFER_ERROR;
        }
    }
    if (buffer->buffer[buffer->tail] != item)
    {
        Printf("Item %c not found in buffer\n", item);
        lock_release(buffer->lock);
        return BUFFER_ERROR;
    }
    //item = buffer->buffer[buffer->tail];
    buffer->tail = (buffer->tail + 1) % BUFFER_SIZE;
    status = BUFFER_SUCCESS;
    if (cond_signal(buffer->c_emptyslots) != SYNC_SUCCESS)
    {
        Printf("Failed to signal condition variable for empty slots\n");
        return BUFFER_ERROR;
    }
    lock_release(buffer->lock);
    return status;
}
