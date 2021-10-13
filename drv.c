/**
 * drv.c
 * Copyright (c) 2021 zhangzhao <zhangzhao@ihep.ac.cn>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**********************************************************************
 *                            Header Files                            *
 **********************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "drv.h"

/**********************************************************************
 *                          Macro Definitions                         *
 **********************************************************************/

/* the default memory page size of the Linux Kernel is 4KB */
#define PAGE_SIZE 4096UL

/* page mask */
#define PAGE_MASK (PAGE_SIZE - 1)

/* base address of the register map. */
#define BASE_ADDR 0x0

/* print error message. */
#define FATAL                                                              \
    do {                                                                   \
        fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", __LINE__, \
                __FILE__, errno, strerror(errno));                         \
        exit(1);                                                           \
    } while (0)

/* calulate register address according to base address and offset. */
#define MAP_ADDR(x) (x + BASE_ADDR)

/* calulate virtual address according to page' first address. */
#define PAGEADDR2VIRTADDR(map_addr, page_addr) \
    page_addr + ((map_addr)&PAGE_MASK)

/**********************************************************************
 *                             Functions                              *
 **********************************************************************/

/**********************************************************************
 *                     phyaddr_mapto_pageaddr()                        *
 ***********************************************************************
 *   Map physical address to virtual address, return the first address *
 *   of the mapped page.                                               *
 **********************************************************************/

void phyaddr_mapto_pageaddr(off_t map_addr, void **page_addr) {
    int fd;
    void *addr;

    /* calulate register address according to base address and offset. */
    map_addr = MAP_ADDR(map_addr);

    /* open an image of the main memory */
    if ((fd = open("/dev/uio0", O_RDWR | O_SYNC)) == -1) FATAL;
    printf("/dev/mem opened.\n");

    /* map one page */
    addr = mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                map_addr & ~PAGE_MASK);
    if (addr == (void *)-1) FATAL;

    printf("One page mapped at address %p.\n", addr);
    *page_addr = addr;
    close(fd);
}

/***********************************************************************
 *                         unmap_pageaddr()                            *
 ***********************************************************************
 *   Free the mapped page.	                                           *
 **********************************************************************/

void unmap_pageaddr(void *page_addr) {
    if (munmap(page_addr, PAGE_SIZE) == -1) FATAL;
    printf("The address has been freed.\n");
}

/***********************************************************************
 *                             read_32()                               *
 ***********************************************************************
 *   Read 32-bit date from mapped address.                             *
 **********************************************************************/

void read_32(off_t src, unsigned int *dest) {
    void *page_addr, *virt_addr;

    phyaddr_mapto_pageaddr(src, &page_addr);

    virt_addr = PAGEADDR2VIRTADDR(src, page_addr);

    /* read 4 bytes from the virtual address and put them in *dest. */
    *dest = *((unsigned int *)virt_addr);

    printf("read 32-bits data %08x from %p\n.", *dest, virt_addr);
    unmap_pageaddr(page_addr);
}

/***********************************************************************
 *                             get_32()                                *
 ***********************************************************************
 *   Get 32-bit date of mapped address and return it. 	               *
 **********************************************************************/

unsigned int get_32(off_t src) {
    void *page_addr, *virt_addr;

    phyaddr_mapto_pageaddr(src, &page_addr);

    virt_addr = PAGEADDR2VIRTADDR(src, page_addr);

    /* read 4 bytes from the virtual address and return it. */
    return (*((unsigned int *)virt_addr));

    unmap_pageaddr(page_addr);
}

/***********************************************************************
 *                             write_32()                              *
 ***********************************************************************
 *   Write 32-bit date of mapped address. 	                           *
 **********************************************************************/

void write_32(unsigned int src, off_t dest) {
    void *page_addr, *virt_addr;

    phyaddr_mapto_pageaddr(dest, &page_addr);

    virt_addr = PAGEADDR2VIRTADDR(dest, page_addr);

    /* write 4 bytes of src to the virtual address. */
    *((unsigned int *)virt_addr) = src;

    printf("write 32-bits data %08x at %p\n.", *((unsigned int *)virt_addr), virt_addr);
    unmap_pageaddr(page_addr);
}
