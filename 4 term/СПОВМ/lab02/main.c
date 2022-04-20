#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <locale.h>

struct output                   // структура для вывода
{
    char **out;
    int len;
};

void addToOutput(struct output *out, char *str)             // функция добавления строки в структуру вывода
{
    out->out = (char **) realloc(out->out, (out->len+1)*sizeof (char*));        // выделение памяти под +1 строку
    if(out->out == NULL)
    {
        printf("Error!");
        exit(1);
    }
    (*(out->out+out->len)) = (char*) calloc(strlen(str)+2,sizeof (char));       // выделение памяти под строку
    strcpy((*(out->out+out->len)), str);
    *(*(out->out+out->len)+strlen(str)+1) = '\0';
    out->len = out->len+1;
}

void dirwalk(char* dirname, char type, struct output *out)      // рекурсивная функция прохода по файлам
{
    DIR* d_fh;                                              // для открытия файла
    struct dirent* entry;                                   // для определения типа файла
    char long_name[4096];                                   // максимальная длинна файла
    if(NULL == (d_fh = opendir(dirname))) {             // попытка открыть по переданному пути
        printf("Couldn't open directory: %s\n ",dirname);
        exit(1);
    }
    while((entry=readdir(d_fh)) != NULL) {            // пока не пройдём все файлы в dir
        if(strncmp(entry->d_name, "..", 2) != 0 && strncmp(entry->d_name, ".", 1) != 0) {
            if (entry->d_type == DT_DIR) {                      // если директория
                strcpy(long_name,dirname);              // копируем путь к ней
                long_name[strlen(dirname)]='/';               // добавляем /
                long_name[strlen(dirname)+1]='\0';            // ставим признак конца строки
                strncat(long_name,entry->d_name, strlen(entry->d_name));    // добавляем имя этой папки
                if(type == 'd') addToOutput(out,long_name);                     // если ищем папки, добавляем путь к ней в вывод
                dirwalk(long_name,type,out);                                // рекурсивный вход в папку
            }
            else {
                if(type == 'f' && entry->d_type != DT_LNK)              // если мы ищем файлы
                {
                    strcpy(long_name,dirname);                // такая же суть как и в папке
                    long_name[strlen(dirname)]='/';                  // добавляем /
                    long_name[strlen(dirname)+1]='\0';                  // ставим признак конца строки
                    strncat(long_name,entry->d_name, strlen(entry->d_name));    // добавляем имя этой папки
                    long_name[strlen(dirname)+1+strlen(entry->d_name)]='\0';         // ставим признак конца строки
                    addToOutput(out,long_name);                                 // добавляем в вывод путь к файлу
                }
                if( type == 'l' && entry->d_type == DT_LNK)                     // если ищем ссылки
                {
                    strcpy(long_name,dirname);
                    long_name[strlen(dirname)]='/';                          // добавляем /
                    long_name[strlen(dirname)+1]='\0';                          // ставим признак конца строки
                    strncat(long_name,entry->d_name, strlen(entry->d_name));    // добавляем имя этой папки
                    long_name[strlen(dirname)+1+strlen(entry->d_name)]='\0';            // ставим признак конца строки
                    addToOutput(out,long_name);                         // добавляем в вывод путь к файлу
                }
            }
        }
    }
    closedir(d_fh);                         // после прохода всех файлов в папке закрываем этот dir
}

int compar(const void *a, const void *b)                    // сравнение строк в соответствии с LC_LOCALE
{
    return strcoll(*(char **)a, *(char **)b);
}

void sortOutput(struct output out)                          // Сортировка структуры вывода
{
    qsort(out.out,out.len,sizeof (char *),compar);
}

void printOutput(struct output *out)                    // вывод всей структуры
{
    for(int i=0;i<out->len;i++)
    {
        printf("%s\n",*(out->out+i));
    }
}

void freeOutput(struct output *out)                 // очитка памяти структуры(строки)
{
    for(int i=0;i<out->len;i++)
    {
        free((*(out->out+i)));
    }
    out->len=0;
}

int main(int argc, char **argv) {
        setlocale(LC_ALL,"Russian");
        struct output out;                          // структура вывода
        out.out = (char **) calloc(0,sizeof (char*));
        out.len = 0;
        int u = 2;
        if(argc == 1) {                 // если ввели мало данных для поиска
            printf("Incorrect path!\n './dirwalk' [path] [option]\n");
            return 0;
        }
        if(argc == 2)               // если ввели только путь
        {
            dirwalk(argv[1],'d',&out);
            dirwalk(argv[1],'f',&out);
            dirwalk(argv[1],'l',&out);
        }
        if(argc > 2)                // если ввели путь и тип поиска
        {
            while (u < argc)
            {
                if(argv[u][1] != 's' && argv[u][1] != 'd' && argv[u][1] != 'l' && argv[u][1] != 'f')
                {
                    printf("Error, incorrect input!Correct: dirwalk [path] [-type]\n[-types]: -l, -d, -f, -s\n");
                    return 0;
                }
                if(argv[u][1] == 's')
                {
                    sortOutput(out);
                }
                dirwalk(argv[1],argv[u][1],&out);
                u++;
            }
        }
        printOutput(&out);          // вывод структуры
        freeOutput(&out);           // очистка памяти структуры
        free(out.out);              // очистка указателя на массив строк
        return 0;
}

