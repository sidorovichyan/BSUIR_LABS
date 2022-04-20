#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int main (int argc, char* argv[], char *envp[]) {
    fprintf(stdout, "\tChild proc is run...\n");
   
    if(argc > 0)
    {
        for(int i=0;i<argc;i++)
        {   
        fprintf(stdout,"\t%s%s\n","-Data from the PARENT: ",argv[i]);
        }
    }
    fprintf(stdout,"\t-My PID: %d \n",getpid());
    fprintf(stdout,"\t-My PPID: %d \n",getppid());
    fprintf(stdout,"\t-Enter 'q': ");
    while(getc(stdin) != 'q');
    exit(0);
}








  //  printf("\n\n%s\n\n",data);
  //  while(envp[k])printf("\t-%s\n", envp[k++]);

