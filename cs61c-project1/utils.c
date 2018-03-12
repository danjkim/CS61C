#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//sign extends a bitfield with given size
/* You may find implementing this function helpful */
int bitSigner(unsigned int field, unsigned int size){
    int number_of_digits = 0;
    Word check = field;
    while (check != 0) {
        check = check >> 1;
        number_of_digits += 1;
    }
    sWord ret;
    ret = (field << (size - number_of_digits)) >> (size-number_of_digits);
    return ret;
}

/* Remember that the offsets should return the offset in BYTES */

// Daniel Kim - I did integer-wise operations for branch and jump
// and figured out that I could do bitwise operations and did bitwise for get_store.
//
//
//
//
//
//


int get_branch_offset(Instruction instruction) {
    unsigned imm5 = instruction.sbtype.imm5;
    unsigned imm7 = instruction.sbtype.imm7;
    unsigned imm11, imm12, bits;

    imm11 = (imm5 & 1) << 10;
    imm5 = imm5 >> 1;
    imm12 = (imm7 & (1 << 6)) << 5;
    imm7 = (imm7 & ~(1 << 6)) <<4;

    bits = imm11 | imm5 | imm12 | imm7;
    bits = bitSigner(bits, 32);
    bits = bits*2;

    if (bits > 4095) {
        bits = bits - 8192;
    }

    /* finish */
    return bits;
}

int get_jump_offset(Instruction instruction) {
    unsigned imm = instruction.ujtype.imm;
    unsigned imm20, imm10, imm11, imm19, bits;
    imm20 = (imm >> 19) << 19;
    imm10 = ((imm >> 9) << 22) >> 22;
    imm11 = ((imm >> 8) << 31) >> 21;
    imm19 = (imm << 24) >> 13;
    bits = imm20 | imm10 | imm11 | imm19;
    bits = bitSigner(bits, 32);
    bits = bits*2;
    if (bits > 1048575) {
        bits = bits - 2097152;
    }
    /* finish */
    return bits;
}

int get_store_offset(Instruction instruction) {
    unsigned imm5 = instruction.stype.imm5;
    unsigned imm7 = instruction.stype.imm7;
    unsigned bits = imm5 | (imm7 << 5);
    bits = bitSigner(bits, 32);
    if (bits > 2047) {
        bits = bits - 4096;
    }
    /* finish */
    return bits;
}

void handle_invalid_instruction(Instruction instruction) {
    printf("Invalid Instruction: 0x%08x\n", instruction.bits); 
}

void handle_invalid_read(Address address) {
    printf("Bad Read. Address: 0x%08x\n", address);
    exit(-1);
}

void handle_invalid_write(Address address) {
    printf("Bad Write. Address: 0x%08x\n", address);
    exit(-1);
}

