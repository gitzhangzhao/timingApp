#include "drv.h"

/* for test */
int main() {
    int a;
    read_32(0x6, &a);
    printf("a:%x\n", a);
    write_32(0x12345678,0x6);
    read_32(0x6, &a);
    printf("a:%x\n", a);
    return 0;
}
