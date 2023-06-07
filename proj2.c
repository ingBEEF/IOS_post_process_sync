/*
 * IOS projekt 2 (synchronizace)
 *
 * author: Jan Velich
 * login: xvelic05
 * 
 * file: proj2.c
 */

#include "proj2.h"

void check_args (int argc, char** argv){
    int check;
    char* r;
    if (argc != 6){
        fprintf(stderr, "ERR:Nespravny pocet argumentu\n");
        exit(1);
    }
    check = strtol(argv[1], &r, INT_BASE);
    if ((check < 0) || (*r != '\0')){
        fprintf(stderr, "ERR:Nevalidni argument NZ\n");
        exit(1);
    }
    check = atoi(argv[2]);
    if (check <= 0){
        fprintf(stderr, "ERR:Nevalidni argument NU\n");
        exit(1);
    }
    check = strtol(argv[3], &r, INT_BASE);
    if ((check < 0) || (check > 10000) || (*r != '\0')){
        fprintf(stderr, "ERR:Nevalidni argument TZ\n");
        exit(1);
    }
    check = strtol(argv[4], &r, INT_BASE);
    if ((check < 0) || (check > 100) || (*r != '\0')){
        fprintf(stderr, "ERR:Nevalidni argument TU\n");
        exit(1);
    }
    check = strtol(argv[5], &r, INT_BASE);
    if ((check < 0) || (check > 10000) || (*r != '\0')){
        fprintf(stderr, "ERR:Nevalidni argument F\n");
        exit(1);
    }
}

void init (char** argv){
    file = fopen("proj2.out", "w");
    if (!file){
        fprintf(stderr, "ERR:Chyba pri otevirani souboru\n");
        exit(1);
    }
    MAP(nz);
    *nz = atoi(argv[1]);
    MAP(nu);
    *nu = atoi(argv[2]);
    MAP(tz);
    *tz = atoi(argv[3]);
    MAP(tu);
    *tu = atoi(argv[4]);
    MAP(f);
    *f = atoi(argv[5]);
    MAP(p_open);
    *p_open = 1;
    MAP(z_left);
    *z_left = 0;
    MAP(id_z);
    *id_z = 0;
    MAP(id_u);
    *id_u = 0;
    MAP(op);
    *op = 1;
    MAP(p1);
    *p1 = 0;
    MAP(p2);
    *p2 = 0;
    MAP(p3);
    *p3 = 0;
    SMAP(s_z_id);
    sem_init(s_z_id, 1, 1);
    SMAP(s_u_id);
    sem_init(s_u_id, 1, 1);
    SMAP(s_pick);
    sem_init(s_pick, 1, 1);
    SMAP(s_u1);
    sem_init(s_u1, 1, 1);
    SMAP(s_u2);
    sem_init(s_u2, 1, 1);
    SMAP(s_u3);
    sem_init(s_u3, 1, 1);
    SMAP(s_ord1);
    sem_init(s_ord1, 1, 0);
    SMAP(s_ord2);
    sem_init(s_ord2, 1, 0);
    SMAP(s_ord3);
    sem_init(s_ord3, 1, 0);
    SMAP(s_op);
    sem_init(s_op, 1, 1);
}

void z_do (int id, int service, int wait_time, sem_t* s_ord, sem_t* s_u){
            sem_wait(s_op);
            fprintf(file, "%d: Z %d: entering office for a service %d\n", *op, id, service);
            fflush(file);
            (*op)++;
            sem_post(s_op);
            // Zakaznik bude cekat na zavolani urednikem
            sem_wait(s_u);
            
            sem_wait(s_op);
            fprintf(file, "%d: Z %d: called by office worker\n", *op, id);
            fflush(file);
            (*op)++;
            sem_post(s_op);
            sem_post(s_ord);
            // Zakaznik obslouzen a uspan
            wait_time = (rand() % 11) * TIME_M;
            usleep(wait_time);
}

void zakaznik (){
    srand(time(NULL));
    int id, wait_time, service;
    // prichod zakaznika - nastaveni id, vyber servisu
    sem_wait(s_z_id);
    (*z_left)++;
    (*id_z)++;
    id = *id_z;
    sem_post(s_z_id);
    sem_wait(s_op);
    fprintf(file, "%d: Z %d: started\n", *op, id);
    fflush(file);
    (*op)++;
    sem_post(s_op);
    wait_time = (rand() % (*tz + 1)) * TIME_M;
    usleep(wait_time);
    if ((*p_open) == 0){ //Posta je zavrena - prisel pozde
        sem_wait(s_z_id);
        (*z_left)--;
        sem_post(s_z_id);
        sem_wait(s_op);
        fprintf(file, "%d: Z %d: going home\n", *op, id);
        fflush(file);
        (*op)--;
        sem_post(s_op);
        return;
    }
    else{ //Prideleni cinnosti
        service = rand() % 3 + 1;
    }
    // Zakaznik bude zarazen do rady podle cinnosti
    switch (service){
        case 1:
            z_do(id, service, wait_time, s_ord1, s_u1);
        break;
        case 2:
            z_do(id, service, wait_time, s_ord2, s_u2);
        break;
        case 3:
            z_do(id, service, wait_time, s_ord3, s_u3);
        break;
        default:
        break;
    }
    sem_wait(s_z_id);
    (*z_left)--;
    sem_post(s_z_id);
    sem_wait(s_op);
    fprintf(file, "%d: Z %d: going home\n", *op, id);
    fflush(file);
    (*op)++;
    sem_post(s_op);
}

void u_do (int id, int wait_time, int service, int *p, sem_t* s_u, sem_t* s_ord){
    int s2;
    sem_getvalue(s_u, &s2);
    // Jestli je nekdo ve fronte
    if (s2 == 0){
        sem_post(s_u); //Obslouzeni zakaznika
        sem_wait(s_ord);
        sem_wait(s_op);
        fprintf(file, "%d: U %d: serving a service of type %d\n", *op, id, service);
        fflush(file);
        (*op)++;
        sem_post(s_op);
        wait_time = (rand() % 11) * TIME_M;
        usleep(wait_time);
        sem_wait(s_op);
        fprintf(file, "%d: U %d: service finished\n", *op, id);
        fflush(file);
        (*op)++;
        sem_post(s_op);
        sem_wait(s_pick);
        *p = 0; //Uvolneni prepazky
        sem_post(s_pick);
    }
    else{ // Nikdo ve fronte - prestavka
        sem_wait(s_pick);
        *p = 0;
        sem_post(s_pick);
        sem_wait(s_op);
        fprintf(file, "%d: U %d: taking break\n", *op, id);
        fflush(file);
        (*op)++;
        sem_post(s_op);
        wait_time = (rand() % (*tu + 1)) * TIME_M;
        usleep(wait_time);
        sem_wait(s_op);
        fprintf(file, "%d: U %d: break finished\n", *op, id);
        fflush(file);
        (*op)++;
        sem_post(s_op);
    }
}

void urednik (){
    srand(time(NULL));
    int id, wait_time, service;
    // Prichod urednika - nastaveni id
    sem_wait(s_u_id);
    (*id_u)++;
    id = *id_u;
    sem_post(s_u_id);
    sem_wait(s_op);
    fprintf(file, "%d: U %d: started\n", *op, id);
    fflush(file);
    (*op)++;
    sem_post(s_op);
    while ((*z_left > 0) || (*p_open == 1)){
        // Vyber prepazky
        sem_wait(s_pick);
        if ((*p1 == 0) && (*p2 == 0) && (*p3 == 0)){
            service = rand() % 3;
            service++;
        }
        else if ((*p1 == 1) && (*p2 == 0) && (*p3 == 0)){
            service = rand() % 2;
            service += 2;
        }
        else if ((*p1 == 1) && (*p2 == 1) && (*p3 == 0)){
            service = 3;
        }
        else if ((*p1 == 0) && (*p2 == 1) && (*p3 == 0)){
            service = rand() % 2;
            service *= 2;
            service++;
        }
        else if ((*p1 == 0) && (*p2 == 0) && (*p3 == 1)){
            service = rand() % 2;
            service++;
        }
        else if ((*p1 == 0) && (*p2 == 1) && (*p3 == 1)){
            service = 1;
        }
        else if ((*p1 == 1) && (*p2 == 0) && (*p3 == 1)){
            service = 2;
        }
        else{
            service = 0;
        }
        switch (service){
            //Obsazeni prepazky
            case 1:
                *p1 = 1;
                break;
            case 2:
                *p2 = 1;
                break;
            case 3:
                *p3 = 1;
                break;
            default:
                break;
        }
        sem_post(s_pick);
        switch (service){
            case 1:
                u_do(id, wait_time, service, p1, s_u1, s_ord1);
            break;
            case 2:
                u_do(id, wait_time, service, p2, s_u2, s_ord2);
            break;
            case 3:
                u_do(id, wait_time, service, p3, s_u3, s_ord3);
            break;
            default: // Prepazky plne - prestavka
                sem_wait(s_op);
                fprintf(file, "%d: U %d: taking break\n", *op, id);
                fflush(file);
                (*op)++;
                sem_post(s_op);
                wait_time = (rand() % (*tu + 1)) * TIME_M;
                usleep(wait_time);
                sem_wait(s_op);
                fprintf(file, "%d: U %d: break finished\n", *op, id);
                fflush(file);
                (*op)++;
                sem_post(s_op);
            break;
        }
    }
    sem_wait(s_op);
    fprintf(file, "%d: U %d: going home\n", *op, id);
    fflush(file);
    (*op)++;
    sem_post(s_op);
}

void gen_process (){
    int i, wait_time;
    wait_time = ((*f / 2) + rand() % (*f / 2 + 1)) * TIME_M;
    pid_t pr;
    for (i = 0; i < *nz; i++){
        pr = fork();
        if (!pr){ //Vytvoreni child procesu
            zakaznik();
            exit(0);
        }
        else if (pr < 0) { //Chyba funkce fork
            fprintf(stderr, "ERR:Chyba funkce fork\n");
            dest();
            kill(0, SIGKILL);
            exit(1);
        }
    }
    for (i = 0; i < *nu; i++){ //Vytvoreni child procesu
        pr = fork();
        if (!pr){ 
            urednik();
            exit(0);
        }
        else if (pr < 0){ //Chyba funkce fork
            fprintf(stderr, "ERR:Chyba funkce fork\n");
            dest();
            kill(0, SIGKILL);
            exit(1);
        }
    }
    usleep(wait_time);
    sem_wait(s_op);
    fprintf(file, "%d: closing\n", *op); //Zavreni posty
    fflush(file);
    (*op)++;
    sem_post(s_op);
    *p_open = 0;
    
    while (wait(NULL) > 0); //Program pocka az skonci vsechny procesy
}

void dest (){
    UNMAP(nz);
    UNMAP(nu);
    UNMAP(tz);
    UNMAP(tu);
    UNMAP(f);
    UNMAP(p_open);
    UNMAP(z_left);
    UNMAP(id_z);
    UNMAP(id_u);
    UNMAP(op);
    UNMAP(p1);
    UNMAP(p2);
    UNMAP(p3);
    sem_destroy(s_z_id);
    sem_destroy(s_u_id);
    sem_destroy(s_pick);
    sem_destroy(s_u1);
    sem_destroy(s_u2);
    sem_destroy(s_u3);
    sem_destroy(s_ord1);
    sem_destroy(s_ord2);
    sem_destroy(s_ord3);
    sem_destroy(s_op);
    fclose(file);
}

int main (int argc, char** argv){
    srand(time(NULL));
    check_args(argc, argv);
    init (argv);
    gen_process();
    dest();
    return 0;
}
