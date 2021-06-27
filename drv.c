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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>

#define FATAL do { fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", \
    __LINE__, __FILE__, errno, strerror(errno)); exit(1); } while(0)

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

#define    control                0x00 
#define    eventramaddr           0x00
#define    eventramdata           0x00
#define    outputpulseenables     0x00
#define    outputlevelenables     0x00
#define    triggereventenables    0x00
#define    eventcounterlo         0x00
#define    eventcounterhi         0x00
#define    timestamplo            0x00
#define    timestamphi            0x00
#define    eventfifo              0x00  /* lsb = event #, msb  lsb of time stamp counter */
#define    EventTimeHi            0x00  /* bits 23-8 of the time stamp counter */
#define    DelayPulseEnables      0x00
#define    DelayPulseSelect       0x00  /* OTP pulses have delays and widths. See HW technical reference */
#define    PulseDelay             0x00
#define    PulseWidth             0x00
#define    IrqVector              0x00
#define    IrqEnables             0x00

/*---------------------
   Extended registers 
 --------------------*/
#define    DBusEnables            0x00/* Distributed bus enable register */
#define    DBusData               0x00/* Distributed bus data register (read only) */
#define    DelayPrescaler         0x00/* Programmable delay pulse clock prescaler */
#define    EventCounterClock      0x00/* Event counter clock prescaler */
#define    Resvd1                 0x00/* 0x2c */
#define    FPGAVersion            0x00/* FPGA Firmware version number */
#define    Resvd3                 0x00/* 0x30 */
#define    Resvd4                 0x00/* 0x32 */
#define    Resvd5                 0x00/* 0x34 */
#define    Resvd6                 0x00/* 0x36 */
#define    Resvd7                 0x00/* 0x38 */
#define    Resvd8                 0x00/* 0x3a */
#define    Resvd9                 0x00/* 0x3c */
#define    Resvd10                0x00/* 0x3e */
#define    FP0Map /* 0x040, Front panel output mapping register */
#define    FP1Map /* 0x042, Front panel output mapping register */
#define    FP2Map /* 0x044, Front panel output mapping register */
#define    FP3Map /* 0x046, Front panel output mapping register */
#define    FP4Map /* 0x048, Front panel output mapping register */
#define    FP5Map /* 0x04a, Front panel output mapping register */
#define    FP6Map /* 0x04c, Front panel output mapping register */
#define    uSecDivider /*Resvd11 0x4e*/
#define    ExtEventCode  /* 0x50 */
#define    ClockControl 
#define    SecondsSR 
#define    TSSec 
#define    Resvd12 
#define    EvFIFOSec 
#define    EvFIFOEvCnt 
#define    OutputPol 
#define    ExtDelay /*extended/32-bit delay register*/
#define    ExtWidth /*extended/32-bit width register*/
#define    Presc0   /* 0x074, Front panel clock #0 presaler */
#define    Presc1	  /* 0x076, Front panel clock #1 presaler */
#define    Presc2   /* 0x078, Front panel clock #2 presaler */
/* gt	 	Resvd13*/ /* 0x7a*/
#define    DataBufCtrl /* 0x7a*/ /*leige */
#define    	RFpattern /* 0x7c*/ /*leige set RFpattern */
#define    FracDiv /* 0x80 leige */
  /* These registers are only of special interest and
     left outside EPICS support for the time being. TK, 25-JUL-05. */
#define    RfDelay 
#define    RxDelay 
#define    Resvd14 /* 0x8c leige */
#define    FBRFFrac /* 0x90 leige */
#define    FbRxFrac 
#define    RFDelyInit 
#define    RxDelyInit /* 09C */
#define    CML4Pat00 /* 0A0 */
#define    CML4Pat01 /* 0A4 */
#define    CML4Pat10 /* 0A8 */
#define    CML4Pat11 /* 0AC */
#define    CML4Ena /* 0B0 */
#define    CML4EnaResv_1 /* 0B4 */
#define    CML4EnaResv_2 /* 0B8 */
#define    CML4EnaResv_3 /* 0BC */
#define    CML5Pat00 /* 0C0 */
#define    CML5Pat01 /* 0C4 */
#define    CML5Pat10 /* 0C8 */
#define    CML5Pat11 /* 0CC */
#define    CML5Ena /* 0D0 */
#define    CML5EnaResv_1 /* 0B4 */
#define    CML5EnaResv_2 /* 0B8 */
#define    CML5EnaResv_3 /* 0BC */
#define    CML6Pat00 /* 0E0 */
#define    CML6Pat01 /* 0E4 */
#define    CML6Pat10 /* 0E8 */
#define    CML6Pat11 /* 0EC */
#define    CML6Ena /* 0F0 */
#define    CML6EnaResv_1 /* 0B4 */
#define    CML6EnaResv_2 /* 0B8 */
#define    CML6EnaResv_3 /* 0BC */


