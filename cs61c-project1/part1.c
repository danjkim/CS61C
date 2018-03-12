#include <stdio.h> // for stderr
#include <stdlib.h> // for exit()
#include "types.h"
#include "utils.h"

/* op codes */
#define r_opcode 51
#define i_opcode 19
#define load_opcode 3
#define s_opcode 35
#define branch_opcode 99
#define uj_opcode 111
#define ecall_opcode 115
#define u_opcode 55

void print_rtype(char *, Instruction);
void print_itype_except_load(char *, Instruction, int);
void print_load(char *, Instruction);
void print_store(char *, Instruction);
void print_branch(char *, Instruction);
void write_rtype(Instruction);
void write_itype_except_load(Instruction); 
void write_load(Instruction);
void write_store(Instruction);
void write_branch(Instruction);
void write_lui(Instruction);
void write_jal(Instruction);
void write_ecall(Instruction);

void decode_instruction(Instruction instruction) {    
    unsigned opcode = instruction.opcode;
    switch(opcode) { 
        case r_opcode:
            write_rtype(instruction);
            break;
        case i_opcode:
            write_itype_except_load(instruction);
            break;
        case load_opcode:
            write_load(instruction);
            break;
        case s_opcode:
            write_store(instruction);
            break;
        case branch_opcode:
            write_branch(instruction);
            break;
        case uj_opcode:
            write_jal(instruction);
            break;
        case ecall_opcode:
            write_ecall(instruction);
            break;
        case u_opcode:
            write_lui(instruction);
            break;
    	default: // undefined opcode
            handle_invalid_instruction(instruction);
    	    break;
    }
}

void write_rtype(Instruction instruction) {
	unsigned funct3 = instruction.rtype.funct3;
    unsigned funct7 = instruction.rtype.funct7;
    switch(funct3) { 
        case 000:
                if (funct7 == 0) {
                    /* add */
                    char name[] =  "add";
                    print_rtype(name, instruction);
                    break;
                }
                else if (funct7 == 1) {
                    /* mul */
                    char name[] = "mul";
                    print_rtype(name, instruction);
                    break;
                }
                else if (funct7 == 32) {
                    /* sub */
                    char name[] = "sub";
                    print_rtype(name, instruction);
                    break;
                }
                else {
                    handle_invalid_instruction(instruction);
                    break; 
                }
        case 001:            
                if (funct7 == 0) {
                    /* sll */
                    char name[] = "sll";
                    print_rtype(name, instruction);
                    break;
                }
                else if (funct7 == 1) {
                    /* mulh */
                    char name[] = "mulh";
                    print_rtype(name, instruction);
                    break;
                }
                else {
                    handle_invalid_instruction(instruction);
                    break;     
                }
        case 2:
            /* slt */
            if (funct7 == 0) {
                char name[] = "slt";
                print_rtype(name, instruction);
                break;
            } else {
                handle_invalid_instruction(instruction);
                break; 
            }
        case 4:
            if (funct7 == 0) {
                /* xor */
                char name[] = "xor";
                print_rtype(name, instruction);
                break;
            }
            else if (funct7 == 1) {
                /* div */
                char name[] = "div";
                print_rtype(name, instruction);
                break;
            }
            else {
                handle_invalid_instruction(instruction);
                break; 
            }        
        case 5:
            if (funct7 == 0) {
                /* srl */
                char name[] = "srl";
                print_rtype(name, instruction);
                break;
            }
            else if (funct7 == 32) {
                /* sra */
                char name[] = "sra";
                print_rtype(name, instruction);
                break;
            }                
            else {
                handle_invalid_instruction(instruction);
                break; 
            }               
        case 6:
            if (funct7 == 0) {
                /* or */
                char name[] = "or";
                print_rtype(name, instruction);
                break;
            }
            else if (funct7 == 1) {
                /* rem */
                char name[] = "rem";
                print_rtype(name, instruction);
                break;
            }
            else {
                handle_invalid_instruction(instruction);
                break;                    
            }     
        case 7:              
            /* and */
            if (funct7 == 0) {
                char name[] = "and";
                print_rtype(name, instruction);
                break;
            } else {
                handle_invalid_instruction(instruction);
                break;
            } 
	    default:
            handle_invalid_instruction(instruction);
            break;
	}
}

void write_itype_except_load(Instruction instruction) {
    // int shiftOp;
    // shiftOp = -1;
    unsigned funct3 = instruction.itype.funct3;
    int imm = instruction.itype.imm;
    if (imm > 2047) {
        imm = imm - 4096;
    }
    int check = imm >> 5;
    switch(funct3) { 
        case 000: {
            /* addi */
            char name[] = "addi";
            print_itype_except_load(name, instruction, imm);
            break;
        }
        case 001: {
            /* slli */
            char name[] = "slli";
            print_itype_except_load(name, instruction, imm);        
            break;
        }
        case 2: {
            /* slti */
            char name[] = "slti";
            print_itype_except_load(name, instruction, imm);        
            break;
        }
        case 4: {
            /* xori */
            char name[] = "xori";
            print_itype_except_load(name, instruction, imm);        
            break;
        }
        case 5:
            /* srli or srai */
            if (check == 0) {
                /* srli */
                char name[] = "srli";
                imm = imm - (check << 5);
                print_itype_except_load(name, instruction, imm);
                break;
            }
            else if (check == 32) {
                /* srai */
                char name[] = "srai";
                imm = imm - (check << 5);
                print_itype_except_load(name, instruction, imm);
                break;      
            }          
            else {
                handle_invalid_instruction(instruction);
                break; 
            }                              
        case 6: {
            /* ori */
            char name[] = "ori";
            print_itype_except_load(name, instruction, imm);       
            break;
        }
        case 7: {              
            /* andi */
            char name[] = "andi";
            print_itype_except_load(name, instruction, imm);      
            break;
        }
        default:
            handle_invalid_instruction(instruction);
            break;         
    }
}

void write_load(Instruction instruction) {
    unsigned funct3 = instruction.itype.funct3;
    switch(funct3) { 
        case 000: {
            /* lb */
            char name[] = "lb";
            print_load(name, instruction);
            break;
        }
        case 001: {
            /* lh */
            char name[] = "lh";
            print_load(name, instruction);
            break;
        }
        case 2: {
            /* lw */
            char name[] = "lw";
            print_load(name, instruction);
            break;
        }
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}

void write_store(Instruction instruction) {
    unsigned funct3 = instruction.stype.funct3;
    switch(funct3) { // What do we switch on?
        case 000: {
            /* sb */
            char name[] = "sb";
            print_store(name, instruction);
            break;
        }
        case 001: {
            /* sh */
            char name[] = "sh";
            print_store(name, instruction);
            break;
        }        
        case 2: {
            /* sw */
            char name[] = "sw";
            print_store(name, instruction);
            break;
        }
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}

void write_branch(Instruction instruction) {
    unsigned funct3 = instruction.sbtype.funct3;
    switch(funct3) { 
        case 000: {
            /* beq */
            char name[] = "beq";
            print_branch(name, instruction);
            break;
        }
        case 001: {
            /* bne */
            char name[] = "bne";
            print_branch(name, instruction);
            break;
        }
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}

/* For the writes, probably a good idea to take a look at utils.h */

void write_lui(Instruction instruction) {
	printf("lui\tx%d, %d\n", instruction.utype.rd, instruction.utype.imm);
}

void write_jal(Instruction instruction) {
	int offset = get_jump_offset(instruction);
	printf("jal\tx%d, %d\n", instruction.ujtype.rd, offset);

}

void write_ecall(Instruction instruction) {
	printf("ecall\n");
}

void print_rtype(char *name, Instruction instruction) {
    unsigned int rd = instruction.rtype.rd;
    unsigned int rs1 = instruction.rtype.rs1;	
    unsigned int rs2 = instruction.rtype.rs2;
    printf(RTYPE_FORMAT, name, rd, rs1, rs2);
    
}

void print_itype_except_load(char *name, Instruction instruction, int imm) {
    unsigned int rd = instruction.itype.rd;
    unsigned int rs1 = instruction.itype.rs1;
    printf(ITYPE_FORMAT, name, rd, rs1, imm);
}

void print_load(char *name, Instruction instruction) {
	unsigned int rd = instruction.itype.rd;
	unsigned int rs1 = instruction.itype.rs1;
	int imm = bitSigner(instruction.itype.imm, 32);
    if (imm > 2047) {
        imm = imm - 4096;
    }
	printf(MEM_FORMAT, name, rd, imm, rs1);
}

void print_store(char *name, Instruction instruction) {
	unsigned int rs1 = instruction.stype.rs1;
	unsigned int rs2 = instruction.stype.rs2;
	int offset = get_store_offset(instruction);
	printf(MEM_FORMAT, name, rs2, offset, rs1);
}

void print_branch(char *name, Instruction instruction) {
	unsigned int rs1 = instruction.sbtype.rs1;
	unsigned int rs2 = instruction.sbtype.rs2;
	int offset = get_branch_offset(instruction);
	printf(BRANCH_FORMAT, name, rs1, rs2, offset);
}
