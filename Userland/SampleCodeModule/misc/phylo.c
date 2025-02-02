// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "sysCalls.h"
#include "stdlib.h"
#include "semLib.h"
#include "procLib.h"
#include "memLib.h"
#include "phylo.h"


#define MAX 8
#define MIN 4
#define MUTEX_ID 999
#define SEM_ID 1000

Philosopher * philos[MAX];
static int currentPhilos = 0;
static int tableMutex;
static int working;

#define RIGHT(i) ((i) + 1) % (currentPhilos)                         
#define LEFT(i) ((i) + currentPhilos - 1) % (currentPhilos) 

void runPhilo(int argc, char *argv[]);
int addPhilo();
int removePhilo();
void attemptForForks(int i);
void releaseForks(int i);
void checkForForks(int i);
void printerAssistant();

void runPhilos(int argc, char *argv[]){
    working = 1;
    tableMutex = semOpen(MUTEX_ID, 1);
    if(tableMutex == -1){
        printf("\nError opening semaphores! Returning...\n");
        return;
    }
    printf("Problema de los filosofos.\n");
    printf("Instrucciones:\n- 'a': Agrega un filosofo\n- 'r': Borra un filosofo\n- 'q': Cierra el programa\nComencemos :)\n\n");
    sleep(2);
    for (int i = 0; i < MIN; i++)
        addPhilo();
    char *args[] = {"printerAssistant"};
    int printerAssistantPid = createProcess(&printerAssistant, 1, args, BG, NULL);
    while (working){
        char key = getchar();
        switch (key){
        case 'a':
            if (addPhilo() == -1)
                printf("No se puede agregar (maximo 8)\n");
            else
                printf("Agregando filosofo...\n");
            break;
        case 'r':
            if (removePhilo() == -1)
                printf("No se puede remover (minimo 4)\n");
            else
                printf("Removiendo filosofo...\n");
            break;
        case 'q':
            printf("\nFinalizando. Hasta la proxima :)\n\n");
            working = 0;
            break;
        default:
            break;
        }
    }

    for (int i = 0; i < currentPhilos; i++)
    {
        semClose(philos[i]->sem);
        killProc(philos[i]->pid);
        freeMem(philos[i]);
    }
    currentPhilos = 0;
    killProc(printerAssistantPid);
    semClose(MUTEX_ID);
}


//    ----------------------------
//    |                          |
//    |       Phylo Logic        |
//    |        functions         |
//    |                          |
//    ----------------------------

int addPhilo(){
    if (currentPhilos == MAX)
        return -1;

    semWait(tableMutex); 

    Philosopher * auxPhilo = malloc(sizeof(Philosopher));
    if (auxPhilo == NULL)
        return -1;

    char buffer[3];
    char * name[] = {"philosopher", itoa(currentPhilos, buffer, 10)};
    auxPhilo->pid = createProcess(&lifecycle, 2, name, BG, NULL);

    auxPhilo->State = THINKING;
    auxPhilo->sem = semOpen(SEM_ID + currentPhilos, 1);

    philos[currentPhilos++] = auxPhilo;

    semPost(tableMutex);

    return 0;
}

int removePhilo()
{
    if (currentPhilos == MIN){
        return -1;
    }

    currentPhilos--;
    Philosopher *chosenPhilo = philos[currentPhilos];
    semClose(chosenPhilo->sem);
    killProc(chosenPhilo->pid);
    freeMem(chosenPhilo);
    semPost(tableMutex);

    return 0;
}


void lifecycle(int argc, char *argv[])
{
    int idx = atoi(argv[1]);
    while (working)
    {
        // Thinking...

        // Time to eat
        attemptForForks(idx);

        // Eating...
        sleep(1); 

        // Done!
        releaseForks(idx);

        // Sleeping...
        sleep(1);
    }
}

void attemptForForks(int i){
    semWait(tableMutex);

    philos[i]->State = HUNGRY;
    checkForForks(i);
    
    semPost(tableMutex);
    
    semWait(philos[i]->sem);
}

void releaseForks(int i){
    semWait(tableMutex);
    philos[i]->State = THINKING;
    checkForForks(LEFT(i));
    checkForForks(RIGHT(i));
    semPost(tableMutex);
}

void checkForForks(int i){
    if (philos[i]->State == HUNGRY && philos[LEFT(i)]->State != EATING && philos[RIGHT(i)]->State != EATING){
        philos[i]->State = EATING;
        semPost(philos[i]->sem);
    }
}



void printerAssistant(int argc, char *argv[]){
    while (working){
        semWait(tableMutex);

        for (int i = 0; i < currentPhilos; i++){
            if(philos[i]->State == EATING){
                putchar('E');
            }
            else{
                putchar('W');
            }
            putchar(' ');
        }
        putchar('\n');

        semPost(tableMutex);

        yieldProc();
    }
}