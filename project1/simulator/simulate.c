/* LC-2K Instruction-level simulator */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000 
typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
int convertNum(int num);

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;
    int op, reg_a, reg_b, reg_d, offset, total = 0;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
            state.numMemory++) {

        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

	state.pc = 0;
    for (int i = 0; i < NUMREGS; i++)
        state.reg[i] = 0;
    
    for (; state.pc < state.numMemory; state.pc++) {
        printState(&state);
        op = state.mem[state.pc] >> 22 & 7;
        reg_a = state.mem[state.pc] >> 19 & 7;
        reg_b = state.mem[state.pc] >> 16 & 7;
        total++;
        if (op == 0) {
            reg_d = state.mem[state.pc] & 7;
            state.reg[reg_d] = state.reg[reg_a] + state.reg[reg_b];
        }
        else if (op == 1) {
            reg_d = state.mem[state.pc] & 7;
            state.reg[reg_d] = ~(state.reg[reg_a] | state.reg[reg_b]);
        }
        else if (op == 2) {
            offset = convertNum(state.mem[state.pc] & 65535);
            state.reg[reg_b] = state.mem[offset + state.reg[reg_a]];
        }
        else if (op == 3) {
            offset = convertNum(state.mem[state.pc] & 65535);
            state.mem[offset + state.reg[reg_a]] = state.reg[reg_b];
        }
        else if (op == 4) {
            if (state.reg[reg_a] == state.reg[reg_b]) {
                offset = convertNum(state.mem[state.pc] & 65535);
                if (state.pc+offset < 0) {
                    printf("error: negative number branch\n");
					exit(1);
                }
                state.pc = state.pc + offset;
            }
        }
        else if (op == 5) {
             state.reg[reg_b] = state.pc + 1;
             state.pc = state.reg[reg_a] - 1;
        }
        else if (op == 6) {
            printf("machine halted\n");
            state.pc++;
            break;
        }
    }
    printf("total of %d instructions executed\n", total);
    printf("final state of machine:\n");
    printState(&state);
    return(0);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i = 0; i < statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i = 0; i < NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}

int convertNum(int num)
{
	/* convert a 16-bit number into a 32-bit Linux integer */
	if (num & (1 << 15)) {
		num -= (1 << 16);
	}
	return (num);
}
