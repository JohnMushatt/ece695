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

    // Check if lock was created successfully
    if ((buffer->lock = lock_create()) == SYNC_FAIL) {
        Printf("Failed to create lock for buffer: %d\n", buffer->lock);
        return BUFFER_ERROR;
    }
    Printf("Buffer.c using locks for synchronization\n");
   
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
    if( lock_acquire(buffer->lock) != SYNC_SUCCESS)
    {
        Printf("Failed to acquire lock\n");
        return BUFFER_ERROR;
    }
    if( (buffer->head + 1) % BUFFER_SIZE != buffer->tail)
    {
        buffer->buffer[buffer->head] = item;
        buffer->head = (buffer->head+1) % BUFFER_SIZE;
    }
    else
    {
        Printf("Buffer is full\n");
        lock_release(buffer->lock);
        return BUFFER_ERROR;
    }
    lock_release(buffer->lock);
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
    if( lock_acquire(buffer->lock) != SYNC_SUCCESS)
    {
        Printf("Failed to acquire lock\n");
        return BUFFER_ERROR;

    }
    if( buffer->head != buffer->tail)
    {
        // Store item in pointer
        *item = buffer->buffer[buffer->tail];
        // Update tail w/ wrapping
        buffer->tail = (buffer->tail + 1) % BUFFER_SIZE;
    }
    else
    {
        Printf("Buffer is empty\n");
        lock_release(buffer->lock);
        return BUFFER_ERROR;
    }
    lock_release(buffer->lock);
    return BUFFER_SUCCESS;
}
