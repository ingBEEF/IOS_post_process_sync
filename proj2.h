/*
 * IOS projekt 2 (synchronizace)
 *
 * author: Jan Velich
 * login: xvelic05
 * 
 * file: proj2.h
 */

#ifndef PROJ2_H
#define PROJ2_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

#define INT_BASE 10 //Soustava pro konverzi
#define TIME_M 1000 //Konstanta pro jednotku casu v milisekundach

// Makra pro praci se sdilenou pameti

#define MAP(ptr) (ptr) = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); //Makro pro namapovani sdilene pameti
#define UNMAP(ptr) munmap((ptr), sizeof(int)); //Makro pro odmapovani sdilene pameti
#define SMAP(ptr) (ptr) = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); //Makro pro namapovani semaforu

FILE* file; //Vystupni soubor

//Globalni pocitadla

int* nz; //Pocet zakazniku
int* nu; //Pocet uredniku
int* tz; //Max cas cekani zakaznika
int* tu; //Max cas cekani urednika
int* f; //Max cas do zavreni posty
int* p_open; //Otevreni posty
int* z_left; //Pocet zakazniku na poste
int* id_z; //ID zakaznika
int* id_u; //ID urednika
int* op;// Cislo operace
int* p1;// Obsazenost prepazky 1
int* p2;// Obsazenost prepazky 2
int* p3;// Obsazenost prepazky 3

//Semafory

sem_t *s_z_id; //Modifikace poctu zakazniku
sem_t* s_u_id; //Modifikace poctu uredniku
sem_t* s_pick; //Zmena obsazenosti prepazek
sem_t* s_u1; //Rizeni cinnosti na prepazce 1
sem_t* s_u2; //Rizeni cinnosti na prepazce 2
sem_t* s_u3; //Rizeni cinnosti na prepazce 3
sem_t* s_ord1; //Spravne poradi vypisu na prepazce 1
sem_t* s_ord2; //Spravne poradi vypisu na prepazce 1
sem_t* s_ord3; //Spravne poradi vypisu na prepazce 1
sem_t* s_op; //Semafory

/**
 * @brief Kontrola vstupnich argumentu programu
 * 
 * @param argc Pocet vstupnich argumentu
 * @param argv Pole s hodnotami vstupnich argumentu
 */
void check_args (int argc, char** argv);

/**
 * @brief Inicializace - alokace sdilene pameti a otevreni souboru pro zapis vystupu programu
 * 
 * @param argv Pole s hodnotami vstupnich argumentu
 */
void init (char** argv);

/**
 * @brief Vnitrni funkce simulujici cinnost zakaznika po prideleni servisu do obslouzeni urednikem 
 * 
 * @param id ID zakaznika
 * @param service Cislo prepazky, ktera byla zakaznikovi prirazena
 * @param wait_time Nahodne vygenerovany cas pro cekani zakaznika
 * @param s_ord Semafor zarizujici spravne poradi vypisu
 * @param s_u Semafor pro rizeni cinnosti zakaznika
 */
void z_do (int id, int service, int wait_time, sem_t* s_ord, sem_t* s_u);

/**
 * @brief Funkce pro obsluhu procesu zakaznik
 * 
 */
void zakaznik ();

/**
 * @brief Vnitrni funkce simulujici cinnost urednika pri obsluze zakaznika
 * 
 * @param id ID urednika
 * @param wait_time Nahodne vygenerovany cas pro cekani urednika
 * @param service Cislo prepazky pridelene urednikovi
 * @param p Globalni pocitadlo obsazenosti prepazky
 * @param s_u Semafor pro rizeni cinnosti urednika
 * @param s_ord Semafor zarizujici spravne poradi vypisu
 */
void u_do (int id, int wait_time, int service, int *p, sem_t* s_u, sem_t* s_ord);

/**
 * @brief Funkce pro obsluhu procesu urednik
 * 
 */
void urednik ();

/**
 * @brief Funkce pro generovani procesu zakaznik a urednik
 * 
 */
void gen_process ();

/**
 * @brief Uvolneni sdilene pameti
 * 
 */
void dest ();

#endif
