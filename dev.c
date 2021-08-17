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
#define CONTROL_REG_OR_STRIP (~0x3c9f)
#define NUM_EVENTS 257

#define ER_IRQ_ALL 0x001f
#define ER_IRQ_OFF 0x0000
#define ER_IRQ_TELL 0xffff
#define ER_IRQ_EVENT 0x0008
#define ER_IRQ_HEART 0x0004
#define ER_IRQ_FIFO 0x0002
#define ER_IRQ_TAXI 0x0001
#define ER_IRQ_DVME 0x0010
#define ER_OT0_POL 0x0800

/* Fractional synthesizer word setting. From HW doc. */
#define FR_SYNTH_WORD 0x0c928166

/* leige add FR_SYNTH_WORD definition for BEPCII */
#define BEPCII_FRAC_4 0x00FE816D
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

static int ErResetAll() {
    write_32(0x201d, Control);
    write_32(0x208d, Control);
    write_32(0, OutputPulseEnables);
    write_32(0, OutputLevelEnables);
    write_32(0, DelayPulseEnables);
    write_32(0, DelayPulseSelect);
    write_32(0, IrqEnables);
    write_32((get_32(Control) & CONTROL_REG_OR_STRIP) | 0x8000, Control);

    return 0;
}

/**********************************************************************
 *                          ErRegisterIrq()                           *
 **********************************************************************
 *   Set up the IRQs for the receiver.                                *
 **********************************************************************/

static ErRegisterIrq() {}

/**********************************************************************
 *                           ErEnableIrq()                            *
 **********************************************************************
 *   Enable or disable the IRQs from the EVR.                         *
 **********************************************************************/

static unsigned int ErEnableIrq(unsigned int mask) {
    if (mask == ER_IRQ_OFF) {
        write_32(0, IrqEnables);
        write_32((get_32(Control) & CONTROL_REG_OR_STRIP) & ~0x4000, Control);

    } else if (mask == ER_IRQ_TELL) {
        if (get_32(Control) & 0x4000) return (get_32(IrqEnables & ER_IRQ_ALL));

    } else {
        write_32(mask, IrqEnables);
        write_32((get_32(Control) & CONTROL_REG_OR_STRIP) | 0x4000, Control);
    }

    return 0;
}

/**********************************************************************
*                           init_record()                            *
**********************************************************************
*                Init routine for the ER record type.                *
**********************************************************************/

static long init_record(struct erRecord *pRec)
{

}

