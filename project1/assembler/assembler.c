/* Assembler code fragment for LC-2K */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000
#define MAXLABEL 201

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int register_check(char *string);

struct symbolic_label {
	char label[MAXLINELENGTH];
	int address;
};

int main(int argc, char *argv[]) 
{
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr;
	char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH], 
			 arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

	struct symbolic_label labels[MAXLABEL];
	int count = 0, total_count = 0;
	int op = -1, answer = 0;
	int reg_a, reg_b, reg_d;

	if (argc != 3) {
		printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
				argv[0]);
		exit(1);
	}

	inFileString = argv[1];
	outFileString = argv[2];

	inFilePtr = fopen(inFileString, "r");
	if (inFilePtr == NULL) {
		printf("error in opening %s\n", inFileString);
		exit(1);
	}
	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileString);
		exit(1);
	}

	/* here is an example for how to use readAndParse to read a line from
		 inFilePtr */
	while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
		if (strlen(label) > 6) {
			printf("error: label too long\n");
			exit(1);
		}
		if (isNumber(label)) {
			printf("error: label starts with number\n");
			exit(1);
		}
		if (!strlen(label)) {
			count++;
			continue;
		}
		for (int i = 0; i < total_count; i++) {
			if (!strcmp(label, labels[i].label)) {
				printf("error: label duplicated\n");
				exit(1);
			}
		}
		strcpy(labels[total_count].label, label);
		labels[total_count++].address = count++;
	}
	/* Phase-1 label calculation */

	/* this is how to rewind the file ptr so that you start reading from the
		 beginning of the file */
	rewind(inFilePtr);

	/* Phase-2 generate machine codes to outfile */

	/* after doing a readAndParse, you may want to do the following to test the
		 opcode */
	count = 0;
	while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
		answer = 0;
		if (!strcmp(opcode, "add"))
			op = 0;
		else if (!strcmp(opcode, "nor"))
			op = 1;
		else if (!strcmp(opcode, "lw"))
			op = 2;
		else if (!strcmp(opcode, "sw"))
			op = 3;
		else if (!strcmp(opcode, "beq"))
			op = 4;
		else if (!strcmp(opcode, "jalr"))
			op = 5;
		else if (!strcmp(opcode, "halt"))
			op = 6;
		else if (!strcmp(opcode, "noop"))
			op = 7;
		else if (!strcmp(opcode, ".fill"))
			op = 8;
		else {
			printf("error: wrong opcode\n");
			exit(1);
		}

		if (op == 0 || op == 1) {
			if (!register_check(arg0) || !register_check(arg1) || !register_check(arg2)) {
				printf("error: wrong register\n");
				exit(1);
			}
			reg_a = atoi(arg0);
			reg_b = atoi(arg1);
			reg_d = atoi(arg2);
			answer = op << 22 | reg_a << 19 | reg_b << 16 | reg_d;
		}
		else if (op == 2 || op == 3 || op == 4) {
			int off_addr = -1;
			if (!register_check(arg0) || !register_check(arg1)) {
				printf("error: wrong register\n");
				exit(1);
			}
			reg_a = atoi(arg0);
			reg_b = atoi(arg1);
			if (isNumber(arg2)) {
				if (atoi(arg2) < -32768 || atoi(arg2) > 32767) {
					printf("error: wrong offset range\n");
					exit(1);
				}
				off_addr = atoi(arg2);
			}
			else {
				for (int i = 0; i < total_count; i++)
					if (!strcmp(arg2, labels[i].label))
						off_addr = labels[i].address;
				if (off_addr == -1) {
					printf("error: undefined label\n");
					exit(1);
				}
				if (op == 4)
					off_addr -= count + 1;
			}
			off_addr &= 65535;
			answer = op << 22 | reg_a << 19 | reg_b << 16 | off_addr;
		}
		else if (op == 5) {
			if (!register_check(arg0) || !register_check(arg1)) {
				printf("error: wrong register\n");
				exit(1);
			}
			reg_a = atoi(arg0);
			reg_b = atoi(arg1);
			answer = op << 22 | reg_a << 19 | reg_b << 16;
		}
		else if (op == 6 || op == 7) {
			answer = op << 22;
		}
		else {
			if (isNumber(arg0)) {
				reg_a = atoi(arg0);
				answer = reg_a;
			}
			else {
				int off_addr = -1;
				for (int i = 0; i < total_count; i++)
					if (!strcmp(arg0, labels[i].label))
						off_addr = labels[i].address;
				if (off_addr == -1) {
					printf("error: undefined label\n");
					exit(1);
				}
				answer = off_addr;
			}
		}
		
		fprintf(outFilePtr, "%d\n", answer);
		count++;
	}

	if (inFilePtr) {
		fclose(inFilePtr);
	}
	if (outFilePtr) {
		fclose(outFilePtr);
	}
	exit(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
		char *arg1, char *arg2)
{
	char line[MAXLINELENGTH];
	char *ptr = line;

	/* delete prior values */
	label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

	/* read the line from the assembly-language file */
	if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
		/* reached end of file */
		return(0);
	}

	/* check for line too long (by looking for a \n) */
	if (strchr(line, '\n') == NULL) {
		/* line too long */
		printf("error: line too long\n");
		exit(1);
	}

	/* is there a label? */
	ptr = line;
	if (sscanf(ptr, "%[^\t\n\r ]", label)) {
		/* successfully read label; advance pointer over the label */
		ptr += strlen(label);
	}

	/*
	 * Parse the rest of the line.  Would be nice to have real regular
	 * expressions, but scanf will suffice.
	 */
	sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%"
			"[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]", opcode, arg0, arg1, arg2);
	return(1);
}

int isNumber(char *string)
{
	/* return 1 if string is a number */
	int i;
	return( (sscanf(string, "%d", &i)) == 1);
}

int register_check(char *string)
{
	if (!isNumber(string))
		return 0;
	if (atoi(string) < 0 || atoi(string) > 7)
		return 0;
	return 1;
}