#include <iostream>
#include "mlib/bitmap_image.hpp"
#include <ctime>
#include <cmath>

int main() {
    unsigned int start_time =  clock();
    FILE *fIn = fopen("sunflower.bmp", "rb");
    FILE *fOut = fopen("grayflower.bmp", "wb");
    if (!fIn || !fOut)
    {
        printf("File error.\n");
        return 0;
    }
    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, fIn);
    fwrite(header, sizeof(unsigned char), 54, fOut);

    int width = *(int*)&header[18];
    int height = abs(*(int*)&header[22]);
    int stride = (width * 3 + 3) & ~3;
    int padding = stride - width * 3;

    unsigned char pixel[3];
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            fread(pixel, 3, 1, fIn);
            unsigned char gray = pixel[0] * 0.3 + pixel[1] * 0.58 + pixel[2] * 0.11;
            memset(pixel, gray, sizeof(pixel));
            fwrite(&pixel, 3, 1, fOut);
        }
        fread(pixel, padding, 1, fIn);
        fwrite(pixel, padding, 1, fOut);
    }

    fclose(fOut);
    fclose(fIn);
    unsigned int end_time = clock();
    unsigned int search_time = end_time - start_time;
    printf("Время выполнения программы без дополнительного потока: %d мс.",search_time);
    return 0;
}
