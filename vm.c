// Chris Chaffin
// PL/0 VM

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

typedef struct instruction
{
    int op;
    int l;
    int m;
} instruction;

instruction code[MAX_CODE_LENGTH];
int stack[MAX_STACK_HEIGHT] = {0};

instruction ir;
int pc = 0, bp = 1, sp = 0, halt = 0, printFlag = 0;

// fetch cycle of the VM
void fetch(void)
{
    ir = code[pc];
    pc++;
}

int base(int l, int base) // l stand for L in the instruction format
{
  int b1; //find base L levels down
  b1 = base;
  while (l > 0)
  {
    b1 = stack[b1 + 1];
    l--;
  }
  return b1;
}

// execute cycle of the VM
void execute(void)
{
    switch(ir.op)   // switch statement based on op code
    {
        case 1:     // LIT
            sp++;
            stack[sp] = ir.m;
            break;
        case 2:     // OPR
            switch(ir.m)
            {
                case 0:     // RET
                    sp = bp - 1;
                    pc = stack[sp + 4];
                    bp = stack[sp + 3];
                    break;
                case 1:     // NEG
                    stack[sp] *= -1;
                    break;
                case 2:     // ADD
                    sp--;
                    stack[sp] += stack[sp + 1];
                    break;
                case 3:     // SUB
                    sp--;
                    stack[sp] -= stack[sp + 1];
                    break;
                case 4:     // MUL
                    sp--;
                    stack[sp] *= stack[sp + 1];
                    break;
                case 5:     // DIV
                    sp--;
                    stack[sp] /= stack[sp + 1];
                    break;
                case 6:     // ODD
                    stack[sp] = stack[sp] % 2;
                    break;
                case 7:     // MOD
                    sp--;
                    stack[sp] %= stack[sp + 1];
                    break;
                case 8:     // EQL
                    sp--;
                    stack[sp] = (stack[sp] == stack[sp + 1]);
                    break;
                case 9:     // NEQ
                    sp--;
                    stack[sp] = (stack[sp] != stack[sp + 1]);
                    break;
                case 10:    // LSS
                    sp--;
                    stack[sp] = (stack[sp] < stack[sp + 1]);
                    break;
                case 11:    // LEQ
                    sp--;
                    stack[sp] = (stack[sp] <= stack[sp + 1]);
                    break;
                case 12:    // GTR
                    sp--;
                    stack[sp] = (stack[sp] > stack[sp + 1]);
                    break;
                case 13:    // GEQ
                    sp--;
                    stack[sp] = (stack[sp] >= stack[sp + 1]);
                    break;
            }
            break;
        case 3:     // LOD
            sp++;
            stack[sp] = stack[base(ir.l, bp) + ir.m];
            break;
        case 4:     // STO
            stack[base(ir.l, bp) + ir.m] = stack[sp];
            sp--;
            break;
        case 5:     // CAL
            stack[sp + 1] = 0;
            stack[sp + 2] = base(ir.l, bp);
            stack[sp + 3] = bp;
            stack[sp + 4] = pc;
            bp = sp + 1;
            pc = ir.m;
            break;
        case 6:     // INC
            sp += ir.m;
            break;
        case 7:     // JMP
            pc = ir.m;
            break;
        case 8:     // JPC
            if(stack[sp] == 0)
                pc = ir.m;
            sp--;
            break;
        case 9:     // SIO
            if(ir.m == 0)
            {
                printf("\nWRITE: ");
                printf("%d\n", stack[sp]);
                sp--;
            }
            else if(ir.m == 1)
            {
                sp++;
                scanf("%d", &stack[sp]);
            }       
            else
            {
                halt = 1;
                // exit(EXIT_SUCCESS);
            }
            break;
    }
}

// Function takes in the op code and converts it to a string for output
char *opString(int op)
{
    switch(op)
    {
        case 1:
            return "lit";
        case 2:
            return "opr";
        case 3:
            return "lod";
        case 4:
            return "sto";
        case 5:
            return "cal";
        case 6:
            return "inc";
        case 7:
            return "jmp";
        case 8:
            return "jpc";
        case 9:
            return "sio";
        default:
            return NULL;
    }
}

int main(int argc, char **argv)
{
    int i;
    for (i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0)
            printFlag = 1;
    }

    FILE *input = fopen("mcode.txt", "r");
    FILE *output = fopen("stacktrace.txt", "w");
    int bpArray[3] = {0}, bpCount = 0;

    i = 0;
    bpArray[0] = 1;

    // Initializes the first 3 elements in stack to 0
    stack[1] = 0;
    stack[2] = 0;
    stack[3] = 0;

    fprintf(output, "Line\t");
    fprintf(output, "OP\t");
    fprintf(output, "L\t");
    fprintf(output, "M\n");
    if (printFlag == 1)
    {
        printf("Line\t");
        printf("OP\t");
        printf("L\t");
        printf("M\n");
    }

    // Reads the input file and stores in struct code
    while(!feof(input))
    {
        fscanf(input, "%d", &code[i].op);
        fscanf(input, "%d", &code[i].l);
        fscanf(input, "%d", &code[i].m);

        if(opString(code[i].op) == NULL)
            break;

        if (printFlag == 1)
        {
            printf("%d\t", i);
            printf("%s\t", opString(code[i].op));
            printf("%d\t", code[i].l);
            printf("%d\n", code[i].m);
        }

        fprintf(output, "%d\t", i);
        fprintf(output, "%s\t", opString(code[i].op));
        fprintf(output, "%d\t", code[i].l);
        fprintf(output, "%d\n", code[i++].m);
    }

    if (printFlag == 1)
    {
        printf("\n\n");
        printf("\t\t\t\tpc\t");
        printf("bp\t");
        printf("sp\t");
        printf("stack\n");

        printf("Initial values");
        printf("\t\t\t%d\t", pc);
        printf("%d\t", bp);
        printf("%d\n", sp);
    }

    fprintf(output, "\n\n");
    fprintf(output, "\t\t\t\tpc\t");
    fprintf(output, "bp\t");
    fprintf(output, "sp\t");
    fprintf(output, "stack\n");

    fprintf(output, "Initial values");
    fprintf(output, "\t\t\t%d\t", pc);
    fprintf(output, "%d\t", bp);
    fprintf(output, "%d\n", sp);

    // Loop that calls the fetch and execute cycle
    while(bp != 0 && halt != 1)
    {
        if (printFlag == 1)
            printf("%d\t", pc);

        fprintf(output, "%d\t", pc);

        fetch();
        execute();

        // Array that stores previous values of bp
        if(bp != bpArray[bpCount])
        {
            if(bpCount != 2)
            {
                bpArray[bpCount+1] = bp;
                bpCount++;
            }
        }

        if (printFlag == 1)
        {
            printf("%s\t", opString(ir.op));
            printf("%d\t", ir.l);
            printf("%d\t", ir.m);
            printf("%d\t", pc);
            printf("%d\t", bp);
            printf("%d\t", sp);
        }

        fprintf(output, "%s\t", opString(ir.op));
        fprintf(output, "%d\t", ir.l);
        fprintf(output, "%d\t", ir.m);
        fprintf(output, "%d\t", pc);
        fprintf(output, "%d\t", bp);
        fprintf(output, "%d\t", sp);

        // For-loop to decide when to output a | to separate activation records
        for(i=1; i<=sp; i++)
        {
            if ((bp > 1) && (i == bpArray[0] || i == bpArray[1] || i == bpArray[2]) && (i != 1))
            {
                fprintf(output, "| ");

                if (printFlag == 1)
                    printf("| ");
            }
                
            if (printFlag == 1)
                printf("%d ", stack[i]);

            fprintf(output, "%d ", stack[i]);
        }

        fprintf(output, "\n");


        if (printFlag == 1)
            printf("\n");
    }

    // Closes input and output file
    fclose(input);
    fclose(output);
}
