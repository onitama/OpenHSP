/*
 * test code
 * g++ -shared -fPIC -o test.so test.cpp -g -O2
 */
#include <stdio.h>

extern "C" {

int count = 0;

int func0() {
    //printf("called func0() %d\n", count);
    return count++;
}

int func1(char *p0, int p1, int p2, int p3) {
    //printf("called func1(%p, %d, %d, %d)\n", (void*)p0, p1, p2, p3);
    for (int i = 0; i < 10; i++) {
        if (p0 && p0[i]) {
            printf("    %d: %c\n", i, p0[i]);
        } else {
            break;
        }
    }
    return p1 + p2 + p3 + 12340000;
}
}