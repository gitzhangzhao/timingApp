/**
 * dev.c
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

/********************
 *  c header files  *
 ********************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************
 *  epics device support header files  *
 ***************************************/

#include <dbScan.h>
#include <devSup.h>

/************************
 *  local header files  *
 ************************/

#include "drv.h"
#include "erconfigure.h"

/**********************************************************************
 *                          Macro Definitions                         *
 **********************************************************************/

/**********************************************************************
 *                             Functions                              *
 **********************************************************************/

/**********************************************************************
 *                            ErInitDev()                             *
 **********************************************************************
 *   Regester and init the IRQ handers for EVR.                       *
 **********************************************************************/

static long ErInitDev(int pass) {

    static int oneshotflag = 1;

    if (oneshotflag) {
        oneshotflag = 0;
        configure_lock = 1;
        ErResetAll();
        ErRegisterIrq();
    }

    if (pass == 1) {
        ErEnableIrq();
    }

    return 0;
}

/**********************************************************************
 *                            ErResetAll()                            *
 **********************************************************************
 *   Reset, clear and disable everything on the board.                *
 **********************************************************************/

static int ErResetAll()
{
    write_32(0x201d, Control);
    write_32(0x208d, Control);
    write_32(0, OutputPulseEnables);
    write_32(0, OutputLevelEnables);
    write_32(0, DelayPulseEnables);
    write_32(0, DelayPulseSelect);
    write_32(0, IrqEnables);;
    write_32()

}
