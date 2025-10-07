#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"
#include "atmosphere_data.h"
#include "spawn.h"
//#include "buffer.h"

void main (int argc, char *argv[])
{
  int num_N3, num_H2O;               // Used to store number of processes to create
  int i;                          // Loop index variable
  atmosphere_data *atm;               // Used to get address of shared memory page
  uint32 h_mem;                   // Used to hold handle to shared memory page
  sem_t s_procs_completed;        // Semaphore used to wait until all spawned processes have completed
  char h_mem_str[10];             // Used as command-line argument to pass mem_handle to new processes
  char s_procs_completed_str[10]; // Used as command-line argument to pass page_mapped handle to new processes
  char process_type[3];           // Used as command-line argument to pass process type to new processes
  int status;
  if (argc != 3) {
    Printf("Usage: "); Printf(argv[0]); Printf(" <number of N3 molecules> <number of H2O molecules>, argc=%d\n", argc);
    Exit();
  }

  // Convert string from ascii command line argument to integer number
  num_N3 = dstrtol(argv[1], NULL, 10); // the "10" means base 10
  num_H2O = dstrtol(argv[2], NULL, 10); // the "10" means base 10
  //Printf("Starting reaction between %d N3 molecules and %d H2O molecules\n", num_N3, num_H2O);

  // Allocate space for a shared memory page, which is exactly 64KB
  // Note that it doesn't matter how much memory we actually need: we 
  // always get 64KB
  if ((h_mem = shmget()) == 0) {
    Printf("ERROR: could not allocate shared memory page in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
  //Printf("Shared memory page allocated @ %p\n", h_mem);

  // Map shared memory page into this process's memory space
  if ((atm = (atmosphere_data *)shmat(h_mem)) == NULL) {
    Printf("Could not map the shared page to virtual address in "); Printf(argv[0]); Printf(", exiting..\n");
    Exit();
  }
  //Printf("Shared memory page mapped @ %p\n", atm);

  status = atm_init(atm, num_N3, num_H2O);
  if(status == ATMOSPHERE_DATA_ERROR)
  {
    Printf("ERROR: could not initialize atmosphere data in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
  else
  {
    Printf("Atmosphere data initialized successfully\n\n");
  }

  // Put some values in the shared memory, to be read by other processes
  //mc->numprocs = numprocs;
  //c->really_important_char = 'A';

  // Create semaphore to not exit this process until all other processes 
  // have signalled that they are complete.  To do this, we will initialize
  // the semaphore to (-1) * (number of signals), where "number of signals"
  // should be equal to the number of processes we're spawning - 1.  Once 
  // each of the processes has signaled, the semaphore should be back to
  // zero and the final sem_wait below will return.
  if ((s_procs_completed = sem_create(-(5-1))) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  //Printf("Main process will wait for 5 processes to complete\n");

  // Setup the command-line arguments for the new process.  We're going to
  // pass the handles to the shared memory page and the semaphore as strings
  // on the command line, so we must first convert them from ints to strings.
  ditoa(h_mem, h_mem_str);
  ditoa(s_procs_completed, s_procs_completed_str);
  //p_type[0] = 'p';
  //c_type[0] = 'c';
  // Now we can create the processes.  Note that you MUST end your call to
  // process_create with a NULL argument so that the operating system
  // knows how many arguments you are sending.
  //Printf("Creating 5 processes\n");
  for(i = 0; i < 2; i++)
  {
    process_type[0] = '0'+i;

    process_create(FILENAME_TO_RUN_PRODUCER, h_mem_str, s_procs_completed_str,process_type, NULL);
  }
  for(i = 2; i < 5; i++)
  {    
    process_type[0] = '0'+i;

    process_create(FILENAME_TO_RUN_CONSUMER, h_mem_str, s_procs_completed_str,process_type, NULL);
  }
  /*
  for(i=0; i<5; i++) {
    // Create injection/reaction process
    process_type[0] = '0'+i;
    process_create(FILENAME_TO_RUN, h_mem_str, s_procs_completed_str,process_type, NULL);
    // Create consumer process
    //process_create(FILENAME_TO_RUN, h_mem_str, s_procs_completed_str,c_type, NULL);
    //Printf("Process %d created: type = %c\n", i, process_type[0]);
  }
  */
  // And finally, wait until all spawned processes have finished.
  if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf("\n");
    Exit();
  }
  Printf("All other processes completed, exiting main process.\n");
}
