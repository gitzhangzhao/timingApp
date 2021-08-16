/**
 * erconfigure.c
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

/* #include "erconfigure.h" */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "drv.h"
/* #include "devEr.h" */

/**********************************************************************
 *                          Macro Definitions                         *
 **********************************************************************/

#define BEPCII_FRAC_4 0x00FE816D

/* print error message. */
#define FATAL                                                              \
    do {                                                                   \
        fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", __LINE__, \
                __FILE__, errno, strerror(errno));                         \
        exit(1);                                                           \
    } while (0)

/**********************************************************************
 *                          Global Variable                           *
 **********************************************************************/

/* extern variable */
int configure_lock = 0;

/**********************************************************************
 *                             Functions                              *
 **********************************************************************/

/***********************************************************************
 *                         configure_evr()                             *
 ***********************************************************************
 *   This routine is to be called in the startup script in order to    *
 *   init the evr (By default there are no cards configures).          *
 **********************************************************************/

int configure_evr(unsigned int divider) {
    /* check the configuration lock*/
    if (configure_lock != 0) {
        printf(
            "Erconfigure: Cannot change configuration after init. Request "
            "ignored\n");
        return -1;
    }

    /* the configuration process */
    switch (divider) {
        case 0:
            printf("use card default\n");
            break;
        case 4:
            printf("set 0xC6318 to register 0x7c, divider is 4\n");
            write_32(0xC6318, RFpattern);
            write_32(BEPCII_FRAC_4, FracDiv);
            write_32(125, uSecDivider);
            break;
        case 5:
            printf("set 0xCCCCC to register 0x7c, divider is 5\n");
            write_32(0xCCCCC, RFpattern);
            write_32(0x025b41ed, FracDiv);
            write_32(100, uSecDivider);
            break;
        case 10:
            printf("set 0xAAAAA to register 0x7c, divider is 10\n");
            write_32(0xAAAAA, RFpattern);
            write_32(0x025b43ad, FracDiv);
            write_32(50, uSecDivider);
            break;
        default:
            printf("the divider variable is wrong.\n");
            return -1;
    }

    return 0;
}

/**********************************************************************
 *                                main                                *
 **********************************************************************/

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: erconfigure [divider]");
        return 0;
    }

    if (configure_evr(atoi(argv[1])) == -1) printf("the configuration failed.");

    return 0;
}
