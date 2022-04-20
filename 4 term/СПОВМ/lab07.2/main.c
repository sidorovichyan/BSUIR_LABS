#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "time.h"
#include <semaphore.h>
#include <wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/shm.h>

#define sizeQueue 5
#define SEM_NAME "mysemafore"

struct mesg_buffer {
    long mesg_type;
    char mesg_text[100];
    struct mesg_buffer *next;

};

char *generateChar() {
    char *str = (char *) calloc(11, sizeof(char));
    srand(time(NULL));
    for (int i = 0; i < 10; i++) {

        *(str + i) = (65 + rand() % (90 - 65 + 1));
    }
    str[10] = '\0';
    return str;
}

void outputQueue(struct mesg_buffer messag) {
    struct mesg_buffer *ptr = &messag;
    while (ptr != NULL) {
        printf("Data in Queue is : %s \n",
               ptr->mesg_text);
        ptr = ptr->next;
    }
}

struct mesg_buffer generateMess(struct mesg_buffer messag) {
    sleep(1);
    strcpy((messag).mesg_text, generateChar());
    (messag).mesg_type = 1;
    (messag).next = NULL;
    return messag;
}

struct mesg_buffer addMessage(struct mesg_buffer head, struct mesg_buffer *mess) {
    struct mesg_buffer *ptr = &head;
    while (ptr->next != NULL) {
        ptr = ptr->next;
    }
    ptr->next = mess;
    return head;
}

struct mesg_buffer dellMessage(struct mesg_buffer head) {
    if (head.next == NULL) {
        return head;
    }
    struct mesg_buffer *buffer = head.next->next;
    free(head.next);
    head.next = buffer;
    return head;
}

void producer(sem_t *sem) {

}

void user(sem_t *sem) {

}

struct messag {
    char mess[100];
    struct messag *next;
};

struct queue {
    struct messag *head;
    struct messag *end;

    int n_input;
    int n_output;

    int size;
};

//void addMess(struct queue *queue, struct messag *messag) {
//
//    if (queue->size == 0) {
//        queue->head = messag;
//        queue->end = messag;
//        queue->size++;
//        queue->n_input++;
//        return;
//    }
//
//    struct messag *ptr = queue->head;
//    while (ptr->next != NULL) {
//        ptr = ptr->next;
//    }
//    ptr->next = messag;
//    queue->end = ptr->next;
//    queue->size++;
//    queue->n_input++;
//    return;
//}

void dellMess(struct queue *queue) {
    if (queue->head == NULL) {
        return;
    }
    if (queue->head == queue->end) {
        queue->size--;
        queue->n_output++;
        queue->head = NULL;
        queue->end = NULL;
        return;
    }

    struct messag *ptr = queue->head;
    queue->head = queue->head->next;
    queue->size--;
    queue->n_output++;
}

void outPutQueue(struct queue queue) {
    printf("Queue. Size: %d\n", queue.size);
    if (queue.size == 0) {
        printf("Empty queue\n");
        return;
    }
    struct messag *ptr = queue.head;
    while (ptr != NULL) {
        printf("\t\t%s\n", ptr->mess);
        ptr = ptr->next;
    }
}

struct queue initQueue(struct queue queue) {
    queue.size = 0;
    queue.n_input = 0;
    queue.n_output = 0;
    return queue;
}

struct messag generateMessag() {
    sleep(3);
    struct messag messag;
    messag.next = NULL;
    strcpy(messag.mess, "LOL");
    return messag;
}

void addMess(struct queue *queue, char *mess) {
    struct messag *ptr = queue->head;
    if (queue->size < sizeQueue) {
        for (int i = 0; i < queue->size; i++) {
            ptr = ptr->next;
        }
        strcpy(ptr->mess, mess);
        printf("Queue: ADD mess %s\n", mess);
        queue->size++;
        return;
    }
    printf("Full queue\n");
}

struct mes {
    char data[100];
    struct mes *next;
};

void delMess( struct mes *mes)
{
    struct mes *ptr = mes;
    strcpy(ptr->data,ptr->next->data);
    ptr = ptr->next;
    //strcpy(ptr->data,ptr->next->data);
    printf("--%s\n",ptr->data);
    printf("--%s\n",ptr->next->data);
    strcpy(ptr->data,ptr->next->data);
    ptr->next->data[0] = '\0';
 //   ptr->data[0] = '3';
    /// while(ptr!=NULL)
   // {

    //}
}


const char *data = "Hello there!";

enum {
    SEGMENT_SIZE = 0x6400
};

int main(int argc, char *argv[]) {
    int status;
    int segment_id;
    key_t key = 100;
    segment_id = shmget(key, sizeof(struct mes) * 10,IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    struct mes *mes = (struct mes *) shmat(segment_id, 0, 0);

    struct mes *data2 = (struct mes *) calloc(1, sizeof(struct mes));
    data2->next = (struct mes *) calloc(1, sizeof(struct mes));
    data2->next->next = (struct mes *) calloc(1, sizeof(struct mes));
    strcpy(data2->data, generateChar());
    sleep(1);
    strcpy(data2->next->data, generateChar());
    sleep(1);
    strcpy(data2->next->next->data, generateChar());
//    printf("Segment ID %d\n", segment_id);
//    printf("Attached at %p\n", sh_mem);
    //  memmove(mes->data, "ParentData\0",11);
    memmove(mes, data2, sizeof(struct mes) * 10);
    printf("%s\n", mes->data);
    printf("%s\n", mes->next->data);
    printf("%s\n\n", mes->next->next->data);
    //  printf("%s\n", mes->data);

    pid_t child_pid = fork();
    if (child_pid == -1)
        perror("fork");
    if (child_pid == 0) {
        struct mes *mes2 = (struct mes *) shmat(segment_id, 0, 0);
        strcpy(mes->data, mes->next->data);
        sleep(1);
        strcpy(mes->next->data, mes->next->next->data);
        sleep(1);
        strcpy(mes->next->next->data, "\0");
    //    printf("%s\n", mes2->data);
      //  printf("%s\n", mes2->next->data);
      //  printf("%s\n\n", mes2->next->next->data);
       // memmove(mes, mes2, sizeof(struct mes) * 10);
      //  mes2 = mes2->next;
      //  printf("--%s\n", mes2->data);
      //  printf("--%s\n", mes2->next->data);
      //  strcpy(mes2->data, mes2->next->data);

        //char *buf;
   //     strcpy(buf, mes2->next->data);
    //    strcpy(mes2->data, buf);
       // strcpy(mes2->next->data, mes2->next->next->data);
      //  strcpy(mes2->next->next->data, "\0");
      //  delMess(mes);
     //   printf("%s\n", mes->data);
     //   printf("%s\n", mes->next->data);
     //   printf("%s\n\n", mes->next->next->data);
        //strcpy(mes->data, "123\0");
        // strcpy(data2->next->data, generateChar());
        // strcpy(data2->next->next->data, generateChar());
        // strcpy(mes->data, "Child Process\0");
        printf("child pid - %d\n", getpid());
        exit(EXIT_SUCCESS);
    } else {
        pid_t ret = waitpid(child_pid, &status, WUNTRACED | WCONTINUED);
        if (ret == -1)
            perror("waitpid");

        if (WIFEXITED(status))
            printf("Child exited, status - %d\n", WEXITSTATUS(status));

        if (WEXITSTATUS(status) == 0) {
            printf("\n%s\n", mes->data);
            printf("%s\n", mes->next->data);
            printf("%s\n", mes->next->next->data);

        }

    }

    shmdt(mes);
    shmctl(segment_id, IPC_RMID, 0);
    exit(EXIT_SUCCESS);

}
//    int status;
//    int segment_id;
//    struct queue queue;
//
////    segment_id = shmget (IPC_PRIVATE, sizeof (struct messag) * 10,IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
////    struct messag *shmem = shmat(segment_id, 0, 0);
////    struct messag*ptr = shmem;
////    for(int i=0;i<10;i++)
////    {
////        ptr->next = (struct messag*) calloc(1,sizeof (struct messag));
////        ptr=ptr->next;
////    }
//    key_t key = 10;
//    segment_id = shmget (IPC_PRIVATE, sizeof (struct queue),IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
//    struct queue *shmem =(struct queue*) shmat(segment_id, 0, 0);
//    shmem->head = (struct messag*) calloc(1,sizeof (struct messag));
//    struct messag * ptr = shmem->head;
//    for(int i=0;i<5;i++)
//    {
//        ptr->next = (struct messag*) calloc(1,sizeof (struct messag));
//        ptr = ptr->next;
//        //ptr->head = (struct messag*) calloc(1,sizeof (struct messag));
//       // ptr->head = ptr->head->next;
//    }
//    shmem->end = ptr;
//    addMess(shmem,generateChar());
//
//    printf("PARENT:\n");
//    outPutQueue(*shmem);
//
// //   strcpy(shmem->mess ,"LOL");
// //   shmem->next = (struct messag*) calloc(1,sizeof (struct messag));
// //   strcpy(shmem->next->mess ,"LOL");
//
//
//
//   // shmem->next->mess = messag;
////    shmem+sizeof (struct messag) = &messag;
//
//    int id;
//  //  FILE_entry *entries;
//
// //   entries = (FILE_entry *) shmat(id, NULL, 0);
//
//  //  struct queue queue;
//   // queue = initQueue(queue);
//  //  struct messag messag;
//   // messag = generateMessag();
//  //  addMess(shmem,&messag);
//  //  outPutQueue(*shmem);
//  //  char *sh_mem = (char *) shmat(segment_id, 0, 0);
//
//   // printf("Segment ID %d\n", segment_id);
// //   printf("Attached at %p\n", sh_mem);
////    memmove(shmem, &queue, sizeof(struct queue));
//  //  outPutQueue(*shmem);
//    //printf("%s\n", sh_mem);
//
//    pid_t child_pid = fork();
//    if (child_pid == -1)
//        perror("fork");
//
//    if (child_pid == 0) {
//        sleep(2);
//        shmem->head->mess[0] = '3';
//      //  int segment_id2;
//      //  struct queue *queue2;
//      //  key_t key2 = 10;
//      //  segment_id2 = shmget (IPC_PRIVATE, sizeof (struct queue),IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
//      //  queue2=(struct queue*) shmat(segment_id2, 0, 0);
//      //  outPutQueue(*queue2);
//       // queue2->head->mess[0] = '3';
//       // struct queue *shmem2 = shmat(segment_id, 0, 0);
//       // addMess(shmem,generateChar());
//       // ptr3->mess
//       // shmem->next->next = (struct messag*) calloc(1,sizeof (struct messag));
//       // strcpy(shmem->next->next->mess ,"LOLlll");
//       // addMess(shmem,&messag2);
//        //outPutQueue(queue);
//        //memcpy(shmem,&queue,sizeof (struct queue));
//     //   strcpy(sh_mem, "NewData\0");
//
//        printf("child pid - %d\n", getpid());
//        exit(EXIT_SUCCESS);
//    } else {
//        pid_t ret = waitpid(child_pid, &status, WUNTRACED | WCONTINUED);
//        if (ret == -1)
//            perror("waitpid");
//        if (WEXITSTATUS(status) == 0){
//            outPutQueue(*shmem);
//          //  printf("%s\n",shmem->mess);
//          //  printf("%s\n",shmem->next->mess);
//          //  printf("%s\n",shmem->next->next->mess);
//        }
//
//    }
//    shmdt(shmem);
//    shmctl(segment_id, IPC_RMID, 0);
//    exit(EXIT_SUCCESS);


//
//int main() {
//
//    key_t my_key;
//    int msg_id;
//    my_key = ftok("progfile", 65); //create unique key
//    msg_id = msgget(my_key, 0666 | IPC_CREAT); //create message queue and return id
//    struct mesg_buffer queue = generateMess(queue);
//    strcpy(queue.mesg_text, "HEAD");
//    for (int i = 0; i < 2; i++) {
//        struct mesg_buffer *next = (struct mesg_buffer *) calloc(1, sizeof(struct mesg_buffer));
//        *next = generateMess(*next);
//        queue = addMessage(queue, next);
//    }
//    msgsnd(msg_id, &queue, sizeof(queue)*10, 0); //send message
//    pid_t pid;
//    switch (pid = fork()) {
//        case -1:
//            perror("fork"); /* произошла ошибка */
//            exit(1); /*выход из родительского процесса*/
//        case 0:
//            printf("");
//            struct mesg_buffer mesgBuffer;
//            key_t my_key2;
//            int msg_id2;
//            my_key2 = ftok("progfile", 65); //create unique key
//            msg_id2 = msgget(my_key2, 0666 | IPC_CREAT); //create message queue and return id
//            msgrcv(msg_id2, &mesgBuffer, sizeof(mesgBuffer), 1, 0); //used to receive message
//            struct mesg_buffer *next2 = (struct mesg_buffer *) calloc(1, sizeof(struct mesg_buffer));
//            *next2 = generateMess(*next2);
//            mesgBuffer = addMessage(mesgBuffer, next2);
//            outputQueue(mesgBuffer);
//            msgsnd(msg_id2, &mesgBuffer, sizeof(queue)*10, 0); //send message
//
//           // printf("Received Message is : %s \n", message.msg);
//           // msgctl(msg_id2, IPC_RMID, NULL); //destroy the message queue
//            exit(0);
//        default:
//            break;
//    }
//    sleep(5);
//    printf("\n");
//    msgrcv(msg_id, &queue, sizeof(queue)*10, 1, 0); //used to receive message
//    outputQueue(queue);
//    msgctl(msg_id, IPC_RMID, NULL);
//  //  sem_close(sem);
//    return 0;
//}

//
//int main(int argc, char *argv[]) {
//
//    void* shmem = create_shared_memory(128);
//
//
//    struct messag *messag;
//    generateMessag(&messag);
//    memcpy(shmem, messag->mess, sizeof(messag->mess));
//
//    int flag = 0;
//    char doIt[80];
//    char com[2];
//    pid_t pid;
//    int i;
//    fprintf(stdout, "PARENT: Parent process is running... PID: %d\n", getpid());
//    do {
//        i = 0;
//        doIt[i] = '\n';
//        fprintf(stdout, "\n---------------------------------------------\n"
//                        "p <- creat creator \n"
//                        "u <- creat user \n"
//                        "q <- quit \n");
//        fprintf(stdout, "PARENT - what to do?: \n");
//        fgets(doIt, MAX, stdin);
//        while (doIt[i] != '\n') {
//            switch (doIt[i]) {
//                case '+':
//                    if(mass.size == 9){
//                        printf("PARENT: Достигнут максимум процессов (9)\n");
//                        break;
//                    }
//                    switch (pid = fork()) {
//                        case -1:
//                            perror("fork"); /* произошла ошибка */
//                            exit(1); /*выход из родительского процесса*/
//                        case 0:
//                            execve("./child",argv,NULL);
//                        default:
//                            break;
//                            // printf("\t\t\t\t\t\t\t\t\t\t\tPARENT: Мой PID -- %d\n", getpid());
//                    }
//                    mass = addProc(mass, pid);
//                    break;
//                case '-':
//                    if (mass.size == 0) {
//                        printf("\tPARENT: Удаление не возможно. Процессов 0!");
//                        break;
//                    }
//                    printf("\tPARENT: Удален %d   Осталось %d процессов", *(*(mass.ms + mass.size - 1)), mass.size - 1);
//                    mass = dellLast(mass);
//                    break;
//                case 'k':
//                    printf("\tPARENT: Удалены все %d процессы", mass.size);
//                    mass = dellAll(mass);
//                    break;
//                case 's':
//                    if(doIt[i+1]!='\n')
//                    {
//                        int aa =(int) doIt[i+1] - 48;
//                        if(aa>=0 && aa<=9 && mass.size !=0){
//                            kill((*(*mass.ms) + aa), SIGUSR2);
//                        }
//                        doIt[0]='\n';
//                        i=-1;
//                    }else{
//                        stopOutChild(mass);
//                    }
//                    break;
//                case 'g':
//                    if(doIt[i+1]!='\n')
//                    {
//                        int aa =(int) doIt[i+1] - 48;
//                        if(aa>=0 && aa<=9 && mass.size !=0){
//                            kill((*(*mass.ms) + aa), SIGUSR1);
//                        }
//                        doIt[0]='\n';
//                        i=-1;
//                    }else{
//                        startOutChild(mass);
//                    }
//                    break;
//                case 'o':
//                    outAllProc(mass);
//                    break;
//                case 'p':
//                    stopOutChild(mass);
//                    if(doIt[i+1]!='\n')
//                    {
//                        int aa =(int) doIt[i+1] - 48;
//                        if(aa>=0 && aa<=9 && mass.size !=0){
//                            kill((*(*mass.ms) + aa), SIGUSR1);
//                        }
//                        doIt[0]='\n';
//                        i=-1;
//                    }
//                    fgets(com, 2, stdin);
//                    sleep(5);
//                    if(com[0] != 's') startOutChild(mass);
//                    break;
//                case 'q':
//                    printf("\tPARENT: Удалены все %d процессы\n", mass.size);
//                    mass = dellAll(mass);
//                    flag = 1;
//                    break;
//            }
//            if (flag == 1) break;
//            i++;
//        }
//    } while (flag != 1);
//
//
//
//
//
//    int pid = fork();
//
//    if (pid == 0) {
//        creat(shmem);
//    } else {
//        printf("Parent read: %s\n", shmem);
//        sleep(3);
//        printf("After 1s, parent read: %s\n", shmem);
//    }
//
//
////    char *shm;
////    key_t key;
////    int pid;
////    struct queue que;
////    struct queue *queue = &que;
////    *queue = initQueue(*queue);
////    struct messag *messag = (struct messag *) calloc(1, sizeof(struct messag));
////    generateMessag(&messag);
////    *queue = addMess(*queue, messag);
////    generateMessag(&messag);
////    *queue = addMess(*queue, messag);
////    generateMessag(&messag);
////    *queue = addMess(*queue, messag);
////
////    struct queue *queue2;
////
////
////
////    memcpy(queue, queue2, sizeof(struct queue));
//
//    //   key = shmget(UNIQENUMBER, SHMSIZE, 0666 | IPC_CREAT); // инициализация sharedMemory
//    //0666 устанавливает права доступа для сегмента памяти,
//    //а IPC_CREAT говорит системе создать новый сегмент памяти для разделяемой памяти.
//    //   queue = (struct queue*) shmat(key, (void *) 0, 0);
//    //  *queue = initQueue(*queue);
//    //  struct queue *s = queue; // выставляем указатель на sharedMemory
////
////    struct messag *messag = (struct messag *) calloc(1, sizeof(struct messag));
////    generateMessag(&messag);
////    *s = addMess(*s,messag);
//
////   while (1) {
////        pid = fork();           // создаем новый процесс
////        memcpy(s,queue,sizeof (struct queue));
////        switch (pid) {
////            case 0: {
////                client();
////                sleep(10);
////                kill(pid, SIGTERM); // поылаем сигнал для запроса завершения процесса.
////                            // завершаем серверный процесс
////                kill(getpid(), SIGTERM); // завершение дочернего процесса
////                break;
////            }
////            case -1: { // не получилось создать процесс
////                perror("ERROR");
////                exit(0);
////            }
////            default: {
////                sleep(5);
////                struct queue *queue1;
////                int key1 = shmget(UNIQENUMBER, SHMSIZE, 0);
////                queue1 = (struct queue*) shmat(key1, (void *) 0, 0);
////                struct queue *s1 = queue1; // выставляем указатель на sharedMemory
////
////                memcpy(s1,queue1,sizeof (struct queue));
////                outPutQueue(*queue1);
////
////                printf("1");
////                wait(NULL); // приостанавливает  выполнение  текущего  процесса  до тех пор, пока дочерний процесс не прекратит выполнение.
////                //Если дочерний процесс к моменту вызова функции уже завершился (так  называемый  "зомби"),  то  функция  немедленно  возвращается.
////                //Системные ресурсы, связанные с дочерним процессом, освобождаются
////                break;
////            }
////        }
////    }
//    return 0;
//}




