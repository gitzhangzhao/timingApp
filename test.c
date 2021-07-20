#include "drv.h"
#include<stdio.h>
#include<unistd.h>

/* for test */
int main() {
    int a;
    /* read_32(0x0300B058, &a); */
    /* printf("gpio PC12:%lx\n", a); */
    while (1) {
        write_32(0x00001000, 0x0300B058);
        read_32(0x0300B058, &a);
        printf("gpio PC12:0x%08x\n", a);
        sleep(1);

        write_32(0x00002000, 0x0300B058);
        read_32(0x0300B058, &a);
        printf("gpio PC12:0x%08x\n", a);
        sleep(1);
    }
    return 0;
}
