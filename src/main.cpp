#include <stdio.h>
#include "memory.cpp"

static const char* WINDOW_NAME_1 = "Ventana 1";
static const char* WINDOW_NAME_2 = "Ventana 2";
static const char* WINDOW_NAME_3 = "Ventana 3";
static const char* WINDOW_NAME_4 = "Ventana 4";
static const char* WINDOW_NAME_5 = "Ventana 5";
static const char* WINDOW_NAME_6 = "Ventana 6";

int main() {
    size_t count = 32;
    Memory memory;
    if (InitMemory(&memory, 512, count)) {
        puts("Hola mundo c:");
    } else {
        puts("Adios mundo :C");
        return -1;
    }

    while (true) {
        PushAllocator(&memory, WINDOW_NAME_1);

        int* array_int_4 = (int*)Allocate(&memory, sizeof(int)*4);
        if (*array_int_4 == 0) {
            array_int_4[0] = 1;
            array_int_4[2] = 2;
            array_int_4[3] = 3;
        }

        PushAllocator(&memory, WINDOW_NAME_2);

        int* array_int_2 = (int*)Allocate(&memory, sizeof(int)*2);
        if (*array_int_2 == 0) {
            array_int_2[0] = 10;
            array_int_2[1] = 20;
        }

        int* array_int_3 = (int*)Allocate(&memory, sizeof(int)*3);
        if (*array_int_3 == 0) {
            array_int_3[0] = 30;
            array_int_3[1] = 30;
            array_int_3[2] = 40;
        }

        Clear(&memory);
        PopAllocator(&memory);

        PushAllocator(&memory, WINDOW_NAME_3);

        int* array_int_10 = (int*)Allocate(&memory, sizeof(int)*10);
        if (*array_int_10 == 0) {
            array_int_10[0] = 1000;
            array_int_10[8] = 100;
            array_int_10[5] = 200;
        }

        int* array_int_1 = (int*)Allocate(&memory, sizeof(int));
        if (*array_int_1 == 0) {
            *array_int_1 = 10;
        }

        ClearToZero(&memory);
        PopAllocator(&memory);

        int* array_int_5 = (int*)Allocate(&memory, sizeof(int)*5);
        if (*array_int_5 == 0) {
            array_int_5[0] = 50;
            array_int_5[2] = 60;
            array_int_5[4] = 98123;
        }

        Clear(&memory);
        PopAllocator(&memory);
    }

    if (memory.blocks) {
        FreeMemory(&memory);
    }
    return 0;
}
