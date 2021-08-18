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

#include <asm-generic/errno.h>
#include <errno.h>
#include <features.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************
 *  epics device support header files  *
 ***************************************/

/* #include <dbScan.h> */
/* #include <devSup.h> */

/************************
 *  local header files  *
 ************************/

#include "dev.h"
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
 *   Init routine for the ER record type.                             *
 **********************************************************************/

static long init_record(struct erRecord *pRec) {
    proc();

    return 0;
}

/**********************************************************************
 *                               proc()                               *
 **********************************************************************
 *   Process routine for the ER record type.                          *
 **********************************************************************/

static long proc(struct erRecord *pRec) {
    if (pRec->tpro > 10) {
        printf("dev::proc(%s) entered\n", pRec->name);
    }

    if (pRec->enab != ErMasterEnableGet()) {
        ErMasterEnableSet(pRec->enab);
    }
}

/**********************************************************************
 *                        ErMasterEnableGet()                         *
 **********************************************************************
 *   Inquire as to if EVR is enabled or not.                          *
 **********************************************************************/

static long ErMasterEnableGet() { return ((get_32(Control) & 0x8000) != 0); }

/**********************************************************************
 *                        ErMasterEnableSet()                         *
 **********************************************************************
 *   Enable/disable EVR.                                              *
 **********************************************************************/

static long ErMasterEnableSet(int enable) {
    if (enable != 0) {
        write_32((get_32(Control) & CONTROL_REG_OR_STRIP) | 0x8000, Control);
    } else {
        write_32((get_32(Control) & CONTROL_REG_OR_STRIP) & ~0x8000, Control);
    }

    return 0;
}

/**********************************************************************
 *                              ErSetTrg                              *
 **********************************************************************
 *   Set/clear an enable bit in the trigger mask.                     *
 **********************************************************************/

static long ErSetTrg(int channel, int enable) {
    unsigned short mask;

    mask = 1;
    mask <<= channel;

    if (enable != 0) {
        write_32((get_32(TriggerEventEnables) | mask), TriggerEventEnables);
    } else {
        write_32((get_32(TriggerEventEnables) & ~mask), TriggerEventEnables);
    }

    return 0;
}

*ErSetOtp() *
    /**********************************************************************
     **********************************************************************
     *   Set/clear an enable bit in the one-shot mask.                    *
     **********************************************************************/

    static long ErSetOtp(int channel, int enable, unsigned short width,
                         unsigned short pol, unsigned int delay) {
    unsigned short mask;
    unsigned int pol_mask;

    mask = 1;
    mask <<= channel;

    /* set the pluse width */
    write_32((channel | 0x10), DelayPulseSelect);
    write_32(width, PulseWidth);
    write_32(delay, ExtDelay);

    pol_mask = (ER_OT0_POL << channel);

    if (pol == 0) {
        write_32((get_32(OutputPol) & ~pol_mask), OutputPol);
    } else {
        write_32((get_32(OutputPol) | pol_mask), OutputPol);
    }

    if (enable != 0) {
        write_32((get_32(OutputPulseEnables) | mask), OutputPulseEnables);
        write_32((get_32(OutputPulseEnables) & ~mask), OutputPulseEnables);
    }

    return 0;
}

/**********************************************************************
 *                             ErSetOtl()                             *
 **********************************************************************
 *   Set/clear an enable bit in the level-trigger mask.               *
 **********************************************************************/

static long ErSetOtl(int channel, int enable) {
    unsigned short mask;

    mask = 1;
    mask <<= channel;

    if (enable != 0) {
        write_32((get_32(OutputLevelEnables) | mask), OutputLevelEnables);
        write_32((get_32(OutputLevelEnables) & ~mask), OutputLevelEnables);
    }

    return 0;
}

/**********************************************************************
 *                             ErSetOtb()                             *
 **********************************************************************
 *   Enable/Disable distributed bus channels on the output.           *
 **********************************************************************/

static long ErSetOtb(int channel, int enable) {
    unsigned short mask;

    mask = 1;
    mask <<= channel;

    if (enable != 0) {
        write_32((get_32(DBusEnables) | mask), DBusEnables);
        write_32((get_32(DBusEnables) & ~mask), DBusEnables);
    }

    return 0;
}

/**********************************************************************
 *                         ErGetFpgaVersion()                         *
 **********************************************************************
 *   Get FPGA version                                                 *
 **********************************************************************/

static long ErGetFpgaVersion() { return (get_32(FPGAVersion) & 0x0fff); }

/**********************************************************************
 *                           ErSetTickPre()                           *
 **********************************************************************
 *   Set the tick counter prescaler (0=count '124' events, other      *
 *   value= value to divide the internal clock with.)                 *
 **********************************************************************/

static long ErSetTickPre(int scaler_val) {
    if (!(scaler_val >= 0 || scaler_val < 65535)) {
        printf("ErSetTickPre: scaler_val: %d is out of range.");

        return -1;

    } else
        write_32(scaler_val, EventCounterClock);

    return 0;
}

/**********************************************************************
 *                            ErSetFPMap()                            *
 **********************************************************************
 *   Set the front panel output map.                                  *
 **********************************************************************/

static long ErSetFPMap(int channel, int map) {
    if (channel < 0 || channel > 6) return -1;

    if (map < 0 || map > EVR_FPMAP_MAX) return -1;

    switch (channel) {
        case 0:
            write_32(map, FP0Map);
            break;
        case 1:
            write_32(map, FP1Map);
            break;
        case 2:
            write_32(map, FP2Map);
            break;
        case 3:
            write_32(map, FP3Map);
            break;
        case 4:
            write_32(map, FP4Map);
            break;
        case 5:
            write_32(map, FP5Map);
            break;
        case 6:
            write_32(map, FP6Map);
            break;
    }

    return 0;
}

/**********************************************************************
 *                             ErSetDg()                              *
 **********************************************************************
 *   Set/clear an enable bit in the programmable pulse delay mask.    *
 *   If enabling a channel, the delay and width are also set.         *
 **********************************************************************/

static long ErSetDg(int channel, int enable, unsigned int delay,
                    unsigned int width, unsigned short prescaler,
                    unsigned short pol) {
    unsigned short mask, pol_mask;

    mask = 1;
    mask <<= channel;
    pol_mask = 1;
    pol_mask <<= channel + 4;

    if (enable != 0) {
        write_32(channel, DelayPulseSelect);
        write_32(delay, ExtDelay);
        write_32(width, ExtWidth);
        write_32(prescaler, DelayPrescaler);
        write_32((get_32(DelayPulseEnables) | mask), DelayPulseEnables);

        if (pol != 0)
            write_32((get_32(DelayPulseEnables) | pol_mask), DelayPulseEnables);
        else
            write_32((get_32(DelayPulseEnables) & ~pol_mask),
                     DelayPulseEnables);

    } else
        write_32((get_32(DelayPulseEnables) & ~mask), DelayPulseEnables);

    return 0;
}
