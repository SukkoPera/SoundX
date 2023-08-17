/* SoundX GAL Equations
 * Copyright 2023 SukkoPera <software@sukkology.net>
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/* Inputs */
Pin 1 = a15;
Pin 2 = a14;
Pin 3 = a13;
Pin 4 = a12;
Pin 5 = a11;
Pin 6 = a10;
Pin 7 = a8;
Pin 8 = a6;
Pin 9 = a4;
Pin 10 = a2;
Pin 11 = a0;
Pin 14 = a1;
Pin 16 = a3;
Pin 17 = a5;
Pin 18 = a7;
Pin 19 = a9;

Pin 13 = phi2;
Pin 23 = rw;

/* Outputs */
Pin 15 = midi_cs;
Pin 20 = ym_cs;
Pin 21 = ym_rd;
Pin 22 = ym_a0;


/** Declarations and Intermediate Variable Definitions **/
Field addr = [a15..0];


/** Logic Equations **/

/* The logic implemented on the original C64 Sound Expander uses 3 registers:
 * 
 *      R/W A5 A4 /IO2 PHI2 | /CS /RD /WR A0 FUNCTION
 * DF40 0/W 0  0  0    0    | 1   1   0   0  Hi-Z
 * DF40 0/W 0  0  0    1    | 0   1   0   0  Write Address
 * DF40 1/R 0  0  0    0    | 1   1   1   0  Hi-Z
 * DF40 1/R 0  0  0    1    | 0   1   1   0  ???
 * DF50 0/W 0  1  0    0    | 1   1   0   1  Hi-Z
 * DF50 0/W 0  1  0    1    | 0   1   0   1  Write Data
 * DF50 1/R 0  1  0    0    | 1   1   1   1  Hi-Z
 * DF50 1/R 0  1  0    1    | 0   1   1   1  ???
 * DF60 0/W 1  0  0    0    | 1   1   0   0  Hi-Z
 * DF60 0/W 1  0  0    1    | 0   1   0   0  Write Address (Do not use)
 * DF60 1/R 1  0  0    0    | 1   0   1   0  Hi-Z
 * DF60 1/R 1  0  0    1    | 0   0   1   0  Status Read
 * 
 * 
 * But first of all we must used a different address. We have MIDI I/O onboard and we can map that to $FDE0/1: those are
 * reserved for MIDI in Solder's I/O map and even though I have never seen his MIDI interface, I have a strong suspicion
 * that it works exactly the same as ours, which is a good enough reason to use the same addresses. Then, in the sake of
 * consistency, we can just use the subsequent addresses for the OPL registers. If we wanted to do the same as the Sound
 * Expander we would need 3 addresses, which would bring the total to 5. It would be much better if we could reduce that
 * to 4 in order not to pollute the address space, so let's go AdLib-style! Instead of using a dedicated address for the
 * status register, which is read-only, we map that to the address of the address latch (which is write-only). We also
 * map that to the data latch, which is not strictly AdLib-style, but well, we need to do something. We can always
 * change that later on, that's why we use a GAL for address decoding :). So, all in all:
 * 
 *      R/W A1 A0 PHI2 | /CS /RD /WR A0 FUNCTION
 * FDE2 x   1  0  0    | 1   x   x   x  Hi-Z
 * FDE2 0/W 1  0  1    | 0   1   0   0  Write Address
 * FDE2 1/R 1  0  1    | 0   0   1   0  Status Read
 * FDE3 x   1  1  0    | 1   x   x   x  Hi-Z
 * FDE3 0/W 1  1  1    | 0   1   0   1  Write Data
 * FDE3 1/R 1  1  1    | 0   0   1   0  Status Read
 */

addressed = phi2 & addr:'b'1111110111100xxx;	// $FDE0-$FDE7
wrAddress = addressed & !rw & !a2 & a1 & !a0;	// $FDE2 Write
rdStatus1 = addressed & rw & !a2 & a1 & !a0;	// $FDE2 Read
wrData = addressed & !rw & !a2 & a1 & a0;		// $FDE3 Write
rdStatus2 = addressed & rw & !a2 & a1 & a0;		// $FDE3 Read
kbdRead = addressed & a2 & !a1 & a0;			// $FDE5

ym_cs = !(wrAddress # wrData # rdStatus1 # rdStatus2);
ym_rd = !(rdStatus1 # rdStatus2);
ym_a0 = wrData;
midi_cs = !kbdRead;
