
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#define MAX_COUNT 20
int signal1 = 0;
int signal2 = 0;

struct stat{
    int ed;
    int des;
};

struct stat add(struct stat statis)
{
    if( (statis.ed == 0 && statis.des == 0) |(statis.ed == 0 && statis.des == 1))    // 00  10
    {
        statis.ed++;
    }else if((statis.ed == 1 && statis.des == 1)){
        statis.ed=0;
        statis.des=0;
    } else if((statis.ed == 1 && statis.des == 0))
    {
        statis.ed=0;
        statis.des++;
    }
    return statis;
}
void check_signalChild(int sig) {
    if (sig == SIGUSR1) {
        signal1 = 1;
    }
    if (sig == SIGUSR2) {
        signal1 = 0;
    }
}

int main(void)
{
    struct stat statis;
    struct stat old_stat;
    statis.ed = 0;
    statis.des = 0;
    printf("\t\t\t\t\t\t\t\t\t\t\tPARENT: Создан процесс потомок %d\n", getpid());
    struct sigaction signal_struct;
    signal_struct.sa_handler = check_signalChild;
    sigemptyset(&signal_struct.sa_mask);
    signal_struct.sa_flags = 0;
    if (sigaction(SIGUSR1, &signal_struct, NULL) == -1) {
        perror("CHILD: Error in sigaction (SIGUSR1)!\n");
        exit(errno);
    }
    signal_struct.sa_handler = check_signalChild;
    sigemptyset(&signal_struct.sa_mask);
    signal_struct.sa_flags = 0;
    if (sigaction(SIGUSR2, &signal_struct, NULL) == -1) {
        perror("CHILD: Error in sigaction (SIGUSR2)!\n");
        exit(errno);
    }
    struct timespec tw = {2};
    struct timespec tr;
    int count=0;
    while (1) {
        nanosleep (&tw, &tr);
        if(count == 1)
        {
            if (signal1 == 1) {
                int u = 0;
                char *str = "\t\t\t\t\t\t\t\t\t\tCHILD: Мой PID -- \n";
                while (*(str + u) != '\n') {
                    printf("%c", *(str + u));
                    u++;
                }
                printf("%d Мой PPID -- %d ", getpid(), getppid());
                printf("{%d-%d} {%d-%d}\n", statis.des,statis.ed, old_stat.ed,old_stat.des);
                nanosleep (&tw, &tr);
            }
            count = 0;
        }
        count++;
        old_stat = statis;
        statis = add(statis);

    }
    exit(0);
}