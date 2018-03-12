#include <stdio.h> // for stderr
#include <stdlib.h> // for exit()
#include "types.h"
#include "utils.h"
#include "riscv.h"

void execute_rtype(Instruction, Processor *);
void execute_itype_except_load(Instruction, Processor *);
void execute_branch(Instruction, Processor *);
void execute_jal(Instruction, Processor *);
void execute_load(Instruction, Processor *, Byte *);
void execute_store(Instruction, Processor *, Byte *);
void execute_ecall(Processor *, Byte *);
void execute_lui(Instruction, Processor *);

/* op codes */
#define r_opcode 51
#define i_opcode 19
#define load_opcode 3
#define s_opcode 35
#define branch_opcode 99
#define uj_opcode 111
#define ecall_opcode 115
#define u_opcode 55

void execute_instruction(Instruction instruction,Processor *processor,Byte *memory) {    
    unsigned opcode = instruction.opcode;
    switch(opcode) { 
        case r_opcode:
            execute_rtype(instruction, processor);
            processor->PC += 4;
            break;
        case i_opcode:
            execute_itype_except_load(instruction, processor);
            processor->PC += 4;
            break;
        case load_opcode:
            execute_load(instruction, processor, memory);
            processor->PC += 4;
            break;
        case s_opcode:
            execute_store(instruction, processor, memory);
            processor->PC += 4;
            break;
        case branch_opcode:
            execute_branch(instruction, processor);
            break;
        case uj_opcode:
            execute_jal(instruction, processor);
            break;
        case ecall_opcode:
            execute_ecall(processor, memory);
            break;
        case u_opcode:
            execute_lui(instruction, processor);
            processor->PC += 4;
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_rtype(Instruction instruction, Processor *processor) {
    unsigned funct3 = instruction.rtype.funct3;
    unsigned funct7 = instruction.rtype.funct7;    
    Register rd;
    Register rs1 = processor->R[instruction.rtype.rs1];
    Register rs2 = processor->R[instruction.rtype.rs2];
    switch(funct3) {
        case 000:
                if (funct7 == 0) {
                    /* add */
                    rd = rs1 + rs2;
                    processor->R[instruction.rtype.rd] = rd;
                    break;
                }
                else if (funct7 == 1) {
                    /* mul */
                    Double rd2 = rs1 * rs2;
                    rd2 = (rd2 << 32) >> 32;
                    processor->R[instruction.rtype.rd] = rd2;
                    break;
                }
                else if (funct7 == 32) {
                    /* sub */
                    rd = rs1 - rs2;
                    processor->R[instruction.rtype.rd] = rd;
                    break;
                }
                else {
                    handle_invalid_instruction(instruction);
                    break; 
                }
        case 001:            
                if (funct7 == 0) {
                    /* sll */
                    rd = rs1 << rs2;
                    processor->R[instruction.rtype.rd] = rd;
                    break;
                }
                else if (funct7 == 1) {
                    /* mulh */
                    Double rd2 = rs1 * rs2;
                    rd2 = rd2 >> 32;
                    processor->R[instruction.rtype.rd] = rd2;
                    break;
                }
                else {
                    handle_invalid_instruction(instruction);
                    break;     
                }
        case 2:
            if (funct7 == 0) {
                /* slt */
                rd = (sWord) rs1 < (sWord) rs2;
                processor->R[instruction.rtype.rd] = rd;
                break;
            } else {
                handle_invalid_instruction(instruction);
                break; 
            }
        case 4:
            if (funct7 == 0) {
                /* xor */
                rd = rs1 ^ rs2;
                processor->R[instruction.rtype.rd] = rd;
                break;
            }
            else if (funct7 == 1) {
                /* div */
                rd = rs1 / rs2;
                processor->R[instruction.rtype.rd] = rd;
                break;
            }
            else {
                handle_invalid_instruction(instruction);
                break; 
            }        
        case 5:
            if (funct7 == 0) {
                /* srl */
                rd = rs1 >> rs2;
                processor->R[instruction.rtype.rd] = rd;
                break;
            }
            else if (funct7 == 32) {
                /* sra */
                sWord tempd, temp1, temp2;
                temp1 = (sWord) rs1;
                temp2 = (sWord) rs2;
                tempd = temp1 >> temp2;
                rd = tempd;
                processor->R[instruction.rtype.rd] = rd;
                break;
            }                
            else {
                handle_invalid_instruction(instruction);
                break; 
            }               
        case 6:
            if (funct7 == 0) {
                /* or */
                rd = rs1 | rs2;
                processor->R[instruction.rtype.rd] = rd;
                break;
            }
            else if (funct7 == 1) {
                /* rem */
                rd = rs1 % rs2;
                processor->R[instruction.rtype.rd] = rd;
                break;
            }
            else {
                handle_invalid_instruction(instruction);
                break;                    
            }     
        case 7:              
            if (funct7 == 0) {
                /*and*/
                rd = rs1 & rs2;
                processor->R[instruction.rtype.rd] = rd;
                break;
            } else {
                handle_invalid_instruction(instruction);
                exit(-1);
                break;
            } 
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_itype_except_load(Instruction instruction, Processor *processor) {
    // int shiftOp;
    // shiftOp = -1;
    unsigned funct3 = instruction.itype.funct3;
    Register rs1 = processor->R[instruction.itype.rs1];
    Register rd;
    int imm = instruction.itype.imm;
    int check = imm >> 5;
    switch(funct3) {
        case 000: {
            /* addi */
            if (imm > 2047) {
                imm = imm - 4096;
            }
            rd = rs1 + imm;
            processor->R[instruction.itype.rd] = rd;
            break;
        }
        case 001: {
            /* slli */     
            if (imm > 2047) {
                imm = imm - 4096;
            }
            rd = rs1 << imm;   
            processor->R[instruction.itype.rd] = rd;
            break;
        }
        case 2: {
            /* slti */ 
            if (imm > 2047) {
                imm = imm - 4096;
            }
            int val;
            val = (int) rs1;
            rd = val < imm;   
            processor->R[instruction.itype.rd] = rd;
            break;
        }
        case 4: {
            /* xori */    
            rd = rs1 ^ imm;  
            processor->R[instruction.itype.rd] = rd;
            break;
        }
        case 5:
            /* srli or srai */
            if (imm > 2047) {
                imm = imm - 4096;
            }
            if (check == 0) {
                /* srli */
                imm = imm - (check << 5);
                rd = rs1 >> imm;
                processor->R[instruction.itype.rd] = rd;
                break;
            }
            else if (check == 32) {
                if (imm > 2047) {
                    imm = imm - 4096;
                }
                /* srai */
                imm = imm - (check << 5);
                sWord tempd, temp1, temp2;
                temp1 = (sWord) rs1;
                temp2 = (sWord) imm;
                tempd = temp1 >> temp2;
                rd = tempd;
                processor->R[instruction.itype.rd] = rd;
                break;      
            }          
            else {
                handle_invalid_instruction(instruction);
                break; 
            }                              
        case 6: {
            /* ori */
            rd = rs1 | imm;
            processor->R[instruction.itype.rd] = rd;
            break;
        }
        case 7: {              
            /* andi */   
            rd = rs1 & imm;
            processor->R[instruction.itype.rd] = rd;
            break;
        }
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}

void execute_ecall(Processor *p, Byte *memory) {
    Register a0 = p->R[10];
    switch(a0) { // What do we switch on?
        /* YOUR CODE HERE */
        case 1:
            printf("%i", p->R[11]);
            p->PC += 4;
            break;

        case 10:
            printf("exiting the simulator\n");
            exit(0);
            break;

        default: // undefined ecall
            printf("Illegal ecall number %d\n", -1); // What stores the ecall arg?
            exit(-1);
            break;
    }
}

void execute_branch(Instruction instruction, Processor *processor) {
    // int branchaddr;
    // branchaddr = 0;
    unsigned funct3 = instruction.sbtype.funct3;
    Register rs1 = processor->R[instruction.sbtype.rs1];
    Register rs2 = processor->R[instruction.sbtype.rs2];
    int offset = get_branch_offset(instruction);
    /* Remember that the immediate portion of branches
       is counting half-words, so make sure to account for that. */
    switch(funct3) {
        case 000: {
            /* beq */
            if (rs1 == rs2) {
                (processor->PC) += offset;
            } else {
                processor->PC += 4;
            }
            break;
        }
        case 001: {
            /* bne */
            if (rs1 != rs2) {
                (processor->PC) += offset;
            } else {
                processor->PC += 4;
            }
            break;
        }
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_load(Instruction instruction, Processor *processor, Byte *memory) {
    unsigned funct3 = instruction.itype.funct3;
    int offset = bitSigner(instruction.itype.imm, 32);
    if (offset > 2047) {
        offset = offset - 4096;
    }
    switch(funct3) { 
        case 000: {
            Word output = load(memory, processor->R[instruction.itype.rs1] + offset, LENGTH_BYTE, 0);
            if (output > 127) {
                output = ((sWord) output << 24) >> 24;
            }
            processor->R[instruction.itype.rd] = output;
            break;
        }
        case 001: {
            Word output = load(memory, processor->R[instruction.itype.rs1] + offset, LENGTH_HALF_WORD, 0);
            if (output > 32767) {
                output = ((sWord) output << 16) >> 16;
            }
            processor->R[instruction.itype.rd] = output;
            /* lh */
            break;
        }
        case 2: {
            Word output = load(memory, processor->R[instruction.itype.rs1] + offset, LENGTH_WORD, 0);
            processor->R[instruction.itype.rd] = output;
            /* lw */
            break;
        }
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}

void execute_store(Instruction instruction, Processor *processor, Byte *memory) {
    unsigned funct3 = instruction.stype.funct3;
    int offset = get_store_offset(instruction);
    switch(funct3) {
        case 000: {
            /* sb */
            store(memory, processor->R[instruction.stype.rs1] + offset, LENGTH_BYTE, 
                processor->R[instruction.stype.rs2], 0);
            break;
        }
        case 001: {
            /* sh */
            store(memory, processor->R[instruction.stype.rs1] + offset, LENGTH_HALF_WORD, 
                processor->R[instruction.stype.rs2], 0);
            break;
        }        
        case 2: {
            /* sw */
            store(memory, processor->R[instruction.stype.rs1] + offset, LENGTH_WORD, 
                processor->R[instruction.stype.rs2], 0);
            break;
        }
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_jal(Instruction instruction, Processor *processor) {
    // int offset = get_jump_offset(instruction);
    // int nextPC;
    // nextPC = processor->PC + 4;
    // processor->PC += offset;
    // /* TO DO: Call execute_instruction on next instruction */
    // processor->PC = nextPC;

    int offset = get_jump_offset(instruction);
    processor->R[instruction.ujtype.rd] = processor->PC + 4;
    processor->PC += offset;

}

void execute_lui(Instruction instruction, Processor *processor) {
    Word imm = instruction.utype.imm << 12;
    processor->R[instruction.utype.rd] = imm;
}

/* Checks that the address is aligned correctly */
int check(Address address,Alignment alignment) {
    if(address>0 && address < MEMORY_SPACE){
        if(alignment == LENGTH_BYTE){
            return 1;
        }
        else if( alignment == LENGTH_HALF_WORD ){
            return address%2 == 0;
        }
        else if (alignment == LENGTH_WORD){
            return address%4 ==0;
        }
    }
    
    return 0;
    
}

void store(Byte *memory,Address address,Alignment alignment,Word value, int check_align) {
    if((check_align && !check(address,alignment)) || (address >= MEMORY_SPACE)) {
        handle_invalid_write(address);
    }
    switch (alignment){
        case 4: {
            memory[address] = (value << 24) >> 24;
            memory[address+1] = (value << 16) >> 24;
            memory[address+2] = (value << 8) >> 24;
            memory[address=3] = value >> 24;
            break;
        }
        case 2: {
            memory[address] = (value << 24) >> 24;
            memory[address+1] = (value << 16) >> 24;
            break;
        }
        case 1: {
            memory[address] = (value << 24) >> 24;
            break;
        }
    }
    
}

Word load(Byte *memory,Address address,Alignment alignment, int check_align) {
    if((check_align && !check(address,alignment)) || (address >= MEMORY_SPACE)) {
        handle_invalid_read(address);
    }

    uint32_t data = 0; 
    switch (alignment){
        case 4: {
            data += memory[address];
            data += (memory[address+1] << 8);
            data += (memory[address+2] << 16);
            data += (memory[address+3]) <<24;
            break; 
        }
        case 2: {
            data += memory[address];
            data += (memory[address+1] << 8);
            break;
        }
        case 1: {
            data += memory[address];
            break;
        }
        default:
            handle_invalid_read(address);
            break;
    }


    return data;
}


