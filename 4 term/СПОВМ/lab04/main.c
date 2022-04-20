#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#define MAX 3

int signal1 = 0;

int signal2 = 0;

struct myChildProcess {
    pid_t **ms;
    int size;
};

void outAllProc(struct myChildProcess mas)
{
    printf("\n");
    if(mas.size == 0)
    {
        printf("Empty\n");
        return;
    }
    for(int i=0;i<mas.size;i++)
    {
        printf("%d - Процесс PID:%d\n",i,*(*(mas.ms + i)));
    }
}

void check_signalParent(int sig) {

    if (sig == SIGUSR1) {
        signal2 = 1;

    }
    if (sig == SIGUSR2) {
        signal2 = 0;
    }
}

struct myChildProcess addProc(struct myChildProcess mas, pid_t pid) {
    mas.ms = (pid_t **) realloc(mas.ms, (mas.size + 1) * sizeof(pid_t *));        // выделение памяти под +1 строку
    if (mas.ms == NULL) {
        printf("Error!");
        exit(1);
    }
    (*(mas.ms + mas.size)) = (pid_t *) calloc(1, sizeof(pid_t));       // выделение памяти под строку
    *(*(mas.ms + mas.size)) = pid;
    mas.size = mas.size + 1;
    return mas;
}

struct myChildProcess dellLast(struct myChildProcess mas) {
    kill(*(*(mas.ms + mas.size - 1)), SIGKILL);
    free(*(mas.ms + mas.size - 1));
    mas.ms = (pid_t **) realloc(mas.ms, (mas.size - 1) * sizeof(pid_t *));
    mas.size--;
    return mas;
}

struct myChildProcess dellAll(struct myChildProcess mas) {
    while (mas.size != 0) {
        kill(*(*(mas.ms + mas.size - 1)), SIGKILL);
        free(*(mas.ms + mas.size - 1));
        mas.ms = (pid_t **) realloc(mas.ms, (mas.size - 1) * sizeof(pid_t *));
        mas.size--;
    }
    return mas;
}

void startOutChild(struct myChildProcess mas) {
    for (int i = 0; i < mas.size; i++) {
        kill((*(*mas.ms) + i), SIGUSR1);
        sleep(1);
    }
}

void stopOutChild(struct myChildProcess mas) {
    for (int i = 0; i < mas.size; i++) {
        kill((*(*mas.ms) + i), SIGUSR2);
    }
}


int main(int argc, char **argv) {
    const char *SEM_NAME = "SHRMEM_SCXL";  /* name of mutex */
    struct myChildProcess mass;
    mass.ms = (pid_t **) calloc(1, sizeof(pid_t *));
    mass.size = 0;
    (*(mass.ms + mass.size)) = (pid_t *) calloc(1, sizeof(pid_t));
    struct sigaction signal_struct1;

    sem_t *mutex = SEM_FAILED;             /* ptr to mutex */
    mutex = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0777, 1);

    signal_struct1.sa_handler = check_signalParent;
    sigemptyset(&signal_struct1.sa_mask);
    signal_struct1.sa_flags = 0;
    if (sigaction(SIGUSR1, &signal_struct1, NULL) == -1) {
        perror("CHILD: Error in sigaction (SIGUSR1)!\n");
        exit(errno);
    }
    signal_struct1.sa_handler = check_signalParent;
    sigemptyset(&signal_struct1.sa_mask);
    signal_struct1.sa_flags = 0;
    if (sigaction(SIGUSR2, &signal_struct1, NULL) == -1) {
        perror("CHILD: Error in sigaction (SIGUSR2)!\n");
        exit(errno);
    }

    int flag = 0;
    char doIt[80];
    char com[2];
    pid_t pid;
    int i;
    fprintf(stdout, "PARENT: Parent process is running... PID: %d\n", getpid());
    do {
        i = 0;
        doIt[i] = '\n';
        fprintf(stdout, "\n---------------------------------------------\n"
                        "o <- show all process \n"
                        "+ <- add child process \n"
                        "- <- dell last child process \n"
                        "k <- dell all child process \n"
                        "s <- prohibition of statistics output \n"
                        "g <- enable statistics output \n"
                        "s<num> <- prohibition of statistics output(num) \n"
                        "g<num> <- enable statistics output(num) \n"
                        "p<num> <- prohibition of stat output all process and give stat (num) \n"
                        "q <- quit \n");
        fprintf(stdout, "PARENT - what to do?: \n");
        fgets(doIt, MAX, stdin);
        while (doIt[i] != '\n') {
            switch (doIt[i]) {
                case '+':
                    if(mass.size == 9){
                        printf("PARENT: Достигнут максимум процессов (9)\n");
                        break;
                    }
                    switch (pid = fork()) {
                        case -1:
                            perror("fork"); /* произошла ошибка */
                            exit(1); /*выход из родительского процесса*/
                        case 0:
                            execve("./child",argv,NULL);
                        default:
                            break;
                           // printf("\t\t\t\t\t\t\t\t\t\t\tPARENT: Мой PID -- %d\n", getpid());
                    }
                    mass = addProc(mass, pid);
                    break;
                case '-':
                    if (mass.size == 0) {
                        printf("\tPARENT: Удаление не возможно. Процессов 0!");
                        break;
                    }
                    printf("\tPARENT: Удален %d   Осталось %d процессов", *(*(mass.ms + mass.size - 1)), mass.size - 1);
                    mass = dellLast(mass);
                    break;
                case 'k':
                    printf("\tPARENT: Удалены все %d процессы", mass.size);
                    mass = dellAll(mass);
                    break;
                case 's':
                    if(doIt[i+1]!='\n')
                    {
                        int aa =(int) doIt[i+1] - 48;
                        if(aa>=0 && aa<=9 && mass.size !=0){
                            kill((*(*mass.ms) + aa), SIGUSR2);
                        }
                        doIt[0]='\n';
                        i=-1;
                    }else{
                        stopOutChild(mass);
                    }
                    break;
                case 'g':
                    if(doIt[i+1]!='\n')
                    {
                        int aa =(int) doIt[i+1] - 48;
                        if(aa>=0 && aa<=9 && mass.size !=0){
                            kill((*(*mass.ms) + aa), SIGUSR1);
                        }
                        doIt[0]='\n';
                        i=-1;
                    }else{
                        startOutChild(mass);
                    }
                    break;
                case 'o':
                    outAllProc(mass);
                    break;
                case 'p':
                    stopOutChild(mass);
                    if(doIt[i+1]!='\n')
                    {
                        int aa =(int) doIt[i+1] - 48;
                        if(aa>=0 && aa<=9 && mass.size !=0){
                            kill((*(*mass.ms) + aa), SIGUSR1);
                        }
                        doIt[0]='\n';
                        i=-1;
                    }
                    fgets(com, 2, stdin);
                    sleep(5);
                    if(com[0] != 's') startOutChild(mass);
                    break;
                case 'q':
                    printf("\tPARENT: Удалены все %d процессы\n", mass.size);
                    mass = dellAll(mass);
                    flag = 1;
                    break;
            }
            if (flag == 1) break;
            i++;
        }
    } while (flag != 1);
    sem_close((sem_t *) &mutex);
    return argc;
}