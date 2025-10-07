#ifndef ATMOSPHERE_DATA_H
#define ATMOSPHERE_DATA_H
#include "lab2-api.h"


// Defines

// Return values
#define ATMOSPHERE_DATA_SUCCESS 0
#define ATMOSPHERE_DATA_ERROR -1

typedef struct atmosphere_data {
    int inject_N3;
    int inject_H2O;
    int react_N3;
    int react_H2O;
    int react_N;
    int num_H2;
    int react_O2;
    int react_NO2;


    //lock_t lock;
    sem_t s_N3;
    sem_t s_H2O;
    sem_t s_N;
    sem_t s_H2;
    sem_t s_O2;
    sem_t s_NO2;
} atmosphere_data;

int atm_init(atmosphere_data *atm, int num_N3, int num_H2O);

// Reaction processes
int atm_react_N3(atmosphere_data *atm);
int atm_react_H2O(atmosphere_data *atm);
int atm_react_N_O2(atmosphere_data *atm);

// Injection processes
int atm_inject_N3(atmosphere_data *atm);
int atm_inject_H2O(atmosphere_data *atm);

#endif // ATMOSPHERE_DATA_H