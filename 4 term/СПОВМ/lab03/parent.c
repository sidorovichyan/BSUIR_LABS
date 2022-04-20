#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
	
int main()
{
    fprintf(stdout, "Parent proc run...\n");
    fprintf(stdout, "My PID: %d\n",getpid());
    int status;
    pid_t pid;
    char *argv[] = {"UserData","MyData",NULL} ;
    pid = fork();
    if(pid == -1) {
        fprintf(stdout, "PARENT: Error - %d\n", errno);
    }
    if(pid == 0) {
      execve("./child", argv, NULL);
    }
    wait(&status);
    fprintf(stdout,"\tChild proc exited with code %d\n", status);
    fprintf(stdout,"Parent proc stop\n");
    exit(0);
}







// setenv("User", "Yan", overwrite); 

        // execve("./child", argv, argv);
       // char *args[] = {"gnome-terminal","--","./child","UserData", NULL} ; 
      //  execvp("gnome-terminal", args) ;