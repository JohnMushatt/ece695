#include "atmosphere_data.h"
#include "usertraps.h"

// Reaction #defines

#define N3_TO_N 3
#define H2O_TO_H2 2
#define H2O_TO_O2 1



int atm_init(atmosphere_data *atm, int num_N3, int num_H2O)
{
    int status = ATMOSPHERE_DATA_SUCCESS;
    // Molecules to inject
    atm->inject_N3 = num_N3; // Number of N3 molecules
    atm->inject_H2O = num_H2O; // Number of H2O molecules
    Printf("\nInject:\n%d N3\n%d H2O\n", atm->inject_N3, atm->inject_H2O);

    atm->react_N3 = atm->inject_N3; // Number of N3 molecules to react
    atm->react_H2O = atm->inject_H2O / 2; // Number of H2O molecules to react
    Printf("React:\n%d N3 -> N + N + N\n%d 2H2O -> 2H2 + O2\n", atm->react_N3, atm->react_H2O);
    // Molecules to react

    // Availbe N atoms
    atm->react_N = atm->react_N3 * 3;
    //Available O2 molecules
    atm->react_O2 = atm->react_H2O;
    //Printf("Available N and O2 molecules: %d %d\n", atm->react_N, atm->react_O2);
    if(atm->react_N > atm->react_O2)
    {
        //Printf("N is in excess, limiting NO2 reaction to # of O2 molecules [%d]\n", atm->react_O2);
        atm->react_NO2 = atm->react_O2;
    }
    else
    {
        //Printf("O2 is in excess, limiting NO2 reaction to # of N molecules [%d]\n", atm->react_N);
        atm->react_NO2 = atm->react_N;
    }
    Printf("%d N + O2 -> NO2\n\n", atm->react_NO2);


    // Base reactions are limited by number of N3 and H2O

   
    // Initialize semaphores
    atm->s_N3 = sem_create(0);
    atm->s_H2O = sem_create(0);
    atm->s_N = sem_create(0);
    atm->s_H2 = sem_create(0);  
    atm->s_O2 = sem_create(0);
    atm->s_NO2 = sem_create(0);

    // Check if semaphores were created successfully
    if(atm->s_N3 == SYNC_FAIL || atm->s_H2O == SYNC_FAIL || atm->s_N == SYNC_FAIL || atm->s_H2 == SYNC_FAIL || atm->s_O2 == SYNC_FAIL || atm->s_NO2 == SYNC_FAIL)
    {
        Printf("Failed to create semaphores\n");
        status = ATMOSPHERE_DATA_ERROR;
    }

    
    return status;
}

int atm_inject_N3(atmosphere_data *atm)
{
    int i;
    // Signal semaphore for N3
    atm->inject_N3--;
    if(sem_signal(atm->s_N3) != SYNC_SUCCESS)
    {
        Printf("Failed to signal semaphore for N3\n");
        return ATMOSPHERE_DATA_ERROR;
    }
    //Printf("An N3 molecule is created, %d left to inject\n", atm->inject_N3);
    Printf("A N3 molecule is created\n");

    return ATMOSPHERE_DATA_SUCCESS;
}
int atm_inject_H2O(atmosphere_data *atm)
{
    int i;

    atm->inject_H2O--;
    if(sem_signal(atm->s_H2O) != SYNC_SUCCESS)
    {
        Printf("Failed to signal semaphore for H2O\n");
        return ATMOSPHERE_DATA_ERROR;
    }
    //Printf("An H2O molecule is created, %d left to inject\n", atm->inject_H2O);
    Printf("A H2O molecule is created\n");

    return ATMOSPHERE_DATA_SUCCESS;
}

int atm_react_N3(atmosphere_data *atm)
{
    //1 N3 = 3 N atoms
    int moleculesCreated =3;
    int i;
    // Wait on N3 semaphore
    if( sem_wait(atm->s_N3) != SYNC_SUCCESS)
    {
        Printf("Failed to wait on N3 semaphore\n");
        return ATMOSPHERE_DATA_ERROR;
    }
    //Printf("Acquired N3 semaphore\n");
    atm->react_N3--;

    for( i = 0; i < moleculesCreated; i++)
    {
        if(sem_signal(atm->s_N) != SYNC_SUCCESS)
        {
            Printf("Failed to signal semaphore for N\n");
            return ATMOSPHERE_DATA_ERROR;
        }
        else
        {
            Printf("A N molecule is created\n");
        }
    }
    return ATMOSPHERE_DATA_SUCCESS;
}
/* 
* 2 H2O molecules react to make 2 H2 and 1 O2
*/
int atm_react_H2O(atmosphere_data *atm)
{

    // Wait on 1 H2O semaphore
    if( sem_wait(atm->s_H2O) != SYNC_SUCCESS)
    {
        Printf("Failed to wait on 1st H2O semaphore\n");
        return ATMOSPHERE_DATA_ERROR;
    }
    //Printf("Acquired 1st H2O semaphore\n");
    // Wait on 2 H2 semaphores
    if( sem_wait(atm->s_H2O) != SYNC_SUCCESS)
    {
        Printf("Failed to wait on 2nd H2 semaphore\n");
        return ATMOSPHERE_DATA_ERROR;
    }
    //Printf("Acquired 2nd H2O semaphore\n");
    atm->react_H2O--;
    // If we get here we have 2 H2O
    if(sem_signal(atm->s_H2) != SYNC_SUCCESS)
    {
        Printf("Failed to signal semaphore for H2\n");
        return ATMOSPHERE_DATA_ERROR;
    }
    Printf("A H2 molecule is created\n");
    if(sem_signal(atm->s_H2) != SYNC_SUCCESS)
    {
        Printf("Failed to signal semaphore for H2\n");
        return ATMOSPHERE_DATA_ERROR;
    }
    Printf("A H2 molecule is created\n");
    if(sem_signal(atm->s_O2) != SYNC_SUCCESS)
    {
        Printf("Failed to signal semaphore for O2\n");
        return ATMOSPHERE_DATA_ERROR;
    }
    Printf("An O2 molecule is created\n");

    return ATMOSPHERE_DATA_SUCCESS;
}
int atm_react_N_O2(atmosphere_data *atm)
{
    if( sem_wait(atm->s_N) != SYNC_SUCCESS)
    {
        Printf("Failed to wait on N semaphore\n");
        return ATMOSPHERE_DATA_ERROR;
    }
    if( sem_wait(atm->s_O2) != SYNC_SUCCESS)
    {
        Printf("Failed to wait on O2 semaphore\n");
        return ATMOSPHERE_DATA_ERROR;
    }
    atm->react_NO2--;
    Printf("A NO2 molecule is created\n");
    return ATMOSPHERE_DATA_SUCCESS;
}
