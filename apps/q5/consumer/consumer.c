#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"
#include "atmosphere_data.h"
#include "spawn.h"
//#include "process.h"
void main (int argc, char *argv[])
{
  atmosphere_data *atm;        // Used to access missile codes in shared memory page
  uint32 h_mem;            // Handle to the shared memory page
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  char p_type; // Type of producer/consumer
  int pid;
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
  if ((atm = (atmosphere_data *)shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
  //Printf("spawn_me: PID %d is type %c\n", pid, p_type);
  if(p_type == '0')
  {
    //Printf("N3 injection started, will perform %d injections\n", atm->inject_N3);
    while (atm->inject_N3 > 0)
    {
      atm_inject_N3(atm);
    }
    //Printf("PID %d: N3 injections complete\n", pid);
  }
  else if(p_type == '1')
  {
    //Printf("H2O injection started, will perform %d injections\n", atm->inject_H2O);
    while (atm->inject_H2O > 0)
    {
      atm_inject_H2O(atm);
    }
    //Printf("PID %d: H2O injections complete\n", pid);
  }
  else if(p_type == '2')
  {
    //Printf("N3 reaction started, will perform %d reactions\n", atm->react_N3);
    while(atm->react_N3 > 0)
    {
      atm_react_N3(atm);
    }
    //Printf("PID %d: N3 reactions complete\n", pid);
  }
  else if(p_type == '3')
  {
    //Printf("H2O reaction started, will perform %d reactions\n", atm->react_H2O);
    while(atm->react_H2O > 0)
    {
      atm_react_H2O(atm);
    }
    //Printf("PID %d: H2O reactions complete\n", pid);
  }
  else if(p_type == '4')
  {
    //Printf("N + O2 reaction started, will perform %d reactions\n", atm->react_NO2);
    while(atm->react_NO2 > 0)
    {
      atm_react_N_O2(atm);
    }
    //Printf("PID %d: N + O2 reactions complete\n", pid);
  }
  // Signal the semaphore to tell the original process that we're done
  //Printf("spawn_me: PID %d is complete.\n", Getpid());
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
