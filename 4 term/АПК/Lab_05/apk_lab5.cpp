#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

unsigned int delayTime = 0;

void interrupt newInt(...);
void interrupt(*oldInt)(...);

int to_Decimal(int BCD) {
    return ((BCD / 16 * 10) + (BCD % 16));
}

int to_BCD(int decimal)
{
    return ((decimal / 10 * 16) + (decimal % 10));
}

void set_time()
{
    char to_set[6];

    int enter;

    do {
        rewind(stdin);
        printf("Enter year: ");
        scanf("%d", &enter);
    } while ((enter > 100 || enter < 21));
    to_set[5] = to_BCD(enter);
    do {
        rewind(stdin);
        printf("Enter month: ");
        scanf("%d", &enter);
    } while ((enter > 12 || enter < 1));
    to_set[4] = to_BCD(enter);

    do {
        rewind(stdin);
        printf("Enter day: ");
        scanf("%d", &enter);
    } while ((enter > 31 || enter < 1));
    to_set[3] = to_BCD(enter);

    do {
        rewind(stdin);
        printf("Enter hours: ");
        scanf("%d", &enter);
    } while ((enter > 23 || enter < 0));
    to_set[2] = to_BCD(enter);

    do {
        rewind(stdin);
        printf("Enter minuts: ");
        scanf("%d", &enter);
    } while (enter > 59 || enter < 0);
    to_set[1] = to_BCD(enter);

    do {
        rewind(stdin);
        printf("Enter seconds: ");
        scanf("%d", &enter);
    } while (enter > 59 || enter < 0);
    to_set[0] = to_BCD(enter);

    char addresses_reg[] = { 0x00, 0x02, 0x04, 0x07, 0x08, 0x09 };

    disable();
    outp(0x70, 0x0A);
    if (!(inp(0x71) & 0x80)) {       // проверка можно ли работать

        outp(0x70, 0x0B);           // запрещение обновление часов

        outp(0x71, inp(0x71) | 0x80);

        for (int i = 0; i < 6; i++) {
            outp(0x70, addresses_reg[i]);
            outp(0x71, to_set[i]);
        }
        outp(0x70, 0xB);
        outp(0x71, inp(0x71) & 0x7F);
    }
    else {
        puts("Time cannot be changed yet. Try again later...");
        system("pause");
    }
    enable();
}


void set_delay()
{
    int enter_delay = 0;
    printf("Enter delay(ms): ");
    scanf("%d", &enter_delay);

    disable();
    oldInt = getvect(0x70);
    setvect(0x70, newInt);
    enable();

    outp(0xA1, inp(0xA1) & 0xFE); // 0xFE = 1111 1110

    outp(0x70, 0xB);
    outp(0x71, inp(0x71) | 0x40); // 0x40 = 0100 0000

    delayTime = 0;
    while (delayTime <= enter_delay);
    setvect(0x70, oldInt);

    return;
}

void interrupt newInt(...)
{
    outp(0x70, 0x0C);
    if(inp(0x71) & 0x40)
        delayTime++;

    outp(0x20, 0x20);   //0x20 = 00100000
    outp(0xA0, 0x20);
}

int check_number(int num)
{
    if(num>=0 && num<=9)
        printf("0");
    return num;
}

void print_time()
{
    printf("DATE: ");
    outp(0x70, 0x07);
    printf("%d.", check_number(to_Decimal(inp(0x71))));
    outp(0x70, 0x08);
    printf("%d.", check_number(to_Decimal(inp(0x71))));
    outp(0x70, 0x09);
    printf("%d\n", check_number(to_Decimal(inp(0x71))));
    printf("TIME: ");
    outp(0x70, 0x04);
    printf("%d:", check_number(to_Decimal(inp(0x71))));
    outp(0x70, 0x02);
    printf("%d:", check_number(to_Decimal(inp(0x71))));
    outp(0x70, 0x00);
    printf("%d\n", check_number(to_Decimal(inp(0x71))));
}

int main() {
    system("cls");
    printf("START\n");
    while(1) {
        print_time();
        puts("\n1. Set time\n2. Set delay\n0. Exit");
        char vvod = getchar();
        switch(vvod) {
            case '1':
            {
                system("cls");
                set_time();
                system("cls");
                break;
            }
            case '2':
            {
                system("cls");
                set_delay();
                system("cls");
                break;
            }
            case '0':
                return 0;
            default:
                break;
        }
        system("cls");
    }
    return 0;
}
