#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"
#include "buffer.h"
#include "spawn.h"
//#include "process.h"
void main (int argc, char *argv[])
{
  circular_buffer *cb;        // Used to access missile codes in shared memory page
  uint32 h_mem;            // Handle to the shared memory page
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  char p_type; // Type of producer/consumer
  int pid;
  char str[10] = "0123456789";
  int index = 0;
  if (argc != 4) { 
    Printf("Got argc=%d, Usage: ", argc); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore> <type of producer/consumer>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  h_mem = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  s_procs_completed = dstrtol(argv[2], NULL, 10);
  p_type = argv[3][0];
  pid = Getpid();
  // Map shared memory page into this process's memory space
  if ((cb = (circular_buffer *)shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
  //Printf("spawn_me: PID %d is %c w/ shared memory page @ %p\n", pid, p_type, cb);
  // Now print a message to show that everything worked
  //Printf("spawn_me: This is one of the %d instances you created.  ", mc->numprocs);
  //Printf("spawn_me: Missile code is: %c\n", mc->really_important_char);
  //Printf("spawn_me: My PID is %d\n", Getpid());

 
  while(index < 10)
  {
    if(buffer_put(cb, str[index]) == BUFFER_SUCCESS)
    {
      Printf("Producer %d inserted: %c\n", pid, str[index]);
      index++;
    }
    else
    {
      Printf("Producer %d failed to insert: %c\n", pid, str[index]);
    }
  }
  
  
  // Signal the semaphore to tell the original process that we're done
  //Printf("spawn_me: PID %d is complete.\n", Getpid());
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
