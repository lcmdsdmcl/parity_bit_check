
#include <stdio.h>
#include <stdlib.h>

#define K_VAL 15             // number of characters per row
#define FILE_IN "input.txt"   // input file name
#define FILE_OUT "output.txt"  // output file name

/* *** Prints out Character in Binary form ***
 * input: unsigned character
 */
void printBinary(unsigned char ch) {

    unsigned int binArr[8]; // binary buffer
    int binNo = 0; // number of digits

    while (ch) {
        if (ch & 1)
            binArr[binNo] = 1;
        else
            binArr[binNo] = 0;
        ch = ch >> 1;

        binNo++;
    }
    while (binNo < 8) { //the rest is '0'
        binArr[binNo] = 0;
        binNo++;
    }

    for (binNo = 7; binNo >=0; binNo--) {  // print it out backwards
        printf("%d", binArr[binNo]);
    }

    printf(" ");
}

/* *** checks whether number of 1s is odd or even ***
 * input:
 *      unsigned character
        number of bits (7 or 8)
 *
 * return:
 *      -1 if it's not a 7 or 8 bit character
 *       0 if the number is even
 *       1 if the number is odd
*/
int parityCheck(unsigned char ch, int bitNo) { // checks if number of 1s is odd or even

	int i;
	int binaryOne = 0;

	if (bitNo != 7 && bitNo != 8)   // if bitNo is not 7 or 8, returns -1
		return -1;

	for (i = 0; i < bitNo; i++) {   // 7 or 8 bit characters
        if (ch & 1)                 // if the lowest bit is 1 then increment
            binaryOne++;

        ch = ch >> 1;               // next bit
    }

    if (binaryOne % 2 != 0)         // if the number of 1s is odd
		return 1;
	else
		return 0;
}

/* *** Sets parity bit if needed ***
* input: unsigned character
* returns: character with parity bit
*/
unsigned char setParity(unsigned char ch) {

    int parity;

    if (ch & 128) {         // if the character is 8bit
        printf("Error. It's an 8bit character!\n");
        exit(1);
    }

    parity = parityCheck(ch, 7);

    if (parity == 1)       // if the number of 1s is odd
        ch = ch | 128;     // 8th bit is set to one

    return ch;
}

/* *** Fills a test file with data *** */
void fillFile() {

    unsigned char ch = '0';         // first character ASCII 48
    unsigned char parityCode = 0;   // K+1. value
    int i;

    FILE *filePtr;
    filePtr = fopen(FILE_IN, "w");
    if (filePtr == NULL) {
        printf("Error. Could not write to file. \n");
        exit(1);
    }

    for (i = 1; i <= 75; i++) {     // from 0=48 to z=48+74: charNo%K_VAL != 1
        fputc(setParity(ch), filePtr);
        parityCode ^= setParity(ch);// XOR values = "K+1" value
        ch++;

        if (i % K_VAL == 0) {       // add "K+1." value and endline
            fputc(parityCode, filePtr);
            fputc('\n', filePtr);
            parityCode = 0;         // set "K+1" value to 0 for new line
        }
    }

    if (parityCode != 0)            // if there are still characters without "K+1." value
        fputc(parityCode, filePtr);

    fclose(filePtr);
}

/* *** Parity character correction & data backup *** */
void charCorrection() {

    unsigned int buff[K_VAL+1];
    int max = 0;    // characters per line
    int i = 0;
    unsigned int parityCode = 0;
    unsigned int errorBit = 0;
    unsigned int ch;

    FILE *filePtr;  // INPUT File handler
    FILE *filePtr2; // OUTPUT File handler

    filePtr = fopen(FILE_IN, "r");
    if (filePtr == NULL) {
        printf("Error. Could not open the file. \n");
        exit(1);
    }

    filePtr2 = fopen(FILE_OUT, "w");
    if (filePtr2 == NULL) {
        printf("Error. Could not write into file. \n");
        exit(1);
    }

    do {                // scan the whole file
        parityCode = 0; // reset values
        errorBit = 0;
        max = 0;

        do {           // scan one row
            ch = fgetc(filePtr);
            buff[max] = ch;

            max++;
        } while (ch != '\n' && ch != EOF);

        /* *** Simulated Bit Error :: Beginning *** */
        buff[0] = 67;   // simulate bit error, set A->C: 010000001->01000011
        /* *** Simulated Bit Error :: End *** */

        /* Calculating the new "K+1." parity code */
        for (i = 0; i < max-2; i++) {
            parityCode ^= buff[i];
        }

        /* "K+1." value correction */
        if (parityCode != buff[K_VAL]) {        // if the "K+1." value is wrong
            errorBit = parityCode^buff[K_VAL];  //XOR the two "K+1." value

            for (i = 0; i < max-2; i++) {
                if (parityCheck(buff[i] ,8) == 1) // if there's a wrong bit
                    buff[i] = buff[i]^errorBit;  // XOR value with the "new" K+1. value
            }
        }

        /* Converting back to 7 bit from 8 bit */
        for (i = 0; i < max-2; i++) {
            if (buff[i] > 127)          // if there's an extra bit
                buff[i] = buff[i]^128;  // remove the 8th bit
        }

        /* *** Write correct characters into OUTPUT File *** */
        for (i = 0; i < max-2; i++)
            fputc((char)buff[i], filePtr2);
        fputc('\n', filePtr2);


    } while (ch != EOF);

    fclose(filePtr);
    fclose(filePtr2);
}

/* *** Prints out the output file *** */
void printFile() {

    FILE *filePtr;
    int ch;

    filePtr = fopen(FILE_OUT, "r");
    if (filePtr == NULL) {
        printf("Error. Could not open the file. \n");
        exit(1);
    }

    while ((ch = fgetc(filePtr)) != EOF)
        printf("%c", (char)ch);

    fclose(filePtr);
}

/* *** Main() *** */
int main() {

    printf("... filling the FILE_IN with data \n");
    fillFile();

    printf("... reading data from FILE_IN \n");
    printf("... simulating data corruption \n");
    printf("... saving data into FILE_OUT \n");
    charCorrection();

    printf("... printing out the corrected data \n");
    printFile();

    return 0;
}
