Single Parity Bit Correction in C
---------------------------------

The program simulates single bit parity bit correction. It uses 7 bit characters (0-127) and uses the 8th bit as a parity bit.

Parity bit: The added 8th parity bit will make the total number of 1s an even amount.

It reads 48 characters (from 0 [ASCII 48] to z [ASCII 122]), partitions them into groups of K characters, simulates a bit error per row and then corrects them.

input.txt: string with parity bit
output.txt: corrected, original string