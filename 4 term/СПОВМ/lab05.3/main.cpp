#include <iostream>
#include "mlib/bitmap_image.hpp"
#include <ctime>
#include <cmath>
#include <csignal>
#include <mutex>
#include <vector>
#include "pthread.h"
#define N_THREAD 4
FILE *fIn;
FILE *fOut;
unsigned char header[54];
int width;
int height;
int stride;
int padding;
pthread_mutex_t mutex;

void * ret;
void *runThread(void *args) {
    unsigned char pixel[3];
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            pthread_mutex_lock(&mutex);
            if(feof(fIn)) {
                printf("%lu -- FILE ENDED [y=%d]\n",pthread_self(),y);
                pthread_mutex_unlock(&mutex);
                return ret;
            }
            fread(pixel, 3, 1, fIn);
            unsigned char gray = pixel[0] * 0.3 + pixel[1] * 0.58 + pixel[2] * 0.11;
            memset(pixel, gray, sizeof(pixel));
            fwrite(&pixel, 3, 1, fOut);
            pthread_mutex_unlock(&mutex);
        }
        pthread_mutex_lock(&mutex);
        fread(pixel, padding, 1, fIn);
        fwrite(pixel, padding, 1, fOut);
        pthread_mutex_unlock(&mutex);
    }
}


int main() {
    unsigned int start_time = clock();
    pthread_t threads[N_THREAD];
    fIn = fopen("sunflower.bmp", "rb");
    fOut = fopen("grayflower.bmp", "wb");
    pthread_mutex_init(&mutex, NULL);
    if (!fIn || !fOut) {
        printf("File error.\n");
        return 0;
    }
    fread(header, sizeof(unsigned char), 54, fIn);
    fwrite(header, sizeof(unsigned char), 54, fOut);
    width = *(int *) &header[18];
    height = abs(*(int *) &header[22]);
    stride = (width * 3 + 3) & ~3;
    padding = stride - width * 3;

    int status;
    int status_addr;
    for(int i =0;i<N_THREAD;i++){
        status = pthread_create(&threads[i], NULL, runThread, NULL);
        if (status != 0) {
            printf("main error: can't create thread, status = %d\n", status);
            return 1;
        }
    }


//    unsigned char pixel[3];
//    for (int y = 0; y < height; ++y) {
//        for (int x = 0; x < width; ++x) {
//            pthread_mutex_lock(&mutex);
//            fread(pixel, 3, 1, fIn);
//            unsigned char gray = pixel[0] * 0.3 + pixel[1] * 0.58 + pixel[2] * 0.11;
//            memset(pixel, gray, sizeof(pixel));
//            fwrite(&pixel, 3, 1, fOut);
//            pthread_mutex_unlock(&mutex);
//        }
//        pthread_mutex_lock(&mutex);
//        fread(pixel, padding, 1, fIn);
//        fwrite(pixel, padding, 1, fOut);
//        pthread_mutex_unlock(&mutex);
//    }
    for(int i =0;i<N_THREAD;i++){
        status = pthread_join(threads[i], (void **) &status_addr);
    }

    pthread_mutex_destroy(&mutex);
    fclose(fOut);
    fclose(fIn);

    unsigned int end_time = clock();
    unsigned int search_time = end_time - start_time;
    printf("Время выполнения программы c дополнительным потоком: %d мс.", search_time);
    return 0;

}
