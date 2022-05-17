
#include <conio.h>
#include <dos.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#pragma inline

#define SIZE 10

float array[SIZE];

void inputArray();
void asmAlgorithm();

int main() {
    inputArray();
    asmAlgorithm();
    return 0;
}

void inputArray() {
    int res;
    printf("Input 10 elements: \n");

    for (int i = 0; i < SIZE; ++i) {
        do {
            res = scanf("%f", &array[i]);
            while (getchar() != '\n')
            if (res != 1) printf("Invalid input\n");
        } while (res != 1);
    }
}

void asmAlgorithm() {

    double rezult = 0;
    double ten = 10;    
    asm {
		finit
		xor cx, cx
		mov cx, SIZE
		lea bx, array
	}
	calculate:					
	asm {		
        fld rezult			
		fld dword ptr [bx] 	
    	faddp  					
		fstp rezult	        
		add bx, 4				
		loop calculate
        fld rezult
        fld ten
        fdiv
        fstp rezult
    }
	end: 
	asm fwait
    printf("%lf",rezult);
}